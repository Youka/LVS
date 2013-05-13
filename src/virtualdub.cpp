// Include virtualdub
#include <vd2/vdvideofilt.h>
// Include LVS
#include "LVS.hpp"
// Include GUI
#include "config_dialog.hpp"
// Include utilities
#include "fileinfo.hpp"
#include "textconv.hpp"
#include "image.hpp"
#include <exception>
#include <cstdio>

// DLL instance getter for VC compilers
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE reinterpret_cast<HINSTANCE>(&__ImageBase)

// VirtualDub process
namespace vdub{
	// VirtualDub version
	int version;
	// Filter instance data container
	struct LVSData{
		LVS *lvs;
		CairoImage *image;
		char *filename;
	};
	// Filter initialization
	int init_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		LVSData *inst_data = reinterpret_cast<LVSData*>(fdata->filter_data);
		// Initialize instance data
		inst_data->lvs = NULL;
		inst_data->image = NULL;
		inst_data->filename = NULL;
		// Success
		return 0;
	}
	// Filter deinitialization
	void deinit_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		LVSData *inst_data = reinterpret_cast<LVSData*>(fdata->filter_data);
		// Free instance data
		if(inst_data->lvs){
			delete inst_data->lvs;
			inst_data->lvs = NULL;
		}
		if(inst_data->image){
			delete inst_data->image;
			inst_data->image = NULL;
		}
		if(inst_data->filename){
			delete[] inst_data->filename;
			inst_data->filename = NULL;
		}
	}
	// Filter running
	int run_func(const VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		LVSData *inst_data = reinterpret_cast<LVSData*>(fdata->filter_data);
		// Convert source frame to image
		inst_data->image->Load(reinterpret_cast<unsigned char*>(fdata->src.data), fdata->src.pitch, CairoImage::VDUB);
		// Filter image
		try{
			// Send image data through filter process
			inst_data->lvs->RenderOnFrame(inst_data->image, fdata->src.mFrameNumber);
		}catch(std::exception e){
			// Show UTF8 error message
			wchar_t *werr = utf8_to_utf16(e.what());
			MessageBoxW(0, werr, FILTER_NAMEW L" video error", MB_OK | MB_ICONWARNING);
			delete[] werr;
			// Throw exception to GUI
			ffuncs->Except(e.what());
			return 1;
		}
		// Convert image to destination frame
		inst_data->image->Save(reinterpret_cast<unsigned char*>(fdata->dst.data), fdata->dst.pitch, CairoImage::VDUB);
		// Success
		return 0;
	}
	// Filter parameters check
	long param_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		// VirtualDub version supports color format other than RGB32?
		if(version >= 12)
			// Just RGB formats allowed
			switch(fdata->src.mpPixmapLayout->format){
				case nsVDXPixmap::kPixFormat_RGB888:
				case nsVDXPixmap::kPixFormat_XRGB8888:
					break;
				default:
					return FILTERPARAM_NOT_SUPPORTED;
			}
		// Color format accepted
		return FILTERPARAM_SUPPORTS_ALTFORMATS | FILTERPARAM_SWAP_BUFFERS;
	}
	// Filter configuration
	INT_PTR CALLBACK config_event_handler(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam){
		// Evaluate event
		switch(msg){
			// Dialog initialization
			case WM_INITDIALOG:{
				// Store userdata to window
				LVSData *inst_data = reinterpret_cast<LVSData*>(lParam);
				SetWindowLongPtrA(wnd, DWLP_USER, reinterpret_cast<LONG_PTR>(inst_data));
				// Set default filename
				if(inst_data->filename){
					HWND edit = GetDlgItem(wnd, ID_CONFIG_FILENAME);
					wchar_t *filenamew = utf8_to_utf16(inst_data->filename);
					SendMessageW(edit, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(filenamew));
					delete[] filenamew;
					SendMessageW(edit, EM_SETSEL, 0, -1);
				}
			}break;
			// Dialog action
			case WM_COMMAND:{
				// Evaluate action command
				switch(wParam){
					// '...' button
					case ID_CONFIG_CHOOSE_FILE:{
						// Prepare file dialog properties
						wchar_t file[256]; file[0] = '\0';
						OPENFILENAMEW ofn;
						memset(&ofn, 0, sizeof(ofn));
						ofn.lStructSize = sizeof(OPENFILENAMEW);
						ofn.hwndOwner = wnd;
						ofn.hInstance = DLL_INSTANCE;
						ofn.lpstrFilter = L"Lua file (*.lua)\0*.lua\0\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFile = file;
						ofn.nMaxFile = sizeof(file);
						ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
						// Show file dialog
						if(GetOpenFileNameW(&ofn)){
							// Save filename input to dialog
							HWND edit = GetDlgItem(wnd, ID_CONFIG_FILENAME);
							SendMessageW(edit, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(ofn.lpstrFile));
							SendMessageW(edit, EM_SETSEL, 0, -1);
						}
					}break;
					// 'OK' button
					case IDOK:{
						// Get stored userdata from window
						LVSData *inst_data = reinterpret_cast<LVSData*>(GetWindowLongPtrA(wnd, DWLP_USER));
						// Save filename
						HWND edit = GetDlgItem(wnd, ID_CONFIG_FILENAME);
						int text_len = static_cast<int>(SendMessageW(edit, WM_GETTEXTLENGTH, 0, 0));
						wchar_t *text_buf = new wchar_t[text_len+1];
						SendMessageW(edit, WM_GETTEXT, text_len+1, reinterpret_cast<LPARAM>(text_buf));
						if(inst_data->filename)
							delete[] inst_data->filename;
						inst_data->filename = utf16_to_utf8(text_buf);
						delete[] text_buf;
						// Close dialog
						EndDialog(wnd, 0);
					}break;
					// 'Cancel' button
					case IDCANCEL:{
						// Close dialog
						EndDialog(wnd, 1);
					}break;
				}
			}break;
			// Dialog closure ('X' button)
			case WM_CLOSE:
				EndDialog(wnd, 1);
				break;
			// Message not handled (default behaviour follows)
			default:
				return FALSE;
		}
		// Message handled
		return TRUE;
	}
	int config_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs, VDXHWND wnd){
		// Show configuration dialog and return result
		return DialogBoxParamW(DLL_INSTANCE, MAKEINTRESOURCE(ID_CONFIG_DIALOG), reinterpret_cast<HWND>(wnd), config_event_handler, reinterpret_cast<LPARAM>(fdata->filter_data));
	}
	// Filter description
	void create_description(LVSData *inst_data, char *buf, int maxlen = 128){
		// Fill description buffer with loaded filename
		if(inst_data->filename)
			_snprintf(buf, maxlen, " Script:'%s'", inst_data->filename);
		else
			_snprintf(buf, maxlen, " Script:''");
	}
	void description_func(const VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs, char *buf){
		create_description(reinterpret_cast<LVSData*>(fdata->filter_data), buf);
	}
	void description2_func(const VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs, char *buf, int maxlen){
		create_description(reinterpret_cast<LVSData*>(fdata->filter_data), buf, maxlen);
	}
	// Filter start
	int start_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		LVSData *inst_data = reinterpret_cast<LVSData*>(fdata->filter_data);
		// Check video informations
		if(fdata->pfsi == NULL)
			ffuncs->Except("Video informations are missing!");
		// Get colorspace (RGB or RGBA?)
		bool has_alpha = version < 12 ? true : (fdata->src.mpPixmapLayout->format == nsVDXPixmap::kPixFormat_XRGB8888 ? true : false);
		// Free previous render data (in case of buggy twice start function call)
		if(inst_data->lvs){
			delete inst_data->lvs;
			inst_data->lvs = NULL;
			delete inst_data->image;
			inst_data->image = NULL;
		}
		// Create LVS instance
		try{
			inst_data->lvs = new LVS(inst_data->filename, fdata->src.w, fdata->src.h, has_alpha, static_cast<double>(fdata->src.mFrameRateHi) / fdata->src.mFrameRateLo, fdata->src.mFrameCount);
		}catch(std::exception e){
			ffuncs->Except(FILTER_NAME" initialization failed: %s", e.what());
			return 1;
		}
		// Create image buffer
		inst_data->image = new CairoImage(fdata->src.w, fdata->src.h, has_alpha);
		// Success
		return 0;
	}
	// Filter end
	int end_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		LVSData *inst_data = reinterpret_cast<LVSData*>(fdata->filter_data);
		// Free render data
		if(inst_data->lvs){
			delete inst_data->lvs;
			inst_data->lvs = NULL;
			delete inst_data->image;
			inst_data->image = NULL;
		}
		// Success
		return 0;
	}
}

// Filter registration definitions
VDXFilterDefinition lvs_filter_definition = {
	NULL,	// _next
	NULL,	// _prev
	NULL,	// _module

	FILTER_NAME,	// name
	FILTER_DESC_LONG,	// desc
	FILTER_AUTHOR,	// maker
	NULL,	// private data
	sizeof(vdub::LVSData),	// inst_data_size

	vdub::init_func,	// initProc
	vdub::deinit_func,	// deinitProc
	vdub::run_func,	// runProc
	vdub::param_func,	// paramProc
	vdub::config_func,	// configProc
	vdub::description_func,	// stringProc
	vdub::start_func,	// startProc
	vdub::end_func,	// endProc

	NULL,	// script_obj
	NULL,	// fssProc

	vdub::description2_func,	// stringProc2
	NULL,	// serializeProc
	NULL,	// deserializeProc
	NULL,	// copyProc

	NULL,	// prefetchProc

	NULL,	// copyProc2
	NULL,	// prefetchProc2
	NULL	// eventProc
};
VDXFilterDefinition *registered_filter_definition;

// Register filter
extern "C" __declspec(dllexport) int __cdecl VirtualdubFilterModuleInit2(struct VDXFilterModule *fmodule, const VDXFilterFunctions *ffuncs, int& vdfd_ver, int& vdfd_compat){
	// Create register definition
	registered_filter_definition = ffuncs->addFilter(fmodule, &lvs_filter_definition, sizeof(VDXFilterDefinition));
	// Version & compatibility definition
	vdub::version = vdfd_ver;
	vdfd_ver = VIRTUALDUB_FILTERDEF_VERSION;
	vdfd_compat = VIRTUALDUB_FILTERDEF_COMPATIBLE_COPYCTOR;
	// Success
	return 0;
}

// Unregister filter
extern "C" __declspec(dllexport) void __cdecl VirtualdubFilterModuleDeinit(struct VDXFilterModule *fmodule, const VDXFilterFunctions *ffuncs){
	// Remove register definition
	ffuncs->removeFilter(registered_filter_definition);
}