#pragma once

#include <cstddef>
#include <ostream>

namespace matrix {

struct slice {
    size_t start; // first index
    size_t end; // number of indices included
    size_t stride; // distance between elements in sequence

    slice(size_t start_ = 0, size_t end_ = 1, size_t strid_ = 1)
        : start { start_ }
        , end { end_ }
        , stride { strid_ }
    {
        assert(start < end);
    }

    size_t size() const {
        size_t d = (end - start);
        size_t s = d / stride;
        if (s * stride < d)
            ++s;
        return s; 
    }
};

template <typename C, typename T> std::basic_ostream<C, T>& operator<<(std::basic_ostream<C, T>& os, const slice& s)
{
    return os << '(' << s.start << ' ' << s.end << ' ' << s.stride << ')';
};

}
