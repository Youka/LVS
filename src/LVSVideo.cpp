#include "LVSVideo.hpp"
#include <exception>
#include "cairo.hpp"

LVSVideo::LVSVideo(const char* script, int width, int height, bool has_alpha, double fps, int frames, const char* data_string) : LVSMediaBase(data_string){
	// Set global video informations
	lua_pushstring(this->L, script); lua_setglobal(this->L, "VIDEO_SCRIPT");
	lua_pushnumber(this->L, width); lua_setglobal(this->L, "VIDEO_WIDTH");
	lua_pushnumber(this->L, height); lua_setglobal(this->L, "VIDEO_HEIGHT");
	lua_pushnumber(this->L, has_alpha); lua_setglobal(this->L, "VIDEO_WITH_ALPHA");
	lua_pushnumber(this->L, fps); lua_setglobal(this->L, "VIDEO_FPS");
	lua_pushnumber(this->L, frames); lua_setglobal(this->L, "VIDEO_FRAMES");
	// Load script
	if(luaL_dofile(this->L, script)){
		std::exception e(lua_tostring(this->L, -1));
		lua_pop(this->L, 1);
		throw e;
	}
	// Free resources
	lua_gc(this->L, LUA_GCCOLLECT, 0);
}

void LVSVideo::Render(CairoImage* image, int frame_number){
	// Get render function
	lua_getglobal(this->L, "GetFrame");
	if(lua_isfunction(this->L, -1)){
		// Push function arguments
		*lua_createuserdata<cairo_surface_t*>(L, "LVS_IMAGE") = cairo_image_surface_create_for_data(const_cast<unsigned char*>(image->data), image->has_alpha ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24, image->width, image->height, image->stride);	// Image surface with frame reference (object methods, including surface destruction, in 'g2d' library)
		lua_pushnumber(this->L, frame_number);	// Frame number
		// Call render function
		if(lua_pcall(this->L, 2, 0, 0)){
			// Pop error string from stack and throw error
			std::exception e(lua_tostring(this->L, -1));
			lua_pop(this->L, 1);
			throw e;
		}
		// Run garbage collection
		lua_gc(this->L, LUA_GCCOLLECT, 0);
	}else{
		// Pop non-function from stack and throw error
		lua_pop(this->L, 1);
		throw std::exception("Function 'GetFrame' is missing!");
	}
}