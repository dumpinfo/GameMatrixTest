#pragma once

#include "Leadwerks.h"

#ifdef _WIN32
	#include <Shlobj.h>
#endif
#ifdef PLATFORM_MAC
	#include "FileSystem.MacOS.h"
#endif

#undef CreateFile
#undef CopyFile
#undef DeleteFile

namespace Leadwerks
{
	class Stream;
	class FileFactory;

    class FileSystem//lua
    {
    public:

        //File string commands
        static std::string ExtractDir(const std::string& path);//lua
        static std::string ExtractExt(const std::string& path);//lua
        static std::string StripDir(const std::string& path);//lua
        static std::string StripExt(const std::string& path);//lua
        static std::string StripAll(const std::string& path);//lua
        static std::string FixPath(const std::string& path);
        static std::string RealPath(const std::string& path);//lua
        static std::string CasedPath(const std::string& path);//lua
        static std::string CasedFileName(const std::string& path, const std::string& file, const bool isroot);//helper function
		static bool FileFactoriesInitialized_;
		static vector<FileFactory*> FileFactories__;
		static std::string GetDocumentsPath();
        //Application path commands
        static std::string GetDir();//lua
        static bool SetDir(const std::string& path);//lua

		static std::map<std::string,std::string> VirtualFileSystem;

        //File factory commands
        static void InitFileFactories();
        static std::string LoadString(const std::string& path); 
        static Stream* ReadFile(const std::string& path, uint64_t fileid, const std::string key);
		static Stream* ReadFile(const std::string& path);//lua
        static int GetFileSize(const std::string& path);//lua
		static int GetFileType(const std::string& path, const uint64_t fileid = 0);//lua
        static unsigned int GetFileTime(const std::string& path);//lua
        static int GetFileMode(const std::string& path);
        static Directory* LoadDir(const std::string& path);
        static bool LoadDir(const std::string& path,std::vector<std::string>& files);
		static std::string GetAppDataPath();//lua
		static const int File;//lua
		static const int Dir;//lua

		//=======================================================================================
		//Write functions - Do not expose these in Leadwerks 3.1
		//=======================================================================================
		static Stream* WriteFile(const std::string& path);//lua
		static Stream* OpenFile(const std::string& path);//lua
		static bool CreateFile(const std::string& path);//lua
		static bool DeleteFile(const std::string& path);//lua
		static bool CopyFile(const std::string& src, const std::string& dst);//lua
		static bool RenameFile(const std::string& src, const std::string& dst);//lua
		static bool DeleteDir(const std::string& path, const bool recursive = false);//lua
		static bool CreateDir(const std::string& path, const bool recursive = false);//lua
		static bool RenameDir(const std::string& src, const std::string& dst);//lua
		static bool CopyDir(const std::string& src, const std::string& dst);//lua
		static bool SetFileMode(const std::string& path, const int mode);
		//=======================================================================================

		//=======================================================================================

		static std::map<std::string,std::string> StaticInitializationOrderFiascoAvoider1();
		static bool StaticInitializationOrderFiascoAvoider2();
		static vector<FileFactory*> StaticInitializationOrderFiascoAvoider3();

#ifdef PLATFORM_ANDROID
		static std::string AndroidAppDataPath;
#endif
#ifdef PLATFORM_IOS
		static std::string iOSAppDataPath;
#endif
	};
}

#ifdef PLATFORM_MACOS
extern std::string MacOSGetAppDataPath();
#endif
