#include "LVS.hpp"
#include "cairo.hpp"

LVS::LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames,
				const char* audio_file, int channels, int sample_rate, __int64 samples,
				void* avs_env){

}

LVS::LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames){

}

void LVS::RenderOnFrame(unsigned char* data, int frame_number){
	// TEST START
	cairo_surface_t *surf = cairo_image_surface_create_for_data(data, CAIRO_FORMAT_RGB24, 200, 100, 800);
	cairo_t *ctx = cairo_create(surf);

	cairo_set_line_width(ctx, 10);
	cairo_set_line_cap(ctx, CAIRO_LINE_CAP_ROUND);
	cairo_move_to(ctx, 20, 20);
	cairo_rel_line_to(ctx, 50, 0);
	cairo_stroke(ctx);

	cairo_destroy(ctx);
	cairo_surface_destroy(surf);
	// TEST END
}

void LVS::RenderOnSamples(__int16* buff, __int64 start, __int64 count){

}