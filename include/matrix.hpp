

#include "matrix_impl.hpp"
#include "matrix_ref.hpp"
#include "matrix_slice.hpp"
#include "slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T, size_t Nrows, size_t Ncols> class Matrix {
public:
    static constexpr size_t size() { return Nrows * Ncols; }
    static constexpr size_t n_rows() { return Nrows; }
    static constexpr size_t n_cols() { return Ncols; }

    using value_type = T;
    using iterator = typename std::array<T, size()>::iterator;
    using const_iterator = typename std::array<T, size()>::const_iterator;

    ////// Constructors
    Matrix() = default;
    Matrix(Matrix&&) = default; // move
    Matrix& operator=(Matrix&&) = default;
    Matrix(const Matrix&) = default; // copy
    Matrix& operator=(const Matrix&) = default;
    ~Matrix() = default;

    Matrix(impl::MatrixInitializer<T> list) { setFromInitList(list); }

    Matrix& operator=(impl::MatrixInitializer<T> list)
    {
        setFromInitList(list);
        return *this;
    };

    template <size_t Nrows2, size_t Ncols2> Matrix(MatrixRef<T> mr)
    {
        assert(Nrows == mr.rows() && Ncols == mr.cols());
        setFromMatrixRef(mr);
    }

    template <size_t Nrows2, size_t Ncols2> Matrix& operator=(MatrixRef<T> mr)
    {
        assert(Nrows == mr.rows() && Ncols == mr.cols());
        setFromMatrixRef(mr);
    }

    ////// Accessors
    T* data() { return elems.data(); }
    const T* data() const { return elems.data(); }

    T* begin() { return elems.begin(); }
    const T* begin() const { return elems.begin(); }
    T* end() { return elems.end(); }
    const T* end() const { return elems.end(); }

    T& operator()(size_t i, size_t j)
    {
        assert(i < Nrows && j < Ncols);
        return elems[i * Ncols + j];
    }

    const T& operator()(size_t i, size_t j) const
    {
        assert(i < Nrows && j < Ncols);
        return elems[i * Ncols + j];
    }

    // subscripting with slices
    MatrixRef<T> operator()(slice rows, slice cols)
    {
        assert(rows.start < Nrows && rows.end <= Nrows && rows.start < rows.end);
        assert(cols.start < Ncols && cols.end <= Ncols && cols.start < cols.end);

        MatrixSlice sl { { Nrows, Ncols }, rows, cols };
        return { data(), sl };
    }

    MatrixRef<T> operator()(size_t row, slice cols)
    {
        assert(row < Nrows);
        assert(cols.start < Ncols && cols.end <= Ncols && cols.start < cols.end);

        MatrixSlice sl { { Nrows, Ncols }, slice(row, row + 1), cols };
        return { data(), sl };
    }

    MatrixRef<T> operator()(slice rows, size_t col)
    {
        assert(rows.start < Nrows && rows.end <= Nrows && rows.start < rows.end);
        assert(col < Ncols);

        MatrixSlice sl { { Nrows, Ncols }, rows, slice(col, col + 1) };
        return { data(), sl };
    }

    MatrixRef<T> operator[](size_t row_index) { return row(row_index); }

    MatrixRef<T> row(size_t row_index) { return this->operator()(row_index, slice(0, Ncols)); }

    MatrixRef<T> col(size_t col_index) { return this->operator()(slice(0, Nrows), col_index); }

    //////////// Operations
    template <typename F> Matrix<T, Nrows, Ncols>& apply(F f)
    {
        for (auto& elem : elems) {
            f(elem);
        }
        return *this;
    }

    template <typename F, typename T2>
    template_utils::Enable_if<template_utils::Convertible<T2, T>(), Matrix<T, Nrows, Ncols>&>
    apply(const Matrix<T2, Nrows, Ncols>& m, F f)
    {
        for (auto i = begin(), j = m.begin(); i != end(); ++i, ++j) {
            f(*i, *j);
        }
        return *this;
    }

    template <typename F, typename T2>
    template_utils::Enable_if<template_utils::Convertible<T2, T>(), Matrix<T, Nrows, Ncols>&>
    apply(const MatrixRef<T2>& mr, F f)
    {
        assert(Nrows == mr.n_rows() && Ncols == mr.n_cols());
        size_t i = 0;
        for (size_t row = 0; row < Nrows; ++row) {
            for (size_t col = 0; col < Ncols; ++col) {
                f(elems[i++], mr(row, col));
            }
        }

        return *this;
    }

    bool operator==(const Matrix& rhs) const { return std::equal(elems.begin(), elems.end(), rhs.elems.begin()); }

    bool operator==(const MatrixRef<T>& rhs) const
    {
        if (Nrows != rhs.n_rows() || Ncols != rhs.n_cols()) {
            return false;
        }
        size_t i = 0;
        for (size_t row = 0; row < Nrows; ++row) {
            for (size_t col = 0; col < Ncols; ++col) {
                if (elems[i++] != rhs(row, col)) {
                    return false;
                }
            }
        }
        return true;
    }

    template <typename M>
    template_utils::Enable_if<template_utils::Has_matrix_type<M>(), Matrix<T, Nrows, Ncols>&> operator+=(const M& m)
    {
        using T2 = typename M::value_type;
        return apply(m, [](T& a, const T2& b) { a += b; });
    }

    template <typename T2>
    template_utils::Enable_if<template_utils::Convertible<T, T2>(), Matrix<T, Nrows, Ncols>&>
    operator+=(const T2& scalar)
    {
        return apply([&scalar](T& a) { a += scalar; });
    }

    template <typename M>
    template_utils::Enable_if<template_utils::Has_matrix_type<M>(), Matrix<T, Nrows, Ncols>&> operator-=(const M& m)
    {
        using T2 = typename M::value_type;
        return apply(m, [](T& a, const T2& b) { a -= b; });
    }

    template <typename T2>
    template_utils::Enable_if<template_utils::Convertible<T, T2>(), Matrix<T, Nrows, Ncols>&>
    operator-=(const T2& scalar)
    {
        return apply([&scalar](T& a) { a -= scalar; });
    }

    template <typename M>
    template_utils::Enable_if<template_utils::Has_matrix_type<M>(), Matrix<T, Nrows, Ncols>&> operator*=(const M& m)
    {
        using T2 = typename M::value_type;
        return apply(m, [](T& a, const T2& b) { a *= b; });
    }

    template <typename T2>
    template_utils::Enable_if<template_utils::Convertible<T, T2>(), Matrix<T, Nrows, Ncols>&>
    operator*=(const T2& scalar)
    {
        return apply([&scalar](T& a) { a *= scalar; });
    }

    template <typename M>
    template_utils::Enable_if<template_utils::Has_matrix_type<M>(), Matrix<T, Nrows, Ncols>&> operator/=(const M& m)
    {
        using T2 = typename M::value_type;
        return apply(m, [](T& a, const T2& b) { a /= b; });
    }

    template <typename T2>
    template_utils::Enable_if<template_utils::Convertible<T, T2>(), Matrix<T, Nrows, Ncols>&>
    operator/=(const T2& scalar)
    {
        return apply([&scalar](T& a) { a /= scalar; });
    }

private:
    std::array<T, size()> elems;

    void setFromInitList(impl::MatrixInitializer<T> list)
    {
        size_t i = 0;
        // static_assert(list.size() == Nrows, "wrong number of rows");
        assert(list.size() == Nrows);
        for (const auto& row : list) {
            // static_assert(row.size() == Ncols, "wrong number of columns");
            assert(row.size() == Ncols);
            for (const auto& elem : row) {
                elems[i++] = elem;
            }
        }
    }

    template <size_t Nrows2, size_t Ncols2> void setFromMatrixRef(MatrixRef<T> mr)
    {
        size_t i = 0;
        for (size_t r = 0; r < mr.rows(); ++r) {
            for (size_t c = 0; c < mr.cols(); ++c) {
                elems[i++] = mr(r, c);
            }
        }
    }
};

}
