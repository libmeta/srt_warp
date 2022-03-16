# 添加第三方依赖包
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(openssl)
set(DEP_NAME  ${PROJECT_NAME})

set(${DEP_NAME}_GIT_TAG  1.1.1m-20220130)  # 指定版本
set(${DEP_NAME}_GIT_URL  https://github.com/janbar/openssl-cmake.git)  # 指定git仓库地址

FetchContent_Declare(
    ${DEP_NAME}
    GIT_REPOSITORY    ${${DEP_NAME}_GIT_URL}
    GIT_TAG           ${${DEP_NAME}_GIT_TAG}
    )

FetchContent_MakeAvailable(${DEP_NAME})
include_directories(${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/include)

#add_subdirectory("${CMAKE_BINARY_DIR}/_deps//openssl-cmake")
set(OPENSSL_ROOT_DIR ${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src)

set(OPENSSL_CRYPTO_LIBRARY  ${CMAKE_BINARY_DIR})
set(OPENSSL_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/include")
message(STATUS "Build OpenSSL: ${openssl_BINARY_DIR}")
