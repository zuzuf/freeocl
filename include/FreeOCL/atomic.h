/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_ATOMIC_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_ATOMIC_H__

#include "FreeOCL/config.h"

// Built-in atomic functions
// 32-bit atomics
inline __int atomic_add(volatile __int *p, __int val)
{	return __sync_fetch_and_add(p, val);	}
inline __uint atomic_add(volatile __uint *p, __uint val)
{	return __sync_fetch_and_add(p, val);	}

inline __int atomic_sub(volatile __int *p, __int val)
{	return __sync_fetch_and_sub(p, val);	}
inline __uint atomic_sub(volatile __uint *p, __uint val)
{	return __sync_fetch_and_sub(p, val);	}

inline __int atomic_xchg(volatile __int *p, __int val)
{	return __sync_lock_test_and_set(p, val);	}
inline __uint atomic_xchg(volatile __uint *p, __uint val)
{	return __sync_lock_test_and_set(p, val);	}

inline __int atomic_inc(volatile __int *p)
{	return atomic_add(p, 1);	}
inline __uint atomic_inc(volatile __uint *p)
{	return atomic_add(p, 1U);	}

inline __int atomic_dec(volatile __int *p)
{	return atomic_sub(p, 1);	}
inline __uint atomic_dec(volatile __uint *p)
{	return atomic_sub(p, 1U);	}

inline __int atomic_cmpxchg(volatile __int *p, __int cmp, __int val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}
inline __uint atomic_cmpxchg(volatile __uint *p, __uint cmp, __uint val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}

inline __int atomic_min(volatile __int *p, __int val)
{
	__int old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}
inline __uint atomic_min(volatile __uint *p, __uint val)
{
	__uint old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}

inline __int atomic_max(volatile __int *p, __int val)
{
	__int old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}
inline __uint atomic_max(volatile __uint *p, __uint val)
{
	__uint old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}

inline __int atomic_and(volatile __int *p, __int val)
{	return __sync_fetch_and_and(p, val);	}
inline __uint atomic_and(volatile __uint *p, __uint val)
{	return __sync_fetch_and_and(p, val);	}

inline __int atomic_or(volatile __int *p, __int val)
{	return __sync_fetch_and_or(p, val);	}
inline __uint atomic_or(volatile __uint *p, __uint val)
{	return __sync_fetch_and_or(p, val);	}

inline __int atomic_xor(volatile __int *p, __int val)
{	return __sync_fetch_and_xor(p, val);	}
inline __uint atomic_xor(volatile __uint *p, __uint val)
{	return __sync_fetch_and_xor(p, val);	}

// 64-bit atomics
inline __long atomic_add(volatile __long *p, __long val)
{	return __sync_fetch_and_add(p, val);	}
inline __ulong atomic_add(volatile __ulong *p, __ulong val)
{	return __sync_fetch_and_add(p, val);	}

inline __long atomic_sub(volatile __long *p, __long val)
{	return __sync_fetch_and_sub(p, val);	}
inline ulong atomic_sub(volatile __ulong *p, __ulong val)
{	return __sync_fetch_and_sub(p, val);	}

inline __long atomic_xchg(volatile __long *p, __long val)
{	return __sync_lock_test_and_set(p, val);	}
inline __ulong atomic_xchg(volatile __ulong *p, __ulong val)
{	return __sync_lock_test_and_set(p, val);	}

inline __long atomic_inc(volatile __long *p)
{	return atomic_add(p, 1);	}
inline __ulong atomic_inc(volatile __ulong *p)
{	return atomic_add(p, 1U);	}

inline __long atomic_dec(volatile __long *p)
{	return atomic_sub(p, 1);	}
inline __ulong atomic_dec(volatile __ulong *p)
{	return atomic_sub(p, 1U);	}

inline __long atomic_cmpxchg(volatile __long *p, __long cmp, __long val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}
inline __ulong atomic_cmpxchg(volatile __ulong *p, __ulong cmp, __ulong val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}

inline __long atomic_min(volatile __long *p, __long val)
{
	__long old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}
inline __ulong atomic_min(volatile __ulong *p, __ulong val)
{
	__ulong old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}

inline __long atomic_max(volatile __long *p, __long val)
{
	__long old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}
inline __ulong atomic_max(volatile __ulong *p, __ulong val)
{
	__ulong old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}

inline __long atomic_and(volatile __long *p, __long val)
{	return __sync_fetch_and_and(p, val);	}
inline __ulong atomic_and(volatile __ulong *p, __ulong val)
{	return __sync_fetch_and_and(p, val);	}

inline __long atomic_or(volatile __long *p, __long val)
{	return __sync_fetch_and_or(p, val);	}
inline __ulong atomic_or(volatile __ulong *p, __ulong val)
{	return __sync_fetch_and_or(p, val);	}

inline __long atomic_xor(volatile __long *p, __long val)
{	return __sync_fetch_and_xor(p, val);	}
inline __ulong atomic_xor(volatile __ulong *p, __ulong val)
{	return __sync_fetch_and_xor(p, val);	}

// atom_* versions (which use non-volatile pointers)
// 32-bit atomics
inline int atom_add(int *p, int val)
{	return __sync_fetch_and_add(p, val);	}
inline uint atom_add(uint *p, uint val)
{	return __sync_fetch_and_add(p, val);	}

inline int atom_sub(int *p, int val)
{	return __sync_fetch_and_sub(p, val);	}
inline uint atom_sub(uint *p, uint val)
{	return __sync_fetch_and_sub(p, val);	}

inline int atom_xchg(int *p, int val)
{	return __sync_lock_test_and_set(p, val);	}
inline uint atom_xchg(uint *p, uint val)
{	return __sync_lock_test_and_set(p, val);	}

inline int atom_inc(int *p)
{	return atom_add(p, 1);	}
inline uint atom_inc(uint *p)
{	return atom_add(p, 1U);	}

inline int atom_dec(int *p)
{	return atom_sub(p, 1);	}
inline uint atom_dec(uint *p)
{	return atom_sub(p, 1U);	}

inline int atom_cmpxchg(int *p, int cmp, int val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}
inline uint atom_cmpxchg(uint *p, uint cmp, uint val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}

inline int atom_min(int *p, int val)
{
	int old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}
inline uint atom_min(uint *p, uint val)
{
	uint old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}

inline int atom_max(int *p, int val)
{
	int old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}
inline uint atom_max(uint *p, uint val)
{
	uint old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}

inline int atom_and(int *p, int val)
{	return __sync_fetch_and_and(p, val);	}
inline uint atom_and(uint *p, uint val)
{	return __sync_fetch_and_and(p, val);	}

inline int atom_or(int *p, int val)
{	return __sync_fetch_and_or(p, val);	}
inline uint atom_or(uint *p, uint val)
{	return __sync_fetch_and_or(p, val);	}

inline int atom_xor(int *p, int val)
{	return __sync_fetch_and_xor(p, val);	}
inline uint atom_xor(uint *p, uint val)
{	return __sync_fetch_and_xor(p, val);	}

// 64-bit atomics
inline __long atom_add(__long *p, __long val)
{	return __sync_fetch_and_add(p, val);	}
inline __ulong atom_add(__ulong *p, __ulong val)
{	return __sync_fetch_and_add(p, val);	}

inline __long atom_sub(__long *p, __long val)
{	return __sync_fetch_and_sub(p, val);	}
inline __ulong atom_sub(__ulong *p, __ulong val)
{	return __sync_fetch_and_sub(p, val);	}

inline __long atom_xchg(__long *p, __long val)
{	return __sync_lock_test_and_set(p, val);	}
inline __ulong atom_xchg(__ulong *p, __ulong val)
{	return __sync_lock_test_and_set(p, val);	}

inline __long atom_inc(__long *p)
{	return atom_add(p, 1);	}
inline __ulong atom_inc(__ulong *p)
{	return atom_add(p, 1U);	}

inline __long atom_dec(__long *p)
{	return atom_sub(p, 1);	}
inline __ulong atom_dec(__ulong *p)
{	return atom_sub(p, 1U);	}

inline __long atom_cmpxchg(__long *p, __long cmp, __long val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}
inline __ulong atom_cmpxchg(__ulong *p, __ulong cmp, __ulong val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}

inline __long atom_min(__long *p, __long val)
{
	__long old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}
inline __ulong atom_min(__ulong *p, __ulong val)
{
	__ulong old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}

inline __long atom_max(__long *p, __long val)
{
	__long old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}
inline __ulong atom_max(__ulong *p, __ulong val)
{
	__ulong old;
#ifdef FREEOCL_USE_OPENMP
#pragma omp critical
#endif
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}

inline __long atom_and(__long *p, __long val)
{	return __sync_fetch_and_and(p, val);	}
inline __ulong atom_and(__ulong *p, __ulong val)
{	return __sync_fetch_and_and(p, val);	}

inline __long atom_or(__long *p, __long val)
{	return __sync_fetch_and_or(p, val);	}
inline __ulong atom_or(__ulong *p, __ulong val)
{	return __sync_fetch_and_or(p, val);	}

inline __long atom_xor(__long *p, __long val)
{	return __sync_fetch_and_xor(p, val);	}
inline __ulong atom_xor(__ulong *p, __ulong val)
{	return __sync_fetch_and_xor(p, val);	}

#endif
