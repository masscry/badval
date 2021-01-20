## Bad Value Header-Only Library

This C++ library defines `bvl::value_t` variant-like class.
Class created to practice programming in C++14.

There are three possible types to be boxed in `bvl::value_t`:

 * number - 64-bit floating point numbers
 * string - std::string allocated on heap
 * pointer - plain c-pointer with optional cleanup function

### Requirements

 * cmake 3.10
 * C++ compiler with C++14 support (Compilation tested on gcc 9.3.0 and clang 10.0.0)

### How to build

Library project ships with [Visual Studio Code](https://code.visualstudio.com/) project [directory](https://github.com/masscry/badval/blob/master/.vscode) and can be opened in that editor.

Or it can be built using following commands:

```shell
cmake ./
cmake --build ./
```

### Testing

Library is provided with project `badtest` to test and show library usage.


### Links

[LICENSE.md](https://github.com/masscry/badval/blob/master/LICENSE.md)
[badval.hpp](https://github.com/masscry/badval/blob/master/include/badval.hpp)
[badtest.cpp](https://github.com/masscry/badval/blob/master/test/badtest.cpp)

### Contact 

If you'd like to contact me on subject of this project, please create GitHub ussue. Thank you!
