#include "mutex.h"
#include <utility>
#include  <stdexcept>


namespace mutex 
{

MutexGuard::MutexGuard(pthread_mutex_t* mutex)  :
    m_mutex(mutex) {
	if (pthread_mutex_lock(m_mutex))
		throw std::runtime_error("Failed to lock mutex");

}

MutexGuard::MutexGuard(MutexGuard&& other) noexcept  :
    m_mutex(std::move(other.m_mutex)) 
{

  other.m_mutex = nullptr;
}

MutexGuard::~MutexGuard() 
{
  if (m_mutex) 
	  pthread_mutex_unlock(m_mutex);
}

}  
