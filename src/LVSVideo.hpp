#pragma once

#include "LVSMediaBase.hpp"
#include "image.hpp"

// LVS video class
class LVSVideo : public LVSMediaBase{
	public:
		// Prepare and call video script
		LVSVideo(const char* script, int width, int height, bool has_alpha, double fps, int frames);
		// Call frame render function
		void Render(CairoImage* image, int frame_number);
};
