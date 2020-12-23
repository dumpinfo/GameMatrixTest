 

#include "C4OpenDDL.h"


using namespace C4;


/*
<nondigit>				::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
						  | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
						  | "_"

<decimal-digit>			::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

<hex-digit>				::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "A" | "B" | "C" | "D" | "E" | "F" | "a" | "b" | "c" | "d" | "e" | "f"

<octal-digit>			::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7"

<binary-digit>			::= "0" | "1"

<sign>					::= "" | "+" | "-"

<identifier>			::= <nondigit>
						  | <identifier><nondigit>
						  | <identifier><decimal-digit>

<global-name>			::= "$"<identifier>

<local-name>			::= "%"<identifier>

<name>					::= <global-name>
						  | <local-name>

<reference>				::= <name>
						  | <reference> <local-name>
						  | "null"

<bool-literal>			::= "false"
						  | "true"

<decimal-literal>		::= <decimal-digit>
						  | <decimal-literal><decimal-digit>
						  | <decimal-literal>"_"<decimal-digit>

<hex-literal>			::= "0x"<hex-digit>
						  | "0X"<hex-digit>
						  | <hex-literal><hex-digit>
						  | <hex-literal>"_"<hex-digit>

<octal-literal>			::= "0o"<octal-digit>
						  | "0O"<octal-digit>
						  | <octal-literal><octal-digit>
						  | <octal-literal>"_"<octal-digit>

<binary-literal>		::= "0b"<binary-digit>
						  | "0B"<binary-digit>
						  | <binary-literal><binary-digit>
						  | <binary-literal>"_"<binary-digit>

<char>					::= ASCII character in the ranges [0x0020,0x0026], [0x0028,0x005B], [0x005D,0x007E]
						  | <escape-char>

<char-seq>				::= <char>
						  | <char-seq><char>

<char-literal>			::= "'"<char-seq>"'"

<unsigned-literal>		::= <decimal-literal>
						  | <hex-literal>
						  | <octal-literal>
						  | <binary-literal>
						  | <char-literal>

<integer-literal>		::= <sign> <unsigned-literal>

<float-exponent>		::= "e"<sign><decimal-literal>
						  | "E"<sign><decimal-literal>

<float-magnitude>		::= <decimal-literal>"."<decimal-literal><float-exponent>
						  | <decimal-literal>"."<decimal-literal>
						  | <decimal-literal>"."
						  | <decimal-literal><float-exponent>
						  | <decimal-literal>
						  | "."<decimal-literal><float-exponent>
						  | "."<decimal-literal>
						  | <hex-literal>
						  | <octal-literal>
						  | <binary-literal>

<float-literal>			::= <sign> <float-magnitude>

<escape-char>			::= "\"'"' | "\'" | "\?" | "\\" | "\a" | "\b" | "\f" | "\n" | "\r" | "\t" | "\v"
						  | "\x"<hex-digit><hex-digit>

<string-char>			::= Unicode character in the ranges [0x0020,0x0021], [0x0023,0x005B], [0x005D,0x007E], [0x00A0,0xD7FF], [0xE000,0xFFFD], [0x010000,0x10FFFF]
						  | <escape-char>
						  | "\u"<hex-digit><hex-digit><hex-digit><hex-digit>
						  | "\U"<hex-digit><hex-digit><hex-digit><hex-digit><hex-digit><hex-digit>

<string-char-seq>		::= <string-char>
						  | <string-char-seq><string-char>
 
<string-literal>		::= '"'<string-char-seq>'"'
						  | <string-literal> '"'<string-char-seq>'"'
 
<data-type>				::= "bool"
						  | "int8" 
						  | "int16"
						  | "int32"
						  | "int64" 
						  | "unsigned_int8"
						  | "unsigned_int16" 
						  | "unsigned_int32" 
						  | "unsigned_int64"
						  | "half"
						  | "float"
						  | "double" 
						  | "string"
						  | "ref"
						  | "type"

<property-value>		::= <bool-literal>
						  | <integer-literal>
						  | <float-literal>
						  | <string-literal>
						  | <reference>
						  | <data-type>

<property>				::= <identifier> "=" <property-value>

<property-list>			::= <property-seq>
						  | ""

<property-seq>			::= <property>
						  | <property-seq> "," <property>

<bool-list>				::= <bool-literal>
						  | <bool-list> "," <bool-literal>

<integer-list>			::= <integer-literal>
						  | <integer-list> "," <integer-literal>

<float-list>			::= <float-literal>
						  | <float-list> "," <float-literal>

<string-list>			::= <string-literal>
						  | <string-list> "," <string-literal>

<ref-list>				::= <reference>
						  | <ref-list> "," <reference>

<type-list>				::= <data-type>
						  | <type-list> "," <data-type>

<data-list>				::= <bool-list>
						  | <integer-list>
						  | <float-list>
						  | <string-list>
						  | <ref-list>
						  | <type-list>
						  | ""

<bool-array-list>		::= "{" <bool-list> "}"
						  | <bool-array-list> "," "{" <bool-list> "}"

<integer-array-list>	::= "{" <integer-list> "}"
						  | <integer-array-list> "," "{" <integer-list> "}"

<float-array-list>		::= "{" <float-list> "}"
						  | <float-array-list> "," "{" <float-list> "}"

<string-array-list>		::= "{" <string-list> "}"
						  | <string-array-list> "," "{" <string-list> "}"

<ref-array-list>		::= "{" <ref-list> "}"
						  | <ref-array-list> "," "{" <ref-list> "}"

<type-array-list>		::= "{" <type-list> "}"
						  | <type-array-list> "," "{" <type-list> "}"

<data-array-list>		::= <bool-array-list>
						  | <integer-array-list>
						  | <float-array-list>
						  | <string-array-list>
						  | <ref-array-list>
						  | <type-array-list>
						  | ""

<struct-name>			::= <name>
						  | ""

<struct-decl>			::= <identifier> <struct-name>
						  | <identifier> <struct-name> "(" <property-list> ")"

<structure>				::= <data-type> <struct-name> "{" <data-list> "}"
						  | <data-type> "[" <integer-literal> "]" <struct-name> "{" <data-array-list> "}"
						  | <struct-decl> "{" <struct-seq> "}"

<struct-seq>			::= <structure>
						  | <struct-seq> <structure>
						  | ""
*/

/*
	struct $instances (id = "instances")
	{
		data
		{
			type {int8, int16, int32, int64, unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64}
			instances {int32 {1, 1}}
			elements {int32 {1, 2}}
		}
	}

	struct $elements (id = "elements")
	{
		data
		{
			type {int8, int16, int32, int64, unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64}
			instances {int32 {1, 1}}
			elements {int32 {1, 2}}
		}
	}

	struct $array_size (id = "array_size")
	{
		data
		{
			type {int8, int16, int32, int64, unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64}
			instances {int32 {1, 1}}
			elements {int32 {2, 2}}
		}
	}

	struct $default (id = "default")
	{
		data
		{
			type {bool, int8, int16, int32, int64, unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64, float, double, string, ref, type}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}
	}

	struct $base (id = "base")
	{
		data
		{
			type {ref}
			instances {int32 {1, 1}}
			elements {int32 {1}}
		}
	}

	struct $property (id = "property")
	{
		property (id = "id")
		{
			type {string}
		}

		data
		{
			type {type}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}

		sub
		{
			ref {$default}
			instances {int32 {0, 1}}
		}
	}

	struct $sub (id = "sub")
	{
		data
		{
			type {ref}
			instances {int32 {1, 1}}
			elements {int32 {1}}
		}

		sub
		{
			ref {$instances}
			instances {int32 {0, 1}}
		}
	}

	struct $data (id = "data")
	{
		data
		{
			type {type}
			instances {int32 {1, 1}}
			elements {int32 {1}}
		}

		sub
		{
			ref {$instances, $elements, $array_size}
			instances {int32 {0, 1}}
		}
	}

	struct $struct (id = "struct")
	{
		property (id = "id")
		{
			type {string}
		}

		sub
		{
			ref {$base}
			instances {int32 {0, 1}}
		}

		sub
		{
			ref {$property, $sub, $data}
		}
	}
*/


namespace C4
{
	namespace Data
	{
		const int8 hexadecimalCharValue[55] =
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
			-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, 10, 11, 12, 13, 14, 15
		};

		const int8 identifierCharState[256] =
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
			0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
			0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
		};


		int32 ReadEscapeChar(const char *text, unsigned_int32 *value);
		int32 ReadStringEscapeChar(const char *text, int32 *stringLength, char *restrict string);
		DataResult ReadCharLiteral(const char *text, int32 *textLength, unsigned_int64 *value);
		DataResult ReadDecimalLiteral(const char *text, int32 *textLength, unsigned_int64 *value);
		DataResult ReadHexadecimalLiteral(const char *text, int32 *textLength, unsigned_int64 *value);
		DataResult ReadOctalLiteral(const char *text, int32 *textLength, unsigned_int64 *value);
		DataResult ReadBinaryLiteral(const char *text, int32 *textLength, unsigned_int64 *value);
		bool ParseSign(const char *& text);
	}
}


int32 Data::GetWhitespaceLength(const char *text)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);
	for (;;)
	{
		unsigned_int32 c = byte[0];
		if (c == 0)
		{
			break;
		}

		if (c >= 33U)
		{
			if (c != '/')
			{
				break;
			}

			c = byte[1];
			if (c == '/')
			{
				byte += 2;
				for (;;)
				{
					c = byte[0];
					if (c == 0)
					{
						goto end;
					}

					byte++;

					if (c == 10)
					{
						break;
					}
				}

				continue;
			}
			else if (c == '*')
			{
				byte += 2;
				for (;;)
				{
					c = byte[0];
					if (c == 0)
					{
						goto end;
					}

					byte++;

					if ((c == '*') && (byte[0] == '/'))
					{
						byte++;
						break;
					}
				}

				continue;
			}

			break;
		}

		byte++;
	}

	end:
	return ((int32) (reinterpret_cast<const char *>(byte) - text));
}

DataResult Data::ReadDataType(const char *text, int32 *textLength, DataType *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int32 c = byte[0];
	if (c == 'i')
	{
		if ((byte[1] == 'n') && (byte[2] == 't'))
		{
			if ((byte[3] == '8') && (identifierCharState[byte[4]] == 0))
			{
				*value = kDataInt8;
				*textLength = 4;
				return (kDataOkay);
			}

			if ((byte[3] == '1') && (byte[4] == '6') && (identifierCharState[byte[5]] == 0))
			{
				*value = kDataInt16;
				*textLength = 5;
				return (kDataOkay);
			}

			if ((byte[3] == '3') && (byte[4] == '2') && (identifierCharState[byte[5]] == 0))
			{
				*value = kDataInt32;
				*textLength = 5;
				return (kDataOkay);
			}

			if ((byte[3] == '6') && (byte[4] == '4') && (identifierCharState[byte[5]] == 0))
			{
				*value = kDataInt64;
				*textLength = 5;
				return (kDataOkay);
			}
		}
	}
	else if (c == 'u')
	{
		if (Text::CompareText(&text[1], "nsigned_int", 11))
		{
			if ((byte[12] == '8') && (identifierCharState[byte[13]] == 0))
			{
				*value = kDataUnsignedInt8;
				*textLength = 13;
				return (kDataOkay);
			}

			if ((byte[12] == '1') && (byte[13] == '6') && (identifierCharState[byte[14]] == 0))
			{
				*value = kDataUnsignedInt16;
				*textLength = 14;
				return (kDataOkay);
			}

			if ((byte[12] == '3') && (byte[13] == '2') && (identifierCharState[byte[14]] == 0))
			{
				*value = kDataUnsignedInt32;
				*textLength = 14;
				return (kDataOkay);
			}

			if ((byte[12] == '6') && (byte[13] == '4') && (identifierCharState[byte[14]] == 0))
			{
				*value = kDataUnsignedInt64;
				*textLength = 14;
				return (kDataOkay);
			}
		}
	}
	else
	{
		if ((Text::CompareText(text, "bool", 4)) && (identifierCharState[byte[4]] == 0))
		{
			*value = kDataBool;
			*textLength = 4;
			return (kDataOkay);
		}

		if ((Text::CompareText(text, "half", 5)) && (identifierCharState[byte[4]] == 0))
		{
			*value = kDataHalf;
			*textLength = 4;
			return (kDataOkay);
		}

		if ((Text::CompareText(text, "float", 5)) && (identifierCharState[byte[5]] == 0))
		{
			*value = kDataFloat;
			*textLength = 5;
			return (kDataOkay);
		}

		if ((Text::CompareText(text, "double", 6)) && (identifierCharState[byte[6]] == 0))
		{
			*value = kDataDouble;
			*textLength = 6;
			return (kDataOkay);
		}

		if ((Text::CompareText(text, "string", 6)) && (identifierCharState[byte[6]] == 0))
		{
			*value = kDataString;
			*textLength = 6;
			return (kDataOkay);
		}

		if ((Text::CompareText(text, "ref", 3)) && (identifierCharState[byte[3]] == 0))
		{
			*value = kDataRef;
			*textLength = 3;
			return (kDataOkay);
		}

		if ((Text::CompareText(text, "type", 4)) && (identifierCharState[byte[4]] == 0))
		{
			*value = kDataType;
			*textLength = 4;
			return (kDataOkay);
		}
	}

	return (kDataTypeInvalid);
}

DataResult Data::ReadIdentifier(const char *text, int32 *textLength, char *restrict identifier)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);
	int32 count = 0;

	unsigned_int32 c = byte[0];
	int32 state = identifierCharState[c];

	if (state == 1)
	{
		if (c < 'A')
		{
			return (kDataIdentifierIllegalChar);
		}

		if (identifier)
		{
			identifier[count] = (char) c;
		}

		count++;
		for (;;)
		{
			c = byte[count];
			state = identifierCharState[c];

			if (state == 1)
			{
				if (identifier)
				{
					identifier[count] = (char) c;
				}

				count++;
				continue;
			}
			else if (state == 2)
			{
				return (kDataIdentifierIllegalChar);
			}

			break;
		}

		if (identifier)
		{
			identifier[count] = 0;
		}

		*textLength = count;
		return (kDataOkay);
	}
	else if (state == 2)
	{
		return (kDataIdentifierIllegalChar);
	}

	return (kDataIdentifierEmpty);
}

int32 Data::ReadEscapeChar(const char *text, unsigned_int32 *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);
	unsigned_int32 c = byte[0];

	if ((c == '\"') || (c == '\'') || (c == '?') || (c == '\\'))
	{
		*value = c;
		return (1);
	}
	else if (c == 'a')
	{
		*value = '\a';
		return (1);
	}
	else if (c == 'b')
	{
		*value = '\b';
		return (1);
	}
	else if (c == 'f')
	{
		*value = '\f';
		return (1);
	}
	else if (c == 'n')
	{
		*value = '\n';
		return (1);
	}
	else if (c == 'r')
	{
		*value = '\r';
		return (1);
	}
	else if (c == 't')
	{
		*value = '\t';
		return (1);
	}
	else if (c == 'v')
	{
		*value = '\v';
		return (1);
	}
	else if (c == 'x')
	{
		c = byte[1] - '0';
		if (c < 55U)
		{
			int32 x = hexadecimalCharValue[c];
			if (x >= 0)
			{
				c = byte[2] - '0';
				if (c < 55U)
				{
					int32 y = hexadecimalCharValue[c];
					if (y >= 0)
					{
						*value = (char) ((x << 4) | y);
						return (3);
					}
				}
			}
		}
	}

	return (0);
}

int32 Data::ReadStringEscapeChar(const char *text, int32 *stringLength, char *restrict string)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);
	unsigned_int32 c = byte[0];

	if (c == 'u')
	{
		unsigned_int32 code = 0;

		for (machine a = 1; a <= 4; a++)
		{
			c = byte[a] - '0';
			if (c >= 55U)
			{
				return (0);
			}

			int32 x = hexadecimalCharValue[c];
			if (x < 0)
			{
				return (0);
			}

			code = (code << 4) | x;
		}

		if (code != 0)
		{
			if (string)
			{
				*stringLength = Text::WriteGlyphCodeUTF8(string, code);
			}
			else
			{
				*stringLength = 1 + (code >= 0x000080) + (code >= 0x000800);
			}

			return (5);
		}
	}
	if (c == 'U')
	{
		unsigned_int32 code = 0;

		for (machine a = 1; a <= 6; a++)
		{
			c = byte[a] - '0';
			if (c >= 55U)
			{
				return (0);
			}

			int32 x = hexadecimalCharValue[c];
			if (x < 0)
			{
				return (0);
			}

			code = (code << 4) | x;
		}

		if ((code != 0) && (code <= 0x10FFFF))
		{
			if (string)
			{
				*stringLength = Text::WriteGlyphCodeUTF8(string, code);
			}
			else
			{
				*stringLength = 1 + (code >= 0x000080) + (code >= 0x000800) + (code >= 0x010000);
			}

			return (7);
		}
	}
	else
	{
		unsigned_int32		value;

		int32 textLength = ReadEscapeChar(text, &value);
		if (textLength != 0)
		{
			if (string)
			{
				*string = (char) value;
			}

			*stringLength = 1;
			return (textLength);
		}
	}

	return (0);
}

DataResult Data::ReadStringLiteral(const char *text, int32 *textLength, int32 *stringLength, char *restrict string)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);
	int32 count = 0;

	for (;;)
	{
		unsigned_int32 c = byte[0];
		if ((c == 0) || (c == '\"'))
		{
			break;
		}

		if ((c < 32U) || (c == 127U))
		{
			return (kDataStringIllegalChar);
		}

		if (c != '\\')
		{
			int32 len = Text::ValidateGlyphCodeUTF8(reinterpret_cast<const char *>(byte));
			if (len == 0)
			{
				return (kDataStringIllegalChar);
			}

			if (string)
			{
				for (machine a = 0; a < len; a++)
				{
					string[a] = (char) byte[a];
				}

				string += len;
			}

			byte += len;
			count += len;
		}
		else
		{
			int32	stringLen;

			int32 textLen = ReadStringEscapeChar(reinterpret_cast<const char *>(++byte), &stringLen, string);
			if (textLen == 0)
			{
				return (kDataStringIllegalEscape);
			}

			if (string)
			{
				string += stringLen;
			}

			byte += textLen;
			count += stringLen;
		}
	}

	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	*stringLength = count;
	return (kDataOkay);
}

DataResult Data::ReadBoolLiteral(const char *text, int32 *textLength, bool *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int32 c = byte[0];
	if (c == 'f')
	{
		if ((byte[1] == 'a') && (byte[2] == 'l') && (byte[3] == 's') && (byte[4] == 'e') && (identifierCharState[byte[5]] == 0))
		{
			*value = false;
			*textLength = 5;
			return (kDataOkay);
		}
	}
	else if (c == 't')
	{
		if ((byte[1] == 'r') && (byte[2] == 'u') && (byte[3] == 'e') && (identifierCharState[byte[4]] == 0))
		{
			*value = true;
			*textLength = 4;
			return (kDataOkay);
		}
	}

	return (kDataBoolInvalid);
}

DataResult Data::ReadDecimalLiteral(const char *text, int32 *textLength, unsigned_int64 *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int64 v = 0;
	bool separator = false;
	for (;;)
	{
		unsigned_int32 x = byte[0] - '0';
		if (x < 10U)
		{
			if (v >= 0x199999999999999AULL)
			{
				return (kDataIntegerOverflow);
			}

			unsigned_int64 w = v;
			v = v * 10 + x;

			if ((w >= 9U) && (v < 9U))
			{
				return (kDataIntegerOverflow);
			}

			separator = true;
		}
		else
		{
			if ((x != 47) || (!separator))
			{
				break;
			}

			separator = false;
		}

		byte++;
	}

	if (!separator)
	{
		return (kDataSyntaxError);
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

DataResult Data::ReadHexadecimalLiteral(const char *text, int32 *textLength, unsigned_int64 *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text + 2);

	unsigned_int64 v = 0;
	bool separator = false;
	for (;;)
	{
		unsigned_int32 c = byte[0] - '0';
		if (c >= 55U)
		{
			break;
		}

		int32 x = hexadecimalCharValue[c];
		if (x >= 0)
		{
			if ((v >> 60) != 0)
			{
				return (kDataIntegerOverflow);
			}

			v = (v << 4) | x;
			separator = true;
		}
		else
		{
			if ((c != 47) || (!separator))
			{
				break;
			}

			separator = false;
		}

		byte++;
	}

	if (!separator)
	{
		return (kDataSyntaxError);
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

DataResult Data::ReadOctalLiteral(const char *text, int32 *textLength, unsigned_int64 *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text + 2);

	unsigned_int64 v = 0;
	bool separator = false;
	for (;;)
	{
		unsigned_int32 x = byte[0] - '0';
		if (x < 8U)
		{
			if (v >= 0x2000000000000000ULL)
			{
				return (kDataIntegerOverflow);
			}

			unsigned_int64 w = v;
			v = v * 8 + x;

			if ((w >= 7U) && (v < 7U))
			{
				return (kDataIntegerOverflow);
			}

			separator = true;
		}
		else
		{
			if ((x != 47) || (!separator))
			{
				break;
			}

			separator = false;
		}

		byte++;
	}

	if (!separator)
	{
		return (kDataSyntaxError);
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

DataResult Data::ReadBinaryLiteral(const char *text, int32 *textLength, unsigned_int64 *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text + 2);

	unsigned_int64 v = 0;
	bool separator = false;
	for (;;)
	{
		unsigned_int32 x = byte[0] - '0';
		if (x < 2U)
		{
			if ((v >> 63) != 0)
			{
				return (kDataIntegerOverflow);
			}

			v = (v << 1) | x;
			separator = true;
		}
		else
		{
			if ((x != 47) || (!separator))
			{
				break;
			}

			separator = false;
		}

		byte++;
	}

	if (!separator)
	{
		return (kDataSyntaxError);
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

DataResult Data::ReadCharLiteral(const char *text, int32 *textLength, unsigned_int64 *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int64 v = 0;
	for (;;)
	{
		unsigned_int32 c = byte[0];
		if ((c == 0) || (c == '\''))
		{
			break;
		}

		if ((c < 32U) || (c >= 127U))
		{
			return (kDataCharIllegalChar);
		}

		if (c != '\\')
		{
			if ((v >> 56) != 0)
			{
				return (kDataIntegerOverflow);
			}

			v = (v << 8) | c;
			byte++;
		}
		else
		{
			unsigned_int32		x;

			int32 length = ReadEscapeChar(reinterpret_cast<const char *>(++byte), &x);
			if (length == 0)
			{
				return (kDataCharIllegalEscape);
			}

			if ((v >> 56) != 0)
			{
				return (kDataIntegerOverflow);
			}

			v = (v << 8) | x;
			byte += length;
		}
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

DataResult Data::ReadUnsignedLiteral(const char *text, int32 *textLength, unsigned_int64 *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int32 c = byte[0];
	if (c == '0')
	{
		c = byte[1];

		if ((c == 'x') || (c == 'X'))
		{
			return (ReadHexadecimalLiteral(text, textLength, value));
		}

		if ((c == 'o') || (c == 'O'))
		{
			return (ReadOctalLiteral(text, textLength, value));
		}

		if ((c == 'b') || (c == 'B'))
		{
			return (ReadBinaryLiteral(text, textLength, value));
		}
	}
	else if (c == '\'')
	{
		int32	len;

		DataResult result = ReadCharLiteral(reinterpret_cast<const char *>(byte + 1), &len, value);
		if (result == kDataOkay)
		{
			if (byte[len + 1] != '\'')
			{
				return (kDataCharEndOfFile);
			}

			*textLength = len + 2;
		}

		return (result);
	}

	return (ReadDecimalLiteral(text, textLength, value));
}

DataResult Data::ReadFloatMagnitude(const char *text, int32 *textLength, Half *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int32 c = byte[0];
	if (c == '0')
	{
		c = byte[1];

		if ((c == 'x') || (c == 'X'))
		{
			unsigned_int64		v;

			DataResult result = ReadHexadecimalLiteral(text, textLength, &v);
			if (result == kDataOkay)
			{
				if (v > 0x000000000000FFFF)
				{
					return (kDataFloatOverflow);
				}

				*reinterpret_cast<unsigned_int32 *>(value) = (unsigned_int32) v;
			}

			return (result);
		}

		if ((c == 'o') || (c == 'O'))
		{
			unsigned_int64		v;

			DataResult result = ReadOctalLiteral(text, textLength, &v);
			if (result == kDataOkay)
			{
				if (v > 0x000000000000FFFF)
				{
					return (kDataFloatOverflow);
				}

				*reinterpret_cast<unsigned_int32 *>(value) = (unsigned_int32) v;
			}

			return (result);
		}

		if ((c == 'b') || (c == 'B'))
		{
			unsigned_int64		v;

			DataResult result = ReadBinaryLiteral(text, textLength, &v);
			if (result == kDataOkay)
			{
				if (v > 0x000000000000FFFF)
				{
					return (kDataFloatOverflow);
				}

				*reinterpret_cast<unsigned_int32 *>(value) = (unsigned_int32) v;
			}

			return (result);
		}
	}

	float v = 0.0F;
	bool separator = false;
	for (;;)
	{
		unsigned_int32 x = byte[0] - '0';
		if (x < 10U)
		{
			v = v * 10.0F + (float) x;
			separator = true;
		}
		else
		{
			if ((x != 47) || (!separator))
			{
				break;
			}

			separator = false;
		}

		byte++;
	}

	if (!separator)
	{
		return (kDataSyntaxError);
	}

	c = byte[0];
	if (c == '.')
	{
		byte++;

		float decimal = 10.0F;
		separator = false;
		for (;;)
		{
			unsigned_int32 x = byte[0] - '0';
			if (x < 10U)
			{
				v += (float) x / decimal;
				decimal *= 10.0F;
				separator = true;
			}
			else
			{
				if ((x != 47) || (!separator))
				{
					break;
				}

				separator = false;
			}

			byte++;
		}

		if (!separator)
		{
			return (kDataSyntaxError);
		}

		c = byte[0];
	}

	if ((c == 'e') || (c == 'E'))
	{
		bool negative = false;

		c = (++byte)[0];
		if (c == '-')
		{
			negative = true;
			byte++;
		}
		else if (c == '+')
		{
			byte++;
		}
		else if (c - '0' >= 10U)
		{
			return (kDataFloatInvalid);
		}

		int32 exponent = 0;
		bool digit = false;
		separator = false;
		for (;;)
		{
			unsigned_int32 x = byte[0] - '0';
			if (x < 10U)
			{
				exponent = Min(exponent * 10 + x, 65535);
				digit = false;
				separator = true;
			}
			else
			{
				if ((x != 47) || (!separator))
				{
					break;
				}

				separator = false;
			}

			byte++;
		}

		if ((!digit) || (!separator))
		{
			return (kDataSyntaxError);
		}

		if (exponent != 0)
		{
			if (negative)
			{
				exponent = -exponent;
			}

			v *= Exp((float) exponent * 2.3025850929940456840179914546844F);
		}
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

DataResult Data::ReadFloatMagnitude(const char *text, int32 *textLength, float *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int32 c = byte[0];
	if (c == '0')
	{
		c = byte[1];

		if ((c == 'x') || (c == 'X'))
		{
			unsigned_int64		v;

			DataResult result = ReadHexadecimalLiteral(text, textLength, &v);
			if (result == kDataOkay)
			{
				if (v > 0x00000000FFFFFFFF)
				{
					return (kDataFloatOverflow);
				}

				*reinterpret_cast<unsigned_int32 *>(value) = (unsigned_int32) v;
			}

			return (result);
		}

		if ((c == 'o') || (c == 'O'))
		{
			unsigned_int64		v;

			DataResult result = ReadOctalLiteral(text, textLength, &v);
			if (result == kDataOkay)
			{
				if (v > 0x00000000FFFFFFFF)
				{
					return (kDataFloatOverflow);
				}

				*reinterpret_cast<unsigned_int32 *>(value) = (unsigned_int32) v;
			}

			return (result);
		}

		if ((c == 'b') || (c == 'B'))
		{
			unsigned_int64		v;

			DataResult result = ReadBinaryLiteral(text, textLength, &v);
			if (result == kDataOkay)
			{
				if (v > 0x00000000FFFFFFFF)
				{
					return (kDataFloatOverflow);
				}

				*reinterpret_cast<unsigned_int32 *>(value) = (unsigned_int32) v;
			}

			return (result);
		}
	}

	float v = 0.0F;
	bool separator = false;
	for (;;)
	{
		unsigned_int32 x = byte[0] - '0';
		if (x < 10U)
		{
			v = v * 10.0F + (float) x;
			separator = true;
		}
		else
		{
			if ((x != 47) || (!separator))
			{
				break;
			}

			separator = false;
		}

		byte++;
	}

	if (!separator)
	{
		return (kDataSyntaxError);
	}

	c = byte[0];
	if (c == '.')
	{
		byte++;

		float decimal = 10.0F;
		separator = false;
		for (;;)
		{
			unsigned_int32 x = byte[0] - '0';
			if (x < 10U)
			{
				v += (float) x / decimal;
				decimal *= 10.0F;
				separator = true;
			}
			else
			{
				if ((x != 47) || (!separator))
				{
					break;
				}

				separator = false;
			}

			byte++;
		}

		if (!separator)
		{
			return (kDataSyntaxError);
		}

		c = byte[0];
	}

	if ((c == 'e') || (c == 'E'))
	{
		bool negative = false;

		c = (++byte)[0];
		if (c == '-')
		{
			negative = true;
			byte++;
		}
		else if (c == '+')
		{
			byte++;
		}
		else if (c - '0' >= 10U)
		{
			return (kDataFloatInvalid);
		}

		int32 exponent = 0;
		bool digit = false;
		separator = false;
		for (;;)
		{
			unsigned_int32 x = byte[0] - '0';
			if (x < 10U)
			{
				exponent = Min(exponent * 10 + x, 65535);
				digit = true;
				separator = true;
			}
			else
			{
				if ((x != 47) || (!separator))
				{
					break;
				}

				separator = false;
			}

			byte++;
		}

		if ((!digit) || (!separator))
		{
			return (kDataSyntaxError);
		}

		if (exponent != 0)
		{
			if (negative)
			{
				exponent = -exponent;
			}

			v *= Exp((float) exponent * 2.3025850929940456840179914546844F);
		}
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

DataResult Data::ReadFloatMagnitude(const char *text, int32 *textLength, double *value)
{
	const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);

	unsigned_int32 c = byte[0];
	if (c == '0')
	{
		c = byte[1];

		if ((c == 'x') || (c == 'X'))
		{
			unsigned_int64		v;

			DataResult result = ReadHexadecimalLiteral(text, textLength, &v);
			if (result == kDataIntegerOverflow)
			{
				return (kDataFloatOverflow);
			}

			*reinterpret_cast<unsigned_int64 *>(value) = v;
			return (result);
		}

		if ((c == 'o') || (c == 'O'))
		{
			unsigned_int64		v;

			DataResult result = ReadOctalLiteral(text, textLength, &v);
			if (result == kDataIntegerOverflow)
			{
				return (kDataFloatOverflow);
			}

			*reinterpret_cast<unsigned_int64 *>(value) = v;
			return (result);
		}

		if ((c == 'b') || (c == 'B'))
		{
			unsigned_int64		v;

			DataResult result = ReadBinaryLiteral(text, textLength, &v);
			if (result == kDataIntegerOverflow)
			{
				return (kDataFloatOverflow);
			}

			*reinterpret_cast<unsigned_int64 *>(value) = v;
			return (result);
		}
	}

	double v = 0.0;
	bool separator = false;
	for (;;)
	{
		unsigned_int32 x = byte[0] - '0';
		if (x < 10U)
		{
			v = v * 10.0 + (double) x;
			separator = true;
		}
		else
		{
			if ((x != 47) || (!separator))
			{
				break;
			}

			separator = false;
		}

		byte++;
	}

	if (!separator)
	{
		return (kDataSyntaxError);
	}

	c = byte[0];
	if (c == '.')
	{
		double decimal = 10.0;
		separator = false;
		for (;;)
		{
			unsigned_int32 x = byte[0] - '0';
			if (x < 10U)
			{
				v += (double) x / decimal;
				decimal *= 10.0;
				separator = true;
			}
			else
			{
				if ((x != 47) || (!separator))
				{
					break;
				}

				separator = false;
			}

			byte++;
		}

		if (!separator)
		{
			return (kDataSyntaxError);
		}

		c = byte[0];
	}

	if ((c == 'e') || (c == 'E'))
	{
		bool negative = false;

		c = (++byte)[0];
		if (c == '-')
		{
			negative = true;
			byte++;
		}
		else if (c == '+')
		{
			byte++;
		}
		else if (c - '0' >= 10U)
		{
			return (kDataFloatInvalid);
		}

		int32 exponent = 0;
		bool digit = false;
		separator = false;
		for (;;)
		{
			unsigned_int32 x = byte[0] - '0';
			if (x < 10U)
			{
				exponent = Min(exponent * 10 + x, 65535);
				digit = true;
				separator = true;
			}
			else
			{
				if ((x != 47) || (!separator))
				{
					break;
				}

				separator = false;
			}

			byte++;
		}

		if ((!digit) || (!separator))
		{
			return (kDataSyntaxError);
		}

		if (exponent != 0)
		{
			if (negative)
			{
				exponent = -exponent;
			}

			v *= Exp((double) exponent * 2.3025850929940456840179914546844);
		}
	}

	*value = v;
	*textLength = (int32) (reinterpret_cast<const char *>(byte) - text);
	return (kDataOkay);
}

bool Data::ParseSign(const char *& text)
{
	char c = text[0];

	if (c == '-')
	{
		text++;
		text += GetWhitespaceLength(text);
		return (true);
	}

	if (c == '+')
	{
		text++;
		text += GetWhitespaceLength(text);
	}

	return (false);
}


StructureRef::StructureRef(bool global)
{
	globalRefFlag = global;
}

StructureRef::~StructureRef()
{
}

void StructureRef::Reset(bool global)
{
	nameArray.Purge();
	globalRefFlag = global;
}


DataResult BoolDataType::ParseValue(const char *& text, PrimType *value)
{
	int32	length;

	DataResult result = Data::ReadBoolLiteral(text, &length, value);
	if (result != kDataOkay)
	{
		return (result);
	}

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult Int8DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (!negative)
	{
		if (unsignedValue > 0x7F)
		{
			return (kDataIntegerOverflow);
		}

		*value = (int8) unsignedValue;
	}
	else
	{
		if (unsignedValue > 0x80)
		{
			return (kDataIntegerOverflow);
		}

		*value = (int8) -(int64) unsignedValue;
	}

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult Int16DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (!negative)
	{
		if (unsignedValue > 0x7FFF)
		{
			return (kDataIntegerOverflow);
		}

		*value = (int16) unsignedValue;
	}
	else
	{
		if (unsignedValue > 0x8000)
		{
			return (kDataIntegerOverflow);
		}

		*value = (int16) -(int64) unsignedValue;
	}

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult Int32DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (!negative)
	{
		if (unsignedValue > 0x7FFFFFFF)
		{
			return (kDataIntegerOverflow);
		}

		*value = (int32) unsignedValue;
	}
	else
	{
		if (unsignedValue > 0x80000000)
		{
			return (kDataIntegerOverflow);
		}

		*value = (int32) -(int64) unsignedValue;
	}

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult Int64DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (!negative)
	{
		if (unsignedValue > 0x7FFFFFFFFFFFFFFF)
		{
			return (kDataIntegerOverflow);
		}

		*value = unsignedValue;
	}
	else
	{
		if (unsignedValue > 0x8000000000000000)
		{
			return (kDataIntegerOverflow);
		}

		*value = -(int64) unsignedValue;
	}

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult UnsignedInt8DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (negative)
	{
		unsignedValue = (unsigned_int64) -(int64) unsignedValue;
	}

	*value = (unsigned_int8) unsignedValue;

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult UnsignedInt16DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (negative)
	{
		unsignedValue = (unsigned_int64) -(int64) unsignedValue;
	}

	*value = (unsigned_int16) unsignedValue;

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult UnsignedInt32DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (negative)
	{
		unsignedValue = (unsigned_int64) -(int64) unsignedValue;
	}

	*value = (unsigned_int32) unsignedValue;

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult UnsignedInt64DataType::ParseValue(const char *& text, PrimType *value)
{
	int32			length;
	unsigned_int64	unsignedValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadUnsignedLiteral(text, &length, &unsignedValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (negative)
	{
		unsignedValue = (unsigned_int64) -(int64) unsignedValue;
	}

	*value = unsignedValue;

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult HalfDataType::ParseValue(const char *& text, PrimType *value)
{
	int32		length;
	Half		floatValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadFloatMagnitude(text, &length, &floatValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (negative)
	{
		floatValue = -floatValue;
	}

	*value = floatValue;

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult FloatDataType::ParseValue(const char *& text, PrimType *value)
{
	int32	length;
	float	floatValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadFloatMagnitude(text, &length, &floatValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (negative)
	{
		floatValue = -floatValue;
	}

	*value = floatValue;

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult DoubleDataType::ParseValue(const char *& text, PrimType *value)
{
	int32		length;
	double		floatValue;

	bool negative = Data::ParseSign(text);

	DataResult result = Data::ReadFloatMagnitude(text, &length, &floatValue);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (negative)
	{
		floatValue = -floatValue;
	}

	*value = floatValue;

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


DataResult StringDataType::ParseValue(const char *& text, PrimType *value)
{
	int32	textLength;
	int32	stringLength;

	if (text[0] != '"')
	{
		return (kDataStringInvalid);
	}

	int32 accumLength = 0;
	for (;;)
	{
		text++;

		DataResult result = Data::ReadStringLiteral(text, &textLength, &stringLength);
		if (result != kDataOkay)
		{
			return (result);
		}

		value->SetLength(accumLength + stringLength);
		Data::ReadStringLiteral(text, &textLength, &stringLength, &(*value)[accumLength]);
		accumLength += stringLength;

		text += textLength;
		if (text[0] != '"')
		{
			return (kDataStringInvalid);
		}

		text++;
		text += Data::GetWhitespaceLength(text);

		if (text[0] != '"')
		{
			break;
		}
	}

	return (kDataOkay);
}


DataResult RefDataType::ParseValue(const char *& text, PrimType *value)
{
	int32	textLength;

	char c = text[0];
	value->Reset(c != '%');

	if ((unsigned_int32) (c - '$') > 2U)
	{
		const unsigned_int8 *byte = reinterpret_cast<const unsigned_int8 *>(text);
		if ((byte[0] == 'n') && (byte[1] == 'u') && (byte[2] == 'l') && (byte[3] == 'l') && (Data::identifierCharState[byte[4]] == 0))
		{
			text += 4;
			text += Data::GetWhitespaceLength(text);

			return (kDataOkay);
		}

		return (kDataReferenceInvalid);
	}

	do
	{
		text++;

		DataResult result = Data::ReadIdentifier(text, &textLength);
		if (result != kDataOkay)
		{
			return (result);
		}

		String<>	string;

		string.SetLength(textLength);
		Data::ReadIdentifier(text, &textLength, string);
		value->AddName(static_cast<String<>&&>(string));

		text += textLength;
		text += Data::GetWhitespaceLength(text);
	} while (text[0] == '%');

	return (kDataOkay);
}


DataResult TypeDataType::ParseValue(const char *& text, PrimType *value)
{
	int32	length;

	DataResult result = Data::ReadDataType(text, &length, value);
	if (result != kDataOkay)
	{
		return (result);
	}

	text += length;
	text += Data::GetWhitespaceLength(text);

	return (kDataOkay);
}


Structure::Structure(StructureType type)
{
	structureType = type;
	baseStructureType = 0;
	globalNameFlag = true;
}

Structure::~Structure()
{
}

Structure *Structure::GetFirstSubstructure(StructureType type) const
{
	Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == type)
		{
			return (structure);
		}

		structure = structure->Next();
	}

	return (nullptr);
}

Structure *Structure::GetLastSubstructure(StructureType type) const
{
	Structure *structure = GetLastSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == type)
		{
			return (structure);
		}

		structure = structure->Previous();
	}

	return (nullptr);
}

Structure *Structure::FindStructure(const StructureRef& reference, int32 index) const
{
	if ((index != 0) || (!reference.GetGlobalRefFlag()))
	{
		const ImmutableArray<String<>>& nameArray = reference.GetNameArray();

		int32 count = nameArray.GetElementCount();
		if (count != 0)
		{
			Structure *structure = structureMap.Find(nameArray[index]);
			if (structure)
			{
				if (++index < count)
				{
					structure = structure->FindStructure(reference, index);
				}

				return (structure);
			}
		}
	}

	return (nullptr);
}

bool Structure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	return (false);
}

bool Structure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (true);
}

DataResult Structure::ProcessData(DataDescription *dataDescription)
{
	Structure *structure = GetFirstSubnode();
	while (structure)
	{
		DataResult result = structure->ProcessData(dataDescription);
		if (result != kDataOkay)
		{
			if (!dataDescription->errorStructure)
			{
				dataDescription->errorStructure = structure;
			}

			return (result);
		}

		structure = structure->Next();
	}

	return (kDataOkay);
}


PrimitiveStructure::PrimitiveStructure(StructureType type) : Structure(type)
{
	SetBaseStructureType(kStructurePrimitive);

	arraySize = 0;
}

PrimitiveStructure::~PrimitiveStructure()
{
}


template <class type> DataStructure<type>::DataStructure() : PrimitiveStructure(type::kStructureType)
{
}

template <class type> DataStructure<type>::~DataStructure()
{
}

template <class type> DataResult DataStructure<type>::ParseData(const char *& text)
{
	int32 count = 0;

	unsigned_int32 arraySize = GetArraySize();
	if (arraySize == 0)
	{
		for (;;)
		{
			dataArray.SetElementCount(count + 1);

			DataResult result = type::ParseValue(text, &dataArray[count]);
			if (result != kDataOkay)
			{
				return (result);
			}

			text += Data::GetWhitespaceLength(text);

			if (text[0] == ',')
			{
				text++;
				text += Data::GetWhitespaceLength(text);

				count++;
				continue;
			}

			break;
		}
	}
	else
	{
		for (;;)
		{
			if (text[0] != '{')
			{
				return (kDataPrimitiveInvalidFormat);
			}

			text++;
			text += Data::GetWhitespaceLength(text);

			dataArray.SetElementCount(count + arraySize);

			for (unsigned_machine index = 0; index < arraySize; index++)
			{
				if (index != 0)
				{
					if (text[0] != ',')
					{
						return (kDataPrimitiveArrayUnderSize);
					}

					text++;
					text += Data::GetWhitespaceLength(text);
				}

				DataResult result = type::ParseValue(text, &dataArray[count + index]);
				if (result != kDataOkay)
				{
					return (result);
				}

				text += Data::GetWhitespaceLength(text);
			}

			char c = text[0];
			if (c != '}')
			{
				return ((c == ',') ? kDataPrimitiveArrayOverSize : kDataPrimitiveInvalidFormat);
			}

			text++;
			text += Data::GetWhitespaceLength(text);

			if (text[0] == ',')
			{
				text++;
				text += Data::GetWhitespaceLength(text);

				count += arraySize;
				continue;
			}

			break;
		}
	}

	return (kDataOkay);
}


RootStructure::RootStructure() : Structure(kStructureRoot)
{
}

RootStructure::~RootStructure()
{
}

bool RootStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (dataDescription->ValidateTopLevelStructure(structure));
}


DataDescription::DataDescription()
{
}

DataDescription::~DataDescription()
{
}

Structure *DataDescription::FindStructure(const StructureRef& reference) const
{
	if (reference.GetGlobalRefFlag())
	{
		const ImmutableArray<String<>>& nameArray = reference.GetNameArray();

		int32 count = nameArray.GetElementCount();
		if (count != 0)
		{
			Structure *structure = structureMap.Find(nameArray[0]);
			if ((structure) && (count > 1))
			{
				structure = structure->FindStructure(reference, 1);
			}

			return (structure);
		}
	}

	return (nullptr);
}

Structure *DataDescription::CreatePrimitive(const String<>& identifier)
{
	int32		length;
	DataType	value;

	if (Data::ReadDataType(identifier, &length, &value) == kDataOkay)
	{
		switch (value)
		{
			case kDataBool:
				return (new DataStructure<BoolDataType>);
			case kDataInt8:
				return (new DataStructure<Int8DataType>);
			case kDataInt16:
				return (new DataStructure<Int16DataType>);
			case kDataInt32:
				return (new DataStructure<Int32DataType>);
			case kDataInt64:
				return (new DataStructure<Int64DataType>);
			case kDataUnsignedInt8:
				return (new DataStructure<UnsignedInt8DataType>);
			case kDataUnsignedInt16:
				return (new DataStructure<UnsignedInt16DataType>);
			case kDataUnsignedInt32:
				return (new DataStructure<UnsignedInt32DataType>);
			case kDataUnsignedInt64:
				return (new DataStructure<UnsignedInt64DataType>);
			case kDataHalf:
				return (new DataStructure<HalfDataType>);
			case kDataFloat:
				return (new DataStructure<FloatDataType>);
			case kDataDouble:
				return (new DataStructure<DoubleDataType>);
			case kDataString:
				return (new DataStructure<StringDataType>);
			case kDataRef:
				return (new DataStructure<RefDataType>);
			case kDataType:
				return (new DataStructure<TypeDataType>);
		}
	}

	return (nullptr);
}

Structure *DataDescription::CreateStructure(const String<>& identifier) const
{
	return (nullptr);
}

bool DataDescription::ValidateTopLevelStructure(const Structure *structure) const
{
	return (true);
}

DataResult DataDescription::ProcessData(void)
{
	return (rootStructure.ProcessData(this));
}

DataResult DataDescription::ParseProperties(const char *& text, Structure *structure)
{
	for (;;)
	{
		int32		length;
		DataType	type;
		void		*value;

		DataResult result = Data::ReadIdentifier(text, &length);
		if (result != kDataOkay)
		{
			return (result);
		}

		String<>	identifier;

		identifier.SetLength(length);
		Data::ReadIdentifier(text, &length, identifier);

		if (!structure->ValidateProperty(this, identifier, &type, &value))
		{
			return (kDataPropertyUndefined);
		}

		identifier.Purge();

		text += length;
		text += Data::GetWhitespaceLength(text);

		if (text[0] != '=')
		{
			return (kDataPropertySyntaxError);
		}

		text++;
		text += Data::GetWhitespaceLength(text);

		switch (type)
		{
			case kDataBool:
				result = BoolDataType::ParseValue(text, static_cast<BoolDataType::PrimType *>(value));
				break;
			case kDataInt8:
				result = Int8DataType::ParseValue(text, static_cast<Int8DataType::PrimType *>(value));
				break;
			case kDataInt16:
				result = Int16DataType::ParseValue(text, static_cast<Int16DataType::PrimType *>(value));
				break;
			case kDataInt32:
				result = Int32DataType::ParseValue(text, static_cast<Int32DataType::PrimType *>(value));
				break;
			case kDataInt64:
				result = Int64DataType::ParseValue(text, static_cast<Int64DataType::PrimType *>(value));
				break;
			case kDataUnsignedInt8:
				result = UnsignedInt8DataType::ParseValue(text, static_cast<UnsignedInt8DataType::PrimType *>(value));
				break;
			case kDataUnsignedInt16:
				result = UnsignedInt16DataType::ParseValue(text, static_cast<UnsignedInt16DataType::PrimType *>(value));
				break;
			case kDataUnsignedInt32:
				result = UnsignedInt32DataType::ParseValue(text, static_cast<UnsignedInt32DataType::PrimType *>(value));
				break;
			case kDataUnsignedInt64:
				result = UnsignedInt64DataType::ParseValue(text, static_cast<UnsignedInt64DataType::PrimType *>(value));
				break;
			case kDataHalf:
				result = HalfDataType::ParseValue(text, static_cast<HalfDataType::PrimType *>(value));
				break;
			case kDataFloat:
				result = FloatDataType::ParseValue(text, static_cast<FloatDataType::PrimType *>(value));
				break;
			case kDataDouble:
				result = DoubleDataType::ParseValue(text, static_cast<DoubleDataType::PrimType *>(value));
				break;
			case kDataString:
				result = StringDataType::ParseValue(text, static_cast<StringDataType::PrimType *>(value));
				break;
			case kDataRef:
				result = RefDataType::ParseValue(text, static_cast<RefDataType::PrimType *>(value));
				break;
			case kDataType:
				result = TypeDataType::ParseValue(text, static_cast<TypeDataType::PrimType *>(value));
				break;
			default:
				return (kDataPropertyInvalidType);
		}

		if (result != kDataOkay)
		{
			return (result);
		}

		if (text[0] == ',')
		{
			text++;
			text += Data::GetWhitespaceLength(text);

			continue;
		}

		break;
	}

	return (kDataOkay);
}

DataResult DataDescription::ParseStructures(const char *& text, Structure *root)
{
	for (;;)
	{
		int32	length;

		DataResult result = Data::ReadIdentifier(text, &length);
		if (result != kDataOkay)
		{
			return (result);
		}

		String<>	identifier;

		identifier.SetLength(length);
		Data::ReadIdentifier(text, &length, identifier);

		bool primitive = false;

		Structure *structure = CreatePrimitive(identifier);
		if (structure)
		{
			primitive = true;
		}
		else
		{
			structure = CreateStructure(identifier);
			if (!structure)
			{
				return (kDataStructUndefined);
			}
		}

		identifier.Purge();

		AutoDelete<Structure> structurePtr(structure);
		structure->textLocation = text;

		text += length;
		text += Data::GetWhitespaceLength(text);

		if ((primitive) && (text[0] == '['))
		{
			unsigned_int64		value;

			text++;
			text += Data::GetWhitespaceLength(text);

			if (Data::ParseSign(text))
			{
				return (kDataPrimitiveIllegalArraySize);
			}

			result = Data::ReadUnsignedLiteral(text, &length, &value);
			if (result != kDataOkay)
			{
				return (result);
			}

			if ((value == 0) || (value > kDataMaxPrimitiveArraySize))
			{
				return (kDataPrimitiveIllegalArraySize);
			}

			text += length;
			text += Data::GetWhitespaceLength(text);

			if (text[0] != ']')
			{
				return (kDataPrimitiveSyntaxError);
			}

			text++;
			text += Data::GetWhitespaceLength(text);

			static_cast<PrimitiveStructure *>(structure)->arraySize = (unsigned_int32) value;
		}

		if (!root->ValidateSubstructure(this, structure))
		{
			return (kDataInvalidStructure);
		}

		char c = text[0];
		if ((unsigned_int32) (c - '$') < 2U)
		{
			text++;

			result = Data::ReadIdentifier(text, &length);
			if (result != kDataOkay)
			{
				return (result);
			}

			Data::ReadIdentifier(text, &length, structure->structureName.SetLength(length));

			bool global = (c == '$');
			structure->globalNameFlag = global;

			Map<Structure> *map = (global) ? &structureMap : &root->structureMap;
			if (!map->Insert(structure))
			{
				return (kDataStructNameExists);
			}

			text += length;
			text += Data::GetWhitespaceLength(text);
		}

		if ((!primitive) && (text[0] == '('))
		{
			text++;
			text += Data::GetWhitespaceLength(text);

			if (text[0] != ')')
			{
				result = ParseProperties(text, structure);
				if (result != kDataOkay)
				{
					return (result);
				}

				if (text[0] != ')')
				{
					return (kDataPropertySyntaxError);
				}
			}

			text++;
			text += Data::GetWhitespaceLength(text);
		}

		if (text[0] != '{')
		{
			return (kDataSyntaxError);
		}

		text++;
		text += Data::GetWhitespaceLength(text);

		if (text[0] != '}')
		{
			if (primitive)
			{
				result = static_cast<PrimitiveStructure *>(structure)->ParseData(text);
				if (result != kDataOkay)
				{
					return (result);
				}
			}
			else
			{
				result = ParseStructures(text, structure);
				if (result != kDataOkay)
				{
					return (result);
				}
			}
		}

		if (text[0] != '}')
		{
			return (kDataSyntaxError);
		}

		text++;
		text += Data::GetWhitespaceLength(text);

		root->AppendSubnode(structure);
		structurePtr = nullptr;

		c = text[0];
		if ((c == 0) || (c == '}'))
		{
			break;
		}
	}

	return (kDataOkay);
}

DataResult DataDescription::ProcessText(const char *text)
{
	rootStructure.PurgeSubtree();

	errorStructure = nullptr;
	errorLine = 0;

	const char *start = text;
	text += Data::GetWhitespaceLength(text);

	DataResult result = ParseStructures(text, &rootStructure);
	if ((result == kDataOkay) && (text[0] != 0))
	{
		result = kDataSyntaxError;
	}

	if (result == kDataOkay)
	{
		result = ProcessData();
		if ((result != kDataOkay) && (errorStructure))
		{
			text = errorStructure->textLocation;
		}
	}

	if (result != kDataOkay)
	{
		rootStructure.PurgeSubtree();

		int32 line = 1;
		while (text != start)
		{
			if ((--text)[0] == '\n')
			{
				line++;
			}
		}

		errorLine = line;
	}

	return (result);
}

// ZYUQURM
