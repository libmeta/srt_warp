cmake_minimum_required(VERSION 3.14)

project(test_bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

foreach(BIN_NAME srt_send_bin)
    add_executable(${BIN_NAME} ${BIN_NAME}.cpp)
    target_link_libraries(${BIN_NAME} PRIVATE srt_wrap)
endforeach(BIN_NAME)

