

#include "matrix_base.hpp"
#include <array>
#include <cstddef>

namespace tensor {

template <typename T, size_t... Ns>
class Tensor : public TensorBase<T, Ns...> {
public:
    using iterator = typename std::array<T, 100>::iterator;
};

}
