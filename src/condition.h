#ifndef __FREEOCL_CONDITION_H__
#define __FREEOCL_CONDITION_H__

#include <pthread.h>
#include <errno.h>

namespace FreeOCL
{
	class condition
	{
	public:
		condition();
		~condition();

		void wait();
		void wait_locked();

		inline void lock()
		{
			pthread_mutex_lock(&pm);
		}

		inline void unlock()
		{
			pthread_mutex_unlock(&pm);
		}

		inline void wakeup()
		{
			lock();
			++i;
			pthread_cond_broadcast(&pc);
			unlock();
		}

	private:
		pthread_cond_t pc;
		pthread_mutex_t pm;
		volatile size_t i;
	};
}

#endif
