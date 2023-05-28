#pragma once

#include <array>
#include <cstddef>

#include "matrix_impl.hpp"

template <size_t N>
struct MatrixSlice {
    size_t size; // total number of elements
    size_t start; // index of first element
    std::array<size_t, N> extents; // number of elements in each dimension
    std::array<size_t, N> strides; // offset between elements in each dimension

    MatrixSlice()
        : size { 1 }
        , start { 0 }
    {
        std::fill(extents.begin(), extents.end(), 0);
        std::fill(strides.begin(), strides.end(), 1);
    };

    MatrixSlice(size_t s, std::initializer_list<size_t> exts)
        : start { s }
    {
        static_assert(exts.size() == N, "Error: wrong number of extents");
        std::copy(exts.begin(), exts.end(), extents.begin());
        size = matrix_impl::compute_strides(extents, strides);
    }

    MatrixSlice(size_t s, std::initializer_list<size_t> exts,
        std::initializer_list<size_t> strs)
    {
        static_assert(exts.size() == N, "Error: wrong number of extents");
        static_assert(strs.size() == N, "Error: wrong number of strides");
        std::copy(exts.begin(), exts.end(), extents.begin());
        std::copy(strs.begin(), strs.end(), strides.begin());
        size = matrix_impl::compute_size(extents);
    }

    MatrixSlice(size_t s, const std::initializer_list<size_t>& exts)
        : start { s }
        , extents { exts }
    {
        static_assert(exts.size() == N, "Error: wrong number of extents");
        size = matrix_impl::compute_strides(extents, strides);
    }

    template <typename... Dims>
    MatrixSlice(Dims... dims)
        : start { 0 }
        , extents { size_t(dims)... }
    {
        static_assert(sizeof...(Dims) == N, "Error: wrong number of dimensions");
        size = matrix_impl::compute_strides(extents, strides);
    }

    template <typename... Dims>
    std::size_t operator()(Dims... dims) const
    {
        static_assert(sizeof...(Dims) == N,
            "MatrixSlice<N>::operator(): dimension mismatch");
        std::size_t args[N] { std::size_t(dims)... }; // copy arguments into an array
        return std::inner_product(args, args + N, strides.begin(), start);
    }
};
