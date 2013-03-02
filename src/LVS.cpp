#include "LVS.hpp"
// TEST START
#include "cairo.hpp"
// TEST END

LVS::LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames,
				const char* audio_file, int channels, int sample_rate, __int64 samples,
				void* avs_env){

}

LVS::LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames){

}

void LVS::RenderOnFrame(unsigned char* data, int frame_number){
	// TEST START
	cairo_surface_t *surf = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_RGB24, 400, 200, 1600);
	cairo_t *ctx = cairo_create(surf);

	cairo_rectangle(ctx, 20, 20, 50, 50);
	cairo_set_source_rgb(ctx, 1, 1, 0.5);
	cairo_fill(ctx);

	cairo_destroy(ctx);
	cairo_surface_destroy(surf);
	// TEST END
}

void LVS::RenderOnSamples(float* buff, __int64 start, __int64 count){

}