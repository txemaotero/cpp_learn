#include "matrix.hpp"
#include <gtest/gtest.h>

// Example test case
TEST(MatrixTest, Initialization)
{
    matrix::Matrix<int, 2, 3> m;

    EXPECT_EQ(m.n_rows(), 2);
    EXPECT_EQ(m.n_cols(), 3);
    EXPECT_EQ(m.size(), 6);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
