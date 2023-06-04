#include "matrix.hpp"
#include <gtest/gtest.h>

// Example test case
TEST(MatrixTest, EmptyInitialization)
{
    matrix::Matrix<int, 2, 3> m;

    EXPECT_EQ(m.n_rows(), 2);
    EXPECT_EQ(m.n_cols(), 3);
    EXPECT_EQ(m.size(), 6);

}

TEST(MatrixTest, ListInitialization)
{
    matrix::Matrix<int, 3, 4> m {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9,10,11,12}
    };
    EXPECT_EQ(m.n_rows(), 3);
    EXPECT_EQ(m.n_cols(), 4);

    auto data = m.data();
    for (int i = 0; i < m.size(); ++i)
        EXPECT_EQ(data[i], i+1);
}

TEST(MatrixTest, SingleElementAccess)
{
    matrix::Matrix<int, 3, 2> m {
        {1, 2},
        {5, 6},
        {9,10}
    };
    EXPECT_EQ(m(0, 0), 1);
    EXPECT_EQ(m(0, 1), 2);
    EXPECT_EQ(m(1, 0), 5);
    EXPECT_EQ(m(1, 1), 6);
    EXPECT_EQ(m(2, 0), 9);
    EXPECT_EQ(m(2, 1), 10);
}

TEST(MatrixTest, SliceAccess)
{
    matrix::Matrix<int, 3, 4> m {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9,10,11,12}
    };

    using matrix::slice;
    auto s = m(slice(0, 2), slice(1, 3));

    // Slice of matrix
    EXPECT_EQ(s.n_rows(), 2);
    EXPECT_EQ(s.n_cols(), 2);
    EXPECT_EQ(s.size(), 4);
    EXPECT_EQ(s(0, 0), 2);
    EXPECT_EQ(s(0, 1), 3);
    EXPECT_EQ(s(1, 0), 6);
    EXPECT_EQ(s(1, 1), 7);

    // Slice of slice
    auto s2 = s(slice(0, 1), slice(0, 2));
    EXPECT_EQ(s2.n_rows(), 1);
    EXPECT_EQ(s2.n_cols(), 2);
    EXPECT_EQ(s2.size(), 2);
    EXPECT_EQ(s2(0, 0), 2);
    EXPECT_EQ(s2(0, 1), 3);

    // With stride
    auto s3 = m(slice(0, 3, 2), slice(0, 4, 2));
    EXPECT_EQ(s3.n_rows(), 2);
    EXPECT_EQ(s3.n_cols(), 2);
    EXPECT_EQ(s3.size(), 4);
    matrix::Matrix<int, 2, 2> m_com = {
        {1, 3},
        {9,11}
    };
    EXPECT_EQ(s3, m_com);
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
