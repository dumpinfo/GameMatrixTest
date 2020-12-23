 

#include "C4Fonts.h"


using namespace C4;


ResourceDescriptor FontResource::descriptor("fnt", 0, 0, "font/Gui");


FontResource::FontResource(const char *name, ResourceCatalog *catalog) : Resource<FontResource>(name, catalog)
{
	owningFont = nullptr;
}

FontResource::~FontResource()
{
}


C4::Font::Font(FontResource *resource, const char *name) : textureAttribute(name)
{
	fontResource = resource;
	resource->SetOwningFont(this);
	fontHeader = resource->GetFontHeader();
}

C4::Font::~Font()
{
	fontResource->Release();
}

C4::Font *C4::Font::Get(const char *name)
{
	FontResource *resource = FontResource::Get(name);

	Font *font = resource->GetOwningFont();
	if (font)
	{
		font->Retain();
		resource->Release();
		return (font);
	}

	return (new Font(resource, name));
}

float C4::Font::GetTextWidth(const char *text) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	for (;; text++)
	{
		unsigned_int32 code = *reinterpret_cast<const unsigned_int8 *>(text);
		if (code == 0)
		{
			break;
		}

		w += glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
	}

	return (FmaxZero(w));
}

float C4::Font::GetTextWidth(const char *text, int32 length) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	while (length > 0)
	{
		unsigned_int32 code = *reinterpret_cast<const unsigned_int8 *>(text);
		if (code == 0)
		{
			break;
		}

		text++;
		length--;
		w += glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
	}

	return (FmaxZero(w));
}

int32 C4::Font::GetTextLengthFitWidth(const char *text, float width, float *actual) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	int32 len = 0;

	for (;; text++)
	{
		unsigned_int32 code = *reinterpret_cast<const unsigned_int8 *>(text);
		if (code == 0)
		{ 
			break;
		}
 
		float x = w + glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		if (x > width) 
		{
			break;
		} 

		w = x; 
		len++; 
	}

	if (actual)
	{ 
		*actual = FmaxZero(w);
	}

	return (len);
}

int32 C4::Font::GetTextLengthFitWidth(const char *text, int32 length, float width, float *actual) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	int32 len = 0;

	for (machine a = 0; a < length; a++, text++)
	{
		unsigned_int32 code = *reinterpret_cast<const unsigned_int8 *>(text);
		if (code == 0)
		{
			break;
		}

		float x = w + glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		if (x > width)
		{
			break;
		}

		w = x;
		len++;
	}

	if (actual)
	{
		*actual = FmaxZero(w);
	}

	return (len);
}

int32 C4::Font::GetBrokenTextLengthFitWidth(const char *text, float width, bool partialWord, float *actual, bool *hardBreak) const
{
	float	textWidth;

	bool hard = false;
	int32 length = 0;

	for (;;)
	{
		unsigned_int32 code = text[length];
		if (code == 0)
		{
			break;
		}

		length++;
		if ((code == 0x000A) || (code == 0x000D))
		{
			hard = true;
			break;
		}
	}

	if (hardBreak)
	{
		*hardBreak = hard;
	}

	if (length == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	int32 len = GetTextLengthFitWidth(text, length, width, &textWidth);
	if (len == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	if (len < length)
	{
		int32 l = len;
		float w = textWidth;

		for (; len > 0; len--)
		{
			unsigned_int32 code = reinterpret_cast<const unsigned_int8 *>(text)[len - 1];
			if ((code == 0x0020) || (code == '-') || (code == '/'))
			{
				break;
			}

			textWidth -= glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		}

		if ((len == 0) && (partialWord))
		{
			len = l;
			textWidth = w;
		}
	}

	if (actual)
	{
		float spaceWidth = glyphData[GetGlyphIndex(0x0020)].glyphWidth + spacing;

		int32 pos = len - 1;
		while ((pos >= 0) && (text[pos] == 0x0020))
		{
			textWidth -= spaceWidth;
			pos--;
		}

		*actual = textWidth;
	}

	while (text[len] == 0x0020)
	{
		len++;
	}

	return (len);
}

int32 C4::Font::GetBrokenTextLengthFitWidth(const char *text, int32 length, float width, bool partialWord, float *actual, bool *hardBreak) const
{
	float	textWidth;

	int32 hard = false;
	int32 end = length;

	for (length = 0; length < end;)
	{
		unsigned_int32 code = text[length];
		if (code == 0)
		{
			break;
		}

		length++;
		if ((code == 0x000A) || (code == 0x000D))
		{
			hard = true;
			break;
		}
	}

	if (hardBreak)
	{
		*hardBreak = hard;
	}

	if (length == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	int32 len = GetTextLengthFitWidth(text, length, width, &textWidth);
	if (len == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	if (len < length)
	{
		int32 l = len;
		float w = textWidth;

		for (; len > 0; len--)
		{
			unsigned_int32 code = reinterpret_cast<const unsigned_int8 *>(text)[len - 1];
			if ((code == 0x0020) || (code == '-') || (code == '/'))
			{
				break;
			}

			textWidth -= glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		}

		if ((len == 0) && (partialWord))
		{
			len = l;
			textWidth = w;
		}
	}

	if (actual)
	{
		float spaceWidth = glyphData[GetGlyphIndex(0x0020)].glyphWidth + spacing;

		int32 pos = len - 1;
		while ((pos >= 0) && (text[pos] == 0x0020))
		{
			textWidth -= spaceWidth;
			pos--;
		}

		*actual = textWidth;
	}

	while (text[len] == 0x0020)
	{
		len++;
	}

	return (len);
}

int32 C4::Font::CountBrokenTextLines(const char *text, float width) const
{
	int32 length = Text::GetTextLength(text);
	int32 position = 0;
	int32 line = 0;

	while (position < length)
	{
		position += GetBrokenTextLengthFitWidth(&text[position], width);
		line++;
	}

	return (line);
}

float C4::Font::GetTextWidthUTF8(const char *text) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	for (;;)
	{
		unsigned_int32	code;

		int32 read = Text::ReadGlyphCodeUTF8(text, &code);
		if (code == 0)
		{
			break;
		}

		text += read;
		w += glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
	}

	return (FmaxZero(w));
}

float C4::Font::GetTextWidthUTF8(const char *text, int32 length) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	while (length > 0)
	{
		unsigned_int32	code;

		int32 read = Text::ReadGlyphCodeUTF8(text, &code);
		if (code == 0)
		{
			break;
		}

		text += read;
		length -= read;
		w += glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
	}

	return (FmaxZero(w));
}

int32 C4::Font::GetTextLengthFitWidthUTF8(const char *text, float width, float *actual) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	int32 len = 0;

	for (;;)
	{
		unsigned_int32	code;

		int32 read = Text::ReadGlyphCodeUTF8(text, &code);
		if (code == 0)
		{
			break;
		}

		float x = w + glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		if (x > width)
		{
			break;
		}

		w = x;
		len += read;
		text += read;
	}

	if (actual)
	{
		*actual = FmaxZero(w);
	}

	return (len);
}

int32 C4::Font::GetTextLengthFitWidthUTF8(const char *text, int32 length, float width, float *actual) const
{
	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	float w = -spacing;
	int32 len = 0;

	while (len < length)
	{
		unsigned_int32	code;

		int32 read = Text::ReadGlyphCodeUTF8(text, &code);
		if (code == 0)
		{
			break;
		}

		float x = w + glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		if (x > width)
		{
			break;
		}

		w = x;
		len += read;
		text += read;
	}

	if (actual)
	{
		*actual = FmaxZero(w);
	}

	return (len);
}

int32 C4::Font::GetBrokenTextLengthFitWidthUTF8(const char *text, float width, bool partialWord, float *actual, bool *hardBreak) const
{
	float	textWidth;

	bool hard = false;
	int32 length = 0;

	for (;;)
	{
		unsigned_int32	code;

		int32 read = Text::ReadGlyphCodeUTF8(&text[length], &code);
		if (code == 0)
		{
			break;
		}

		length += read;
		if ((code == 0x000A) || (code == 0x000D))
		{
			hard = true;
			break;
		}
	}

	if (hardBreak)
	{
		*hardBreak = hard;
	}

	if (length == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	int32 len = GetTextLengthFitWidthUTF8(text, length, width, &textWidth);
	if (len == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	if (len < length)
	{
		int32 l = len;
		float w = textWidth;

		for (; len > 0; len--)
		{
			const char *byte = &text[len - 1];
			unsigned_int32 code = *reinterpret_cast<const unsigned_int8 *>(byte);
			if (code < 0x0080)
			{
				if ((code == 0x0020) || (code == '-') || (code == '/'))
				{
					break;
				}
			}
			else if (code >= 0x00C0)
			{
				Text::ReadGlyphCodeUTF8(byte, &code);
			}

			textWidth -= glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		}

		if ((len == 0) && (partialWord))
		{
			len = l;
			textWidth = w;
		}
	}

	if (actual)
	{
		float spaceWidth = glyphData[GetGlyphIndex(0x0020)].glyphWidth + spacing;

		int32 pos = len - 1;
		while ((pos >= 0) && (text[pos] == 0x0020))
		{
			textWidth -= spaceWidth;
			pos--;
		}

		*actual = textWidth;
	}

	while (text[len] == 0x0020)
	{
		len++;
	}

	return (len);
}

int32 C4::Font::GetBrokenTextLengthFitWidthUTF8(const char *text, int32 length, float width, bool partialWord, float *actual, bool *hardBreak) const
{
	float	textWidth;

	int32 hard = false;
	int32 end = length;

	for (length = 0; length < end;)
	{
		unsigned_int32	code;

		int32 read = Text::ReadGlyphCodeUTF8(&text[length], &code);
		if (code == 0)
		{
			break;
		}

		length += read;
		if ((code == 0x000A) || (code == 0x000D))
		{
			hard = true;
			break;
		}
	}

	if (hardBreak)
	{
		*hardBreak = hard;
	}

	if (length == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	int32 len = GetTextLengthFitWidthUTF8(text, length, width, &textWidth);
	if (len == 0)
	{
		if (actual)
		{
			*actual = 0.0F;
		}

		return (0);
	}

	const GlyphData *glyphData = fontHeader->GetGlyphData();
	float spacing = fontHeader->fontSpacing;

	if (len < length)
	{
		int32 l = len;
		float w = textWidth;

		for (; len > 0; len--)
		{
			const char *byte = &text[len - 1];
			unsigned_int32 code = *reinterpret_cast<const unsigned_int8 *>(byte);
			if (code < 0x0080)
			{
				if ((code == 0x0020) || (code == '-') || (code == '/'))
				{
					break;
				}
			}
			else if (code >= 0x00C0)
			{
				Text::ReadGlyphCodeUTF8(byte, &code);
			}

			textWidth -= glyphData[GetGlyphIndex(code)].glyphWidth + spacing;
		}

		if ((len == 0) && (partialWord))
		{
			len = l;
			textWidth = w;
		}
	}

	if (actual)
	{
		float spaceWidth = glyphData[GetGlyphIndex(0x0020)].glyphWidth + spacing;

		int32 pos = len - 1;
		while ((pos >= 0) && (text[pos] == 0x0020))
		{
			textWidth -= spaceWidth;
			pos--;
		}

		*actual = textWidth;
	}

	while (text[len] == 0x0020)
	{
		len++;
	}

	return (len);
}

int32 C4::Font::CountBrokenTextLinesUTF8(const char *text, float width) const
{
	int32 length = Text::GetTextLength(text);
	int32 position = 0;
	int32 line = 0;

	while (position < length)
	{
		position += GetBrokenTextLengthFitWidth(&text[position], width);
		line++;
	}

	return (line);
}

// ZYUQURM
