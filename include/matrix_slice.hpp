#pragma once

#include "template_utils.hpp"
#include <cstddef>
#include <initializer_list>
#include <numeric>

namespace matrix {

template <size_t Nrows, size_t Ncols>
class MatrixSlice {
    static constexpr size_t totalSize = Nrows * Ncols;
    static constexpr size_t nRows = Nrows;
    static constexpr size_t nCols = Ncols;
    size_t start;
    std::array<size_t, 2> strides { Ncols, 1 };

    MatrixSlice(MatrixSlice&&) = default;
    MatrixSlice& operator=(MatrixSlice&&) = default;
    MatrixSlice(const MatrixSlice&) = default;
    MatrixSlice& operator=(const MatrixSlice&) = default;
    ~MatrixSlice() = default;

    MatrixSlice(size_t _start = 0)
        : start { _start }
    {
        static_assert(_start < totalSize, "start index out of range");
    }

    MatrixSlice(std::array<size_t, 2> _strides, size_t _start = 0)
        : start { _start }
        , strides { _strides }
    {
        static_assert(_start < totalSize, "start index out of range");
    }

    std::size_t operator()(size_t i, size_t j) const
    {
        return start + i * strides[0] + j * strides[1];
    }
};

}
