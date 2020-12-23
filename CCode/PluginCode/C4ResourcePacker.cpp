//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "C4ResourcePacker.h"


using namespace C4;


ResourcePacker *C4::TheResourcePacker = nullptr;


C4::Plugin *CreatePlugin(void)
{
	return (new ResourcePacker);
}


FileEntry::FileEntry(const char *name, const char *path, unsigned_int32 size)
{
	fileName = name;
	filePath = path;
	fileSize = size;
}

FileEntry::~FileEntry()
{
}

void FileEntry::CalculatePositions(unsigned_int32& mapOffset, unsigned_int32& dataSize)
{
	entryOffset = mapOffset;
	mapOffset += sizeof(PackFileEntry) + MaxZero(((fileName.Length() + 4) & ~3) - 4);

	dataStart = dataSize >> 4;
	dataSize += (fileSize + 15) & ~15;

	FileEntry *entry = GetLeftSubnode();
	if (entry)
	{
		entry->CalculatePositions(mapOffset, dataSize);
	}

	entry = GetRightSubnode();
	if (entry)
	{
		entry->CalculatePositions(mapOffset, dataSize);
	}
}

void FileEntry::WriteMapData(File *file) const
{
	PackFileEntry	output;

	const FileEntry *leftEntry = GetLeftSubnode();
	output.leftOffset = (leftEntry) ? leftEntry->GetEntryOffset() : 0;

	const FileEntry *rightEntry = GetRightSubnode();
	output.rightOffset = (rightEntry) ? rightEntry->GetEntryOffset() : 0;

	output.dataStart = dataStart;
	output.dataSize = fileSize;

	file->Write(&output, sizeof(PackFileEntry) - 4);
	file->Write(&fileName, fileName.Length() + 1);
	file->WritePad(4);

	if (leftEntry)
	{
		leftEntry->WriteMapData(file);
	}

	if (rightEntry)
	{
		rightEntry->WriteMapData(file);
	}
}

void FileEntry::WriteResourceData(File *file) const
{
	File	resource;

	resource.Open(filePath);
	char *data = new char[fileSize];
	resource.Read(data, fileSize);
	resource.Close();

	file->Write(data, fileSize);
	file->WritePad(16);

	delete[] data;

	const FileEntry *entry = GetLeftSubnode();
	if (entry)
	{
		entry->WriteResourceData(file);
	}

	entry = GetRightSubnode();
	if (entry)
	{
		entry->WriteResourceData(file);
	}
} 


TypeEntry::TypeEntry(ResourceType type) 
{
	resourceType = type; 
}

TypeEntry::~TypeEntry() 
{
} 
 
void TypeEntry::CalculateTypePositions(unsigned_int32& mapOffset)
{
	entryOffset = mapOffset;
	mapOffset += sizeof(PackTypeEntry); 

	TypeEntry *entry = GetLeftSubnode();
	if (entry)
	{
		entry->CalculateTypePositions(mapOffset);
	}

	entry = GetRightSubnode();
	if (entry)
	{
		entry->CalculateTypePositions(mapOffset);
	}
}

void TypeEntry::CalculateFilePositions(unsigned_int32& mapOffset, unsigned_int32& dataSize)
{
	FileEntry *fileEntry = fileMap.GetRootNode();
	if (fileEntry)
	{
		fileEntry->CalculatePositions(mapOffset, dataSize);
	}

	TypeEntry *typeEntry = GetLeftSubnode();
	if (typeEntry)
	{
		typeEntry->CalculateFilePositions(mapOffset, dataSize);
	}

	typeEntry = GetRightSubnode();
	if (typeEntry)
	{
		typeEntry->CalculateFilePositions(mapOffset, dataSize);
	}
}

void TypeEntry::WriteTypeMapData(File *file) const
{
	PackTypeEntry	output;

	const TypeEntry *leftEntry = GetLeftSubnode();
	output.leftOffset = (leftEntry) ? leftEntry->GetEntryOffset() : 0;

	const TypeEntry *rightEntry = GetRightSubnode();
	output.rightOffset = (rightEntry) ? rightEntry->GetEntryOffset() : 0;

	output.resourceType = resourceType;

	const FileEntry *fileEntry = fileMap.GetRootNode();
	output.fileRootOffset = (fileEntry) ? fileEntry->GetEntryOffset() : 0;

	file->Write(&output, sizeof(PackTypeEntry));

	if (leftEntry)
	{
		leftEntry->WriteTypeMapData(file);
	}

	if (rightEntry)
	{
		rightEntry->WriteTypeMapData(file);
	}
}

void TypeEntry::WriteFileMapData(File *file) const
{
	FileEntry *fileEntry = fileMap.GetRootNode();
	if (fileEntry)
	{
		fileEntry->WriteMapData(file);
	}

	TypeEntry *typeEntry = GetLeftSubnode();
	if (typeEntry)
	{
		typeEntry->WriteFileMapData(file);
	}

	typeEntry = GetRightSubnode();
	if (typeEntry)
	{
		typeEntry->WriteFileMapData(file);
	}
}

void TypeEntry::WriteResourceData(File *file) const
{
	FileEntry *fileEntry = fileMap.GetRootNode();
	if (fileEntry)
	{
		fileEntry->WriteResourceData(file);
	}

	TypeEntry *typeEntry = GetLeftSubnode();
	if (typeEntry)
	{
		typeEntry->WriteResourceData(file);
	}

	typeEntry = GetRightSubnode();
	if (typeEntry)
	{
		typeEntry->WriteResourceData(file);
	}
}


DirectoryEntry::DirectoryEntry(const char *name)
{
	directoryName = name;
	directoryMap = new Map<DirectoryEntry>;
}

DirectoryEntry::~DirectoryEntry()
{
	delete directoryMap;
}

void DirectoryEntry::CalculatePositions(unsigned_int32& mapOffset, unsigned_int32& dataSize)
{
	entryOffset = mapOffset;
	mapOffset += sizeof(PackDirectoryEntry) + MaxZero(((directoryName.Length() + 4) & ~3) - 4);

	DirectoryEntry *directoryEntry = GetLeftSubnode();
	if (directoryEntry)
	{
		directoryEntry->CalculatePositions(mapOffset, dataSize);
	}

	directoryEntry = GetRightSubnode();
	if (directoryEntry)
	{
		directoryEntry->CalculatePositions(mapOffset, dataSize);
	}

	TypeEntry *typeEntry = GetTypeRoot();
	if (typeEntry)
	{
		typeEntry->CalculateTypePositions(mapOffset);
		typeEntry->CalculateFilePositions(mapOffset, dataSize);
	}

	directoryEntry = GetDirectoryRoot();
	if (directoryEntry)
	{
		directoryEntry->CalculatePositions(mapOffset, dataSize);
	}
}

void DirectoryEntry::WriteMapData(File *file) const
{
	PackDirectoryEntry	output;

	const DirectoryEntry *leftEntry = GetLeftSubnode();
	output.leftOffset = (leftEntry) ? leftEntry->GetEntryOffset() : 0;

	const DirectoryEntry *rightEntry = GetRightSubnode();
	output.rightOffset = (rightEntry) ? rightEntry->GetEntryOffset() : 0;

	const TypeEntry *typeEntry = GetTypeRoot();
	output.typeRootOffset = (typeEntry) ? typeEntry->GetEntryOffset() : 0;

	const DirectoryEntry *directoryEntry = GetDirectoryRoot();
	output.directoryRootOffset = (directoryEntry) ? directoryEntry->GetEntryOffset() : 0;

	file->Write(&output, sizeof(PackDirectoryEntry) - 4);
	file->Write(&directoryName, directoryName.Length() + 1);
	file->WritePad(4);

	if (leftEntry)
	{
		leftEntry->WriteMapData(file);
	}

	if (rightEntry)
	{
		rightEntry->WriteMapData(file);
	}

	if (typeEntry)
	{
		typeEntry->WriteTypeMapData(file);
		typeEntry->WriteFileMapData(file);
	}

	if (directoryEntry)
	{
		directoryEntry->WriteMapData(file);
	}
}

void DirectoryEntry::WriteResourceData(File *file) const
{
	DirectoryEntry *directoryEntry = GetLeftSubnode();
	if (directoryEntry)
	{
		directoryEntry->WriteResourceData(file);
	}

	directoryEntry = GetRightSubnode();
	if (directoryEntry)
	{
		directoryEntry->WriteResourceData(file);
	}

	TypeEntry *typeEntry = GetTypeRoot();
	if (typeEntry)
	{
		typeEntry->WriteResourceData(file);
	}

	directoryEntry = GetDirectoryRoot();
	if (directoryEntry)
	{
		directoryEntry->WriteResourceData(file);
	}
}


ResourcePacker::ResourcePacker() :
		Singleton<ResourcePacker>(TheResourcePacker),
		stringTable("ResourcePacker/strings"),
		packCommandObserver(this, &ResourcePacker::HandlePackCommand),
		packCommand("pack", &packCommandObserver)
{
	TheEngine->AddCommand(&packCommand);
}

ResourcePacker::~ResourcePacker()
{
}

DirectoryEntry *ResourcePacker::ProcessDirectory(const char *name, const char *path)
{
	Map<FileReference>		fileMap;
	File					file;

	FileMgr::BuildFileMap(path, &fileMap);
	if (!fileMap.Empty())
	{
		DirectoryEntry *directoryEntry = new DirectoryEntry(name);

		const FileReference *reference = fileMap.First();
		while (reference)
		{
			const char *fileName = reference->GetName();

			ResourcePath fullPath(path);
			fullPath += '/';
			fullPath += fileName;

			if (reference->GetFlags() & kFileDirectory)
			{
				DirectoryEntry *entry = ProcessDirectory(fileName, fullPath);
				if (entry)
				{
					directoryEntry->AddDirectory(entry);
				}
			}
			else
			{
				ResourceName resourceName(fileName);
				int32 len = resourceName.Length();

				if ((len > 4) && (resourceName[len - 4] == '.'))
				{
					ResourceType type = (resourceName[len - 3] << 16) | (resourceName[len - 2] << 8) | resourceName[len - 1];

					TypeEntry *typeEntry = directoryEntry->FindType(type);
					if (!typeEntry)
					{
						typeEntry = new TypeEntry(type);
						directoryEntry->AddType(typeEntry);
					}

					file.Open(fullPath);
					unsigned_int64 size = file.GetSize();
					file.Close();

					resourceName[Text::GetResourceNameLength(resourceName)] = 0;
					typeEntry->AddFile(new FileEntry(resourceName, fullPath, (unsigned_int32) size));
				}
			}

			reference = reference->Next();
		}

		return (directoryEntry);
	}

	return (nullptr);
}

void ResourcePacker::HandlePackCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourcePath	directory;

		Text::ReadString(text, directory, kMaxResourcePathLength);
		ResourcePath fullPath(TheResourceMgr->GetGenericCatalog()->GetRootPath());
		fullPath += directory;

		DirectoryEntry *entry = ProcessDirectory("", fullPath);
		if (entry)
		{
			PackHeader	packHeader;
			File		packFile;

			unsigned_int32 mapOffset = 0;
			unsigned_int32 dataSize = 0;

			entry->CalculatePositions(mapOffset, dataSize);
			mapOffset = (mapOffset + 15) & ~15;

			packHeader.endian = 1;
			packHeader.version = 1;
			packHeader.packFileType = kPackFileDefault;
			packHeader.mapSize = mapOffset;

			fullPath += ".pak";
			if (packFile.Open(fullPath, kFileCreate) == kFileOkay)
			{
				packFile.Write(&packHeader, sizeof(PackHeader));

				entry->WriteMapData(&packFile);
				packFile.WritePad(16);

				entry->WriteResourceData(&packFile);
			}
			else
			{
				String<kMaxCommandLength> output(stringTable.GetString(StringID('FAIL')));
				output += fullPath;
				Engine::Report(output);
			}

			delete entry;
		}
		else
		{
			String<kMaxCommandLength> output(stringTable.GetString(StringID('NDIR')));
			output += directory;
			Engine::Report(output);
		}
	}
}

// ZYUQURM
