#pragma once
#include <windows.h>

// Multithreading class
template<class T>
class Threads{
	private:
		// CPUs/threads number
		DWORD cpu_num;
		// Thread running routine
		LPTHREAD_START_ROUTINE routine;
		// Threads data
		T *data;
		// Thread handles
		HANDLE *threads;
	public:
		// Ctor (allocate threading data)
		Threads(LPTHREAD_START_ROUTINE routine) : routine(routine){
			// Ger number of processors
			SYSTEM_INFO system_info = {0};
			GetSystemInfo(&system_info);
			this->cpu_num = system_info.dwNumberOfProcessors;
			// Allocate threads data
			this->data = new T[this->cpu_num];
			// Allocate thread handles
			this->threads = new HANDLE[this->cpu_num-1];
		}
		// Dtor (free threading data)
		~Threads(){
			// Free thread handles
			delete[] this->threads;
			// Free threads data
			delete[] this->data;
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
