
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <numeric>
#include <vector>

#include "matrix_impl.hpp"

template <typename T, size_t N>
using Matrix_initializer = typename matrix_impl::MatrixInit<T, N>::type;

// Common base class for public Matrix and MatrixRef
template <typename T, size_t N> class Matrix_base {
public:
  static constexpr size_t order = N;
  using value_type = T;
  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;

  Matrix_base(Matrix_base &&) = default;
  Matrix_base &operator=(Matrix_base &&) = default;

  Matrix_base(const Matrix_base &) = default;
  Matrix_base &operator=(const Matrix_base &) = default;

  ~Matrix_base() = default;
};

template <typename T, size_t N> class Matrix_ref : public Matrix_base<T, N> {
public:
  Matrix_ref(const MatrixSlice<N> &s, T *p) : desc{s}, ptr{p} {}

private:
  MatrixSlice<N> desc;
  T *ptr;
};

template <typename T, size_t N> class Matrix : public Matrix_base<T, N> {
public:
  Matrix() = default;
  Matrix(Matrix &&) = default;
  Matrix &operator=(Matrix &&) = default;

  Matrix(const Matrix &) = default;
  Matrix &operator=(const Matrix &) = default;

  ~Matrix() = default;

  template <typename... Exts>
  Matrix(Exts... exts) : desc{exts...}, elems(desc.size) {}

  Matrix(Matrix_initializer<T, N> init) {
    matrix_impl::derive_extents(init, desc.extents);
    elems.reserve(desc.size);
    matrix_impl::insert_flat(init, elems);
    assert(elems.size() == desc.size);
  };

  size_t rows() const { return desc.extents[0]; }

  // Get row
  Matrix_ref<T, N - 1> operator[](size_t i) { return row(i); }

  Matrix_ref<T, N - 1> row(size_t i) {
    assert(i < rows());
    Matrix_slice<N - 1>;
    row;
    matrix_impl::slice_dim<0>(i, desc, row);
    return {row, data()};
  }

  T *data() { return elems.data(); }

private:
  MatrixSlice<N> desc;
  std::vector<T> elems;
};
