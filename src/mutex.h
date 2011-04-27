#ifndef __FREEOCL_MUTEX_H__
#define __FREEOCL_MUTEX_H__

#include <pthread.h>
#include <errno.h>
#include <set>

namespace FreeOCL
{
	class mutex
	{
	public:
		mutex();
		~mutex();

		inline void lock()
		{
			pthread_mutex_lock(&pm);
		}

		inline bool try_lock()
		{
			return pthread_mutex_trylock(&pm) == EBUSY;
		}

		inline void unlock()
		{
			pthread_mutex_unlock(&pm);
		}

	protected:
		pthread_mutex_t pm;
	};

	class unlocker
	{
	public:
		~unlocker();

		void handle(mutex *m);
		inline void handle(mutex &m)	{	handle(&m);	}

		void forget(mutex *m);
		inline void forget(mutex &m)	{	forget(&m);	}

		void unlockall();

	private:
		std::set<mutex*> objects;
	};
}

#endif
