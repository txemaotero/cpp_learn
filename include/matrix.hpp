

#include "matrix_base.hpp"
#include "matrix_impl.hpp"
#include "matrix_ref.hpp"
#include "matrix_slice.hpp"
#include "slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T, size_t Nrows, size_t Ncols>
class Matrix : public MatrixBase<T, Nrows, Ncols> {
public:
    using MatrixBase<T, Nrows, Ncols>::size;

    using iterator = typename std::array<T, size>::iterator;

    Matrix() = default;
    Matrix(Matrix&&) = default; // move
    Matrix& operator=(Matrix&&) = default;
    Matrix(const Matrix&) = default; // copy
    Matrix& operator=(const Matrix&) = default;
    ~Matrix() = default;

    Matrix(impl::MatrixInitializer<T> list)
    {
        setFromInitList(list);
    }
    Matrix& operator=(impl::MatrixInitializer<T> list)
    {
        setFromInitList(list);
        return *this;
    };

    template <size_t Nrows2, size_t Ncols2>
    Matrix(MatrixRef<T, Nrows2, Ncols2> mr)
    {
        assert(Nrows == mr.rows() && Ncols == mr.cols());
        setFromMatrixRef(mr);
    }

    template <size_t Nrows2, size_t Ncols2>
    Matrix& operator=(MatrixRef<T, Nrows2, Ncols2> mr)
    {
        assert(Nrows == mr.rows() && Ncols == mr.cols());
        setFromMatrixRef(mr);
    }

    T* data() override { return elems_.data(); }
    const T* data() const override { return elems_.data(); }

    using MatrixBase<T, Nrows, Ncols>::operator();

    // subscripting with slices
    MatrixRef<T, Nrows, Ncols> operator()(slice rows, slice cols)
    {
        assert(rows.start < Nrows && rows.end <= Nrows && rows.start < rows.end);
        assert(cols.start < Ncols && cols.end <= Ncols && cols.start < cols.end);

        std::array<size_t, 2> strides { Nrows * rows.stride, cols.stride };
        MatrixSlice slice { strides, { Nrows, Ncols }, { rows.size(), cols.size() }, rows.start * Nrows + cols.start };
        return { data(), slice };
    }

    MatrixRef<T, Nrows, Ncols> operator()(size_t row, slice cols)
    {
        assert(row < Nrows);
        assert(cols.start < Ncols && cols.end <= Ncols && cols.start < cols.end);

        std::array<size_t, 2> strides { Nrows, cols.stride };
        MatrixSlice slice { strides, { Nrows, Ncols }, { 1, cols.size() }, row * Nrows + cols.start };
        return { data(), slice };
    }

    MatrixRef<T, Nrows, Ncols> operator()(slice rows, size_t col)
    {
        assert(rows.start < Nrows && rows.end <= Nrows && rows.start < rows.end);
        assert(col < Ncols);

        std::array<size_t, 2> strides { Nrows * rows.stride, 1 };
        MatrixSlice slice { strides, { Nrows, Ncols }, { rows.size(), 1 }, rows.start * Nrows + col };
        return { data(), slice };
    }

private:
    std::array<T, size> elems_;

    void setFromInitList(impl::MatrixInitializer<T> list)
    {
        size_t i = 0;
        static_assert(list.size() == Nrows, "wrong number of rows");
        for (const auto& row : list) {
            static_assert(row.size() == Ncols, "wrong number of columns");
            for (const auto& elem : row) {
                elems_[i++] = elem;
            }
        }
    }

    template <size_t Nrows2, size_t Ncols2>
    void setFromMatrixRef(MatrixRef<T, Nrows2, Ncols2> mr)
    {
        size_t i = 0;
        for (size_t r = 0; r < mr.rows(); ++r) {
            for (size_t c = 0; c < mr.cols(); ++c) {
                elems_[i++] = mr(r, c);
            }
        }
    }
};

}
