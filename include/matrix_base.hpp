#pragma once

#include "matrix_slice.hpp"
#include "template_utils.hpp"
#include <cstddef>

namespace matrix {

template <typename T, size_t Nrows, size_t Ncols>
class MatrixBase {
public:
    static constexpr size_t size = Nrows * Ncols;
    static constexpr size_t nRows = Nrows;
    static constexpr size_t nCols = Ncols;
    using value_type = T;

    MatrixBase(MatrixBase&&) = default;
    MatrixBase& operator=(MatrixBase&&) = default;
    MatrixBase(const MatrixBase&) = default;
    MatrixBase& operator=(const MatrixBase&) = default;
    ~MatrixBase() = default;

    MatrixBase()
        : extents { { nRows, nCols } }
    {
    }

    explicit MatrixBase(const MatrixSlice& ms)
        : extents { ms }
    {
    }

    virtual T* data() = 0;
    virtual const T* data() const = 0;

    constexpr size_t n_rows() const { return nRows; }
    constexpr size_t n_cols() const { return nCols; }

    // m(i,j,k) subscripting with integers
    T& operator()(size_t i, size_t j)
    {
        assert(i < nRows && j < nCols);
        return *(data() + extents(i, j));
    }

    const T& operator()(size_t i, size_t j) const
    {
        assert(i < nRows && j < nCols);
        return *(data() + extents(i, j));
    }

protected:
    MatrixSlice extents;
};

}
