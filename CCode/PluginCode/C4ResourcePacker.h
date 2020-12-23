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


#ifndef C4ResourcePacker_h
#define C4ResourcePacker_h


#include "C4Plugins.h"
#include "C4StringTable.h"


extern "C"
{
	C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
}


namespace C4
{
	class FileEntry : public MapElement<FileEntry>
	{
		private:

			ResourceName			fileName;
			ResourcePath			filePath;
			unsigned_int32			fileSize;

			unsigned_int32			entryOffset;
			unsigned_int32			dataStart;

		public:

			typedef StringKey KeyType;

			FileEntry(const char *name, const char *path, unsigned_int32 size);
			~FileEntry();

			KeyType GetKey(void) const
			{
				return (fileName);
			}

			unsigned_int32 GetEntryOffset(void) const
			{
				return (entryOffset);
			}

			void CalculatePositions(unsigned_int32& mapOffset, unsigned_int32& dataSize);

			void WriteMapData(File *file) const;
			void WriteResourceData(File *file) const;
	};


	class TypeEntry : public MapElement<TypeEntry>
	{
		private:

			ResourceType			resourceType;
			unsigned_int32			entryOffset;

			Map<FileEntry>			fileMap;

		public:

			typedef ResourceType KeyType;

			TypeEntry(ResourceType type);
			~TypeEntry();

			KeyType GetKey(void) const
			{
				return (resourceType);
			}

			unsigned_int32 GetEntryOffset(void) const
			{
				return (entryOffset);
			}

			void AddFile(FileEntry *file)
			{
				fileMap.Insert(file);
			}

			void CalculateTypePositions(unsigned_int32& mapOffset);
			void CalculateFilePositions(unsigned_int32& mapOffset, unsigned_int32& dataSize);

			void WriteTypeMapData(File *file) const;
			void WriteFileMapData(File *file) const;
			void WriteResourceData(File *file) const;
	};


	class DirectoryEntry : public MapElement<DirectoryEntry>
	{
		private:

			ResourceName			directoryName;
			unsigned_int32			entryOffset;

			Map<TypeEntry>			typeMap;
			Map<DirectoryEntry>		*directoryMap;
 
		public:

			typedef StringKey KeyType; 

			DirectoryEntry(const char *name); 
			~DirectoryEntry();

			KeyType GetKey(void) const 
			{
				return (directoryName); 
			} 

			unsigned_int32 GetEntryOffset(void) const
			{
				return (entryOffset); 
			}

			void AddType(TypeEntry *type)
			{
				typeMap.Insert(type);
			}

			TypeEntry *FindType(ResourceType type) const
			{
				return (typeMap.Find(type));
			}

			TypeEntry *GetTypeRoot(void) const
			{
				return (typeMap.GetRootNode());
			}

			void AddDirectory(DirectoryEntry *directory)
			{
				directoryMap->Insert(directory);
			}

			DirectoryEntry *GetDirectoryRoot(void) const
			{
				return (directoryMap->GetRootNode());
			}

			void CalculatePositions(unsigned_int32& mapOffset, unsigned_int32& dataSize);

			void WriteMapData(File *file) const;
			void WriteResourceData(File *file) const;
	};


	class ResourcePacker : public Plugin, public Singleton<ResourcePacker>
	{
		private:

			StringTable							stringTable;

			CommandObserver<ResourcePacker>		packCommandObserver;
			Command								packCommand;

			static DirectoryEntry *ProcessDirectory(const char *name, const char *path);

			void HandlePackCommand(Command *command, const char *text);

		public:

			ResourcePacker();
			~ResourcePacker();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}
	};


	extern ResourcePacker *TheResourcePacker;
}


#endif

// ZYUQURM
