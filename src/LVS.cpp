#include "LVS.hpp"

LVS::LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames,
				const char* audio_file, int channels, int sample_rate, __int64 samples) : video_proc(0), audio_proc(0){
	// Create video process
	if(video_file)
		this->video_proc = new LVSVideo(video_file, width, height, has_alpha, fps, frames);
	// Create audio process
	if(audio_file)
		try{
			this->audio_proc = new LVSAudio(audio_file, channels, sample_rate, samples);
		}catch(...){
			if(this->video_proc)
				delete this->video_proc;
			throw;
		}
}

LVS::LVS(const char* video_file, int width, int height, bool has_alpha, double fps, int frames) : video_proc(0), audio_proc(0){
	// Create video process
	this->video_proc = new LVSVideo(video_file, width, height, has_alpha, fps, frames);
}

LVS::~LVS(){
	// Free video process
	if(this->video_proc)
		delete this->video_proc;
	// Free audio process
	if(this->audio_proc)
		delete this->audio_proc;
}

void LVS::RenderOnFrame(CairoImage *image, int frame_number){
	// Pass frame to video process
	if(this->video_proc)
		this->video_proc->Render(image, frame_number);
}

void LVS::RenderOnSamples(float* buf, __int64 start, __int64 count){
	// Pass samples to audio process
	if(this->audio_proc)
		this->audio_proc->Render(buf, start, count);
}