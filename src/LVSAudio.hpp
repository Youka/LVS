#pragma once

#include "LVSMediaBase.hpp"

class LVSAudio : public LVSMediaBase{
	public:
		LVSAudio(const char* script, int channels, int sample_rate, __int64 samples);
		void Render(float* buf, __int64 start, __int64 count);
};
