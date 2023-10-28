#!/bin/bash

mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH=/home/konstantin/.local/grpc -DLLAMA_CUBLAS=ON ..
cmake --build . --config Release
cd ..
