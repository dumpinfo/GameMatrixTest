#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Package;

	class PackageFile
	{
	public:
		Package* package;
		int position;
		int size;
		int type;
		int compressionlevel;
		int flags;
		std::string name;
		Bank* data;
		std::string path;
		std::vector<PackageFile*> files;
		
		PackageFile();
		virtual ~PackageFile();
		
		BankStream* Read();//const std::string& password="");
		bool Save(const std::string& path, const int compressionlevel = 0, const std::string& encryptionkey = "");
		
		static PackageFile* Find(const std::string& path);
		static unsigned long crcTable[256];
		static unsigned long getcrc( const char* buf, const int size );
		static bool Find(std::string& path, uint64_t& fileid);
	};
	
	Package* LoadPackage(const std::string& path);
}
