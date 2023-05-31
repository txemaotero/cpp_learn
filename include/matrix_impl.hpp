#include "matrix_slice.hpp"

namespace matrix {
namespace impl {

    template <typename T>
    using MatrixInitializer = typename std::initializer_list<std::initializer_list<T>>;

}
}
