#pragma once

#include "LVSMediaBase.hpp"
#include "image.hpp"

class LVSVideo : public LVSMediaBase{
	public:
		LVSVideo(const char* script, int width, int height, bool has_alpha, double fps, int frames);
		void Render(CairoImage* image, int frame_number);
};
