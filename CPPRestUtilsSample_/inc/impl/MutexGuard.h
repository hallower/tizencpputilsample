#ifndef __CSK_MUTEXGUARD_H__
#define __CSK_MUTEXGUARD_H__


#include<semaphore.h>
#include"csk_log.h"


namespace csk
{

class MutexGuard {
public:
	MutexGuard(sem_t* m)
	: mutex(m)
	 {
		sem_wait(mutex);
	}

	~MutexGuard(void)
	 {
		sem_post(mutex);
	}

private:
	MutexGuard(const MutexGuard& rhs);

	MutexGuard& operator =(const MutexGuard& rhs);

private:
	sem_t* mutex;
};

}
#endif /* __CSK_MUTEXGUARD_H__ */
