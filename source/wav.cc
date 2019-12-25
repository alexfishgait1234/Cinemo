
#include "IO.h"
#include <algorithm>
#include <fstream>
#include <optional>

using namespace IO;

std::optional<Info> WavIn::ReadWavHeaderInfo(std::istream& src)
{
  WavHeader header;
  src.read(reinterpret_cast<char*>(&header), sizeof(header));
  if (!src || src.gcount() != sizeof(header))
	  return std::nullopt;

  if (header.riff_header != g_RiffHr || header.wave_label != g_WaveLabel || header.fmt_header != g_FormatHr ||
	  header.audio_format != g_PcmFormat)
  {
	  return std::nullopt;
  }

  auto next_chunk = g_FormatOffset + header.fmt_size;
  src.seekg(next_chunk - sizeof(header), std::ios_base::cur);
  if (!src)
	  return std::nullopt;

  Info wav_info {header.num_channels, static_cast<int>(header.sample_rate), header.bits_per_sample};
  return wav_info;
}

bool WavIn::FindData(std::istream& src, std::size_t& chunk_size)
{
  while(true)
  {
    uint32_t chunk_header[2];
    src.read(reinterpret_cast<char*>(chunk_header), sizeof(chunk_header));
    if (src.eof())
		return false;
	if (!src || src.gcount() != sizeof(chunk_header))
		return false;
    if (chunk_header[0] == g_DataHr) 
	{
       chunk_size = chunk_header[1];
       return true;
    }
    src.seekg(chunk_header[1], std::ios_base::cur);
  }
}



   WavIn::WavIn(const std::string& fname) :
		blocksize(0),
		blockpos(0)

	{
		src_ = std::ifstream(fname, std::ios::binary);
				
		if (auto wavinfo = ReadWavHeaderInfo(src_); wavinfo != std::nullopt)
		{
			wav_info = *wavinfo;
		}

	}

	Info WavIn::GetInfo() const
	{
		return wav_info;
	}

	std::size_t WavIn::Read(uint8_t* buffer, std::size_t size) 
	{
		if (!blocksize && !FindData(src_, blocksize))
			return 0;
		auto op_size = std::min(size, blocksize - blockpos);
		src_.read(reinterpret_cast<char*>(buffer), op_size);
		if (!src_ || static_cast<std::size_t>(src_.gcount()) != op_size)
			return 0;
		if ((blockpos += op_size) == blocksize)
		{
			blocksize = 0;
			blockpos = 0;
		}
		return op_size;
	}








