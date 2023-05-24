cmake_minimum_required(VERSION 3.0)
project(LearningCpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the compiler to Clang
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE FILEPATH "C++ compiler" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Test" CACHE STRING "" FORCE)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()


# Compiler flags for Debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Add debug symbols
    add_compile_options(-g)
endif()

# Compiler flags for Release build
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Enable optimizations
    add_compile_options(-O3)
endif()

enable_testing()

# Add the 'src' directory as a subdirectory
add_subdirectory(src)

add_subdirectory(tests)

# Include Google Test
add_subdirectory(gtest)
