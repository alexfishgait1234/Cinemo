
#include "mutex.h"
#include "engine.h"

#include <iostream>
#include <vector>
#include <optional>
#include "thread_context.h"
#include <chrono>

using namespace engine;
using namespace thread_contex;
using namespace std::chrono_literals;
typedef  std::unique_ptr<ThreadContext> TContex;

int main(int argc, char** argv) {
  try
  {
    if (argc < 2) 
	{
      std::cout << "Wrong number of arguments" <<  std::endl;
      return 0;
    }
   
    TContex context(new ThreadContext(argv[1]));
    if (context->IsEmpty())
	{
      std::cout << "Directory: no wav files " << argv[1] << std::endl;
      return 0;
    }

    std::vector<pthread_t> threads(Engine::GetNumOfcores());
    for (auto& it : threads) 
	{
		if (pthread_create(&it, nullptr, Engine::ConvertFileThread, context.get()))
			return 1;
    }

	while (false == context->m_no_more_work)
	{
		//use conditional variable instead of wait loop
		std::this_thread::yield();
		std::this_thread::sleep_for(2s);
	}
	//signal all threads to stop..we can extend this to create thread for add on the fly wav files notifications
	context->m_exit = true;
    for (auto it : threads)
	{
		if (pthread_join(it, nullptr))
			return 1;
    }
    return 0;
  }
  catch (const std::exception& ex)
  {
     //log exception
    return 1;
  }
}
