#pragma once

#include <cstddef>
#include <ostream>

namespace matrix {

struct slice {
    std::size_t start; // first index
    std::size_t end; // number of indices included
    std::size_t stride; // distance between elements in sequence
};

template <typename C, typename T>
std::basic_ostream<C, T>& operator<<(std::basic_ostream<C, T>& os,
    const slice& s)
{
    return os << '(' << s.start << ' ' << s.end << ' ' << s.stride << ')';
};

template <size_t Start, size_t End, size_t Stride = 1>
struct static_slice {
    static constexpr size_t start = Start;
    static constexpr size_t end = End;
    static constexpr size_t stride = Stride;
    static constexpr size_t size = (end - start) / stride;
};

}
