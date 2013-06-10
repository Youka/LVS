#pragma once

#include <cstring>

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
	// Load from frame
	void Load(register unsigned char *frame, int pitch, bool rgb32){
		// Prepare image pointers
		register unsigned char *image = const_cast<unsigned char*>(this->data);
		frame += pitch * (this->height-1);
		// Load by colorspace
		if(this->has_alpha || rgb32)
			for(int y = 0; y < this->height; y++){
				::memcpy(image, frame, this->stride);
				frame -= pitch;
				image += this->stride;
			}
		else{
			const int rowsize_pitch = this->width * 3 + pitch;
			for(int y = 0; y < this->height; y++){
				for(int x = 0; x < this->width; x++){
					image[0] = frame[0];
					image[1] = frame[1];
					image[2] = frame[2];
					frame += 3;
					image += 4;
				}
				frame -= rowsize_pitch;
			}
		}
	}
	// Save to frame
	void Save(register unsigned char *frame, int pitch, bool rgb32){
		// Prepare image pointers
		register unsigned char *image = const_cast<unsigned char*>(this->data);
		frame += pitch * (this->height-1);
		// Save by colorspace
		if(this->has_alpha || rgb32)
			for(int y = 0; y < this->height; y++){
				::memcpy(frame, image, this->stride);
				image += this->stride;
				frame -= pitch;
			}
		else{
			const int rowsize_pitch = this->width * 3 + pitch;
			for(int y = 0; y < this->height; y++){
				for(int x = 0; x < this->width; x++){
					frame[0] = image[0];
					frame[1] = image[1];
					frame[2] = image[2];
					image += 4;
					frame += 3;
				}
				frame -= rowsize_pitch;
			}
		}
	}
};
