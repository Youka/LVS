#include "LVSMediaBase.hpp"
// Include libraries loaders
#include "llibs.hpp"
// Include utilities
#include <windows.h>
#include "textconv.hpp"
#include <exception>

// DLL instance getter for VC compilers
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE reinterpret_cast<HINSTANCE>(&__ImageBase)

LVSMediaBase::LVSMediaBase(const char* data_string){
	// Set global userdata string
	if(data_string){
		lua_pushstring(this->L, data_string); lua_setglobal(this->L, "USERDATA");
	}
	// Register additional libraries
	this->LoadInternalLibs();
	this->LoadExternalLibs();
}

void LVSMediaBase::LoadInternalLibs(){
	// Load additional base functions and graphics library
	luaopen_base2(this->L);
	luaopen_g2d(this->L);
}

void LVSMediaBase::LoadExternalLibs(){
	// Get plugin filename
	const int buf_len = 256;
	wchar_t buf[buf_len];
	GetModuleFileNameW(DLL_INSTANCE, buf, buf_len);
	// Get filename pointer to extension character
	wchar_t *ext = wcsrchr(buf, L'.');
	if(ext){
		// Change filename extension to '.lua'
		if(ext - buf + 5 <= buf_len){
			wcscpy(ext, L".lua");
			// File exists?
			FILE *f = _wfopen(buf, L"r");
			if(f){
				fclose(f);
				// Convert filename to UTF-8
				char *filename = utf16_to_utf8(buf);
				// Load Lua file
				if(luaL_dofile(this->L, filename)){
					delete[] filename;
					std::exception e(lua_tostring(this->L, -1));
					lua_pop(this->L, 1);
					throw e;
				}
				// Free resources
				delete[] filename;
				lua_gc(this->L, LUA_GCCOLLECT, 0);
			}
		}
	}
}