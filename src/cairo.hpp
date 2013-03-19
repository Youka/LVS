#pragma once

#define CAIRO_WIN32_STATIC_BUILD
#include <cairo/cairo.h>
#include <windows.h>

// Windows text extents structure
struct cairo_win32_text_extents_t{
	double width, height, ascent, descent, internal_leading, external_leading;
};
// Text extents in windows
static cairo_win32_text_extents_t cairo_win32_text_extents(const wchar_t *text, const wchar_t *face, int size,
													bool bold = false, bool italic = false, bool underline = false, bool strikeout = false,
													BYTE charset = DEFAULT_CHARSET){
	// Create windows context
	HDC dc = CreateCompatibleDC(NULL);
	SetMapMode(dc, MM_TEXT);	// Map for pixels
	// Create windows font (MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/dd145037%28v=vs.85%29.aspx)
	LOGFONTW lf = {0};	// Logfont with default values
	lf.lfHeight = size << 6;	// Font size (64-fold upscaled)
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;	// Bold or nomal weight?
	lf.lfItalic = italic;	// Italic?
	lf.lfUnderline = underline;	// Underlined?
	lf.lfStrikeOut = strikeout;	// Strikeouted?
	lf.lfCharSet = charset;	// Which charset?
	lf.lfOutPrecision = OUT_TT_PRECIS;	// TrueType fonts preferred for output precision
	lf.lfQuality = ANTIALIASED_QUALITY;	// Font antialiased
	lf.lfFaceName[31] = L'\0';	// Make sure the fontname string ends with terminating zero
	wcsncpy(lf.lfFaceName, face, 31);	// Copy until 31 characters of wished font face name to logfont
	HFONT font = CreateFontIndirect(&lf);
	SelectObject(dc, font);
	// Get text extents
	SIZE sz = {0};
	GetTextExtentPoint32W(dc, text, wcslen(text), &sz);
	// Get font metrics
	TEXTMETRICW metrics = {0};
	GetTextMetricsW(dc, &metrics);
	// Free windows objects
	DeleteObject(font);
	DeleteDC(dc);
	// Return text extents (8-fold downscaled)
	cairo_win32_text_extents_t text_extents = {
		static_cast<double>(sz.cx) / 64, static_cast<double>(sz.cy) / 64,
		static_cast<double>(metrics.tmAscent) / 64, static_cast<double>(metrics.tmDescent) / 64,
		static_cast<double>(metrics.tmInternalLeading) / 64, static_cast<double>(metrics.tmExternalLeading) / 64};
	return text_extents;
}

// Text path creator in windows for cairo
static void cairo_win32_text_path(cairo_t *ctx, const wchar_t *text, const wchar_t *face, int size,
													bool bold = false, bool italic = false, bool underline = false, bool strikeout = false,
													BYTE charset = DEFAULT_CHARSET){
	// Create windows context
	HDC dc = CreateCompatibleDC(NULL);
	SetMapMode(dc, MM_TEXT);	// Map for pixels
	SetBkMode(dc, TRANSPARENT);	// Draw text directly, not the stencil
	// Create windows font (MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/dd145037%28v=vs.85%29.aspx)
	LOGFONTW lf = {0};	// Logfont with default values
	lf.lfHeight = size << 6;	// Font size (64-fold upscaled)
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;	// Bold or nomal weight?
	lf.lfItalic = italic;	// Italic?
	lf.lfUnderline = underline;	// Underlined?
	lf.lfStrikeOut = strikeout;	// Strikeouted?
	lf.lfCharSet = charset;	// Which charset?
	lf.lfOutPrecision = OUT_TT_PRECIS;	// TrueType fonts preferred for output precision
	lf.lfQuality = ANTIALIASED_QUALITY;	// Font antialiased
	lf.lfFaceName[31] = L'\0';	// Make sure the fontname string ends with terminating zero
	wcsncpy(lf.lfFaceName, face, 31);	// Copy until 31 characters of wished font face name to logfont
	HFONT font = CreateFontIndirect(&lf);
	SelectObject(dc, font);
	// Draw text path to context
	BeginPath(dc);
	ExtTextOutW(dc, 0, 0, 0x0, NULL, text, wcslen(text), NULL);
	EndPath(dc);
	// Get context path
	POINT *points;
	BYTE *types;
	int points_n = GetPath(dc, NULL, NULL, 0);
	if(points_n > 0){
		points = new POINT[points_n];
		types = new BYTE[points_n];
		GetPath(dc, points, types, points_n);
	}
	// Free windows objects
	DeleteObject(font);
	DeleteDC(dc);
	// Convert windows path (64-fold downscaled) to cairo path
	int point_i = 0;
	while(point_i < points_n)
		switch(types[point_i]){
			case PT_MOVETO:
				cairo_close_path(ctx);
				cairo_move_to(ctx, static_cast<double>(points[point_i].x) / 64, static_cast<double>(points[point_i].y) / 64);
				point_i++;
				break;
			case PT_LINETO:
			case PT_LINETO | PT_CLOSEFIGURE:
				cairo_line_to(ctx, static_cast<double>(points[point_i].x) / 64, static_cast<double>(points[point_i].y) / 64);
				point_i++;
				break;
			case PT_BEZIERTO:
			case PT_BEZIERTO | PT_CLOSEFIGURE:
				cairo_curve_to(ctx,
											static_cast<double>(points[point_i].x) / 64, static_cast<double>(points[point_i].y) / 64,
											static_cast<double>(points[point_i+1].x) / 64, static_cast<double>(points[point_i+1].y) / 64,
											static_cast<double>(points[point_i+2].x) / 64, static_cast<double>(points[point_i+2].y) / 64);
				point_i += 3;
				break;
		}
	cairo_close_path(ctx);
	// Free path resources
	if(points_n > 0){
		delete[] points;
		delete[] types;
	}
}

// Thread data for functions below
struct THREAD_DATA{
	int image_width, image_height, image_stride, image_first_row, image_last_row;
	double *image_src;
	unsigned char *image_dst;
	int filter_width, filter_height;
	double *filter_kernel;
};
// Thread function for cairo image surface (ARGB32) convolution
static DWORD WINAPI cairo_image_surface_convolution_argb(void *userdata){
	THREAD_DATA *thread_data = reinterpret_cast<THREAD_DATA*>(userdata);
	// Storages for pixel processing
	double *src_pixel;
	unsigned char *dst_pixel;
	double accum_r, accum_g, accum_b, accum_a;
	int image_x, image_y;
	double convolution;
	// Iterate through source image pixels
	for(register int y = thread_data->image_first_row; y <= thread_data->image_last_row; y++)
		for(register int x = 0; x < thread_data->image_width; x++){
			// Accumulate pixels by filter rule
			accum_r = accum_g = accum_b = accum_a = 0;
			for(int yy = 0; yy < thread_data->filter_height; yy++){
				image_y = y - (thread_data->filter_height >> 1) + yy;
				if(image_y < 0 || image_y >= thread_data->image_height)
					continue;
				for(int xx = 0; xx < thread_data->filter_width; xx++){
					image_x = x - (thread_data->filter_width >> 1) + xx;
					if(image_x < 0 || image_x >= thread_data->image_width)
						continue;
					convolution = thread_data->filter_kernel[yy * thread_data->filter_width + xx];
					src_pixel = thread_data->image_src + image_y * thread_data->image_stride + (image_x << 2);
					accum_b += src_pixel[0] * convolution;
					accum_g += src_pixel[1] * convolution;
					accum_r += src_pixel[2] * convolution;
					accum_a += src_pixel[3] * convolution;
				}
			}
			// Set accumulator to destination image pixel
			dst_pixel = thread_data->image_dst + y * thread_data->image_stride + (x << 2);
			dst_pixel[0] = accum_b > 255 ? 255 : (accum_b < 0 ? 0 : accum_b);
			dst_pixel[1] = accum_g > 255 ? 255 : (accum_g < 0 ? 0 : accum_g);
			dst_pixel[2] = accum_r > 255 ? 255 : (accum_r < 0 ? 0 : accum_r);
			dst_pixel[3] = accum_a > 255 ? 255 : (accum_a < 0 ? 0 : accum_a);
		}
	return 0;
}
// Thread function for cairo image surface (RGB24) convolution
static DWORD WINAPI cairo_image_surface_convolution_rgb(void *userdata){
	THREAD_DATA *thread_data = reinterpret_cast<THREAD_DATA*>(userdata);
	// Storages for pixel processing
	double *src_pixel;
	unsigned char *dst_pixel;
	double accum_r, accum_g, accum_b;
	int image_x, image_y;
	double convolution;
	// Iterate through source image pixels
	for(register int y = thread_data->image_first_row; y <= thread_data->image_last_row; y++)
		for(register int x = 0; x < thread_data->image_width; x++){
			// Accumulate pixels by filter rule
			accum_r = accum_g = accum_b = 0;
			for(int yy = 0; yy < thread_data->filter_height; yy++){
				image_y = y - (thread_data->filter_height >> 1) + yy;
				if(image_y < 0 || image_y >= thread_data->image_height)
					continue;
				for(int xx = 0; xx < thread_data->filter_width; xx++){
					image_x = x - (thread_data->filter_width >> 1) + xx;
					if(image_x < 0 || image_x >= thread_data->image_width)
						continue;
					convolution = thread_data->filter_kernel[yy * thread_data->filter_width + xx];
					src_pixel = thread_data->image_src + image_y * thread_data->image_stride + (image_x << 2);
					accum_b += src_pixel[0] * convolution;
					accum_g += src_pixel[1] * convolution;
					accum_r += src_pixel[2] * convolution;
				}
			}
			// Set accumulator to destination image pixel
			dst_pixel = thread_data->image_dst + y * thread_data->image_stride + (x << 2);
			dst_pixel[0] = accum_b > 255 ? 255 : (accum_b < 0 ? 0 : accum_b);
			dst_pixel[1] = accum_g > 255 ? 255 : (accum_g < 0 ? 0 : accum_g);
			dst_pixel[2] = accum_r > 255 ? 255 : (accum_r < 0 ? 0 : accum_r);
		}
	return 0;
}
// Thread function for cairo image surface (A8) convolution
static DWORD WINAPI cairo_image_surface_convolution_a8(void *userdata){
	THREAD_DATA *thread_data = reinterpret_cast<THREAD_DATA*>(userdata);
	// Storages for pixel processing
	double accum;
	int image_x, image_y;
	// Iterate through source image pixels
	for(register int y = thread_data->image_first_row; y <= thread_data->image_last_row; y++)
		for(register int x = 0; x < thread_data->image_width; x++){
			// Accumulate pixels by filter rule
			accum = 0;
			for(int yy = 0; yy < thread_data->filter_height; yy++){
				image_y = y - (thread_data->filter_height >> 1) + yy;
				if(image_y < 0 || image_y >= thread_data->image_height)
					continue;
				for(int xx = 0; xx < thread_data->filter_width; xx++){
					image_x = x - (thread_data->filter_width >> 1) + xx;
					if(image_x < 0 || image_x >= thread_data->image_width)
						continue;
					accum += thread_data->image_src[image_y * thread_data->image_stride + image_x] * thread_data->filter_kernel[yy * thread_data->filter_width + xx];
				}
			}
			// Set accumulator to destination image pixel
			thread_data->image_dst[y * thread_data->image_stride + x] = accum > 255 ? 255 : (accum < 0 ? 0 : accum);
		}
	return 0;
}
