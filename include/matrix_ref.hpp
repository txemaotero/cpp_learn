

#include "matrix_base.hpp"
#include "matrix_impl.hpp"
#include <array>
#include <cstddef>

namespace matrix {

template <typename T, size_t Nrows, size_t Ncols>
class MatrixRef : public MatrixBase<T, Nrows, Ncols> {
public:
    using MatrixBase<T, Nrows, Ncols>::size;

    using iterator = typename std::array<T, size>::iterator;

    MatrixRef() = default;
    MatrixRef(MatrixRef&&) = default; // move
    MatrixRef& operator=(MatrixRef&&) = default;
    MatrixRef(const MatrixRef&) = default; // copy
    MatrixRef& operator=(const MatrixRef&) = default;
    ~MatrixRef() = default;

    T* data() override { return elems_; }
    const T* data() const override { return elems_; }

    using MatrixBase<T, Nrows, Ncols>::operator();

    // subscripting with slices

private:
    T* elems_ { nullptr };
};

}
