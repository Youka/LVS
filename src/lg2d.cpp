#include "llibs.hpp"
#include "cairo.hpp"
#include "textconv.hpp"
#include <memory>

// Objects names
#define G2D_IMAGE "G2D_IMAGE"
#define G2D_MATRIX "G2D_MATRIX"
#define G2D_SOURCE "G2D_SOURCE"
#define G2D_CONTEXT "G2D_CONTEXT"

// Converters
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
		cairo_matrix_t matrix;
		cairo_matrix_init_identity(&matrix);
		// Push matrix to Lua
		*lua_createuserdata<cairo_matrix_t>(L, G2D_MATRIX) = matrix;
	}else if(lua_gettop(L) == 6){
		// Get parameters
		double xx = luaL_checknumber(L, 1);
		double yx = luaL_checknumber(L, 2);
		double xy = luaL_checknumber(L, 3);
		double yy = luaL_checknumber(L, 4);
		double x0 = luaL_checknumber(L, 5);
		double y0 = luaL_checknumber(L, 6);
		// Create matrix
		cairo_matrix_t matrix;
		cairo_matrix_init(&matrix, xx, yx, xy, yy, x0, y0);
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
	double *convolution_filter = luaL_checktable<double>(L, 2);
	std::auto_ptr<double> convolution_filter_obj(convolution_filter);
	if(!convolution_filter)
		luaL_error2(L, "table shouldn't be empty");
	size_t filter_size = lua_rawlen(L, 2);
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
	// Create & return Lua pixels table of image area
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
				for(int x = 0; x < area_width; x++)
					lua_pushnumber(L, *row++); lua_rawseti(L, -2, ++table_index);	// A
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

	// TODO

LUA_FUNC_END

// MATRIX OBJECT

// SOURCE OBJECT
LUA_FUNC_1ARG(source_gc, 1)
	cairo_pattern_t *pattern = *reinterpret_cast<cairo_pattern_t**>(luaL_checkuserdata(L, 1, G2D_SOURCE));
	cairo_pattern_destroy(pattern);
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
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pop(L, 1);
	// Define source object methods
	luaL_newmetatable(L, G2D_SOURCE);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_source_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	// Define context object methods
	luaL_newmetatable(L, G2D_CONTEXT);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_context_gc); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	// Nothing pushed to Lua state
	return 0;
}