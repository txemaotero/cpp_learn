#pragma once

#include "matrix_impl.hpp"
#include "matrix_slice.hpp"
#include "template_utils.hpp"
#include <cstddef>

namespace tensor {

template <typename T, size_t... Ns>
class TensorBase {
public:
    static constexpr size_t size = template_utils::Product<Ns...>::value;
    static constexpr size_t rank = sizeof...(Ns);
    using value_type = T;

    TensorBase(TensorBase&&) = default;
    TensorBase& operator=(TensorBase&&) = default;
    TensorBase(const TensorBase&) = default;
    TensorBase& operator=(const TensorBase&) = default;
    ~TensorBase() = default;

    explicit TensorBase(const TensorSlice<Ns...>& ms)
        : extents { ms }
    {
    }

    size_t extent(size_t n) const { return extents.extents[n]; }

    const TensorSlice<Ns...>& descriptor() const { return extents; }

    virtual T* data() = 0;
    virtual const T* data() const = 0;

    size_t n_rows() const { return extents.extents[0]; }
    size_t n_cols() const { return extents.extents[1]; }

    // m(i,j,k) subscripting with integers
    template <typename... Args>
    template_utils::Enable_if<template_utils::Requesting_element<Args...>(), T&>
    operator()(Args... args)
    {
        assert(impl::check_bounds(this->desc_, args...));
        return *(data() + this->desc_(args...));
    }

    template <typename... Args>
    template_utils::Enable_if<template_utils::Requesting_element<Args...>(), const T&>
    operator()(Args... args) const
    {
        assert(impl::check_bounds(this->desc_, args...));
        return *(data() + this->desc_(args...));
    }

protected:
    TensorSlice<Ns...> extents {};
};

}
