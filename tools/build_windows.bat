mkdir ..\output\windows\release
cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -DLIB_TYPE="SHARED" -B ../output/windows/release -S ../tensorflow/lite
cd ..\output\windows\release
cmake --build . --config Release
cd ..\..\..\tools