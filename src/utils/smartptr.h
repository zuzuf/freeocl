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
		size_t get_ref_count() const	{	return counter;	}
	private:
		size_t counter;
	};

	template<bool, class T> struct __if;
	template<class T> struct __if<true, T>	{	typedef T type;	};

	template<class T, class V = void> struct __smartptr_trait_validation
	{
		static inline bool valid(const T *) {	return true;	}
		static inline void invalidate(T *) {}
	};

	template<class T> struct __smartptr_trait_validation<T, typename __if<T::b_has_validation_flag, void>::type>
	{
		static inline bool valid(const T *obj) {	return obj ? obj->valid() : false;	}
		static inline void invalidate(const T *obj)
		{
			if (obj)
				obj->invalidate();
		}
	};

	template<class T, class V = void> struct __smartptr_trait_lockable
	{
		static inline void lock(const T *) {}
		static inline void unlock(const T *) {}
	};

	template<class T> struct __smartptr_trait_lockable<T, typename __if<T::b_is_lockable, void>::type>
	{
		static inline void lock(const T *obj)
		{
			if (obj)
				obj->lock();
		}
		static inline void unlock(const T *obj)
		{
			if (obj)
				obj->unlock();
		}
	};

	template<class T>
	class smartptr
	{
		template<class U> friend class FreeOCL::smartptr;
	public:
		smartptr() : ptr(NULL)	{}
		smartptr(T *ptr) : ptr(ptr)
		{
			if (ptr)
			{
				__smartptr_trait_lockable<T>::lock(ptr);
				ptr->retain();
				__smartptr_trait_lockable<T>::unlock(ptr);
			}
		}
		smartptr(const T *ptr) : ptr(const_cast<T*>(ptr))
		{
			if (this->ptr)
			{
				__smartptr_trait_lockable<T>::lock(this->ptr);
				this->ptr->retain();
				__smartptr_trait_lockable<T>::unlock(this->ptr);
			}
		}
		smartptr(smartptr &ptr) : ptr(ptr.ptr)
		{
			if (this->ptr)
			{
				__smartptr_trait_lockable<T>::lock(this->ptr);
				this->ptr->retain();
				__smartptr_trait_lockable<T>::unlock(this->ptr);
			}
		}
		smartptr(const smartptr &ptr) : ptr(const_cast<T*>(ptr.ptr))
		{
			if (this->ptr)
			{
				__smartptr_trait_lockable<T>::lock(this->ptr);
				this->ptr->retain();
				__smartptr_trait_lockable<T>::unlock(this->ptr);
			}
		}
		template<class U>
		smartptr(const smartptr<U> &ptr) : ptr(dynamic_cast<T*>(const_cast<U*>(ptr.ptr)))
		{
			if (this->ptr)
			{
				__smartptr_trait_lockable<T>::lock(this->ptr);
				this->ptr->retain();
				__smartptr_trait_lockable<T>::unlock(this->ptr);
			}
		}
		template<class U>
		smartptr(const U *ptr) : ptr(dynamic_cast<T*>(const_cast<U*>(ptr)))
		{
			if (this->ptr)
			{
				__smartptr_trait_lockable<T>::lock(this->ptr);
				this->ptr->retain();
				__smartptr_trait_lockable<T>::unlock(this->ptr);
			}
		}
		~smartptr()
		{
			if (ptr)
				clear(ptr);
		}

		smartptr &operator=(const smartptr &ptr)
		{
			if (this->ptr == ptr.ptr)
				return *this;
			__smartptr_trait_lockable<T>::lock(ptr.ptr);
			if (!__smartptr_trait_validation<T>::valid(ptr.ptr))
			{
				__smartptr_trait_lockable<T>::unlock(ptr.ptr);
				clear(this->ptr);
				this->ptr = NULL;
				return *this;
			}

			T *old = this->ptr;
			this->ptr = const_cast<T*>(ptr.ptr);
			if (this->ptr)
				this->ptr->retain();
			__smartptr_trait_lockable<T>::unlock(ptr.ptr);
			if (old)
				clear(old);
			return *this;
		}

		smartptr &operator=(const T *ptr)
		{
			if (this->ptr == ptr)
				return *this;

			__smartptr_trait_lockable<T>::lock(ptr);
			if (!__smartptr_trait_validation<T>::valid(ptr))
			{
				__smartptr_trait_lockable<T>::unlock(ptr);
				clear(this->ptr);
				this->ptr = NULL;
				return *this;
			}

			T *old = this->ptr;
			this->ptr = const_cast<T*>(ptr);
			if (this->ptr)
				this->ptr->retain();
			__smartptr_trait_lockable<T>::unlock(ptr);
			if (old)
				clear(old);
			return *this;
		}

		template<class U>
		smartptr &operator=(const U *ptr)
		{
			return operator=(dynamic_cast<const T*>(ptr));
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

		T * const &weak() const	{	return ptr;	}
		T * &weak()	{	return ptr;	}

	private:
		void clear(T *p) const
		{
			__smartptr_trait_lockable<T>::lock(p);
			p->release();
			if (p->get_ref_count() == 0)
			{
				__smartptr_trait_validation<T>::invalidate(p);
				__smartptr_trait_lockable<T>::unlock(p);
				delete p;
			}
			else
				__smartptr_trait_lockable<T>::unlock(p);
		}
	private:
		T *ptr;
	};
}

#endif
