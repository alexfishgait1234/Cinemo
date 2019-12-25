#ifndef THREAD_CTX_H_
#define THREAD_CTX_H_

#include <vector>
#include <stack>
#include <string>
#include "mutex.h"
#include <thread>
#include <iostream>
#include <atomic>
//#include <condition_variable>

namespace thread_contex
{
	typedef  std::stack<std::string> StackStrings;
	typedef pthread_mutex_t Mutex;

	class ThreadContext
	{
	public:

		ThreadContext(const std::string& path);

		Mutex* GetQueueMutex()  { return &queue_mutex; };
		bool IsEmpty() const { return files.empty(); };
		std::string GetFile();
		
		void Log(const std::string& message);
		StackStrings EnumDir(const std::string& path);
		std::atomic<bool>  m_exit;  //to signal all worke threads to exit
		std::atomic<bool>  m_no_more_work; //to notify main thread that all files were processed
	private:
		
		Mutex log_mutex;
		Mutex queue_mutex;
		StackStrings files;
		
	};
}

#endif