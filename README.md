# gRPC [llama.cpp](https://github.com/ggerganov/llama.cpp)

The main goal of llama.cpp is to run the LLaMA model using 4-bit integer quantization on a MacBook.
This repository provides a gRPC server for the library and proto files for client generation.

## Installation

First install [grpc](https://grpc.io/docs/languages/cpp/quickstart).
Then run:

```bash
mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/grpc -DLLAMA_BUILD_EXAMPLES=ON ..  # other build args like -DLLAMA_CUBLAS=ON
cmake --build . --config Release
cd ..
```

## Run

```bash
./grpc-server --help
./grpc-server --host 0.0.0.0 --port 50051 -m /path/to/model.gguf
```
