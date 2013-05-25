#include "image.hpp"
#include <cstring>

void CairoImage::Load(register unsigned char *frame, int pitch, API api){
	// Image pointer
	register unsigned char *image = const_cast<unsigned char*>(this->data);
	// Load by API
	switch(api){
		case AVS:{
			frame += pitch * (this->height-1);
			if(this->has_alpha)
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
		}break;
		case VDUB:{
			frame += pitch * (this->height-1);
			for(int y = 0; y < this->height; y++){
				::memcpy(image, frame, this->stride);
				frame -= pitch;
				image += this->stride;
			}
		}break;
		case DSHOW:{
			frame += this->stride * (this->height-1);
			if(this->has_alpha)
				for(int y = 0; y < this->height; y++){
					::memcpy(image, frame, this->stride);
					frame -= this->stride;
					image += this->stride;
				}
			else{
				const int rowsize2 = (this->width * 3) << 1;
				for(int y = 0; y < this->height; y++){
					for(int x = 0; x < this->width; x++){
						image[0] = frame[0];
						image[1] = frame[1];
						image[2] = frame[2];
						frame += 3;
						image += 4;
					}
					frame -= rowsize2;
				}
			}
		}break;
	}
}

void CairoImage::Save(register unsigned char *frame, int pitch, API api){
	// Image pointer
	register unsigned char *image = const_cast<unsigned char*>(this->data);
	// Save by API
	switch(api){
		case AVS:{
			frame += pitch * (this->height-1);
			if(this->has_alpha)
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
		}break;
		case VDUB:{
			frame += pitch * (this->height-1);
			for(int y = 0; y < this->height; y++){
				::memcpy(frame, image, this->stride);
				image += this->stride;
				frame -= pitch;
			}
		}break;
		case DSHOW:{
			frame += this->stride * (this->height-1);
			if(this->has_alpha)
				for(int y = 0; y < this->height; y++){
					::memcpy(frame, image, this->stride);
					image += this->stride;
					frame -= this->stride;
				}
			else{
				const int rowsize2 = (this->width * 3) << 1;
				for(int y = 0; y < this->height; y++){
					for(int x = 0; x < this->width; x++){
						frame[0] = image[0];
						frame[1] = image[1];
						frame[2] = image[2];
						image += 4;
						frame += 3;
					}
					frame -= rowsize2;
				}
			}
		}break;
	}
}