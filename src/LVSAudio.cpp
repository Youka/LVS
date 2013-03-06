#include "LVSAudio.hpp"
#include <exception>

LVSAudio::LVSAudio(const char* script, int channels, int sample_rate, __int64 samples){
	// Set global audio informations
	lua_pushnumber(this->L, channels); lua_setglobal(this->L, "AUDIO_CHANNELS");
	lua_pushnumber(this->L, sample_rate); lua_setglobal(this->L, "AUDIO_SAMPLE_RATE");
	lua_pushnumber(this->L, samples); lua_setglobal(this->L, "AUDIO_SAMPLES");
	// Set libraries

	// TODO

	// Load script
	if(luaL_dofile(this->L, script))
		throw std::exception(lua_tostring(this->L, -1));
}

void LVSAudio::Render(float* buf, __int64 start, __int64 count){
}