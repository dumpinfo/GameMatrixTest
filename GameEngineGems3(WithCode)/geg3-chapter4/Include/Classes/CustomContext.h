#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
    class Buffer;
    class Context;
    
	class CustomContext : public Context
	{
    public:
        int (*_GetWidth) (char*);
        int (*_GetHeight) (char*);
        void (*_MakeCurrent) (char*);
        void (*_Sync) (char*,int);
        void (*_Clear) (char*,int);
        char* extra;
        
        CustomContext();
		virtual ~CustomContext();
		
		virtual void SetMask(const bool red, const bool green, const bool blue, const bool alpha);
		virtual void SetAntialias(const bool mode);
		virtual int GetWidth();
		virtual int GetHeight();
		virtual void Enable();
		virtual void Sync(const bool sync=true);
		virtual void GetPixels(const char* buf, const int component);
		virtual void Clear(const int mode);
		virtual void Blit(Buffer* dst, const int components);
        //CustomContext* CreateCustomContext( char* extra, int GetWidth(char* extra), int GetHeight(char* extra), void MakeCurrent(char* extra), void Sync(char* extra,int sync) );
       static  CustomContext* Create( char* extra, int (*GetWidth)(char*), int (*GetHeight) (char*), void (*MakeCurrent) (char*), void (*Sync) (char*,int sync), void (*Clear)(char* extra, int mode) );
	
    };
}
