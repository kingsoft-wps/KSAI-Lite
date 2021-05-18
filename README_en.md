#  KSAI Lite

English | [简体中文](https://ksogit.kingsoft.net/zhai/ksai-lite/blob/develop/README.md)

 [![Documentation Status](https://img.shields.io/badge/中文文档-最新-brightgreen.svg)](https://KAI-lite.readthedocs.io/zh/develop/)  [![Release](https://img.shields.io/github/release/KAIKAI/KAI-Lite.svg)](https://github.com/KAIKAI/KAI-Lite/releases)  [![License](https://img.shields.io/badge/license-Apache%202-blue.svg)](LICENSE)

KSAI Lite is a deep learning inference framework based on tensorflow lite. it is designed with lightweight, flexible, high-performance and easy to extend. supporting various hardware platform including mobile, embedded and server end.

Currently KSAI Lite has been applied into Kingsoft WPS internal service, and will gradually support the 
tasks in kingsoft group and lots of external users.

## Quick start

For KSAI Lite, you just follow few simple steps to deploy the model on multible hardware devices and implement high-performance inference tasks. the steps are as below:

**Prepare model** 

KSAI Lite directly supports TFLite model. If your model is produced by framework like Caffe, MXNet, PyTorch etc, then you can 
use tool to convert the model into TFLite format.

**Model Optimization**
Based on TFLite optimization method, KAI Lite framework has excellent accelaration, optimization strategy and realization, including optimization methods like quantization, subgraph fusion, Kernel preference . The optimized
model is more lightweight, and cost much less resources, and faster inference speed.

**Download or Edit-Translation** 

KSAI Lite provides official Release inference library of multiple platforms, we recommend you directly download  KAI Lite pre-build lib, including [Linux-X64](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/linux/X64/libtensorflow-lite.so), [Linux-ARM](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/linux/ARM/libtensorflow-lite.so), [Linux-MIPS64](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/linux/MIPS64/libtensorflow-lite.so)以及[Windows-X64 lib](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/windows/tensorflow-lite.lib)和[Windows-X64 dll](https://sdk.ai.wpscdn.cn/KSAI/KSAI-Lite/pre-build/windows/tensorflow-lite.dll)。
You can also choose source code compilation method according to your platform. KSAI Lite provide  source code compilation scripts, path is `tools/`,you just need to prepare the environment according to the environment setup.md in `docs/` folder and then compile with the scripts in `tools/` folder.

**Inference sample**  

KAI Lite provide C++ API, and provide correspondent complete API examples: path is `tensorflow/lite/examples/reg_test/reg_test.cc`
You can quickly understand usage method according to the example, and intergrate the API into your project; you can also directly refer to the project [KSAI-Toolkits]().

## Key features

- **Multi-platforms support** 
	- KSAI Lite has been verified on various hardware platforms, from Mobile devices to Server devices, including intel X86, ARM, huawei Kunpeng 920, Loongson-3A R3, zhaoxin C4600, Phytium FT1500a, and it will support more new hardware platforms.
- **Lightweight deployment**：
  -  The decoupling design between graph optimization module and execute module makes Mobile terminal can directly deploy execution step without depending on any third-partys.
- **High-performance**：
	- Ultimate ARM and X86 CPU performance optimization; realize kernel customization for different micro architecture to maximum the performance, displayed leading speed advantage on main models.
- **Multi-model, Mutil-ops**：
  - The ops of KSAI Lite is aligned with tensorflow, which provides extensive model support ability.
  - Currently KSAI Lite supports CV models, which including classification, detection and recognition, specially support the OCR model, and constantly enriching. 
- **Strong graph analysis and optimization ability**：
	- Different from common mobile inference engine which convert model based on Python tools, KSAI Lite framework has IR and Pass group completely based on C++, to support various 
	  compute graph optimization like op fusion, Calculation pruning, storage optimization and quantization.
##  CI

| System | X86 Linux | ARM Linux | MIPS64 Linux| windows |
|:-:|:-:|:-:|:-:|:-:|
| CPU(32bit) | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)
| CPU(64bit) | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)
| Qualcomm 845 | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - |
| huaweikunpeng920 | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - |
| Loongson-3A | - | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - |
| zhaoxin C4600 | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - |
| Phytium FT1500a | - | ![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg) | - | - |

##  Communicate and feedback 
* Welcome to submit questions, reports and suggestions through[Github Issues](https://github.com/KSAI/KSAI-Lite/issues).

## License
KSAI-Lite is supported by [Apache-2.0 license](LICENSE)
