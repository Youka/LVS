#pragma once

#include "image.hpp"

// Filter process class
class LVS{
	public:
		// Get video + audio + script environment
		LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames,
				const char* audio_file, int channels, int sample_rate, __int64 samples,
				void* avs_env);
		// Get only video
		LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames);
		// Render on frame
		void RenderOnFrame(CairoImage* data, int frame_number);
		// Render on samples
		void RenderOnSamples(float* buf, __int64 start, __int64 count);
};
