# Basic build informations
cmake_minimum_required(VERSION 3.13.4)
project(MyPass)

# Use C++ 14
set(CMAKE_CXX_STANDARD 14 CACHE STRING "")

# [!] Edit this with your own installation/include directory
set(LLVM_INSTALL_DIR "" CACHE PATH "/usr/lib/llvm-12")
set(LLVM_INCLUDE_DIRS "" CACHE PATH "/usr/include/llvm-12/")

# loads settings for llvm project
list(APPEND CMAKE_PREFIX_PATH "{$LLVM_INSTALL_DIR}/lib/cmake/llvm/")
find_package(LLVM REQUIRED CONFIG)

# adding include directories to project
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

# disable RTTI (LLVM has his own dynamic_cast<>)
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# Build the pass as a shared library (.so)
add_library(PassTheFisheDown SHARED Pass.cpp)

# Allow undefined symbols in shared objects
target_link_libraries(PassTheFisheDown
  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")
