 

#ifndef C4PrefixWindows_h
#define C4PrefixWindows_h


#define C4FASTBUILD		1


#pragma warning(disable: 4201)		// nonstandard extension used : nameless struct/union


#if !C4FASTBUILD


#define _WIN32_WINNT			0x0502
#define DIRECTINPUT_VERSION		0x0800

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOMENUS
#define NOICONS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCOLOR
#define NOMETAFILE
#define NOMINMAX
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#include <windows.h>

#undef GetObject
#undef CopyMemory
#undef FillMemory
#undef DeleteFile
#undef CreateDirectory
#undef SetCurrentDirectory
#undef AppendMenu
#undef SendMessage
#undef GetCurrentTime
#undef Yield

#include <winsock2.h>
#include <mmsystem.h>
#include <dinput.h>
#include <dsound.h>
#include <xinput.h>
#include <xaudio2.h>
#include <shlobj.h>
#include <wbemcli.h>
#include <oleauto.h>
#include <shellapi.h>
#include <math.h>
#include <intrin.h>
#include <gl/gl.h>

#undef near
#undef far

#include <new>


#else // C4FASTBUILD != 0


#define NULL 0

#define WINAPI							__stdcall
#define CALLBACK						__stdcall
#define PASCAL							__stdcall
#define STDAPICALLTYPE					__stdcall
#define STDMETHODCALLTYPE				__stdcall
#define APIENTRY						__stdcall
#define WSAAPI							__stdcall
#define WINBASEAPI						__declspec(dllimport)
#define WINUSERAPI						__declspec(dllimport)
#define WINGDIAPI						__declspec(dllimport)
#define WINADVAPI						__declspec(dllimport)
#define WINSOCK_API_LINKAGE				__declspec(dllimport)
#define WINOLEAPI						__declspec(dllimport) HRESULT STDAPICALLTYPE
#define WINOLEAPI_(type)				__declspec(dllimport) type STDAPICALLTYPE
#define WINOLEAUTAPI					__declspec(dllimport) HRESULT STDAPICALLTYPE
#define WINOLEAUTAPI_(type)				__declspec(dllimport) type STDAPICALLTYPE
#define SHSTDAPI_(type)					__declspec(dllimport) type STDAPICALLTYPE
#define SHFOLDERAPI						__declspec(dllimport) HRESULT STDAPICALLTYPE

#define MAX_PATH						260
#define INVALID_HANDLE_VALUE			((HANDLE) (LONG_PTR) -1)
 
#define FILE_BEGIN						0
#define GENERIC_READ					0x80000000L
#define GENERIC_WRITE					0x40000000L 
#define FILE_SHARE_READ					0x00000001
#define FILE_ATTRIBUTE_HIDDEN			0x00000002 
#define FILE_ATTRIBUTE_DIRECTORY		0x00000010
#define FILE_ATTRIBUTE_NORMAL			0x00000080
#define CREATE_ALWAYS					2 
#define OPEN_EXISTING					3
 
#define SEVERITY_ERROR					1 
#define FACILITY_WIN32					7
#define MAKE_HRESULT(sev, fac, code) ((HRESULT) (((unsigned long) (sev) << 31) | ((unsigned long) (fac) << 16) | ((unsigned long) (code))))

#define S_OK							((HRESULT) 0L) 
#define S_FALSE							((HRESULT) 1L)
#define ERROR_SUCCESS					0L
#define ERROR_ACCESS_DENIED				5L
#define ERROR_INVALID_ACCESS			12L
#define ERROR_WRITE_PROTECT				19L
#define ERROR_READ_FAULT				30L
#define ERROR_HANDLE_DISK_FULL			39L
#define ERROR_DISK_FULL					112L
#define ERROR_ALREADY_EXISTS			183L
#define GDI_ERROR						0xFFFFFFFFL

#define LANG_NEUTRAL					0x00
#define SUBLANG_SYS_DEFAULT				0x02
#define SORT_DEFAULT					0x00
#define MAKELANGID(p, s)				((((WORD) (s)) << 10) | (WORD) (p))
#define MAKELCID(lgid, srtid)			((DWORD) ((((DWORD) ((WORD) (srtid))) << 16) | ((DWORD) ((WORD) (lgid)))))
#define LANG_SYSTEM_DEFAULT				MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT)
#define LOCALE_SYSTEM_DEFAULT			MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT)

#define REG_SZ							1
#define REG_DWORD						4
#define KEY_QUERY_VALUE					0x0001
#define HKEY_LOCAL_MACHINE ((HKEY) (ULONG_PTR) ((LONG) 0x80000002))

#define PF_XMMI_INSTRUCTIONS_AVAILABLE		6
#define PF_XMMI64_INSTRUCTIONS_AVAILABLE	10
#define PF_SSE3_INSTRUCTIONS_AVAILABLE		13

#define THREAD_PRIORITY_LOWEST			-2
#define THREAD_PRIORITY_NORMAL			0
#define THREAD_PRIORITY_HIGHEST			2
#define THREAD_PRIORITY_TIME_CRITICAL   15

#define INFINITE						0xFFFFFFFF
#define WAIT_OBJECT_0					0UL
#define WAIT_TIMEOUT					258L

#define CF_UNICODETEXT					13
#define GMEM_MOVEABLE					0x0002
#define PM_REMOVE						0x0001
#define CS_OWNDC						0x0020
#define CS_NOCLOSE						0x0200

#define CCHDEVICENAME					32
#define CCHFORMNAME						32
#define DM_DISPLAYFREQUENCY				0x00400000L
#define DISPLAY_DEVICE_PRIMARY_DEVICE	0x00000004
#define DM_BITSPERPEL					0x00040000L
#define DM_PELSWIDTH					0x00080000L
#define DM_PELSHEIGHT					0x00100000L

#define SM_CXFULLSCREEN					16
#define SM_CYFULLSCREEN					17
#define CDS_FULLSCREEN					0x00000004
#define DISP_CHANGE_SUCCESSFUL			0

#define HWND_NOTOPMOST					(HWND) -2
#define GWL_STYLE						-16
#define SW_SHOWNORMAL					1
#define SW_MINIMIZE						6
#define SW_RESTORE						9
#define SWP_NOSIZE						0x0001
#define SWP_NOMOVE						0x0002
#define SWP_NOZORDER					0x0004
#define SWP_FRAMECHANGED				0x0020
#define SWP_SHOWWINDOW					0x0040
#define SWP_NOCOPYBITS					0x0100
#define WS_POPUP						0x80000000L
#define WS_CHILD						0x40000000L
#define WS_MINIMIZE						0x20000000L
#define WS_VISIBLE						0x10000000L
#define WS_CLIPCHILDREN					0x02000000L
#define WS_CAPTION						0x00C00000L
#define WS_BORDER						0x00800000L
#define WS_SYSMENU						0x00080000L
#define WS_MINIMIZEBOX					0x00020000L

#define WHEEL_DELTA						120
#define WM_CREATE						0x0001
#define WM_CHAR							0x0102
#define WM_DEADCHAR						0x0103
#define WM_CLOSE						0x0010
#define WM_ERASEBKGND					0x0014
#define WM_ACTIVATEAPP					0x001C
#define WM_SHOWWINDOW					0x0018
#define WM_NCMOUSEMOVE					0x00A0
#define WM_INPUT						0x00FF
#define WM_KEYDOWN						0x0100
#define WM_KEYUP						0x0101
#define WM_SYSKEYDOWN					0x0104
#define WM_SYSKEYUP						0x0105
#define WM_SYSCOMMAND					0x0112
#define WM_MOUSEMOVE					0x0200
#define WM_LBUTTONDOWN					0x0201
#define WM_LBUTTONUP					0x0202
#define WM_RBUTTONDOWN					0x0204
#define WM_RBUTTONUP					0x0205
#define WM_MBUTTONDOWN					0x0207
#define WM_MBUTTONUP					0x0208
#define WM_MOUSEWHEEL					0x020A
#define WM_DEVICECHANGE					0x0219
#define WM_APP							0x8000
#define SC_KEYMENU						0xF100
#define SC_SCREENSAVE					0xF140
#define SC_MONITORPOWER					0xF170

#define MB_OK							0x00000000L
#define MB_ICONINFORMATION				0x00000040L

#define PFD_TYPE_RGBA					0
#define PFD_MAIN_PLANE					0
#define PFD_DOUBLEBUFFER				0x00000001
#define PFD_DRAW_TO_WINDOW				0x00000004
#define PFD_SUPPORT_OPENGL				0x00000020
#define PFD_SWAP_EXCHANGE				0x00000200
#define PFD_GENERIC_ACCELERATED			0x00001000
#define PFD_SUPPORT_COMPOSITION			0x00008000

#define FW_NORMAL						400
#define FW_BOLD							700
#define ANSI_CHARSET					0
#define OUT_TT_PRECIS					4
#define CLIP_DEFAULT_PRECIS				0
#define ANTIALIASED_QUALITY				4
#define DEFAULT_PITCH					0
#define FF_DONTCARE						0
#define GGO_GRAY8_BITMAP				6

#define VK_SHIFT						0x10
#define VK_CONTROL						0x11
#define VK_MENU							0x12
#define VK_PRIOR						0x21
#define VK_END							0x23
#define VK_HOME							0x24
#define VK_LEFT							0x25
#define VK_UP							0x26
#define VK_RIGHT						0x27
#define VK_DOWN							0x28
#define VK_DELETE						0x2E
#define VK_F1							0x70
#define VK_LSHIFT						0xA0
#define VK_RSHIFT						0xA1
#define VK_LCONTROL						0xA2
#define VK_RCONTROL						0xA3
#define VK_LMENU						0xA4
#define VK_RMENU						0xA5

#define RIM_TYPEHID						2
#define RID_INPUT						0x10000003
#define RIDI_DEVICEINFO					0x2000000B
#define RIDEV_REMOVE					0x00000001

#define SEE_MASK_NOASYNC				0x00000100
#define SEE_MASK_FLAG_NO_UI				0x00000400

#define CSIDL_PERSONAL					0x0005
#define CSIDL_LOCAL_APPDATA				0x001C
#define CSIDL_APPDATA					0x001A
#define CSIDL_FLAG_CREATE				0x8000

#define AF_INET							2
#define IPPROTO_UDP						17
#define INADDR_ANY						0x00000000UL
#define INADDR_BROADCAST				0xFFFFFFFFUL
#define SIO_GET_BROADCAST_ADDRESS		0x48000005
#define SOCK_DGRAM						2
#define INVALID_SOCKET					(SOCKET) ~0
#define SOL_SOCKET						0xFFFF
#define SO_BROADCAST					0x0020
#define FD_READ							1
#define FD_WRITE						2
#define FD_MAX_EVENTS					10
#define MAXGETHOSTSTRUCT				1024
#define WSAEVENT						HANDLE
#define WSA_WAIT_EVENT_0				WAIT_OBJECT_0
#define WSA_INFINITE					INFINITE
#define WSADESCRIPTION_LEN				256
#define WSASYS_STATUS_LEN				128
#define MAX_PROTOCOL_CHAIN				7
#define WSAPROTOCOL_LEN					255
#define WSAGETASYNCERROR(x)				HIWORD(x)
#define WSAEMSGSIZE						10040L
#define WSAOVERLAPPED					OVERLAPPED


typedef char					CHAR;
typedef unsigned char			UCHAR;
typedef wchar_t					WCHAR;
typedef CHAR					*LPSTR;
typedef const CHAR				*LPCSTR;
typedef WCHAR					*LPWSTR;
typedef const WCHAR				*LPCWSTR;
typedef WCHAR					OLECHAR;
typedef OLECHAR					*BSTR;
typedef unsigned char			BYTE;
typedef unsigned char			*LPBYTE;
typedef short					SHORT;
typedef unsigned short			USHORT;
typedef int						INT;
typedef unsigned int			UINT;
typedef unsigned int			UINT32;
typedef long					LONG;
typedef long					*LPLONG;
typedef unsigned long			ULONG;
typedef unsigned long			DWORD;
typedef unsigned long			*LPDWORD;
typedef int						BOOL;
typedef unsigned char			BYTE;
typedef unsigned short			WORD;
typedef __int64					LONGLONG;
typedef unsigned __int64		ULONGLONG;
typedef ULONGLONG				DWORDLONG;
typedef unsigned __int64		UINT64;
typedef float					FLOAT;
typedef double					DOUBLE;
typedef WORD					ATOM;
typedef void					*HANDLE;
typedef HANDLE					HGLOBAL;

#ifdef _WIN64

	typedef __int64				INT_PTR;
	typedef unsigned __int64	UINT_PTR;
	typedef __int64				LONG_PTR;
	typedef unsigned __int64	ULONG_PTR;

#else

	typedef int					INT_PTR;
	typedef unsigned int		UINT_PTR;
	typedef long				LONG_PTR;
	typedef unsigned long		ULONG_PTR;

#endif

typedef ULONG_PTR				DWORD_PTR;
typedef ULONG_PTR				SIZE_T;
typedef UINT_PTR				WPARAM;
typedef LONG_PTR				LPARAM;
typedef LONG_PTR				LRESULT;
typedef long					HRESULT;
typedef LONG					LSTATUS;
typedef unsigned int			GROUP;
typedef UINT_PTR				SOCKET;
typedef unsigned short			u_short;
typedef DWORD					LCID;
typedef DWORD					REGSAM;

#ifdef _WIN64

	typedef INT_PTR				(WINAPI *PROC)();
	typedef INT_PTR				(WINAPI *FARPROC)();

#else

	typedef int					(WINAPI *PROC)();
	typedef int					(WINAPI *FARPROC)();

#endif


struct LARGE_INTEGER
{
	LONGLONG	QuadPart;
};

struct ULARGE_INTEGER
{
	ULONGLONG	QuadPart;
};


struct OVERLAPPED
{
	ULONG_PTR	Internal;
	ULONG_PTR	InternalHigh;

	union
	{
		struct
		{
			DWORD	Offset;
			DWORD	OffsetHigh;
		};

		void	*Pointer;
	};

	HANDLE		hEvent;
};

typedef OVERLAPPED *LPOVERLAPPED;
typedef void (CALLBACK WSAOVERLAPPED_COMPLETION_ROUTINE)(DWORD, DWORD, WSAOVERLAPPED *, DWORD);


enum
{
	COINIT_MULTITHREADED		= 0,
	COINIT_APARTMENTTHREADED	= 2,
	COINIT_DISABLE_OLE1DDE		= 4,
	COINIT_SPEED_OVER_MEMORY	= 8
};

enum
{
	CLSCTX_INPROC_SERVER		= 0x01
};


#define LOBYTE(w)						((BYTE) (((DWORD_PTR) (w)) & 0xFF))
#define LOWORD(x)						((WORD) (((DWORD_PTR) (x)) & 0xFFFF))
#define HIWORD(x)						((WORD) ((((DWORD_PTR) (x)) >> 16) & 0xFFFF))
#define MAKEINTRESOURCE(i)				((CHAR *) ((ULONG_PTR) ((WORD) (i))))
#define MAKEWORD(a, b)					((WORD) (((BYTE) (((DWORD_PTR) (a)) & 0xFF)) | ((WORD) ((BYTE) (((DWORD_PTR) (b)) & 0xFF))) << 8))
#define GET_WHEEL_DELTA_WPARAM(wParam)	((short) HIWORD(wParam))
#define SUCCEEDED(hr)					(((HRESULT) (hr)) >= 0)
#define FAILED(hr)						(((HRESULT) (hr)) < 0)


typedef struct _GUID
{
	unsigned long		Data1;
	unsigned short		Data2;
	unsigned short		Data3;
	unsigned char		Data4[8];
} GUID;

typedef GUID IID;
typedef GUID *LPGUID;
typedef const GUID *LPCGUID;
#define REFGUID const GUID&
#define REFIID const IID&
#define REFCLSID const IID&

struct __declspec(uuid("00000000-0000-0000-C000-000000000046")) __declspec(novtable) IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) = 0;
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

	template <class Q> HRESULT STDMETHODCALLTYPE QueryInterface(Q **pp)
	{
		return (QueryInterface(__uuidof(Q), (void **) pp));
	}
};

typedef IUnknown *LPUNKNOWN;
typedef void *RPC_AUTH_IDENTITY_HANDLE;

#define interface struct

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) extern "C" const GUID name

#define DECLSPEC_UUID_WRAPPER(x) __declspec(uuid(#x))
#define DEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) class DECLSPEC_UUID_WRAPPER(l##-##w1##-##w2##-##b1##b2##-##b3##b4##b5##b6##b7##b8) className; extern "C" const GUID CLSID_##className
#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) interface DECLSPEC_UUID_WRAPPER(l##-##w1##-##w2##-##b1##b2##-##b3##b4##b5##b6##b7##b8) interfaceName; extern "C" const GUID IID_##interfaceName

#define STDMETHOD(method) virtual __declspec(nothrow) HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type, method) virtual __declspec(nothrow) type STDMETHODCALLTYPE method
#define DECLARE_INTERFACE(iface) interface __declspec(novtable) iface
#define DECLARE_INTERFACE_(iface, baseiface) interface __declspec(novtable) iface : public baseiface


#define DECLARE_HANDLE(name) struct name##__ {int unused;}; typedef struct name##__ *name

DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HGLRC);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HKEY);
DECLARE_HANDLE(HRAWINPUT);
DECLARE_HANDLE(HFONT);

typedef HICON			HCURSOR;
typedef HINSTANCE		HMODULE;
typedef void			*HGDIOBJ;

#define IDC_ARROW MAKEINTRESOURCE(32512)


struct CRITICAL_SECTION;

struct LIST_ENTRY
{
	LIST_ENTRY	*Flink;
	LIST_ENTRY	*Blink;
};

struct CRITICAL_SECTION_DEBUG
{
	WORD				Type;
	WORD				CreatorBackTraceIndex;
	CRITICAL_SECTION	*CriticalSection;
	LIST_ENTRY			ProcessLocksList;
	DWORD				EntryCount;
	DWORD				ContentionCount;
	DWORD				Flags;
	WORD				CreatorBackTraceIndexHigh;
	WORD				SpareWORD;
};

struct CRITICAL_SECTION
{
	CRITICAL_SECTION_DEBUG		*DebugInfo;

	LONG			LockCount;
	LONG			RecursionCount;
	HANDLE			OwningThread;
	HANDLE			LockSemaphore;
	ULONG_PTR		SpinCount;
};


typedef DWORD (WINAPI THREAD_START_ROUTINE)(void *);
typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);


struct SECURITY_ATTRIBUTES
{
	DWORD		nLength;
	void		*lpSecurityDescriptor;
	BOOL		bInheritHandle;
};


struct WNDCLASSEXW
{
	UINT		cbSize;
	UINT		style;
	WNDPROC		lpfnWndProc;
	int			cbClsExtra;
	int			cbWndExtra;
	HINSTANCE	hInstance;
	HICON		hIcon;
	HCURSOR		hCursor;
	HBRUSH		hbrBackground;
	LPCWSTR		lpszMenuName;
	LPCWSTR		lpszClassName;
	HICON		hIconSm;
};


struct POINT
{
	LONG		x;
	LONG		y;
};

struct RECT
{
	LONG		left;
	LONG		top;
	LONG		right;
	LONG		bottom;
};


struct MSG
{
	HWND		hwnd;
	UINT		message;
	WPARAM		wParam;
	LPARAM		lParam;
	DWORD		time;
	POINT		pt;
};


struct SYSTEMTIME
{
	WORD		wYear;
	WORD		wMonth;
	WORD		wDayOfWeek;
	WORD		wDay;
	WORD		wHour;
	WORD		wMinute;
	WORD		wSecond;
	WORD		wMilliseconds;
};

struct FILETIME
{
	DWORD		dwLowDateTime;
	DWORD		dwHighDateTime;
};

struct WIN32_FIND_DATA
{
	DWORD		dwFileAttributes;
	FILETIME	ftCreationTime;
	FILETIME	ftLastAccessTime;
	FILETIME	ftLastWriteTime;
	DWORD		nFileSizeHigh;
	DWORD		nFileSizeLow;
	DWORD		dwReserved0;
	DWORD		dwReserved1;
	CHAR		cFileName[MAX_PATH];
	CHAR		cAlternateFileName[14];
};


struct OSVERSIONINFO
{
	DWORD		dwOSVersionInfoSize;
	DWORD		dwMajorVersion;
	DWORD		dwMinorVersion;
	DWORD		dwBuildNumber;
	DWORD		dwPlatformId;
	CHAR		szCSDVersion[128];
};

struct SYSTEM_INFO
{
	WORD		wProcessorArchitecture;
	WORD		wReserved;
	DWORD		dwPageSize;
	void		*lpMinimumApplicationAddress;
	void		*lpMaximumApplicationAddress;
	DWORD_PTR	dwActiveProcessorMask;
	DWORD		dwNumberOfProcessors;
	DWORD		dwProcessorType;
	DWORD		dwAllocationGranularity;
	WORD		wProcessorLevel;
	WORD		wProcessorRevision;
};

struct MEMORYSTATUSEX
{
	DWORD		dwLength;
	DWORD		dwMemoryLoad;
	DWORDLONG	ullTotalPhys;
	DWORDLONG	ullAvailPhys;
	DWORDLONG	ullTotalPageFile;
	DWORDLONG	ullAvailPageFile;
	DWORDLONG	ullTotalVirtual;
	DWORDLONG	ullAvailVirtual;
	DWORDLONG	ullAvailExtendedVirtual;
};


struct DEVMODE
{
	BYTE		dmDeviceName[CCHDEVICENAME];
	WORD		dmSpecVersion;
	WORD		dmDriverVersion;
	WORD		dmSize;
	WORD		dmDriverExtra;
	DWORD		dmFields;

	union
	{
		struct
		{
			short	dmOrientation;
			short	dmPaperSize;
			short	dmPaperLength;
			short	dmPaperWidth;
		};

		POINT		dmPosition;
	};

	short		dmScale;
	short		dmCopies;
	short		dmDefaultSource;
	short		dmPrintQuality;
	short		dmColor;
	short		dmDuplex;
	short		dmYResolution;
	short		dmTTOption;
	short		dmCollate;
	BYTE		dmFormName[CCHFORMNAME];
	WORD		dmLogPixels;
	DWORD		dmBitsPerPel;
	DWORD		dmPelsWidth;
	DWORD		dmPelsHeight;

	union
	{
		DWORD	dmDisplayFlags;
		DWORD	dmNup;
	};

	DWORD		dmDisplayFrequency;
	DWORD		dmICMMethod;
	DWORD		dmICMIntent;
	DWORD		dmMediaType;
	DWORD		dmDitherType;
	DWORD		dmReserved1;
	DWORD		dmReserved2;
	DWORD		dmPanningWidth;
	DWORD		dmPanningHeight;
};

struct DISPLAY_DEVICEA
{
	DWORD		cb;
	CHAR		DeviceName[32];
	CHAR		DeviceString[128];
	DWORD		StateFlags;
	CHAR		DeviceID[128];
	CHAR		DeviceKey[128];
};

struct PIXELFORMATDESCRIPTOR
{
	WORD		nSize;
	WORD		nVersion;
	DWORD		dwFlags;
	BYTE		iPixelType;
	BYTE		cColorBits;
	BYTE		cRedBits;
	BYTE		cRedShift;
	BYTE		cGreenBits;
	BYTE		cGreenShift;
	BYTE		cBlueBits;
	BYTE		cBlueShift;
	BYTE		cAlphaBits;
	BYTE		cAlphaShift;
	BYTE		cAccumBits;
	BYTE		cAccumRedBits;
	BYTE		cAccumGreenBits;
	BYTE		cAccumBlueBits;
	BYTE		cAccumAlphaBits;
	BYTE		cDepthBits;
	BYTE		cStencilBits;
	BYTE		cAuxBuffers;
	BYTE		iLayerType;
	BYTE		bReserved;
	DWORD		dwLayerMask;
	DWORD		dwVisibleMask;
	DWORD		dwDamageMask;
};

struct CREATESTRUCTW
{
	void		*lpCreateParams;
	HINSTANCE	hInstance;
	HMENU		hMenu;
	HWND		hwndParent;
	int			cy;
	int			cx;
	int			y;
	int			x;
	LONG		style;
	LPCWSTR		lpszName;
	LPCWSTR		lpszClass;
	DWORD		dwExStyle;
};


struct WCRANGE
{
	WCHAR		wcLow;
	USHORT		cGlyphs;
};

struct GLYPHSET
{
	DWORD		cbThis;
	DWORD		flAccel;
	DWORD		cGlyphsSupported;
	DWORD		cRanges;
	WCRANGE		ranges[1];
};

struct LOGFONTA
{
	LONG		lfHeight;
	LONG		lfWidth;
	LONG		lfEscapement;
	LONG		lfOrientation;
	LONG		lfWeight;
	BYTE		lfItalic;
	BYTE		lfUnderline;
	BYTE		lfStrikeOut;
	BYTE		lfCharSet;
	BYTE		lfOutPrecision;
	BYTE		lfClipPrecision;
	BYTE		lfQuality;
	BYTE		lfPitchAndFamily;
	CHAR		lfFaceName[32];
};

#pragma pack(push, 4)

struct TEXTMETRICA
{
	LONG		tmHeight;
	LONG		tmAscent;
	LONG		tmDescent;
	LONG		tmInternalLeading;
	LONG		tmExternalLeading;
	LONG		tmAveCharWidth;
	LONG		tmMaxCharWidth;
	LONG		tmWeight;
	LONG		tmOverhang;
	LONG		tmDigitizedAspectX;
	LONG		tmDigitizedAspectY;
	BYTE		tmFirstChar;
	BYTE		tmLastChar;
	BYTE		tmDefaultChar;
	BYTE		tmBreakChar;
	BYTE		tmItalic;
	BYTE		tmUnderlined;
	BYTE		tmStruckOut;
	BYTE		tmPitchAndFamily;
	BYTE		tmCharSet;
};

#pragma pack(pop)

struct FIXED
{
	WORD		fract;
	short		value;
};

struct MAT2
{
	FIXED		eM11;
	FIXED		eM12;
	FIXED		eM21;
	FIXED		eM22;
};

struct GLYPHMETRICS
{
	UINT		gmBlackBoxX;
	UINT		gmBlackBoxY;
	POINT		gmptGlyphOrigin;
	short		gmCellIncX;
	short		gmCellIncY;
};

typedef int (CALLBACK *FONTENUMPROCA)(const LOGFONTA *, const TEXTMETRICA *, DWORD, LPARAM);


struct RAWINPUTHEADER
{
	DWORD		dwType;
	DWORD		dwSize;
	HANDLE		hDevice;
	WPARAM		wParam;
};

struct RAWMOUSE
{
	USHORT		usFlags;
	union
	{
		ULONG	ulButtons;
		struct
		{
			USHORT	usButtonFlags;
			USHORT	usButtonData;
		};
	};
	ULONG		ulRawButtons;
	LONG		lLastX;
	LONG		lLastY;
	ULONG		ulExtraInformation;
};

struct RAWKEYBOARD
{
	USHORT		MakeCode;
	USHORT		Flags;
	USHORT		Reserved;
	USHORT		VKey;
	UINT		Message;
	ULONG		ExtraInformation;
};

struct RAWHID
{
	DWORD		dwSizeHid;
	DWORD		dwCount;
	BYTE		bRawData[1];
};

struct RAWINPUT
{
	RAWINPUTHEADER header;
	union
	{
		RAWMOUSE		mouse;
		RAWKEYBOARD		keyboard;
		RAWHID			hid;
	} data;
};

struct RID_DEVICE_INFO_MOUSE
{
	DWORD		dwId;
	DWORD		dwNumberOfButtons;
	DWORD		dwSampleRate;
	BOOL		fHasHorizontalWheel;
};

struct RID_DEVICE_INFO_KEYBOARD
{
	DWORD		 dwType;
	DWORD		 dwSubType;
	DWORD		 dwKeyboardMode;
	DWORD		 dwNumberOfFunctionKeys;
	DWORD		 dwNumberOfIndicators;
	DWORD		 dwNumberOfKeysTotal;
};

struct RID_DEVICE_INFO_HID
{
	DWORD		dwVendorId;
	DWORD		dwProductId;
	DWORD		dwVersionNumber;
	USHORT		usUsagePage;
	USHORT		usUsage;
};

struct RID_DEVICE_INFO
{
	DWORD		cbSize;
	DWORD		dwType;
	union
	{
		RID_DEVICE_INFO_MOUSE		mouse;
		RID_DEVICE_INFO_KEYBOARD	keyboard;
		RID_DEVICE_INFO_HID			hid;
	};
};

struct RAWINPUTDEVICE
{
	USHORT		usUsagePage;
	USHORT		usUsage;
	DWORD		dwFlags;
	HWND		hwndTarget;
};


struct SHELLEXECUTEINFOA
{
	DWORD		cbSize;
	ULONG		fMask;
	HWND		hwnd;
	LPCSTR		lpVerb;
	LPCSTR		lpFile;
	LPCSTR		lpParameters;
	LPCSTR		lpDirectory;
	int			nShow;
	HINSTANCE	hInstApp;
	void		*lpIDList;
	LPCSTR		lpClass;
	HKEY		hkeyClass;
	DWORD		dwHotKey;
	union
	{
		HANDLE	hIcon;
		HANDLE	hMonitor;
	} DUMMYUNIONNAME;
	HANDLE		hProcess;
};


union CY
{
	struct
	{
		unsigned long	Lo;
		long			Hi;
	};
	LONGLONG	int64;
};

struct DECIMAL
{
	USHORT wReserved;
	union
	{
		struct
		{
			BYTE	scale;
			BYTE	sign;
		};
		USHORT	signscale;
	};
	ULONG Hi32;
	union
	{
		struct
		{
			ULONG	Lo32;
			ULONG	Mid32;
		};
		ULONGLONG	Lo64;
	};
};

struct SAFEARRAYBOUND
{
	ULONG	cElements;
	LONG	lLbound;
};

struct SAFEARRAY
{
	USHORT	cDims;
	USHORT	fFeatures;
	ULONG	cbElements;
	ULONG	cLocks;
	void	*pvData;
	SAFEARRAYBOUND rgsabound[1];
};

typedef unsigned short VARTYPE;
typedef short VARIANT_BOOL;
typedef LONG SCODE;
typedef double DATE;
typedef interface IDispatch IDispatch;
typedef interface IRecordInfo IRecordInfo;

struct VARIANT
{
	union
	{
		struct
		{
			VARTYPE vt;
			WORD wReserved1;
			WORD wReserved2;
			WORD wReserved3;
			union
			{
				LONGLONG llVal;
				LONG lVal;
				BYTE bVal;
				SHORT iVal;
				FLOAT fltVal;
				DOUBLE dblVal;
				VARIANT_BOOL boolVal;
				SCODE scode;
				CY cyVal;
				DATE date;
				BSTR bstrVal;
				IUnknown *punkVal;
				IDispatch *pdispVal;
				SAFEARRAY *parray;
				BYTE *pbVal;
				SHORT *piVal;
				LONG *plVal;
				LONGLONG *pllVal;
				FLOAT *pfltVal;
				DOUBLE *pdblVal;
				VARIANT_BOOL *pboolVal;
				SCODE *pscode;
				CY *pcyVal;
				DATE *pdate;
				BSTR *pbstrVal;
				IUnknown **ppunkVal;
				IDispatch **ppdispVal;
				SAFEARRAY **pparray;
				VARIANT *pvarVal;
				void *byref;
				CHAR cVal;
				USHORT uiVal;
				ULONG ulVal;
				ULONGLONG ullVal;
				INT intVal;
				UINT uintVal;
				DECIMAL *pdecVal;
				CHAR *pcVal;
				USHORT *puiVal;
				ULONG *pulVal;
				ULONGLONG *pullVal;
				INT *pintVal;
				UINT *puintVal;
				struct
				{
					void *pvRecord;
					IRecordInfo *pRecInfo;
				};
			};
		};
		DECIMAL decVal;
	};
};


struct WSADATA
{
	WORD				wVersion;
	WORD				wHighVersion;

	#ifdef _WIN64

		unsigned short	iMaxSockets;
		unsigned short	iMaxUdpDg;
		char			*lpVendorInfo;
		char			szDescription[WSADESCRIPTION_LEN + 1];
		char			szSystemStatus[WSASYS_STATUS_LEN + 1];

	#else

		char			szDescription[WSADESCRIPTION_LEN + 1];
		char			szSystemStatus[WSASYS_STATUS_LEN + 1];
		unsigned short	iMaxSockets;
		unsigned short	iMaxUdpDg;
		char			*lpVendorInfo;

	#endif
};

struct HOSTENT
{
	char	*h_name;
	char	**h_aliases;
	short	h_addrtype;
	short	h_length;
	char	**h_addr_list;

	#define h_addr h_addr_list[0]
};

struct IN_ADDR
{
	union
	{
		struct
		{
			UCHAR	s_b1,s_b2,s_b3,s_b4;
		} S_un_b;

		struct
		{
			USHORT	s_w1,s_w2;
		} S_un_w;

		ULONG S_addr;
	} S_un;

	#define s_addr		S_un.S_addr
	#define s_host		S_un.S_un_b.s_b2
	#define s_net		S_un.S_un_b.s_b1
	#define s_imp		S_un.S_un_w.s_w2
	#define s_impno		S_un.S_un_b.s_b4
	#define s_lh		S_un.S_un_b.s_b3
};

struct SOCKADDR_IN
{
	short		sin_family;
	USHORT		sin_port;
	IN_ADDR		sin_addr;
	CHAR		sin_zero[8];
};

struct SOCKADDR
{
	u_short		sa_family;
	CHAR		sa_data[14];
};

struct WSAPROTOCOLCHAIN
{
	int			ChainLen;
	DWORD		ChainEntries[MAX_PROTOCOL_CHAIN];
};

struct WSAPROTOCOL_INFOW
{
	DWORD				dwServiceFlags1;
	DWORD				dwServiceFlags2;
	DWORD				dwServiceFlags3;
	DWORD				dwServiceFlags4;
	DWORD				dwProviderFlags;
	GUID				ProviderId;
	DWORD				dwCatalogEntryId;
	WSAPROTOCOLCHAIN	ProtocolChain;
	int					iVersion;
	int					iAddressFamily;
	int					iMaxSockAddr;
	int					iMinSockAddr;
	int					iSocketType;
	int					iProtocol;
	int					iProtocolMaxOffset;
	int					iNetworkByteOrder;
	int					iSecurityScheme;
	DWORD				dwMessageSize;
	DWORD				dwProviderReserved;
	WCHAR				szProtocol[WSAPROTOCOL_LEN + 1];
};

struct WSANETWORKEVENTS
{
	long	lNetworkEvents;
	int		iErrorCode[FD_MAX_EVENTS];
};

struct WSABUF
{
	ULONG	len;
	CHAR	*buf;
};


extern "C"
{
	unsigned char _BitScanReverse(unsigned long *, unsigned long);
	#pragma intrinsic(_BitScanReverse)

	long _InterlockedAnd(volatile long *, long);
	#pragma intrinsic(_InterlockedAnd)

	long _InterlockedOr(volatile long *, long);
	#pragma intrinsic(_InterlockedOr)

	#ifdef _WIN64

		void __faststorefence(void);
		#define MemoryBarrier __faststorefence

	#else

		__forceinline void MemoryBarrier(void)
		{
			LONG barrier;
			__asm { xchg barrier,eax }
		}

	#endif

	WINBASEAPI DWORD WINAPI GetLastError(void);
	WINBASEAPI void WINAPI RaiseException(DWORD, DWORD, DWORD, const ULONG_PTR *);
	WINBASEAPI void WINAPI GetSystemInfo(SYSTEM_INFO *);
	WINBASEAPI BOOL WINAPI IsProcessorFeaturePresent(DWORD);
	WINBASEAPI BOOL WINAPI GlobalMemoryStatusEx(MEMORYSTATUSEX *);
	WINBASEAPI BOOL WINAPI CloseHandle(HANDLE);
	WINBASEAPI HGLOBAL WINAPI GlobalAlloc(UINT, SIZE_T);
	WINBASEAPI void *WINAPI GlobalLock(HGLOBAL);
	WINBASEAPI BOOL WINAPI GlobalUnlock(HGLOBAL);
	WINBASEAPI void *WINAPI HeapAlloc(HANDLE, DWORD, SIZE_T);
	WINBASEAPI BOOL WINAPI HeapFree(HANDLE, DWORD, void *);
	WINBASEAPI BOOL WINAPI VirtualLock(void *, SIZE_T);
	WINBASEAPI BOOL WINAPI VirtualUnlock(void *, SIZE_T);
	WINBASEAPI HANDLE WINAPI GetProcessHeap(void);
	WINBASEAPI DWORD WINAPI GetModuleFileNameA(HMODULE, LPSTR, DWORD);
	WINBASEAPI HANDLE WINAPI CreateFileA(const CHAR *, DWORD, DWORD, SECURITY_ATTRIBUTES *, DWORD, DWORD, HANDLE);
	WINBASEAPI BOOL WINAPI GetFileSizeEx(HANDLE, LARGE_INTEGER *);
	WINBASEAPI DWORD WINAPI SetFilePointerEx(HANDLE, LARGE_INTEGER, LARGE_INTEGER *, DWORD);
	WINBASEAPI BOOL WINAPI SetEndOfFile(HANDLE);
	WINBASEAPI BOOL WINAPI ReadFile(HANDLE, void *, DWORD, DWORD *, OVERLAPPED *);
	WINBASEAPI BOOL WINAPI WriteFile(HANDLE, const void *, DWORD, DWORD *, OVERLAPPED *);
	WINBASEAPI BOOL WINAPI DeleteFileA(const CHAR *);
	WINBASEAPI BOOL WINAPI CreateDirectoryA(const CHAR *, SECURITY_ATTRIBUTES *);
	WINBASEAPI DWORD WINAPI GetFullPathNameA(LPCSTR, DWORD, LPSTR, LPSTR *);
	WINBASEAPI HANDLE WINAPI FindFirstFileA(const CHAR *, WIN32_FIND_DATA *);
	WINBASEAPI BOOL WINAPI FindNextFileA(HANDLE, WIN32_FIND_DATA *);
	WINBASEAPI BOOL WINAPI FindClose(HANDLE);
	WINBASEAPI BOOL WINAPI SetCurrentDirectoryA(LPCSTR);
	WINBASEAPI HANDLE WINAPI CreateEventA(SECURITY_ATTRIBUTES *, BOOL, BOOL, const CHAR *);
	WINBASEAPI HANDLE WINAPI CreateThread(SECURITY_ATTRIBUTES *, SIZE_T, THREAD_START_ROUTINE *, void *, DWORD, DWORD *);
	WINBASEAPI HANDLE WINAPI CreateMutexA(SECURITY_ATTRIBUTES *, BOOL, const CHAR *);
	WINBASEAPI BOOL WINAPI ReleaseMutex(HANDLE);
	WINBASEAPI BOOL WINAPI SetEvent(HANDLE);
	WINBASEAPI BOOL WINAPI SetThreadPriority(HANDLE, int);
	WINBASEAPI DWORD WINAPI WaitForSingleObjectEx(HANDLE, DWORD, BOOL);
	WINBASEAPI DWORD WINAPI WaitForMultipleObjectsEx(DWORD, const HANDLE *, BOOL, DWORD, BOOL);
	WINBASEAPI void WINAPI InitializeCriticalSection(CRITICAL_SECTION *);
	WINBASEAPI void WINAPI DeleteCriticalSection(CRITICAL_SECTION *);
	WINBASEAPI void WINAPI EnterCriticalSection(CRITICAL_SECTION *);
	WINBASEAPI BOOL WINAPI TryEnterCriticalSection(CRITICAL_SECTION *);
	WINBASEAPI void WINAPI LeaveCriticalSection(CRITICAL_SECTION *);
	WINBASEAPI void WINAPI Sleep(DWORD);
	WINBASEAPI BOOL WINAPI SwitchToThread(void);
	WINBASEAPI DWORD WINAPI GetCurrentThreadId(void);
	WINBASEAPI BOOL WINAPI GetExitCodeThread(HANDLE, LPDWORD);
	WINBASEAPI HMODULE WINAPI LoadLibraryA(const CHAR *);
	WINBASEAPI BOOL WINAPI FreeLibrary(HMODULE);
	WINBASEAPI FARPROC WINAPI GetProcAddress(HMODULE, const CHAR *);
	WINBASEAPI BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER *);
	WINBASEAPI BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER *);
	WINBASEAPI DWORD WINAPI GetTickCount(void);
	WINBASEAPI void WINAPI GetSystemTime(SYSTEMTIME *);
	WINBASEAPI void WINAPI GetLocalTime(SYSTEMTIME *);
	WINBASEAPI int WINAPI GetDateFormatA(LCID, DWORD, const SYSTEMTIME *, const CHAR *, CHAR *, int);
	WINBASEAPI int WINAPI GetTimeFormatA(LCID, DWORD, const SYSTEMTIME *, const CHAR *, CHAR *, int);
	WINUSERAPI ATOM WINAPI RegisterClassExW(const WNDCLASSEXW *);
	WINUSERAPI BOOL WINAPI UnregisterClassW(LPCWSTR, HINSTANCE);
	WINUSERAPI HWND WINAPI CreateWindowExW(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, void *);
	WINUSERAPI BOOL WINAPI DestroyWindow(HWND);
	WINUSERAPI BOOL WINAPI ShowWindow(HWND, int);
	WINUSERAPI BOOL WINAPI AdjustWindowRectEx(RECT *, DWORD, BOOL, DWORD);
	WINUSERAPI BOOL WINAPI ScreenToClient(HWND, POINT *);
	WINUSERAPI BOOL WINAPI SetWindowPos(HWND, HWND, int, int, int, int, UINT);
	WINUSERAPI LONG WINAPI SetWindowLongA(HWND, int, LONG);
	#ifdef _WIN64
		WINUSERAPI LONG_PTR WINAPI SetWindowLongPtrA(HWND, int, LONG_PTR);
	#else
		#define SetWindowLongPtrA SetWindowLongA
	#endif
	WINUSERAPI LRESULT WINAPI DefWindowProcW(HWND, UINT, WPARAM, LPARAM);
	WINUSERAPI HWND WINAPI GetCapture(void);
	WINUSERAPI HWND WINAPI SetCapture(HWND);
	WINUSERAPI BOOL WINAPI ReleaseCapture(void);
	WINUSERAPI HWND WINAPI SetFocus(HWND);
	WINUSERAPI HCURSOR WINAPI SetCursor(HCURSOR);
	WINUSERAPI int WINAPI ToUnicode(UINT, UINT, const BYTE *, LPWSTR, int, UINT);
	WINUSERAPI BOOL WINAPI PeekMessageA(MSG *, HWND, UINT, UINT, UINT);
	WINUSERAPI BOOL WINAPI TranslateMessage(const MSG *);
	WINUSERAPI LRESULT WINAPI DispatchMessageA(const MSG *);
	WINUSERAPI SHORT WINAPI GetAsyncKeyState(int);
	WINUSERAPI BOOL WINAPI GetKeyboardState(BYTE *lpKeyState);
	WINUSERAPI BOOL WINAPI EnumDisplayDevicesA(LPCSTR, DWORD, DISPLAY_DEVICEA *, DWORD);
	WINUSERAPI BOOL WINAPI EnumDisplaySettingsA(const CHAR *, DWORD, DEVMODE *);
	WINUSERAPI LONG WINAPI ChangeDisplaySettingsA(DEVMODE *, DWORD);
	WINUSERAPI int WINAPI GetSystemMetrics(int);
	WINUSERAPI UINT WINAPI GetDoubleClickTime(void);
	WINUSERAPI UINT WINAPI GetCaretBlinkTime(void);
	WINUSERAPI HICON WINAPI LoadIconA(HINSTANCE, LPCSTR);
	WINUSERAPI HCURSOR WINAPI LoadCursorA(HINSTANCE, LPCSTR);
	WINUSERAPI int WINAPI ShowCursor(BOOL);
	WINUSERAPI BOOL WINAPI OpenClipboard(HWND);
	WINUSERAPI BOOL WINAPI CloseClipboard(void);
	WINUSERAPI BOOL WINAPI EmptyClipboard(void);
	WINUSERAPI HANDLE WINAPI GetClipboardData(UINT);
	WINUSERAPI HANDLE WINAPI SetClipboardData(UINT, HANDLE);
	WINUSERAPI int WINAPI MessageBoxA(HWND, LPCSTR, LPCSTR, UINT);
	WINUSERAPI UINT WINAPI GetRawInputData(HRAWINPUT, UINT, void *, UINT *, UINT);
	WINUSERAPI UINT WINAPI GetRawInputDeviceInfoA(HANDLE, UINT, void *, UINT *);
	WINUSERAPI BOOL WINAPI RegisterRawInputDevices(const RAWINPUTDEVICE *, UINT, UINT);
	WINUSERAPI HDC WINAPI GetDC(HWND);
	WINUSERAPI int WINAPI ReleaseDC(HWND, HDC);
	WINGDIAPI BOOL WINAPI DeleteDC(HDC);
	WINGDIAPI HDC WINAPI CreateCompatibleDC(HDC);
	WINGDIAPI int WINAPI ChoosePixelFormat(HDC, const PIXELFORMATDESCRIPTOR *);
	WINGDIAPI BOOL WINAPI SetPixelFormat(HDC, int, const PIXELFORMATDESCRIPTOR *);
	WINGDIAPI HGLRC WINAPI wglCreateContext(HDC);
	WINGDIAPI BOOL WINAPI wglDeleteContext(HGLRC);
	WINGDIAPI BOOL WINAPI wglShareLists(HGLRC, HGLRC);
	WINGDIAPI BOOL WINAPI wglMakeCurrent(HDC, HGLRC);
	WINGDIAPI PROC WINAPI wglGetProcAddress(LPCSTR);
	WINGDIAPI BOOL WINAPI SwapBuffers(HDC);
	WINGDIAPI HGDIOBJ WINAPI SelectObject(HDC, HGDIOBJ);
	WINGDIAPI BOOL WINAPI DeleteObject(HGDIOBJ);
	WINGDIAPI BOOL WINAPI GetTextMetricsA(HDC, TEXTMETRICA *);
	WINGDIAPI int WINAPI EnumFontFamiliesExA(HDC hdc, LOGFONTA *, FONTENUMPROCA, LPARAM, DWORD);
	WINGDIAPI HFONT WINAPI CreateFontA(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR);
	WINGDIAPI DWORD WINAPI GetFontUnicodeRanges(HDC, GLYPHSET *);
	WINGDIAPI DWORD WINAPI GetGlyphOutlineW(HDC, UINT, UINT, GLYPHMETRICS *, DWORD, void *, const MAT2 *);
	WINADVAPI LSTATUS APIENTRY RegOpenKeyExA(HKEY, LPCSTR, DWORD, REGSAM, HKEY *);
	WINADVAPI LSTATUS APIENTRY RegQueryValueExA(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
	WINOLEAPI CoInitializeEx(void *, DWORD);
	WINOLEAPI_(void) CoUninitialize(void);
	WINOLEAPI CoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, void **);
	WINOLEAPI CoSetProxyBlanket(IUnknown *, DWORD, DWORD, OLECHAR *, DWORD, DWORD, RPC_AUTH_IDENTITY_HANDLE, DWORD);
	WINOLEAUTAPI_(BSTR) SysAllocString(const OLECHAR *);
	WINOLEAUTAPI_(void) SysFreeString(BSTR);
	WINOLEAUTAPI VariantClear(VARIANT *);
	SHSTDAPI_(BOOL) ShellExecuteExA(SHELLEXECUTEINFOA *);
	SHFOLDERAPI SHGetFolderPathA(HWND, int, HANDLE, DWORD, LPSTR);
	WINSOCK_API_LINKAGE int WSAAPI WSAStartup(WORD, WSADATA *);
	WINSOCK_API_LINKAGE int WSAAPI WSACleanup(void);
	WINSOCK_API_LINKAGE SOCKET WSAAPI WSASocketW(int, int, int, WSAPROTOCOL_INFOW *, GROUP, DWORD);
	WINSOCK_API_LINKAGE WSAEVENT WSAAPI WSACreateEvent(void);
	WINSOCK_API_LINKAGE int WSAAPI WSAEventSelect(SOCKET, WSAEVENT, long);
	WINSOCK_API_LINKAGE BOOL WSAAPI WSASetEvent(WSAEVENT);
	WINSOCK_API_LINKAGE DWORD WSAAPI WSAWaitForMultipleEvents(DWORD, const WSAEVENT *, BOOL, DWORD, BOOL);
	WINSOCK_API_LINKAGE BOOL WSAAPI WSACloseEvent(WSAEVENT);
	WINSOCK_API_LINKAGE int WSAAPI WSAEnumNetworkEvents(SOCKET, WSAEVENT, WSANETWORKEVENTS *);
	WINSOCK_API_LINKAGE int WSAAPI WSASendTo(SOCKET, WSABUF *, DWORD, DWORD *, DWORD, SOCKADDR *, int, WSAOVERLAPPED *, WSAOVERLAPPED_COMPLETION_ROUTINE *);
	WINSOCK_API_LINKAGE int WSAAPI WSARecvFrom(SOCKET, WSABUF *, DWORD, DWORD *, DWORD *, SOCKADDR *, INT *, WSAOVERLAPPED *, WSAOVERLAPPED_COMPLETION_ROUTINE *);
	WINSOCK_API_LINKAGE int WSAAPI WSAIoctl(SOCKET, DWORD, void *, DWORD, void *, DWORD, DWORD *, WSAOVERLAPPED *, WSAOVERLAPPED_COMPLETION_ROUTINE *);
	WINSOCK_API_LINKAGE HANDLE WSAAPI WSAAsyncGetHostByName(HWND, unsigned int, const char *, char *, int);
	WINSOCK_API_LINKAGE int WSAAPI WSACancelAsyncRequest(HANDLE);
	WINSOCK_API_LINKAGE int WSAAPI WSAGetLastError(void);
	WINSOCK_API_LINKAGE unsigned short WSAAPI htons(unsigned short);
	WINSOCK_API_LINKAGE unsigned short WSAAPI ntohs(unsigned short);
	WINSOCK_API_LINKAGE unsigned long WSAAPI htonl(unsigned long);
	WINSOCK_API_LINKAGE unsigned long WSAAPI ntohl(unsigned long);
	WINSOCK_API_LINKAGE int WSAAPI bind(SOCKET, const SOCKADDR *, int);
	WINSOCK_API_LINKAGE int WSAAPI setsockopt(SOCKET, int, int, const char *, int);
	WINSOCK_API_LINKAGE int WSAAPI closesocket(SOCKET);
	WINSOCK_API_LINKAGE int WSAAPI getsockname(SOCKET, SOCKADDR *, int *);
	WINSOCK_API_LINKAGE int WSAAPI gethostname(char *, int);
	WINSOCK_API_LINKAGE HOSTENT *WSAAPI gethostbyname(const char *);
}


#ifdef _WIN64

	typedef unsigned __int64 size_t;
	typedef __int64 ptrdiff_t;

#else

	typedef unsigned int size_t;
	typedef int ptrdiff_t;

#endif

namespace std
{
	using ::ptrdiff_t;
	using ::size_t;
}

extern "C"
{
	void *__cdecl memcpy(void *, const void *, size_t);
	#pragma intrinsic(memcpy)

	void *__cdecl memset(void *, int, size_t);
	#pragma intrinsic(memset)
}

void *__cdecl operator new(size_t);
void *__cdecl operator new[](size_t);
void __cdecl operator delete(void *);
void __cdecl operator delete[](void *);

inline void *__cdecl operator new(size_t, void *ptr)
{
	return (ptr);
}

inline void *__cdecl operator new[](size_t, void *ptr)
{
	return (ptr);
}

inline void __cdecl operator delete(void *, void *)
{
}

inline void __cdecl operator delete[](void *, void *)
{
}


#endif // C4FASTBUILD


#endif

// ZYUQURM
