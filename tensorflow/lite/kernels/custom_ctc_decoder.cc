/*!
* \file ctc_decoder.cc
* \brief tensorflow lite custom operator ctc_decoder implemention
*
* tensorflow lite custom operator ctc_decoder implemention
*
* \author xuxiaoyu email:xuxiaoyu3@wps.cn
*/


#include "Eigen/Core"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/c_api_internal.h"
#include "tensorflow/lite/kernels/internal/tensor.h"
#include "tensorflow/lite/kernels/internal/tensor_utils.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/op_macros.h"
#include "tensorflow/lite/kernels/padding.h"
#include "tensorflow/lite/kernels/eigen_support.h"

using namespace tflite;
using namespace Eigen;

namespace tflite {
namespace ops {
namespace custom {


typedef Eigen::Map<const Eigen::ArrayXi> SequenceLength;
typedef std::vector<std::vector<int>> Output;

typedef Matrix<float,Dynamic,Dynamic,RowMajor> MatrixXfc;
typedef Eigen::Map<const MatrixXfc> Input;
typedef Eigen::Map<MatrixXfc> ScoreOutput;


class CTCDecoder {
public:

    CTCDecoder(int num_classes, int batch_size, bool merge_repeated)
        : num_classes_(num_classes),
          blank_index_(num_classes - 1),
          batch_size_(batch_size),
          merge_repeated_(merge_repeated) {}

    virtual ~CTCDecoder() {}

    // Dimensionality of the input/output is expected to be:
    //  - seq_len[b] - b = 0 to batch_size_
    //  - input[t].rows(b) - t = 0 to timesteps; b = 0 t batch_size_
    //  - output.size() specifies the number of beams to be returned.
    //  - scores(b, i) - b = 0 to batch_size; i = 0 to output.size()
    virtual bool Decode(const SequenceLength& seq_len,
                        const std::vector<Input>& input,
                        std::vector<Output>* output, ScoreOutput* scores) = 0;

    int batch_size() { return batch_size_; }
    int num_classes() { return num_classes_; }

protected:
    int num_classes_;
    int blank_index_;
    int batch_size_;
    bool merge_repeated_;
};

// CTCGreedyDecoder is an implementation of the simple best path decoding
// algorithm, selecting at each timestep the most likely class at each timestep.
class CTCGreedyDecoder : public CTCDecoder {
public:
    CTCGreedyDecoder(int num_classes, int batch_size, bool merge_repeated)
        : CTCDecoder(num_classes, batch_size, merge_repeated) {}

    bool Decode(const SequenceLength& seq_len,
                const std::vector<Input>& input,
                std::vector<Output>* output,
                ScoreOutput* scores) override {

        if (output->empty() || (*output)[0].size() < batch_size_) {
            //return false;
        }
        if (scores->rows() < batch_size_ || scores->cols() == 0) {
            return false;
        }

        std::vector<std::vector<std::vector<int> > > sequences(batch_size_);

        // For each batch entry, identify the transitions
        for (int b = 0; b < batch_size_; ++b) {
            int seq_len_b = seq_len[b];
            // Only writing to beam 0
            (*output)[b].resize(1);
            std::vector<int>& output_b = (*output)[b][0];

            int prev_class_ix = -1;
            (*scores)(b, 0) = 0;
            for (int t = 0; t < seq_len_b; ++t) {
                auto row = input[t].row(b);
                int max_class_ix;
                //LOGD("ctc_max:%f",-row.maxCoeff(&max_class_ix));
                (*scores)(b, 0) += -row.maxCoeff(&max_class_ix);
                if (max_class_ix != blank_index_ &&
                        !(merge_repeated_ && max_class_ix == prev_class_ix)) {
                    output_b.push_back(max_class_ix);
                }

                prev_class_ix = max_class_ix;
            }
        }

        return true;
    }
};



constexpr int kInputTensor1 = 0;
constexpr int kInputTensor2 = 1;
constexpr int kOutputTensor1 = 0;
constexpr int kOutputTensor2 = 1;
constexpr int kOutputTensor3 = 2;
constexpr int kOutputTensor4 = 3;
struct OpData
{
    std::vector<Output> ctc_decoder_output;
};


TfLiteStatus ResizeOutputTensors(TfLiteContext* context, TfLiteNode* node,int idx, int total_decoded_outputs) {

    ///logits: [max_time, batch_size, num_classes]
    ///seq_len: [batch_size]
    const TfLiteTensor* logits = GetInput(context, node,kInputTensor1);
    //const TfLiteTensor* seq_len = GetInput(context,node,kInputTensor2);
    //int max_time = logits->dims->data[0];
    int batch_size = logits->dims->data[1];
    //int num_classes = logits->dims->data[2];

    /// decoded_ix: (total_decoded_outputs, 2) The rows store: [batch, time]
    /// decoded_val: (total_decoded_outputs) stores the decoded classes
    /// decoded_shape: (2) [batch_size, total_decoded_outputs]
    /// log_prob: (batch_size x 1)

    TfLiteTensor* decoded_ix = GetOutput(context, node,kOutputTensor1);
    TfLiteIntArray* decoded_ix_dims = TfLiteIntArrayCreate(2);
    decoded_ix_dims->data[0] = total_decoded_outputs;
    decoded_ix_dims->data[1] = 2;
    TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, decoded_ix, decoded_ix_dims));

    TfLiteTensor* decoded_val = GetOutput(context, node,kOutputTensor2);
    TfLiteIntArray* decoded_val_dims = TfLiteIntArrayCreate(1);
    decoded_val_dims->data[0] = total_decoded_outputs;
    TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, decoded_val, decoded_val_dims));

    TfLiteTensor* decoded_shape = GetOutput(context, node,kOutputTensor3);
    TfLiteIntArray* decoded_shape_dims = TfLiteIntArrayCreate(1);
    decoded_shape_dims->data[0] = 2;
    TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, decoded_shape, decoded_shape_dims));

    TfLiteTensor* log_prob = GetOutput(context, node,kOutputTensor4);
    TfLiteIntArray* log_prob_dims = TfLiteIntArrayCreate(2);
    log_prob_dims->data[0] = batch_size;
    log_prob_dims->data[1] = 1;
    TF_LITE_ENSURE_STATUS(context->ResizeTensor(context, log_prob, log_prob_dims));

    return kTfLiteOk;
}



/// sequences[b][p][ix] stores decoded value "ix" of path "p" for batch "b".
int StoreAllDecodedSequences(TfLiteContext *context,TfLiteNode* node,int top_paths,
                             const std::vector<Output>& sequences, ScoreOutput& score) {

    TfLiteTensor* decoded_ix = GetOutput(context, node,kOutputTensor1);
    TfLiteTensor* decoded_val = GetOutput(context, node,kOutputTensor2);
    TfLiteTensor* decoded_shape = GetOutput(context, node,kOutputTensor3);
    TfLiteTensor* log_prob = GetOutput(context, node,kOutputTensor4);

    /// Calculate the total number of entries for each path
    const int64_t batch_size = sequences.size();
    std::vector<int64_t> num_entries;
    num_entries.resize(static_cast<size_t>(top_paths));
    /// Calculate num_entries per path
    for (const auto& batch_s : sequences) {
        TF_LITE_ENSURE_EQ(context,batch_s.size(), top_paths);
        for (int p = 0; p < top_paths; ++p) {
            num_entries[p] += batch_s[p].size();
        }
    }

    for (int p = 0; p < top_paths; ++p) {

        const int64_t p_num = num_entries[p];

        ResizeOutputTensors(context,node,p,static_cast<int>(p_num));
        float* prob_t = GetTensorData<float>(log_prob);
        for(int i = 0; i < batch_size;++i)
        {
            prob_t[p*top_paths+i] = score(i,p);
        }

        int64_t* indices_t = GetTensorData<int64_t>(decoded_ix);
        int64_t* values_t = GetTensorData<int64_t>(decoded_val);
        int64_t* shape_t = GetTensorData<int64_t>(decoded_shape);

        int64_t max_decoded = 0;
        int64_t offset = 0;

        for (int64_t b = 0; b < batch_size; ++b) {

            auto& p_batch = sequences[b][p];
            int64_t num_decoded = p_batch.size();
            max_decoded = std::max(max_decoded, num_decoded);
            std::copy_n(p_batch.begin(), num_decoded, &values_t[offset]);
            for (int64_t t = 0; t < num_decoded; ++t, ++offset) {
                indices_t[offset*2] = b;
                indices_t[offset*2+1] = t;
            }
        }

        shape_t[0] = batch_size;
        shape_t[1] = max_decoded;
    }
    return 0;
}



void *Init(TfLiteContext *context, const char *buffer, size_t length)
{
    OpData* data = new OpData;
    data->ctc_decoder_output.resize(1); //batch_size
    data->ctc_decoder_output[0].resize(1);
    data->ctc_decoder_output[0][0].resize(40);
    //eigen_support::IncrementUsageCounter(context);
    return data;
}

void Free(TfLiteContext *context, void *buffer)
{
    //eigen_support::DecrementUsageCounter(context);
    delete reinterpret_cast<OpData*>(buffer);
}

TfLiteStatus UseDynamicOutputTensors(TfLiteContext* context, TfLiteNode* node) {
    for (int i = 0; i < NumOutputs(node); ++i) {
        SetTensorToDynamic(GetOutput(context, node, i));
    }
    return kTfLiteOk;
}

TfLiteStatus Prepare(TfLiteContext *context, TfLiteNode *node)
{
    /// Check number of inputs/outputs
    //LOGD("CtcGreedyDecoder Prepare input:%d output:%d",NumInputs(node),NumOutputs(node));
    TF_LITE_ENSURE_EQ(context, NumInputs(node), 2);
    TF_LITE_ENSURE_EQ(context, NumOutputs(node), 4);
    const TfLiteTensor* logits = GetInput(context, node,kInputTensor1);
    const TfLiteTensor* seq_len = GetInput(context,node,kInputTensor2);
    TfLiteTensor* decoded_ix = GetOutput(context, node,kOutputTensor1);
    TfLiteTensor* decoded_val = GetOutput(context, node,kOutputTensor2);
    TfLiteTensor* decoded_shape = GetOutput(context, node,kOutputTensor3);
    TfLiteTensor* log_prob = GetOutput(context, node,kOutputTensor4);

    /// Check dimensionality of inputs
    TF_LITE_ENSURE_EQ(context, logits->dims->size, 3);
    TF_LITE_ENSURE_EQ(context, seq_len->dims->size, 1);

    /// Check types.
    TF_LITE_ENSURE(context,
                   logits->type == kTfLiteFloat32/* || input_type == kTfLiteUInt8*/);
    TF_LITE_ENSURE(context,seq_len->type == kTfLiteInt32);

    decoded_ix->type = kTfLiteInt64;
    decoded_val->type = kTfLiteInt64;
    decoded_shape->type = kTfLiteInt64;
    log_prob->type = kTfLiteFloat32;

    return UseDynamicOutputTensors(context, node);
}

TfLiteStatus Invoke(TfLiteContext *context, TfLiteNode *node)
{
    //LOGD("CtcGreedyDecoder Invoke");
    const TfLiteTensor* logits = GetInput(context, node,kInputTensor1);
    const TfLiteTensor* seq_len = GetInput(context,node,kInputTensor2);
    TfLiteTensor* decoded_ix = GetOutput(context, node,kOutputTensor1);
    TfLiteTensor* decoded_val = GetOutput(context, node,kOutputTensor2);
    TfLiteTensor* decoded_shape = GetOutput(context, node,kOutputTensor3);
    TfLiteTensor* log_prob = GetOutput(context, node,kOutputTensor4);

    //OpData* data = reinterpret_cast<OpData*>(node->user_data);

    int max_time = logits->dims->data[0];
    int batch_size = logits->dims->data[1];
    int num_classes = logits->dims->data[2];
    CTCGreedyDecoder ctc = CTCGreedyDecoder(num_classes,batch_size,true);
    std::vector<Input> ctc_input;
    for(int i = 0;i < max_time;++i)
    {
        ctc_input.emplace_back(logits->data.f + i * batch_size * num_classes,batch_size,num_classes);
    }

    std::vector<Output> ctc_output(batch_size);
    std::vector<float> prob(batch_size*1);
    ScoreOutput ctc_score = ScoreOutput(prob.data(),batch_size,1);
    SequenceLength s = SequenceLength(seq_len->data.i32,1);

    ctc.Decode(SequenceLength(seq_len->data.i32,1),ctc_input,&ctc_output,&ctc_score);

    //LOGD("ctc_score:%f",ctc_score(0,0));

    StoreAllDecodedSequences(context,node,1,ctc_output,ctc_score);

    return kTfLiteOk;

}

TfLiteRegistration* Register_CTC_GREEDY_DECODER()
{
    static TfLiteRegistration r = {nullptr,
                                   nullptr,
                                   Prepare,
                                   Invoke};
    return &r;
}

} // namespace custom
} // namespace ops
} // namespace tflite
