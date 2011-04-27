#include "condition.h"

namespace FreeOCL
{
	condition::condition() : i(0)
	{
		pthread_cond_init(&pc, NULL);
	}

	condition::~condition()
	{
		wakeup();
		pthread_cond_destroy(&pc);
	}

	void condition::wait()
	{
		pthread_mutex_lock(&pm);
		const size_t ref = i;
		while(pthread_cond_wait(&pc, &pm) == 0 && ref == i)	{}

		pthread_mutex_unlock(&pm);
	}

	void condition::wait_locked()
	{
		const size_t ref = i;
		while(pthread_cond_wait(&pc, &pm) == 0 && ref == i)	{}
	}
}
