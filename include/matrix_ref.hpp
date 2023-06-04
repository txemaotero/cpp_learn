


#include "matrix_fwd.hpp"
#include "matrix_impl.hpp"
#include "matrix_slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T> class MatrixRef {
public:
    using value_type = T;
    MatrixRef() = delete;
    MatrixRef(MatrixRef&&) = default; // move
    MatrixRef& operator=(MatrixRef&&) = default;
    MatrixRef(const MatrixRef&) = default; // copy
    MatrixRef& operator=(const MatrixRef&) = default;
    ~MatrixRef() = default;

    MatrixRef(T* elems_, const MatrixSlice& ms)
        : elems { elems_ }
        , extents { ms }
    {
    }

    size_t n_rows() const { return extents.rows(); }
    size_t n_cols() const { return extents.cols(); }
    size_t size() const { return n_rows() * n_cols(); }

    // m(i,j,k) subscripting with integers
    T& operator()(size_t i, size_t j)
    {
        assert(i < extents.rows() && j < extents.cols());
        return *(elems + extents(i, j));
    }

    const T& operator()(size_t i, size_t j) const
    {
        assert(i < extents.rows() && j < extents.cols());
        return *(elems + extents(i, j));
    }

    // subscripting with slices
    MatrixRef<T> operator()(slice rows, slice cols)
    {
        assert(rows.start < extents.rows() && rows.end <= extents.rows() && rows.start < rows.end);
        assert(cols.start < extents.cols() && cols.end <= extents.cols() && cols.start < cols.end);

        return { elems, extents(rows, cols) };
    }

    MatrixRef<T> operator()(size_t row, slice cols)
    {
        assert(row < extents.rows());
        assert(cols.start < extents.cols() && cols.end <= extents.cols() && cols.start < cols.end);

        return { elems, extents({ row, row + 1 }, cols) };
    }

    MatrixRef<T> operator()(slice rows, size_t col)
    {
        assert(col < extents.cols());
        assert(rows.start < extents.rows() && rows.end <= extents.rows() && rows.start < rows.end);

        return { elems, extents(rows, { col, col + 1 }) };
    }

    ///// Operators
    template <typename F> MatrixRef<T>& apply(F f)
    {
        for (size_t i = 0; i < n_rows(); ++i) {
            for (size_t j = 0; j < n_cols(); ++j) {
                f((*this)(i, j));
            }
        }
        return *this;
    }

    template <typename F, typename T2>
    template_utils::Enable_if<template_utils::Convertible<T2, T>(), MatrixRef<T>&>
    apply(const MatrixRef<T2>& mr, F f)
    {
        size_t nrows = n_rows();
        size_t ncols = n_cols();
        assert(nrows == mr.n_rows() && ncols == mr.n_cols());
        for (size_t row = 0; row < nrows; ++row) {
            for (size_t col = 0; col < ncols; ++col) {
                f(this(row, col), mr(row, col));
            }
        }

        return *this;
    }

    bool operator==(const MatrixRef<T>& rhs) const
    {
        if (n_rows() != rhs.n_rows() || n_cols() != rhs.n_cols())
            return false;

        for (size_t i = 0; i < n_rows(); ++i) {
            for (size_t j = 0; j < n_cols(); ++j) {
                if ((*this)(i, j) != rhs(i, j))
                    return false;
            }
        }

        return true;
    }

    template<size_t Nrows, size_t Ncols>
    bool operator==(const Matrix<T, Nrows, Ncols>& rhs) const
    {
        return rhs == *this;
    }

private:
    T* elems { nullptr };
    MatrixSlice extents;
};

} // namespace matrix
