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
#include "textconv.hpp"
#include "image.hpp"
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
	virtual char* GetFile() = 0;
	virtual void SetFile(char*) = 0;
};

// Filter property page
class LVSVideoFilterPropertyPage : public CBasePropertyPage{
	private:
		// Data interface of filter
		ILVSVideoFilterConfiguration *config;
		// Configuration dialog event handling
		static INT_PTR CALLBACK ConfigEventHandler(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam){
			// Evaluate event
			switch(msg){
				// Dialog initialization
				case WM_INITDIALOG:{
					// Store userdata to window
					ILVSVideoFilterConfiguration *config = reinterpret_cast<ILVSVideoFilterConfiguration*>(lParam);
					SetWindowLongPtrA(wnd, DWLP_USER, reinterpret_cast<LONG_PTR>(config));
					// Set default filename
					HWND edit = GetDlgItem(wnd, ID_CONFIG_FILENAME);
					char *filename = config->GetFile();
					wchar_t *filenamew = utf8_to_utf16(filename);
					delete[] filename;
					SendMessageW(edit, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(filenamew));
					delete[] filenamew;
					SendMessageW(edit, EM_SETSEL, 0, -1);
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
							ILVSVideoFilterConfiguration *config = reinterpret_cast<ILVSVideoFilterConfiguration*>(lParam);
							// Save filename
							HWND edit = GetDlgItem(wnd, ID_CONFIG_FILENAME);
							int text_len = static_cast<int>(SendMessageW(edit, WM_GETTEXTLENGTH, 0, 0));
							wchar_t *text_buf = new wchar_t[text_len+1];
							SendMessageW(edit, WM_GETTEXT, text_len+1, reinterpret_cast<LPARAM>(text_buf));
							char *text_buf_utf8 = utf16_to_utf8(text_buf);
							delete[] text_buf;
							config->SetFile(text_buf_utf8);
							delete[] text_buf_utf8;
							// Close dialog
							EndDialog(wnd, S_OK);
						}break;
						// 'Cancel' button
						case IDCANCEL:{
							// Close dialog
							EndDialog(wnd, E_UNEXPECTED);
						}break;
					}
				}break;
				// Dialog closure ('X' button)
				case WM_CLOSE:{
					EndDialog(wnd, E_UNEXPECTED);
			   }break;
				// Message not handled (default behaviour follows)
				default:
					return FALSE;
			}
			// Message handled
			return TRUE;
		}
		// Constructor (see 'CreateInstance')
		LVSVideoFilterPropertyPage(IUnknown *unknown) : CBasePropertyPage(FILTER_PROP_NAMEW, unknown, ID_CONFIG_DIALOG, ID_CONFIG_TITLE), config(NULL){}
	public:
		// Create class instance
		static CUnknown* CALLBACK CreateInstance(LPUNKNOWN unknown, HRESULT *result){
			LVSVideoFilterPropertyPage *page = new LVSVideoFilterPropertyPage(unknown);
			if(!page)
				*result = E_OUTOFMEMORY;
			return page;
		}
		// On property page connection
		HRESULT OnConnect(IUnknown *unknown){
			// Valid pointer?
			CheckPointer(unknown, E_POINTER);
			// Get interface to filter data
			if(!this->config)
				return unknown->QueryInterface(IID_ILVSVideoFilterConfiguration, reinterpret_cast<void**>(&this->config));
			else
				return VFW_E_NO_INTERFACE;
		}
		// On property page disconnection
		HRESULT OnDisconnect(){
			// Got interface to filter data?
			CheckPointer(this->config, E_ACCESSDENIED);
			// Free reference to filter data interface
			this->config->Release();
			this->config = NULL;
			// Success
			return S_OK;
		}
		// On property page activation
		HRESULT STDMETHODCALLTYPE Activate(HWND wnd, LPCRECT prect, BOOL modal){
			// Show configuration dialog and return result
			return DialogBoxParamW(DLL_INSTANCE, MAKEINTRESOURCE(this->m_DialogId), reinterpret_cast<HWND>(wnd), ConfigEventHandler, reinterpret_cast<LPARAM>(this->config));
		}
		// On property page deactivation
		HRESULT STDMETHODCALLTYPE Deactivate(){
			// Wayne
			return S_OK;
		}
		// On property page information request
		HRESULT STDMETHODCALLTYPE GetPageInfo(__out LPPROPPAGEINFO info){
			// Valid pointer?
			CheckPointer(info, E_POINTER);
			// Get property page title
			wchar_t title[STR_MAX_LENGTH];
			LoadStringW(DLL_INSTANCE, this->m_TitleId, title, sizeof(title));
			// Allocate managed dynamic memory in OLE for the property page title and description
			LPOLESTR ptitle, pdesc;
			HRESULT hr = AMGetWideString(title, &ptitle);
			if (FAILED(hr))
				return hr;
			hr = AMGetWideString(FILTER_DESCW, &pdesc);
			if (FAILED(hr))
				return hr;
			// Set property page information
			info->cb = sizeof(PROPPAGEINFO);
			info->pszTitle = ptitle;
			info->pszDocString = pdesc;
			info->pszHelpFile = NULL;
			info->dwHelpContext = 0;
			info->size.cx = 340;
			info->size.cy = 100;
			// Success
			return S_OK;
		}
};

// Filter
class LVSVideoFilter : public CVideoTransformFilter, public ILVSVideoFilterConfiguration, public ISpecifyPropertyPages{
	private:
		// LVS instance for filtering process
		LVS *lvs;
		// Image buffer for frame conversion
		CairoImage *image;
		// Critical section for save configuration access from other interfaces
		CCritSec  crit_section;
		// Configuration
		char *filename;
		// Constructor (see 'CreateInstance')
		LVSVideoFilter(IUnknown *unknown) : CVideoTransformFilter(FILTER_NAMEW, unknown, CLSID_LVSVideoFilter), lvs(NULL), image(NULL){
			// Initialize configuration with an empty string
			this->filename = new char;
			*this->filename = '\0';
		}
	public:
		// Create class instance
		static CUnknown* CALLBACK CreateInstance(LPUNKNOWN unknown, HRESULT *result){
			LVSVideoFilter *filter = new LVSVideoFilter(unknown);
			if(!filter)
				*result = E_OUTOFMEMORY;
			return filter;
		}
		// Destructor
		~LVSVideoFilter(){
			// Free LVS instance
			if(this->lvs)
				delete this->lvs;
			// Free image buffer
			if(this->image)
				delete this->image;
			// Free configuration
			if(this->filename)
				delete[] this->filename;
		}
		// Check validation of input media stream
		HRESULT CheckInputType(const CMediaType *In){
			// Valid pointer?
			CheckPointer(In, E_POINTER);
			// Valid stream type?
			if (In->majortype != MEDIATYPE_Video || (In->subtype != MEDIASUBTYPE_RGB24 && In->subtype != MEDIASUBTYPE_RGB32) ||
				In->formattype != FORMAT_VideoInfo || In->cbFormat < sizeof(VIDEOINFOHEADER))
				return VFW_E_TYPE_NOT_ACCEPTED;
			// Valid bitmap?
			VIDEOINFOHEADER *header = reinterpret_cast<VIDEOINFOHEADER*>(In->pbFormat);
			if (header->bmiHeader.biBitCount != 24 && header->bmiHeader.biBitCount != 32)
				return VFW_E_TYPE_NOT_ACCEPTED;
			// Media type accepted
			return S_OK;
		}
		// Prefered output media stream type
		HRESULT GetMediaType(int position, CMediaType *Out){
			// Valid pointer?
			CheckPointer(Out, E_POINTER);
			// Input pin isn't connected
			if(!m_pInput->IsConnected())
				return VFW_E_NOT_CONNECTED;
			// Item pick error
			if (position < 0)
				return E_ACCESSDENIED;
			// No further items
			if(position > 0)
				return VFW_S_NO_MORE_ITEMS;
			// Output type = input type
			HRESULT hr = m_pInput->ConnectionMediaType(Out);
			if (FAILED(hr))
				return hr;
			// Output accepted
			return S_OK;
		}
		// Checks compatibility of input & output pin
		HRESULT CheckTransform(const CMediaType *In, const CMediaType *Out){
			// Valid pointers?
			CheckPointer(In, E_POINTER);
			CheckPointer(Out, E_POINTER);
			// In- & output the same?
			if(this->CheckInputType(In) == S_OK && *In == *Out)
				return S_OK;
			else
				return VFW_E_INVALIDMEDIATYPE;
		}
		// Allocate buffers for in- & output
		HRESULT DecideBufferSize(IMemAllocator *alloc, ALLOCATOR_PROPERTIES *props){
			// Valid pointers?
			CheckPointer(alloc, E_POINTER);
			CheckPointer(props, E_POINTER);
			// Input pin isn't connected
			if (!m_pInput->IsConnected())
				return VFW_E_NOT_CONNECTED;
			// Set buffer size
			props->cBuffers = 1;
			props->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
			// Allocate buffer memory
			ALLOCATOR_PROPERTIES actual;
			HRESULT hr = alloc->SetProperties(props,&actual);
			if (FAILED(hr))
				return hr;
			// Enough memory allocated?
			if (actual.cBuffers < props->cBuffers ||
				actual.cbBuffer < props->cbBuffer)
						return E_OUTOFMEMORY;
			// Got memory
			return S_OK;
		}
		// Frame processing
		HRESULT Transform(IMediaSample *In, IMediaSample *Out){
			// Valid pointers?
			CheckPointer(In, E_POINTER);
			CheckPointer(Out, E_POINTER);
			// Set output size
			Out->SetActualDataLength(In->GetActualDataLength());
			// Declarations
			BYTE *src, *dst;
			LONGLONG start, end;
			HRESULT hr;
			// Get time
			hr = In->GetMediaTime(&start, &end);
			if(FAILED(hr))
				return hr;
			// Get frame pointers
			hr = In->GetPointer(&src);
			if(FAILED(hr))
				return hr;
			hr = Out->GetPointer(&dst);
			if(FAILED(hr))
				return hr;
			// Convert source frame to image
			this->image->Load(src, 0, CairoImage::DSHOW);
			// Filter image
			try{
				// Send image data through filter process
				this->lvs->RenderOnFrame(this->image, start-1);
			}catch(std::exception e){
				// Show UTF8 error message
				wchar_t *werr = utf8_to_utf16(e.what());
				MessageBoxW(0, werr, FILTER_NAMEW L" video error", MB_OK | MB_ICONWARNING);
				delete[] werr;
				// Return error code
				return E_FAIL;
			}
			// Convert image to destination frame
			this->image->Save(dst, 0, CairoImage::DSHOW);
			// Frame successfully filtered
			return S_OK;
		}
		// Start frame streaming
		HRESULT StartStreaming(){
			// Get video info
			VIDEOINFOHEADER *video = reinterpret_cast<VIDEOINFOHEADER*>(this->m_pInput->CurrentMediaType().pbFormat);
			BITMAPINFOHEADER *bmp = &video->bmiHeader;
			double fps = 10.0e+07L / video->AvgTimePerFrame;
			unsigned long frames = this->m_pInput->CurrentStopTime() / video->AvgTimePerFrame;
			// Free previous render data (in case of buggy twice start function call)
			if(this->lvs){
				delete this->lvs;
				this->lvs = NULL;
				delete[] this->image;
				this->image = NULL;
			}
			// Create LVS instance
			try{
				this->lvs = new LVS(this->filename, bmp->biWidth, bmp->biHeight, bmp->biBitCount == 32, fps, frames);
			}catch(std::exception e){
				// Show UTF8 error message
				wchar_t *werr = utf8_to_utf16(e.what());
				MessageBoxW(0, werr, FILTER_NAMEW L" initialization failed", MB_OK | MB_ICONWARNING);
				delete[] werr;
				// Return error code
				return VFW_E_WRONG_STATE;
			}
			// Create image buffer (4-bytes per pixel for cairo stride alignment)
			this->image = new CairoImage(bmp->biWidth, bmp->biHeight, bmp->biBitCount == 32);
			// Continue with default behaviour
			return CVideoTransformFilter::StartStreaming();
		}
		// Stop frame streaming
		HRESULT StopStreaming(){
			// Free render data
			if(this->lvs){
				delete this->lvs;
				this->lvs = NULL;
				delete this->image;
				this->image = NULL;
			}
			// Continue with default behaviour
			return CVideoTransformFilter::StopStreaming();
		}
		// Number of filter pins
		int GetPinCount(){
			return 2;
		}
		// Get filter pins
		CBasePin* GetPin(int n){
			// Pick pin by index
			switch(n){
				case 0:
					// Input pin
					if (!this->m_pInput){
						// Create new one
						HRESULT hr = S_OK;
						this->m_pInput = new CTransformInputPin(L"Video input", this, &hr, L"Video input");
						if (FAILED(hr))
							return NULL;
					}
					return this->m_pInput;
				case 1:
					// Output pin
					if (!this->m_pOutput){
						// Create new one
						HRESULT hr = S_OK;
						this->m_pOutput = new CTransformOutputPin(L"Video output", this, &hr, L"Video output");
						if (FAILED(hr))
							return NULL;
					}
					return this->m_pOutput;
				default:
					// Not expected pin
					return NULL;
			}
		}
		// Define COM object base methods
		DECLARE_IUNKNOWN;
		// Answer to interface requests from outside
		HRESULT CALLBACK NonDelegatingQueryInterface(REFIID riid, __deref_out void **ppv){
			// Valid pointer?
			CheckPointer(ppv, E_POINTER);
			// Return filter configuration interface
			if(riid == IID_ILVSVideoFilterConfiguration)
				return GetInterface(static_cast<ILVSVideoFilterConfiguration*>(this), ppv);
			// Return filter property page interface
			if(riid == IID_ISpecifyPropertyPages)
				return GetInterface(static_cast<ISpecifyPropertyPages*>(this), ppv);
			// Return default interfaces
			return CVideoTransformFilter::NonDelegatingQueryInterface(riid, ppv);
		}
		// Define property pages belonging to this filter
		HRESULT CALLBACK GetPages(CAUUID *pages){
			// Valid pointer?
			CheckPointer(pages, E_POINTER);
			// Number of property pages
			pages->cElems = 1;
			// Allocate managed memory of COM for property pages
			pages->pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID));
			if(!pages->pElems)
				return E_OUTOFMEMORY;
			// Set property page
			*pages->pElems = CLSID_LVSVideoFilterPropertyPage;
			// Successfully defined filter property page
			return S_OK;
		}
		// Filter configuration interface
		char* GetFile(){
			// Lock critical section for thread-safety
			CAutoLock lock(&this->crit_section);
			// Copy filename
			char *filename = new char[strlen(this->filename)+1];
			strcpy(filename, this->filename);
			// Return copy
			return filename;
		}
		void SetFile(char *filename){
			// Lock critical section for thread-safety
			CAutoLock lock(&this->crit_section);
			// Copy parameter to filename
			delete[] this->filename;
			this->filename = new char[strlen(filename)+1];
			strcpy(this->filename, filename);
		}
};

// Filter pins
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

// Filter setup
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
int g_cTemplates = sizeof(g_Templates) / sizeof(CFactoryTemplate);	// Number of COM objects

// Register filter to server
STDAPI DllRegisterServer(){
    return AMovieDllRegisterServer2( TRUE );
}

// Unregister filter from server
STDAPI DllUnregisterServer(){
    return AMovieDllRegisterServer2( FALSE );
}