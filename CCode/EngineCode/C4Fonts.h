 

#ifndef C4Fonts_h
#define C4Fonts_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4Resources.h"
#include "C4Attributes.h"


namespace C4
{
	class Font;


	//# \struct		GlyphData	Defines the properties of a single glyph within a font.
	//
	//# The $GlyphData$ structure defines the properties of a single glyph within a font.
	//
	//# \data	GlyphData
	//
	//# \also	$@FontHeader@$


	//# \member		GlyphData

	struct GlyphData
	{
		unsigned_int32		glyphFlags;				//## Flags for the glyph.
		float				glyphWidth;				//## The width of the glyph, in pixels.
		float				glyphHeight;			//## The height of the glyph, in pixels.
		float				glyphTop;				//## The distance from the top of the font to the top of the glyph, in pixels.
		Point2D				glyphTexcoord[2];		//## The texture coordinates corresponding to the top-left and bottom-right corners of the glyph within the font's texture.
	};


	//# \struct		FontHeader	Defines the properties of a font.
	//
	//# The $FontHeader$ structure defines the properties of a font.
	//
	//# \data	FontHeader
	//
	//# \also	$@GlyphData@$


	//# \member		FontHeader

	struct FontHeader
	{
		float				fontHeight;				//## The total height of the font, in pixels.
		float				fontBaseline;			//## The distance from the top of the font to the font's baseline, in pixels.
		float				fontSpacing;			//## The amount of extra horizontal space added between characters in the font, in pixels.
		float				fontLeading;			//## The amount of extra vertical space added between consecutive lines, in pixels.

		Vector2D			shadowOffset[2];

		int32				pageCount;
		int32				pageIndexOffset;
		int32				pageTableOffset;

		int32				glyphCount;
		int32				glyphDataOffset;

		unsigned_int32		auxiliaryDataSize;
		int32				auxiliaryDataOffset;

		unsigned_int8 *GetPageIndex(void)
		{
			return (reinterpret_cast<unsigned_int8 *>(this) + pageIndexOffset);
		}

		const unsigned_int8 *GetPageIndex(void) const
		{
			return (reinterpret_cast<const unsigned_int8 *>(this) + pageIndexOffset);
		}

		unsigned_int16 *GetPageTable(void)
		{
			return (reinterpret_cast<unsigned_int16 *>(reinterpret_cast<char *>(this) + pageTableOffset));
		}

		const unsigned_int16 *GetPageTable(void) const
		{
			return (reinterpret_cast<const unsigned_int16 *>(reinterpret_cast<const char *>(this) + pageTableOffset));
		}

		GlyphData *GetGlyphData(void)
		{
			return (reinterpret_cast<GlyphData *>(reinterpret_cast<char *>(this) + glyphDataOffset));
		}

		const GlyphData *GetGlyphData(void) const
		{
			return (reinterpret_cast<const GlyphData *>(reinterpret_cast<const char *>(this) + glyphDataOffset));
		}
	};


	class FontResource : public Resource<FontResource> 
	{
		friend class Resource<FontResource>;
 
		private:
 
			static C4API ResourceDescriptor		descriptor;

			Font		*owningFont; 

			~FontResource(); 
 
		public:

			FontResource(const char *name, ResourceCatalog *catalog);
 
			Font *GetOwningFont(void) const
			{
				return (owningFont);
			}

			void SetOwningFont(Font *font)
			{
				owningFont = font;
			}

			int32 GetVersion(void) const
			{
				return (static_cast<const int32 *>(GetData())[1]);
			}

			const FontHeader *GetFontHeader(void) const
			{
				return (reinterpret_cast<const FontHeader *>(static_cast<const int32 *>(GetData()) + 2));
			}
	};


	//# \class	Font	Encapsulates a text font.
	//
	//# The $Font$ class encapsulates a text font.
	//
	//# \def	class Font : public Shared
	//
	//# \ctor	Font(FontResource *resource, const char *name);
	//
	//# The $Font$ constructor has private access. The $@Font::Get@$ function should be used to
	//# create font objects.
	//
	//# \desc
	//# The $Font$ object encapsulates the resources needed to manage a text font. New font objects are created by
	//# calling the $@Font::Get@$ function.
	//
	//# \base	Utilities/Shared	Font objects are reference counted.


	//# \function	Font::Get		Returns a new reference to a font object.
	//
	//# \proto	static Font *Get(const char *name);
	//
	//# \param	name	The name of the font resource.
	//
	//# \desc
	//# The $Get$ function returns a new reference to the font object specified by the $name$ parameter. If the font
	//# resource has already been loaded, then its reference count is incremented and a pointer to the existing object
	//# is returned. If the font specified by the $name$ parameter does not exist, then a pointer to the default font
	//# is returned.
	//#
	//# The $@Utilities/Shared::Release@$ function must be called for the returned font object to balance each call to
	//# the $Get$ function. A convenient way to accomplish this automatically is to use the $@AutoReleaseFont@$ class.
	//
	//# \also	$@AutoReleaseFont@$
	//# \also	$@Utilities/Shared::Release@$


	//# \function	Font::GetFontHeader		Returns the font header.
	//
	//# \proto	const FontHeader *GetFontHeader(void) const;
	//
	//# \desc
	//# The $GetFontHeader$ function returns a pointer to the $@FontHeader@$ structure describing a particular font.
	//
	//# \also	$@FontHeader@$
	//# \also	$@Font::GetLineSpacing@$


	//# \function	Font::GetLineSpacing		Returns the font's machine line spacing.
	//
	//# \proto	float GetLineSpacing(void) const;
	//
	//# \desc
	//# The $GetLineSpacing$ function returns the machine line spacing for a font. The line spacing is equal
	//# to the sum of the font height and the font leading.
	//
	//# \also	$@Font::GetFontHeader@$


	//# \function	Font::GetTextWidth		Returns the width of a text string.
	//
	//# \proto	float GetTextWidth(const char *text) const;
	//# \proto	float GetTextWidth(const char *text, int32 length) const;
	//
	//# \param	text	The text string to measure.
	//# \param	length	The maximum number of characters to include in the width.
	//
	//# \desc
	//# The $GetTextWidth$ function returns the width, in pixels, that the text string specified by the $text$
	//# parameter would occupy if it were rendered in the font for which the function is called. The total width
	//# accounts for the width of the individual characters in the string and the spacing between them.
	//
	//# \also	$@Font::GetFontHeader@$


	class Font : public Shared
	{
		private:

			FontResource				*fontResource;
			const FontHeader			*fontHeader;

			DiffuseTextureAttribute		textureAttribute;

			Font(FontResource *resource, const char *name);
			~Font();

		public:

			const FontHeader *GetFontHeader(void) const
			{
				return (fontHeader);
			}

			float GetLineSpacing(void) const
			{
				return (fontHeader->fontHeight + fontHeader->fontLeading);
			}

			int32 GetGlyphIndex(unsigned_int32 code) const
			{
				return (fontHeader->GetPageTable()[(fontHeader->GetPageIndex()[(code >> 8) & 0x00FF] << 8) + (code & 0x00FF)]);
			}

			float GetGlyphWidth(unsigned_int32 code) const
			{
				return (fontHeader->GetGlyphData()[GetGlyphIndex(code)].glyphWidth);
			}

			float GetFullGlyphWidth(unsigned_int32 code) const
			{
				return (fontHeader->GetGlyphData()[GetGlyphIndex(code)].glyphWidth + fontHeader->fontSpacing);
			}

			const DiffuseTextureAttribute *GetTextureAttribute(void) const
			{
				return (&textureAttribute);
			}

			C4API static Font *Get(const char *name);

			C4API float GetTextWidth(const char *text) const;
			C4API float GetTextWidth(const char *text, int32 length) const;
			C4API int32 GetTextLengthFitWidth(const char *text, float width, float *actual = nullptr) const;
			C4API int32 GetTextLengthFitWidth(const char *text, int32 length, float width, float *actual = nullptr) const;
			C4API int32 GetBrokenTextLengthFitWidth(const char *text, float width, bool partialWord = true, float *actual = nullptr, bool *hardBreak = nullptr) const;
			C4API int32 GetBrokenTextLengthFitWidth(const char *text, int32 length, float width, bool partialWord = true, float *actual = nullptr, bool *hardBreak = nullptr) const;
			C4API int32 CountBrokenTextLines(const char *text, float width) const;

			C4API float GetTextWidthUTF8(const char *text) const;
			C4API float GetTextWidthUTF8(const char *text, int32 length) const;
			C4API int32 GetTextLengthFitWidthUTF8(const char *text, float width, float *actual = nullptr) const;
			C4API int32 GetTextLengthFitWidthUTF8(const char *text, int32 length, float width, float *actual = nullptr) const;
			C4API int32 GetBrokenTextLengthFitWidthUTF8(const char *text, float width, bool partialWord = true, float *actual = nullptr, bool *hardBreak = nullptr) const;
			C4API int32 GetBrokenTextLengthFitWidthUTF8(const char *text, int32 length, float width, bool partialWord = true, float *actual = nullptr, bool *hardBreak = nullptr) const;
			C4API int32 CountBrokenTextLinesUTF8(const char *text, float width) const;
	};


	//# \class	AutoReleaseFont		A helper class that wraps a pointer to a $Font$ object.
	//
	//# \def	class AutoReleaseFont : public AutoRelease<Font>
	//
	//# \ctor	AutoReleaseFont(const char *name);
	//
	//# \param	name	The name of the font resource.
	//
	//# \desc
	//# The $AutoReleaseFont$ class is a helper class that wraps a pointer to a $Font$ object.
	//# When an $AutoReleaseFont$ is constructed, it calls the $@Font::Get@$ function to obtain
	//# a pointer to the font specified by the $name$ parameter. It then wraps that pointer
	//# in an $@Utilities/AutoRelease@$ object. When an $AutoReleaseFont$ object goes out
	//# of scope, it automatically releases the font object that it wraps.
	//
	//# \base	Utilities/AutoRelease<Font>		Font objects are reference counted.
	//
	//# \also	$@Font::Get@$


	class AutoReleaseFont : public AutoRelease<Font>
	{
		public:

			AutoReleaseFont(const char *name) : AutoRelease<Font>(Font::Get(name))
			{
			}
	};
}


#endif

// ZYUQURM
