::for /l %%a in (1,1,100) do (
cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release -B ./build_release -S ./
::)