

#include "matrix_base.hpp"
#include "matrix_impl.hpp"
#include "matrix_slice.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T, size_t Nrows, size_t Ncols>
class MatrixRef : public MatrixBase<T, Nrows, Ncols> {
public:
    using MatrixBase<T, Nrows, Ncols>::size;

    using iterator = typename std::array<T, size>::iterator;

    MatrixRef() = delete;
    MatrixRef(MatrixRef&&) = default; // move
    MatrixRef& operator=(MatrixRef&&) = default;
    MatrixRef(const MatrixRef&) = default; // copy
    MatrixRef& operator=(const MatrixRef&) = default;
    ~MatrixRef() = default;

    MatrixRef(T* elems_)
        : elems { elems_ }
    {
    }

    MatrixRef(T* elems_, const MatrixSlice& ms)
        : MatrixBase<T, Nrows, Ncols>(ms)
        , elems { elems_ }
    {
    }

    T* data() override { return elems; }
    const T* data() const override { return elems; }

    using MatrixBase<T, Nrows, Ncols>::operator();

    // subscripting with slices

private:
    T* elems { nullptr };
};

}
