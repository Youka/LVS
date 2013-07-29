#include "LVSMediaBase.hpp"
// Include libraries loaders
#include "llibs.hpp"
// Include utilities
#include "dir.hpp"
#include "textconv.hpp"
#include <exception>

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
	// Get plugin directory
	std::wstring dir = GetCurrentDir();
	if(dir.length() > 0){
		// Change directory to subdirectory 'LVS'
		dir.append(L"LVS\\");
		// Get directory files
		std::vector<std::wstring> files = GetDirFiles(dir);
		// Iterate through files
		for(size_t i = 0; i < files.size(); ++i){
			std::wstring file = files[i];
			// Lua file?
			if(file.find(L".lua", file.length()-4) != std::wstring::npos){
				// Load Lua file
				if(luaL_dofile(this->L, utf16_to_utf8(dir + file).c_str())){
					std::exception e(lua_tostring(this->L, -1));
					lua_pop(this->L, 1);
					throw e;
				}
				// Free resources
				lua_gc(this->L, LUA_GCCOLLECT, 0);
			}
		}
	}
}