// Include windows 'Globally Unique Identifier'
#include <initguid.h>
#include <CGuid.h>
// Include windows streams
#include <streams.h>
// Include LVS
#include "LVS.hpp"
// Include GUI
#include "config_dialog.hpp"
#include <Commdlg.h>
// Include utilities
#include "fileinfo.hpp"
#include <exception>

// DLL instance getter for VC compilers
extern "C" IMAGE_DOS_HEADER __ImageBase;
#define DLL_INSTANCE reinterpret_cast<HINSTANCE>(&__ImageBase)

// GUIDs for filter
// {C08A541A-04FE-477c-9948-EF317E21F655}
DEFINE_GUID(CLSID_LVSVideoFilter,
0xc08a541a, 0x4fe, 0x477c, 0x99, 0x48, 0xef, 0x31, 0x7e, 0x21, 0xf6, 0x55);
// {35DCC96B-7416-45a5-82FB-8941296780AD}
DEFINE_GUID(CLSID_LVSVideoFilterPropertyPage,
0x35dcc96b, 0x7416, 0x45a5, 0x82, 0xfb, 0x89, 0x41, 0x29, 0x67, 0x80, 0xad);
// {0FAA6F16-272B-469d-A7CD-CB6441BD6825}
DEFINE_GUID(IID_ILVSVideoFilterConfiguration,
0xfaa6f16, 0x272b, 0x469d, 0xa7, 0xcd, 0xcb, 0x64, 0x41, 0xbd, 0x68, 0x25);

// Filter configuration interface
interface ILVSVideoFilterConfiguration : public IUnknown{
	virtual wchar_t* GetFile() = 0;
	virtual void SetFile(wchar_t*) = 0;
};

// Filter
class LVSVideoFilter{
	public:
		// Create class instance
		static CUnknown* CALLBACK CreateInstance(LPUNKNOWN unknown, HRESULT *result){
			return 0;
		}
};

// Filter property page
class LVSVideoFilterPropertyPage{
	public:
		// Create class instance
		static CUnknown* CALLBACK CreateInstance(LPUNKNOWN unknown, HRESULT *result){
			return 0;
		}
};

// Filter setup
const AMOVIESETUP_MEDIATYPE sudPinTypes[] =
{
	{
		&MEDIATYPE_Video,
		&MEDIASUBTYPE_RGB24
	},
	{
		&MEDIATYPE_Video,
		&MEDIASUBTYPE_RGB32
	}
};

const AMOVIESETUP_PIN sudpPins[] =
{
    { L"Input",             // Pin string name
      FALSE,                // Is it rendered
      FALSE,                // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      2,                    // Number of media types
      sudPinTypes          // Media types
    },
    { L"Output",            // Pin string name
      FALSE,                // Is it rendered
      TRUE,                 // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      2,                    // Number of media types
      sudPinTypes          // Media types
    }
};

const AMOVIESETUP_FILTER sudFilter =
{
    &CLSID_LVSVideoFilter,         // Filter CLSID
    FILTER_NAMEW,       // Filter name
    MERIT_DO_NOT_USE,       // Filter merit
    2,                      // Number of pins
    sudpPins                // Pin information
};

// Filter definition as COM objects
CFactoryTemplate g_Templates[] = {
	{ 
		FILTER_NAMEW,	// Filter name
		&CLSID_LVSVideoFilter,		// Filter CLSID
		LVSVideoFilter::CreateInstance,	// Filter instance creation
		NULL,		// Init routine
		&sudFilter		// Filter setup
	},
	{
		FILTER_PROP_NAMEW,	// Property page name
		&CLSID_LVSVideoFilterPropertyPage,		// Property page CLSID
		LVSVideoFilterPropertyPage::CreateInstance,	// Property page instance creation
		NULL, NULL		//Unneeded
	}
};
int g_cTemplates = sizeof(g_Templates) / sizeof(CFactoryTemplate);	// Number of filter definitions

// Register filter to server
STDAPI DllRegisterServer(){
    return AMovieDllRegisterServer2( TRUE );
}

// Unregister filter from server
STDAPI DllUnregisterServer(){
    return AMovieDllRegisterServer2( FALSE );
}