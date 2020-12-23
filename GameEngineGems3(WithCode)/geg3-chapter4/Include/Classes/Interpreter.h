#pragma once
#include "../Leadwerks.h"

#undef require

namespace Leadwerks
{
	class Interpreter//lua
	{
	public:
		static Address DebuggerAddress;
		static lua_State* L;
		static Client* client;
		static Peer* debugger;
		static bool connected;
		static bool skipdebugdefaultglobals;
		static std::map<std::string,Object*> debugobjectmap;
		static std::map<std::string,bool> hiddenglobals;
		static bool running;
        static std::vector<std::string> executionstack;
		static int steppingmode;
		static bool skipnextdebugstep;
		static int STEP_IN;
		static int STEP_OUT;
		static int STEP_LINE;
		//static char* lua_ReaderDataPointer;
		//static size_t lua_ReaderDataSize;
		static std::map<int,std::map<std::string,int> > breakpoints;
		static long LastUpdateTime;
		static bool AllowLuaErrors;
		static uint64_t currentexecutionfileid;
		static bool sandboxmode;

		static bool DisableErrorFunctionCheck;
        static std::map<std::string,bool> ExecutedFiles;
		//static char* lua_Reader(lua_State *L,void *data,size_t *size);
        //static void DebugHook(lua_State* L,lua_Debug* ar);
		static bool LoadBreakpoints(const std::string& path);
		static void DebugHook(lua_State* L,lua_Debug* ar);
		//static void LineHook(lua_State* L,lua_Debug* ar);
		//static void ReturnHook(lua_State* L,lua_Debug* ar);
		static void Reset();//lua
		static int PanicHook(lua_State* L);
		static void Pop(const int levels=1);//lua
		static void PushValue(const int index);//lua
		static void Remove(const int index);//lua
		static void DebugUpdate(const std::string& err="", const bool wait=true);
		static bool DebugStart();
		static int GetStackSize();//lua
		static void PushFloat(const float f);//lua
		static void PushInt(const int i);//lua
		static void SetStackSize(const int size);//lua
		static bool ExecuteString(std::string& source);//lua
		static bool Invoke(const int in, const int out, const int errorfunctionindex);//lua
		static std::string GetCallStack();
		static void HandleError();
		static std::string GetVariableValue(const int index=-1, const int depth=3, const bool quotes=true);
		static int GetCallStackSize();
		static void GetField(const std::string& name,const int index=-1);//lua
		static void SetField(const std::string& name,const int index=-2);//lua
		static void SetTable(const int index=-3);//lua
		static void GetTable(const int index=-2);//lua
		static void GetGlobal(const std::string& s);//lua
		static void PushString(const std::string& s);//lua
		static bool IsString(const int index=-1);//lua
		static std::string ToString(const int index=-1);//lua
		static bool IsNumber(const int index=-1);//lua
		static float ToNumber(const int index=-1);//lua
		static void PushObject(Object* o, const bool takeownership=false);//lua
		static void PushCObject(Object* o);//lua
		static bool IsObject(const int index=-1);//lua
		static Object* ToObject(const int index=-1);//lua
		static void SetGlobal(const std::string& s);//lua
		static bool IsFunction(const int index=-1);//lua
		static bool IsTable(const int index=-1);//lua
		static bool IsBool(const int index=-1);//lua
		static bool ToBool(const int index=-1);//lua
		static void NewTable();//lua
		static void CollectGarbage();//lua
		static void PushVec2(const Vec2& v);//lua
		static void PushVec3(const Vec3& v);//lua
		static void PushVec4(const Vec4& v);//lua
		static void PushMat3(const Mat3& m);//lua
		static void PushMat4(const Mat4& m);//lua
		static void PushAABB(const AABB& aabb);//lua
		static void PushBool(const bool b);//lua
        static bool ExecuteFile(const std::string& path, const uint64_t fileid=0);//lua
		static bool EvaluateFile(const std::string& path);//lua
		static bool EvaluateString(const std::string& path);//lua
		static void PushNull();//lua
		static bool Connect(const std::string& hostname="localhost", const int port=7777, const int timeout=5000);//lua
		static bool Disconnect(const int timeout=5000);//lua
		static void SetBreakpoint(const std::string& source, const int linenumber);
		static void RemoveBreakpoint(const std::string& source, const int linenumber);
		static bool IsBreakpoint(const std::string& source, const int linenumber);
		static bool IsConnected();//lua
		static void CheckError();//lua
		static void ReleaseObject(Object* o);
		static void TakeOwnership(Object* o);
	};
    
    //bool dofile(const std::string& path);
    bool import(const std::string& path);
}
