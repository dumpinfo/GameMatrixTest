#pragma once

#include "../Leadwerks.h"
#ifdef _WIN32
#include "..\Libraries/glew-1.6.0/include/GL/glew.h"
#include "..\Libraries/glew-1.6.0/include/GL/wglew.h"
#include "..\Libraries\OculusSDK\LibOVR\Include\OVR.h"
#include "..\Libraries\OculusSDK\LibOVR\Src\OVR_CAPI.h"
#include "..\Libraries\OculusSDK\LibOVR\Src\OVR_CAPI_GL.h"
#endif

namespace Leadwerks
{
	class Camera;
	class World;
	class Window;
	class Context;
	class Buffer;

	class VR//lua
	{
	public:
#ifdef _WIN32
		static ovrHmd hmd;
		static ovrTexture m_EyeTexture[2];
		static ovrPosef m_EyeRenderPose[2];
		static ovrEyeRenderDesc m_EyeRenderDesc[2];
		static Buffer* m_buffer[2];
		static float m_eyewidth;
		static int framestate;
		static bool initialized;
#endif
		static void Recenter();//lua
		static void BeginFrame();
		static void EndFrame(bool vsyncmode);
		static void HideWarning();//lua
		static bool Initialize();
		static void Shutdown();
		static Vec3 GetHeadPosition();//lua
		static Vec3 GetHeadRotation();//lua
		static bool AttachToWindow(Window* window);
		static void RenderCamera(Camera* camera);
		static void RenderWorld(World* world, Camera* camera, Context* context);
	};
}
