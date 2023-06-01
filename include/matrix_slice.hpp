#pragma once

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

    MatrixSlice(std::array<size_t, 2> _extents, size_t _start = 0)
        : start { _start }
        , target_extents { _extents }
        , extents { _extents }
    {

        assert(_start < originSize());
        computeStrides();
    }

    MatrixSlice(std::array<size_t, 2> _extents, std::array<size_t, 2> _target_extents, size_t _start = 0)
        : start { _start }
        , target_extents { _target_extents }
        , extents { _extents }
    {

        assert(_start < originSize());
        computeStrides();
    }

    MatrixSlice(std::array<size_t, 2> _strides, std::array<size_t, 2> _extents, std::array<size_t, 2> _target_extents, size_t _start = 0)
        : start { _start }
        , target_extents { _target_extents }
        , extents { _extents }
        , strides { _strides }
    {
        assert(_start < originSize());
        assert(_strides[0] % _extents[1] == 0);
        assert(_strides[0] / _extents[1] < _extents[0]);
        assert(_strides[1] < _extents[1]);
    }

    size_t operator()(size_t i, size_t j) const
    {
        assert(i < target_extents[0] && j < target_extents[1]);
        return start + i * strides[0] + j * strides[1];
    }

    size_t size() const { return target_extents[0] * target_extents[1]; }

    size_t rows() const { return extents[0]; }
    size_t cols() const { return extents[1]; }

private:
    std::array<size_t, 2> strides; // How the index to the next element changes when we increment the index by 1
    std::array<size_t, 2> extents; // The rows and columns of the original Matrix
    std::array<size_t, 2> target_extents; // The rows and columns target Matrix
    size_t start { 0 }; // Offset of the first element

    void computeStrides()
    {
        strides[0] = extents[1];
        strides[1] = 1;
    }

    size_t originSize() const { return extents[0] * extents[1]; }
};

}
