# Learning C++

After reading the _The C++ Programming Language_ by Bjarne Stroustrup I will try
to reproduce the Matrix template class presented in chapter 29 to deeply
understand the concepts and also to have a nice peace of code in my toolbox. The
original class presented in the book uses the std::vector under the hood to
store the data so in the first attempt I will reproduce exactly that. Once I
have it properly running I will try to implement a stack based approach using
std::array to store the date and pass the number of elements in each dimension
as template arguments.

I will also use some additional tools to familiarize with the C++ ecosystem such
as CMake to manage the project and the GTest framework to perform some tests.

## Path

After trying to generalize the idea of a compile time based tensor, where you
can specified the number of dimensions in each index, it turns out that this
problem is very hard (maybe not possible) in C++11. The main problem is related
with the access to the tensor components. Imagine you want to access to a
sub-view of the tensor in the second index, accessing to a given index. The
problem in that situation is that it is very hard to translate the variadic
template argumentos of the original tensor to those of the sub-view. To simplify
the problem I will implement the matrix class (only 2 index) and also the
vectors (row and column vectors) as a sub-instances of the matrix. For most of
the use cases this will cover all my necessities and it is still a very good
problem to learn new stuffs.


## Matrix<T, Nrows, Ncols>

The idea is know at compile time the dimensions of the matrix as well as the
type of the elements. The elements are internally stored in a `std::array<T,
Nrows*Ncols>`. All `Matrix` have a `MatrixSlice` member that is in charge of
translating a (i_row, j_col) pair of integers to the correct index of the
internal array. This may seem overkill for a simple matrix but it comes in handy
when accessing slices of a `Matrix`.

You can access to a slice of a `Matrix` by accessing its elements using the
`slice`  struct. The `MatrixRef` is very similar to the `Matrix` one but it
stores a pointer to the data in other `Matrix`. This means that the dimensions
of a `MatrixRef<Nrows, Ncols>` will not generally match with the dimensions of
the inner data pointer. However, the correct access is guaranteed due to the
inner `MatrixSlice` member.

You can always build a `Matrix` from a `MatrixRef` with different template
dimension to create a reduced `Matrix` from the reference.

## TODOs

- [X] Copy the implementation from the book using the vector as container.
  Achieve initialization and elemnt access.

- [ ] Implement the base matrix template class (with only two indexes)

