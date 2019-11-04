# Interface of MATLAB to Libtorch
By no means is this clean code, but it works.

## Requirements
* CMake >3.15


Tested on macOS, but some precautions were made so that it should work on Windows as well.

## Steps
* Set the Matlab root dir (MALTAB_ROOT_DIR env variable) before running or modify the line in CMakeLists.txt
```
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=[PATH TO LIBTORCH] ..
make  # on unix, on windows open visual studio and build
```