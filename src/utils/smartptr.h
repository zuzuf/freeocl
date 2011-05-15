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
	template<class T>
	class smartptr
	{
	private:
		class wrapper
		{
		public:
			wrapper(T *ptr) : ptr(ptr), ref_count(1)	{}
			~wrapper()	{	delete ptr;	}

			void retain()	{	++ref_count;	}
			void release()	{	--ref_count;	}
			size_t getRefCount() const	{	return ref_count;	}
			T *getPtr()	{	return ptr;	}
			const T *getPtr() const	{	return ptr;	}
		private:
			size_t ref_count;
			T *ptr;
		};

	public:
		smartptr() : pwrapper(NULL)	{}
		smartptr(T *ptr) : pwrapper(new wrapper(ptr))	{}
		smartptr(smartptr &ptr) : pwrapper(ptr.pwrapper)	{	if (pwrapper)	pwrapper->retain();	}
		smartptr(const smartptr &ptr) : pwrapper(const_cast<wrapper*>(ptr.pwrapper))	{	if (pwrapper)	pwrapper->retain();	}
		~smartptr()
		{
			if (pwrapper)
			{
				pwrapper->release();
				if (pwrapper->getRefCount() == 0)
					delete pwrapper;
			}
		}

		smartptr &operator=(const smartptr &ptr)
		{
			if (pwrapper == ptr.pwrapper)
				return *this;
			wrapper *old = pwrapper;
			pwrapper = const_cast<wrapper*>(ptr.pwrapper);
			if (pwrapper)
				pwrapper->retain();
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
			wrapper *old = pwrapper;
			if (ptr)
				pwrapper = new wrapper(const_cast<T*>(ptr));
			else
				pwrapper = NULL;
			if (old)
			{
				old->release();
				if (old->getRefCount() == 0)
					delete old;
			}
			return *this;
		}

		T *operator->()	{	return pwrapper->getPtr();	}
		const T *operator->() const	{	return pwrapper->getPtr();	}
		T &operator*()	{	return *(pwrapper->getPtr());	}
		const T &operator*() const	{	return *(pwrapper->getPtr());	}

		operator bool() const	{	return pwrapper != NULL;	}
	private:
		wrapper *pwrapper;
	};
}

#endif
