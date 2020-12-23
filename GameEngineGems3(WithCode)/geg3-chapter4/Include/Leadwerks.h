#pragma once

#ifdef _WIN32
	#pragma warning(disable:4290)
	#pragma warning(disable:4996)
	#pragma warning(disable:4244)
	#pragma warning(disable:4305)
#endif

#undef None

typedef unsigned int GLuint;
typedef unsigned char GLubyte;

#include "OpenGL.h"

#ifdef OS_IOS
    void iOSSwapBuffers();
#endif
#ifdef __APPLE__
    #ifndef LIBRARY
int main_(int argc,const char *argv[]);
    #endif
#endif

typedef double flt;					// single place to change all floating number types
typedef const double const_flt;		// same as const version
#ifndef INFINITY
	const_flt INFINITY=999999.999999;	// NaN flt
#endif

#define COPY_INSTANCE 0
#define COPY_DUPLICATE 1

#undef GetFileType
#undef GetFileSize
#undef CreateWindow
#undef DrawText

//==========================================================
//C Libraries
//==========================================================
extern "C"
{

//ZLib
//Doesn't build in OSX static lib with DEBUG macro enabled
#include "Libraries/zlib-1.2.5/contrib/minizip/zip.h"
#include "Libraries/zlib-1.2.5/contrib/minizip/unzip.h"
#include "Libraries/zlib-1.2.5/zlib.h"

//Nvidia GPU affinity workaround
#ifdef _WIN32
#include "Libraries/nvapi/nvapi.h"
#endif

//Lua and LuaJIT
#ifdef _WIN32
	#include "Libraries/LuaJIT/src/lua.h"
	#include "Libraries/LuaJIT/src/lauxlib.h"
	#include "Libraries/LuaJIT/src/lualib.h"
#else
	#if defined(__LP64__) || defined(_LP64)
		#include "Libraries/LuaJIT/src/lua.h"
		#include "Libraries/LuaJIT/src/lauxlib.h"
		#include "Libraries/LuaJIT/src/lualib.h"
	#else
		#include "Libraries/lua-5.1.4/lua.h"
		#include "Libraries/lua-5.1.4/lauxlib.h"
		#include "Libraries/lua-5.1.4/lualib.h"
	#endif
#endif

//ToLua++
#include "Libraries/tolua++-1.0.93/include/tolua++.h"

//ENet
#include "Libraries/enet-1.3.1/include/enet/enet.h"

//OpenAL
#ifdef WINDOWS
    #include "Libraries/OpenAL/include/alc.h"
    #include "Libraries/OpenAL/include/al.h"
    //#include "Libraries/OpenAL/include/efx.h"
    //#include "Libraries/OpenAL/include/efx-creative.h"
    //#include "Libraries/OpenAL/include/EFX-Util.h"
    //#include "Libraries/OpenAL/include/xram.h"
#elif __ANDROID__
        #include "alc.h"
        #include "al.h"
#else
    #ifdef __linux__
        #include <AL/al.h>
        #include <AL/alc.h>
    #else
        #include <OpenAL/al.h>
        #include <OpenAL/alc.h>
    #endif
#endif

//FreeType
#include "Libraries/freetype-2.4.7/include/ft2build.h"
#include "Libraries/freetype-2.4.7/include/freetype/freetype.h"
#include "Libraries/freetype-2.4.7/include/freetype/ftlcdfil.h"
}

//==========================================================
//C++ Libraries
//==========================================================
//HACD
#include "Libraries/VHACD/src/VHACD_Lib/public/VHACD.h"

//Steam SDK
#ifdef __STEAM__
	#include "Libraries/steamworks/public/steam/steam_api.h"
#endif

//Newton Dynamics
//#define NEWTON_ASYNCHRONOUS_UPDATE
#include "Libraries/NewtonDynamics/coreLibrary_300/source/newton/Newton.h"

/*#include "Libraries/NewtonDynamics/Packages/dCustomJoints/Custom6DOF.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomBallAndSocket.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomControllerManager.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomCorkScrew.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomDryRollingFriction.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomGear.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomHinge.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomJoint.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomJointLibraryStdAfx.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomKinematicController.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomPathFollow.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomPlayerControllerManager.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomPulley.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomRagDoll.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomSlider.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomSlidingContact.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomTriggerManager.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomUniversal.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomUpVector.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomUserBlank.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomUniversal.h"
#include "Libraries/NewtonDynamics/Packages/dCustomJoints/CustomWormGear.h"*/

#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomVehicleControllerManager.h"
//#include "Libraries/NewtonDynamics/Packages/dCustomJoints/JointLibrary.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomBallAndSocket.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomHinge.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomJoint.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomKinematicController.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomSlider.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomControllerManager.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomSliderActuator.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomHingeActuator.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomJointLibraryStdAfx.h"
#include "Libraries/NewtonDynamics/packages/dCustomJoints/CustomControllerManager.h"

//Recast Navigation
#include "Libraries/RecastNavigation/Recast/Include/Recast.h"
#include "Libraries/RecastNavigation/Recast/Include/RecastAlloc.h"
#include "Libraries/RecastNavigation/Recast/Include/RecastAssert.h"

//DebugUtils
#include "Libraries/RecastNavigation/DebugUtils/Include/DebugDraw.h"
#include "Libraries/RecastNavigation/DebugUtils/Include/DetourDebugDraw.h"
#include "Libraries/RecastNavigation/DebugUtils/Include/RecastDebugDraw.h"
#include "Libraries/RecastNavigation/DebugUtils/Include/RecastDump.h"

//Detour
#include "Libraries/RecastNavigation/Detour/Include/DetourAlloc.h"
#include "Libraries/RecastNavigation/Detour/Include/DetourAssert.h"
#include "Libraries/RecastNavigation/Detour/Include/DetourCommon.h"
#include "Libraries/RecastNavigation/Detour/Include/DetourNavMesh.h"
#include "Libraries/RecastNavigation/Detour/Include/DetourNavMeshBuilder.h"
#include "Libraries/RecastNavigation/Detour/Include/DetourNavMeshQuery.h"
#include "Libraries/RecastNavigation/Detour/Include/DetourNode.h"
#include "Libraries/RecastNavigation/Detour/Include/DetourStatus.h"

//Detour Crowd
#include "Libraries/RecastNavigation/DetourCrowd/Include/DetourCrowd.h"
#include "Libraries/RecastNavigation/DetourCrowd/Include/DetourLocalBoundary.h"
#include "Libraries/RecastNavigation/DetourCrowd/Include/DetourObstacleAvoidance.h"
#include "Libraries/RecastNavigation/DetourCrowd/Include/DetourPathCorridor.h"
#include "Libraries/RecastNavigation/DetourCrowd/Include/DetourPathQueue.h"
#include "Libraries/RecastNavigation/DetourCrowd/Include/DetourProximityGrid.h"

//Detour Tile Cache
#include "Libraries/RecastNavigation/DetourTileCache/Include/DetourTileCache.h"
#include "Libraries/RecastNavigation/DetourTileCache/Include/DetourTileCacheBuilder.h"

using namespace std;

//Standard libraries
#ifdef _WIN32
	#include <direct.h>
	#include <io.h>
	#include <Psapi.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <cctype>
#include <stdint.h>

#ifdef _WIN32
	#include <functional>//Required for Visual Studio 2013
    #ifdef _DEBUG
        #define _CRTDBG_MAP_ALLOC
        #include <stdlib.h>
        #include <crtdbg.h>
    #else
        #include <stdlib.h>
    #endif
#else
    #include <stdlib.h>
#endif
#include <list>
#ifdef _WIN32
	#include "dirent.win32.h"
#else
	#include <dirent.h>
#endif
#include <algorithm>
#include <map>
#include <string>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <time.h>

//Declare tolua++ initialization function
int tolua_luacommands_open(lua_State* tolua_S);

namespace Leadwerks
{
	class Object;

	//Constants
	extern const int COLOR_DIFFUSE;
	extern const int COLOR_SPECULAR;
	extern const int COLOR_EDIT;

    class Stream;
    class Vec2;
    extern bool _TouchDownState[20];
    extern bool _TouchHitState[20];
    extern Vec2 _TouchPosition[20];
    extern bool logmode;
    extern Stream* logstream__;
    extern std::string logtext;

	//Luabind exception handling
	//#ifdef OS_WINDOWS
	//	void straight_to_debugger(unsigned int, _EXCEPTION_POINTERS*);
	//#endif

    void SetLogMode(const bool mode);

	void print( const std::string& s );//lua
	void print( const int i );//lua
	void print( const float f );//lua
	void print( Object* o );//lua
	void print( const double d );//lua

	//Deprecated, but may be in use:
	void Print( const std::string& s );
	void Print( const int i );
	void Print( const float f );
	void Print( Object* o );
	void Print( const double d );

	std::string IntToString( int i );
	std::string String( double f );
	std::string String( int i );
	std::string String( unsigned int i );
	std::string String( long l );
	std::string String( uint64_t i );

    //Touch input
    bool GetTouchDown(const int index);
    bool GetTouchHit(const int index);
    float GetTouchX(const int index);
    float GetTouchY(const int index);

    #ifdef ANDROID
	extern int AndroidDeviceOrientation;
	#endif

	unsigned long GetMemoryUsage();
	void GenerateError();

	//#define Asset::CreateNew 1
	//#define Asset::Unmanaged 2

	extern std::string logtext;

	#define MESSAGE_CONNECT -1
	#define MESSAGE_DISCONNECT -2
	#define MESSAGE_SENDCALLSTACK -101
	#define MESSAGE_DEBUGRESUME -104
	#define MESSAGE_DEBUGSTEP -105
	#define MESSAGE_DEBUGSTEPIN -106
	#define MESSAGE_DEBUGSTEPOUT -107
	#define MESSAGE_SENDERRORMESSAGE -108
	#define MESSAGE_DEBUGPOINTERINFOREQUEST -109
	#define MESSAGE_DEBUGPOINTERINFO -110
	#define MESSAGE_EDITBREAKPOINTS -111
	#define MESSAGE_PAUSE -112

	#define MESSAGE_SEQUENCED 1
	#define MESSAGE_RELIABLE 2
}

#ifdef OS_IOS
    void iOSSwapBuffers();
    string iOSGetAppPath(const std::string& filename);
#endif

//Math
#include "Classes/Math/Math3D.h"

//Includes
#include "String_.h"
#include "Classes/Directory.h"
#include "FileSystem.h"
#include "Timing.h"
#include "Debug.h"
#include "System.h"

//Classes
#include "Classes/VR.h"
#include "Classes/Steamworks.h"
#include "Classes/Key.h"
#include "Classes/Initializer.h"
#include "Classes/Uniform.h"
#include "Classes/Blend.h"
#include "Classes/Color.h"
#include "Classes/SortedObject.h"
#include "Classes/Transform.h"
#include "Classes/Particle.h"
#include "Classes/Edge.h"
#include "Classes/Face.h"
#include "Classes/ConvexHull.h"
#include "Classes/ConvexDecomposition.h"
#include "Classes/Driver.h"
#include "Classes/Thread.h"
#include "Classes/Mutex.h"
#include "Classes/LuaHookContainer.h"
#include "Classes/Object.h"
#include "Classes/Heightfield.h"
#include "Classes/Event.h"
#include "Classes/EventQueue.h"
#include "Classes/Vertex.h"
#include "Classes/Character.h"
#include "Classes/PickInfo.h"
#include "Classes/Argument.h"
#include "Classes/Collision.h"
#include "Classes/OctreeNode.h"
#include "Classes/VertexArray.h"
#include "Classes/IndiceArray.h"
#include "Classes/Asset.h"
#include "Classes/Device.h"
#include "Classes/Font.h"
#include "Classes/ComponentBase.h"
#include "Classes/Component.h"
#include "Classes/Object.h"
#include "Classes/Shader.h"
#include "Classes/Bank.h"
#include "Classes/Stream.h"
#include "Classes/Map.h"
#include "Classes/Prefab.h"
#include "Classes/PrefabBase.h"
#include "Classes/Texture.h"
#include "Classes/Shape.h"
#include "Classes/Entity.h"
#include "Classes/Body.h"
#include "Classes/World.h"
#include "Classes/Batch.h"
#include "Classes/Surface.h"
#include "Classes/OcclusionQuery.h"
#include "Classes/BankStream.h"
#include "Classes/Package.h"
#include "Classes/PackageFile.h"
#include "Classes/Simulation.h"
#include "Classes/Buffer.h"
#include "Classes/Context.h"
#include "Classes/CustomContext.h"
#include "Classes/Connection.h"
#include "Classes/MaterialValue.h"
#include "Classes/Material.h"
#include "Classes/Chunk.h"
#include "Classes/Message.h"
#include "Classes/Address.h"
#include "Classes/Peer.h"
#include "Classes/Client.h"
#include "Classes/Server.h"
#include "Classes/Interpreter.h"
#include "Classes/NavGeom.h"
#include "Classes/NavPath.h"
#include "Classes/NavMesh.h"
#include "Classes/NavTile.h"
#include "Classes/NavMeshDebugger.h"
#include "Classes/Agent.h"
#include "Classes/Sound.h"
#include "Classes/Source.h"
#include "Classes/Channel.h"
#include "Classes/Vehicle.h"
//#include "Classes/VR.h"
//#include "Classes/ParticleSystem.h"
//#include "Classes/Loader.h"
//#include "Classes/BMPLoader.h"
#include "Classes/ModelFactory.h"
#include "Classes/Vegetation.h"

//Factories
#include "Classes/Factories/File/FileFactory.h"
#include "Classes/Factories/File/PackageFactory.h"

//Entities
#include "Classes/Entities/Bone.h"
#include "Classes/Entities/Pivot.h"
#include "Classes/Entities/WaterPlane.h"
#include "Classes/Entities/Camera.h"
#include "Classes/Entities/Model.h"
#include "Classes/Entities/Brush.h"
#include "Classes/Entities/Joint.h"
#include "Classes/Entities/Decal.h"
#include "Classes/Entities/Light.h"
#include "Classes/Entities/Attractor.h"
#include "Classes/Entities/Listener.h"
#include "Classes/Entities/Sprite.h"
#include "Classes/Entities/LensFlare.h"
#include "Classes/Entities/Emitter.h"
#include "Classes/Entities/CharacterController.h"
#include "Classes/Entities/Terrain.h"
#include "Classes/Entities/BuildTerrainPatches.h"

//GUI
#include "Classes/GUI/GUI.h"
#include "Classes/GUI/Widget.h"
#include "Classes/GUI/Button.h"
#include "Classes/GUI/TextButton.h"
#include "Classes/GUI/Slider.h"
#include "Classes/GUI/ChoiceBox.h"
#include "Classes/GUI/Label.h"
#include "Classes/GUI/Panel.h"

//Lights
#include "Classes/Entities/Light/SpotLight.h"
#include "Classes/Entities/Light/PointLight.h"
#include "Classes/Entities/Light/DirectionalLight.h"

//Window
#ifdef _WIN32
	#include "Classes/Win32.Window.h"
#endif
#ifdef PLATFORM_MACOS
	#include "Classes/MacOS.Window.h"
#endif
#ifdef PLATFORM_IOS
	#include "Classes/iOS.Window.h"
#endif
#ifdef __ANDROID__
    #include "Classes/Android.Window.h"
#endif
#ifndef __ANDROID__
	#ifdef __linux__
		#include "Classes/Linux.Window.h"
	#endif
#endif

//Drivers
#include "Classes/Drivers/Graphics/GraphicsDriver.h"
#include "Classes/Drivers/Physics/PhysicsDriver.h"
#include "Classes/Drivers/Network/NetworkDriver.h"
#include "Classes/Drivers/Sound/SoundDriver.h"

//Graphics drivers
#ifdef OPENGL
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2GraphicsDriver.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2OcclusionQuery.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2VertexArray.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2IndiceArray.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Texture.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Context.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Surface.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Uniform.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Shader.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Camera.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Buffer.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Batch.h"
	#include "Classes/Drivers/Graphics/OpenGL2/OpenGL2Material.h"
	#ifdef LEADWERKS_3_1
		//#ifdef _WIN32
			#include "Classes/Drivers/Graphics/OpenGL4/OpenGL4GraphicsDriver.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Context.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4OcclusionQuery.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4VertexArray.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4IndiceArray.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Texture.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Surface.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Uniform.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Shader.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Camera.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Buffer.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Batch.h"
			#include "Classes/Drivers/Graphics/OpenGL4/Classes/OpenGL4Material.h"
		//#endif
	#endif
#endif
#ifdef OPENGLES
	#include "Classes/Drivers/Graphics/OpenGLES2/OpenGLES2GraphicsDriver.h"
#endif

//Physics drivers
#include "Classes/Drivers/Physics/NewtonDynamics/NewtonDynamicsPhysicsDriver.h"

//Network drivers
#include "Classes/Drivers/Network/ENet/ENetNetworkDriver.h"

//Sound drivers
#include "Classes/Drivers/Sound/OpenAL/OpenALSoundDriver.h"

#ifdef PLATFORM_MACOS
extern int MacOSMessageBox( const char* header, const char* message, unsigned long message_type );
#endif

#ifdef _WIN32
	#ifdef DEBUG
		//#include <vld.h>
	#endif
#endif


