#pragma once

#include "template_utils.hpp"
#include <cstddef>
#include <initializer_list>
#include <numeric>

template <size_t... exts>
class TensorSlice {
    static constexpr size_t rank = sizeof...(exts);
    static constexpr size_t size = template_utils::Product<exts...>::value;
    static std::array<size_t, size> extents { size_t { exts }... };

    std::array<size_t, size> strides;
    size_t start;

    TensorSlice(size_t _start = 0)
        : start { _start }
    {
        static_assert(_start < size, "start index out of range");
        computeStrides();
    }

    TensorSlice(std::initializer_list<size_t> _strides, size_t _start = 0)
        : start { _start }
    {
        static_assert(_start < size, "start index out of range");
        static_assert(_strides.size() == rank, "wrong number of strides");
        std::copy(_strides.begin(), _strides.end(), strides.begin());
    }

    template <typename... Dims>
    std::size_t operator()(Dims... dims) const
    {
        static_assert(sizeof...(Dims) == rank, "wrong number of indices");
        std::size_t args[rank] { std::size_t(dims)... }; // copy arguments into an array
        return std::inner_product(args, args + rank, strides.begin(), start);
    }

private:
    void computeStrides()
    {
        size_t size = 1;
        for (int i = rank - 1; i >= 0; --i) {
            strides[i] = size;
            size *= extents[i];
        }
    }
};
