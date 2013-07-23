// Include avisynth
#include <windows.h>
#include <avisynth.h>
// Include LVS
#include "LVS.hpp"
// Include utilities
#include "fileinfo.hpp"
#include "textconv.hpp"
#include "image.hpp"
#include <exception>

// Clip with frames & samples filter
class LVSFilteredClip : public GenericVideoFilter{
	private:
		// LVS instance for filtering process
		LVS *lvs;
		// Image buffer for frame conversion
		CairoImage *image;
	public:
		// Clip constructor
		LVSFilteredClip(IScriptEnvironment* env, PClip clip, const char* video_file, const char* audio_file, const char* data_string) : GenericVideoFilter(clip), lvs(0), image(0){
			// Create LVS instance
			try{
				this->lvs = new LVS(video_file, this->vi.width, this->vi.height, this->vi.IsRGB32(), static_cast<double>(this->vi.fps_numerator) / this->vi.fps_denominator, this->vi.num_frames,
													audio_file, this->vi.nchannels, this->vi.audio_samples_per_second, this->vi.num_audio_samples,
													data_string);
			}catch(std::exception e){
				env->ThrowError(FILTER_NAME" initialization failed: %s", e.what());
			}
			// Create image buffer
			this->image = new CairoImage(this->vi.width, this->vi.height, this->vi.IsRGB32());
		}
		// Clip destructor
		~LVSFilteredClip(){
			// Free LVS instance
			if(this->lvs)
				delete this->lvs;
			// Free image buffer
			if(this->image)
				delete this->image;
		}
		// Frames callback
		PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env){
			// Get current frame from child clip & add writing support
			PVideoFrame frame = this->child->GetFrame(n, env);
			env->MakeWritable(&frame);
			// Convert frame to image
			this->image->Load(frame->GetWritePtr(), frame->GetPitch(), false);
			// Filter image
			try{
				// Send image data through filter process
				this->lvs->RenderOnFrame(this->image, n);
			}catch(std::exception e){
				// Show UTF8 error message
				int choice = MessageBoxW(0, utf8_to_utf16(e.what()).c_str(), FILTER_NAMEW L" video error", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONWARNING);
				// Throw exception upwards?
				if(choice == IDCANCEL)
					env->ThrowError(e.what());
			}
			// Convert image to frame
			this->image->Save(frame->GetWritePtr(), frame->GetPitch(), false);
			// Return filtered frame
			return frame;
		}
		// Samples callback
		void __stdcall GetAudio(void* buf, __int64 start, __int64 count, IScriptEnvironment* env){
			// Copy current samples from child clip
			this->child->GetAudio(buf, start, count, env);
			// Filter samples
			try{
				// Send samples data through filter process
				this->lvs->RenderOnSamples(reinterpret_cast<float*>(buf), count * this->vi.nchannels, start);
			}catch(std::exception e){
				// Show UTF8 error message
				int choice = MessageBoxW(0, utf8_to_utf16(e.what()).c_str(), FILTER_NAMEW L" audio error", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONWARNING);
				// Throw exception upwards?
				if(choice == IDCANCEL)
					env->ThrowError(e.what());
			}
		}
};

// Filter execution
AVSValue LVSExecute(AVSValue args, void* user_data, IScriptEnvironment* env){
	// Extract filter arguments
	PClip clip = args[0].AsClip();
	const char *video_file = args[1].AsString(0);
	const char *audio_file = args[2].AsString(0);
	const char *data_string = args[3].AsString(0);
	// Check arguments
	VideoInfo clip_info = clip->GetVideoInfo();
	if(!video_file && !audio_file)
		env->ThrowError("Video and/or audio script expected!");
	else if(video_file && !clip_info.HasVideo())
		env->ThrowError("No video stream for script '%s'!", video_file);
	else if(audio_file && !clip_info.HasAudio())
		env->ThrowError("No audio stream for script '%s'!", audio_file);
	// Convert video to RGB colorspace
	if(video_file && !clip_info.IsRGB())
		try{
			clip = env->Invoke("ConvertToRGB24", clip).AsClip();
		}catch(IScriptEnvironment::NotFound e){
			env->ThrowError("Couldn't convert video to RGB colorspace!");
		}
	// Convert audio to float sample size
	if(audio_file && clip_info.SampleType() !=  SAMPLE_FLOAT)
		try{
			clip = env->Invoke("ConvertAudioToFloat", clip).AsClip();
		}catch(IScriptEnvironment::NotFound e){
			env->ThrowError("Couldn't convert audio to float sample size!");
		}
	// Return filtered clip
	return new LVSFilteredClip(env, clip, video_file, audio_file, data_string);
}

//Register filter
extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env) {
	// Valid avisynth interface version?
	env->CheckVersion();
	// Register LVS function
	env->AddFunction(FILTER_NAME, "c[video]s[audio]s[data]s", LVSExecute, 0);
	// Return filter description
	return FILTER_DESC_LONG;
}