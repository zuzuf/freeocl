#include "mutex.h"

namespace FreeOCL
{
	mutex::mutex()
	{
		// Create a default mutex
		pthread_mutex_init(&pm, NULL);
	}

	mutex::~mutex()
	{
		// Destroy the mutex
		pthread_mutex_destroy(&pm);
	}

	unlocker::~unlocker()
	{
		unlockall();
	}

	void unlocker::handle(mutex *m)
	{
		objects.insert(m);
	}

	void unlocker::forget(mutex *m)
	{
		objects.erase(m);
	}

	void unlocker::unlockall()
	{
		for(std::set<mutex*>::const_iterator i = objects.begin() ; i != objects.end() ; ++i)
			(*i)->unlock();
		objects.clear();
	}
}
