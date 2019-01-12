Packed String Library
=====================

The library constists of C++17 files for working with packed string.
Currently, longest common prefix queries can be answered.

# Longest Common Prefix Query

Depending on the string length, a longest common prefix function is chosen that is geared towards
this string length. 
We use the technique of word packing for comparision.
This is either done by using the machine word size (64-bits), or vectorizing techniques of the SSE and AVX instruction sets.

The following graph shows an evalution made with naive character comparison (`character`) and word-packing in machine word size (`packed`),
or with the instruction sets `SSE2` and `AVX2`.
![LCP Evaluation](https://raw.githubusercontent.com/koeppl/packed_string/master/images/lcp.svg)
The evaluation was conducted on an Intel Core i7-8550U CPU.


# Dependencies

- Command line tools
  - cmake
  - make
  - a C++17 compiler like gcc or clang 
  - gtest (optional) for tests
  - celero (optional) for benchmarking

# Installation

This package ships as a library with a test program.
Invoke `cmake` and `make` to compile, `make test` to test the compilation.



# License

The framework is published under the
[Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0)

