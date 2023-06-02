

#include "matrix_impl.hpp"
#include "matrix_slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T> class MatrixRef {
public:
    MatrixRef() = delete;
    MatrixRef(MatrixRef&&) = default; // move
    MatrixRef& operator=(MatrixRef&&) = default;
    MatrixRef(const MatrixRef&) = default; // copy
    MatrixRef& operator=(const MatrixRef&) = default;
    ~MatrixRef() = default;

    MatrixRef(T* elems_, const MatrixSlice& ms)
        : elems { elems_ }
        , extents { ms }
    {
    }

    // m(i,j,k) subscripting with integers
    T& operator()(size_t i, size_t j)
    {
        assert(i < extents.rows() && j < extents.cols());
        return *(elems + extents(i, j));
    }

    const T& operator()(size_t i, size_t j) const
    {
        assert(i < extents.rows() && j < extents.cols());
        return *(elems + extents(i, j));
    }

    // subscripting with slices
    MatrixRef<T> operator()(slice rows, slice cols)
    {
        assert(rows.start < extents.rows() && rows.end <= extents.rows() && rows.start < rows.end);
        assert(cols.start < extents.cols() && cols.end <= extents.cols() && cols.start < cols.end);

        return { elems, extents(rows, cols) };
    }

    MatrixRef<T> operator()(size_t row, slice cols)
    {
        assert(row < extents.rows());
        assert(cols.start < extents.cols() && cols.end <= extents.cols() && cols.start < cols.end);

        return { elems, extents({ row, row + 1 }, cols) };
    }

    MatrixRef<T> operator()(slice rows, size_t col)
    {
        assert(col < extents.cols());
        assert(rows.start < extents.rows() && rows.end <= extents.rows() && rows.start < rows.end);

        return { elems, extents(rows, { col, col + 1 }) };
    }

private:
    T* elems { nullptr };
    MatrixSlice extents;
};

} // namespace matrix
