#ifndef __FREEOCL_THREAD_H__
#define __FREEOCL_THREAD_H__

#include <pthread.h>
#include <errno.h>
#include "mutex.h"

namespace FreeOCL
{
	class thread : public mutex
	{
	public:
		thread();
		~thread();

		virtual int proc() = 0;

		void start();
		void kill();
		void join();
		inline bool running() const	{	return b_running;	}

	private:
		static void *exec(void *p);

	private:
		pthread_t pt;
		volatile bool b_running;
	};
}

#endif
