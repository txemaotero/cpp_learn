#pragma once

#include <cstddef>
#include <ostream>

struct slice {
  slice() : start(-1), length(-1), stride(1) {}
  explicit slice(std::size_t s) : start(s), length(-1), stride(1) {}
  slice(std::size_t s, std::size_t l, std::size_t n = 1)
      : start(s), length(l), stride(n) {}

  std::size_t operator()(std::size_t i) const { return start + i * stride; }

  static slice all;

  std::size_t start;  // first index
  std::size_t length; // number of indices included
  std::size_t stride; // distance between elements in sequence
};

template <typename C, typename T>
std::basic_ostream<C, T> &operator<<(std::basic_ostream<C, T> &os,
                                     const slice &s) {
  return os << '(' << s.start << ' ' << s.length << ' ' << s.stride << ')';
}
