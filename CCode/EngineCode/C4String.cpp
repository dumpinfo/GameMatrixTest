 

#include "C4String.h"
#include "C4Constants.h"


using namespace C4;


const char Text::hexDigit[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


char String<0>::emptyString[1] = "";


int32 Text::ReadGlyphCodeUTF8(const char *text, unsigned_int32 *code)
{
	int32 c = text[0];
	if (c >= 0)
	{
		*code = c;
		return (1);
	}

	unsigned_int32 byte1 = c & 0xFF;
	if (byte1 - 0xC0 < 0x38)
	{
		unsigned_int32 byte2 = reinterpret_cast<const unsigned_int8 *>(text)[1];

		if (byte1 < 0xE0)
		{
			*code = ((byte1 << 6) & 0x0007C0) | (byte2 & 0x00003F);
			return (2);
		}

		unsigned_int32 byte3 = reinterpret_cast<const unsigned_int8 *>(text)[2];

		if (byte1 < 0xF0)
		{
			*code = ((byte1 << 12) & 0x00F000) | ((byte2 << 6) & 0x000FC0) | (byte3 & 0x00003F);
			return (3);
		}

		unsigned_int32 byte4 = reinterpret_cast<const unsigned_int8 *>(text)[3];
		*code = ((byte1 << 18) & 0x1C0000) | ((byte2 << 12) & 0x03F000) | ((byte3 << 6) & 0x000FC0) | (byte4 & 0x00003F);
		return (4);
	}

	*code = byte1;
	return (1);
}

int32 Text::WriteGlyphCodeUTF8(char *text, unsigned_int32 code)
{
	if (code <= 0x00007F)
	{
		text[0] = (char) code;
		return (1);
	}

	if (code <= 0x0007FF)
	{
		text[0] = (char) (((code >> 6) & 0x1F) | 0xC0);
		text[1] = (char) ((code & 0x3F) | 0x80);
		return (2);
	}

	if (code <= 0x00FFFF)
	{
		text[0] = (char) (((code >> 12) & 0x0F) | 0xE0);
		text[1] = (char) (((code >> 6) & 0x3F) | 0x80);
		text[2] = (char) ((code & 0x3F) | 0x80);
		return (3);
	}

	if (code <= 0x10FFFF)
	{
		text[0] = (char) (((code >> 18) & 0x07) | 0xF0);
		text[1] = (char) (((code >> 12) & 0x3F) | 0x80);
		text[2] = (char) (((code >> 6) & 0x3F) | 0x80);
		text[3] = (char) ((code & 0x3F) | 0x80);
		return (4);
	}

	return (0);
}

int32 Text::ValidateGlyphCodeUTF8(const char *text)
{
	int32 c = text[0];
	if (c >= 0)
	{
		return (1);
	}

	unsigned_int32 byte1 = c & 0xFF;
	if (byte1 - 0xC0 < 0x38)
	{
		unsigned_int32 byte2 = reinterpret_cast<const unsigned_int8 *>(text)[1];
		if ((byte2 & 0xC0) != 0x80)
		{
			return (0);
		} 

		if (byte1 < 0xE0)
		{ 
			return (2);
		} 

		unsigned_int32 byte3 = reinterpret_cast<const unsigned_int8 *>(text)[2];
		if ((byte3 & 0xC0) != 0x80) 
		{
			return (0); 
		} 

		if (byte1 < 0xF0)
		{
			return (3); 
		}

		unsigned_int32 byte4 = reinterpret_cast<const unsigned_int8 *>(text)[2];
		if ((byte4 & 0xC0) != 0x80)
		{
			return (0);
		}

		return (4);
	}

	return (0);
}

int32 Text::GetGlyphCodeByteCountUTF8(unsigned_int32 code)
{
	if (code <= 0x00007F)
	{
		return (1);
	}

	if (code <= 0x0007FF)
	{
		return (2);
	}

	if (code <= 0x00FFFF)
	{
		return (3);
	}

	if (code <= 0x10FFFF)
	{
		return (4);
	}

	return (1);
}

int32 Text::GetGlyphCountUTF8(const char *text)
{
	int32 count = 0;
	for (;; count++)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == 0)
		{
			break;
		}

		if ((c < 0xC0) || (c >= 0xF8))
		{
			text++;
		}
		else if (c < 0xE0)
		{
			text += 2;
		}
		else if (c < 0xF0)
		{
			text += 3;
		}
		else
		{
			text += 4;
		}
	}

	return (count);
}

int32 Text::GetGlyphCountUTF8(const char *text, int32 max)
{
	int32 count = 0;
	const char *end = text + max;
	for (; text < end; count++)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == 0)
		{
			break;
		}

		if ((c < 0xC0) || (c >= 0xF8))
		{
			text++;
		}
		else if (c < 0xE0)
		{
			text += 2;
		}
		else if (c < 0xF0)
		{
			text += 3;
		}
		else
		{
			text += 4;
		}
	}

	return (count);
}

int32 Text::GetPreviousGlyphByteCountUTF8(const char *text, int32 max)
{
	int32 count = 0;
	while (--max >= 0)
	{
		count--;
		unsigned_int32 c = reinterpret_cast<const unsigned_int8 *>(text)[count];
		if (c - 0x80 >= 0x40U)
		{
			break;
		}
	}

	return (-count);
}

int32 Text::GetNextGlyphByteCountUTF8(const char *text, int32 max)
{
	char c = text[0];
	if (c < 0)
	{
		unsigned_int32 byte = c & 0xFF;
		if (byte >= 0xC0)
		{
			if (byte < 0xE0)
			{
				return (Min(max, 2));
			}

			if (byte < 0xF0)
			{
				return (Min(max, 3));
			}

			if (byte < 0xF8)
			{
				return (Min(max, 4));
			}
		}
	}

	return (Min(max, 1));
}

int32 Text::GetGlyphStringByteCountUTF8(const char *text, int32 glyphCount)
{
	int32 count = 0;
	for (machine a = 0; a < glyphCount; a++)
	{
		char c = text[count];
		if (c == 0)
		{
			break;
		}

		int32 size = 1;
		if (c < 0)
		{
			unsigned_int32 byte = c & 0xFF;
			if (byte >= 0xC0)
			{
				if (byte < 0xE0)
				{
					size += 1;
				}
				else if (byte < 0xF0)
				{
					size += 2;
				}
				else if (byte < 0xF8)
				{
					size += 3;
				}
			}
		}

		count += size;
	}

	return (count);
}

int32 Text::GetTextLength(const char *text)
{
	const char *start = text;
	while (*text != 0)
	{
		text++;
	}

	return ((int32) (text - start));
}

unsigned_int32 Text::Hash(const char *text)
{
	unsigned_int32 hash = 0;
	for (;;)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == 0)
		{
			break;
		}

		hash ^= c;
		hash = hash * 0x6B84DF47 + 1;

		text++;
	}

	return (hash);
}

int32 Text::FindChar(const char *text, unsigned_int32 k)
{
	const char *start = text;
	for (;;)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == 0)
		{
			break;
		}

		if (c == k)
		{
			return ((int32) (text - start));
		}

		text++;
	}

	return (-1);
}

int32 Text::FindChar(const char *text, unsigned_int32 k, int32 max)
{
	const char *start = text;
	while (--max >= 0)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == 0)
		{
			break;
		}

		if (c == k)
		{
			return ((int32) (text - start));
		}

		text++;
	}

	return (-1);
}

int32 Text::FindUnquotedChar(const char *text, unsigned_int32 k)
{
	bool quote = false;
	bool backslash = false;

	const char *start = text;
	for (;;)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == 0)
		{
			break;
		}

		if (c == 34)
		{
			if (!quote)
			{
				quote = true;
			}
			else if (!backslash)
			{
				quote = false;
			}
		}

		if ((c == k) && (!quote))
		{
			return ((int32) (text - start));
		}

		backslash = ((c == 92) && (!backslash));
		text++;
	}

	return (-1);
}

int32 Text::CountChars(const char *text, unsigned_int32 k, int32 max)
{
	int32 count = 0;
	while (max > 0)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == 0)
		{
			break;
		}

		count += (c == k);
		text++;
		max--;
	}

	return (count);
}

int32 Text::CopyText(const char *source, char *dest)
{
	const char *c = source;
	for (;;)
	{
		unsigned_int32 k = *reinterpret_cast<const unsigned_int8 *>(c);
		*dest++ = (char) k;
		if (k == 0)
		{
			break;
		}

		c++;
	}

	return ((int32) (c - source));
}

int32 Text::CopyText(const char *source, char *dest, int32 max)
{
	const char *c = source;
	while (--max >= 0)
	{
		unsigned_int32 k = *reinterpret_cast<const unsigned_int8 *>(c);
		if (k == 0)
		{
			break;
		}

		*dest++ = (char) k;
		c++;
	}

	dest[0] = 0;
	return ((int32) (c - source));
}

int32 Text::CopyText(const wchar_t *source, char *dest, int32 max)
{
	char *d = dest;
	for (;;)
	{
		unsigned_int32 k = source[0];
		if (k == 0)
		{
			break;
		}

		unsigned_int32 p1 = k - 0xD800;
		if (p1 < 0x0400U)
		{
			unsigned_int32 p2 = source[1] - 0xDC00;
			if (p2 < 0x0400U)
			{
				k = ((p1 << 10) | p2) + 0x010000;
				source++;
			}
		}

		int32 n = GetGlyphCodeByteCountUTF8(k);
		if (n > max)
		{
			break;
		}

		d += WriteGlyphCodeUTF8(d, k);
		max -= n;
		source++;
	}

	d[0] = 0;
	return ((int32) (d - dest));
}

bool Text::CompareText(const char *s1, const char *s2)
{
	for (machine a = 0;; a++)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x != y)
		{
			return (false);
		}

		if (x == 0)
		{
			break;
		}
	}

	return (true);
}

bool Text::CompareText(const char *s1, const char *s2, int32 max)
{
	for (machine a = 0;; a++)
	{
		if (--max < 0)
		{
			break;
		}

		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x != y)
		{
			return (false);
		}

		if (x == 0)
		{
			break;
		}
	}

	return (true);
}

bool Text::CompareTextCaseless(const char *s1, const char *s2)
{
	for (machine a = 0;; a++)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x - 'A' < 26U)
		{
			x += 32;
		}

		if (y - 'A' < 26U)
		{
			y += 32;
		}

		if (x != y)
		{
			return (false);
		}

		if (x == 0)
		{
			break;
		}
	}

	return (true);
}

bool Text::CompareTextCaseless(const char *s1, const char *s2, int32 max)
{
	for (machine a = 0;; a++)
	{
		if (--max < 0)
		{
			break;
		}

		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x - 65 < 26U)
		{
			x += 32;
		}

		if (y - 65 < 26U)
		{
			y += 32;
		}

		if (x != y)
		{
			return (false);
		}

		if (x == 0)
		{
			break;
		}
	}

	return (true);
}

bool Text::CompareTextLessThan(const char *s1, const char *s2)
{
	for (machine a = 0;; a++)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if ((x != y) || (x == 0))
		{
			return (x < y);
		}
	}
}

bool Text::CompareTextLessThan(const char *s1, const char *s2, int32 max)
{
	for (machine a = 0;; a++)
	{
		if (--max < 0)
		{
			break;
		}

		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if ((x != y) || (x == 0))
		{
			return (x < y);
		}
	}

	return (false);
}

bool Text::CompareTextLessThanCaseless(const char *s1, const char *s2)
{
	for (machine a = 0;; a++)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x - 'a' < 26U)
		{
			x -= 32;
		}

		if (y - 'a' < 26U)
		{
			y -= 32;
		}

		if ((x != y) || (x == 0))
		{
			return (x < y);
		}
	}
}

bool Text::CompareTextLessThanCaseless(const char *s1, const char *s2, int32 max)
{
	for (machine a = 0;; a++)
	{
		if (--max < 0)
		{
			break;
		}

		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x - 'a' < 26U)
		{
			x -= 32;
		}

		if (y - 'a' < 26U)
		{
			y -= 32;
		}

		if ((x != y) || (x == 0))
		{
			return (x < y);
		}
	}

	return (false);
}

bool Text::CompareTextLessEqual(const char *s1, const char *s2)
{
	for (machine a = 0;; a++)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if ((x != y) || (x == 0))
		{
			return (x <= y);
		}
	}
}

bool Text::CompareTextLessEqual(const char *s1, const char *s2, int32 max)
{
	for (machine a = 0;; a++)
	{
		if (--max < 0)
		{
			break;
		}

		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if ((x != y) || (x == 0))
		{
			return (x <= y);
		}
	}

	return (true);
}

bool Text::CompareTextLessEqualCaseless(const char *s1, const char *s2)
{
	for (machine a = 0;; a++)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x - 'a' < 26U)
		{
			x -= 32;
		}

		if (y - 'a' < 26U)
		{
			y -= 32;
		}

		if ((x != y) || (x == 0))
		{
			return (x <= y);
		}
	}
}

bool Text::CompareTextLessEqualCaseless(const char *s1, const char *s2, int32 max)
{
	for (machine a = 0;; a++)
	{
		if (--max < 0)
		{
			break;
		}

		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1 + a);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2 + a);

		if (x - 'a' < 26U)
		{
			x -= 32;
		}

		if (y - 'a' < 26U)
		{
			y -= 32;
		}

		if ((x != y) || (x == 0))
		{
			return (x <= y);
		}
	}

	return (true);
}

bool Text::CompareNumberedTextLessThan(const char *s1, const char *s2)
{
	for (;;)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1++);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2++);

		unsigned_int32 xnum = x - '0';
		unsigned_int32 ynum = y - '0';

		if ((xnum < 10U) && (ynum < 10U))
		{
			int32 xcount = 1;
			int32 ycount = 1;

			for (;;)
			{
				x = *reinterpret_cast<const unsigned_int8 *>(s1) - '0';
				if (x >= 10U)
				{
					break;
				}

				xnum = xnum * 10 + x;
				xcount++;
				s1++;
			}

			for (;;)
			{
				y = *reinterpret_cast<const unsigned_int8 *>(s2) - '0';
				if (y >= 10U)
				{
					break;
				}

				ynum = ynum * 10 + y;
				ycount++;
				s2++;
			}

			if (xnum != ynum)
			{
				return (xnum < ynum);
			}
			else if (xcount < ycount)
			{
				return (true);
			}
		}
		else
		{
			if ((x != y) || (x == 0))
			{
				return (x < y);
			}
		}
	}
}

bool Text::CompareNumberedTextLessThanCaseless(const char *s1, const char *s2)
{
	for (;;)
	{
		unsigned_int32 x = *reinterpret_cast<const unsigned_int8 *>(s1++);
		unsigned_int32 y = *reinterpret_cast<const unsigned_int8 *>(s2++);

		unsigned_int32 xnum = x - '0';
		unsigned_int32 ynum = y - '0';

		if ((xnum < 10U) && (ynum < 10U))
		{
			int32 xcount = 1;
			int32 ycount = 1;

			for (;;)
			{
				x = *reinterpret_cast<const unsigned_int8 *>(s1) - '0';
				if (x >= 10U)
				{
					break;
				}

				xnum = xnum * 10 + x;
				xcount++;
				s1++;
			}

			for (;;)
			{
				y = *reinterpret_cast<const unsigned_int8 *>(s2) - '0';
				if (y >= 10U)
				{
					break;
				}

				ynum = ynum * 10 + y;
				ycount++;
				s2++;
			}

			if (xnum != ynum)
			{
				return (xnum < ynum);
			}
			else if (xcount < ycount)
			{
				return (true);
			}
		}
		else
		{
			if (x - 'a' < 26U)
			{
				x -= 32;
			}

			if (y - 'a' < 26U)
			{
				y -= 32;
			}

			if ((x != y) || (x == 0))
			{
				return (x < y);
			}
		}
	}
}

int32 Text::FindText(const char *s1, const char *s2)
{
	const char *start = s1;
	int32 first = *reinterpret_cast<const unsigned_int8 *>(s2);

	for (;;)
	{
		int32 c = *reinterpret_cast<const unsigned_int8 *>(s1++);
		if (c == 0)
		{
			break;
		}

		if (c == first)
		{
			const unsigned_int8 *s3 = reinterpret_cast<const unsigned_int8 *>(s1);
			const unsigned_int8 *s4 = reinterpret_cast<const unsigned_int8 *>(s2);

			for (;;)
			{
				int32 x = *++s4;
				if (x == 0)
				{
					return ((int32) (s1 - start - 1));
				}

				int32 y = *s3++;
				if (y == 0)
				{
					return (-1);
				}

				if (x != y)
				{
					break;
				}
			}
		}
	}

	return (-1);
}

int32 Text::FindTextCaseless(const char *s1, const char *s2)
{
	const char *start = s1;
	int32 first = *reinterpret_cast<const unsigned_int8 *>(s2);

	for (;;)
	{
		int32 c = *reinterpret_cast<const unsigned_int8 *>(s1++);
		if (c == 0)
		{
			break;
		}

		if (c == first)
		{
			const unsigned_int8 *s3 = reinterpret_cast<const unsigned_int8 *>(s1);
			const unsigned_int8 *s4 = reinterpret_cast<const unsigned_int8 *>(s2);

			for (;;)
			{
				int32 x = *++s4;
				if (x == 0)
				{
					return ((int32) (s1 - start - 1));
				}

				int32 y = *s3++;
				if (y == 0)
				{
					return (-1);
				}

				if (x - 'a' < 26U)
				{
					x -= 32;
				}

				if (y - 'a' < 26U)
				{
					y -= 32;
				}

				if (x != y)
				{
					break;
				}
			}
		}
	}

	return (-1);
}

int32 Text::IntegerToString(int32 num, char *text, int32 max)
{
	char	c[16];

	bool negative = (num < 0);
	num = Abs(num) & 0x7FFFFFFF;

	machine length = 0;
	do
	{
		int32 p = num % 10;
		c[length++] = (char) (p + 48);
		num /= 10;
	} while (num != 0);

	machine a = -1;
	if (negative)
	{
		if (++a < max)
		{
			text[a] = '-';
		}
		else
		{
			text[a] = 0;
			return (a);
		}
	}

	do
	{
		if (++a < max)
		{
			text[a] = c[--length];
		}
		else
		{
			text[a] = 0;
			return (a);
		}
	} while (length != 0);

	text[++a] = 0;
	return (a);
}

int32 Text::Integer64ToString(int64 num, char *text, int32 max)
{
	char	c[32];

	bool negative = (num < 0);
	num = Abs64(num) & 0x7FFFFFFFFFFFFFFFULL;

	machine length = 0;
	do
	{
		int32 p = num % 10;
		c[length++] = (char) (p + 48);
		num /= 10;
	} while (num != 0);

	machine a = -1;
	if (negative)
	{
		if (++a < max)
		{
			text[a] = '-';
		}
		else
		{
			text[a] = 0;
			return (a);
		}
	}

	do
	{
		if (++a < max)
		{
			text[a] = c[--length];
		}
		else
		{
			text[a] = 0;
			return (a);
		}
	} while (length != 0);

	text[++a] = 0;
	return (a);
}

int32 Text::StringToInteger(const char *text)
{
	int32 value = 0;
	bool negative = false;

	for (;;)
	{
		unsigned_int32 x = *text++;
		if (x == 0)
		{
			break;
		}

		if (x == '-')
		{
			negative = true;
		}
		else
		{
			x -= '0';
			if (x < 10)
			{
				value = value * 10 + x;
			}
		}
	}

	return ((negative) ? -value : value);
}

int64 Text::StringToInteger64(const char *text)
{
	int64 value = 0;
	bool negative = false;

	for (;;)
	{
		unsigned_int32 x = *text++;
		if (x == 0)
		{
			break;
		}

		if (x == '-')
		{
			negative = true;
		}
		else
		{
			x -= '0';
			if (x < 10)
			{
				value = value * 10 + x;
			}
		}
	}

	return ((negative) ? -value : value);
}

int32 Text::FloatToString(float num, char *text, int32 max)
{
	if (max < 1)
	{
		text[0] = 0;
		return (0);
	}

	int32 binary = *reinterpret_cast<int32 *>(&num);
	int32 exponent = (binary >> 23) & 0xFF;

	if (exponent == 0)
	{
		if (max >= 3)
		{
			text[0] = '0';
			text[1] = '.';
			text[2] = '0';
			text[3] = 0;
			return (3);
		}

		text[0] = '0';
		text[1] = 0;
		return (1);
	}

	int32 mantissa = binary & 0x007FFFFF;

	if (exponent == 0xFF)
	{
		if (max >= 4)
		{
			bool b = (binary < 0);
			if (b)
			{
				*text++ = '-';
			}

			if (mantissa == 0)
			{
				text[0] = 'I';
				text[1] = 'N';
				text[2] = 'F';
				text[3] = 0;
			}
			else
			{
				text[0] = 'N';
				text[1] = 'A';
				text[2] = 'N';
				text[3] = 0;
			}

			return (3 + b);
		}

		text[0] = 0;
		return (0);
	}

	int32 power = 0;
	float absolute = Fabs(num);
	if ((absolute < 1.0e-4F) || (!(absolute < 1.0e5F)))
	{
		float f = Floor(Log10(absolute));
		absolute /= Exp(f * K::ln_10);
		power = (int32) f;

		binary = *reinterpret_cast<int32 *>(&absolute);
		exponent = (binary >> 23) & 0xFF;
		mantissa = binary & 0x007FFFFF;
	}

	exponent -= 0x7F;
	mantissa |= 0x00800000;

	machine len = 0;
	if (num < 0.0F)
	{
		text[0] = '-';
		len = 1;
	}

	if (exponent >= 0)
	{
		int32 whole = mantissa >> (23 - exponent);
		mantissa = (mantissa << exponent) & 0x007FFFFF;

		len += IntegerToString(whole, &text[len], max - len);
		if (len < max)
		{
			text[len++] = '.';
		}

		if (len == max)
		{
			goto end;
		}
	}
	else
	{
		if (len + 2 <= max)
		{
			text[len++] = '0';
			text[len++] = '.';
			if (len == max)
			{
				goto end;
			}
		}
		else
		{
			if (len < max)
			{
				text[len++] = '0';
			}

			goto end;
		}

		mantissa >>= -exponent;
	}

	for (machine a = 0, zeroCount = 0, nineCount = 0; (a < 7) && (len < max); a++)
	{
		mantissa *= 10;
		int32 n = (mantissa >> 23) + 48;
		text[len++] = (char) n;

		if (n == '0')
		{
			if ((++zeroCount >= 4) && (a >= 4))
			{
				break;
			}
		}
		else if (n == '9')
		{
			if ((++nineCount >= 4) && (a >= 4))
			{
				break;
			}
		}

		mantissa &= 0x007FFFFF;
		if (mantissa < 2)
		{
			break;
		}
	}

	if ((text[len - 1] == '9') && (text[len - 2] == '9'))
	{
		for (machine a = len - 3;; a--)
		{
			char c = text[a];
			if (c != '9')
			{
				if (c != '.')
				{
					text[a] = c + 1;
					len = a + 1;
				}

				break;
			}
		}
	}
	else
	{
		while (text[len - 1] == '0')
		{
			len--;
		}

		if (text[len - 1] == '.')
		{
			text[len++] = '0';
		}
	}

	if ((power != 0) && (len < max))
	{
		text[len++] = 'e';
		return (IntegerToString(power, &text[len], max - len));
	}

	end:
	text[len] = 0;
	return (len);
}

float Text::StringToFloat(const char *text)
{
	float value = 0.0F;
	float expon = 0.0F;
	float decplace = 0.1F;

	bool negative = false;
	bool exponent = false;
	bool exponNeg = false;
	bool decimal = false;

	for (;;)
	{
		unsigned_int32 x = *text++;
		if (x == 0)
		{
			break;
		}

		if (x == '-')
		{
			if (exponent)
			{
				exponNeg = true;
			}
			else
			{
				negative = true;
			}
		}
		else if (x == '.')
		{
			decimal = true;
		}
		else if ((x == 'e') || (x == 'E'))
		{
			exponent = true;
		}
		else
		{
			x -= '0';
			if (x < 10)
			{
				if (exponent)
				{
					expon = expon * 10.0F + x;
				}
				else
				{
					if (decimal)
					{
						value += x * decplace;
						decplace *= 0.1F;
					}
					else
					{
						value = value * 10.0F + x;
					}
				}
			}
		}
	}

	if (exponent)
	{
		if (exponNeg)
		{
			expon = -expon;
		}

		value *= Exp(expon * K::ln_10);
	}

	return ((negative) ? -value : value);
}

String<31> Text::Integer64ToHexString16(unsigned_int64 num)
{
	String<31>	text;

	text[0] = hexDigit[(num >> 60) & 15];
	text[1] = hexDigit[(num >> 56) & 15];
	text[2] = hexDigit[(num >> 52) & 15];
	text[3] = hexDigit[(num >> 48) & 15];
	text[4] = hexDigit[(num >> 44) & 15];
	text[5] = hexDigit[(num >> 40) & 15];
	text[6] = hexDigit[(num >> 36) & 15];
	text[7] = hexDigit[(num >> 32) & 15];
	text[8] = hexDigit[(num >> 28) & 15];
	text[9] = hexDigit[(num >> 24) & 15];
	text[10] = hexDigit[(num >> 20) & 15];
	text[11] = hexDigit[(num >> 16) & 15];
	text[12] = hexDigit[(num >> 12) & 15];
	text[13] = hexDigit[(num >> 8) & 15];
	text[14] = hexDigit[(num >> 4) & 15];
	text[15] = hexDigit[num & 15];
	text[16] = 0;

	return (text);
}

String<15> Text::IntegerToHexString8(unsigned_int32 num)
{
	String<15>	text;

	text[0] = hexDigit[(num >> 28) & 15];
	text[1] = hexDigit[(num >> 24) & 15];
	text[2] = hexDigit[(num >> 20) & 15];
	text[3] = hexDigit[(num >> 16) & 15];
	text[4] = hexDigit[(num >> 12) & 15];
	text[5] = hexDigit[(num >> 8) & 15];
	text[6] = hexDigit[(num >> 4) & 15];
	text[7] = hexDigit[num & 15];
	text[8] = 0;

	return (text);
}

String<7> Text::IntegerToHexString4(unsigned_int32 num)
{
	String<7>	text;

	text[0] = hexDigit[(num >> 12) & 15];
	text[1] = hexDigit[(num >> 8) & 15];
	text[2] = hexDigit[(num >> 4) & 15];
	text[3] = hexDigit[num & 15];
	text[4] = 0;

	return (text);
}

String<3> Text::IntegerToHexString2(unsigned_int32 num)
{
	String<3>	text;

	text[0] = hexDigit[(num >> 4) & 15];
	text[1] = hexDigit[num & 15];
	text[2] = 0;

	return (text);
}

unsigned_int32 Text::StringToType(const char *string)
{
	unsigned_int32 type = 0;

	unsigned_int32 c = reinterpret_cast<const unsigned_int8 *>(string)[0];
	if (c != 0)
	{
		type = c << 24;

		c = reinterpret_cast<const unsigned_int8 *>(string)[1];
		if (c != 0)
		{
			type |= c << 16;

			c = reinterpret_cast<const unsigned_int8 *>(string)[2];
			if (c != 0)
			{
				type |= c << 8;

				c = reinterpret_cast<const unsigned_int8 *>(string)[3];
				if (c != 0)
				{
					type |= c;
				}
			}
		}
	}

	return (type);
}

String<4> Text::TypeToString(unsigned_int32 type)
{
	unsigned_int32 c = type >> 24;
	if (c != 0)
	{
		return (String<4>((char) c, (char) (type >> 16), (char) (type >> 8), (char) type));
	}

	return (String<4>((char) (type >> 16), (char) (type >> 8), (char) type, 0));
}

String<31> Text::TypeToHexCharString(unsigned_int32 type)
{
	String<31> string("0x");
	string += Text::IntegerToHexString8(type);
	string += " '";
	string += Text::TypeToString(type);
	string += '\'';
	return (string);
}

int32 Text::GetResourceNameLength(const char *text)
{
	int32 len = GetTextLength(text);
	for (machine a = len - 1; a >= 0; a--)
	{
		unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(&text[a]);
		if (c == '.')
		{
			return (a);
		}

		if (c == '/')
		{
			break;
		}
	}

	return (len);
}

int32 Text::GetDirectoryPathLength(const char *text)
{
	int32 len = 0;
	for (;;)
	{
		int32 x = FindChar(&text[len], '/');
		if (x == -1)
		{
			break;
		}

		len += x + 1;
	}

	return (len);
}

int32 Text::ReadInteger(const char *text, char *number, int32 max)
{
	const char *start = text;

	unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
	if (c == '-')
	{
		if (--max >= 0)
		{
			*number++ = (char) c;
			text++;
		}
	}

	while (--max >= 0)
	{
		c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c - '0' >= 10U)
		{
			break;
		}

		*number++ = (char) c;
		text++;
	}

	*number = 0;
	return ((int32) (text - start));
}

int32 Text::ReadFloat(const char *text, char *number, int32 max)
{
	const char *start = text;

	unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
	if (c == '-')
	{
		if (--max >= 0)
		{
			*number++ = (char) c;
			text++;
		}
	}

	bool decimal = false;
	bool exponent = false;
	bool expneg = true;

	while (--max >= 0)
	{
		c = *reinterpret_cast<const unsigned_int8 *>(text);
		if (c == '.')
		{
			if (decimal)
			{
				break;
			}

			decimal = true;
		}
		else if ((c == 'e') || (c == 'E'))
		{
			if (exponent)
			{
				break;
			}

			exponent = true;
			expneg = false;
		}
		else
		{
			if ((c == '-') && (expneg))
			{
				break;
			}
			else if (c - '0' >= 10U)
			{
				break;
			}

			expneg = true;
		}

		*number++ = (char) c;
		text++;
	}

	*number = 0;
	return ((int32) (text - start));
}

int32 Text::ReadString(const char *text, char *string, int32 max)
{
	const char *start = text;

	if (*text == 34)
	{
		text++;
		bool backslash = false;

		while (--max >= 0)
		{
			unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
			if (c == 0)
			{
				break;
			}

			text++;

			if ((c != 92) || (backslash))
			{
				if ((c == 34) && (!backslash))
				{
					break;
				}

				*string++ = (char) c;
				backslash = false;
			}
			else
			{
				backslash = true;
			}
		}
	}
	else
	{
		while (--max >= 0)
		{
			unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
			if ((c == 0) || (c < 33) || ((c == '/') && (text[1] == '/')))
			{
				break;
			}

			*string++ = (char) c;
			text++;
		}
	}

	*string = 0;
	return ((int32) (text - start));
}

int32 Text::ReadType(const char *text, unsigned_int32 *type)
{
	if (*text == '\'')
	{
		const char *start = text;
		unsigned_int32 value = 0;

		text++;
		bool backslash = false;

		for (;;)
		{
			unsigned_int32 c = *reinterpret_cast<const unsigned_int8 *>(text);
			if (c == 0)
			{
				break;
			}

			text++;

			if ((c != 92) || (backslash))
			{
				if ((c == '\'') && (!backslash))
				{
					break;
				}

				value = (value << 8) | c;
				backslash = false;
			}
			else
			{
				backslash = true;
			}
		}

		*type = value;
		return ((int32) (text - start));
	}

	*type = 0;
	return (0);
}


String<0>::String()
{
	logicalSize = 1;
	physicalSize = 0;
	stringPointer = emptyString;
}

String<0>::~String()
{
	if (stringPointer != emptyString)
	{
		delete[] stringPointer;
	}
}

String<0>::String(const String& s)
{
	int32 size = s.logicalSize;
	logicalSize = size;
	if (size > 1)
	{
		physicalSize = GetPhysicalSize(size);
		stringPointer = new char[physicalSize];
		Text::CopyText(s, stringPointer);
	}
	else
	{
		physicalSize = 0;
		stringPointer = emptyString;
	}
}

String<0>::String(const char *s)
{
	int32 size = Text::GetTextLength(s) + 1;
	logicalSize = size;
	if (size > 1)
	{
		physicalSize = GetPhysicalSize(size);
		stringPointer = new char[physicalSize];
		Text::CopyText(s, stringPointer);
	}
	else
	{
		physicalSize = 0;
		stringPointer = emptyString;
	}
}

String<0>::String(const char *s, int32 length)
{
	length = Min(length, Text::GetTextLength(s));

	int32 size = length + 1;
	logicalSize = size;
	if (size > 1)
	{
		physicalSize = GetPhysicalSize(size);
		stringPointer = new char[physicalSize];
		Text::CopyText(s, stringPointer, length);
	}
	else
	{
		physicalSize = 0;
		stringPointer = emptyString;
	}
}

String<0>::String(const wchar_t *s)
{
	const wchar_t *t = s;
	while (*t != 0)
	{
		t++;
	}

	unsigned_int32 length = (unsigned_int32) (t - s);
	if (length > 0)
	{
		char *buffer = new char[length * 4 + 1];
		char *c = buffer;

		for (unsigned_machine a = 0; a < length; a++)
		{
			c += Text::WriteGlyphCodeUTF8(c, s[a]);
		}

		c[0] = 0;

		length = (unsigned_int32) (c - buffer);
		int32 size = length + 1;
		logicalSize = size;

		physicalSize = GetPhysicalSize(size);
		stringPointer = new char[physicalSize];
		Text::CopyText(buffer, stringPointer, length);

		delete[] buffer;
	}
	else
	{
		logicalSize = 1;
		physicalSize = 0;
		stringPointer = emptyString;
	}
}

String<0>::String(int32 n)
{
	physicalSize = kStringAllocSize + 1;
	stringPointer = new char[kStringAllocSize + 1];
	logicalSize = Text::IntegerToString(n, stringPointer, kStringAllocSize) + 1;
}

String<0>::String(unsigned_int32 n)
{
	physicalSize = kStringAllocSize + 1;
	stringPointer = new char[kStringAllocSize + 1];
	logicalSize = Text::IntegerToString(n, stringPointer, kStringAllocSize) + 1;
}

String<0>::String(int64 n)
{
	physicalSize = kStringAllocSize + 1;
	stringPointer = new char[kStringAllocSize + 1];
	logicalSize = Text::Integer64ToString(n, stringPointer, kStringAllocSize) + 1;
}

String<0>::String(float n)
{
	physicalSize = kStringAllocSize + 1;
	stringPointer = new char[kStringAllocSize + 1];
	logicalSize = Text::FloatToString(n, stringPointer, kStringAllocSize) + 1;
}

String<0>::String(const char *s1, const char *s2)
{
	int32 len1 = Text::GetTextLength(s1);
	int32 len2 = Text::GetTextLength(s2);

	int32 size = len1 + len2 + 1;
	logicalSize = size;
	if (size > 1)
	{
		physicalSize = GetPhysicalSize(size);
		stringPointer = new char[physicalSize];
		Text::CopyText(s1, stringPointer);
		Text::CopyText(s2, stringPointer + len1);
	}
	else
	{
		physicalSize = 0;
		stringPointer = emptyString;
	}
}

String<0>::String(int32 n, const char *s1)
{
	int32 len1 = Text::GetTextLength(s1);

	int32 size = len1 + kStringAllocSize + 1;
	physicalSize = GetPhysicalSize(size);
	stringPointer = new char[physicalSize];
	Text::CopyText(s1, stringPointer);
	logicalSize = len1 + Text::IntegerToString(n, stringPointer + len1, kStringAllocSize) + 1;
}

String<0>::String(unsigned_int32 n, const char *s1)
{
	int32 len1 = Text::GetTextLength(s1);

	int32 size = len1 + kStringAllocSize + 1;
	physicalSize = GetPhysicalSize(size);
	stringPointer = new char[physicalSize];
	Text::CopyText(s1, stringPointer);
	logicalSize = len1 + Text::IntegerToString(n, stringPointer + len1, kStringAllocSize) + 1;
}

String<0>::String(int64 n, const char *s1)
{
	int32 len1 = Text::GetTextLength(s1);

	int32 size = len1 + kStringAllocSize + 1;
	physicalSize = GetPhysicalSize(size);
	stringPointer = new char[physicalSize];
	Text::CopyText(s1, stringPointer);
	logicalSize = len1 + Text::Integer64ToString(n, stringPointer + len1, kStringAllocSize) + 1;
}

void String<0>::Purge(void)
{
	if (stringPointer != emptyString)
	{
		delete[] stringPointer;
		stringPointer = emptyString;

		logicalSize = 1;
		physicalSize = 0;
	}
}

void String<0>::Resize(int32 size)
{
	logicalSize = size;
	if ((size > physicalSize) || (size < physicalSize / 2))
	{
		if (stringPointer != emptyString)
		{
			delete[] stringPointer;
		}

		physicalSize = GetPhysicalSize(size);
		stringPointer = new char[physicalSize];
	}
}

String<0>& String<0>::Set(const char *s, int32 length)
{
	length = Min(length, Text::GetTextLength(s));

	int32 size = length + 1;
	if (size > 1)
	{
		Resize(size);
		Text::CopyText(s, stringPointer, length);
	}
	else
	{
		Purge();
	}

	return (*this);
}

String<0>& String<0>::operator =(String&& s)
{
	if (stringPointer != emptyString)
	{
		delete[] stringPointer;
	}

	logicalSize = s.logicalSize;
	physicalSize = s.physicalSize;
	stringPointer = s.stringPointer;

	s.stringPointer = emptyString;
	return (*this);
}

String<0>& String<0>::operator =(const String& s)
{
	int32 size = s.logicalSize;
	if (size > 1)
	{
		Resize(size);
		Text::CopyText(s, stringPointer);
	}
	else
	{
		Purge();
	}

	return (*this);
}

String<0>& String<0>::operator =(const char *s)
{
	int32 size = Text::GetTextLength(s) + 1;
	if (size > 1)
	{
		Resize(size);
		Text::CopyText(s, stringPointer);
	}
	else
	{
		Purge();
	}

	return (*this);
}

String<0>& String<0>::operator =(int32 n)
{
	Resize(kStringAllocSize);
	logicalSize = Text::IntegerToString(n, stringPointer, kStringAllocSize - 1) + 1;
	return (*this);
}

String<0>& String<0>::operator =(unsigned_int32 n)
{
	Resize(kStringAllocSize);
	logicalSize = Text::IntegerToString(n, stringPointer, kStringAllocSize - 1) + 1;
	return (*this);
}

String<0>& String<0>::operator =(int64 n)
{
	Resize(kStringAllocSize);
	logicalSize = Text::Integer64ToString(n, stringPointer, kStringAllocSize - 1) + 1;
	return (*this);
}

String<0>& String<0>::operator =(float n)
{
	Resize(kStringAllocSize);
	logicalSize = Text::FloatToString(n, stringPointer, kStringAllocSize - 1) + 1;
	return (*this);
}

String<0>& String<0>::operator +=(const String<>& s)
{
	int32 length = s.Length();
	if (length > 0)
	{
		int32 size = logicalSize + length;
		if (size > 1)
		{
			if (size > physicalSize)
			{
				physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
				char *newPointer = new char[physicalSize];

				if (stringPointer != emptyString)
				{
					Text::CopyText(stringPointer, newPointer);
					delete[] stringPointer;
				}

				stringPointer = newPointer;
			}

			Text::CopyText(s, stringPointer + logicalSize - 1);
			logicalSize = size;
		}
	}

	return (*this);
}

String<0>& String<0>::operator +=(const char *s)
{
	int32 length = Text::GetTextLength(s);
	if (length > 0)
	{
		int32 size = logicalSize + length;
		if (size > 1)
		{
			if (size > physicalSize)
			{
				physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
				char *newPointer = new char[physicalSize];

				if (stringPointer != emptyString)
				{
					Text::CopyText(stringPointer, newPointer);
					delete[] stringPointer;
				}

				stringPointer = newPointer;
			}

			Text::CopyText(s, stringPointer + logicalSize - 1);
			logicalSize = size;
		}
	}

	return (*this);
}

String<0>& String<0>::operator +=(char k)
{
	int32 size = logicalSize + 1;
	if (size > physicalSize)
	{
		physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
		char *newPointer = new char[physicalSize];

		if (stringPointer != emptyString)
		{
			Text::CopyText(stringPointer, newPointer);
			delete[] stringPointer;
		}

		stringPointer = newPointer;
	}

	stringPointer[logicalSize - 1] = k;
	stringPointer[logicalSize] = 0;
	logicalSize = size;
	return (*this);
}

String<0>& String<0>::operator +=(int32 n)
{
	int32 size = logicalSize + kStringAllocSize;
	if (size > physicalSize)
	{
		physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
		char *newPointer = new char[physicalSize];

		if (stringPointer != emptyString)
		{
			Text::CopyText(stringPointer, newPointer);
			delete[] stringPointer;
		}

		stringPointer = newPointer;
	}

	logicalSize += Text::IntegerToString(n, stringPointer + logicalSize - 1, kStringAllocSize);
	return (*this);
}

String<0>& String<0>::operator +=(unsigned_int32 n)
{
	int32 size = logicalSize + kStringAllocSize;
	if (size > physicalSize)
	{
		physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
		char *newPointer = new char[physicalSize];

		if (stringPointer != emptyString)
		{
			Text::CopyText(stringPointer, newPointer);
			delete[] stringPointer;
		}

		stringPointer = newPointer;
	}

	logicalSize += Text::IntegerToString(n, stringPointer + logicalSize - 1, kStringAllocSize);
	return (*this);
}

String<0>& String<0>::operator +=(int64 n)
{
	int32 size = logicalSize + kStringAllocSize;
	if (size > physicalSize)
	{
		physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
		char *newPointer = new char[physicalSize];

		if (stringPointer != emptyString)
		{
			Text::CopyText(stringPointer, newPointer);
			delete[] stringPointer;
		}

		stringPointer = newPointer;
	}

	logicalSize += Text::Integer64ToString(n, stringPointer + logicalSize - 1, kStringAllocSize);
	return (*this);
}

String<0>& String<0>::operator +=(unsigned_int64 n)
{
	int32 size = logicalSize + kStringAllocSize;
	if (size > physicalSize)
	{
		physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
		char *newPointer = new char[physicalSize];

		if (stringPointer != emptyString)
		{
			Text::CopyText(stringPointer, newPointer);
			delete[] stringPointer;
		}

		stringPointer = newPointer;
	}

	logicalSize += Text::Integer64ToString(n, stringPointer + logicalSize - 1, kStringAllocSize);
	return (*this);
}

String<0>& String<0>::SetLength(int32 length)
{
	int32 size = length + 1;
	if (size > 1)
	{
		if (size != logicalSize)
		{
			logicalSize = size;
			if ((size > physicalSize) || (size < physicalSize / 2))
			{
				physicalSize = GetPhysicalSize(size);
				char *newPointer = new char[physicalSize];

				if (stringPointer != emptyString)
				{
					Text::CopyText(stringPointer, newPointer, length);
					delete[] stringPointer;
				}

				stringPointer = newPointer;
			}

			stringPointer[length] = 0;
		}
	}
	else
	{
		Purge();
	}

	return (*this);
}

String<0>& String<0>::Append(const char *s, int32 length)
{
	if (length > 0)
	{
		int32 size = logicalSize + length;
		if (size > 1)
		{
			if (size > physicalSize)
			{
				physicalSize = Max(GetPhysicalSize(size), physicalSize + physicalSize / 2);
				char *newPointer = new char[physicalSize];

				if (stringPointer != emptyString)
				{
					Text::CopyText(stringPointer, newPointer);
					delete[] stringPointer;
				}

				stringPointer = newPointer;
			}

			Text::CopyText(s, stringPointer + logicalSize - 1, length);
			logicalSize = size;
		}
	}

	return (*this);
}

String<0>& String<0>::ConvertToLowerCase(void)
{
	unsigned_int8 *byte = reinterpret_cast<unsigned_int8 *>(stringPointer);
	for (;;)
	{
		unsigned_int32 c = byte[0];
		if (c == 0)
		{
			break;
		}

		if (c - 'A' < 26U)
		{
			byte[0] = (unsigned_int8) (c + 32);
		}

		byte++;
	}

	return (*this);
}

String<0>& String<0>::ConvertToUpperCase(void)
{
	unsigned_int8 *byte = reinterpret_cast<unsigned_int8 *>(stringPointer);
	for (;;)
	{
		unsigned_int32 c = byte[0];
		if (c == 0)
		{
			break;
		}

		if (c - 'a' < 26U)
		{
			byte[0] = (unsigned_int8) (c - 32);
		}

		byte++;
	}

	return (*this);
}

String<0>& String<0>::EncodeEscapeSequences(void)
{
	static const unsigned_int8 encodedSize[128] =
	{
		1, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4
	};

	int32 encodedLength = 0;
	int32 maxCharSize = 1;

	unsigned_int8 *byte = reinterpret_cast<unsigned_int8 *>(stringPointer);
	for (;;)
	{
		unsigned_int32 c = byte[0];
		if (c == 0)
		{
			break;
		}

		if (c < 128U)
		{
			int32 size = encodedSize[c];
			maxCharSize = Max(maxCharSize, size);
			encodedLength += size;
		}
		else
		{
			encodedLength++;
		}

		byte++;
	}

	if (maxCharSize > 1)
	{
		logicalSize = encodedLength;
		physicalSize = GetPhysicalSize(encodedLength);
		char *encodedString = new char[physicalSize];

		byte = reinterpret_cast<unsigned_int8 *>(stringPointer);
		unsigned_int8 *encodedByte = reinterpret_cast<unsigned_int8 *>(encodedString);
		for (;;)
		{
			unsigned_int32 c = byte[0];
			if (c == 0)
			{
				break;
			}

			if (c < 128U)
			{
				int32 size = encodedSize[c];
				if (size == 1)
				{
					encodedByte[0] = (unsigned_int8) c;
				}
				else if (size == 2)
				{
					encodedByte[0] = '\\';

					if (c < 32U)
					{
						static const unsigned_int8 encodedChar[7] =
						{
							'a', 'b', 't', 'n', 'v', 'f', 'r'
						};

						encodedByte[1] = encodedChar[c - 7];
					}
					else
					{
						encodedByte[1] = c;
					}
				}
				else
				{
					encodedByte[0] = '\\';
					encodedByte[1] = 'x';
					encodedByte[2] = Text::hexDigit[c >> 4];
					encodedByte[3] = Text::hexDigit[c & 15];
				}

				encodedByte += size;
			}
			else
			{
				encodedByte[0] = (unsigned_int8) c;
				encodedByte++;
			}

			byte++;
		}

		encodedByte[0] = 0;

		delete[] stringPointer;
		stringPointer = encodedString;
	}

	return (*this);
}

// ZYUQURM
