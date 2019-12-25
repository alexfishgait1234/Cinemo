

#include "thread_context.h"
#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;

namespace thread_contex
{
	void ThreadContext::Log(const std::string& message)
	{
  #ifdef NDEBUG
   // nondebug -disable prints
  #else
  mutex::MutexGuard locker(&log_mutex);
		std::thread::id this_id = std::this_thread::get_id();
		std::cout << message << "    "<<this_id << std::endl;
  #endif
		
	};

	std::string ThreadContext::GetFile() 
	{  auto result = files.top();
		files.pop(); 
		return result;
	}

	ThreadContext::ThreadContext(const std::string& path) :
		log_mutex(PTHREAD_MUTEX_INITIALIZER),
		queue_mutex(PTHREAD_MUTEX_INITIALIZER),
		m_exit(false),
		m_no_more_work(false)
		{
		   files = EnumDir(path);
		}

	StackStrings ThreadContext::EnumDir(const std::string& folder_path) 
	{
	  StackStrings result;
	 
 
	  for (const auto& entry : fs::directory_iterator(folder_path))
	  {
		  if (!entry.is_regular_file())
			  continue;
		 
		 
		  auto ext = fs::path(entry).extension();
		  auto new_ext = ext.string();
		  auto old_ext = ext;
		  std::transform(new_ext.begin(),new_ext.end(),new_ext.begin(),::tolower);
		 
		  if (new_ext == ".wav")
		  {
			  const auto path_filenameStr = entry.path().string();
			  result.push(path_filenameStr);
		  }
		  else  //for non wav files return old extension
			  fs::path(entry).replace_extension(old_ext);
	  }
   
	  return result;
	}
}