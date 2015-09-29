The main goal of FreeOCL is portability, not performance however it's always interesting to see how it compares to other implementations.



# Introduction #

This page compares FreeOCL to other OpenCL implementations when running various softwares.

# Benchmarks #

## Raytracing ##

### ratGPU ###
[ratGPU](http://www.ratgpu.com/) is a raytracer with a built-in benchmark. It allows to compare the performance of all installed OpenCL implementations. There is also a C++ renderer which can be used as a reference for CPU implementations.

The test machine is an Intel Core i7 2700K @ 4.2GHz with an Nvidia GTX 480 GPU running Kubuntu 12.04. Tests were ran with the following implementations:
  * the C++ reference implementation
  * Intel OpenCL 1.1 (Build 31360.31426)
  * FreeOCL/GCC ([r173](https://code.google.com/p/freeocl/source/detail?r=173))
  * FreeOCL/GCC-4.6.3 ([r262](https://code.google.com/p/freeocl/source/detail?r=262))
  * FreeOCL/Clang ([r224](https://code.google.com/p/freeocl/source/detail?r=224)) (Clang 3.2 trunk [r158419](https://code.google.com/p/freeocl/source/detail?r=158419))
  * FreeOCL/Clang ([r233](https://code.google.com/p/freeocl/source/detail?r=233)) (Clang 3.2 trunk [r159075](https://code.google.com/p/freeocl/source/detail?r=159075))
  * FreeOCL/Clang ([r262](https://code.google.com/p/freeocl/source/detail?r=262)) (Clang 3.1 trunk [r146587](https://code.google.com/p/freeocl/source/detail?r=146587))
  * Nvidia OpenCL 1.1 CUDA 4.2.1

The relative rendering times (lower is better) give the following chart:

![http://chart.apis.google.com/chart?chs=260x125&chd=t:5.010,26.38,26.59,26.61,27.80,29.499,29.794,43.732&cht=bvg&nonsense=something_that_ends_with.png](http://chart.apis.google.com/chart?chs=260x125&chd=t:5.010,26.38,26.59,26.61,27.80,29.499,29.794,43.732&cht=bvg&nonsense=something_that_ends_with.png)

From left to right: NVidia, FreeOCL/GCC-4.6.3 ([r262](https://code.google.com/p/freeocl/source/detail?r=262)), C++, FreeOCL/Clang ([r262](https://code.google.com/p/freeocl/source/detail?r=262)), FreeOCL/Clang ([r233](https://code.google.com/p/freeocl/source/detail?r=233)), FreeOCL/Clang ([r224](https://code.google.com/p/freeocl/source/detail?r=224)), Intel, FreeOCL/GCC. Not that bad without explicit kernel vectorization ;).

NB: the performance improvement from [r224](https://code.google.com/p/freeocl/source/detail?r=224) to [r233](https://code.google.com/p/freeocl/source/detail?r=233) is due to scheduling improvements (no built-ins optimizations, still no auto vectorization)
NB2: the performance improvement from [r233](https://code.google.com/p/freeocl/source/detail?r=233) to [r262](https://code.google.com/p/freeocl/source/detail?r=262) is mostly due to inlining/linkage optimizations (no built-ins optimizations, still no auto vectorization)

Snapshots of all the scores:

![http://freeocl.googlecode.com/svn/wiki/ratgpu_nvidia.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_nvidia.png)
![http://freeocl.googlecode.com/svn/wiki/ratgpu_cxx.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_cxx.png)
![http://freeocl.googlecode.com/svn/wiki/ratgpu_intel.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_intel.png)
![http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl.png)
![http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_clang.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_clang.png)
![http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_clang_r233.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_clang_r233.png)
![http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_clang_r262.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_clang_r262.png)
![http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_gcc463_r262.png](http://freeocl.googlecode.com/svn/wiki/ratgpu_freeocl_gcc463_r262.png)