cmake_minimum_required(VERSION 3.7)

project(
  eh_playground
  VERSION 1.1
  DESCRIPTION "Playground for an Extendible Hashing datastructure."
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES src/*)

add_executable(${PROJECT_NAME} ${SRC_FILES})
add_compile_definitions(PROG_NAME="${PROJECT_NAME}" PROG_VERSION="${PROJECT_VERSION}" PROG_DESC="${PROJECT_DESCRIPTION}")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

## TESTING
include(CTest)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
