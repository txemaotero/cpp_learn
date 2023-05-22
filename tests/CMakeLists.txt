# Add the test_main.cpp file as an executable
add_executable(UnitTests test_main.cpp)

# Link the Google Test library
target_link_libraries(UnitTests PRIVATE gtest)

# Include the 'include' directory for test files
target_include_directories(UnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Include the 'gtest' directory for Google Test headers
target_include_directories(UnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../gtest/googletest/include)

add_test(NAME UnitTests COMMAND UnitTests)
