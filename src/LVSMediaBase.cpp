#include "LVSMediaBase.hpp"
#include "textconv.hpp"
#include <exception>

// DLL instance getter for VC compilers
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE reinterpret_cast<HINSTANCE>(&__ImageBase)

LVSMediaBase::LVSMediaBase(){
	// Get plugin filename
	const int buf_len = 256;
	wchar_t buf[buf_len];
	GetModuleFileNameW(DLL_INSTANCE, buf, buf_len);
	// Extract filename without extension
	wchar_t *ext = wcsrchr(buf, L'.');
	if(ext){
		// Build filename with .lua extension
		if(ext - buf + 5 <= buf_len){
			wcscpy(ext, L".lua");
			// Convert filename to UTF-8
			char *filename = utf16_to_utf8(buf);
			// Load Lua file
			if(luaL_dofile(L, filename)){
				delete[] filename;
				throw std::exception(lua_tostring(L, -1));
			}
			// Free resources
			delete[] filename;
			lua_gc(L, LUA_GCCOLLECT, 0);
		}
	}
}