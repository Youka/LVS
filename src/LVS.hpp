#pragma once

#include "LVSVideo.hpp"
#include "LVSAudio.hpp"

// Filter process class
class LVS{
	private:
		// Video process
		LVSVideo *video_proc;
		// Audio process
		LVSAudio *audio_proc;
	public:
		// Get video + audio + data string
		LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames,
				const char* audio_file, int channels, int sample_rate, __int64 samples,
				const char* data_string = 0);
		// Get only video
		LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames);
		// Free video & audio process
		~LVS();
		// Render on frame
		void RenderOnFrame(CairoImage* image, int frame_number);
		// Render on samples
		void RenderOnSamples(float* buf, __int64 buf_size, __int64 start_sample);
};
