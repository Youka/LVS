#pragma once

struct CairoImage{
	// Dimension
	const int width, height, stride;
	// RGB or RGBA?
	const bool has_alpha;
	// Pixel data
	unsigned char* const data;
	// Constructor / initialize image information & data
	CairoImage(int width, int height, bool has_alpha) : width(width), height(height), stride(width<<2), has_alpha(has_alpha), data(new unsigned char[stride*height]){}
	// Destructor / free image data
	~CairoImage(){delete[] this->data;}
	// Frame types
	enum API : unsigned char{AVS = 0, VDUB, DSHOW};
	// Load from frame
	void Load(register unsigned char *frame, int pitch, API api);
	// Save to frame
	void Save(register unsigned char *frame, int pitch, API api);
};
