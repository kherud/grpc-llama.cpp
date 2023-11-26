cmake_minimum_required(VERSION 3.25)

project("grpc-llama.cpp" C CXX)

set(TARGET grpc-server)
set(GRPC_TAG v1.59.3)
set(LLAMA_CPP_TAG b1566)
set(NLOHMANN_JSON_TAG v3.11.2)

include(FetchContent)
include(build-args.cmake)

find_package(Threads REQUIRED)

#################### GRPC ####################

set(ABSL_ENABLE_INSTALL ON)
FetchContent_Declare(
	grpc
	GIT_REPOSITORY https://github.com/grpc/grpc.git
	GIT_TAG        ${GRPC_TAG}
)
FetchContent_MakeAvailable(grpc)
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_REFLECTION grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP grpc++)
if(CMAKE_CROSSCOMPILING)
	find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
	set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

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
      DEPENDS "${llm_proto}"
)

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
#add_subdirectory(third_party/llama.cpp)
#add_subdirectory(third_party/llama.cpp/examples/llava)

# llm_grpc_proto
add_library(llm_grpc_proto
  ${llm_grpc_srcs}
  ${llm_grpc_hdrs}
  ${llm_proto_srcs}
  ${llm_proto_hdrs}
)
target_link_libraries(llm_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)

#################### json ####################

FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json
	GIT_TAG        ${NLOHMANN_JSON_TAG}
)
FetchContent_MakeAvailable(json)

#################### llama.cpp ####################

set(LLAMA_BUILD_EXAMPLES ON)
FetchContent_Declare(
	llama.cpp
	GIT_REPOSITORY https://github.com/ggerganov/llama.cpp
	GIT_TAG        ${LLAMA_CPP_TAG}
)
FetchContent_MakeAvailable(llama.cpp)

#################### grpc-llama.cpp ####################

add_executable(${TARGET} server.cpp)
target_link_libraries(${TARGET} PRIVATE
    llm_grpc_proto
    common
    llama
    llava
    nlohmann_json
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    ${CMAKE_THREAD_LIBS_INIT}
    ${LLAMA_EXTRA_LIBS}
)
target_compile_features(${TARGET} PRIVATE cxx_std_11)
target_compile_definitions(${TARGET} PRIVATE
    SERVER_VERBOSE=$<BOOL:${LLAMA_SERVER_VERBOSE}>
)

add_executable(grpc-client client.cpp)
target_link_libraries(grpc-client PRIVATE
    llm_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
)
