 

#include "C4Compression.h"


using namespace C4;


namespace C4
{
	namespace Comp
	{
		enum
		{
			kUncompressedTinyLength,
			kUncompressedStandardLength,
			kCompressedTinyLengthShortDistance,
			kCompressedStandardLengthShortDistance,
			kCompressedTinyLengthLongDistance,
			kCompressedStandardLengthLongDistance,
			kCompressionInvalid = 6,
			kCompressionTerminator = 7
		};


		enum
		{
			kWindowSize			= 32768,
			kWindowMask			= kWindowSize - 1,
			kHashBitCount		= 12,
			kHashTableSize		= 1 << kHashBitCount,
			kHashShift			= 16 - kHashBitCount,
			kMaxMatchDepth		= 256,
			kMinMatchCount		= 4,
			kMaxTinyLength		= 31 + kMinMatchCount,
			kMaxShortLength		= 256 + kMaxTinyLength,
			kMaxLongLength		= 65536 + kMaxShortLength
		};

		static_assert((kWindowSize & (kWindowSize - 1)) == 0, "kWindowSize must be a power of two");


		struct DictEntry
		{
			DictEntry				*prev;
			DictEntry				*next;
			const unsigned_int8		*data;
		};


		inline unsigned_machine GetDataHash(const unsigned_int8 *data)
		{
			return ((data[0] << (8 - kHashShift)) | (data[1] >> kHashShift));
		}


		void UpdateDictEntry(DictEntry *dictTable, DictEntry **hashTable, const unsigned_int8 *data);
		unsigned_machine CountMatchingBytes(const unsigned_int8 *x, const unsigned_int8 *y, unsigned_machine max);
		unsigned_machine WriteUncompressedCode(const unsigned_int8 *data, unsigned_machine length, unsigned_int8 *restrict code, unsigned_machine max);
		unsigned_machine WriteCompressedCode(unsigned_machine length, unsigned_machine distance, unsigned_int8 *restrict code, unsigned_machine max);
	}
}


#if C4VISUALC

	#pragma optimize("gt", on)

#endif


void Comp::UpdateDictEntry(DictEntry *dictTable, DictEntry **hashTable, const unsigned_int8 *data)
{
	DictEntry *entry = &dictTable[GetPointerAddress(data) & kWindowMask];
	if (entry->data)
	{
		DictEntry *prev = entry->prev;
		DictEntry *next = entry->next;

		if (prev)
		{
			prev->next = next;
		}
		else
		{
			hashTable[GetDataHash(entry->data)] = next;
		}

		if (next)
		{
			next->prev = prev;
		}
	}

	entry->data = data;
	entry->prev = nullptr;

	DictEntry **start = &hashTable[GetDataHash(data)];
	DictEntry *first = *start;
	if (first)
	{
		entry->next = first;
		first->prev = entry; 
	}
	else
	{ 
		entry->next = nullptr;
	} 

	*start = entry;
} 

unsigned_machine Comp::CountMatchingBytes(const unsigned_int8 *x, const unsigned_int8 *y, unsigned_machine max) 
{ 
	unsigned_machine count = 0;
	do
	{
		if (x[count] != y[count]) 
		{
			break;
		}
	} while (++count < max);

	if (count >= kMinMatchCount)
	{
		return (count);
	}

	return (0);
}

unsigned_machine Comp::WriteUncompressedCode(const unsigned_int8 *data, unsigned_machine length, unsigned_int8 *restrict code, unsigned_machine max)
{
	unsigned_int8 *start = code;
	while (length > 65824)
	{
		if (max < 65827)
		{
			return (0);
		}

		code[0] = (unsigned_int8) ((kUncompressedStandardLength << 5) | 0x01);
		code[1] = 0xFF;
		code[2] = 0xFF;
		code += 3;

		MemoryMgr::CopyMemory(data, code, 65824);

		code += 65824;
		data += 65824;
		length -= 65824;
		max -= 65827;
	}

	if (length != 0)
	{
		if (length <= 32)
		{
			if (length + 1 > max)
			{
				return (0);
			}

			code[0] = (unsigned_int8) ((kUncompressedTinyLength << 5) | (length - 1));
			code++;
		}
		else if (length <= 288)
		{
			if (length + 2 > max)
			{
				return (0);
			}

			code[0] = (unsigned_int8) (kUncompressedStandardLength << 5);
			code[1] = (unsigned_int8) (length - 33);
			code += 2;
		}
		else
		{
			if (length + 3 > max)
			{
				return (0);
			}

			unsigned_machine x = length - 289;
			code[0] = (unsigned_int8) ((kUncompressedStandardLength << 5) | 0x01);
			code[1] = (unsigned_int8) (x >> 8);
			code[2] = (unsigned_int8) x;
			code += 3;
		}

		for (unsigned_machine a = 0; a < length; a++)
		{
			code[a] = data[a];
		}
	}

	return ((unsigned_machine) (code - start + length));
}

unsigned_machine Comp::WriteCompressedCode(unsigned_machine length, unsigned_machine distance, unsigned_int8 *restrict code, unsigned_machine max)
{
	unsigned_machine size = 0;
	if (--distance < 256)
	{
		while (length > kMaxLongLength)
		{
			if (max < 4)
			{
				return (0);
			}

			code[0] = (unsigned_int8) ((kCompressedStandardLengthShortDistance << 5) | 0x01);
			code[1] = 0xFF;
			code[2] = 0xFF;
			code[3] = (unsigned_int8) distance;
			size += 4;

			length -= kMaxLongLength;
			code += 4;
			max -= 4;
		}

		if (length != 0)
		{
			if (length <= kMaxTinyLength)
			{
				if (max < 2)
				{
					return (0);
				}

				code[0] = (unsigned_int8) ((kCompressedTinyLengthShortDistance << 5) | (length - kMinMatchCount));
				code[1] = (unsigned_int8) distance;
				size += 2;
			}
			else if (length <= kMaxShortLength)
			{
				if (max < 3)
				{
					return (0);
				}

				code[0] = (unsigned_int8) (kCompressedStandardLengthShortDistance << 5);
				code[1] = (unsigned_int8) (length - (kMaxTinyLength + 1));
				code[2] = (unsigned_int8) distance;
				size += 3;
			}
			else
			{
				if (max < 4)
				{
					return (0);
				}

				unsigned_machine x = length - (kMaxShortLength + 1);
				code[0] = (unsigned_int8) ((kCompressedStandardLengthShortDistance << 5) | 0x01);
				code[1] = (unsigned_int8) (x >> 8);
				code[2] = (unsigned_int8) x;
				code[3] = (unsigned_int8) distance;
				size += 4;
			}
		}
	}
	else
	{
		while (length > kMaxLongLength)
		{
			if (max < 5)
			{
				return (0);
			}

			code[0] = (unsigned_int8) ((kCompressedStandardLengthLongDistance << 5) | 0x01);
			code[1] = 0xFF;
			code[2] = 0xFF;
			code[3] = (unsigned_int8) (distance >> 8);
			code[4] = (unsigned_int8) distance;
			size += 5;

			length -= kMaxLongLength;
			code += 5;
			max -= 5;
		}

		if (length != 0)
		{
			if (length <= kMaxTinyLength)
			{
				if (max < 3)
				{
					return (0);
				}

				code[0] = (unsigned_int8) ((kCompressedTinyLengthLongDistance << 5) | (length - kMinMatchCount));
				code[1] = (unsigned_int8) (distance >> 8);
				code[2] = (unsigned_int8) distance;
				size += 3;
			}
			else if (length <= kMaxShortLength)
			{
				if (max < 4)
				{
					return (0);
				}

				code[0] = (unsigned_int8) (kCompressedStandardLengthLongDistance << 5);
				code[1] = (unsigned_int8) (length - (kMaxTinyLength + 1));
				code[2] = (unsigned_int8) (distance >> 8);
				code[3] = (unsigned_int8) distance;
				size += 4;
			}
			else
			{
				if (max < 5)
				{
					return (0);
				}

				unsigned_machine x = length - (kMaxShortLength + 1);
				code[0] = (unsigned_int8) ((kCompressedStandardLengthLongDistance << 5) | 0x01);
				code[1] = (unsigned_int8) (x >> 8);
				code[2] = (unsigned_int8) x;
				code[3] = (unsigned_int8) (distance >> 8);
				code[4] = (unsigned_int8) distance;
				size += 5;
			}
		}
	}

	return (size);
}

unsigned_int32 Comp::CompressData(const void *input, unsigned_int32 dataSize, unsigned_int8 *restrict code)
{
	if (dataSize < 4)
	{
		return (0);
	}

	DictEntry *dictTable = new DictEntry[kWindowSize];
	for (machine a = 0; a < kWindowSize; a++)
	{
		dictTable[a].data = nullptr;
	}

	DictEntry **hashTable = new DictEntry *[kHashTableSize];
	for (machine a = 0; a < kHashTableSize; a++)
	{
		hashTable[a] = nullptr;
	}

	const unsigned_int8 *data = static_cast<const unsigned_int8 *>(input);
	UpdateDictEntry(dictTable, hashTable, data);

	unsigned_machine codeSize = 0;
	unsigned_machine startPosition = 0;
	unsigned_machine compressPosition = 1;

	for (;;)
	{
		if (dataSize - compressPosition <= 4)
		{
			unsigned_machine remain = dataSize - startPosition;
			if (remain != 0)
			{
				unsigned_machine size = WriteUncompressedCode(&data[startPosition], remain, &code[codeSize], dataSize - codeSize);
				if (size != 0)
				{
					codeSize += size;
				}
				else
				{
					codeSize = 0;
				}
			}

			break;
		}

		const unsigned_int8 *compressData = &data[compressPosition];
		const unsigned_int8 *bestMatchData = compressData - 1;
		unsigned_machine maxLength = dataSize - compressPosition;
		unsigned_machine bestMatchLength = CountMatchingBytes(compressData, bestMatchData, maxLength);

		if (bestMatchLength < maxLength)
		{
			unsigned_machine bestControlSize = (bestMatchLength > 35) + (bestMatchLength > 291);

			machine depth = 0;
			const DictEntry *entry = hashTable[GetDataHash(compressData)];
			while (entry)
			{
				unsigned_machine matchLength = CountMatchingBytes(compressData, entry->data, maxLength);
				unsigned_machine controlSize = (matchLength > 35) + (matchLength > 291);
				if (matchLength - controlSize > bestMatchLength - bestControlSize)
				{
					bestMatchData = entry->data;
					bestMatchLength = matchLength;
					bestControlSize = controlSize;

					if (bestMatchLength == maxLength)
					{
						break;
					}
				}

				if (++depth == kMaxMatchDepth)
				{
					break;
				}

				entry = entry->next;
			}
		}

		if (bestMatchLength != 0)
		{
			if (compressPosition != startPosition)
			{
				unsigned_machine size = WriteUncompressedCode(&data[startPosition], compressPosition - startPosition, &code[codeSize], dataSize - codeSize);
				if (size == 0)
				{
					codeSize = 0;
					break;
				}

				codeSize += size;
			}

			unsigned_machine size = WriteCompressedCode(bestMatchLength, (unsigned_machine) (compressData - bestMatchData), &code[codeSize], dataSize - codeSize);
			if (size == 0)
			{
				codeSize = 0;
				break;
			}

			codeSize += size;

			for (unsigned_machine a = 0; a < bestMatchLength; a++)
			{
				UpdateDictEntry(dictTable, hashTable, &compressData[a]);
			}

			compressPosition += bestMatchLength;
			startPosition = compressPosition;
		}
		else
		{
			UpdateDictEntry(dictTable, hashTable, compressData);
			compressPosition++;
		}
	}

	delete[] hashTable;
	delete[] dictTable;

	unsigned_machine totalSize = (codeSize + 3) & ~3;
	for (unsigned_machine a = codeSize; a < totalSize; a++)
	{
		code[a] = kCompressionTerminator << 5;
	}

	return ((unsigned_int32) totalSize);
}

void Comp::DecompressData(const unsigned_int8 *restrict code, unsigned_int32 codeSize, void *output)
{
	unsigned_int8 *data = static_cast<unsigned_int8 *>(output);

	const unsigned_int8 *restrict end = code + codeSize;
	while (code < end)
	{
		unsigned_machine	length;

		unsigned_machine control = code[0];
		unsigned_machine type = control >> 5;

		if (type <= kUncompressedStandardLength)
		{
			if (type == kUncompressedTinyLength)
			{
				length = (control & 0x1F) + 1;
				code++;

				do
				{
					*data++ = *code++;
				} while (--length != 0);
			}
			else
			{
				if ((control & 0x1F) != 0)
				{
					length = ((code[1] << 8) | code[2]) + 289;
					code += 3;
				}
				else
				{
					length = code[1] + 33;
					code += 2;
				}

				memcpy(data, code, length);
				data += length;
				code += length;
			}
		}
		else if (type <= kCompressedStandardLengthLongDistance)
		{
			unsigned_machine	distance;

			if (type <= kCompressedStandardLengthShortDistance)
			{
				if (type == kCompressedTinyLengthShortDistance)
				{
					length = (control & 0x1F) + kMinMatchCount;
					distance = code[1] + 1;
					code += 2;
				}
				else
				{
					if ((control & 0x1F) != 0)
					{
						length = ((code[1] << 8) | code[2]) + (kMaxShortLength + 1);
						distance = code[3] + 1;
						code += 4;
					}
					else
					{
						length = code[1] + (kMaxTinyLength + 1);
						distance = code[2] + 1;
						code += 3;
					}
				}
			}
			else
			{
				if (type == kCompressedTinyLengthLongDistance)
				{
					length = (control & 0x1F) + kMinMatchCount;
					distance = ((code[1] << 8) | code[2]) + 1;
					code += 3;
				}
				else
				{
					if ((control & 0x1F) != 0)
					{
						length = ((code[1] << 8) | code[2]) + (kMaxShortLength + 1);
						distance = ((code[3] << 8) | code[4]) + 1;
						code += 5;
					}
					else
					{
						length = code[1] + (kMaxTinyLength + 1);
						distance = ((code[2] << 8) | code[3]) + 1;
						code += 4;
					}
				}
			}

			const unsigned_int8 *source = data - distance;
			do
			{
				*data++ = *source++;
			} while (--length != 0);
		}
		else
		{
			break;
		}
	}
}

// ZYUQURM
