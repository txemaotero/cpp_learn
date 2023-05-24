#pragma once

#include <cstddef>
#include <functional>
#include <numeric>

#include "matrix_fwd.hpp"
#include "matrix_slice.hpp"

namespace matrix_impl {
// Compute the strides of the matrix given the extents and returns the total
// number of elements
template <size_t N>
size_t compute_strides(const std::array<size_t, N> &extents,
                       std::array<size_t, N> &strides) {
  size_t size = 1;
  for (int i = N - 1; i >= 0; --i) {
    strides[i] = size;
    size *= extents[i];
  }
  return size;
}

template <size_t N> size_t compute_size(const std::array<size_t, N> &extents) {
  return std::accumulate(extents.begin(), extents.end(), 1,
                         std::multiplies<size_t>());
}

template <typename T, size_t N> struct MatrixInit {
  using type = std::initializer_list<typename MatrixInit<T, N - 1>::type>;
};

template <typename T> struct MatrixInit<T, 1> {
  using type = std::initializer_list<T>;
};

template <typename T> struct MatrixInit<T, 0>; // undefined

template <typename List> bool check_non_jagged(const List &list) {
  auto i = list.begin();
  for (auto j = i + 1; j != list.end(); ++j)
    if (j->size() != i->size())
      return false;
  return true;
}

template <size_t N, typename I, typename List>
std::enable_if<(N > 1), void> add_extents(I &first, const List &list) {
  assert(check_non_jagged(list));
  *first = list.size();
  add_extents<N - 1>(++first, *list.begin());
}

template <size_t N, typename I, typename List>
std::enable_if<(N == 1), void> add_extents(I &first, const List &list) {
  *first++ = list.size();
}

// Given an initializer_list of initializer_lists, put the size of each of the
// dimensions in the extents array
template <size_t N, typename List>
void derive_extents(const List &list, std::array<size_t, N> extents) {
  auto f = extents.begin();
  add_extents<N>(f, list);
}

template <typename T, typename Vec>
void add_list(std::initializer_list<T> *first, std::initializer_list<T> *last,
              Vec &vec) {
  for (; first != last; ++first)
    add_list(first->begin(), first->end(), vec);
}

template <typename T, typename Vec>
void add_list(const T *first, const T *last, Vec &vec) {
  vec.insert(vec.end(), first, last);
}

// Put the elements of the initializer_list into the vector
template <typename T, typename Vec>
void insert_flat(std::initializer_list<T> list, Vec &vec) {
  add_list(list.begin(), list.end(), vec);
}

// Get a row (or column or whatever the I dimension index stablish) with
// the given index "offset" from the orig matrix slice and put it in the dest
// matrix slice
template <size_t I, size_t N>
void slice_dim(size_t offset, MatrixSlice<N> &orig, MatrixSlice<N - 1> dest) {
  static_assert(I < N, "slice_dim: dimension index out of range");
  dest.start = orig.start;
  int j = N - 2;
  for (int i = N - 1; i >= 0; --i) {
    if (i == I)
      dest.start += orig.strides[i] * offset;
    else {
      dest.extents[j] = orig.extents[i];
      dest.strides[j] = orig.strides[i];
      --j;
    }
  }
  dest.size = matrix_impl::compute_size(dest.extents);
}
} // namespace matrix_impl
