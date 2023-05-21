add_executable(LearningCpp main.cpp)

set_target_properties(LearningCpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)

target_include_directories(LearningCpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
