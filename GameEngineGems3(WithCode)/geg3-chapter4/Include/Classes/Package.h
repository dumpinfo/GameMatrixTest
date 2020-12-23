#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class PackageFile;
	
	class Package : Object
	{
	public:
		bool modified;
		bool passwordverified;
        //Zlib doesn't build in Xcode DYLib
//#ifdef ZLIB        
        unzFile zip;
//#endif
        
        std::string path;
		std::string password;
		map<std::string,PackageFile*> contents;
		map<std::string,std::string> folders;
		PackageFile* root;
		bool isgame;

		Package();
		virtual ~Package();
		
		virtual std::string GetFileName(const int index);
		virtual int CountFiles();
		virtual bool ExtractAll(const std::string& path, const bool onlynewer=false);
		virtual bool ExtractFile(const std::string& file, const std::string& path, const bool onlynewer = false, const bool stripdir = false);
		virtual bool Close();
		virtual bool Reload();
		
		static Package* Load(const std::string& path, const std::string& password="", const bool regist=true);
	};
}



