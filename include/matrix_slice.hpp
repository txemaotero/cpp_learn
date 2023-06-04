#pragma once

#include "slice.hpp"
#include "template_utils.hpp"
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <numeric>

namespace matrix {

class MatrixSlice {
public:
    MatrixSlice(MatrixSlice&&) = default;
    MatrixSlice& operator=(MatrixSlice&&) = default;
    MatrixSlice(const MatrixSlice&) = default;
    MatrixSlice& operator=(const MatrixSlice&) = default;
    ~MatrixSlice() = default;

    MatrixSlice(std::array<size_t, 2> _extents, slice _rows, slice _cols)
        : extents { _extents }
        , rowSlice { _rows }
        , colSlice { _cols }
        , start { _rows.start * _extents[1] + _cols.start }
    {
        assert(_rows.end <= _extents[0]);
        assert(_cols.end <= _extents[1]);
        strides[0] = _rows.stride * extents[1];
        strides[1] = _cols.stride;
    }

    size_t operator()(size_t i, size_t j) const
    {
        assert(i < rowSlice.size() && j < colSlice.size());
        return start + i * strides[0] + j * strides[1];
    }

    size_t rows() const { return rowSlice.size(); }
    size_t cols() const { return colSlice.size(); }

    size_t size() const { return rows() * cols(); }

    const slice& getRowSlice() const { return rowSlice; }
    const slice& getColSlice() const { return colSlice; }

    MatrixSlice operator()(slice rows, slice cols) const
    {
        assert(rows.end <= extents[0]);
        assert(cols.end <= extents[1]);

        auto new_rowStride = rowSlice.stride * rows.stride;
        slice newRows { rowSlice.start + rows.start, rowSlice.start + rows.size() * new_rowStride, new_rowStride };
        auto new_colStride = colSlice.stride * cols.stride;
        slice newCols { colSlice.start + cols.start, colSlice.start + cols.size() * new_colStride, new_colStride };

        return { extents, newRows, newCols };
    }

private:
    std::array<size_t, 2> extents; // The rows and columns of the original Matrix
    std::array<size_t, 2> strides; // Strides to iterate over the original Matrix
    size_t start { 0 }; // Offset of the first element
    slice rowSlice;
    slice colSlice;

    size_t originSize() const { return extents[0] * extents[1]; }
};

}
