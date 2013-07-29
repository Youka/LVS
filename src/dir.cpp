#include "dir.hpp"
#include <windows.h>
#include <cstring>

// DLL instance getter for VC compilers
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE reinterpret_cast<HINSTANCE>(&__ImageBase)

std::wstring GetCurrentDir(){
	// Directory storage
	std::wstring dir;
	// Get DLL path
	wchar_t buf[MAX_PATH];
	if(GetModuleFileNameW(DLL_INSTANCE, buf, MAX_PATH) > 0){
		// Cut path to directory
		const wchar_t *pos = wcsrchr(buf, L'\\');
		if(pos)
			dir.assign(buf, pos+1 - buf);
	}
	// Return result
	return dir;
}

std::vector<std::wstring> GetDirFiles(std::wstring &dir){
	// Result
	std::vector<std::wstring> files;
	// Find data
	HANDLE handle;
	WIN32_FIND_DATAW file_info;
	// Valid directory?
	handle = FindFirstFileW((dir + L"*").c_str(), &file_info);
	if(handle != INVALID_HANDLE_VALUE){
		// Iterate through files
		do
			// Not a directory?
			if((file_info.dwFileAttributes  & FILE_ATTRIBUTE_DIRECTORY) == 0)
				files.push_back(file_info.cFileName);
		while(FindNextFileW(handle, &file_info));
		// Clear resources
		FindClose(handle);
	}
	// Return result
	return files;
}