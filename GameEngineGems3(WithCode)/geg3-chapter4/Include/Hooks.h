#pragma once

#include <map>

namespace Leadwerks
{
	#define HOOK_WRITELOG 1
    #define HOOK_RUNTIMEERROR 2
    
	extern std::map<int,char*> hooks;
    void SetHook(const int hookid, char* hook);
    char* GetHook(const int hookid);
}
