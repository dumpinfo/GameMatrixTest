 

#include "C4StringTable.h"


using namespace C4;


ResourceDescriptor StringTableResource::descriptor("str");

const char StringTable::missingString[] = "<missing>";


StringTableResource::StringTableResource(const char *name, ResourceCatalog *catalog) : Resource<StringTableResource>(name, catalog)
{
}

StringTableResource::~StringTableResource()
{
}


StringTable::StringTable(const char *name)
{
	stringTableResource = StringTableResource::Get(name);
	stringHeader = (stringTableResource) ? stringTableResource->GetStringHeader() : nullptr;
}

StringTable::~StringTable()
{
	if (stringTableResource)
	{
		stringTableResource->Release();
	}
}

const char *StringTable::GetString(const StringID& stringID) const
{
	unsigned_int32 count = 0;
	unsigned_int32 size = stringID.size;

	const StringHeader *sh = stringHeader;
	while (sh)
	{
		unsigned_int32 id = stringID.id[count];
		for (;;)
		{
			if (sh->GetStringID() == id)
			{
				if (++count == size)
				{
					return (sh->GetString());
				}

				break;
			}

			sh = sh->GetNextString();
			if (!sh)
			{
				goto missing;
			}
		}

		sh = sh->GetFirstSubstring();
	}

	missing:
	return (missingString);
}

// ZYUQURM
