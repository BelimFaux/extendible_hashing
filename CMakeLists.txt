cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(extendible_hashing VERSION 1.1)

file(GLOB_RECURSE SRC_FILES src/*)

add_executable(${PROJECT_NAME} ${SRC_FILES})
add_compile_definitions(PROG_NAME="${PROJECT_NAME}" PROG_VERSION="${CMAKE_PROJECT_VERSION}")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
