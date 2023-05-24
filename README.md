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

## TODOs

- [ ] Copy the implementation from the book using the vector as container.
  Achieve initialization and elemnt access.
- [ ] Create the same structure but using std::array and full static memory
  allocation.
- [ ] Maybe create a constructor function that decide if the matrix can fit in
  the stack.


