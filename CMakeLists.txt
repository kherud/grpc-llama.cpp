cmake_minimum_required(VERSION 3.13)

project("grpc-llama.cpp" C CXX)

include(common.cmake)

# Proto file
get_filename_component(llm_proto "protos/llm.proto" ABSOLUTE)
get_filename_component(llm_proto_path "${llm_proto}" PATH)

# Generated sources
set(llm_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/llm.pb.cc")
set(llm_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/llm.pb.h")
set(llm_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/llm.grpc.pb.cc")
set(llm_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/llm.grpc.pb.h")
add_custom_command(
      OUTPUT "${llm_proto_srcs}" "${llm_proto_hdrs}" "${llm_grpc_srcs}" "${llm_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${llm_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${llm_proto}"
      DEPENDS "${llm_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_subdirectory(third_party/llama.cpp)

# llm_grpc_proto
add_library(llm_grpc_proto
  ${llm_grpc_srcs}
  ${llm_grpc_hdrs}
  ${llm_proto_srcs}
  ${llm_proto_hdrs})
target_link_libraries(llm_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

add_executable(server "llm_server.cc")
target_link_libraries(server
    llm_grpc_proto
    common
    llama
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
)
target_compile_features(server PRIVATE cxx_std_11)
