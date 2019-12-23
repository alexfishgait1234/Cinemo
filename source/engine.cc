

#include "engine.h"
#include "thread_context.h"
#include "IO.h"
#include <algorithm>
using namespace thread_contex;
using namespace IO;

namespace engine
{
	
	    void Engine::EncodeFile(const std::string& path)
		{
		  try
		  {
			  std::string path_to_check = path;
	          std::transform(path_to_check.begin(),path_to_check.end(),path_to_check.begin(),::tolower);
			  if (fs::path filePath = path_to_check; filePath.extension() == ".wav")
			  {
				  auto src = ReadWavFile(path);
				  const auto& src_info = src->GetInfo();
				  Info dst_info = { src_info.num_channels, src_info.sample_rate, src_info.bits_per_sample, Mp3Quality };

				  filePath.replace_extension(".mp3");
				  auto dst = WriteMp3File(filePath.string());
				  if (auto status = dst->SetInfo(dst_info); !status)
					  throw;
				  HandleStreams(*src, *dst);
			  }
		  } catch (const std::exception&)
		  {
			const auto& msg = "failedd encoding::exception for " + path;
			std::throw_with_nested(std::runtime_error(msg));
		  }
        }


		std::optional<std::string> Engine::GetNextFile(ThreadContext* ctx)
		{
			try
			{
				mutex::MutexGuard guard_lock(ctx->GetQueueMutex());


				if (ctx->IsEmpty())
				{
					ctx->m_no_more_work = true; //in feature producer thread will set it
					return std::nullopt;
				}
				
				return ctx->GetFile();;
			}
			catch (...)
			{
				return "EXC";
			}
		}


		int Engine::GetNumOfcores()
		{
			#ifndef _WIN32
			  return sysconf(_SC_NPROCESSORS_ONLN);
			#else  // _WIN32
			  SYSTEM_INFO sysinfo;
			  GetSystemInfo(&sysinfo);
			  return sysinfo.dwNumberOfProcessors;
			#endif  // _WIN32
		}

		void* Engine::ConvertFileThread(void* arg)
		{
		  auto ctx = reinterpret_cast<ThreadContext*>(arg);
		  auto dbg_work_counter = 0; //to check if thread did work and how many files processed
    
		  do {
			  try
			  {
				  const auto& filename = GetNextFile(ctx);
				  if (filename == std::nullopt)
				  {
					  //we will wait for exit flag from main thread..could just exit if there is no work 
					  //but i prefer to keep options for extending ..like usage of conditional variables and producer
					  //thread which notifes about wav creation/addition
					  std::this_thread::yield();
					  continue;
					  //return nullptr;
				  }
				  if (*filename == "EXC")
					  continue;

				  ctx->Log("Processing " + *filename +" in thread ");
			 
				  Engine::EncodeFile(*filename);
				  dbg_work_counter++;
			  }
			  catch (const std::exception& ex)
			  {
				  ctx->Log("exception::ConvertFileThread in thread ");
				 
			  }
		  } while (!ctx->m_exit);

		  std::string str = std::to_string(dbg_work_counter);
		  ctx->Log("exiting thread  "+ str);
		  return nullptr;
		}


		void Engine::HandleStreams( IInput& src, IOutput& dst)
		{
			  try
			  {
				std::vector<uint8_t> buffer(BufSize);
				for (;;)
				{
				  auto size = src.Read(buffer.data(), buffer.size());
				  if (!size)
					  break;
				  if (dst.Write(buffer.data(), size) != size)
					throw std::runtime_error("Failed to write compete data");
				}
			  } catch (const std::exception&) 
			  {
				std::throw_with_nested(std::runtime_error("Streams handling failed"));
			  }
        }
	
}
