#include "../../../Leadwerks.h"

#undef DeleteFile
#undef CreateFile
#undef CopyFile

namespace Leadwerks
{	
	class PackageFactory : public FileFactory
	{
		public:
		
		virtual Stream* ReadFile(const std::string& path);
		virtual Stream* WriteFile(const std::string& path);
		virtual Stream* OpenFile(const std::string& path);
		
		virtual bool CreateFile(const std::string& path);
		virtual bool DeleteFile(const std::string& path);
		virtual bool CopyFile(const std::string& src, const std::string& dst);
		virtual bool RenameFile(const std::string& src, const std::string& dst);
		
		virtual bool DeleteDir(const std::string& path, const bool recursive);
		virtual bool CreateDir(const std::string& path, const bool recursive);
		virtual bool RenameDir(const std::string& src, const std::string& dst);
		
		virtual int GetFileSize(const std::string& path);
		virtual int GetFileType(const std::string& path);
		virtual unsigned int GetFileTime(const std::string& path);
		virtual int GetFileMode(const std::string& path);
		virtual bool SetFileMode(const std::string& path, const int mode);
		
		virtual Directory* LoadDir(const std::string& path);
	};
	
	extern std::map<std::string,PackageFile*> PackageMap__;
	
	bool RegisterPackage(Package* package);
	//void RegisterDir(const std::string& path);
}
