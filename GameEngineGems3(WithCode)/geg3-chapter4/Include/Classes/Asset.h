#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Asset : public Object//lua
	{
    public:
        int time;
        std::string path;//lua
        std::string storagepath;
        std::list<Asset*>::iterator link;
        Driver* driver;
		uint64_t fileid;
		std::string workshopfilepath;

		Asset();
		virtual ~Asset();

		virtual std::string Debug();
        virtual std::string GetPath();//lua
        virtual bool Reload(const int flags=0)=0;
		virtual void Reset()=0;
		virtual Asset* Copy()=0;//lua
        virtual std::string GetClassName();

        static const int Unmanaged;//lua
        static const int CreateNew;//lua
		static const int LoadQuiet;//lua
		static const int SkipTextures;//lua

        static std::list<Asset*> List;
	};
}
