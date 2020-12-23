#pragma once

#include "../Leadwerks.h"
#ifdef ANDROID
//#include "../../Projects/Android/jni/leandroid.h"
#endif

namespace Leadwerks
{
	class Device//lua
	{
	public:
		static int orientation;
		static Vec3 acceleration;
#ifdef IOS
        static int iOSOrientation;
        static int IOSAccumulatedAccelerationsCount;
        static Vec3 IOSAccumulatedAccelerations;
#endif        
		static void SetOrientation(const int orientation);//lua
		static int GetOrientation();//lua
		static Vec3 GetAcceleration();//lua
		static const int Landscape;//lua
		static const int Portrait;//lua
		static const int LandscapeLeft;
		static const int LandscapeRight;
		static const int PortraitUpsideDown;
		/*static const int ORIENTATION_LANDSCAPE_RIGHT;
		static const int ORIENTATION_LANDSCAPE_LEFT;
		static const int ORIENTATION_PORTRAIT_UPSIDEDOWN;*/
	};
}
