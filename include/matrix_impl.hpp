#include "matrix_slice.hpp"

namespace tensor {

namespace impl {

    template <std::size_t N, typename... Dims>
    bool check_bounds(const TensorSlice<N>& ms, Dims... dims)
    {
        std::size_t indexes[N] { std::size_t(dims)... };
        return std::equal(indexes, indexes + N, ms.extents.begin(),
            std::less<std::size_t> {});
    }

}
}
