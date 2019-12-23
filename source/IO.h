#ifndef IO_H_
#define IO_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <lame.h>

namespace IO
{
	class IOutput;
	class IInput;

	struct Info
	{
	  int num_channels;
	  int sample_rate;
	  int bits_per_sample;
	  int quality;
	};

	#pragma pack(push, 1)
	struct WavHeader 
	{
	  uint32_t riff_header;
	  uint32_t riff_size;
	  uint32_t wave_label;
	  uint32_t fmt_header;
	  uint32_t fmt_size;
	  uint16_t audio_format;
	  uint16_t num_channels;
	  uint32_t sample_rate;
	  uint32_t byte_rate;
	  uint16_t block_align;
	  uint16_t bits_per_sample;
	};
    #pragma pack(pop)


	using cout_mp3 = std::unique_ptr<IOutput>;
	
	using cin_wav = std::unique_ptr<IInput>;
	

	class IInput 
	{
		public:
		  virtual std::size_t Read(uint8_t* buffer, std::size_t size) = 0;
		  virtual Info GetInfo() const = 0;
		  virtual ~IInput() = default;
		protected:
		  Info wav_info;
	};

	class IOutput 
	{
		public:
		  virtual std::size_t Write(const uint8_t* buffer, std::size_t size) = 0;
		  virtual bool SetInfo(const Info& info) = 0;
		  virtual ~IOutput() = default;
		protected:
		  Info mp3_info;
	};


using LamePtr = std::unique_ptr<lame_global_flags, decltype(&lame_close)>;
class Mp3Out : public IOutput
{
	
	public:
		Mp3Out(const std::string& filename);
		bool SetInfo(const Info& info) override;
		std::size_t Write(const uint8_t* buffer, std::size_t size) override;
 
		~Mp3Out();
		 inline static const std::size_t kBufferSize = 128 * 1024;
	 private:
		std::vector<uint8_t> out_buffer;
		std::ofstream destination;
 
		bool was_exception;
		LamePtr ptrlame_lib;

		int EncodeSamples(const uint8_t* src_buffer, int num_samples, uint8_t* dst_buffer, int dst_size);
		
}; 

static cout_mp3 WriteMp3File(const std::string& fname)
{
  return cout_mp3(new Mp3Out(fname));
}


class WavIn : public IInput
{


public:
	WavIn(const std::string& fname);

	Info GetInfo() const override;
	

	std::size_t Read(uint8_t* buffer, std::size_t size) override;
	
private:
	std::size_t blocksize;
	std::size_t blockpos;
	std::ifstream src_;

	std::optional<Info> ReadWavHeaderInfo(std::istream& src);
	bool FindData(std::istream& src, std::size_t& chunk_size);
public:
	inline static const std::size_t g_FormatOffset = 5 * sizeof(uint32_t);
	inline static const uint16_t g_PcmFormat = 1;
	inline static const uint32_t g_DataHr = 0x61746164;  
	inline static const uint32_t g_FormatHr = 0x20746d66;  
	inline static const uint32_t g_RiffHr = 0x46464952;  
	inline static const uint32_t g_WaveLabel = 0x45564157;  
};

static cin_wav ReadWavFile(const std::string& fname)
{
  return cin_wav(new WavIn(fname));
}


} 

#endif  
