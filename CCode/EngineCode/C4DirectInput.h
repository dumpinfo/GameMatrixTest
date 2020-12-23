 

#ifndef C4DirectInput_h
#define C4DirectInput_h


#include "C4Defines.h"


#define DIRECTINPUT_VERSION			0x0800
#define DI8DEVCLASS_ALL				0

#define DI_OK						S_OK
#define DI_BUFFEROVERFLOW			S_FALSE

#define DIERR_NOTACQUIRED			MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_ACCESS)
#define DIERR_INPUTLOST				MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_READ_FAULT)

#define DIENUM_CONTINUE				1
#define DIEDFL_ATTACHEDONLY			0x00000001

#define DISCL_EXCLUSIVE				0x00000001
#define DISCL_NONEXCLUSIVE			0x00000002
#define DISCL_FOREGROUND			0x00000004
#define DISCL_NOWINKEY				0x00000010

#define DIPROPAXISMODE_ABS			0
#define DIPROPAXISMODE_REL			1

#define DIDF_ABSAXIS				0x00000001
#define DIDF_RELAXIS				0x00000002

#define DIDFT_ALL					0x00000000
#define DIDFT_RELAXIS				0x00000001
#define DIDFT_ABSAXIS				0x00000002
#define DIDFT_BUTTON				0x0000000C
#define DIDFT_GETTYPE(n)			LOBYTE(n)

#define DIEFT_CONSTANTFORCE			0x00000001
#define DIEFT_CUSTOMFORCE			0x00000005

#define MAKEDIPROP(prop) (*(const GUID *)(prop))
#define DIPROP_BUFFERSIZE			MAKEDIPROP(1)
#define DIPROP_AXISMODE				MAKEDIPROP(2)
#define DIPROP_RANGE				MAKEDIPROP(4)
#define DIPROP_DEADZONE				MAKEDIPROP(5)

#define DIPH_DEVICE					0
#define DIPH_BYID					2

#define DIK_ESCAPE					0x01
#define DIK_GRAVE					0x29

#define GET_DIDEVICE_TYPE(dwDevType) LOBYTE(dwDevType)
#define DI8DEVTYPE_MOUSE			0x12
#define DI8DEVTYPE_KEYBOARD			0x13


DEFINE_GUID(IID_IDirectInput8W, 0xBF798031,0x483A,0x4DA2,0xAA,0x99,0x5D,0x64,0xED,0x36,0x97,0x00);
DEFINE_GUID(GUID_XAxis, 0xA36D02E0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_YAxis, 0xA36D02E1,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_ZAxis, 0xA36D02E2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RxAxis, 0xA36D02F4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RyAxis, 0xA36D02F5,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_RzAxis, 0xA36D02E3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Slider, 0xA36D02E4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Button, 0xA36D02F0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_Key, 0x55728220,0xD33C,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_POV, 0xA36D02F2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);


struct IDirectInput8W;
struct IDirectInputDevice8W;
struct IDirectInputEffect;

typedef struct IDirectInput8W *LPDIRECTINPUT8W;
typedef struct IDirectInputDevice8W *LPDIRECTINPUTDEVICE8W;
typedef struct IDirectInputEffect *LPDIRECTINPUTEFFECT;


typedef struct
{
	DWORD	dwSize;
	DWORD	dwHeaderSize;
	DWORD	dwObj;
	DWORD	dwHow;
} DIPROPHEADER, *LPDIPROPHEADER;
typedef const DIPROPHEADER *LPCDIPROPHEADER;

typedef struct
{
	DIPROPHEADER	diph;
	DWORD			dwData;
} DIPROPDWORD, *LPDIPROPDWORD;

typedef struct
{
	DIPROPHEADER	diph;
	LONG			lMin;
	LONG			lMax;
} DIPROPRANGE, *LPDIPROPRANGE;

typedef struct 
{
	const GUID		*pguid;
	DWORD			dwOfs; 
	DWORD			dwType;
	DWORD			dwFlags; 
} DIOBJECTDATAFORMAT, *LPDIOBJECTDATAFORMAT;

typedef struct 
{
	DWORD					dwSize; 
	DWORD					dwObjSize; 
	DWORD					dwFlags;
	DWORD					dwDataSize;
	DWORD					dwNumObjs;
	LPDIOBJECTDATAFORMAT	rgodf; 
} DIDATAFORMAT, *LPDIDATAFORMAT;
typedef const DIDATAFORMAT *LPCDIDATAFORMAT;

typedef struct
{
	DWORD		dwOfs;
	DWORD		dwData;
	DWORD		dwTimeStamp;
	DWORD		dwSequence;
	UINT_PTR	uAppData;
} DIDEVICEOBJECTDATA, *LPDIDEVICEOBJECTDATA;
typedef const DIDEVICEOBJECTDATA *LPCDIDEVICEOBJECTDATA;

typedef struct
{
	DWORD	dwSize;
	GUID	guidInstance;
	GUID	guidProduct;
	DWORD	dwDevType;
	WCHAR	tszInstanceName[MAX_PATH];
	WCHAR	tszProductName[MAX_PATH];
	GUID	guidFFDriver;
	WORD	wUsagePage;
	WORD	wUsage;
} DIDEVICEINSTANCEW, *LPDIDEVICEINSTANCEW;
typedef const DIDEVICEINSTANCEW *LPCDIDEVICEINSTANCEW;

typedef struct
{
	DWORD	dwSize;
	GUID	guidType;
	DWORD	dwOfs;
	DWORD	dwType;
	DWORD	dwFlags;
	WCHAR	tszName[MAX_PATH];
	DWORD	dwFFMaxForce;
	DWORD	dwFFForceResolution;
	WORD	wCollectionNumber;
	WORD	wDesignatorIndex;
	WORD	wUsagePage;
	WORD	wUsage;
	DWORD	dwDimension;
	WORD	wExponent;
	WORD	wReportId;
} DIDEVICEOBJECTINSTANCEW, *LPDIDEVICEOBJECTINSTANCEW;
typedef const DIDEVICEOBJECTINSTANCEW *LPCDIDEVICEOBJECTINSTANCEW;

typedef struct
{
	UINT_PTR	uAppData;
	DWORD		dwSemantic;
	DWORD		dwFlags;
	union
	{
		LPCWSTR	lptszActionName;
		UINT	uResIdString;
	};
	GUID		guidInstance;
	DWORD		dwObjID;
	DWORD		dwHow;
} DIACTIONW, *LPDIACTIONW;

typedef struct
{
	DWORD		dwSize;
	DWORD		dwActionSize;
	DWORD		dwDataSize;
	DWORD		dwNumActions;
	LPDIACTIONW	rgoAction;
	GUID		guidActionMap;
	DWORD		dwGenre;
	DWORD		dwBufferSize;
	LONG		lAxisMin;
	LONG		lAxisMax;
	HINSTANCE	hInstString;
	FILETIME	ftTimeStamp;
	DWORD		dwCRC;
	WCHAR		tszActionMap[MAX_PATH];
} DIACTIONFORMATW, *LPDIACTIONFORMATW;

typedef struct
{
	DWORD	dwSize;
	DWORD	dwAttackLevel;
	DWORD	dwAttackTime;
	DWORD	dwFadeLevel;
	DWORD	dwFadeTime;
} DIENVELOPE, *LPDIENVELOPE;

typedef struct
{
	DWORD			dwSize;
	DWORD			dwFlags;
	DWORD			dwDuration;
	DWORD			dwSamplePeriod;
	DWORD			dwGain;
	DWORD			dwTriggerButton;
	DWORD			dwTriggerRepeatInterval;
	DWORD			cAxes;
	LPDWORD			rgdwAxes;
	LPLONG			rglDirection;
	LPDIENVELOPE	lpEnvelope;
	DWORD			cbTypeSpecificParams;
	void			*lpvTypeSpecificParams;
	DWORD			dwStartDelay;
} DIEFFECT, *LPDIEFFECT;
typedef const DIEFFECT *LPCDIEFFECT;

typedef struct
{
	DWORD	dwSize;
	GUID	guid;
	DWORD	dwEffType;
	DWORD	dwStaticParams;
	DWORD	dwDynamicParams;
	WCHAR	tszName[MAX_PATH];
} DIEFFECTINFOW, *LPDIEFFECTINFOW;
typedef const DIEFFECTINFOW *LPCDIEFFECTINFOW;

typedef struct
{
	DWORD	dwSize;
	DWORD	dwCommand;
	void	*lpvInBuffer;
	DWORD	cbInBuffer;
	void	*lpvOutBuffer;
	DWORD	cbOutBuffer;
} DIEFFESCAPE, *LPDIEFFESCAPE;

typedef struct
{
	DWORD		dwSize;
	GUID		GuidEffect;
	LPCDIEFFECT	lpDiEffect;
	CHAR		szFriendlyName[MAX_PATH];
} DIFILEEFFECT, *LPDIFILEEFFECT;
typedef const DIFILEEFFECT *LPCDIFILEEFFECT;

typedef struct
{
	WCHAR		tszImagePath[MAX_PATH];
	DWORD		dwFlags;
	DWORD		dwViewID;
	RECT		rcOverlay;
	DWORD		dwObjID;
	DWORD		dwcValidPts;
	POINT		rgptCalloutLine[5];
	RECT		rcCalloutRect;
	DWORD		dwTextAlign;
} DIDEVICEIMAGEINFOW, *LPDIDEVICEIMAGEINFOW;

typedef struct
{
	DWORD					dwSize;
	DWORD					dwSizeImageInfo;
	DWORD					dwcViews;
	DWORD					dwcButtons;
	DWORD					dwcAxes;
	DWORD					dwcPOVs;
	DWORD					dwBufferSize;
	DWORD					dwBufferUsed;
	LPDIDEVICEIMAGEINFOW	lprgImageInfoArray;
} DIDEVICEIMAGEINFOHEADERW, *LPDIDEVICEIMAGEINFOHEADERW;

typedef struct
{
	DWORD	dwSize;
	DWORD	cTextFore;
	DWORD	cTextHighlight;
	DWORD	cCalloutLine;
	DWORD	cCalloutHighlight;
	DWORD	cBorder;
	DWORD	cControlFill;
	DWORD	cHighlightFill;
	DWORD	cAreaFill;
} DICOLORSET, *LPDICOLORSET;

typedef struct
{
	 DWORD				dwSize;
	 DWORD				dwcUsers;
	 LPWSTR				lptszUserNames;
	 DWORD				dwcFormats;
	 LPDIACTIONFORMATW	lprgFormats;
	 HWND				hwnd;
	 DICOLORSET			dics;
	 IUnknown			*lpUnkDDSTarget;
} DICONFIGUREDEVICESPARAMSW, *LPDICONFIGUREDEVICESPARAMSW;

typedef struct
{
	DWORD	dwSize;
	DWORD	dwFlags;
	DWORD	dwDevType;
	DWORD	dwAxes;
	DWORD	dwButtons;
	DWORD	dwPOVs;
	DWORD	dwFFSamplePeriod;
	DWORD	dwFFMinTimeResolution;
	DWORD	dwFirmwareRevision;
	DWORD	dwHardwareRevision;
	DWORD	dwFFDriverVersion;
} DIDEVCAPS, *LPDIDEVCAPS;


typedef BOOL (PASCAL *LPDIENUMDEVICESCALLBACKW)(LPCDIDEVICEINSTANCEW, void *);
typedef BOOL (PASCAL *LPDIENUMDEVICEOBJECTSCALLBACKW)(LPCDIDEVICEOBJECTINSTANCEW, void *);
typedef BOOL (PASCAL *LPDICONFIGUREDEVICESCALLBACK)(IUnknown *, void *);
typedef BOOL (PASCAL *LPDIENUMEFFECTSCALLBACKW)(LPCDIEFFECTINFOW, void *);
typedef BOOL (PASCAL *LPDIENUMCREATEDEFFECTOBJECTSCALLBACK)(LPDIRECTINPUTEFFECT, void *);
typedef BOOL (PASCAL *LPDIENUMEFFECTSINFILECALLBACK)(LPCDIFILEEFFECT, void *);
typedef BOOL (PASCAL *LPDIENUMDEVICESBYSEMANTICSCBW)(LPCDIDEVICEINSTANCEW, LPDIRECTINPUTDEVICE8W, DWORD, DWORD, void *);


extern "C"
{
	extern HRESULT WINAPI DirectInput8Create(HINSTANCE, DWORD, REFIID, void **, LPUNKNOWN);

	__inline int IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
	{
		const unsigned long *d1 = &rguid1.Data1;
		const unsigned long *d2 = &rguid2.Data1;
		return ((d1[0] == d2[0]) && (d1[1] == d2[1]) && (d1[2] == d2[2]) && (d1[3] == d2[3]));
	}
}


#undef INTERFACE
#define INTERFACE IDirectInput8W
DECLARE_INTERFACE_(IDirectInput8W, IUnknown)
{
    STDMETHOD(QueryInterface)(REFIID, void **) = 0;
    STDMETHOD_(ULONG, AddRef)(void) = 0;
    STDMETHOD_(ULONG, Release)(void) = 0;
    STDMETHOD(CreateDevice)(REFGUID, LPDIRECTINPUTDEVICE8W *, LPUNKNOWN) = 0;
    STDMETHOD(EnumDevices)(DWORD, LPDIENUMDEVICESCALLBACKW, void *, DWORD) = 0;
    STDMETHOD(GetDeviceStatus)(REFGUID) = 0;
    STDMETHOD(RunControlPanel)(HWND, DWORD) = 0;
    STDMETHOD(Initialize)(HINSTANCE, DWORD) = 0;
    STDMETHOD(FindDevice)(REFGUID, LPCWSTR, LPGUID) = 0;
    STDMETHOD(EnumDevicesBySemantics)(LPCWSTR, LPDIACTIONFORMATW, LPDIENUMDEVICESBYSEMANTICSCBW, void *, DWORD) = 0;
    STDMETHOD(ConfigureDevices)(LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMSW, DWORD, void *) = 0;
};

#undef INTERFACE
#define INTERFACE IDirectInputDevice8W
DECLARE_INTERFACE_(IDirectInputDevice8W, IUnknown)
{
    STDMETHOD(QueryInterface)(REFIID, void **) = 0;
    STDMETHOD_(ULONG, AddRef)(void) = 0;
    STDMETHOD_(ULONG, Release)(void) = 0;
    STDMETHOD(GetCapabilities)(LPDIDEVCAPS) = 0;
    STDMETHOD(EnumObjects)(LPDIENUMDEVICEOBJECTSCALLBACKW, void *, DWORD) = 0;
    STDMETHOD(GetProperty)(REFGUID, LPDIPROPHEADER) = 0;
    STDMETHOD(SetProperty)(REFGUID, LPCDIPROPHEADER) = 0;
    STDMETHOD(Acquire)(void) = 0;
    STDMETHOD(Unacquire)(void) = 0;
    STDMETHOD(GetDeviceState)(DWORD, void *) = 0;
    STDMETHOD(GetDeviceData)(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) = 0;
    STDMETHOD(SetDataFormat)(LPCDIDATAFORMAT) = 0;
    STDMETHOD(SetEventNotification)(HANDLE) = 0;
    STDMETHOD(SetCooperativeLevel)(HWND, DWORD) = 0;
    STDMETHOD(GetObjectInfo)(LPDIDEVICEOBJECTINSTANCEW, DWORD, DWORD) = 0;
    STDMETHOD(GetDeviceInfo)(LPDIDEVICEINSTANCEW) = 0;
    STDMETHOD(RunControlPanel)(HWND, DWORD) = 0;
    STDMETHOD(Initialize)(HINSTANCE, DWORD, REFGUID) = 0;
    STDMETHOD(CreateEffect)(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT *, LPUNKNOWN) = 0;
    STDMETHOD(EnumEffects)(LPDIENUMEFFECTSCALLBACKW, void *, DWORD) = 0;
    STDMETHOD(GetEffectInfo)(LPDIEFFECTINFOW, REFGUID) = 0;
    STDMETHOD(GetForceFeedbackState)(LPDWORD) = 0;
    STDMETHOD(SendForceFeedbackCommand)(DWORD) = 0;
    STDMETHOD(EnumCreatedEffectObjects)(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, void *, DWORD) = 0;
    STDMETHOD(Escape)(LPDIEFFESCAPE) = 0;
    STDMETHOD(Poll)(void) = 0;
    STDMETHOD(SendDeviceData)(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD) = 0;
    STDMETHOD(EnumEffectsInFile)(LPCWSTR, LPDIENUMEFFECTSINFILECALLBACK, void *, DWORD) = 0;
    STDMETHOD(WriteEffectToFile)(LPCWSTR, DWORD, LPDIFILEEFFECT, DWORD) = 0;
    STDMETHOD(BuildActionMap)(LPDIACTIONFORMATW, LPCWSTR, DWORD) = 0;
    STDMETHOD(SetActionMap)(LPDIACTIONFORMATW, LPCWSTR, DWORD) = 0;
    STDMETHOD(GetImageInfo)(LPDIDEVICEIMAGEINFOHEADERW) = 0;
};


#define RPC_C_AUTHZ_NONE				0
#define RPC_C_AUTHN_LEVEL_CALL			3
#define RPC_C_IMP_LEVEL_IMPERSONATE		3
#define RPC_C_AUTHN_WINNT				10

typedef long CIMTYPE;

enum
{
	EOAC_NONE = 0
};

enum
{
	VT_BSTR = 8
};

enum
{
	WBEM_FLAG_RETURN_IMMEDIATELY	= 0x10,
	WBEM_FLAG_FORWARD_ONLY			= 0x20
};

typedef interface IWbemContext IWbemContext;
typedef interface IWbemLocator IWbemLocator;
typedef interface IWbemServices IWbemServices;
typedef interface IWbemCallResult IWbemCallResult;
typedef interface IWbemClassObject IWbemClassObject;
typedef interface IWbemObjectSink IWbemObjectSink;
typedef interface IEnumWbemClassObject IEnumWbemClassObject;
typedef interface IWbemQualifierSet IWbemQualifierSet;

class __declspec(uuid("4590f811-1d3a-11d0-891f-00aa004b2e24")) WbemLocator;

struct __declspec(uuid("dc12a687-737f-11cf-884d-00aa004b2e24")) __declspec(novtable) IWbemLocator : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE ConnectServer(const BSTR, const BSTR, const BSTR, const BSTR, long, const BSTR, IWbemContext *, IWbemServices **) = 0;
};

struct __declspec(uuid("9556dc99-828c-11cf-a37e-00aa003240c7")) __declspec(novtable) IWbemServices : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE OpenNamespace(const BSTR, long, IWbemContext *, IWbemServices **, IWbemCallResult **) = 0;
	virtual HRESULT STDMETHODCALLTYPE CancelAsyncCall(IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE QueryObjectSink(long, IWbemObjectSink **) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetObject(const BSTR, long, IWbemContext *, IWbemClassObject **, IWbemCallResult **) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetObjectAsync(const BSTR, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE PutClass(IWbemClassObject *, long, IWbemContext *, IWbemCallResult **) = 0;
	virtual HRESULT STDMETHODCALLTYPE PutClassAsync(IWbemClassObject *, long , IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteClass(const BSTR, long, IWbemContext *, IWbemCallResult **) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteClassAsync(const BSTR, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateClassEnum(const BSTR, long, IWbemContext *, IEnumWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateClassEnumAsync(const BSTR, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE PutInstance(IWbemClassObject *, long, IWbemContext *, IWbemCallResult **) = 0;
	virtual HRESULT STDMETHODCALLTYPE PutInstanceAsync(IWbemClassObject *, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteInstance(const BSTR, long, IWbemContext *, IWbemCallResult **) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteInstanceAsync(const BSTR, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateInstanceEnum(const BSTR, long, IWbemContext *, IEnumWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateInstanceEnumAsync(const BSTR, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE ExecQuery(const BSTR, const BSTR, long, IWbemContext *, IEnumWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE ExecQueryAsync(const BSTR, const BSTR, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE ExecNotificationQuery(const BSTR, const BSTR, long, IWbemContext *, IEnumWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE ExecNotificationQueryAsync(const BSTR, const BSTR, long, IWbemContext *, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE ExecMethod(const BSTR, const BSTR, long, IWbemContext *, IWbemClassObject *, IWbemClassObject **, IWbemCallResult **) = 0;
	virtual HRESULT STDMETHODCALLTYPE ExecMethodAsync(const BSTR, const BSTR, long, IWbemContext *, IWbemClassObject *, IWbemObjectSink *) = 0;
};

struct __declspec(uuid("dc12a681-737f-11cf-884d-00aa004b2e24")) __declspec(novtable) IWbemClassObject : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetQualifierSet(IWbemQualifierSet **) = 0;
	virtual HRESULT STDMETHODCALLTYPE Get(LPCWSTR, long, VARIANT *, CIMTYPE *, long *) = 0;
	virtual HRESULT STDMETHODCALLTYPE Put(LPCWSTR, long, VARIANT *, CIMTYPE) = 0;
	virtual HRESULT STDMETHODCALLTYPE Delete(LPCWSTR) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetNames(LPCWSTR, long, VARIANT *, SAFEARRAY **) = 0;
	virtual HRESULT STDMETHODCALLTYPE BeginEnumeration(long) = 0;
	virtual HRESULT STDMETHODCALLTYPE Next(long, BSTR *, VARIANT *, CIMTYPE *, long *) = 0;
	virtual HRESULT STDMETHODCALLTYPE EndEnumeration(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetPropertyQualifierSet(LPCWSTR, IWbemQualifierSet **) = 0;
	virtual HRESULT STDMETHODCALLTYPE Clone(IWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetObjectText(long, BSTR *) = 0;
	virtual HRESULT STDMETHODCALLTYPE SpawnDerivedClass(long, IWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE SpawnInstance(long, IWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE CompareTo(long, IWbemClassObject *) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetPropertyOrigin(LPCWSTR, BSTR *) = 0;
	virtual HRESULT STDMETHODCALLTYPE InheritsFrom(LPCWSTR) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethod(LPCWSTR, long, IWbemClassObject **, IWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE PutMethod(LPCWSTR, long, IWbemClassObject *, IWbemClassObject *) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteMethod(LPCWSTR) = 0;
	virtual HRESULT STDMETHODCALLTYPE BeginMethodEnumeration(long) = 0;
	virtual HRESULT STDMETHODCALLTYPE NextMethod(long, BSTR *, IWbemClassObject **, IWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE EndMethodEnumeration(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodQualifierSet(LPCWSTR, IWbemQualifierSet **) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetMethodOrigin(LPCWSTR, BSTR *) = 0;
};

struct __declspec(uuid("027947e1-d731-11ce-a357-000000000001")) __declspec(novtable) IEnumWbemClassObject : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Next(long, ULONG, IWbemClassObject **, ULONG *) = 0;
	virtual HRESULT STDMETHODCALLTYPE NextAsync(ULONG, IWbemObjectSink *) = 0;
	virtual HRESULT STDMETHODCALLTYPE Clone(IEnumWbemClassObject **) = 0;
	virtual HRESULT STDMETHODCALLTYPE Skip(long, ULONG) = 0;
};


#undef INTERFACE


#endif

// ZYUQURM
