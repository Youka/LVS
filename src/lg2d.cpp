#include "llibs.hpp"
#include "cairo.hpp"
#include "textconv.hpp"
#include <memory>
#define M_PI       3.14159265358979323846	// From "math.h"

// Objects names
#define G2D_IMAGE "G2D_IMAGE"
#define G2D_MATRIX "G2D_MATRIX"
#define G2D_SOURCE "G2D_SOURCE"
#define G2D_CONTEXT "G2D_CONTEXT"

// Type<->string converters
inline cairo_format_t cairo_format_from_string(const char *format_string){
	if(strcmp(format_string, "ARGB") == 0)
		return CAIRO_FORMAT_ARGB32;
	else if(strcmp(format_string, "RGB") == 0)
		return CAIRO_FORMAT_RGB24;
	else if(strcmp(format_string, "ALPHA") == 0)
		return CAIRO_FORMAT_A8;
	else if(strcmp(format_string, "BINARY") == 0)
		return CAIRO_FORMAT_A1;
	else
		return CAIRO_FORMAT_INVALID;
}

inline const char* cairo_format_to_string(cairo_format_t format){
	switch(format){
		case CAIRO_FORMAT_ARGB32: return "ARGB";
		case CAIRO_FORMAT_RGB24: return "RGB";
		case CAIRO_FORMAT_A8: return "ALPHA";
		case CAIRO_FORMAT_A1: return "BINARY";
		default: return "UNKNOWN";
	}
}

inline cairo_extend_t cairo_extend_from_string(const char *extend_string){
	if(strcmp(extend_string, "NONE") == 0)
		return CAIRO_EXTEND_NONE;
	else if(strcmp(extend_string, "REPEAT") == 0)
		return CAIRO_EXTEND_REPEAT;
	else if(strcmp(extend_string, "MIRROR") == 0)
		return CAIRO_EXTEND_REFLECT;
	else if(strcmp(extend_string, "FLOW") == 0)
		return CAIRO_EXTEND_PAD;
	else
		return CAIRO_EXTEND_NONE;
}

inline const char* cairo_extend_to_string(cairo_extend_t extend){
	switch(extend){
		case CAIRO_EXTEND_NONE: return "NONE";
		case CAIRO_EXTEND_REPEAT: return "REPEAT";
		case CAIRO_EXTEND_REFLECT: return "MIRROR";
		case CAIRO_EXTEND_PAD: return "FLOW";
		default: return "UNKNOWN";
	}
}

inline cairo_filter_t cairo_filter_from_string(const char *filter_string){
	if(strcmp(filter_string, "FAST") == 0)
		return CAIRO_FILTER_FAST;
	else if(strcmp(filter_string, "GOOD") == 0)
		return CAIRO_FILTER_GOOD;
	else if(strcmp(filter_string, "BEST") == 0)
		return CAIRO_FILTER_BEST;
	else
		return CAIRO_FILTER_BEST;
}

inline const char* cairo_filter_to_string(cairo_filter_t filter){
	switch(filter){
		case CAIRO_FILTER_FAST: return "FAST";
		case CAIRO_FILTER_GOOD: return "GOOD";
		case CAIRO_FILTER_BEST: return "BEST";
		default: return "UNKNOWN";
	}
}

 inline const char* cairo_pattern_type_to_string(cairo_pattern_type_t type){
	 switch(type){
		case CAIRO_PATTERN_TYPE_SOLID: return "COLOR";
		case CAIRO_PATTERN_TYPE_LINEAR: return "LINEAR_GRADIENT";
		case CAIRO_PATTERN_TYPE_RADIAL: return "RADIAL_GRADIENT";
		case CAIRO_PATTERN_TYPE_MESH: return "MESH_GRADIENT";
		case  CAIRO_PATTERN_TYPE_SURFACE: return "IMAGE";
		default: return "UNKNOWN";
	 }
 }

// G2D LIBRARY
LUA_FUNC_1ARG(create_image, 3)
	// Get parameters
	cairo_format_t format = cairo_format_from_string(luaL_checkstring(L, 1));
	int width = luaL_checknumber(L, 2);
	int height = luaL_checknumber(L, 3);
	// Create image
	cairo_surface_t *surface = cairo_image_surface_create(format, width, height);
	cairo_status_t status = cairo_surface_status(surface);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_surface_destroy(surface);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = surface;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_sub_image, 5)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0 = luaL_checknumber(L, 2);
	int y0 = luaL_checknumber(L, 3);
	int x1 = luaL_checknumber(L, 4);
	int y1 = luaL_checknumber(L, 5);
	if(x0 < 0 || y0 < 0 ||
		x1 <= x0 || y1 <= y0 ||
		x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
		luaL_error2(L, "invalid area");
	// Create sub image
	cairo_surface_t *sub_surface = cairo_image_surface_create(cairo_image_surface_get_format(surface), x1-x0, y1-y0);
	cairo_t *ctx = cairo_create(sub_surface);
	cairo_set_source_surface(ctx, surface, x0, y0);
	cairo_paint(ctx);
	cairo_destroy(ctx);
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = sub_surface;
	return 1;
LUA_FUNC_END

cairo_status_t png_stream_reader(void *closure, unsigned char *data, unsigned int length){
	FILE *file = (FILE*)closure;
	if(fread(data, 1, length, file) == length)
		return CAIRO_STATUS_SUCCESS;
	else
		return CAIRO_STATUS_READ_ERROR;
}
LUA_FUNC_1ARG(create_image_from_png, 1)
	// Get parameter
	const char *filename = luaL_checkstring(L, 1);
	// Create image from png
	wchar_t *filenamew = utf8_to_utf16(filename);
	FILE *file = _wfopen(filenamew, L"rb");
	delete[] filenamew;
	if(!file)
		luaL_error2(L, "file not found");
	cairo_surface_t *surface = cairo_image_surface_create_from_png_stream(png_stream_reader, file);
	fclose(file);
	cairo_status_t status = cairo_surface_status(surface);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_surface_destroy(surface);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = surface;
	return 1;
LUA_FUNC_END

cairo_status_t png_stream_writer(void *closure, const unsigned char *data, unsigned int length){
	FILE *file = (FILE*)closure;
	if(fwrite(data, 1, length, file) == length)
		return CAIRO_STATUS_SUCCESS;
	else
		return CAIRO_STATUS_WRITE_ERROR;
}
LUA_FUNC_1ARG(create_png_from_image, 2)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	const char *filename = luaL_checkstring(L, 2);
	// Create png from image
	wchar_t *filenamew = utf8_to_utf16(filename);
	FILE *file = _wfopen(filenamew, L"wb");
	delete[] filenamew;
	if(!file)
		luaL_error2(L, "couldn't create file");
	cairo_status_t status = cairo_surface_write_to_png_stream(surface, png_stream_writer, file);
	fclose(file);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(create_context, 1)
	// Get parameter
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	// Create context
	cairo_t *ctx = cairo_create(surface);
	cairo_status_t status = cairo_status(ctx);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_destroy(ctx);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push context to Lua
	*lua_createuserdata<cairo_t*>(L, G2D_CONTEXT) = ctx;
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(create_matrix, 0, 6)
	if(lua_gettop(L) == 0){
		// Create matrix
		cairo_matrix_t matrix = {1, 0, 0, 1, 0, 0};
		// Push matrix to Lua
		*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	}else if(lua_gettop(L) == 6){
		// Create matrix
		cairo_matrix_t matrix = {luaL_checknumber(L, 1), luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6)};
		// Push matrix to Lua
		*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	}
	return 1;
LUA_FUNC_END

LUA_FUNC_2ARG(create_source_color, 3, 4)
	// Get parameters
	double r = luaL_checknumber(L, 1);
	double g = luaL_checknumber(L, 2);
	double b = luaL_checknumber(L, 3);
	double a = luaL_optnumber(L, 4, 1.0L);
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_rgba(r, g, b, a);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_source_linear_gradient, 4)
	// Get parameters
	double x0 = luaL_checknumber(L, 1);
	double y0 = luaL_checknumber(L, 2);
	double x1 = luaL_checknumber(L, 3);
	double y1 = luaL_checknumber(L, 4);
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_linear(x0, y0, x1, y1);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_source_radial_gradient, 6)
	// Get parameters
	double cx0 = luaL_checknumber(L, 1);
	double cy0 = luaL_checknumber(L, 2);
	double r0 = luaL_checknumber(L, 3);
	double cx1 = luaL_checknumber(L, 4);
	double cy1 = luaL_checknumber(L, 5);
	double r1 = luaL_checknumber(L, 6);
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_radial(cx0, cy0, r0, cx1, cy1, r1);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_source_mesh_gradient, 0)
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_mesh();
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(create_source_image, 1)
	// Get parameter
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	// Create source
	cairo_pattern_t *pattern = cairo_pattern_create_for_surface(surface);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS){
		cairo_pattern_destroy(pattern);
		luaL_error2(L, cairo_status_to_string(status));
	}
	// Push source to Lua
	*lua_createuserdata<cairo_pattern_t*>(L, G2D_SOURCE) = pattern;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(image_convolution, 2)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	size_t filter_size;
	double *convolution_filter = luaL_checktable<double>(L, 2, &filter_size);
	std::auto_ptr<double> convolution_filter_obj(convolution_filter);
	lua_getfield(L, 2, "width");
	if(!lua_isnumber(L, -1)) luaL_error2(L, "table needs a valid field 'width'");
	int filter_width = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 2, "height");
	if(!lua_isnumber(L, -1)) luaL_error2(L, "table needs a valid field 'height'");
	int filter_height = lua_tonumber(L, -1);
	lua_pop(L, 1);
	if(filter_width < 1 || !filter_width&0x1 || filter_height < 1 || !filter_height&0x1)
		luaL_error2(L, "table field(s) invalid");
	else if(filter_width * filter_height != filter_size)
		luaL_error2(L, "table fields and elements number don't fit");
	// Get image data
	cairo_format_t image_format = cairo_image_surface_get_format(surface);
	int image_width = cairo_image_surface_get_width(surface);
	int image_height = cairo_image_surface_get_height(surface);
	int image_stride = cairo_image_surface_get_stride(surface);
	unsigned long image_data_size = image_height * image_stride;
	cairo_surface_flush(surface);
	register unsigned char *image_data = cairo_image_surface_get_data(surface);
	// Apply convolution filter to image
	switch(image_format){
		case CAIRO_FORMAT_ARGB32:{
			// Image data copy (use copy as source, original as destination)
			register unsigned char *image_data_copy = new unsigned char[image_data_size];
			memcpy(image_data_copy, image_data, image_data_size);
			// Storages for pixel processing
			unsigned char *src_pixel, *dst_pixel;
			double accum_r, accum_g, accum_b, accum_a;
			int image_x, image_y;
			double convolution;
			// Iterate through source image pixels
			for(register int y = 0; y < image_height; y++)
				for(register int x = 0; x < image_width; x++){
					// Accumulate pixels by filter rule
					accum_r = accum_g = accum_b = accum_a = 0;
					for(int yy = 0; yy < filter_height; yy++)
						for(int xx = 0; xx < filter_width; xx++){
							image_x = x - (filter_width >> 1) + xx;
							image_y = y - (filter_height >> 1) + yy;
							if(image_x >= 0 && image_x < image_width && image_y >= 0 && image_y < image_height){
								convolution = convolution_filter[yy * filter_width + xx];
								src_pixel = image_data_copy + image_y * image_stride + (image_x << 2);
								accum_b += src_pixel[0] * convolution;
								accum_g += src_pixel[1] * convolution;
								accum_r += src_pixel[2] * convolution;
								accum_a += src_pixel[3] * convolution;
							}
						}
					// Set accumulator to destination image pixel
					dst_pixel = image_data + y * image_stride + (x << 2);
					dst_pixel[0] = accum_b > 255 ? 255 : (accum_b < 0 ? 0 : accum_b);
					dst_pixel[1] = accum_g > 255 ? 255 : (accum_g < 0 ? 0 : accum_g);
					dst_pixel[2] = accum_r > 255 ? 255 : (accum_r < 0 ? 0 : accum_r);
					dst_pixel[3] = accum_a > 255 ? 255 : (accum_a < 0 ? 0 : accum_a);
				}
			// Free image data copy
			delete[] image_data_copy;
		}break;
		case CAIRO_FORMAT_RGB24:{
			// Image data copy (use copy as source, original as destination)
			register unsigned char *image_data_copy = new unsigned char[image_data_size];
			memcpy(image_data_copy, image_data, image_data_size);
			// Storages for pixel processing
			unsigned char *src_pixel, *dst_pixel;
			double accum_r, accum_g, accum_b;
			int image_x, image_y;
			double convolution;
			// Iterate through source image pixels
			for(register int y = 0; y < image_height; y++)
				for(register int x = 0; x < image_width; x++){
					// Accumulate pixels by filter rule
					accum_r = accum_g = accum_b = 0;
					for(int yy = 0; yy < filter_height; yy++)
						for(int xx = 0; xx < filter_width; xx++){
							image_x = x - (filter_width >> 1) + xx;
							image_y = y - (filter_height >> 1) + yy;
							if(image_x >= 0 && image_x < image_width && image_y >= 0 && image_y < image_height){
								convolution = convolution_filter[yy * filter_width + xx];
								src_pixel = image_data_copy + image_y * image_stride + (image_x << 2);
								accum_b += src_pixel[0] * convolution;
								accum_g += src_pixel[1] * convolution;
								accum_r += src_pixel[2] * convolution;
							}
						}
					// Set accumulator to destination image pixel
					dst_pixel = image_data + y * image_stride + (x << 2);
					dst_pixel[0] = accum_b > 255 ? 255 : (accum_b < 0 ? 0 : accum_b);
					dst_pixel[1] = accum_g > 255 ? 255 : (accum_g < 0 ? 0 : accum_g);
					dst_pixel[2] = accum_r > 255 ? 255 : (accum_r < 0 ? 0 : accum_r);
				}
			// Free image data copy
			delete[] image_data_copy;
		}break;
		case CAIRO_FORMAT_A8:{
			// Image data copy (use copy as source, original as destination)
			register unsigned char *image_data_copy = new unsigned char[image_data_size];
			memcpy(image_data_copy, image_data, image_data_size);
			// Storages for pixel processing
			double accum;
			int image_x, image_y;
			// Iterate through source image pixels
			for(register int y = 0; y < image_height; y++)
				for(register int x = 0; x < image_width; x++){
					// Accumulate pixels by filter rule
					accum = 0;
					for(int yy = 0; yy < filter_height; yy++)
						for(int xx = 0; xx < filter_width; xx++){
							image_x = x - (filter_width >> 1) + xx;
							image_y = y - (filter_height >> 1) + yy;
							if(image_x >= 0 && image_x < image_width && image_y >= 0 && image_y < image_height)
								accum += image_data_copy[image_y * image_stride + image_x] * convolution_filter[yy * filter_width + xx];
						}
					// Set accumulator to destination image pixel
					image_data[y * image_stride + x] = accum > 255 ? 255 : (accum < 0 ? 0 : accum);
				}
			// Free image data copy
			delete[] image_data_copy;
		}break;
		default:
			luaL_error2(L, "image format not supported");
	}
	// Set image data as dirty
	cairo_surface_mark_dirty(surface);
LUA_FUNC_END

LUA_FUNC_2ARG(text_extents, 3, 7)
	// Get parameters
	const char *text = luaL_checkstring(L, 1);
	const char *face = luaL_checkstring(L, 2);
	int size = luaL_checknumber(L, 3);
	bool bold = luaL_optboolean(L, 4, false);
	bool italic = luaL_optboolean(L, 5, false);
	bool underline = luaL_optboolean(L, 6, false);
	bool strikeout = luaL_optboolean(L, 7, false);
	// Get text extents
	wchar_t *textw = utf8_to_utf16(text), *facew = utf8_to_utf16(face);
	cairo_win32_text_extents_t extents = cairo_win32_text_extents(textw, facew, size, bold, italic, underline, strikeout);
	delete[] textw; delete[] facew;
	// Push text extents to Lua
	lua_pushnumber(L, extents.width);
	lua_pushnumber(L, extents.height);
	lua_pushnumber(L, extents.ascent);
	lua_pushnumber(L, extents.descent);
	lua_pushnumber(L, extents.internal_leading);
	lua_pushnumber(L, extents.external_leading);
	return 6;
LUA_FUNC_END

// IMAGE OBJECT
LUA_FUNC_1ARG(image_gc, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	cairo_surface_destroy(surface);
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_width, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	lua_pushnumber(L, cairo_image_surface_get_width(surface));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_height, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	lua_pushnumber(L, cairo_image_surface_get_height(surface));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_format, 1)
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	lua_pushstring(L, cairo_format_to_string(cairo_image_surface_get_format(surface)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(image_get_data, 5)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0 = luaL_checknumber(L, 2);
	int y0 = luaL_checknumber(L, 3);
	int x1 = luaL_checknumber(L, 4);
	int y1 = luaL_checknumber(L, 5);
	if(x0 < 0 || y0 < 0 ||
		x1 <= x0 || y1 <= y0 ||
		x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
		luaL_error2(L, "invalid area");
	// Get image data
	cairo_format_t image_format = cairo_image_surface_get_format(surface);
	int image_stride = cairo_image_surface_get_stride(surface);
	cairo_surface_flush(surface);
	register unsigned char *image_data = cairo_image_surface_get_data(surface);
	// Further area data
	int area_width = x1-x0;
	int area_height = y1-y0;
	// Create & return image data table of image area
	switch(image_format){
		case CAIRO_FORMAT_ARGB32:{
			lua_createtable(L, area_width * area_height << 2, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
					lua_pushnumber(L, row[3]); lua_rawseti(L, -2, ++table_index);	// A
					lua_pushnumber(L, row[2]); lua_rawseti(L, -2, ++table_index);	// R
					lua_pushnumber(L, row[1]); lua_rawseti(L, -2, ++table_index);	// G
					lua_pushnumber(L, row[0]); lua_rawseti(L, -2, ++table_index);	// B
					row += 4;
				}
			}
			return 1;
		}break;
		case CAIRO_FORMAT_RGB24:{
			lua_createtable(L, area_width * area_height * 3, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
					lua_pushnumber(L, row[2]); lua_rawseti(L, -2, ++table_index);	// R
					lua_pushnumber(L, row[1]); lua_rawseti(L, -2, ++table_index);	// G
					lua_pushnumber(L, row[0]); lua_rawseti(L, -2, ++table_index);	// B
					row += 4;
				}
			}
			return 1;
		}break;
		case CAIRO_FORMAT_A8:{
			lua_createtable(L, area_width * area_height, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + x0;
				for(int x = 0; x < area_width; x++){
					lua_pushnumber(L, *row++); lua_rawseti(L, -2, ++table_index);	// A
				}
			}
			return 1;
		}break;
		case CAIRO_FORMAT_A1:{
			lua_createtable(L, area_width * area_height, 0);
			int table_index = 0;
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride;
				for(int x = x0; x < x1; x++){
					div_t quot_rem = div(x, 8);
					lua_pushnumber(L, row[quot_rem.quot] >> quot_rem.rem & 0x1); lua_rawseti(L, -2, ++table_index);	// A
				}
			}
			return 1;
		}break;
	}
LUA_FUNC_END

LUA_FUNC_1ARG(image_set_data, 6)
	// Get parameters
	cairo_surface_t *surface = *reinterpret_cast<cairo_surface_t**>(luaL_checkuserdata(L, 1, G2D_IMAGE));
	int x0 = luaL_checknumber(L, 2);
	int y0 = luaL_checknumber(L, 3);
	int x1 = luaL_checknumber(L, 4);
	int y1 = luaL_checknumber(L, 5);
	if(x0 < 0 || y0 < 0 ||
		x1 <= x0 || y1 <= y0 ||
		x1 > cairo_image_surface_get_width(surface) || y1 > cairo_image_surface_get_height(surface))
		luaL_error2(L, "invalid area");
	size_t new_data_size;
	unsigned char *new_data = luaL_checktable<unsigned char>(L, 6, &new_data_size);
	std::auto_ptr<unsigned char> new_data_obj(new_data);
	// Get image data
	cairo_format_t image_format = cairo_image_surface_get_format(surface);
	int image_stride = cairo_image_surface_get_stride(surface);
	cairo_surface_flush(surface);
	register unsigned char *image_data = cairo_image_surface_get_data(surface);
	// Further area data
	int area_width = x1-x0;
	int area_height = y1-y0;
	// Set image data table to image area
	switch(image_format){
		case CAIRO_FORMAT_ARGB32:{
			if(area_width * area_height << 2 != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
					row[3] = *new_data++;	// A
					row[2] = *new_data++;	// R
					row[1] = *new_data++;	// G
					row[0] = *new_data++;	// B
					row += 4;
				}
			}
		}break;
		case CAIRO_FORMAT_RGB24:{
			if(area_width * area_height * 3 != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + (x0 << 2);
				for(int x = 0; x < area_width; x++){
					row[2] = *new_data++;	// R
					row[1] = *new_data++;	// G
					row[0] = *new_data++;	// B
					row += 4;
				}
			}
		}break;
		case CAIRO_FORMAT_A8:{
			if(area_width * area_height != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride + x0;
				for(int x = 0; x < area_width; x++)
					*row++ = *new_data++;	// A
			}
		}break;
		case CAIRO_FORMAT_A1:{
			if(area_width * area_height != new_data_size)
				luaL_error2(L, "wrong table size");
			unsigned char *row, dst, mask, src;
			for(int y = y0; y < y1; y++){
				row = image_data + y * image_stride;
				for(int x = x0; x < x1; x++){
					div_t quot_rem = div(x, 8);
					mask = 0x1 << quot_rem.rem;
					src = *new_data++;
					dst = row[quot_rem.quot] & mask;
					if(src && !dst)
						row[quot_rem.quot] |= mask;	// A
					else if(!src && dst)
						row[quot_rem.quot] &= ~mask;	// A
				}
			}
		}break;
	}
	// Set image data as dirty
	cairo_surface_mark_dirty_rectangle(surface, x0, y0, area_width, area_height);
LUA_FUNC_END

// MATRIX OBJECT
LUA_FUNC_1ARG(matrix_get_data, 1)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	lua_pushnumber(L, matrix->xx);
	lua_pushnumber(L, matrix->yx);
	lua_pushnumber(L, matrix->xy);
	lua_pushnumber(L, matrix->yy);
	lua_pushnumber(L, matrix->x0);
	lua_pushnumber(L, matrix->y0);
	return 6;
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_set_data, 7)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	matrix->xx = luaL_checknumber(L, 2);
	matrix->yx = luaL_checknumber(L, 3);
	matrix->xy = luaL_checknumber(L, 4);
	matrix->yy = luaL_checknumber(L, 5);
	matrix->x0 = luaL_checknumber(L, 6);
	matrix->y0 = luaL_checknumber(L, 7);
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_identity, 1)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_init_identity(matrix);
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_invert, 1)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_status_t status = cairo_matrix_invert(matrix);
	if(status != CAIRO_STATUS_SUCCESS)
				luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_multiply, 7)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_t a = {luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7)};
	cairo_matrix_t b = *matrix;
	cairo_matrix_multiply(matrix, &a, &b);
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_translate, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_translate(matrix, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_scale, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_scale(matrix, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_rotate, 2)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_rotate(matrix, luaL_checknumber(L, 2) / 180.0L * M_PI);
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_shear, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	cairo_matrix_t a = {1, luaL_checknumber(L, 2), luaL_checknumber(L, 3), 1, 0, 0};
	cairo_matrix_t b = *matrix;
	cairo_matrix_multiply(matrix, &a, &b);
LUA_FUNC_END

LUA_FUNC_1ARG(matrix_transform, 3)
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 1, G2D_MATRIX));
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_transform_point(matrix, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
LUA_FUNC_END

// SOURCE OBJECT
LUA_FUNC_1ARG(source_gc, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_destroy(pattern);
LUA_FUNC_END

LUA_FUNC_2ARG(source_add_color, 5, 6)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_add_color_stop_rgba(pattern, luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_optnumber(L, 6, 1.0L));
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_2ARG(source_get_color, 1, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	// Get color of source color
	if(lua_gettop(L) == 1){
		double r, g, b, a;
		cairo_status_t status = cairo_pattern_get_rgba(pattern, &r, &g, &b, &a);
		if(status != CAIRO_STATUS_SUCCESS)
			luaL_error2(L, cairo_status_to_string(status));
		lua_pushnumber(L, r);
		lua_pushnumber(L, g);
		lua_pushnumber(L, b);
		lua_pushnumber(L, a);
		return 4;
	// Get color of source linear gradient or source radial gradient
	}else{
		double pct, r, g, b, a;
		cairo_status_t status = cairo_pattern_get_color_stop_rgba(pattern, luaL_checknumber(L, 2), &pct, &r, &g, &b, &a);
		if(status == CAIRO_STATUS_INVALID_INDEX)
			return 0;
		else if(status != CAIRO_STATUS_SUCCESS)
			luaL_error2(L, cairo_status_to_string(status));
		lua_pushnumber(L, pct);
		lua_pushnumber(L, r);
		lua_pushnumber(L, g);
		lua_pushnumber(L, b);
		lua_pushnumber(L, a);
		return 5;
	}
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_points, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	double x0, y0, x1, y1;
	cairo_status_t status = cairo_pattern_get_linear_points(pattern, &x0, &y0, &x1, &y1);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	lua_pushnumber(L, x0);
	lua_pushnumber(L, y0);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	return 4;
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_circles, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	double x0, y0, r0, x1, y1, r1;
	cairo_status_t status = cairo_pattern_get_radial_circles(pattern, &x0, &y0, &r0, &x1, &y1, &r1);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	lua_pushnumber(L, x0);
	lua_pushnumber(L, y0);
	lua_pushnumber(L, r0);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, r1);
	return 6;
LUA_FUNC_END

LUA_FUNC_2ARG(source_add_mesh, 19, 25)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_mesh_pattern_begin_patch(pattern);
	cairo_mesh_pattern_move_to(pattern, luaL_checknumber(L, 2), luaL_checknumber(L, 3));
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 0, luaL_checknumber(L, 4), luaL_checknumber(L, 5), luaL_checknumber(L, 6), luaL_checknumber(L, 7));
	cairo_mesh_pattern_line_to(pattern, luaL_checknumber(L, 8), luaL_checknumber(L, 9));
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 1, luaL_checknumber(L, 10), luaL_checknumber(L, 11), luaL_checknumber(L, 12), luaL_checknumber(L, 13));
	cairo_mesh_pattern_line_to(pattern, luaL_checknumber(L, 14), luaL_checknumber(L, 15));
	cairo_mesh_pattern_set_corner_color_rgba(pattern, 2, luaL_checknumber(L, 16), luaL_checknumber(L, 17), luaL_checknumber(L, 18), luaL_checknumber(L, 19));
	if(lua_gettop(L) == 25){
		cairo_mesh_pattern_line_to(pattern, luaL_checknumber(L, 20), luaL_checknumber(L, 21));
		cairo_mesh_pattern_set_corner_color_rgba(pattern, 3, luaL_checknumber(L, 22), luaL_checknumber(L, 23), luaL_checknumber(L, 24), luaL_checknumber(L, 25));
	}
	cairo_mesh_pattern_end_patch(pattern);
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_mesh, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	unsigned int index = luaL_checknumber(L, 2);
	// Get mesh points
	cairo_path_t *path = cairo_mesh_pattern_get_path(pattern, index);
	if(path->status == CAIRO_STATUS_INVALID_INDEX)
		return 0;
	else if(path->status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(path->status));
	// Get mesh colors
	double r0, g0, b0, a0, r1, g1, b1, a1, r2, g2, b2, a2, r3, g3, b3, a3;
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 0, &r0, &g0, &b0, &a0);
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 1, &r1, &g1, &b1, &a1);
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 2, &r2, &g2, &b2, &a2);
	cairo_mesh_pattern_get_corner_color_rgba(pattern, index, 3, &r3, &g3, &b3, &a3);
	// Push points and colors to Lua
	lua_pushnumber(L, path->data[1].point.x);
	lua_pushnumber(L, path->data[1].point.y);
	lua_pushnumber(L, r0);
	lua_pushnumber(L, g0);
	lua_pushnumber(L, b0);
	lua_pushnumber(L, a0);
	lua_pushnumber(L, path->data[5].point.x);
	lua_pushnumber(L, path->data[5].point.y);
	lua_pushnumber(L, r1);
	lua_pushnumber(L, g1);
	lua_pushnumber(L, b1);
	lua_pushnumber(L, a1);
	lua_pushnumber(L, path->data[9].point.x);
	lua_pushnumber(L, path->data[9].point.y);
	lua_pushnumber(L, r2);
	lua_pushnumber(L, g2);
	lua_pushnumber(L, b2);
	lua_pushnumber(L, a2);
	lua_pushnumber(L, path->data[13].point.x);
	lua_pushnumber(L, path->data[13].point.y);
	lua_pushnumber(L, r3);
	lua_pushnumber(L, g3);
	lua_pushnumber(L, b3);
	lua_pushnumber(L, a3);
	return 24;
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_image, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_surface_t *surface;
	cairo_status_t status = cairo_pattern_get_surface(pattern, &surface);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	// Create image copy
	cairo_surface_t *new_surface = cairo_image_surface_create(cairo_image_surface_get_format(surface), cairo_image_surface_get_width(surface), cairo_image_surface_get_height(surface));
	cairo_surface_flush(surface); cairo_surface_flush(new_surface);
	unsigned char *surface_data = cairo_image_surface_get_data(surface), *new_surface_data = cairo_image_surface_get_data(new_surface);
	memcpy(new_surface_data, surface_data, cairo_image_surface_get_height(surface) * cairo_image_surface_get_stride(surface));
	cairo_surface_mark_dirty(new_surface);
	// Push image to Lua
	*lua_createuserdata<cairo_surface_t*>(L, G2D_IMAGE) = new_surface;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_extender, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	lua_pushstring(L, cairo_extend_to_string(cairo_pattern_get_extend(pattern)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_set_extender, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_set_extend(pattern, cairo_extend_from_string(luaL_checkstring(L, 2)));
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_filter, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	lua_pushstring(L, cairo_filter_to_string(cairo_pattern_get_filter(pattern)));
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_set_filter, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_set_filter(pattern, cairo_filter_from_string(luaL_checkstring(L, 2)));
	cairo_status_t status = cairo_pattern_status(pattern);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_matrix, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	// Get matrix
	cairo_matrix_t matrix;
	cairo_pattern_get_matrix(pattern, &matrix);
	// Push matrix to Lua
	*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	return 1;
LUA_FUNC_END

LUA_FUNC_1ARG(source_set_matrix, 2)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_matrix_t *matrix = reinterpret_cast<cairo_matrix_t*>(luaL_checkuserdata(L, 2, G2D_MATRIX));
	// Create inverted matrix for pattern space to user space (see 'cairo_pattern_set_matrix' description)
	cairo_matrix_t inverted_matrix = *matrix;
	cairo_status_t status = cairo_matrix_invert(&inverted_matrix);
	if(status != CAIRO_STATUS_SUCCESS)
		luaL_error2(L, cairo_status_to_string(status));
	// Set matrix to pattern
	cairo_pattern_set_matrix(pattern, &inverted_matrix);
LUA_FUNC_END

LUA_FUNC_1ARG(source_get_type, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	lua_pushstring(L, cairo_pattern_type_to_string(cairo_pattern_get_type(pattern)));
	return 1;
LUA_FUNC_END

// CONTEXT OBJECT
LUA_FUNC_1ARG(context_gc, 1)
	cairo_t *ctx = *reinterpret_cast<cairo_t**>(luaL_checkuserdata(L, 1, G2D_CONTEXT));
	cairo_destroy(ctx);
LUA_FUNC_END

// Register
int luaopen_g2d(lua_State *L){
	// Create 'g2d' table
	lua_newtable(L);
	// Register 'g2d' functions to table
	const luaL_Reg g2d_lib[] = {
		"create_image", l_create_image,
		"create_sub_image", l_create_sub_image,
		"create_image_from_png", l_create_image_from_png,
		"create_png_from_image", l_create_png_from_image,
		"create_context", l_create_context,
		"create_matrix", l_create_matrix,
		"create_source_color", l_create_source_color,
		"create_source_linear_gradient", l_create_source_linear_gradient,
		"create_source_radial_gradient", l_create_source_radial_gradient,
		"create_source_mesh_gradient", l_create_source_mesh_gradient,
		"create_source_image", l_create_source_image,
		"image_convolution", l_image_convolution,
		"text_extents", l_text_extents,
		0, 0
	};
	luaL_setfuncs(L, g2d_lib, 0);
	// Set table to  global name 'g2d'
	lua_setglobal(L, "g2d");
	// Define image object methods
	luaL_newmetatable(L, G2D_IMAGE);
	lua_pushstring(L, G2D_IMAGE); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_image_gc); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_image_get_width); lua_setfield(L, -2, "get_width");
	lua_pushcfunction(L, l_image_get_height); lua_setfield(L, -2, "get_height");
	lua_pushcfunction(L, l_image_get_format); lua_setfield(L, -2, "get_format");
	lua_pushcfunction(L, l_image_get_data); lua_setfield(L, -2, "get_data");
	lua_pushcfunction(L, l_image_set_data); lua_setfield(L, -2, "set_data");
	lua_pop(L, 1);
	// Define matrix object methods
	luaL_newmetatable(L, G2D_MATRIX);
	lua_pushstring(L, G2D_MATRIX); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_matrix_get_data); lua_setfield(L, -2, "get_data");
	lua_pushcfunction(L, l_matrix_set_data); lua_setfield(L, -2, "set_data");
	lua_pushcfunction(L, l_matrix_identity); lua_setfield(L, -2, "identity");
	lua_pushcfunction(L, l_matrix_invert); lua_setfield(L, -2, "invert");
	lua_pushcfunction(L, l_matrix_multiply); lua_setfield(L, -2, "multiply");
	lua_pushcfunction(L, l_matrix_translate); lua_setfield(L, -2, "translate");
	lua_pushcfunction(L, l_matrix_scale); lua_setfield(L, -2, "scale");
	lua_pushcfunction(L, l_matrix_rotate); lua_setfield(L, -2, "rotate");
	lua_pushcfunction(L, l_matrix_shear); lua_setfield(L, -2, "shear");
	lua_pushcfunction(L, l_matrix_transform); lua_setfield(L, -2, "transform");
	lua_pop(L, 1);
	// Define source object methods
	luaL_newmetatable(L, G2D_SOURCE);
	lua_pushstring(L, G2D_SOURCE); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_source_gc); lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, l_source_add_color); lua_setfield(L, -2, "add_color");
	lua_pushcfunction(L, l_source_get_color); lua_setfield(L, -2, "get_color");
	lua_pushcfunction(L, l_source_get_points); lua_setfield(L, -2, "get_points");
	lua_pushcfunction(L, l_source_get_circles); lua_setfield(L, -2, "get_circles");
	lua_pushcfunction(L, l_source_add_mesh); lua_setfield(L, -2, "add_mesh");
	lua_pushcfunction(L, l_source_get_mesh); lua_setfield(L, -2, "get_mesh");
	lua_pushcfunction(L, l_source_get_image); lua_setfield(L, -2, "get_image");
	lua_pushcfunction(L, l_source_get_extender); lua_setfield(L, -2, "get_extender");
	lua_pushcfunction(L, l_source_set_extender); lua_setfield(L, -2, "set_extender");
	lua_pushcfunction(L, l_source_get_filter); lua_setfield(L, -2, "get_filter");
	lua_pushcfunction(L, l_source_set_filter); lua_setfield(L, -2, "set_filter");
	lua_pushcfunction(L, l_source_get_matrix); lua_setfield(L, -2, "get_matrix");
	lua_pushcfunction(L, l_source_set_matrix); lua_setfield(L, -2, "set_matrix");
	lua_pushcfunction(L, l_source_get_type); lua_setfield(L, -2, "get_type");
	lua_pop(L, 1);
	// Define context object methods
	luaL_newmetatable(L, G2D_CONTEXT);
	lua_pushstring(L, G2D_CONTEXT); lua_setfield(L, -2, "__metatable");
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_context_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	// Nothing pushed to Lua state
	return 0;
}