#pragma once

#include "matrix_base.hpp"
#include "matrix_fwd.hpp"
#include "matrix_impl.hpp"
#include "matrix_ref.hpp"
#include "matrix_slice.hpp"

template <typename T, std::size_t N>
class Matrix : public MatrixBase<T, N> {
public:
    //! @cond Doxygen_Suppress
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    Matrix() = default;
    Matrix(Matrix&&) = default; // move
    Matrix& operator=(Matrix&&) = default;
    Matrix(const Matrix&) = default; // copy
    Matrix& operator=(const Matrix&) = default;
    ~Matrix() = default;

    //! construct from Matrix
    template <typename M, typename = Enable_if<Matrix_type<M>()>>
    Matrix(const M& x)
        : MatrixBase<T, N>(x.descriptor())
        , elems_(x.begin(), x.end())
    {
        static_assert(Convertible<typename M::value_type, T>(), "");
    }
    //! assign from Matrix
    template <typename M, typename = Enable_if<Matrix_type<M>()>>
    Matrix& operator=(const M& x)
    {
        static_assert(Convertible<typename M::value_type, T>(), "");

        this->desc_ = x.descriptor();
        elems_.assign(x.begin(), x.end());
        return *this;
    }

    //! construct from MatrixRef
    template <typename U>
    Matrix(const MatrixRef<U, N>& x)
        : MatrixBase<T, N> { x.descriptor().extents }
        , elems_ { x.begin(), x.end() }
    {
        static_assert(Convertible<U, T>(),
            "Matrix constructor: incompatible element types");
    }
    //! assign from MatrixRef
    template <typename U>
    Matrix& operator=(const MatrixRef<U, N>& x)
    {
        static_assert(Convertible<U, T>(), "Matrix =: incompatible element types");

        this->desc_ = x.descriptor();
        this->desc_.start = 0;
        elems_.assign(x.begin(), x.end());
        return *this;
    }

    //! specify the extents
    template <typename... Exts>
    explicit Matrix(Exts... exts)
        : MatrixBase<T, N> { exts... }
        , // copy extents
        elems_(this->desc_.size) // allocate desc_.size elements and initialize
    {
    }

    //! initialize from list
    Matrix(MatrixInitializer<T, N> init)
    {
        // intialize start
        this->desc_.start = 0;
        // deduce extents from initializer list
        this->desc_.extents = matrix_impl::derive_extents<N>(init);
        // compute strides and size
        this->desc_.size = matrix_impl::compute_strides(this->desc_.extents, this->desc_.strides);

        elems_.reserve(this->desc_.size); // make room for slices
        matrix_impl::insert_flat(init, elems_); // initialize from initializer list
        assert(elems_.size() == this->desc_.size);
    };

    //! assign from list
    Matrix& operator=(MatrixInitializer<T, N> init)
    {
        elems_.clear();

        // intialize start
        this->desc_.start = 0;
        // deduce extents from initializer list
        this->desc_.extents = matrix_impl::derive_extents<N>(init);
        // compute strides and size
        this->desc_.size = matrix_impl::compute_strides(this->desc_.extents, this->desc_.strides);

        elems_.reserve(this->desc_.size); // make room for slices
        matrix_impl::insert_flat(init, elems_); // initialize from initializer list
        assert(elems_.size() == this->desc_.size);

        return *this;
    }

    //! don't use {} except for elements
    ///@{
    template <typename U, std::size_t NN = N,
        typename = Enable_if<Convertible<U, std::size_t>()>,
        typename = Enable_if<(NN > 1)>>
    Matrix(std::initializer_list<U>) = delete;
    template <typename U, std::size_t NN = N,
        typename = Enable_if<Convertible<U, std::size_t>()>,
        typename = Enable_if<(NN > 1)>>
    Matrix& operator=(std::initializer_list<U>) = delete;
    ///@}

    //! total number of elements
    std::size_t size() const { return elems_.size(); }

    //! "flat" element access
    ///@{
    T* data() { return elems_.data(); }
    const T* data() const { return elems_.data(); }
    ///@}

private:
    std::vector<T> elems_; // the elements

public:
    //! m(i,j,k) subscripting with integers
    using MatrixBase<T, N>::operator();

    //! m(s1, s2, s3) subscripting with slides
    template <typename... Args>
    Enable_if<matrix_impl::Requesting_slice<Args...>(), MatrixRef<T, N>>
    operator()(const Args&... args)
    {
        MatrixSlice<N> d;
        d.start = matrix_impl::do_slice(this->desc_, d, args...);
        d.size = matrix_impl::compute_size(d.extents);
        return { d, data() };
    };

    template <typename... Args>
    Enable_if<matrix_impl::Requesting_slice<Args...>(), MatrixRef<const T, N>>
    operator()(const Args&... args) const
    {
        MatrixSlice<N> d;
        d.start = matrix_impl::do_slice(this->desc_, d, args...);
        d.size = matrix_impl::compute_size(d.extents);
        return { d, data() };
    };

    //! m[i] row access
    ///@{
    MatrixRef<T, N - 1> operator[](std::size_t i) { return row(i); }
    MatrixRef<const T, N - 1> operator[](std::size_t i) const { return row(i); }
    ///@}

    //! row access
    MatrixRef<T, N - 1> row(std::size_t n)
    {
        assert(n < this->n_rows());
        MatrixSlice<N - 1> row;
        matrix_impl::slice_dim<0>(n, this->desc_, row);
        return { row, data() };
    };

    MatrixRef<const T, N - 1> row(std::size_t n) const
    {
        assert(n < this->n_rows());
        MatrixSlice<N - 1> row;
        matrix_impl::slice_dim<0>(n, this->desc_, row);
        return { row, data() };
    };

    //! column access
    MatrixRef<T, N - 1> col(std::size_t n)
    {
        assert(n < this->n_cols());
        MatrixSlice<N - 1> col;
        matrix_impl::slice_dim<1>(n, this->desc_, col);
        return { col, data() };
    };

    MatrixRef<const T, N - 1> col(std::size_t n) const
    {
        assert(n < this->n_cols());
        MatrixSlice<N - 1> col;
        matrix_impl::slice_dim<1>(n, this->desc_, col);
        return { col, data() };
    };

    //! multiple rows access
    MatrixRef<T, N> rows(std::size_t i, std::size_t j)
    {
        assert(i <= j);
        assert(j < this->n_rows());

        MatrixSlice<N> d;
        d.start = this->desc_.start;
        d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice { i, j - i + 1 });
        std::size_t NRest = N - 1;
        while (NRest >= 1) {
            d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice { 0 }, NRest);
            --NRest;
        }
        return { d, data() };
    };

    MatrixRef<const T, N> rows(std::size_t i, std::size_t j) const
    {
        assert(i <= j);
        assert(j < this->n_rows());

        MatrixSlice<N> d;
        d.start = this->desc_.start;
        d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice { i, j - i + 1 });
        std::size_t NRest = N - 1;
        while (NRest >= 1) {
            d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice { 0 }, NRest);
            --NRest;
        }
        return { d, data() };
    };

    //! multiple columns access
    MatrixRef<T, N> cols(std::size_t i, std::size_t j)
    {
        assert(N >= 2);
        assert(i <= j);
        assert(j < this->n_cols());

        MatrixSlice<N> d;
        d.start = this->desc_.start;
        d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice { 0 });
        d.start += matrix_impl::do_slice_dim<N - 1>(this->desc_, d, slice { i, j - i + 1 });

        std::size_t NRest = N - 2;
        while (NRest >= 1) {
            d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice { 0 }, NRest);
            --NRest;
        }
        return { d, data() };
    };

    MatrixRef<const T, N> cols(std::size_t i, std::size_t j) const
    {
        assert(N >= 2);
        assert(i <= j);
        assert(j < this->n_cols());

        MatrixSlice<N> d;
        d.start = this->desc_.start;
        d.start += matrix_impl::do_slice_dim<N>(this->desc_, d, slice { 0 });
        d.start += matrix_impl::do_slice_dim<N - 1>(this->desc_, d, slice { i, j - i + 1 });

        std::size_t NRest = N - 2;
        while (NRest >= 1) {
            d.start += matrix_impl::do_slice_dim2(this->desc_, d, slice { 0 }, NRest);
            --NRest;
        }
        return { d, data() };
    };

    //! element iterators
    iterator begin() { return elems_.begin(); }
    const_iterator begin() const { return elems_.cbegin(); }
    iterator end() { return elems_.end(); }
    const_iterator end() const { return elems_.cend(); }

public:
    //! matrix arithmetic operations
    ///@{
    template <typename F>
    Matrix& apply(F f)
    {
        for (auto& x : elems_)
            f(x); // this loop uses stride iterators
        return *this;
    };

    // f(x, mx) for corresponding elements of *this and m
    template <typename M, typename F>
    Enable_if<Matrix_type<M>(), Matrix&> apply(const M& m, F f)
    {
        assert(same_extents(this->desc_, m.descriptor()));
        auto j = m.begin();
        for (auto i = begin(); i != end(); ++i) {
            f(*i, *j);
            ++j;
        }

        return *this;
    };
};

// Specialization for 0-dimensional matrix
template <typename T>
class Matrix<T, 0> : public MatrixBase<T, 0> {
public:
    //! @cond Doxygen_Suppress
    using iterator = typename std::array<T, 1>::iterator;
    using const_iterator = typename std::array<T, 1>::const_iterator;
    //! @endcond

    //! construct from an element
    Matrix(const T& x = T {})
        : elem_ { x }
    {
    }
    //! assign from an element
    Matrix& operator=(const T& value)
    {
        elem_[0] = value;
        return *this;
    }

    //! total number of elements
    std::size_t size() const { return 1; }

    //! "flat" element access
    ///@{
    T* data() { return elem_.data(); }
    const T* data() const { return elem_.data(); }
    ///@}

    //! m() subscripting
    ///@{
    T& operator()() { return elem_[0]; }
    const T& operator()() const { return elem_[0]; }
    ///@}

    //! conversion from Matrix<T, 0> to type T
    ///@{
    operator T&() { return elem_[0]; }
    operator const T&() { return elem_[0]; }
    ///@}

    //! element iterators
    ///@{
    iterator begin() { return elem_.begin(); }
    const_iterator begin() const { return elem_.cbegin(); }
    iterator end() { return elem_.end(); }
    const_iterator end() const { return elem_.end(); }
    ///@}

private:
    std::array<T, 1> elem_;
};
