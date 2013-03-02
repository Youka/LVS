#pragma once

// Text
char* utf16_to_utf8(const wchar_t *wc);
wchar_t* utf8_to_utf16(const char *c);
// Image
void avisynth_frame_to_image(unsigned char *frame_data, int frame_pitch, unsigned char *image_data, int image_stride, int rowsize, int height);
void image_to_avisynth_frame(unsigned char *image_data, int image_stride, unsigned char *frame_data, int frame_pitch, int rowsize, int height);