mkdir ..\output\windows\release
cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DLIB_TYPE="STATIC" -B ../output/windows/release -S ../tensorflow/lite