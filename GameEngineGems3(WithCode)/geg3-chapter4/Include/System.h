#pragma once
#include "Leadwerks.h"

namespace Leadwerks
{
	class Hook;
	class Event;

    class System//lua
    {
    public:
		static int GCDepth;
		static int DebugErrorHook;
		static int UnmetDependencyHook;
		static std::multimap<int,void*> hooks;
		static std::map<std::string, void*> callbacks;
		static std::map<std::string,std::string> Arguments;
		static std::map<std::string, std::string> Settings;
		static std::string AppPath;//lua

		static bool SaveSettings(std::string file);
		static bool LoadSettings(std::string file);
		static long GetMemoryUsage();//lua
        static void Notify(const std::string& message,const int flags=0);
		static void (*callback_emitevent)(Event*);
		static void (*callback_print)(char*);
		static void (*callback_navtileupdate)(void);
		static void (*callback_debugerror)(char*);
		static char* (*callback_materialloadbestguess)(char*);
		static void SetCallback(const int id, char* funcptr);
		static void CollectGarbage();//lua
		static void SetCallback(const std::string& name, void* callback);
		static void ParseCommandLine(int argc,const char *argv[]);
		static std::string GetProperty(const std::string& key, const std::string defaultvalue="");//lua
		static void SetProperty(const std::string& key, const std::string& value);//lua
        static void Print(const std::string& s);//lua
        static void Print(Object* o);//lua
		static void Print(Object& o);//lua
        static void Print(const int f);
        static void Print(const float f);
        static void Print(const double f);//lua
		static void Print(const long i);
		static void Print(const unsigned long i);
		static void AddHook(const int hookid, void* hook);
		static void RemoveHook(const int hookid, void* hook);
		static std::string GetPlatformName();//lua
		static bool IgnoreOpenGLErrors;//lua
		//Hook* AddHook(const int hookid, Object* (*callback)(Object* extra), Object* extra = NULL);
		static Object* DebugObject0;
		static void GCSuspend();//lua
		static void GCResume();//lua
		static const int CALLBACK_NAVTILEUPDATE;
		static std::string AppName;//lua
		static int GetVersion();
		static void Shutdown();

		static void* MemAlloc(int size);
		static void MemFree(void* mem);
		static void MemSet(void* mem, int val, int size);
		static void MemCopy(void* dst, void* src, int size);
		static std::map<void*, int> MemAlloced;

        static std::vector<iVec2> graphicsmodes;
        static int CountGraphicsModes();//lua
        static iVec2 GetGraphicsMode(const int index);//lua
	};
}
