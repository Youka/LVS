#pragma once

#include <cstring>

class CairoImage{
	private:
		// Dimension
		int width, height, stride;
		// RGB or RGBA?
		bool has_alpha;
		// Pixel data
		unsigned char *data;
	public:
		// Constructor / initialize image information & data
		CairoImage(int width, int height, bool has_alpha) : width(width), height(height), stride(width<<2), has_alpha(has_alpha), data(new unsigned char[stride*height]){}
		// Destructor / free image data
		~CairoImage(){
			delete[] this->data;
		}
		// Data access
		operator unsigned char*(){
			return this->data;
		}
		// Load from frame
		void load_avisynth_frame(unsigned char *frame, int pitch){
			// Data pointers
			unsigned char *image = this->data;
			frame += pitch * (this->height-1);
			// RGBA data
			if(this->has_alpha){
				for(int y = 0; y < this->height; y++){
					memcpy(image, frame, stride);
					frame -= pitch;
					image += stride;
				}
			// RGB data
			}else{
				int rowsize = this->width * 3;
				for(int y = 0; y < this->height; y++){
					for(int x = 0; x < this->width; x++){
						image[0] = frame[0];
						image[1] = frame[1];
						image[2] = frame[2];
						frame += 3;
						image += 4;
					}
					frame -= pitch + rowsize;
				}
			}
		}
		void load_vdub_frame(){
			// RGBA data
			if(this->has_alpha){

			// RGB data
			}else{

			}
		}
		void load_dshow_frame(){
			// RGBA data
			if(this->has_alpha){

			// RGB data
			}else{

			}
		}
		// Save to frame
		void save_avisynth_frame(unsigned char *frame, int pitch){
			// Data pointers
			unsigned char *image = this->data;
			frame += pitch * (this->height-1);
			// RGBA data
			if(this->has_alpha){
				for(int y = 0; y < this->height; y++){
					memcpy(frame, image, stride);
					image += stride;
					frame -= pitch;
				}
			// RGB data
			}else{
				int rowsize = this->width * 3;
				for(int y = 0; y < this->height; y++){
					for(int x = 0; x < this->width; x++){
						frame[0] = image[0];
						frame[1] = image[1];
						frame[2] = image[2];
						image += 4;
						frame += 3;
					}
					frame -= pitch + rowsize;
				}
			}
		}
		void save_vdub_frame(){
			// RGBA data
			if(this->has_alpha){

			// RGB data
			}else{

			}
		}
		void save_dshow_frame(){
			// RGBA data
			if(this->has_alpha){

			// RGB data
			}else{

			}
		}
};
/*
inline void avisynth_frame_to_image(unsigned char *frame_data, int frame_pitch, unsigned char *image_data, int image_stride, int rowsize, int height){
	frame_data += frame_pitch * (height-1);
	for(int y = 0; y < height; y++){
		memcpy(image_data, frame_data, rowsize);
		frame_data -= frame_pitch;
		image_data += image_stride;
	}
}

inline void image_to_avisynth_frame(unsigned char *image_data, int image_stride, unsigned char *frame_data, int frame_pitch, int rowsize, int height){
	frame_data += frame_pitch * (height-1);
	for(int y = 0; y < height; y++){
		memcpy(frame_data, image_data, rowsize);
		image_data += image_stride;
		frame_data -= frame_pitch;
	}
}*/