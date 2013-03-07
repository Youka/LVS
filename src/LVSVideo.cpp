#include "LVSVideo.hpp"
#include <exception>

// TEST START
#include "cairo.hpp"
// TEST END

LVSVideo::LVSVideo(const char* script, int width, int height, bool has_alpha, double fps, int frames){
	// Set global video informations
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

	// TODO

	// TEST START
	cairo_surface_t *surf = cairo_image_surface_create_for_data(*image, CAIRO_FORMAT_ARGB32, 720, 480, 2880);
	cairo_t *ctx = cairo_create(surf);

	cairo_rectangle(ctx, 300, 20, 50, 50);
	cairo_set_source_rgba(ctx, 1, 1, 0.5, 0.7);
	cairo_fill(ctx);

	cairo_destroy(ctx);
	cairo_surface_destroy(surf);
	// TEST END
}