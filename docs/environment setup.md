Linux各平台以及android编译时所需要cmake的版本为3.16及以上， 安装方法为：
```
sudo apt-get install cmake
```
Linux各平台以及android编译生成的文件包括了libtensorflow-lite.so库文件，以及benchmark_model和reg_test两个测试程序。
## linux（ubuntu16.04） android环境配置（基于cmake）：
#### NDK安装

* 1.下载对应版本的安装包，当前ksai-lite对应的ndk版本为21,链接为：
https://developer.android.google.cn/ndk/downloads
* 2.解压包,执行如下命令：

```
unzip android-ndk-r21e-linux-x86_64.zip
```
* 3.配置环境变量
> 打开环境变量文件

```
sudo gedit /etc/profile
```
> 将解压路径添加到对应的变量

```
export ANDROID_NDK=/home/xxxx/software/android-ndk-r21e
export PATH="$ANDROID_NDK:$PATH"
```
* 4.环境变量生效
```
source /etc/profile
```

#### 编译android

切换到源码目录，并进入tools目录下，执行以下命令：

```
./build.sh android
```
## Linux（ubuntu16.04）native编译

* 1.确认GCC版本为5.4及以上


* 2.切换到源码目录，并进入tools目录下，执行以下命令
```
build.sh linux native
```
## Linux（ubuntu16.04）交叉编译arm

* 1.下载toolchain

```
# for AArch64(ARM64)
curl -LO https://storage.googleapis.com/mirror.tensorflow.org/developer.arm.com/media/Files/downloads/gnu-a/8.3-2019.03/binrel/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz
mkdir -p ${HOME}/toolchains
tar xvf gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz -C ${HOME}/toolchains
```

* 2.交叉编译aarch64

切换到源码目录，并进入tools目录下，执行以下命令：

```
./build.sh linux cross_aarch64
```



## Linux（ubuntu16.04）交叉编译mips

1、安装交叉编译工具链

```
#查看可安装版本
apt-cache search mips64
#输出类似如下信息：
qemu-system-mips - QEMU full system emulation binaries (mips)
binutils-mips64-linux-gnuabi64 - GNU binary utilities, for mips64-linux-gnuabi64 target
binutils-mips64el-linux-gnuabi64 - GNU binary utilities, for mips64el-linux-gnuabi64 target
cpp-5-mips64-linux-gnuabi64 - GNU C preprocessor
cpp-5-mips64el-linux-gnuabi64 - GNU C preprocessor
cpp-mips64-linux-gnuabi64 - GNU C preprocessor (cpp) for the mips64 architecture
cpp-mips64el-linux-gnuabi64 - GNU C preprocessor (cpp) for the mips64el architecture
g++-5-mips64-linux-gnuabi64 - GNU C++ compiler
g++-5-mips64el-linux-gnuabi64 - GNU C++ compiler
g++-5-multilib-mips64-linux-gnuabi64 - GNU C++ compiler (multilib support)
g++-5-multilib-mips64el-linux-gnuabi64 - GNU C++ compiler (multilib support)
g++-mips64-linux-gnuabi64 - GNU C++ compiler for the mips64 architecture
g++-mips64el-linux-gnuabi64 - GNU C++ compiler for the mips64el architecture
g++-multilib-mips64-linux-gnuabi64 - GNU C++ compiler for the mips64 architecture
g++-multilib-mips64el-linux-gnuabi64 - GNU C++ compiler for the mips64el architecture
gcc-5-mips64-linux-gnuabi64 - GNU C compiler
gcc-5-mips64-linux-gnuabi64-base - GCC, the GNU Compiler Collection (base package)
gcc-5-mips64el-linux-gnuabi64 - GNU C compiler
gcc-5-mips64el-linux-gnuabi64-base - GCC, the GNU Compiler Collection (base package)
gcc-5-multilib-mips64-linux-gnuabi64 - GNU C compiler (multilib support)
gcc-5-multilib-mips64el-linux-gnuabi64 - GNU C compiler (multilib support)
gcc-mips64-linux-gnuabi64 - GNU C compiler for the mips64 architecture
gcc-mips64el-linux-gnuabi64 - GNU C compiler for the mips64el architecture
gcc-multilib-mips64-linux-gnuabi64 - GNU C compiler for the mips64 architecture
gcc-multilib-mips64el-linux-gnuabi64 - GNU C compiler for the mips64el architecture
......

#根据自己需要交叉编译的系统选择安装gcc/g++， 以mips64el为例
sudo apt-get install gcc-5-mips64el-linux-gnuabi64
sudo apt-get install g++-5-mips64el-linux-gnuabi64
```

2、查询安装的版本，可查到说明没问题

```
mips64el-linux-gnuabi64-gcc-5 -v
```

3、为方便起见，建立软连接
```
sudo ln -sf /usr/bin/mips64el-linux-gnuabi64-gcc-5 /usr/bin/mips64el-linux-gnuabi64-gcc
sudo ln -sf /usr/bin/mips64el-linux-gnuabi64-g++-5 /usr/bin/mips64el-linux-gnuabi64-g++
```
4、交叉编译mips64

切换到源码目录，并进入tools目录下，执行以下命令：

```
./build.sh linux cross_mips64
```
## Linux（ubuntu16.04）交叉编译loongarch

* 1.下载龙芯GCC交叉编译工具至 /usr 目录下，更改命名为 loongarch64-linux-gnu，如需下载到特定地址或者想用自己的命名，请对应修改build_linux.sh 文件中cross_loong64部分的CMAKE_C_COMPILER和CMAKE_CXX_COMPILER的路径，详情如下：

```
//build_linux.sh 46行处
-DCMAKE_C_COMPILER={你的路径} \
-DCMAKE_CXX_COMPILER={你的路径}} \
```

* 2.交叉编译loongarch

切换到源码目录，并进入tools目录下，执行以下命令：

```
./build.sh linux cross_loong64
```



## Windows 10编译Windows版本
1、安装VS2019 community版本，以及cmake3.16版本。

2、编译sln文件：

（1）如果想使用自带的benchmark工具，则修改CMakelist.txt文件，将tensorflow-lite生成库类型设为STATIC，静态库，用编译脚本编译出sln文件。

（2）如果不用自带工具，只是想生成动态库，提供对外使用，则修改CMakelist.txt文件，将tensorflow-lite生成库类型设为SHARED，动态库，用编译脚本编译出sln文件。

3、用VS2019打开sln，然后修改pthreadpool属性中的内容，属性->C/C++->命令行->其他选项，将其中的”%(AdditionalOptions) -std:c11 “删除，因为当前版本需要依赖是C++14，编译pthreadpool。

4、然后用VS2019编译出库文件tensorflow.lib。

5、如果编的是静态库，可以将benchmark或者reg_test编译出来测试一下库是否有问题。
