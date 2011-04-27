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
}
