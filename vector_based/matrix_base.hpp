#pragma once

#include "matrix_slice.hpp"
#include <cstddef>
#include <iostream>
#include <vector>

// Common base class for public Matrix and MatrixRef
template <typename T, size_t N>
class MatrixBase {
public:
    static constexpr size_t order_ = N;
    using value_type = T;

    MatrixBase(MatrixBase&&) = default;
    MatrixBase& operator=(MatrixBase&&) = default;
    MatrixBase(const MatrixBase&) = default;
    MatrixBase& operator=(const MatrixBase&) = default;
    ~MatrixBase() = default;

    //! specify the extents
    template <typename... Exts>
    explicit MatrixBase(Exts... exts)
        : desc_ { exts... }
    {
    }

    explicit MatrixBase(const MatrixSlice<N>& ms)
        : desc_ { ms }
    {
    }

    //! number of dimensions
    static constexpr std::size_t order() { return order_; }

    //! #elements in the nth dimension
    std::size_t extent(std::size_t n) const
    {
        assert(n < order_);
        return desc_.extents[n];
    }

    //! total number of elements
    virtual std::size_t size() const = 0;

    //! the slice defining subscripting
    const MatrixSlice<N>& descriptor() const { return desc_; }

    //! "flat" element access
    virtual T* data() = 0;
    virtual const T* data() const = 0;

    std::size_t n_rows() const { return desc_.extents[0]; }
    std::size_t n_cols() const { return desc_.extents[1]; }

    // m(i,j,k) subscripting with integers
    template <typename... Args>
    Enable_if<matrix_impl::Requesting_element<Args...>(), T&>
    operator()(Args... args)
    {
        assert(matrix_impl::check_bounds(this->desc_, args...));
        return *(data() + this->desc_(args...));
    }

    template <typename... Args>
    Enable_if<matrix_impl::Requesting_element<Args...>(), const T&>
    operator()(Args... args) const
    {
        assert(matrix_impl::check_bounds(this->desc_, args...));
        return *(data() + this->desc_(args...));
    }

protected:
    MatrixSlice<N> desc_; // slice defining extents in the N dimensions
};

template <typename M>
Enable_if<Matrix_type<M>(), std::ostream&> operator<<(std::ostream& os,
    const M& m)
{
    os << '{';
    for (std::size_t i = 0; i != m.n_rows(); ++i) {
        os << m[i];
        if (i + 1 != m.n_rows())
            os << ',';
    }
    return os << '}';
}
