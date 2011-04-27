#ifndef __FREEOCL_CONDITION_H__
#define __FREEOCL_CONDITION_H__

#include <pthread.h>
#include <errno.h>
#include "mutex.h"

namespace FreeOCL
{
	class condition : public mutex
	{
	public:
		condition();
		~condition();

		void wait();
		void wait_locked();

		inline void wakeup()
		{
			lock();
			++i;
			pthread_cond_broadcast(&pc);
			unlock();
		}

	private:
		pthread_cond_t pc;
		volatile size_t i;
	};
}

#endif
