 

#ifndef C4DirectSound_h
#define C4DirectSound_h


#include "C4XAudio.h"


#define DS_OK				S_OK
#define DSCBSTART_LOOPING	0x00000001

DEFINE_GUID(IID_IDirectSoundCapture, 0xb0210781, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16);
DEFINE_GUID(IID_IDirectSoundCaptureBuffer, 0xb0210782, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16);
DEFINE_GUID(IID_IDirectSoundNotify, 0xb0210783, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16);
DEFINE_GUID(DSDEVID_DefaultVoiceCapture, 0xdef00003, 0x9c6d, 0x47ed, 0xaa, 0xf1, 0x4d, 0xda, 0x8f, 0x2b, 0x5c, 0x03);

struct IDirectSoundCapture;
struct IDirectSoundCaptureBuffer;

typedef struct IDirectSoundCapture			*LPDIRECTSOUNDCAPTURE;
typedef struct IDirectSoundCaptureBuffer	*LPDIRECTSOUNDCAPTUREBUFFER;

typedef struct
{
	DWORD		dwSize;
	DWORD		dwFlags;
	DWORD		dwFormats;
	DWORD		dwChannels;
} DSCCAPS, *LPDSCCAPS;
typedef const DSCCAPS *LPCDSCCAPS;

typedef struct
{
	DWORD		dwSize;
	DWORD		dwFlags;
	DWORD		dwBufferBytes;
	DWORD		dwReserved;
} DSCBCAPS, *LPDSCBCAPS;
typedef const DSCBCAPS *LPCDSCBCAPS;

typedef struct
{
	DWORD		dwSize;
	DWORD		dwFlags;
	GUID		guidDSCFXClass;
	GUID		guidDSCFXInstance;
	DWORD		dwReserved1;
	DWORD		dwReserved2;
} DSCEFFECTDESC, *LPDSCEFFECTDESC;
typedef const DSCEFFECTDESC *LPCDSCEFFECTDESC;

typedef struct
{
	DWORD			dwSize;
	DWORD			dwFlags;
	DWORD			dwBufferBytes;
	DWORD			dwReserved;
	LPWAVEFORMATEX	lpwfxFormat;
	DWORD			dwFXCount;
	LPDSCEFFECTDESC	lpDSCFXDesc;
} DSCBUFFERDESC, *LPDSCBUFFERDESC;
typedef const DSCBUFFERDESC *LPCDSCBUFFERDESC;

typedef struct
{
	DWORD		dwOffset;
	HANDLE		hEventNotify;
} DSBPOSITIONNOTIFY, *LPDSBPOSITIONNOTIFY;
typedef const DSBPOSITIONNOTIFY *LPCDSBPOSITIONNOTIFY;


extern "C"
{
	extern HRESULT WINAPI DirectSoundCaptureCreate8(LPCGUID, LPDIRECTSOUNDCAPTURE *, LPUNKNOWN);
}


#undef INTERFACE
#define INTERFACE IDirectSoundCapture
DECLARE_INTERFACE_(IDirectSoundCapture, IUnknown)
{
	STDMETHOD(QueryInterface)(REFIID, void **) = 0;
	STDMETHOD_(ULONG,AddRef)(void) = 0;
	STDMETHOD_(ULONG,Release)(void) = 0;
	STDMETHOD(CreateCaptureBuffer)(LPCDSCBUFFERDESC, LPDIRECTSOUNDCAPTUREBUFFER *, LPUNKNOWN) = 0;
	STDMETHOD(GetCaps)(LPDSCCAPS) = 0;
	STDMETHOD(Initialize)(LPCGUID) = 0;
};

#undef INTERFACE
#define INTERFACE IDirectSoundCaptureBuffer
DECLARE_INTERFACE_(IDirectSoundCaptureBuffer, IUnknown)
{
	STDMETHOD(QueryInterface)(REFIID, void **) = 0;
	STDMETHOD_(ULONG,AddRef)(void) = 0;
	STDMETHOD_(ULONG,Release)(void) = 0;
	STDMETHOD(GetCaps)(LPDSCBCAPS) = 0;
	STDMETHOD(GetCurrentPosition)(LPDWORD, LPDWORD) = 0;
	STDMETHOD(GetFormat)(LPWAVEFORMATEX, DWORD, LPDWORD) = 0;
	STDMETHOD(GetStatus)(LPDWORD) = 0;
	STDMETHOD(Initialize)(LPDIRECTSOUNDCAPTURE, LPCDSCBUFFERDESC) = 0;
	STDMETHOD(Lock)(DWORD, DWORD, void **, LPDWORD, void **, LPDWORD, DWORD) = 0; 
	STDMETHOD(Start)(DWORD) = 0;
	STDMETHOD(Stop)(void) = 0;
	STDMETHOD(Unlock)(void *, DWORD, void *, DWORD) = 0; 
};
 
#undef INTERFACE
#define INTERFACE IDirectSoundNotify
DECLARE_INTERFACE_(IDirectSoundNotify, IUnknown) 
{
	STDMETHOD(QueryInterface) (REFIID, void **) = 0; 
	STDMETHOD_(ULONG,AddRef) (void) = 0; 
	STDMETHOD_(ULONG,Release) (void) = 0;
	STDMETHOD(SetNotificationPositions) (DWORD, LPCDSBPOSITIONNOTIFY) = 0;
};
 

#undef INTERFACE


#endif

// ZYUQURM
