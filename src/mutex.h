#ifndef __FREEOCL_MUTEX_H__
#define __FREEOCL_MUTEX_H__

#include <pthread.h>
#include <errno.h>

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

	private:
		pthread_mutex_t pm;
	};
}

#endif
