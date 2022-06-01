#  KSAI Lite

[English](README_en.md) | 简体中文

 [![Documentation Status](https://img.shields.io/badge/中文文档-最新-brightgreen.svg)](https://KSAI-lite.readthedocs.io/zh/develop/)  [![Release](https://img.shields.io/github/release/KSAIKSAI/KSAI-Lite.svg)](https://github.com/KSAIKSAI/KSAI-Lite/releases)  [![License](https://img.shields.io/badge/license-Apache%202-blue.svg)](LICENSE)

KSAI Lite是一个轻量级、灵活性强、高性能且易于扩展的深度学习推理框架，底层基于tensorflow lite，定位支持包括移动端、嵌入式以及服务器端在内的多硬件平台。

当前KSAI Lite已经应用在金山office内部业务中，并逐步支持金山企业的生产任务和众多外部用户。

## 快速入门

使用KSAI Lite，只需几个简单的步骤，就可以把模型部署到多种终端设备中，运行高性能的推理任务，使用流程如下所示：

**一. 准备模型**

KSAI Lite框架直接支持模型结构为tflite模型。
如果您手中的模型是由诸如Caffe、MXNet、PyTorch等框架产出的，那么您可以使用工具将模型转换为tflite格式。

**二. 模型优化**

KSAI Lite框架基于底层tensorflow lite的优化方法，拥有优秀的加速、优化策略及实现，包含量化、子图融合、Kernel优选等优化手段。优化后的模型更轻量级，耗费资源更少，并且执行速度也更快。

**三. 下载或编译**

KSAI Lite提供了多平台的官方Release预测库下载，我们优先推荐您直接下载 KSAI Lite预编译库,包括了[Linux-X64](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/linux/X64/libtensorflow-lite.so), [Linux-ARM](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/linux/ARM/libtensorflow-lite.so), [Linux-MIPS64](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/linux/MIPS64/libtensorflow-lite.so)以及[Windows-X64索引库](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/windows/tensorflow-lite.lib)和[Windows-X64动态链接库](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/windows/tensorflow-lite.dll)。
您也可以根据目标平台选择对应的源码编译方法。KSAI Lite 提供了源码编译脚本，位于 `tools/`目录下，只需要按照`docs/`目录下的准备环境说明文档environment setup.md搭建好环境然后切到`tools/`目录调用编译脚本两个步骤即可一键编译得到目标平台的KSAI Lite预测库。

**四. 预测示例**

KSAI Lite提供了C++ API，并且提供了相应API的完整使用示例:
目录为`tensorflow/lite/examples/reg_test/reg_test.cc`
您可以参考示例快速了解使用方法，并集成到您自己的项目中去，也可以参考[KSAI-Toolkits]()该项目。



## 主要特性

- **多硬件支持**
	- KSAI Lite架构已经验证和完整支持从 Mobile 到 Server 多种硬件平台，包括 intel X86、ARM、华为 Kunpeng 920、龙芯Loongson-3A R3、兆芯C4600、Phytium FT1500a等，且正在不断增加更多新硬件支持。
- **轻量级部署**：
	- KSAI Lite在设计上对图优化模块和执行引擎实现了良好的解耦拆分，移动端可以直接部署执行阶段，无任何第三方依赖。
- **高性能**
	- 极致的 ARM及X86 CPU 性能优化：针对不同微架构特点实现kernel的定制，最大发挥计算性能，在主流模型上展现出领先的速度优势。
- **多模型多算子**：
	- KSAI Lite和tensorflow训练框架的OP对齐，提供广泛的模型支持能力。
	- 目前已对视觉类模型做到了较为充分的支持，覆盖分类、检测和识别，包含了特色的OCR模型的支持，并在不断丰富中。
- **强大的图分析和优化能力**：
	- 不同于常规的移动端预测引擎基于 Python 脚本工具转化模型， Lite 架构上有完整基于 C++ 开发的 IR 及相应 Pass 集合，以支持操作融合，计算剪枝，存储优化，量化计算等多类计算图优化。

## 持续集成

| platform | Linux(ubuntu 16.04) | Windows (Win 10) | Android | IOS | Comments |
|:-:|:-:|:-:|:-:|:-:|:-:|
| intel x86 | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | i5-9500 |
| intel x86-64 | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | i5-9500 |
| 高通 ARM | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - |  - | 骁龙845 |
| 华为 ARM | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | - | kunpeng920 |
| 龙芯 MIPS64 | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | - | Loongson-3A |
| 兆芯 X86 | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | C4600 |
| Phytium ARM | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | - | FT1500a |

##  交流与反馈
* 欢迎您通过[Github Issues](https://github.com/KSAI/KSAI-Lite/issues)来提交问题、报告与建议
## 版权和许可证
KSAI-Lite由[Apache-2.0 license](LICENSE)提供
