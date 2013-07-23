#pragma once
#include <windows.h>
#include <vector>

// Multithreading class
template<class T>
class Threads{
	private:
		// CPUs/threads number
		DWORD cpu_num;
		// Thread running routine
		LPTHREAD_START_ROUTINE routine;
		// Threads data
		std::vector<T> data;
		// Thread handles
		std::vector<HANDLE> threads;
	public:
		// Ctor (allocate threading data)
		Threads(LPTHREAD_START_ROUTINE routine) : routine(routine){
			// Ger number of processors
			SYSTEM_INFO system_info = {0};
			GetSystemInfo(&system_info);
			this->cpu_num = system_info.dwNumberOfProcessors;
			// Allocate threads data memory
			this->data.reserve(this->cpu_num);
			// Allocate thread handles memory
			this->threads.reserve(this->cpu_num-1);
		}
		// Get CPUs/threads number
		inline const DWORD size(){
			return this->cpu_num;
		}
		// Thread data
		inline T* get(DWORD i){
			return &this->data[i];
		}
		// Run threads
		void Run(){
			// Create & run threads
			for(DWORD i = 0; i < this->cpu_num - 1; ++i)
				this->threads[i] = CreateThread(NULL, 0, this->routine, reinterpret_cast<LPVOID>(&this->data[i]), 0x0, NULL);
			this->routine(reinterpret_cast<LPVOID>(&this->data[this->cpu_num - 1]));
			// Wait for and close threads
			for(DWORD i = 0; i < this->cpu_num - 1; ++i){
				WaitForSingleObject(this->threads[i], INFINITE);
				CloseHandle(this->threads[i]);
			}
		}
};
