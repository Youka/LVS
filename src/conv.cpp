#include "conv.hpp"
#include <windows.h>

// Text conversion
char* utf16_to_utf8(const wchar_t *wc){
    int c_len = WideCharToMultiByte(CP_UTF8, 0, wc, wcslen(wc)+1, 0, 0, 0, 0);
    char *c = new char[c_len];
    WideCharToMultiByte(CP_UTF8, 0, wc, wcslen(wc)+1, c, c_len, 0, 0);
    return c;
}

wchar_t* utf8_to_utf16(const char *c){
    int w_len = MultiByteToWideChar(CP_UTF8, 0, c, strlen(c)+1, 0, 0);
    wchar_t *wc = new wchar_t[w_len];
    MultiByteToWideChar(CP_UTF8, 0, c, strlen(c)+1, wc, w_len);
    return wc;
}

// Image conversion
void avisynth_frame_to_image(unsigned char *frame_data, int frame_pitch, unsigned char *image_data, int image_stride, int rowsize, int height){
	frame_data += frame_pitch * (height-1);
	for(int y = 0; y < height; y++){
		memcpy(image_data, frame_data, rowsize);
		frame_data -= frame_pitch;
		image_data += image_stride;
	}
}

void image_to_avisynth_frame(unsigned char *image_data, int image_stride, unsigned char *frame_data, int frame_pitch, int rowsize, int height){
	frame_data += frame_pitch * (height-1);
	for(int y = 0; y < height; y++){
		memcpy(frame_data, image_data, rowsize);
		image_data += image_stride;
		frame_data -= frame_pitch;
	}
}

void vdub_frame_to_image(){

}

void image_to_vdub_frame(){

}