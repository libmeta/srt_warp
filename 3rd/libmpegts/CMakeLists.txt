# 添加第三方依赖包
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(libmpegts)
set(DEP_NAME  ${PROJECT_NAME})

set(${DEP_NAME}_GIT_TAG  main)  # 指定版本
set(${DEP_NAME}_GIT_URL https://github.com/libmeta/libmpegts.git)  # 指定git仓库地址

FetchContent_Declare(
    ${DEP_NAME}
    GIT_REPOSITORY    ${${DEP_NAME}_GIT_URL}
    GIT_TAG           ${${DEP_NAME}_GIT_TAG}
    )

FetchContent_MakeAvailable(${DEP_NAME})
include_directories(${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/include)
