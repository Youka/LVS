#pragma once

#include "lua.hpp"

// Base class for LVS video&audio class
class LVSMediaBase{
	protected:
		// Lua environment
		Lua L;
	public:
		// Initialize Lua environment
		LVSMediaBase(const char* data_string = 0);
	private:
		// Lua libraries loaders
		void LoadInternalLibs();
		void LoadExternalLibs();
};