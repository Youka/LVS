#include "LVSAudio.hpp"
#include <exception>

LVSAudio::LVSAudio(const char* script, int channels, int sample_rate, __int64 samples, const char* data_string) : LVSMediaBase(data_string){
	// Set global audio informations
	lua_pushstring(this->L, script); lua_setglobal(this->L, "AUDIO_SCRIPT");
	lua_pushnumber(this->L, channels); lua_setglobal(this->L, "AUDIO_CHANNELS");
	lua_pushnumber(this->L, sample_rate); lua_setglobal(this->L, "AUDIO_SAMPLE_RATE");
	lua_pushnumber(this->L, samples); lua_setglobal(this->L, "AUDIO_SAMPLES");
	// Load script
	if(luaL_dofile(this->L, script)){
		std::exception e(lua_tostring(this->L, -1));
		lua_pop(this->L, 1);
		throw e;
	}
	// Free resources
	lua_gc(this->L, LUA_GCCOLLECT, 0);
}

void LVSAudio::Render(float* buf, __int64 buf_size, __int64 start_sample){
	// Create and fill table with samples data
	lua_pushtable<float>(this->L, buf, buf_size);
	// Get render function
	lua_getglobal(this->L, "GetSamples");
	if(lua_isfunction(this->L, -1)){
		// Push function arguments
		lua_pushvalue(this->L, -2);	// Samples reference
		lua_pushnumber(this->L, buf_size);	// Samples number
		lua_pushnumber(this->L, start_sample);	// Start sample index
		// Call render function
		if(lua_pcall(this->L, 3, 0, 0)){
			// Pop samples table & error string from stack and throw error
			std::exception e(lua_tostring(this->L, -1));
			lua_pop(this->L, 2);
			throw e;
		}
		// Run garbage collection
		lua_gc(this->L, LUA_GCCOLLECT, 0);
	}else{
		// Pop samples table & non-function from stack and throw error
		lua_pop(this->L, 2);
		throw std::exception("Function 'GetSamples' is missing!");
	}
	// Get samples data back from samples table and remove him
	for(__int64 i = 0; i < buf_size; i++){
		lua_rawgeti(this->L, -1, i+1);
		if(!lua_isnumber(this->L, -1)){
			// Pop table element & samples table and throw error
			lua_pop(this->L, 2);
			throw std::exception("Samples must be numbers!");
		}
		buf[i] = lua_tonumber(this->L, -1);
		lua_pop(this->L, 1);
	}
	lua_pop(this->L, 1);
}