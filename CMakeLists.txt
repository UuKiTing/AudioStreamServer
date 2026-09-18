cmake_minimum_required(VERSION 3.14)
project(AudioStreamServer VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译 纯 C 底层内核静态库
file(GLOB_RECURSE C_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/c_core/src/*.c)
add_library(c_core STATIC ${C_SOURCES})

target_include_directories(c_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/c_core/include)

# C++ 网络层与 HTTP 服务可执行程序
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${CPP_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    RESOURCE_GROUPS="${CMAKE_CURRENT_SOURCE_DIR}/resource"
)


target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/c_core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/net
    ${CMAKE_CURRENT_SOURCE_DIR}/src/http
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pool
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db
)

find_package(nlohmann_json REQUIRED)


target_link_libraries(${PROJECT_NAME} PRIVATE 
    pthread 
    dl
    c_core
    mysqlclient
    nlohmann_json::nlohmann_json
)