#pragma once

#include <cstddef>

#include "matrix_base.hpp"
#include "matrix_fwd.hpp"
#include "matrix_impl.hpp"

template <typename T, std::size_t N> class MatrixRefIterator;

template <typename T, std::size_t N> class MatrixRef : public MatrixBase<T, N> {
  // ----------------------------------------
  // The core member functions in book 'TCPL'
  // ----------------------------------------
public:
  //! @cond Doxygen_Suppress
  using iterator = MatrixRefIterator<T, N>;
  using const_iterator = MatrixRefIterator<const T, N>;

  MatrixRef() = delete;
  MatrixRef(MatrixRef &&) = default; // move
  MatrixRef &operator=(MatrixRef &&);
  MatrixRef(const MatrixRef &) = default; // copy
  MatrixRef &operator=(const MatrixRef &);
  ~MatrixRef() = default;
  //! @endcond

  //! construct from MatrixRef
  template <typename U> MatrixRef(const MatrixRef<U, N> &x);
  //! assign from MatrixRef
  template <typename U> MatrixRef &operator=(const MatrixRef<U, N> &x);

  //! construct from Matrix
  template <typename U> MatrixRef(const Matrix<U, N> &);
  //! assign from Matrix
  template <typename U> MatrixRef &operator=(const Matrix<U, N> &);

  //! assign from list
  MatrixRef &operator=(MatrixInitializer<T, N>);

  MatrixRef(const MatrixSlice<N> &s, T *p) : MatrixBase<T, N>{s}, ptr_{p} {}

  //! total number of elements
  std::size_t size() const { return this->desc_.size; }

  //! "flat" element access
  ///@{
  T *data() { return ptr_; }
  const T *data() const { return ptr_; }
  ///@}

private:
  T *ptr_;

  // ---------------------------------------------
  // Member functions for subscripting and slicing
  // ---------------------------------------------
public:
  //! m(i,j,k) subscripting with integers
  using MatrixBase<T, N>::operator();

  //! m(s1, s2, s3) subscripting with slides
  ///@{
  template <typename... Args>
  Enable_if<matrix_impl::Requesting_slice<Args...>(), MatrixRef<T, N>>
  operator()(const Args &...args);

  template <typename... Args>
  Enable_if<matrix_impl::Requesting_slice<Args...>(), MatrixRef<const T, N>>
  operator()(const Args &...args) const;
  ///@}

  //! m[i] row access
  ///@{
  MatrixRef<T, N - 1> operator[](std::size_t i) { return row(i); }
  MatrixRef<const T, N - 1> operator[](std::size_t i) const { return row(i); }
  ///@}

  //! row access
  ///@{
  MatrixRef<T, N - 1> row(std::size_t n);
  MatrixRef<const T, N - 1> row(std::size_t n) const;
  ///@}

  //! column access
  ///@{
  MatrixRef<T, N - 1> col(std::size_t n);
  MatrixRef<const T, N - 1> col(std::size_t n) const;
  ///@}

  //! multiple rows access
  ///@{
  MatrixRef<T, N> rows(std::size_t i, std::size_t j);
  MatrixRef<const T, N> rows(std::size_t i, std::size_t j) const;
  ///@}

  //! multiple columns access
  ///@{
  MatrixRef<T, N> cols(std::size_t i, std::size_t j);
  MatrixRef<const T, N> cols(std::size_t i, std::size_t j) const;
  ///@}

  //! element iterators
  ///@{
  iterator begin() { return {this->desc_, ptr_}; }
  const_iterator begin() const { return {this->desc_, ptr_}; }
  iterator end() { return {this->desc_, ptr_, true}; }
  const_iterator end() const { return {this->desc_, ptr_, true}; }
  ///@}

  // --------------------------------------------------
  // Member functions for matrix arithmetic operations
  // --------------------------------------------------
public:
  //! matrix arithmetic operations
  ///@{
  template <typename F> MatrixRef &apply(F f); // f(x) for every element x

  // f(x, mx) for corresponding elements of *this and m
  template <typename M, typename F>
  Enable_if<Matrix_type<M>(), MatrixRef &> apply(const M &m, F f);
};

template <typename T, std::size_t N>
MatrixRef<T, N> &MatrixRef<T, N>::operator=(MatrixRef &&x) {
  assert(same_extents(this->desc_, x.desc_));
  std::move(x.begin(), x.end(), begin());

  return *this;
}

template <typename T, std::size_t N>
MatrixRef<T, N> &MatrixRef<T, N>::operator=(const MatrixRef &x) {
  assert(same_extents(this->desc_, x.desc_));
  std::copy(x.begin(), x.end(), begin());

  return *this;
}

template <typename T, std::size_t N>
template <typename U>
MatrixRef<T, N>::MatrixRef(const MatrixRef<U, N> &x)
    : MatrixBase<T, N>{x.descriptor()}, ptr_(x.data()) {}

template <typename T, std::size_t N>
template <typename U>
MatrixRef<T, N> &MatrixRef<T, N>::operator=(const MatrixRef<U, N> &x) {
  static_assert(Convertible<U, T>(), "MatrixRef =: incompatible element types");
  assert(this->desc_.extents == x.descriptor().extents);

  std::copy(x.begin(), x.end(), begin());
  return *this;
}

template <typename T, std::size_t N>
template <typename U>
MatrixRef<T, N>::MatrixRef(const Matrix<U, N> &x)
    : MatrixBase<T, N>{x.descriptor()}, ptr_(x.data()) {}

template <typename T, std::size_t N>
template <typename U>
MatrixRef<T, N> &MatrixRef<T, N>::operator=(const Matrix<U, N> &x) {
  static_assert(Convertible<U, T>(), "MatrixRef =: incompatible element types");
  assert(this->desc_.extents == x.descriptor().extents);

  std::copy(x.begin(), x.end(), begin());
  return *this;
}

template <typename T, std::size_t N>
MatrixRef<T, N> &MatrixRef<T, N>::operator=(MatrixInitializer<T, N> init) {
  // std::array<std::size_t, N> extents = matrix_impl::derive_extents<N>(init);
  assert(matrix_impl::derive_extents<N>(init) == this->desc_.extents);

  auto iter = begin();
  matrix_impl::copy_flat(init, iter);

  return *this;
}

template <typename T, size_t N>
template <typename... Args>
Enable_if<matrix_impl::Requesting_slice<Args...>(), MatrixRef<T, N>>
MatrixRef<T, N>::operator()(const Args &...args) {
  MatrixSlice<N> d;
  d.start = this->desc_.start + matrix_impl::do_slice(this->desc_, d, args...);
  d.size = matrix_impl::compute_size(d.extents);
  return {d, data()};
}

template <typename T, size_t N>
template <typename... Args>
Enable_if<matrix_impl::Requesting_slice<Args...>(), MatrixRef<const T, N>>
MatrixRef<T, N>::operator()(const Args &...args) const {
  MatrixSlice<N> d;
  d.start = this->desc_.start + matrix_impl::do_slice(this->desc_, d, args...);
  d.size = matrix_impl::compute_size(d.extents);
  return {d, data()};
}

// row
template <typename T, size_t N>
MatrixRef<T, N - 1> MatrixRef<T, N>::row(size_t n) {
  assert(n < this->n_rows());
  MatrixSlice<N - 1> row;
  matrix_impl::slice_dim<0>(n, this->desc_, row);
  return {row, ptr_};
}

template <typename T, size_t N>
MatrixRef<const T, N - 1> MatrixRef<T, N>::row(size_t n) const {
  assert(n < this->n_rows());
  MatrixSlice<N - 1> row;
  matrix_impl::slice_dim<0>(n, this->desc_, row);
  return {row, ptr_};
}

// col
template <typename T, size_t N>
MatrixRef<T, N - 1> MatrixRef<T, N>::col(size_t n) {
  assert(n < this->cols());
  MatrixSlice<N - 1> col;
  matrix_impl::slice_dim<1>(n, this->desc_, col);
  return {col, ptr_};
}

template <typename T, size_t N>
MatrixRef<const T, N - 1> MatrixRef<T, N>::col(size_t n) const {
  assert(n < this->cols());
  MatrixSlice<N - 1> col;
  matrix_impl::slice_dim<1>(n, this->desc_, col);
  return {col, ptr_};
}

template <typename T, std::size_t N>
MatrixRef<T, N> MatrixRef<T, N>::rows(std::size_t i, std::size_t j) {
  assert(i < j);
  assert(j < this->n_rows());

  MatrixSlice<N> d;
  d.start = this->desc_.start;
  d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice{i, j - i + 1});
  std::size_t NRest = N - 1;
  while (NRest >= 1) {
    d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice{0}, NRest);
    --NRest;
  }
  return {d, data()};
}

template <typename T, std::size_t N>
MatrixRef<const T, N> MatrixRef<T, N>::rows(std::size_t i,
                                            std::size_t j) const {
  assert(i < j);
  assert(j < this->n_rows());

  MatrixSlice<N> d;
  d.start = this->desc_.start;
  d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice{i, j - i + 1});
  std::size_t NRest = N - 1;
  while (NRest >= 1) {
    d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice{0}, NRest);
    --NRest;
  }
  return {d, data()};
}

template <typename T, std::size_t N>
MatrixRef<T, N> MatrixRef<T, N>::cols(std::size_t i, std::size_t j) {
  assert(N >= 2);
  assert(i < j);
  assert(j < this->n_cols());

  MatrixSlice<N> d;
  d.start = this->desc_.start;
  d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice{0});
  d.start +=
      matrix_impl::do_slice_dim<N - 1>(this->desc_, d, slice{i, j - i + 1});

  std::size_t NRest = N - 2;
  while (NRest >= 1) {
    d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice{0}, NRest);
    --NRest;
  }
  return {d, data()};
}

template <typename T, std::size_t N>
MatrixRef<const T, N> MatrixRef<T, N>::cols(std::size_t i,
                                            std::size_t j) const {
  assert(N >= 2);
  assert(i < j);
  assert(j < this->n_cols());

  MatrixSlice<N> d;
  d.start = this->desc_.start;
  d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice{0});
  d.start +=
      matrix_impl::do_slice_dim<N - 1>(this->desc_, d, slice{i, j - i + 1});

  std::size_t NRest = N - 2;
  while (NRest >= 1) {
    d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice{0}, NRest);
    --NRest;
  }
  return {d, data()};
}

template <typename T, std::size_t N>
template <typename F>
MatrixRef<T, N> &MatrixRef<T, N>::apply(F f) {
  for (auto iter = begin(); iter != end(); ++iter)
    f(*iter);
  return *this;
}

template <typename T, std::size_t N>
template <typename M, typename F>
Enable_if<Matrix_type<M>(), MatrixRef<T, N> &>
MatrixRef<T, N>::apply(const M &m, F f) {
  assert(same_extents(this->desc_, m.descriptor()));
  auto j = m.begin();
  for (auto i = begin(); i != end(); ++i) {
    f(*i, *j);
    ++j;
  }

  return *this;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const MatrixRef<T, 0> &mr0) {
  return os << (const T &)mr0;
}

template <typename T> class MatrixRef<T, 0> : public MatrixBase<T, 0> {
public:
  using iterator = T *;
  using const_iterator = const T *;

  MatrixRef(const MatrixSlice<0> &s, T *p) : ptr_{p + s.start} {}

  //! total number of elements
  std::size_t size() const { return 1; }

  //! "flat" element access
  ///@{
  T *data() { return ptr_; }
  const T *data() const { return ptr_; }
  ///@}

  T &operator()() { return *ptr_; };
  const T &operator()() const { return *ptr_; }

  operator T &() { return *ptr_; }
  operator const T &() const { return *ptr_; }

  iterator begin() { return iterator(data()); }
  const_iterator begin() const { return const_iterator(data()); }
  iterator end() { return iterator(data() + 1); }
  const_iterator end() const { return const_iterator(data() + 1); }

private:
  T *ptr_;
};
