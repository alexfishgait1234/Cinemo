#ifndef ENGINE_H_
#define ENGINE_H_

#include <vector>
#include <stack>
#include <string>
#include "mutex.h"
#include <thread>
#include <iostream>

#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#else  // _WIN32
#include <dirent.h>
#include <unistd.h>
#endif  // _WIN32
#include <optional>
#include "thread_context.h"
#include "io.h"

namespace fs = std::filesystem;
using namespace thread_contex;
using namespace IO;
typedef  std::unique_ptr<ThreadContext> TContex;

namespace engine
{
	class Engine
	{
	  public:
		//the only public interface
		static int GetNumOfcores();
		static void* ConvertFileThread(void* arg);
		
	  private:
		static void EncodeFile(const std::string& path);
		static void HandleStreams( IInput& src, IOutput& dst);
		static std::optional<std::string> GetNextFile(ThreadContext* ctx);
	  private:
		static const auto BufSize = 1024 * 1024;
		static const int Mp3Quality = 5; //es required ... 0 is best,9 -worst
	};
}

#endif