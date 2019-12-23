
#include "io.h"
#include <algorithm>
#include <fstream>
#include <vector>
#include <lame.h>
using namespace IO;

  int Mp3Out::EncodeSamples(const uint8_t* src_buffer, int num_samples, uint8_t* dst_buffer, int dst_size)
  {
    std::unique_ptr<int16_t[]> newsem;
    int16_t* src = nullptr;
    switch (mp3_info.bits_per_sample)
	{
      case 8:
        newsem.reset(new int16_t[num_samples * mp3_info.num_channels]);
        src = newsem.get();
        for (int i = 0; i < num_samples * mp3_info.num_channels; ++i)
          src[i] = (src_buffer[i] - 128) << 8;
        break;
      case 16:
        src = const_cast<int16_t*>(reinterpret_cast<const int16_t*>(src_buffer));
        break;
      default: 
		  return -1;
    }

	if (1 == mp3_info.num_channels)
	{
		return lame_encode_buffer(ptrlame_lib.get(), src, nullptr, num_samples, dst_buffer, dst_size);
	}
	if (2 == mp3_info.num_channels)
	{
		return lame_encode_buffer_interleaved(ptrlame_lib.get(), src, num_samples, dst_buffer, dst_size);
	}
    
     return -1;
   
  }//EncodeSamples

 
  Mp3Out::Mp3Out(const std::string& filename) :
	  out_buffer(kBufferSize),
	  destination(filename, std::ios::binary),
	  was_exception(false),
	  ptrlame_lib(lame_init(), lame_close)
  {
	
    if (!destination  || !ptrlame_lib)
	{
       throw std::runtime_error("exception in lame  initialize");
    }
   
  }

  bool Mp3Out::SetInfo(const Info& info)
  {
	  auto status{ false };
	  do
	  {
		  if (info.num_channels != 1 && info.num_channels != 2)
		  {
			  break;
		  }
		  if (info.bits_per_sample != 8 && info.bits_per_sample != 16)
		  {
			  break;
		  }
		  mp3_info = info;
		  if (lame_set_num_channels(ptrlame_lib.get(), mp3_info.num_channels))
			  break;
		  if (lame_set_mode(ptrlame_lib.get(), mp3_info.num_channels == 1 ? MONO : STEREO))
			  break;
		  if (lame_set_in_samplerate(ptrlame_lib.get(), mp3_info.sample_rate))
			  break;
		  if (lame_set_quality(ptrlame_lib.get(), mp3_info.quality))
			  break;
		  if (lame_init_params(ptrlame_lib.get()))
			  break;
		  status = true;
	  } while (0);

		return status;
  }

  std::size_t Mp3Out::Write(const uint8_t* buffer, std::size_t size)
  {
    try 
	{
      const auto item_size = (mp3_info.bits_per_sample * mp3_info.num_channels) >> 3;
      for (decltype(size) iter = 0; iter < size; iter += out_buffer.size())
	  {
        auto in_size = std::min(size, iter + out_buffer.size()) - iter;
		if (in_size % item_size) 
			return 0;
        auto out_size = EncodeSamples(buffer + iter, in_size / item_size, out_buffer.data(), out_buffer.size());
		if (out_size < 0) 
			return 0;
        destination.write(reinterpret_cast<const char*>(out_buffer.data()), out_size);
		if (!destination)
			return 0;
      }


      return size;
    }
	catch (const std::exception&) 
	{
      was_exception = true;
      throw;
    }
  }

  Mp3Out::~Mp3Out()
  {
    if (was_exception) 
		return;
    auto out_size = lame_encode_flush(ptrlame_lib.get(), out_buffer.data(), out_buffer.size());
    if (out_size > 0) 
		destination.write(reinterpret_cast<char*>(out_buffer.data()), out_size);
  }




 
