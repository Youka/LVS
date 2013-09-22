#include "dir.hpp"
#include <windows.h>

// DLL instance getter for VC compilers
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE reinterpret_cast<HINSTANCE>(&__ImageBase)

std::wstring GetCurrentDir(){
	// Get DLL path
	std::wstring dir(MAX_PATH, L'\0');
	if(GetModuleFileNameW(DLL_INSTANCE, const_cast<LPWCH>(dir.data()), dir.size()) > 0){
		// Cut path to directory
		std::wstring::size_type hit = dir.rfind(L'\\');
		if(hit != std::wstring::npos)
			// Success
			return dir.substr(0, hit + 1);
	}
	// Fail
	return L"";
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