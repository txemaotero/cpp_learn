

#include "matrix_base.hpp"
#include "matrix_impl.hpp"
#include "matrix_ref.hpp"
#include "slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T, size_t Nrows, size_t Ncols>
class Matrix : public MatrixBase<T, Nrows, Ncols> {
public:
    using MatrixBase<T, Nrows, Ncols>::size;

    using iterator = typename std::array<T, size>::iterator;

    Matrix() = default;
    Matrix(Matrix&&) = default; // move
    Matrix& operator=(Matrix&&) = default;
    Matrix(const Matrix&) = default; // copy
    Matrix& operator=(const Matrix&) = default;
    ~Matrix() = default;

    Matrix(impl::MatrixInitializer<T> list)
    {
        setFromInitList(list);
    }
    Matrix& operator=(impl::MatrixInitializer<T> list)
    {
        setFromInitList(list);
        return *this;
    };

    Matrix(MatrixRef<T, Nrows, Ncols> mr)
    {
        for (size_t i = 0; i < size; ++i) {
            elems_[i] = mr.data()[i];
        }
    }
    Matrix& operator=(MatrixRef<T, Nrows, Ncols> mr)
    {
        for (size_t i = 0; i < size; ++i) {
            elems_[i] = mr.data()[i];
        }
        return *this;
    }

    T* data() override { return elems_.data(); }
    const T* data() const override { return elems_.data(); }

    using MatrixBase<T, Nrows, Ncols>::operator();

    // subscripting with slices
    template <size_t RowStart, size_t RowEnd, size_t RowStride, size_t ColStart, size_t ColEnd, size_t CorStride>
    auto operator()(static_slice<RowStart, RowEnd, RowStride> rows, static_slice<ColStart, ColEnd, CorStride> cols) -> MatrixRef<T, rows.size(), cols.size()>
    {
        static_assert(RowStart < Nrows && RowEnd <= Nrows && RowStart < RowEnd, "invalid row slice");
        static_assert(ColStart < Ncols && ColEnd <= Ncols && ColStart < ColEnd, "invalid column slice");
        return MatrixRef<T, rows.size(), cols.size()>(rows, cols, *this);
    }

private:
    std::array<T, size> elems_;

    void setFromInitList(impl::MatrixInitializer<T> list)
    {
        size_t i = 0;
        static_assert(list.size() == Nrows, "wrong number of rows");
        for (const auto& row : list) {
            static_assert(row.size() == Ncols, "wrong number of columns");
            for (const auto& elem : row) {
                elems_[i++] = elem;
            }
        }
    }
};

}
