 

#include "C4Files.h"


using namespace C4;


FileMgr *C4::TheFileMgr = nullptr;


namespace C4
{
	template <> FileMgr Manager<FileMgr>::managerObject(0);
	template <> FileMgr **Manager<FileMgr>::managerPointer = &TheFileMgr;

	template <> const char *const Manager<FileMgr>::resultString[] =
	{
		nullptr,
		"File open failed",
		"File IO error",
		"File locked",
		"File access denied",
		"File write protected",
		"File disk full",
		"File creation failed",
		"File deletion failed"
	};

	template <> const unsigned_int32 Manager<FileMgr>::resultIdentifier[] =
	{
		0, 'OPEN', 'IOIO', 'LOCK', 'ACCS', 'PROT', 'FULL', 'CREA', 'DELT'
	};

	template class Manager<FileMgr>;
}


#if C4WINDOWS

	FileName::FileName(const char *name)
	{
		char *output = fileName;
		char prev = 0;

		for (machine a = 0; a < kMaxFilePathLength; a++)
		{
			char c = *name;
			if (c != '/')
			{
				*output = c;
				if (c == 0)
				{
					break;
				}

				output++;
			}
			else if (prev != '/')
			{
				*output = '\\';
				output++;
			}

			prev = c;
			name++;
		}
	}

#elif C4MACOS

	FileName::FileName(const char *name)
	{
		if (name[0] == '/')
		{
			fileName = name;
		}
		else
		{
			fileName = TheFileMgr->GetResourcesPath();
			fileName += name;
		}
	}

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]


FileReference::FileReference(const char *name, unsigned_int32 flags)
{
	fileName = name;
	fileFlags = flags;
}

FileReference::~FileReference()
{ 
}

 
File::File()
{ 
	fileOpen = false;
	filePosition = 0;
} 

File::~File() 
{ 
	Close();
}

FileResult File::TranslateErrorCode(int32 error, FileResult defaultResult) 
{
	#if C4WINDOWS

		if (error == ERROR_ACCESS_DENIED)
		{
			return (kFileAccessDenied);
		}

		if (error == ERROR_WRITE_PROTECT)
		{
			return (kFileWriteProtected);
		}

		if ((error == ERROR_HANDLE_DISK_FULL) || (error == ERROR_DISK_FULL))
		{
			return (kFileDiskFull);
		}

	#elif C4POSIX

		if ((error == EACCES) || (error == EDQUOT))
		{
			return (kFileAccessDenied);
		}

		if (error == ENOSPC)
		{
			return (kFileDiskFull);
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	return (defaultResult);
}

FileResult File::Open(const char *name, FileOpenMode mode)
{
	if (!fileOpen)
	{
		#if C4WINDOWS

			DWORD access = (mode == kFileReadOnly) ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
			DWORD creation = (mode == kFileCreate) ? CREATE_ALWAYS : OPEN_EXISTING;

			fileHandle = CreateFileA(FileName(name), access, FILE_SHARE_READ, nullptr, creation, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				return (TranslateErrorCode(GetLastError(), kFileOpenFailed));
			}

		#elif C4POSIX

			if (mode == kFileCreate)
			{
				fileDesc = open(FileName(name), O_RDWR | O_CREAT | O_TRUNC, 0666);
			}
			else
			{
				fileDesc = open(FileName(name), (mode == kFileReadOnly) ? O_RDONLY : O_RDWR, 0);
			}

			if (fileDesc < 0)
			{
				return (TranslateErrorCode(errno, kFileOpenFailed));
			}

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		fileOpen = true;
		filePosition = 0;
		return (kFileOkay);
	}

	return (kFileOpenFailed);
}

void File::Close(void)
{
	if (fileOpen)
	{
		fileOpen = false;
		filePosition = 0;

		#if C4WINDOWS

			CloseHandle(fileHandle);

		#elif C4POSIX

			close(fileDesc);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}
}

#if C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

FileResult File::Read(void *buffer, unsigned_int32 size)
{
	if (!fileOpen)
	{
		return (kFileNotOpen);
	}

	Assert(buffer, "File::Read(), reading to nullptr\n");

	#if C4WINDOWS

		LARGE_INTEGER	position;
		DWORD			actual;

		position.QuadPart = filePosition;
		SetFilePointerEx(fileHandle, position, nullptr, FILE_BEGIN);

		if ((ReadFile(fileHandle, buffer, size, &actual, nullptr)) && (actual == size))
		{
			filePosition += size;
			return (kFileOkay);
		}

		return (TranslateErrorCode(GetLastError(), kFileIOFailed));

	#elif C4POSIX

		lseek(fileDesc, filePosition, SEEK_SET);

		if (read(fileDesc, buffer, size) >= 0)
		{
			filePosition += size;
			return (kFileOkay);
		}

		return (TranslateErrorCode(errno, kFileIOFailed));

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

FileResult File::Write(const void *buffer, unsigned_int32 size)
{
	if (!fileOpen)
	{
		return (kFileNotOpen);
	}

	Assert(buffer, "File::Write(), writing from nullptr\n");

	#if C4WINDOWS

		LARGE_INTEGER	position;
		DWORD			actual;

		position.QuadPart = filePosition;
		SetFilePointerEx(fileHandle, position, nullptr, FILE_BEGIN);

		if ((WriteFile(fileHandle, buffer, size, &actual, nullptr)) && (actual == size))
		{
			filePosition += size;
			return (kFileOkay);
		}

		return (TranslateErrorCode(GetLastError(), kFileIOFailed));

	#elif C4POSIX

		lseek(fileDesc, filePosition, SEEK_SET);

		if (write(fileDesc, buffer, size) >= 0)
		{
			filePosition += size;
			return (kFileOkay);
		}

		return (TranslateErrorCode(errno, kFileIOFailed));

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

unsigned_int64 File::GetSize(void) const
{
	if (!fileOpen)
	{
		return (0);
	}

	#if C4WINDOWS

		LARGE_INTEGER	size;

		GetFileSizeEx(fileHandle, &size);
		return (size.QuadPart);

	#elif C4POSIX

		struct stat		stat;

		fstat(fileDesc, &stat);
		return (stat.st_size);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void File::SetSize(unsigned_int64 size)
{
	if (fileOpen)
	{
		#if C4WINDOWS

			LARGE_INTEGER	position;

			position.QuadPart = size;
			SetFilePointerEx(fileHandle, position, nullptr, FILE_BEGIN);
			SetEndOfFile(fileHandle);

		#elif C4POSIX

			ftruncate(fileDesc, size);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}
}

unsigned_int64 File::SetPosition(int64 position, FilePositioningMode mode)
{
	if (!fileOpen)
	{
		return (0);
	}

	switch (mode)
	{
		case kFileBegin:

			filePosition = position;
			break;

		case kFileCurrent:

			filePosition += position;
			break;

		case kFileEnd:

			filePosition = GetSize() - position;
			break;
	}

	return (filePosition);
}

FileResult File::WritePad(int32 align)
{
	static const int32 zero[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	if (!fileOpen)
	{
		return (kFileNotOpen);
	}

	unsigned_int64 mask = align - 1;
	int32 len = (int32) (((filePosition + mask) & ~mask) - filePosition);
	if (len != 0)
	{
		return (Write(zero, len));
	}

	return (kFileOkay);
}

File& File::operator <<(const char *text)
{
	unsigned_int32 len = Text::GetTextLength(text);
	if (len != 0)
	{
		Write(text, len);
	}

	return (*this);
}


FileMgr::FileMgr(int)
{
}

FileMgr::~FileMgr()
{
}

EngineResult FileMgr::Construct(void)
{
	#if C4MACOS || C4IOS

		FilePath	bundlePath;

		CFBundleRef applicationBundle = CFBundleGetMainBundle();

		CFURLRef bundleURL = CFBundleCopyBundleURL(applicationBundle);
		CFURLGetFileSystemRepresentation(bundleURL, false, reinterpret_cast<UInt8 *>(&bundlePath), PATH_MAX);
		CFRelease(bundleURL);

		chdir(bundlePath);

		#if C4MACOS

			CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(applicationBundle);
			CFURLGetFileSystemRepresentation(resourcesURL, false, reinterpret_cast<UInt8 *>(&resourcesPath), PATH_MAX);
			CFRelease(resourcesURL);

			resourcesPath += '/';

			// The plugins path needs to start with a slash so that the
			// FileName class does not prepend the resources path to it.

			CFURLRef pluginsURL = CFBundleCopyBuiltInPlugInsURL(applicationBundle);
			CFURLGetFileSystemRepresentation(pluginsURL, false, reinterpret_cast<UInt8 *>(&pluginsPath[1]), PATH_MAX - 1);
			CFRelease(pluginsURL);

			pluginsPath[0] = '/';
			pluginsPath += '/';

		#endif

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	return (kEngineOkay);
}

void FileMgr::Destruct(void)
{
	#if C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

FileResult FileMgr::DeleteFile(const char *name)
{
	#if C4WINDOWS

		if (DeleteFileA(FileName(name)))
		{
			return (kFileOkay);
		}

		return (kFileDeleteFailed);

	#elif C4POSIX

		if (unlink(FileName(name)) != 0)
		{
			return (kFileDeleteFailed);
		}

		return (kFileOkay);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

FileResult FileMgr::CreateDirectory(const char *directory)
{
	#if C4WINDOWS

		if (!CreateDirectoryA(FileName(directory), nullptr))
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				return (kFileCreateFailed);
			}
		}

		return (kFileOkay);

	#elif C4POSIX

		if ((mkdir(FileName(directory), 0777) != 0) && (errno != EEXIST))
		{
			return (kFileCreateFailed);
		}

		return (kFileOkay);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

FileResult FileMgr::CreateDirectoryPath(const char *path)
{
	if (path[0] != 0)
	{
		int32 start = 1;
		while (path[start] != 0)
		{
			String<kMaxFileNameLength>	directory;

			int32 slash = Text::FindChar(path + start, '/');
			if (slash < 0)
			{
				break;
			}

			directory.Set(path, start + slash);
			start += slash + 1;

			if (CreateDirectory(directory) != kFileOkay)
			{
				return (kFileCreateFailed);
			}
		}
	}

	return (kFileOkay);
}

bool FileMgr::DefaultFilter(const char *name, unsigned_int32 flags, void *cookie)
{
	return (((flags & kFileInvisible) == 0) && (name[0] != '.'));
}

bool FileMgr::FileFilter(const char *name, unsigned_int32 flags, void *cookie)
{
	return (((flags & (kFileDirectory | kFileInvisible)) == 0) && (name[0] != '.'));
}

bool FileMgr::DirectoryFilter(const char *name, unsigned_int32 flags, void *cookie)
{
	return (((flags & (kFileDirectory | kFileInvisible)) == kFileDirectory) && (name[0] != '.'));
}

void FileMgr::BuildFileMap(const char *directory, Map<FileReference> *map, FileFilterProc *proc, void *cookie)
{
	MapReservation		reservation;

	if (!proc)
	{
		proc = &DefaultFilter;
	}

	#if C4WINDOWS

		WIN32_FIND_DATA		findData;

		HANDLE h = FindFirstFileA(FileName(String<kMaxFileNameLength>(directory) += "/*.*"), &findData);
		if (h == INVALID_HANDLE_VALUE)
		{
			return;
		}

		do
		{
			unsigned_int32 flags = 0;

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				flags |= kFileDirectory;
			}

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			{
				flags |= kFileInvisible;
			}

			const char *name = findData.cFileName;
			if (((*proc)(name, flags, cookie)) && (map->Reserve(name, &reservation)))
			{
				map->Insert(new FileReference(name, flags), &reservation);
			}
		} while (FindNextFileA(h, &findData));

		FindClose(h);

	#elif C4POSIX

		FilePath path = (const char *) FileName(directory);

		int32 x = path.Length() - 1;
		if (path[x] == '/')
		{
			path[x] = 0;
		}

		DIR *dir = opendir(&path[path[0] == '/']);
		if (dir)
		{
			for (;;)
			{
				const dirent *dirent = readdir(dir);
				if (!dirent)
				{
					break;
				}

				unsigned_int32 flags = 0;
				if (dirent->d_type == DT_DIR)
				{
					flags |= kFileDirectory;
				}
				else if (dirent->d_type != DT_REG)
				{
					continue;
				}

				const char *name = dirent->d_name;
				if (((*proc)(name, flags, cookie)) && (map->Reserve(name, &reservation)))
				{
					map->Insert(new FileReference(name, flags), &reservation);
				}
			}

			closedir(dir);
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

// ZYUQURM
