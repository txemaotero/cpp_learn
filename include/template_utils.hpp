#pragma once

#include "slice.hpp"
#include "matrix_fwd.hpp"
#include <array>
#include <cstddef>

namespace matrix {
namespace template_utils {

    template <bool B, typename T = void> using Enable_if = typename std::enable_if<B, T>::type;

    template <typename X, typename Y> constexpr bool Convertible() { return std::is_convertible<X, Y>::value; }

    template <typename X, typename Y> constexpr bool Same() { return std::is_same<X, Y>::value; }

    struct substitution_failure { };

    template <typename T> struct substitution_succeeded : std::true_type { };

    template <> struct substitution_succeeded<substitution_failure> : std::false_type { };

    template <typename M> struct get_matrix_type_result {
        template <typename T, size_t Nrows, size_t Ncols> static bool check(const Matrix<T, Nrows, Ncols>& m);

        template <typename T> static bool check(const MatrixRef<T>& m);

        static substitution_failure check(...);

        using type = decltype(check(std::declval<M>()));
    };

    template <typename T> struct has_matrix_type : substitution_succeeded<typename get_matrix_type_result<T>::type> { };

    template <typename M> constexpr bool Has_matrix_type() { return has_matrix_type<M>::value; }
}
}
