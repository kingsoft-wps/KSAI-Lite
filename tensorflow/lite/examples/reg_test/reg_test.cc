/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <time.h>
#include "tensorflow/lite/c/c_api.h"

#define TFLITE_REG_TEST_CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

template <typename T>
int compareResult(const T* golden, const T* result, const T minThreshold,
                  const T maxThreshold, const int outNumTh,
                  const int datasize) {
  int outNum = 0;
  for(int i = 0; i < datasize; i++){
    if (((golden[i] - result[i]) < minThreshold) || ((golden[i] - result[i]) > maxThreshold)) {
      outNum++;
    }
  }
  std::cout << "the different element num between golden and result is :  " << outNum << "/" << datasize << ", num threshold is : " << outNumTh << std::endl; 
  if (outNum > outNumTh) {
    return false;
  }
  return true;
}
void readStream(const std::string filename, float* buf) {
  std::ifstream in;
  in.open(filename, std::ios::in);
  if (!in.is_open()) {
    std::cout << "open File Failed." << std::endl;
    return;
  }
  std::string strOne;
  int i = 0;
  while (!in.eof()) {
    in >> buf[i];
    i++;
  }
  in.close();
}

template <typename T>
void dumpData(const std::string filename, const T* data, const int dumpnum) {
  std::ofstream out(filename.c_str());
  float result = 0;
  for (int i = 0; i < dumpnum; i++) {
    result = data[i];
    out << result << std::endl;
  }
  out.close();
}

void fillInputBufs(TfLiteInterpreter*& interpreter, const std::vector<int> dims,
                   const float* dataSrc) {
  int inputNum = TfLiteInterpreterGetInputTensorCount(interpreter);
  for (int i = 0;  i < inputNum; i++) {
    long long inputDataSize = dims[1] * dims[2] * dims[3];
    TfLiteTensor* inputTensor = TfLiteInterpreterGetInputTensor(interpreter, i);
    TfLiteType inputType = TfLiteTensorType(inputTensor);
    switch (inputType) {
      case kTfLiteFloat32: {
        TfLiteTensorCopyFromBuffer(inputTensor, dataSrc, inputDataSize * sizeof(float));
        break;
      }
      case kTfLiteInt8: {
        std::cout << "char input" << std::endl;
        memcpy((char*)TfLiteTensorData(inputTensor), dataSrc,
               inputDataSize * sizeof(unsigned char));
        break;
      }
      case kTfLiteUInt8: {
        std::cout << "unsigned char input" << std::endl;
        memcpy((unsigned char*)TfLiteTensorData(inputTensor), dataSrc,
               inputDataSize * sizeof(char));
        break;
      }
      default:
        std::cout << "cannot handle input type yet" << std::endl;
        exit(-1);
    }
  }
}

template <typename T>
T getThreshold(T* dataPtr, long long dataSize, const int percent) {
  T maxVal = -10000;
  T minVal = 10000;
  T threshold = 0;
  for (long long i = 0; i < dataSize; i++) {
    if (dataPtr[i] > maxVal) {
      maxVal = dataPtr[i];
    }
    if (dataPtr[i] < minVal) {
      minVal = dataPtr[i];
    }
  }
  threshold = (T)((maxVal - minVal) * 1.0f / percent);
  return threshold;
}

bool readOutputAndCompareWithGolden(TfLiteInterpreter* interpreter,
                                    const std::string goldenfile) {
  const TfLiteTensor *outputTensor = TfLiteInterpreterGetOutputTensor(interpreter, 0);
  int32_t outputDimsNum = TfLiteTensorNumDims(outputTensor);
  long long outputDataSize = 1;
  bool passFlg = true;
  for (int32_t i = 0;i < outputDimsNum; i++) {
    int32_t dimsI = TfLiteTensorDim(outputTensor, i);
    outputDataSize *= (dimsI <= 0 ? 1 : dimsI);
  }
  int diffNumThread = outputDataSize / 100;
  float* dataDst = new float[outputDataSize];
  readStream(goldenfile.c_str(), dataDst);
  switch (TfLiteTensorType(outputTensor)) {
    case kTfLiteFloat32:
      dumpData<float>("output.txt", (float*)TfLiteTensorData(outputTensor), outputDataSize);
      passFlg = compareResult<float>(
          dataDst, (float*)TfLiteTensorData(outputTensor),
          -getThreshold<float>(dataDst, outputDataSize, 100),
          getThreshold<float>(dataDst, outputDataSize, 100),
          diffNumThread, outputDataSize);
      break;
    case kTfLiteInt8:
      passFlg = compareResult<char>(
          (char*)(dataDst), (char*)TfLiteTensorData(outputTensor),
          -getThreshold<char>((char*)dataDst, outputDataSize, 100),
          getThreshold<char>((char*)dataDst, outputDataSize, 100),
          diffNumThread, outputDataSize);
      break;
    case kTfLiteUInt8:

      passFlg = compareResult<unsigned char>(
          (unsigned char*)(dataDst),
          (unsigned char*)TfLiteTensorData(outputTensor),
          -getThreshold<unsigned char>(
              (unsigned char*)dataDst, outputDataSize, 100),
          getThreshold<unsigned char>((unsigned char*)dataDst,
                                          outputDataSize, 100),
          diffNumThread, outputDataSize);
      break;
    default:
      std::cout << "cannot handle output type yet" << std::endl;
      exit(-1);
  }
  delete[] dataDst;
  return passFlg;
}

/*
bool TensorWriteModelBuffer(TfLiteModel* &model, size_t graph_index,
    const char* tensor_name, const void* model_buffer,
    size_t data_size) {
  auto buffers = model->impl->GetModel()->buffers();
  auto subgraphs = model->impl->GetModel()->subgraphs();
  if (graph_index >= subgraphs->size()) {
    return false;
  }
  const tflite::SubGraph* subgraph = (*subgraphs)[graph_index];
  auto tensors = subgraph->tensors();
  for (size_t i = 0; i < tensors->Length(); i++) {
    const auto tensor = tensors->Get(i);
    if (tensor->name()->str().compare(tensor_name) == 0) {
      auto buffer = (*buffers)[tensor->buffer()];
      auto array = buffer->data();
      memcpy((void*)array->data(), model_buffer, data_size);
    }
  }
  return true;
}

bool ModifyModelResizeBilinearParams(TfLiteModel* &model, int width,
                                     int height) {
  if (!model) {
    return false;
  }
  int p1[] = {height / 16, width / 16};
  if(TensorWriteModelBuffer(model, 0,
                                 "feature_fusion/ResizeBilinear/size", p1,
                                 8) != true) {
    return false;
  }

  int p2[] = {height / 8, width / 8};
  if (TensorWriteModelBuffer(model, 0,
                                 "feature_fusion/ResizeBilinear_1/size", p2, 8) != true) {
    return false;
  }
  int p3[] = {height / 4, width / 4};
  if (TensorWriteModelBuffer(model, 0, "feature_fusion/ResizeBilinear_2/size",
                             p3, 8) != true) {
    return false;
  }
  return true;
}
*/
int init(const std::string modelfile, const int numOfThreads,
         const std::vector<int> dims,
         TfLiteInterpreter*& interpreter) {
  std::cout << "the model is : " << modelfile << std::endl;
  // Load model
  std::cout << "load model stage!" << std::endl;
  TfLiteModel* model = TfLiteModelCreateFromFile(modelfile.c_str());
  TFLITE_REG_TEST_CHECK(model != nullptr);

  std::cout << "Build the interpreter stage!" << std::endl;
  // Build the interpreter
  TfLiteInterpreterOptions* options = TfLiteInterpreterOptionsCreate();
  TfLiteInterpreterOptionsSetNumThreads(options, numOfThreads);
  interpreter = TfLiteInterpreterCreate(model, options);
   TfLiteInterpreterOptionsDelete(options);
  TfLiteModelDelete(model);
  TFLITE_REG_TEST_CHECK(interpreter != nullptr);

  // if (1 == use_xnnpack) {
  //   std::cout << "with xnnpack delegate!" << std::endl;
  //   auto delegate = tflite::evaluation::CreateXNNPACKDelegate(numOfThreads);
  //   if (!delegate) {
  //     std::cout << "XNNPACK acceleration is unsupported on this platform."
  //               << std::endl;
  //   } else {
  //     if (interpreter->impl->ModifyGraphWithDelegate(std::move(delegate)) !=
  //         kTfLiteOk) {
  //       std::cout << "Failed to apply xnnpack delegate." << std::endl;
  //     } else {
  //       std::cout << "Applied xnnpack delegate." << std::endl;
  //     }
  //   }
  // } else {
  //   std::cout << "without xnnpack delegate!" << std::endl;
  // }
  //just for the variable parameter network like mobileeast
  /*int inputdims[4] = {1, dims[1], dims[2], dims[3]};
  TfLiteInterpreterResizeInputTensor(interpreter, 0, inputdims, 4);
  ModifyModelResizeBilinearParams(model, dims[1], dims[2]);*/
  TFLITE_REG_TEST_CHECK(TfLiteInterpreterAllocateTensors(interpreter) ==
                        kTfLiteOk);
  return 1;
}

template <typename T>
int randomDataGen(const int H, const int W, const int C, const int maxVal,
                  T*& dataPtr) {
  const long long dataSize = H * W * C;
  for (long long i = 0; i < dataSize; i++) {
    unsigned int times = (unsigned int)time(NULL);
    srand(times * (i + 1));
    dataPtr[i] = rand() % maxVal;
  }
  return 1;
}

int loadData(TfLiteInterpreter* &interpreter, const std::vector<int> dims,
             const std::string inputfile, const int useRandomData) {
  int inputH = dims[1];
  int inputW = dims[2];
  int inputC = dims[3];
  long long inputDataSize = inputH * inputW * inputC;
  float* dataSrc = new float[inputDataSize];
  if (useRandomData) {
    randomDataGen<float>(inputH, inputW, inputC, 100, dataSrc);
  } else {
    readStream(inputfile.c_str(), dataSrc);
  }
  fillInputBufs(interpreter, dims, dataSrc);
  delete[] dataSrc;
  return 1;
}

int inference(TfLiteInterpreter* &interpreter) {
  std::cout << std::endl << "Start run inference!" << std::endl;
  clock_t start_time, stop_time;
  double duration;

  const int warmupCount = 5;
  start_time = clock();
  for (int i = 0; i < warmupCount; i++) {
    if (TfLiteInterpreterInvoke(interpreter) != kTfLiteOk) {
      std::cout << "Failed to invoke tflite!" << std::endl;
    }
  }
  stop_time = clock();
  duration = (double)(stop_time - start_time) / CLOCKS_PER_SEC;
  std::cout << "warmup Count : " << warmupCount << std::endl;
  std::cout << "average warmup time: " << duration * 1000 / warmupCount
            << " ms " << std::endl;

  const int execCount = 30;
  start_time = clock();
  for (int i = 0; i < execCount; i++) {
    if (TfLiteInterpreterInvoke(interpreter) != kTfLiteOk) {
      std::cout << "Failed to invoke tflite!" << std::endl;
    }
  }
  stop_time = clock();
  duration = (double)(stop_time - start_time) / CLOCKS_PER_SEC;
  std::cout << std::endl << "regular invoke Count : " << execCount << std::endl;
  std::cout << "average invoke time: " << duration * 1000 / execCount << " ms "
            << std::endl;
  return 1;
}

int main(int argc, char* argv[]) {
  
  // parser args
  if (argc != 11) {
    fprintf(stderr,
            "reg_test <tflite model>  <input data file> <golden data file> "
            "<numOfThread> <inputN> <inputH> <inputW> <inputC> <useRandomData> <needCheckResult>\n");
    return 1;
  }
  const std::string modelfile = argv[1];
  const std::string inputfile = argv[2];
  const std::string goldenfile = argv[3];
  const int numOfThreads = atoi(argv[4]);
  const int inputN = atoi(argv[5]);
  const int inputH = atoi(argv[6]);
  const int inputW = atoi(argv[7]);
  const int inputC = atoi(argv[8]);
  const int useRandomData = atoi(argv[9]);
  const int needCheckResult = atoi(argv[10]);
  const std::vector<int> dims = {inputN, inputH, inputW, inputC};

  TfLiteInterpreter* interpreter = nullptr;
  init(modelfile, numOfThreads, dims, interpreter);

  // load data and Fill input buffers
  std::cout << "Fill input buffers stage!" << std::endl;
  loadData(interpreter, dims, inputfile, useRandomData);
  

  // Run inference
  std::cout << "inference stage!" << std::endl;
  inference(interpreter);
  //tflite::PrintInterpreterState(interpreter);

  // Read output buffers and compare with golden 
  if (needCheckResult) {
    std::cout << "dump output and compare stage!" << std::endl;
    bool passFlg = readOutputAndCompareWithGolden(interpreter, goldenfile);
    if (passFlg) {
      std::cout << "congratulation !!!! the new lib can publish" << std::endl;
    } else {
      std::cout << "oh no !!!! the new lib is not ready" << std::endl;
    }
  }  

  TfLiteInterpreterDelete(interpreter);
  return 0;
}
