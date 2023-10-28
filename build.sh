#!/bin/bash

mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH=/home/konstantin/.local/grpc -DLLAMA_CUBLAS=ON -DLLAMA_BUILD_EXAMPLES=ON ..
cmake --build . --config Release
cd ..
