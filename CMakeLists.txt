cmake_minimum_required(VERSION 3.14)

project(srt_wrap LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(macro_func)
add_subdirectory(3rd)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/_deps/srt-build)
include_directories(${CMAKE_BINARY_DIR}/_deps/srt-src)
include_directories(${CMAKE_BINARY_DIR}/_deps/srt-src/common)
include_directories(${CMAKE_BINARY_DIR}/_deps/srt-src/srtcore)


file(GLOB_RECURSE NATIVE_SRCS ${CMAKE_SOURCE_DIR}/src/*)

add_static_libs(srt ${CMAKE_BINARY_DIR} libsrt.a srt.lib srt.lib)
add_library(${PROJECT_NAME} SHARED
  ${NATIVE_SRCS}
)

target_link_libraries(
    ${PROJECT_NAME}
    srt
    )

target_compile_definitions(srt_wrap PRIVATE SRT_WRAP_LIBRARY)
add_subdirectory(test_bin)

