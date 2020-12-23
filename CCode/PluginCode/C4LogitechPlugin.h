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


#ifndef C4LogitechPlugin_h
#define C4LogitechPlugin_h


//# \component	Logitech Plugin
//# \prefix		LogitechPlugin/


#include "C4Plugins.h"
#include "C4Logitech.h"


#ifdef C4LOGITECH

	#define C4LOGITECHAPI C4MODULEEXPORT


	extern "C"
	{
		C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
	}

#else

	#define C4LOGITECHAPI C4MODULEIMPORT

#endif


namespace C4
{
	typedef EngineResult	LogitechResult;


	enum : LogitechResult
	{
		kManagerLogitech		= 'LG',

		kLogitechOkay			= kEngineOkay,
		kLogitechInitFailed		= (kManagerLogitech << 16) | 0x0001
	};


	//# \class	LogitechDevice		Encapsulates functionality for a Logitech device having a LCD display.
	//
	//# \def	class LogitechDevice : public ListElement<LogitechDevice>
	//
	//# \ctor	LogitechDevice();
	//
	//# \desc
	//# The $LogitechDevice$ class encapsulates functionality for a Logitech device having a LCD display.
	//# When the Logitech Plugin is initialized, it searches the computer for compatible devices and
	//# creates a new $LogitechDevice$ instance for each one that it finds. These instances are stored
	//# in a list that can be accessed using the $@LogitechPlugin::GetFirstDevice@$ function.
	//#
	//# Each Logitech device maintains storage for its own 160&times;43 pixel image. The image shown on the
	//# device's LCD display is modified by retrieving a pointer to this storage area using the
	//# $@LogitechDevice::GetDeviceImage@$ function, changing the image data, and then calling the
	//# $@LogitechDevice::UpdateDeviceImage@$ function.
	//
	//# \base	Utilities/ListElement<LogitechDevice>	Used internally by the Logitech Plugin.


	//# \function	LogitechDevice::GetDeviceImage		Returns a pointer to the image storage buffer for a Logitech device.
	//
	//# \proto	Color1C *GetDeviceImage(void);
	//
	//# \desc
	//# The $GetDeviceImage$ function returns a pointer to the image storage buffer for a Logitech device.
	//# This buffer is always 160&times;43 pixels in size, and each pixel occupies one byte. When modifying
	//# data stored in this buffer, the value 0 should be used to indicate white, and the value 255 should
	//# be used to indicate black. The buffer is initially cleared to all zeros.
	//#
	//# After the image data has been modified for a device, the $@LogitechDevice::UpdateDeviceImage@$ function
	//# must be called to move it to the LCD display.
	//
	//# \also	$@LogitechDevice::UpdateDeviceImage@$


	//# \function	LogitechDevice::UpdateDeviceImage	Updates image shown the LCD display for a Logitech device.
	//
	//# \proto	void UpdateDeviceImage(void);
	//
	//# \desc
	//# The $UpdateDeviceImage$ function updates the image shown of the LCD display for a Logitech device
	//# to the pixel image stored in the device instance. This image to display can be directly modified by
	//# calling the $@LogitechDevice::GetDeviceImage@$ function to retrieve a pointer to the pixel storage buffer.
	//
	//# \also	$@LogitechDevice::GetDeviceImage@$


	class LogitechDevice : public ListElement<LogitechDevice>
	{
		private:

			int							deviceHandle;
			LG::lgLcdBitmap160x43x1		deviceImage;

		public:

			LogitechDevice(int connection, int index); 
			~LogitechDevice();

			Color1C *GetDeviceImage(void) 
			{
				return (deviceImage.pixels); 
			}

			C4LOGITECHAPI void UpdateDeviceImage(void); 
	};
 
 
	//# \class	LogitechPlugin		The main class for the Logitech Plugin.
	//
	//# \def	class LogitechPlugin : public Plugin, public Singleton<LogitechPlugin>
	// 
	//# \ctor	LogitechPlugin();
	//
	//# \desc
	//# The $LogitechPlugin$ class manages Logitech devices that have LCD display capability.
	//# A list of such devices attached to the computer can be retrieved by calling the
	//# $@LogitechPlugin::GetFirstDevice@$ function.
	//
	//# \base	System/Plugin						The $LogitechPlugin$ is a specific type of plugin.
	//# \base	System/Singleton<LogitechPlugin>	There is only one $LogitechPlugin$ instance.


	//# \function	LogitechPlugin::GetFirstDevice		Returns a pointer to the first Logitech device instance.
	//
	//# \proto	LogitechDevice *GetFirstDevice(void) const;
	//
	//# \desc
	//# The $GetFirstDevice$ function returns a pointer to the first Logitech device instance in the list of
	//# devices connected to the computer. If there are no Logitech devices connected to the computer having
	//# LCD display capabilities, then the return value is $nullptr$. The $@Utilities/ListElement::Next@$
	//# function can be used to iterate through all devices in the list.
	//
	//# \also	$@LogitechDevice@$


	//# \function	LogitechPlugin::GetLogoData		Returns a pointer to a C4 Engine logo image.
	//
	//# \proto	const Color1C *GetLogoData(void);
	//
	//# \desc
	//# The $GetLogoData$ function returns a pointer to a static image buffer containing the C4 Engine logo.
	//# This image can be displayed on a Logitech device by calling the $@LogitechDevice::GetDeviceImage@$
	//# function to retrieve a pointer to the device's image storage buffer and copying exactly 6880 bytes
	//# (160&times;43 pixels) to it from the logo buffer.
	//
	//# \also	$@LogitechDevice::GetDeviceImage@$


	class LogitechPlugin : public Plugin, public Singleton<LogitechPlugin>
	{
		private:

			bool					logitechActive;
			int						logitechConnection;

			List<LogitechDevice>	deviceList;

			static const Color1C	logoData[160 * 43];

		public:

			LogitechPlugin();
			~LogitechPlugin();

			LogitechDevice *GetFirstDevice(void) const
			{
				return (deviceList.First());
			}

			static const Color1C *GetLogoData(void)
			{
				return (logoData);
			}

			C4LOGITECHAPI LogitechResult Initialize(void);
			C4LOGITECHAPI void Terminate(void);
	};


	C4LOGITECHAPI extern LogitechPlugin *TheLogitechPlugin;
}


#endif

// ZYUQURM
