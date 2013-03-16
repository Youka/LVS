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
	if(GetModuleFileNameW(DLL_INSTANCE, buf, buf_len) > 0){
		// Get plugin LVS folder
		wchar_t *dir_sep = wcsrchr(buf, L'\\');
		if(dir_sep && dir_sep - buf + 7 <= buf_len){
			wcscpy(dir_sep+1, L"LVS\\*");
			// Iterate through folder
			HANDLE finder;
			WIN32_FIND_DATAW file_data;
			finder = FindFirstFileW(buf, &file_data);
			if(finder != INVALID_HANDLE_VALUE){
				do{
					// Valid file?
					size_t len = wcslen(file_data.cFileName);
					if((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
						len > 4 && wcscmp(file_data.cFileName+len-4, L".lua") == 0 &&
						dir_sep - buf + 6 + len <= buf_len){
						// Convert filename to UTF-8
						wcscpy(dir_sep+5, file_data.cFileName);
						char *filename = utf16_to_utf8(buf);
						// Load Lua file
						if(luaL_dofile(this->L, filename)){
							delete[] filename;
							FindClose(finder);
							std::exception e(lua_tostring(this->L, -1));
							lua_pop(this->L, 1);
							throw e;
						}
						// Free resources
						lua_gc(this->L, LUA_GCCOLLECT, 0);
						delete[] filename;
					}
				}while(FindNextFileW(finder, &file_data));
				FindClose(finder);
			}
		}
	}
}