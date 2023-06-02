

#include "matrix_impl.hpp"
#include "matrix_ref.hpp"
#include "matrix_slice.hpp"
#include "slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T, size_t Nrows, size_t Ncols> class Matrix {
public:
    static constexpr size_t size() { return Nrows * Ncols; }
    using iterator = typename std::array<T, size()>::iterator;

    ////// Constructors
    Matrix() = default;
    Matrix(Matrix&&) = default; // move
    Matrix& operator=(Matrix&&) = default;
    Matrix(const Matrix&) = default; // copy
    Matrix& operator=(const Matrix&) = default;
    ~Matrix() = default;

    Matrix(impl::MatrixInitializer<T> list) { setFromInitList(list); }

    Matrix& operator=(impl::MatrixInitializer<T> list)
    {
        setFromInitList(list);
        return *this;
    };

    template <size_t Nrows2, size_t Ncols2> Matrix(MatrixRef<T> mr)
    {
        assert(Nrows == mr.rows() && Ncols == mr.cols());
        setFromMatrixRef(mr);
    }

    template <size_t Nrows2, size_t Ncols2> Matrix& operator=(MatrixRef<T> mr)
    {
        assert(Nrows == mr.rows() && Ncols == mr.cols());
        setFromMatrixRef(mr);
    }

    ////// Accessors
    constexpr size_t n_rows() const { return Nrows; }
    constexpr size_t n_cols() const { return Ncols; }

    T* data() { return elems.data(); }
    const T* data() const { return elems.data(); }

    T& operator()(size_t i, size_t j)
    {
        assert(i < Nrows && j < Ncols);
        return elems[i * Nrows + j];
    }

    const T& operator()(size_t i, size_t j) const
    {
        assert(i < Nrows && j < Ncols);
        return elems[i * Nrows + j];
    }

    // subscripting with slices
    MatrixRef<T> operator()(slice rows, slice cols)
    {
        assert(rows.start < Nrows && rows.end <= Nrows && rows.start < rows.end);
        assert(cols.start < Ncols && cols.end <= Ncols && cols.start < cols.end);

        MatrixSlice sl { { Nrows, Ncols }, rows, cols };
        return { data(), sl };
    }

    MatrixRef<T> operator()(size_t row, slice cols)
    {
        assert(row < Nrows);
        assert(cols.start < Ncols && cols.end <= Ncols && cols.start < cols.end);

        MatrixSlice sl { { Nrows, Ncols }, slice(row, row + 1), cols };
        return { data(), sl };
    }

    MatrixRef<T> operator()(slice rows, size_t col)
    {
        assert(rows.start < Nrows && rows.end <= Nrows && rows.start < rows.end);
        assert(col < Ncols);

        MatrixSlice sl { { Nrows, Ncols }, rows, slice(col, col + 1) };
        return { data(), sl };
    }

private:
    std::array<T, size()> elems;

    void setFromInitList(impl::MatrixInitializer<T> list)
    {
        size_t i = 0;
        static_assert(list.size() == Nrows, "wrong number of rows");
        for (const auto& row : list) {
            static_assert(row.size() == Ncols, "wrong number of columns");
            for (const auto& elem : row) {
                elems[i++] = elem;
            }
        }
    }

    template <size_t Nrows2, size_t Ncols2> void setFromMatrixRef(MatrixRef<T> mr)
    {
        size_t i = 0;
        for (size_t r = 0; r < mr.rows(); ++r) {
            for (size_t c = 0; c < mr.cols(); ++c) {
                elems[i++] = mr(r, c);
            }
        }
    }
};

}
