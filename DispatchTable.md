Description of the dispatch table structure.

# Introduction #

The dispatch table is a function table (very similar to a C++ vtable) which is a key component of the ICD driver interface. Each implementation specific object should begin with a pointer to this table which contains the list of all functions implemented by a driver. In order to work this requires the structure to be known in both driver (ie. **libFreeOCL.so**) and client (ie. **libOpenCL.so**) side.


# Details #

The dispatch table is made of chunks of functions ordered by API version. The end of the chunks is filled with stub pointers. It seems functions are listed in the same order they appear in **cl.h**.

The structure looks like:
  * functions in OpenCL 1.0 API
  * 16 stubs
  * functions in OpenCL 1.1 API which are not in 1.0 API
  * 4 stubs
  * functions in OpenCL 1.2 API which are not in 1.1 API
  * ...

**NB: this is known to be valid up to OpenCL 1.2.**