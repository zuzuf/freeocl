FreeOCL behavior can be changed using environment variables.

# Environment variables #

## C++ compiler ##

### FREEOCL\_CXX\_COMPILER ###

FreeOCL builds OpenCL kernels using a C++ compiler. By default FreeOCL will use the compiler it has been built with, but you can override this with the **FREEOCL\_CXX\_COMPILER** environment variable.

### FREEOCL\_CXX\_FLAGS ###
Depending on your compiler you may have to change compiler flags too. This can be done with the **FREEOCL\_CXX\_FLAGS** environment variable. Beware the compiler must be told to build a shared library and to explicitly parse C++ code. The default flags for GCC are:

_-shared -fPIC -pipe -g -O3 -fomit-frame-pointer -ftree-vectorize -funroll-loops -fno-math-errno -fno-trapping-math -rdynamic -fno-exceptions -fno-rtti -x c++ -Wall -Wextra_ (as of [r384](https://code.google.com/p/freeocl/source/detail?r=384))

### FREEOCL\_CXX\_INCLUDE ###

By default, FreeOCL will look for its headers in the location where it was installed. This can be overridden by setting **FREEOCL\_CXX\_INCLUDE** to the location of the _FreeOCL_ include folder. For example, if you wish to use the FreeOCL headers at _/path/to/alternate/include/FreeOCL/`*`.h_, you would set **FREEOCL\_CXX\_INCLUDE** to _/path/to/alternate_.

### FREEOCL\_DEBUG ###

FreeOCL will delete the temporary source and binary files generated, unless there was an error. These files can be retained in all cases by setting **FREEOCL\_DEBUG** to any non-zero value.