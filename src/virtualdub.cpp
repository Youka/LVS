// Include virtualdub
#include <vd2/vdvideofilt.h>
// Include LVS
#include "LVS.hpp"
// Include GUI
#include <windows.h>
#include <Commdlg.h>
// Include utilities
#include "fileinfo.hpp"
#include <exception>
#include "conv.hpp"
#include <cstdio>

// VirtualDub process
namespace vdub{
	// VirtualDub version
	int version;
	// Filter instance data container
	struct lvs_data{
		LVS *lvs;
		unsigned char *image;
		wchar_t *filename;
	};
	// Filter initialization
	int init_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		return 0;
	}
	// Filter deinitialization
	void deinit_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){

	}
	// Filter running
	int run_func(const VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		return 0;
	}
	// Filter parameters check
	long param_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		return 0;
	}
	// Filter configuration
	int config_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs, VDXHWND wnd){
		return 0;
	}
	// Filter description
	void description_func(const VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs, char *buf){

	}
	void description2_func(const VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs, char *buf, int maxlen){

	}
	// Filter start
	int start_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
		return 0;
	}
	// Filter end
	int end_func(VDXFilterActivation *fdata, const VDXFilterFunctions *ffuncs){
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
	sizeof(vdub::lvs_data),	// inst_data_size

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