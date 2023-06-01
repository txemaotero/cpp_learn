#pragma once

#include <cstddef>
#include <ostream>

namespace matrix {

struct slice {
    size_t start; // first index
    size_t end; // number of indices included
    size_t stride; // distance between elements in sequence

    size_t size() const { return (end - start) / stride; }
};

template <typename C, typename T>
std::basic_ostream<C, T>& operator<<(std::basic_ostream<C, T>& os,
    const slice& s)
{
    return os << '(' << s.start << ' ' << s.end << ' ' << s.stride << ')';
};

}
