#pragma once

#include "matrix_fwd.hpp"
#include <type_traits>

template <typename X, typename Y>
constexpr bool Same()
{
    return std::is_same<X, Y>::value;
}

template <bool B, typename T = void>
using Enable_if = typename std::enable_if<B, T>::type;

template <typename X, typename Y>
constexpr bool Convertible()
{
    return std::is_convertible<X, Y>::value;
}

struct substitution_failure { };

template <typename T>
struct substitution_succeeded : std::true_type {
};

template <>
struct substitution_succeeded<substitution_failure> : std::false_type {
};

template <typename M>
struct get_matrix_type_result {
    template <typename T, size_t N, typename = Enable_if<(N >= 1)>>
    static bool check(const Matrix<T, N>& m);

    template <typename T, size_t N, typename = Enable_if<(N >= 1)>>
    static bool check(const MatrixRef<T, N>& m);

    static substitution_failure check(...);

    using type = decltype(check(std::declval<M>()));
};

template <typename T>
struct has_matrix_type
    : substitution_succeeded<typename get_matrix_type_result<T>::type> {
};

template <typename M>
constexpr bool Has_matrix_type()
{
    return has_matrix_type<M>::value;
}

template <typename M>
constexpr bool Matrix_type()
{
    return Has_matrix_type<M>();
}
