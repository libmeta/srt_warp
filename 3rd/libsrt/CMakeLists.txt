# 添加第三方依赖包
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(srt)
set(DEP_NAME  ${PROJECT_NAME})

set(${DEP_NAME}_GIT_TAG  v1.4.4)  # 指定版本
set(${DEP_NAME}_GIT_URL  https://github.com/Haivision/srt.git)  # 指定git仓库地址

FetchContent_Declare(
    ${DEP_NAME}
    GIT_REPOSITORY    ${${DEP_NAME}_GIT_URL}
    GIT_TAG           ${${DEP_NAME}_GIT_TAG}
    )


if(WIN32)
#    scoop install openssl
#set(HOME_DIR $ENV{HOME})
set(HOME_DIR C:/Users/iscpp)
#SET(ENV{OPENSSL_ROOT_DIR} ${HOME_DIR}/scoop/apps/openssl/current)
#SET(ENV{OPENSSL_INCLUDE_DIR} ${HOME_DIR}/scoop/apps/openssl/current/include)
#SET(ENV{OPENSSL_CRYPTO_LIBRARY} ${HOME_DIR}/scoop/apps/openssl/current/lib)
set(OPENSSL_ROOT_DIR ${HOME_DIR}/scoop/apps/openssl/current)
set(OPENSSL_INCLUDE_DIR ${HOME_DIR}/scoop/apps/openssl/current/include)
set(OPENSSL_CRYPTO_LIBRARY ${HOME_DIR}/scoop/apps/openssl/current/lib)
endif(WIN32)

include_directories(${CMAKE_BINARY_DIR}/_deps/${DEP_NAME}-src/srtcore)

FetchContent_MakeAvailable(${DEP_NAME})

