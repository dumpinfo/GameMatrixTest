//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef C4Logitech_h
#define C4Logitech_h


#define LGLCD_DEVICE_FAMILY_KEYBOARD_G15	0x00000001
#define LGLCD_BMP_FORMAT_160x43x1			0x00000001
#define LGLCD_BMP_WIDTH						160
#define LGLCD_BMP_HEIGHT					43
#define LGLCD_PRIORITY_NORMAL				128


namespace LG
{
	typedef DWORD (WINAPI *lgLcdOnConfigureCB)(int, const void *);
	typedef DWORD (WINAPI *lgLcdOnSoftButtonsCB)(int, DWORD, const void *);


	struct lgLcdConfigureContext
	{
		lgLcdOnConfigureCB		configCallback;
		void					*configContext;
	};

	struct lgLcdSoftbuttonsChangedContext
	{
		lgLcdOnSoftButtonsCB	softbuttonsChangedCallback;
		void					*softbuttonsChangedContext;
	};

	struct lgLcdConnectContext
	{
		LPCSTR					appFriendlyName;
		BOOL					isPersistent;
		BOOL					isAutostartable;
		lgLcdConfigureContext	onConfigure;
		int						connection;
	};

	struct lgLcdOpenContext
	{
		int								connection;
		int								index;
		lgLcdSoftbuttonsChangedContext	onSoftbuttonsChanged;
		int								device;
	};

	struct lgLcdDeviceDesc
	{
		DWORD		Width;
		DWORD		Height;
		DWORD		Bpp;
		DWORD		NumSoftButtons;
	};

	struct lgLcdBitmapHeader
	{
		DWORD		Format;
	};

	struct lgLcdBitmap160x43x1 : lgLcdBitmapHeader
	{
		BYTE		pixels[LGLCD_BMP_WIDTH * LGLCD_BMP_HEIGHT];
	};
}


extern "C"
{
	DWORD WINAPI lgLcdInit(void);
	DWORD WINAPI lgLcdDeInit(void);
	DWORD WINAPI lgLcdConnectA(LG::lgLcdConnectContext *);
	DWORD WINAPI lgLcdDisconnect(int);
	DWORD WINAPI lgLcdSetDeviceFamiliesToUse(int, DWORD, DWORD);
	DWORD WINAPI lgLcdEnumerate(int, int, LG::lgLcdDeviceDesc *);
	DWORD WINAPI lgLcdOpen(LG::lgLcdOpenContext *);
	DWORD WINAPI lgLcdClose(int);
	DWORD WINAPI lgLcdUpdateBitmap(int, const LG::lgLcdBitmapHeader *, DWORD);
}


#endif

// ZYUQURM
