cmake_minimum_required(VERSION 3.0)
project(LearningCpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the compiler to Clang
set(CMAKE_CXX_COMPILER "clang++")

# Add the 'src' directory as a subdirectory
add_subdirectory(src)
