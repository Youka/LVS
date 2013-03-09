#pragma once

#include "LVSMediaBase.hpp"

// LVS audio class
class LVSAudio : public LVSMediaBase{
	public:
		// Prepare and call audio script
		LVSAudio(const char* script, int channels, int sample_rate, __int64 samples, const char* data_string = 0);
		// Call samples render function
		void Render(float* buf, __int64 buf_size, __int64 start_sample);
};
