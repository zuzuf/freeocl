# Introduction #

FreeOCL's goal is to provide a portable, full-featured and reliable implementation of OpenCL 1.2 specifications targeting CPU devices.
Performance is important but not the top priority.

# Details #

FreeOCL relies on an external C++ compiler to build kernels which means a platform is identifier by its OS, CPU and compiler. With C++11 compilers it can use some [C++11](http://en.wikipedia.org/wiki/C++11) features (hash containers instead of binary trees).

Primary goal is to be portable starting with the following major platforms:

  * Linux - x86, x86\_64, ARM, PPC64

Compilers that will be targeted:
  * GCC >= 4.4
  * ICC >= 11.0 (x86, x86\_64)
  * LLVM >= 2.8
  * armcc (to be checked)

In the (near?) future we would like to have the following platforms at least checked:
  * BSD family
  * Solaris/OpenIndiana