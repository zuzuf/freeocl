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
#ifndef __FREEOCL_UTILS_SMARTPTR_H__
#define __FREEOCL_UTILS_SMARTPTR_H__

#include <cstddef>

namespace FreeOCL
{
	class ref_count
	{
	public:
		ref_count() : counter(0)	{}

		void retain()	{	++counter;	}
		void release()	{	--counter;	}
		size_t getRefCount() const	{	return counter;	}
	private:
		size_t counter;
	};

	template<class T>
	class smartptr
	{
		template<class U> friend class FreeOCL::smartptr;
	public:
		smartptr() : ptr(NULL)	{}
		smartptr(T *ptr) : ptr(ptr)	{	if (ptr)	ptr->retain();	}
		smartptr(const T *ptr) : ptr(const_cast<T*>(ptr))	{	if (this->ptr)	this->ptr->retain();	}
		smartptr(smartptr &ptr) : ptr(ptr.ptr)	{	if (this->ptr)	this->ptr->retain();	}
		smartptr(const smartptr &ptr) : ptr(const_cast<T*>(ptr.ptr))	{	if (this->ptr)	this->ptr->retain();	}
		template<class U>
		smartptr(const smartptr<U> &ptr) : ptr(dynamic_cast<T*>(const_cast<U*>(ptr.ptr)))	{	if (this->ptr)	this->ptr->retain();	}
		template<class U>
		smartptr(const U *ptr) : ptr(dynamic_cast<T*>(const_cast<U*>(ptr)))	{	if (this->ptr)	this->ptr->retain();	}
		~smartptr()
		{
			if (ptr)
			{
				ptr->release();
				if (ptr->getRefCount() == 0)
					delete ptr;
			}
		}

		smartptr &operator=(const smartptr &ptr)
		{
			if (this->ptr == ptr.ptr)
				return *this;
			T *old = this->ptr;
			this->ptr = const_cast<T*>(ptr.ptr);
			if (this->ptr)
				this->ptr->retain();
			if (old)
			{
				old->release();
				if (old->getRefCount() == 0)
					delete old;
			}
			return *this;
		}

		smartptr &operator=(const T *ptr)
		{
			T *old = this->ptr;
			this->ptr = const_cast<T*>(ptr);
			if (this->ptr)
				this->ptr->retain();
			if (old)
			{
				old->release();
				if (old->getRefCount() == 0)
					delete old;
			}
			return *this;
		}

		T *operator->()	{	return ptr;	}
		const T *operator->() const	{	return ptr;	}
		T &operator*()	{	return *ptr;	}
		const T &operator*() const	{	return *ptr;	}

		operator bool() const	{	return ptr != NULL;	}

		template<class U>
		U *as()	{	return dynamic_cast<U*>(ptr);	}

		template<class U>
		const U *as() const	{	return dynamic_cast<const U*>(ptr);	}
	private:
		T *ptr;
	};
}

#endif
