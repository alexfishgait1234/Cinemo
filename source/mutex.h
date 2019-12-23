#ifndef CTT_THREAD_H_
#define CTT_THREAD_H_


#include <pthread.h>



namespace mutex
{

class MutexGuard 
{

 public:
	  MutexGuard() = default;
	  MutexGuard(const MutexGuard&) = delete;
	  MutexGuard& operator=(const MutexGuard&) = delete;

	  MutexGuard(pthread_mutex_t* mutex) ;
	  MutexGuard(MutexGuard&& other) noexcept ;
	  ~MutexGuard();
   private:
     pthread_mutex_t* m_mutex;

};

}  

#endif  
