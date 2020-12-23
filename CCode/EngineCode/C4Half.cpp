 

#include "C4Half.h"


using namespace C4;


float Half::GetFloat(void) const
{
	unsigned_int32 h = value;
	unsigned_int32 s = (h & 0x8000) << 16;
	int32 e = int32((h >> 10) & 0x1F) - 15;

	if (e >= -14)
	{
		unsigned_int32		f;

		if (e <= 15)
		{
			unsigned_int32 m = (h & 0x003FF) << 13;
			f = s | ((e + 112) << 23) | m;
		}
		else
		{
			f = s | 0x7C00;
		}

		return (reinterpret_cast<float&>(f));
	}

	return (reinterpret_cast<float&>(s));
}

void Half::SetFloat(float v)
{
	unsigned_int32 f = reinterpret_cast<unsigned_int32&>(v);
	unsigned_int32 s = (f >> 16) & 0x8000;
	int32 e = int32((f >> 23) & 0xFF) - 127;

	if (e >= -14)
	{
		if (e <= 15)
		{
			unsigned_int32 m = (f >> 13) & 0x03FF;
			value = unsigned_int16(s | ((e + 15) << 10) | m);
		}
		else
		{
			value = unsigned_int16(s | 0x7C00);
		}
	}
	else
	{
		value = unsigned_int16(s);
	}
}

// ZYUQURM
