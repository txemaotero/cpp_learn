#pragma once

#include "slice.hpp"
#include <array>
#include <cstddef>

namespace template_utils {

template <size_t Index, size_t N, size_t... Ns>
struct NumAt {
    static constexpr size_t value = NumAt<Index - 1, Ns...>::value;
};

template <size_t N, size_t... Ns>
struct NumAt<0, N, Ns...> {
    static constexpr size_t value = N;
};

template <size_t N, size_t... Ns>
struct Product {
    static constexpr size_t value = N * Product<Ns...>::value;
};

template <size_t N>
struct Product<N> {
    static constexpr size_t value = N;
};

template <bool B, typename T = void>
using Enable_if = typename std::enable_if<B, T>::type;

template <size_t... Args>
std::array<size_t, sizeof...(Args)> convertToArrayHelper()
{
    return std::array<size_t, sizeof...(Args)> {};
}

template <size_t... Ns>
std::array<size_t, sizeof...(Ns)> convertToArray()
{
    static constexpr size_t Nelems = sizeof...(Ns);
    return std::array<size_t, Nelems> {};
}

template <typename X, typename Y>
constexpr bool Convertible()
{
    return std::is_convertible<X, Y>::value;
}

template <typename X, typename Y>
constexpr bool Same()
{
    return std::is_same<X, Y>::value;
}

template <typename... Args>
constexpr bool Requesting_element()
{
    return All(Convertible<Args, size_t>()...);
}

template <typename... Args>
constexpr bool Requesting_slice()
{
    return All((Convertible<Args, size_t>() || Same<Args, ::tensor::slice>())...) && Some(Same<Args, ::tensor::slice>()...);
}

}
