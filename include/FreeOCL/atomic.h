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
// Built-in atomic functions
// 32-bit atomics
inline int atomic_add(volatile int *p, int val)
{	return __sync_fetch_and_add(p, val);	}
inline uint atomic_add(volatile uint *p, uint val)
{	return __sync_fetch_and_add(p, val);	}

inline int atomic_sub(volatile int *p, int val)
{	return __sync_fetch_and_sub(p, val);	}
inline uint atomic_sub(volatile uint *p, uint val)
{	return __sync_fetch_and_sub(p, val);	}

inline int atomic_xchg(volatile int *p, int val)
{	return __sync_lock_test_and_set(p, val);	}
inline uint atomic_xchg(volatile uint *p, uint val)
{	return __sync_lock_test_and_set(p, val);	}

inline int atomic_inc(volatile int *p)
{	return atomic_add(p, 1);	}
inline uint atomic_inc(volatile uint *p)
{	return atomic_add(p, 1U);	}

inline int atomic_dec(volatile int *p)
{	return atomic_sub(p, 1);	}
inline uint atomic_dec(volatile uint *p)
{	return atomic_sub(p, 1U);	}

inline int atomic_cmpxchg(volatile int *p, int cmp, int val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}
inline uint atomic_cmpxchg(volatile uint *p, uint cmp, uint val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}

inline int atomic_min(volatile int *p, int val)
{
	int old;
#pragma omp critical
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}
inline uint atomic_min(volatile uint *p, uint val)
{
	uint old;
#pragma omp critical
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}

inline int atomic_max(volatile int *p, int val)
{
	int old;
#pragma omp critical
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}
inline uint atomic_max(volatile uint *p, uint val)
{
	uint old;
#pragma omp critical
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}

inline int atomic_and(volatile int *p, int val)
{	return __sync_fetch_and_and(p, val);	}
inline uint atomic_and(volatile uint *p, uint val)
{	return __sync_fetch_and_and(p, val);	}

inline int atomic_or(volatile int *p, int val)
{	return __sync_fetch_and_or(p, val);	}
inline uint atomic_or(volatile uint *p, uint val)
{	return __sync_fetch_and_or(p, val);	}

inline int atomic_xor(volatile int *p, int val)
{	return __sync_fetch_and_xor(p, val);	}
inline uint atomic_xor(volatile uint *p, uint val)
{	return __sync_fetch_and_xor(p, val);	}

// 64-bit atomics
inline long atomic_add(volatile long *p, int val)
{	return __sync_fetch_and_add(p, val);	}
inline ulong atomic_add(volatile ulong *p, ulong val)
{	return __sync_fetch_and_add(p, val);	}

inline long atomic_sub(volatile long *p, long val)
{	return __sync_fetch_and_sub(p, val);	}
inline ulong atomic_sub(volatile ulong *p, ulong val)
{	return __sync_fetch_and_sub(p, val);	}

inline long atomic_xchg(volatile long *p, long val)
{	return __sync_lock_test_and_set(p, val);	}
inline ulong atomic_xchg(volatile ulong *p, ulong val)
{	return __sync_lock_test_and_set(p, val);	}

inline long atomic_inc(volatile long *p)
{	return atomic_add(p, 1);	}
inline ulong atomic_inc(volatile ulong *p)
{	return atomic_add(p, 1U);	}

inline long atomic_dec(volatile long *p)
{	return atomic_sub(p, 1);	}
inline ulong atomic_dec(volatile ulong *p)
{	return atomic_sub(p, 1U);	}

inline long atomic_cmpxchg(volatile long *p, long cmp, long val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}
inline ulong atomic_cmpxchg(volatile ulong *p, ulong cmp, ulong val)
{	return __sync_val_compare_and_swap(p, cmp, val);	}

inline long atomic_min(volatile long *p, long val)
{
	long old;
#pragma omp critical
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}
inline ulong atomic_min(volatile ulong *p, ulong val)
{
	ulong old;
#pragma omp critical
	{
		old = *p;
		if (old > val)
			*p = val;
	}
	return old;
}

inline long atomic_max(volatile long *p, long val)
{
	long old;
#pragma omp critical
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}
inline ulong atomic_max(volatile ulong *p, ulong val)
{
	ulong old;
#pragma omp critical
	{
		old = *p;
		if (old < val)
			*p = val;
	}
	return old;
}

inline long atomic_and(volatile long *p, long val)
{	return __sync_fetch_and_and(p, val);	}
inline ulong atomic_and(volatile ulong *p, ulong val)
{	return __sync_fetch_and_and(p, val);	}

inline long atomic_or(volatile long *p, long val)
{	return __sync_fetch_and_or(p, val);	}
inline ulong atomic_or(volatile ulong *p, ulong val)
{	return __sync_fetch_and_or(p, val);	}

inline long atomic_xor(volatile long *p, long val)
{	return __sync_fetch_and_xor(p, val);	}
inline ulong atomic_xor(volatile ulong *p, ulong val)
{	return __sync_fetch_and_xor(p, val);	}

// declare alias for the atom_* versions
#define atom_add		atomic_add
#define atom_sub		atomic_sub
#define atom_and		atomic_and
#define atom_xor		atomic_xor
#define atom_or			atomic_or
#define atom_min		atomic_min
#define atom_max		atomic_max
#define atom_inc		atomic_inc
#define atom_dec		atomic_dec
#define atom_xchg		atomic_xchg
#define atom_cmpxchg	atomic_cmpxchg

#endif
