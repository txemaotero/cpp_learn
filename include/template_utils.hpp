#pragma once

#include "slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {
namespace template_utils {

    template <bool B, typename T = void>
    using Enable_if = typename std::enable_if<B, T>::type;

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

}
}
