#pragma once

#ifndef _WIN32
	#include <semaphore.h>
#endif

class CSemaphore
{
	private:
#ifdef _WIN32
		HANDLE	m_hSem;
#else
		sem_t* m_hSem;
#endif

	public:
		CSemaphore();
		~CSemaphore();

		int	Initialize();
		void	Clear();
		void	Destroy();
		int	Wait();
		int	Release(int count = 1);
};
