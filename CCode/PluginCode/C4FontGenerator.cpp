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


#include "C4FontGenerator.h"
#include "C4TextureImporter.h"


using namespace C4;


namespace
{
	enum
	{
		kGlyphPadding		= 2
	};


	enum
	{
		kFontImportBold		= 1 << 0,
		kFontImportItalic	= 1 << 1
	};
}


FontGenerator *C4::TheFontGenerator = nullptr;


ResourceDescriptor RangesResource::descriptor("rng");


SharedVertexBuffer GlyphWidget::vertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);
SharedVertexBuffer GlyphWidget::indexBuffer(kVertexBufferIndex | kVertexBufferStatic);


const UnicodeBlock FontGeneratorWindow::unicodeBlockTable[kUnicodeBlockCount] =
{
	{0x0020, 0x007E, "Basic Latin"},
	{0x00A0, 0x00FF, "Latin-1 Supplement"},
	{0x0100, 0x017F, "Latin Extended-A"},
	{0x0180, 0x024F, "Latin Extended-B"},
	{0x0250, 0x02AF, "IPA Extensions"},
	{0x02B0, 0x02FF, "Spacing Modifier Letter"},
	{0x0300, 0x036F, "Combining Diacritical Mark"},
	{0x0370, 0x03FF, "Greek and Coptic"},
	{0x0400, 0x04FF, "Cyrillic"},
	{0x0500, 0x052F, "Cyrillic Supplement"},
	{0x0530, 0x058F, "Armenian"},
	{0x0590, 0x05FF, "Hebrew"},
	{0x0600, 0x06FF, "Arabic"},
	{0x0700, 0x074F, "Syriac"},
	{0x0750, 0x077F, "Arabic Supplement"},
	{0x0780, 0x07BF, "Thaana"},
	{0x07C0, 0x07FF, "NKo"},
	{0x0800, 0x083F, "Samaritan"},
	{0x0900, 0x097F, "Devanagari"},
	{0x0980, 0x09FF, "Bengali"},
	{0x0A00, 0x0A7F, "Gurmukhi"},
	{0x0A80, 0x0AFF, "Gujarati"},
	{0x0B00, 0x0B7F, "Oriya"},
	{0x0B80, 0x0BFF, "Tamil"},
	{0x0C00, 0x0C7F, "Telugu"},
	{0x0C80, 0x0CFF, "Kannada"},
	{0x0D00, 0x0D7F, "Malayalam"},
	{0x0D80, 0x0DFF, "Sinhala"},
	{0x0E00, 0x0E7F, "Thai"},
	{0x0E80, 0x0EFF, "Lao"},
	{0x0F00, 0x0FFF, "Tibetan"},
	{0x1000, 0x109F, "Myanmar"},
	{0x10A0, 0x10FF, "Georgian"},
	{0x1100, 0x11FF, "Hangul Jamo"},
	{0x1200, 0x137F, "Ethiopic"},
	{0x1380, 0x139F, "Ethiopic Supplement"},
	{0x13A0, 0x13FF, "Cherokee"},
	{0x1400, 0x167F, "Unified Canadian Aboriginal Syllabics"},
	{0x1680, 0x169F, "Ogham"},
	{0x16A0, 0x16FF, "Runic"},
	{0x1700, 0x171F, "Tagalog"},
	{0x1720, 0x173F, "Hanunoo"},
	{0x1740, 0x175F, "Buhid"},
	{0x1760, 0x177F, "Tagbanwa"},
	{0x1780, 0x17FF, "Khmer"},
	{0x1800, 0x18AF, "Mongolian"},
	{0x18B0, 0x18FF, "Unified Canadian Aboriginal Syllabics Extended"},
	{0x1900, 0x194F, "Limbu"},
	{0x1950, 0x197F, "Tai Le"},
	{0x1980, 0x19DF, "New Tai Lue"},
	{0x19E0, 0x19FF, "Khmer Symbols"},
	{0x1A00, 0x1A1F, "Buginese"},
	{0x1A20, 0x1AAF, "Tai Tham"},
	{0x1B00, 0x1B7F, "Balinese"},
	{0x1B80, 0x1BBF, "Sundanese"},
	{0x1C00, 0x1C4F, "Lepcha"},
	{0x1C50, 0x1C7F, "Ol Chiki"},
	{0x1CD0, 0x1CFF, "Vedic Extensions"},
	{0x1D00, 0x1D7F, "Phonetic Extensions"},
	{0x1D80, 0x1DBF, "Phonetic Extensions Supplement"},
	{0x1DC0, 0x1DFF, "Combining Diacritical Marks Supplement"},
	{0x1E00, 0x1EFF, "Latin Extended Additional"},
	{0x1F00, 0x1FFF, "Greek Extended"},
	{0x2000, 0x206F, "General Punctuation"},
	{0x2070, 0x209F, "Superscripts and Subscripts"},
	{0x20A0, 0x20CF, "Currency Symbols"}, 
	{0x20D0, 0x20FF, "Combining Diacritical Marks for Symbols"},
	{0x2100, 0x214F, "Letterlike Symbols"},
	{0x2150, 0x218F, "Number Forms"}, 
	{0x2190, 0x21FF, "Arrows"},
	{0x2200, 0x22FF, "Mathematical Operators"}, 
	{0x2300, 0x23FF, "Miscellaneous Technical"},
	{0x2400, 0x243F, "Control Pictures"},
	{0x2440, 0x245F, "Optical Character Recognition"}, 
	{0x2460, 0x24FF, "Enclosed Alphanumerics"},
	{0x2500, 0x257F, "Box Drawing"}, 
	{0x2580, 0x259F, "Block Elements"}, 
	{0x25A0, 0x25FF, "Geometric Shapes"},
	{0x2600, 0x26FF, "Miscellaneous Symbols"},
	{0x2700, 0x27BF, "Dingbats"},
	{0x27C0, 0x27EF, "Miscellaneous Mathematical Symbols-A"}, 
	{0x27F0, 0x27FF, "Supplemental Arrows-A"},
	{0x2800, 0x28FF, "Braille Patterns"},
	{0x2900, 0x297F, "Supplemental Arrows-B"},
	{0x2980, 0x29FF, "Miscellaneous Mathematical Symbols-B"},
	{0x2A00, 0x2AFF, "Supplemental Mathematical Operators"},
	{0x2B00, 0x2BFF, "Miscellaneous Symbols and Arrows"},
	{0x2C00, 0x2C5F, "Glagolitic"},
	{0x2C60, 0x2C7F, "Latin Extended-C"},
	{0x2C80, 0x2CFF, "Coptic"},
	{0x2D00, 0x2D2F, "Georgian Supplement"},
	{0x2D30, 0x2D7F, "Tifinagh"},
	{0x2D80, 0x2DDF, "Ethiopic Extended"},
	{0x2DE0, 0x2DFF, "Cyrillic Extended-A"},
	{0x2E00, 0x2E7F, "Supplemental Punctuation"},
	{0x2E80, 0x2EFF, "CJK Radicals Supplement"},
	{0x2F00, 0x2FDF, "Kangxi Radicals"},
	{0x2FF0, 0x2FFF, "Ideographic Description Characters"},
	{0x3000, 0x303F, "CJK Symbols and Punctuation"},
	{0x3040, 0x309F, "Hiragana"},
	{0x30A0, 0x30FF, "Katakana"},
	{0x3100, 0x312F, "Bopomofo"},
	{0x3130, 0x318F, "Hangul Compatibility Jamo"},
	{0x3190, 0x319F, "Kanbun"},
	{0x31A0, 0x31BF, "Bopomofo Extended"},
	{0x31C0, 0x31EF, "CJK Strokes"},
	{0x31F0, 0x31FF, "Katakana Phonetic Extensions"},
	{0x3200, 0x32FF, "Enclosed CJK Letters and Months"},
	{0x3300, 0x33FF, "CJK Compatibility"},
	{0x3400, 0x4DBF, "CJK Unified Ideographs Extension A"},
	{0x4DC0, 0x4DFF, "Yijing Hexagram Symbols"},
	{0x4E00, 0x9FFF, "CJK Unified Ideographs"},
	{0xA000, 0xA48F, "Yi Syllables"},
	{0xA490, 0xA4CF, "Yi Radicals"},
	{0xA4D0, 0xA4FF, "Lisu"},
	{0xA500, 0xA63F, "Vai"},
	{0xA640, 0xA69F, "Cyrillic Extended-B"},
	{0xA6A0, 0xA6FF, "Bamum"},
	{0xA700, 0xA71F, "Modifier Tone Letters"},
	{0xA720, 0xA7FF, "Latin Extended-D"},
	{0xA800, 0xA82F, "Syloti Nagri"},
	{0xA830, 0xA83F, "Common Indic Number Forms"},
	{0xA840, 0xA87F, "Phags-pa"},
	{0xA880, 0xA8DF, "Saurashtra"},
	{0xA8E0, 0xA8FF, "Devanagari Extended"},
	{0xA900, 0xA92F, "Kayah Li"},
	{0xA930, 0xA95F, "Rejang"},
	{0xA960, 0xA97F, "Hangul Jamo Extended-A"},
	{0xA980, 0xA9DF, "Javanese"},
	{0xAA00, 0xAA5F, "Cham"},
	{0xAA60, 0xAA7F, "Myanmar Extended-A"},
	{0xAA80, 0xAADF, "Tai Viet"},
	{0xABC0, 0xABFF, "Meetei Mayek"},
	{0xAC00, 0xD7AF, "Hangul Syllables"},
	{0xD7B0, 0xD7FF, "Hangul Jamo Extended-B"},
	{0xD800, 0xDB7F, "High Surrogates"},
	{0xDB80, 0xDBFF, "High Private Use Surrogates"},
	{0xDC00, 0xDFFF, "Low Surrogates"},
	{0xE000, 0xF8FF, "Private Use Area"},
	{0xF900, 0xFAFF, "CJK Compatibility Ideographs"},
	{0xFB00, 0xFB4F, "Alphabetic Presentation Forms"},
	{0xFB50, 0xFDFF, "Arabic Presentation Forms-A"},
	{0xFE00, 0xFE0F, "Variation Selectors"},
	{0xFE10, 0xFE1F, "Vertical Forms"},
	{0xFE20, 0xFE2F, "Combining Half Marks"},
	{0xFE30, 0xFE4F, "CJK Compatibility Forms"},
	{0xFE50, 0xFE6F, "Small Form Variants"},
	{0xFE70, 0xFEFF, "Arabic Presentation Forms-B"},
	{0xFF00, 0xFFEF, "Halfwidth and Fullwidth Forms"}
};


C4::Plugin *CreatePlugin(void)
{
	return (new FontGenerator);
}


RangesResource::RangesResource(const char *name, ResourceCatalog *catalog) : Resource<RangesResource>(name, catalog)
{
}

RangesResource::~RangesResource()
{
}


SystemFont::SystemFont(const SystemFontData *fontData, int32 size)
{
	validPage[0] = true;
	for (machine a = 1; a < 256; a++)
	{
		validPage[a] = false;
	}

	#if C4WINDOWS

		TEXTMETRICA		textMetrics;

		deviceContext = CreateCompatibleDC(nullptr);
		fontObject = CreateFontA(size, 0, 0, 0, (fontData->fontFlags & kFontImportBold) ? FW_BOLD : FW_NORMAL, (fontData->fontFlags & kFontImportItalic) != 0, false, false, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontData->fontName);
		defaultFont = SelectObject(deviceContext, fontObject);

		GetTextMetricsA(deviceContext, &textMetrics);

		fontHeight = textMetrics.tmHeight;
		fontAscent = textMetrics.tmAscent;

		DWORD rangeSize = GetFontUnicodeRanges(deviceContext, nullptr);
		glyphSet = reinterpret_cast<GLYPHSET *>(new char[rangeSize]);
		glyphSet->cbThis = rangeSize;
		GetFontUnicodeRanges(deviceContext, glyphSet);

		int32 count = glyphSet->cRanges;
		for (machine a = 0; a < count; a++)
		{
			unsigned_int32 first = glyphSet->ranges[a].wcLow;
			if (first < 0xFFF0)
			{
				unsigned_int32 last = glyphSet->ranges[a].cGlyphs + first - 1;

				first >>= 8;
				last >>= 8;
				for (unsigned_machine b = first; b <= last; b++)
				{
					validPage[b] = true;
				}
			}
		}

	#elif C4MACOS

		coreTextFont = CTFontCreateWithFontDescriptor(fontData->fontDescriptor, (CGFloat) size, nullptr);

		fontAscent = (int32) CTFontGetAscent(coreTextFont);
		fontHeight = fontAscent + (int32) CTFontGetDescent(coreTextFont);

		glyphTable = new CGGlyph[65536];

		UniChar *code = new UniChar[65536];
		for (machine a = 0; a < 65536; a++)
		{
			code[a] = (UniChar) a;
		}

		CTFontGetGlyphsForCharacters(coreTextFont, code, glyphTable, 65536);
		delete[] code;

		for (machine a = 0x0100; a < 0xFFF0; a++)
		{
			if (glyphTable[a] != 0)
			{
				validPage[a >> 8] = true;
			}
		}

		CGRect rect = CTFontGetBoundingBox(coreTextFont);
		int32 left = (int32) Floor(CGRectGetMinX(rect));
		int32 right = (int32) Ceil(CGRectGetMaxX(rect));

		contextWidth = right - left;
		contextRowBytes = (contextWidth + 3) & ~3;

		imageStorage = new unsigned_int8[contextRowBytes * fontHeight];
		CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
		graphicsContext = CGBitmapContextCreate(imageStorage, contextWidth, fontHeight, 8, contextRowBytes, colorSpace, kCGImageAlphaNone);
		CGColorSpaceRelease(colorSpace);

		CGContextSetGrayFillColor(graphicsContext, 1.0F, 1.0F);

		CGFontRef graphicsFont = CTFontCopyGraphicsFont(coreTextFont, nullptr);
		CGContextSetFont(graphicsContext, graphicsFont);
		CGContextSetFontSize(graphicsContext, (CGFloat) size);
		CGFontRelease(graphicsFont);

		CTFontGetAdvancesForGlyphs(coreTextFont, kCTFontHorizontalOrientation, &glyphTable[0x0020], &spaceWidth, 1);

	#endif
}

SystemFont::~SystemFont()
{
	#if C4WINDOWS

		delete[] reinterpret_cast<char *>(glyphSet);

		SelectObject(deviceContext, defaultFont);
		DeleteObject(fontObject);
		DeleteDC(deviceContext);

	#elif C4MACOS

		CGContextRelease(graphicsContext);
		CFRelease(coreTextFont);

		delete[] imageStorage;
		delete[] glyphTable;

	#endif
}

bool SystemFont::ValidBlock(const UnicodeBlock *block) const
{
	#if C4WINDOWS

		int32 count = glyphSet->cRanges;
		for (machine a = 0; a < count; a++)
		{
			unsigned_int32 first = glyphSet->ranges[a].wcLow;
			unsigned_int32 last = glyphSet->ranges[a].cGlyphs + first - 1;
			if ((first <= block->lastCode) && (last >= block->firstCode))
			{
				return (true);
			}
		}

	#elif C4MACOS

		unsigned_int32 last = block->lastCode;
		for (unsigned_machine a = block->firstCode; a <= last; a++)
		{
			if (glyphTable[a] != 0)
			{
				return (true);
			}
		}

	#endif

	return (false);
}

bool SystemFont::ValidGlyph(unsigned_int32 code) const
{
	if ((code < 0x20U) || (code - 0x7F < 0x21U) || (code >= 0xFFF0))
	{
		return (false);
	}

	#if C4WINDOWS

		int32 count = glyphSet->cRanges;
		for (machine a = 0; a < count; a++)
		{
			unsigned_int32 first = glyphSet->ranges[a].wcLow;
			unsigned_int32 length = glyphSet->ranges[a].cGlyphs;
			if (code - first < length)
			{
				return (true);
			}
		}

	#elif C4MACOS

		return (glyphTable[code] != 0);

	#endif

	return (false);
}

bool SystemFont::GetGlyphImage(unsigned_int32 code, GlyphMetric *metric, Color2C **image) const
{
	#if C4WINDOWS

		GLYPHMETRICS	glyphMetrics;
		MAT2			matrix;

		*reinterpret_cast<Fixed *>(&matrix.eM11) = 0x00010000;
		*reinterpret_cast<Fixed *>(&matrix.eM12) = 0x00000000;
		*reinterpret_cast<Fixed *>(&matrix.eM21) = 0x00000000;
		*reinterpret_cast<Fixed *>(&matrix.eM22) = 0x00010000;

		DWORD size = GetGlyphOutlineW(deviceContext, code, GGO_GRAY8_BITMAP, &glyphMetrics, 0, nullptr, &matrix);
		if (size != GDI_ERROR)
		{
			if ((code & 0xFF7F) == 0x0020)
			{
				int32 w = glyphMetrics.gmCellIncX;
				metric->width = w;
				metric->height = 1;
				metric->xoffset = 0;
				metric->yoffset = 0;

				if (image)
				{
					Color2C *img = new Color2C[w];
					*image = img;

					for (machine i = 0; i < w; i++)
					{
						img[i].Set(0, 0);
					}
				}

				return (true);
			}
			else if (size != 0)
			{
				int32 w = glyphMetrics.gmBlackBoxX;
				int32 h = glyphMetrics.gmBlackBoxY;

				metric->width = w;
				metric->height = h;
				metric->xoffset = glyphMetrics.gmptGlyphOrigin.x;
				metric->yoffset = fontAscent - glyphMetrics.gmptGlyphOrigin.y;

				if (image)
				{
					unsigned_int8 *data = new unsigned_int8[size];
					GetGlyphOutlineW(deviceContext, code, GGO_GRAY8_BITMAP, &glyphMetrics, size, data, &matrix);

					Color2C *img = new Color2C[w * h];
					*image = img;

					const unsigned_int8 *src = data;
					for (machine j = 0; j < h; j++)
					{
						for (machine i = 0; i < w; i++)
						{
							unsigned_int32 value = (src[i] * 255U) >> 6;
							img[i].Set(255, value);
						}

						src += (w + 3) & ~3;
						img += w;
					}

					delete[] data;
				}

				return (true);
			}
		}

	#elif C4MACOS

		if ((code & 0xFF7F) == 0x0020)
		{
			int32 w = (int32) spaceWidth.width;

			metric->width = w;
			metric->height = 1;
			metric->xoffset = 0;
			metric->yoffset = 0;

			if (image)
			{
				Color2C *img = new Color2C[w];
				*image = img;

				for (machine i = 0; i < w; i++)
				{
					img[i].Set(0, 0);
				}
			}

			return (true);
		}
		else
		{
			MemoryMgr::ClearMemory(imageStorage, contextRowBytes * fontHeight);

			CGGlyph glyph = glyphTable[code];
			CGPoint position = {0.0F, (CGFloat) (fontHeight - fontAscent)};
			CTFontDrawGlyphs(coreTextFont, &glyph, &position, 1, graphicsContext);

			machine top = 0;
			const unsigned_int8 *src = imageStorage;
			for (; top < fontHeight; top++)
			{
				unsigned_int8 accum = src[0];
				for (machine i = 1; i < contextWidth; i++)
				{
					accum |= src[i];
				}

				if (accum != 0)
				{
					break;
				}

				src += contextRowBytes;
			}

			if (top < fontHeight)
			{
				machine bottom = fontHeight - 1;
				src = imageStorage + contextRowBytes * bottom;
				for (; bottom >= 0; bottom--)
				{
					unsigned_int8 accum = src[0];
					for (machine i = 1; i < contextWidth; i++)
					{
						accum |= src[i];
					}

					if (accum != 0)
					{
						break;
					}

					src -= contextRowBytes;
				}

				machine left = 0;
				src = imageStorage;
				for (; left < contextWidth; left++)
				{
					unsigned accum = src[contextRowBytes * top];
					for (machine j = top + 1; j <= bottom; j++)
					{
						accum |= src[contextRowBytes * j];
					}

					if (accum != 0)
					{
						break;
					}

					src++;
				}

				machine right = contextWidth - 1;
				src = imageStorage + right;
				for (; right >= 0; right--)
				{
					unsigned accum = src[contextRowBytes * top];
					for (machine j = top + 1; j <= bottom; j++)
					{
						accum |= src[contextRowBytes * j];
					}

					if (accum != 0)
					{
						break;
					}

					src--;
				}

				int32 w = right - left + 1;
				int32 h = bottom - top + 1;

				metric->width = w;
				metric->height = h;
				metric->xoffset = 0;
				metric->yoffset = top;

				if (image)
				{
					Color2C *img = new Color2C[w * h];
					*image = img;

					src = imageStorage + contextRowBytes * top + left;
					for (machine j = 0; j < h; j++)
					{
						for (machine i = 0; i < w; i++)
						{
							img[i].Set(255, src[i]);
						}

						src += contextRowBytes;
						img += w;
					}
				}

				return (true);
			}
		}

	#endif

	return (false);
}


GlyphWidget::GlyphWidget(const Vector2D& size, unsigned_int32 code, Color2C *image, int32 glyphWidth, int32 glyphHeight) :
		ImageWidget(kWidgetGlyph, size),
		diffuseAttribute(K::white, kAttributeMutable),
		backgroundRenderable(kRenderIndexedTriangles)
{
	static const GlyphVertex backgroundVertex[12] =
	{
		{Point2D(0.0F, 0.0F), Color4C(255, 255, 255, 255)}, {Point2D(0.0F, 32.0F), Color4C(255, 255, 255, 255)},
		{Point2D(32.0F, 32.0F), Color4C(255, 255, 255, 255)}, {Point2D(32.0F, 0.0F), Color4C(255, 255, 255, 255)},
		{Point2D(0.0F, 0.0F), Color4C(0, 0, 0, 255)}, {Point2D(0.0F, 32.0F), Color4C(0, 0, 0, 255)},
		{Point2D(32.0F, 32.0F), Color4C(0, 0, 0, 255)}, {Point2D(32.0F, 0.0F), Color4C(0, 0, 0, 255)},
		{Point2D(-1.0F, -1.0F), Color4C(0, 0, 0, 255)}, {Point2D(-1.0F, 33.0F), Color4C(0, 0, 0, 255)},
		{Point2D(33.0F, 33.0F), Color4C(0, 0, 0, 255)}, {Point2D(33.0F, -1.0F), Color4C(0, 0, 0, 255)}
	};

	static const Triangle backgroundTriangle[10] =
	{
		{{0, 1, 2}}, {{0, 2, 3}},
		{{4, 9, 5}}, {{4, 8, 9}},
		{{5, 10, 6}}, {{5, 9, 10}},
		{{6, 11, 7}}, {{6, 10, 11}},
		{{7, 8, 4}}, {{7, 11, 8}}
	};

	glyphCode = code;
	glyphImage = image;

	if (vertexBuffer.Retain() == 1)
	{
		vertexBuffer.Establish(sizeof(GlyphVertex) * 12, backgroundVertex);
	}

	if (indexBuffer.Retain() == 1)
	{
		indexBuffer.Establish(sizeof(Triangle) * 10, backgroundTriangle);
	}

	textureHeader.textureType = kTextureRectangle;
	textureHeader.textureFlags = kTextureForceHighQuality;
	textureHeader.colorSemantic = kTextureSemanticNone;
	textureHeader.alphaSemantic = kTextureSemanticTransparency;
	textureHeader.imageFormat = kTextureLA8;
	textureHeader.imageWidth = glyphWidth;
	textureHeader.imageHeight = glyphHeight;
	textureHeader.imageDepth = 1;
	textureHeader.wrapMode[0] = kTextureClamp;
	textureHeader.wrapMode[1] = kTextureClamp;
	textureHeader.wrapMode[2] = kTextureClamp;
	textureHeader.mipmapCount = 1;
	textureHeader.mipmapDataOffset = 0;
	textureHeader.auxiliaryDataSize = 0;
	textureHeader.auxiliaryDataOffset = 0;

	SetTexture(0, &textureHeader, image);

	SetWidgetUsage(kWidgetTrackInhibit);
	SetImageScale(Vector2D(1.0F, -1.0F));
	SetImageOffset(Vector2D(0.0F, 1.0F));
}

GlyphWidget::~GlyphWidget()
{
	delete[] glyphImage;

	indexBuffer.Release();
	vertexBuffer.Release();
}

bool GlyphWidget::CalculateBoundingBox(Box2D *box) const
{
	box->min.Set(0.0F, 0.0F);
	box->max.Set(32.0F, 32.0F);
	return (true);
}

void GlyphWidget::Preprocess(void)
{
	ImageWidget::Preprocess();

	InitRenderable(&backgroundRenderable);
	backgroundRenderable.SetVertexCount(12);
	backgroundRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(GlyphVertex));
	backgroundRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	backgroundRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 1);

	backgroundRenderable.SetPrimitiveCount(10);
	backgroundRenderable.SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	attributeList.Append(&diffuseAttribute);
	backgroundRenderable.SetMaterialAttributeList(&attributeList);
}

void GlyphWidget::Build(void)
{
	if (GetWidgetState() & kWidgetHilited)
	{
		diffuseAttribute.SetDiffuseColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite));
	}
	else
	{
		diffuseAttribute.SetDiffuseColor(K::white);
	}

	ImageWidget::Build();
}

void GlyphWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&backgroundRenderable);
	ImageWidget::Render(renderList);
}

void GlyphWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseDown)
	{
		Activate();
	}
}


FontGeneratorWindow::FontGeneratorWindow(const SystemFontData *fontData, int32 size) :
		Window("FontGenerator/Window"),
		systemFont(fontData, size),
		configurationObserver(this, &FontGeneratorWindow::HandleConfigurationEvent),
		glyphObserver(this, &FontGeneratorWindow::HandleGlyphEvent)
{
	TheFontGenerator->AddFontGeneratorWindow(this);

	glyphIncluded = nullptr;
}

FontGeneratorWindow::~FontGeneratorWindow()
{
	delete[] glyphIncluded;
}

int32 FontGeneratorWindow::GetSettingCount(void) const
{
	return (10);
}

Setting *FontGeneratorWindow::GetSetting(int32 index) const
{
	const StringTable *table = TheFontGenerator->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('FGEN', 'FSET'));
		return (new HeadingSetting('FSET', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('FGEN', 'NAME'));
		return (new TextSetting('NAME', outputFontName, title, kMaxResourceNameLength));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('FGEN', 'SPAC'));
		return (new TextSetting('SPAC', Text::IntegerToString(fontSpacing), title, 3, &EditTextWidget::SignedNumberFilter));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('FGEN', 'LEAD'));
		return (new TextSetting('LEAD', Text::IntegerToString(fontLeading), title, 3, &EditTextWidget::SignedNumberFilter));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('FGEN', 'SSET'));
		return (new HeadingSetting('SSET', title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('FGEN', 'SHAD'));
		return (new BooleanSetting('SHAD', shadowFlag, title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('FGEN', 'STYP'));
		MenuSetting *menu = new MenuSetting('STYP', shadowBlurRadius, title, 3);
		menu->SetMenuItemString(0, table->GetString(StringID('FGEN', 'STYP', 'SHRP')));
		menu->SetMenuItemString(1, table->GetString(StringID('FGEN', 'STYP', 'SOFT')));
		menu->SetMenuItemString(2, table->GetString(StringID('FGEN', 'STYP', 'SFTR')));
		return (menu);
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('FGEN', 'DARK'));
		return (new IntegerSetting('DARK', shadowDarkness, title, 1, 255, 1));
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('FGEN', 'SHDX'));
		return (new IntegerSetting('SHDX', shadowOffsetX, title, -4, 4, 1));
	}

	if (index == 9)
	{
		const char *title = table->GetString(StringID('FGEN', 'SHDY'));
		return (new IntegerSetting('SHDY', shadowOffsetY, title, -4, 4, 1));
	}

	return (nullptr);
}

void FontGeneratorWindow::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NAME')
	{
		outputFontName = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'SPAC')
	{
		fontSpacing = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'LEAD')
	{
		fontLeading = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'SHAD')
	{
		shadowFlag = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
	else if (identifier == 'STYP')
	{
		shadowBlurRadius = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else if (identifier == 'DARK')
	{
		shadowDarkness = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'SHDX')
	{
		shadowOffsetX = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'SHDY')
	{
		shadowOffsetY = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
	}
}

void FontGeneratorWindow::Preprocess(void)
{
	Window::Preprocess();

	generateButton = static_cast<PushButtonWidget *>(FindWidget("Generate"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));
	loadButton = static_cast<PushButtonWidget *>(FindWidget("Load"));
	saveButton = static_cast<PushButtonWidget *>(FindWidget("Save"));
	includeButton = static_cast<PushButtonWidget *>(FindWidget("Include"));
	excludeButton = static_cast<PushButtonWidget *>(FindWidget("Exclude"));

	rangeText = static_cast<TextWidget *>(FindWidget("Range"));
	selectionText = static_cast<TextWidget *>(FindWidget("Selection"));

	blockList = static_cast<ListWidget *>(FindWidget("List"));
	Widget *border = FindWidget("Border");

	tableGroup = new Widget;
	tableGroup->SetWidgetPosition(border->GetWidgetPosition());
	AppendNewSubnode(tableGroup);

	int32 blockCount = 0;
	for (machine a = 0; a < kUnicodeBlockCount; a++)
	{
		const UnicodeBlock *block = &unicodeBlockTable[a];
		if (systemFont.ValidBlock(block))
		{
			blockRemapTable[blockCount++] = (unsigned_int8) a;
			blockList->AppendListItem(block->blockName);
		}
	}

	glyphPage = 0;
	selectionBegin = 0;
	selectionEnd = 0;
	rebuildFlag = true;
	usedBlockCount = blockCount;

	glyphIncluded = new bool[65536];
	MemoryMgr::ClearMemory(glyphIncluded, 65536);

	for (machine a = 0x0020; a < 0x007F; a++)
	{
		glyphIncluded[a] = true;
	}

	for (machine a = 0x00A0; a < 0x0100; a++)
	{
		glyphIncluded[a] = true;
	}

	outputFontName[0] = 0;
	fontSpacing = 1;
	fontLeading = 1;

	shadowFlag = false;
	shadowBlurRadius = 0;
	shadowDarkness = 255;
	shadowOffsetX = 1;
	shadowOffsetY = 1;

	configurationWidget = static_cast<ConfigurationWidget *>(FindWidget("Config"));
	configurationWidget->SetObserver(&configurationObserver);
	configurationWidget->BuildConfiguration(this);

	SetFocusWidget(blockList);
}

bool FontGeneratorWindow::GetShadowGlyphImage(unsigned_int32 code, ShadowGlyphMetric *metric, Color2C **image) const
{
	GlyphMetric		gm;
	Color2C			*img1;

	if (systemFont.GetGlyphImage(code, &gm, (image) ? &img1 : nullptr))
	{
		if (shadowFlag)
		{
			int32 dx = shadowOffsetX;
			int32 dy = shadowOffsetY;
			int32 r = shadowBlurRadius;

			int32 w2 = gm.width + Abs(dx) + r * 2;
			int32 h2 = gm.height + Abs(dy) + r * 2;

			if (image)
			{
				enum
				{
					kMaxBlurSize		= 5,
					kBlur3x3Total		= 49,
					kBlur5x5Total		= 81
				};

				const unsigned_int8 kernel[kMaxBlurSize * kMaxBlurSize] =
				{
					1, 2, 3, 2, 1,
					2, 4, 6, 4, 2,
					3, 6, 9, 6, 3,
					2, 4, 6, 4, 2,
					1, 2, 3, 2, 1
				};

				int32 divider = 9;
				if (r == 1)
				{
					divider = kBlur3x3Total;
				}
				else if (r == 2)
				{
					divider = kBlur5x5Total;
				}

				divider *= 255;

				Color2C *img2 = new Color2C[w2 * h2];
				MemoryMgr::ClearMemory(img2, w2 * h2 * sizeof(Color2C));

				for (machine j = 0; j < h2; j++)
				{
					Color2C *dst = img2 + j * w2;

					int32 cy = j - MaxZero(dy) - r;

					int32 t1 = MaxZero(cy - r);
					int32 t2 = Min(cy + r, gm.height - 1);

					for (machine i = 0; i < w2; i++)
					{
						int32 cx = i - MaxZero(dx) - r;

						int32 s1 = MaxZero(cx - r);
						int32 s2 = Min(cx + r, gm.width - 1);

						int32 accum = 0;
						for (machine t = t1; t <= t2; t++)
						{
							const unsigned_int8 *k = kernel + ((t - cy + kMaxBlurSize / 2) * kMaxBlurSize + kMaxBlurSize / 2 - cx);

							const Color2C *src = img1 + t * gm.width;
							for (machine s = s1; s <= s2; s++)
							{
								accum += src[s].GetAlpha() * k[s];
							}
						}

						accum = accum * shadowDarkness / divider;

						dst->Set(0, accum);
						dst++;
					}
				}

				for (machine j = 0; j < gm.height; j++)
				{
					Color2C *src = img1 + j * gm.width;
					Color2C *dst = img2 + ((j - MinZero(dy) + r) * w2 - MinZero(dx) + r);

					for (machine i = 0; i < gm.width; i++)
					{
						// When rendering just the shadow, the color c1 would be computed as follows.
						//
						// c1 = (1 - alpha_shadow) * dst
						//
						// Blending the glyph on top of that gives us the color c2 as follows.
						//
						// c2 = alpha_glyph * src + (1 - alpha_glyph) * c1
						//    = alpha_glyph * src + (1 - alpha_glyph) * (1 - alpha_shadow) * dst
						//    = alpha_glyph * src + (1 - alpha_glyph - alpha_shadow + alpha_glyph * alpha_shadow) * dst
						//
						// Therefore, the alpha value stored in the texture should be the following.
						//
						// alpha = alpha_glyph + alpha_shadow - alpha_glyph * alpha_shadow

						unsigned_int32 a1 = src->GetAlpha();
						unsigned_int32 a2 = dst->GetAlpha();
						unsigned_int32 alpha = ((a1 + a2) * 255 - a1 * a2) / 255;

						dst->Set(Min(a1 * 255 / Max(alpha, 1), 255), alpha);
						src++;
						dst++;
					}
				}

				delete[] img1;
				*image = img2;
			}

			metric->shadowWidth = w2;
			metric->shadowHeight = h2;
			metric->shadowOrigin.Set(MinZero(dx) - r, MinZero(dy) - r);
		}
		else
		{
			metric->shadowWidth = gm.width;
			metric->shadowHeight = gm.height;
			metric->shadowOrigin.Set(0, 0);

			if (image)
			{
				*image = img1;
			}
		}

		metric->width = gm.width;
		metric->height = gm.height;
		metric->xoffset = gm.xoffset;
		metric->yoffset = gm.yoffset;
		return (true);
	}

	return (false);
}

void FontGeneratorWindow::BuildGlyphTable(void)
{
	static char text[13] = "U+0000..00FF";
	text[2] = text[8] = Text::hexDigit[glyphPage >> 4];
	text[3] = text[9] = Text::hexDigit[glyphPage & 0x0F];
	rangeText->SetText(text);

	tableGroup->PurgeSubtree();

	float fontHeight = (float) systemFont.GetFontHeight();
	float scale = 30.0F / Fmax(fontHeight, 30.0F);
	float yshift = Floor(17.0F - fontHeight * scale * 0.5F);

	unsigned_int32 base = glyphPage << 8;
	for (machine a = 0; a < 256; a++)
	{
		unsigned_int32 code = base + a;
		if (systemFont.ValidGlyph(code))
		{
			ShadowGlyphMetric	metric;
			Color2C				*image;

			if (GetShadowGlyphImage(code, &metric, &image))
			{
				float w = (float) metric.shadowWidth * scale;
				float h = (float) metric.shadowHeight * scale;

				GlyphWidget *glyph = new GlyphWidget(Vector2D(w, h), code, image, metric.shadowWidth, metric.shadowHeight);
				glyph->SetObserver(&glyphObserver);
				tableGroup->AppendNewSubnode(glyph);
				glyphList.Append(glyph);

				glyph->SetWidgetPosition(Point3D((float) (a & 15) * 33.0F, (float) (a >> 4) * 33.0F, 0.0F));
				glyph->SetQuadOffset(Vector2D(Floor(16.0F - ((float) metric.width * 0.5F - metric.shadowOrigin.x) * scale), Floor(((float) metric.yoffset + metric.shadowOrigin.y) * scale) + yshift));

				if (glyphIncluded[code])
				{
					glyph->Include();
				}
				else
				{
					glyph->Exclude();
				}

				if ((code >= selectionBegin) && (code <= selectionEnd))
				{
					glyph->Select();
				}
			}
		}
	}
}

void FontGeneratorWindow::SetSelection(unsigned_int32 begin, unsigned_int32 end)
{
	selectionBegin = begin;
	selectionEnd = end;

	if (begin != 0)
	{
		static char text[13] = "U+0000..0000";
		text[2] = Text::hexDigit[begin >> 12];
		text[3] = Text::hexDigit[(begin >> 8) & 0x0F];
		text[4] = Text::hexDigit[(begin >> 4) & 0x0F];
		text[5] = Text::hexDigit[begin & 0x0F];
		text[8] = Text::hexDigit[end >> 12];
		text[9] = Text::hexDigit[(end >> 8) & 0x0F];
		text[10] = Text::hexDigit[(end >> 4) & 0x0F];
		text[11] = Text::hexDigit[end & 0x0F];

		selectionText->SetText(text);
		includeButton->Enable();
		excludeButton->Enable();

		unsigned_int32 beginPage = begin >> 8;
		unsigned_int32 endPage = end >> 8;

		if ((endPage < glyphPage) || (beginPage > glyphPage))
		{
			for (unsigned_machine a = beginPage; a <= endPage; a++)
			{
				if (systemFont.ValidPage(a))
				{
					glyphPage = a;
					rebuildFlag = true;
					return;
				}
			}
		}
	}
	else
	{
		selectionText->SetText("\xE2\x80\x92");	// U+2012
		includeButton->Disable();
		excludeButton->Disable();
	}

	GlyphWidget *glyph = glyphList.First();
	while (glyph)
	{
		unsigned_int32 code = glyph->GetGlyphCode();
		if ((code >= begin) && (code <= end))
		{
			glyph->Select();
		}
		else
		{
			glyph->Unselect();
		}

		glyph = glyph->Next();
	}
}

void FontGeneratorWindow::HandleGlyphEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		GlyphWidget *glyph = static_cast<GlyphWidget *>(widget);

		unsigned_int32 code = glyph->GetGlyphCode();
		if (InterfaceMgr::GetShiftKey())
		{
			SetSelection(Min(selectionBegin, code), Max(selectionEnd, code));
		}
		else
		{
			SetSelection(code, code);
		}
	}
}

void FontGeneratorWindow::HandleConfigurationEvent(SettingInterface *settingInterface)
{
	Type identifier = settingInterface->GetSettingData()->GetSettingIdentifier();
	if ((identifier == 'SHAD') || (identifier == 'STYP') || (identifier == 'DARK') || (identifier == 'SHDX') || (identifier == 'SHDY'))
	{
		bool flag = shadowFlag;
		configurationWidget->CommitConfiguration(this);

		if (flag | shadowFlag)
		{
			BuildGlyphTable();
		}
	}
}

void FontGeneratorWindow::Move(void)
{
	Window::Move();

	if (rebuildFlag)
	{
		rebuildFlag = false;
		BuildGlyphTable();
	}
}

bool FontGeneratorWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		unsigned_int32 keyCode = eventData->keyCode;

		if (((keyCode == kKeyCodeLeftArrow) && (GetFocusWidget() == blockList)) || (keyCode == kKeyCodePageUp))
		{
			if ((!rebuildFlag) && (glyphPage > 0))
			{
				for (machine a = glyphPage - 1; a >= 0; a--)
				{
					if (systemFont.ValidPage(a))
					{
						glyphPage = a;
						rebuildFlag = true;
						break;
					}
				}
			}

			return (true);
		}
		else if (((keyCode == kKeyCodeRightArrow) && (GetFocusWidget() == blockList)) || (keyCode == kKeyCodePageDown))
		{
			if ((!rebuildFlag) && (glyphPage < 255))
			{
				for (machine a = glyphPage + 1; a <= 255; a++)
				{
					if (systemFont.ValidPage(a))
					{
						glyphPage = a;
						rebuildFlag = true;
						break;
					}
				}
			}

			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void FontGeneratorWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventWidgetActivate)
	{
		if (widget == generateButton)
		{
			configurationWidget->CommitConfiguration(this);
			GenerateFont();
			delete this;
		}
		else if (widget == cancelButton)
		{
			delete this;
		}
		else if (widget == includeButton)
		{
			unsigned_int32 begin = selectionBegin;
			unsigned_int32 end = selectionEnd;
			for (unsigned_machine a = begin; a <= end; a++)
			{
				glyphIncluded[a] = true;
			}

			GlyphWidget *glyph = glyphList.First();
			while (glyph)
			{
				unsigned_int32 code = glyph->GetGlyphCode();
				if ((code >= begin) && (code <= end))
				{
					glyph->Include();
				}

				glyph = glyph->Next();
			}
		}
		else if (widget == excludeButton)
		{
			unsigned_int32 begin = Max(selectionBegin, 0x00A0);
			unsigned_int32 end = selectionEnd;
			for (unsigned_machine a = begin; a <= end; a++)
			{
				glyphIncluded[a] = false;
			}

			GlyphWidget *glyph = glyphList.First();
			while (glyph)
			{
				unsigned_int32 code = glyph->GetGlyphCode();
				if ((code >= begin) && (code <= end))
				{
					glyph->Exclude();
				}

				glyph = glyph->Next();
			}
		}
		else if (widget == loadButton)
		{
			const StringTable *table = TheFontGenerator->GetStringTable();
			const char *title = table->GetString(StringID('LPCK'));

			FilePicker *picker = new FilePicker('FRNG', title, ThePluginMgr->GetImportCatalog(), RangesResource::GetDescriptor());
			picker->SetCompletionProc(&LoadRangesPicked, this);
			AddSubwindow(picker);
		}
		else if (widget == saveButton)
		{
			const StringTable *table = TheFontGenerator->GetStringTable();
			const char *title = table->GetString(StringID('SPCK'));

			FilePicker *picker = new FilePicker('FRNG', title, ThePluginMgr->GetImportCatalog(), RangesResource::GetDescriptor(), nullptr, kFilePickerSave);
			picker->SetCompletionProc(&SaveRangesPicked, this);
			AddSubwindow(picker);
		}
	}
	else if (eventType == kEventWidgetChange)
	{
		if (widget == blockList)
		{
			const UnicodeBlock *block = &unicodeBlockTable[blockRemapTable[blockList->GetFirstSelectedIndex()]];
			SetSelection(block->firstCode, block->lastCode);
		}
	}
}

void FontGeneratorWindow::LoadRangesPicked(FilePicker *picker, void *cookie)
{
	ResourceName name(picker->GetFileName());
	name[Text::GetResourceNameLength(name)] = 0;

	RangesResource *ranges = RangesResource::Get(name, 0, ThePluginMgr->GetImportCatalog());
	if (ranges)
	{
		FontGeneratorWindow *window = static_cast<FontGeneratorWindow *>(cookie);
		MemoryMgr::CopyMemory(ranges->GetData(), window->glyphIncluded, 65536);
		ranges->Release();

		GlyphWidget *glyph = window->glyphList.First();
		while (glyph)
		{
			unsigned_int32 code = glyph->GetGlyphCode();
			if (window->glyphIncluded[code])
			{
				glyph->Include();
			}
			else
			{
				glyph->Exclude();
			}

			glyph = glyph->Next();
		}
	}
}

void FontGeneratorWindow::SaveRangesPicked(FilePicker *picker, void *cookie)
{
	File			file;
	ResourcePath	path;

	ResourceName name(picker->GetFileName());
	name[Text::GetResourceNameLength(name)] = 0;

	ThePluginMgr->GetImportCatalog()->GetResourcePath(RangesResource::GetDescriptor(), name, &path);
	TheResourceMgr->CreateDirectoryPath(path);

	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		FontGeneratorWindow *window = static_cast<FontGeneratorWindow *>(cookie);
		file.Write(window->glyphIncluded, 65536);
	}
}

EngineResult FontGeneratorWindow::GenerateFont(void)
{
	ResourcePath	fontPath;
	File			fontFile;
	FontHeader		fontHeader;
	unsigned_int8	pageIndex[256];

	for (machine a = 0; a < 256; a++)
	{
		pageIndex[a] = 0;
	}

	int32 glyphCount = 0;
	int32 totalWidth = 0;
	int32 digitWidth = 0;
	Array<unsigned_int16> glyphCode(256);

	for (machine a = 0; a < 256; a++)
	{
		if (systemFont.ValidPage(a))
		{
			for (machine b = 0; b < 256; b++)
			{
				unsigned_int32 code = (a << 8) | b;
				if ((glyphIncluded[code]) && (systemFont.ValidGlyph(code)))
				{
					ShadowGlyphMetric	metric;

					if (GetShadowGlyphImage(code, &metric))
					{
						glyphCount++;
						pageIndex[a] = 1;
						totalWidth += metric.shadowWidth + kGlyphPadding * 2;
						if (code - 0x0030 < 10U)
						{
							digitWidth = Max(digitWidth, metric.width);
						}

						glyphCode.AddElement((unsigned_int16) code);
					}
				}
			}
		}
	}

	int32 pageCount = 0;
	for (machine a = 0; a < 256; a++)
	{
		if (pageIndex[a] != 0)
		{
			pageIndex[a] = (unsigned_int8) pageCount++;
		}
	}

	unsigned_int16 *pageTable = new unsigned_int16[pageCount * 256];
	MemoryMgr::ClearMemory(pageTable, pageCount * 512);

	for (machine a = 0; a < glyphCount; a++)
	{
		unsigned_int32 code = glyphCode[a];
		unsigned_int32 index = pageIndex[code >> 8];
		pageTable[index * 256 + (code & 0x00FF)] = (unsigned_int16) a;
	}

	GlyphData *glyphData = new GlyphData[glyphCount];

	int32 shadowLeft = 0;
	int32 shadowRight = 0;
	int32 shadowTop = 0;
	int32 shadowBottom = 0;

	if (shadowFlag)
	{
		shadowLeft = shadowBlurRadius - MinZero(shadowOffsetX);
		shadowRight = shadowBlurRadius + MaxZero(shadowOffsetX);
		shadowTop = shadowBlurRadius - MinZero(shadowOffsetY);
		shadowBottom = shadowBlurRadius + MaxZero(shadowOffsetY);
	}

	// Ideally, a square texture map would satisfy w / n = h * n, where n is the number of rows of glyphs,
	// w is the sum of the widths of all glyphs, and h is the height of each row. This gives n = sqrt(w / h).
	// Add an extra row, and round the total height up to the next power of two.

	int32 fontHeight = systemFont.GetFontHeight();
	int32 fontShadowHeight = fontHeight + shadowTop + shadowBottom;
	int32 rowSpacing = fontShadowHeight + kGlyphPadding * 2;
	int32 rowCount = (int32) Sqrt((float) totalWidth / (float) rowSpacing);
	unsigned_int32 textureWidth = Min(Power2Ceil(Max(totalWidth / rowCount, rowSpacing * (rowCount + 1))), 4096);
	unsigned_int32 textureHeight = Min(Power2Ceil((totalWidth / textureWidth + 1) * rowSpacing), 4096);
	unsigned_int32 textureSize = textureWidth * textureHeight;

	Color2C *textureImage = new Color2C[textureSize];
	MemoryMgr::ClearMemory(textureImage, textureSize * sizeof(Color2C));

	unsigned_int32 u = kGlyphPadding;
	unsigned_int32 v = kGlyphPadding;
	float uscale = 1.0F / (float) textureWidth;
	float vscale = 1.0F / (float) textureHeight;
	bool imageFlag = true;

	for (machine a = 0; a < glyphCount; a++)
	{
		ShadowGlyphMetric	metric;
		Color2C				*image;

		unsigned_int32 code = glyphCode[a];
		GetShadowGlyphImage(code, &metric, &image);

		int32 extraPadding = 0;
		int32 leftPadding = 0;
		if ((code - 0x0030 < 10U) && (metric.width < digitWidth))
		{
			extraPadding = digitWidth - metric.width;
			leftPadding = extraPadding - (extraPadding >> 1);
		}

		int32 glyphShadowWidth = metric.shadowWidth + extraPadding;

		if (u >= textureWidth - glyphShadowWidth)
		{
			u = kGlyphPadding;
			v += rowSpacing;
			if (v >= textureHeight - fontShadowHeight)
			{
				imageFlag = false;
			}
		}

		if (imageFlag)
		{
			const Color2C *src = image;
			Color2C *dst = textureImage + ((v + metric.yoffset) * textureWidth + (u + leftPadding));

			for (machine j = 0; j < metric.shadowHeight; j++)
			{
				for (machine i = 0; i < metric.shadowWidth; i++)
				{
					dst[i] = src[i];
				}

				src += metric.shadowWidth;
				dst += textureWidth;
			}
		}

		delete[] image;

		GlyphData *data = &glyphData[a];
		data->glyphFlags = 0;
		data->glyphWidth = (float) (metric.width + extraPadding);
		data->glyphHeight = (float) fontHeight;
		data->glyphTop = 0;
		data->glyphTexcoord[0].Set((float) u * uscale, (float) v * vscale);
		data->glyphTexcoord[1].Set((float) (u + glyphShadowWidth) * uscale, (float) (v + fontShadowHeight) * vscale);

		u += glyphShadowWidth + kGlyphPadding * 2;
	}

	TextureImporter *textureImporter = new TextureImporter(outputFontName);
	textureImporter->SetTextureImage(0, textureWidth, textureHeight, textureImage);

	TextureHeader *textureHeader = textureImporter->GetTextureHeader();
	textureHeader->textureFlags = kTextureForceHighQuality;
	textureHeader->alphaSemantic = kTextureSemanticTransparency;
	textureHeader->wrapMode[0] = kTextureClamp;
	textureHeader->wrapMode[1] = kTextureClamp;

	EngineResult result = textureImporter->ImportTextureImage();
	if (result == kEngineOkay)
	{
		TheResourceMgr->GetGenericCatalog()->GetResourcePath(FontResource::GetDescriptor(), outputFontName, &fontPath);
		TheResourceMgr->CreateDirectoryPath(fontPath);

		result = fontFile.Open(fontPath, kFileCreate);
		if (result == kFileOkay)
		{
			int32 endian = 1;
			fontFile.Write(&endian, 4);

			int32 version = 1;
			fontFile.Write(&version, 4);

			fontHeader.fontHeight = (float) fontHeight;
			fontHeader.fontBaseline = (float) systemFont.GetFontAscent();
			fontHeader.fontSpacing = (float) fontSpacing;
			fontHeader.fontLeading = (float) fontLeading;
			fontHeader.shadowOffset[0].Set((float) -shadowLeft, (float) -shadowTop);
			fontHeader.shadowOffset[1].Set((float) shadowRight, (float) shadowBottom);
			fontHeader.pageCount = pageCount;
			fontHeader.pageIndexOffset = sizeof(FontHeader);
			fontHeader.pageTableOffset = sizeof(FontHeader) + 256;
			fontHeader.glyphCount = glyphCount;
			fontHeader.glyphDataOffset = sizeof(FontHeader) + 256 + pageCount * 512;
			fontHeader.auxiliaryDataSize = 0;
			fontHeader.auxiliaryDataOffset = 0;

			fontFile.Write(&fontHeader, sizeof(FontHeader));
			fontFile.Write(pageIndex, 256);
			fontFile.Write(pageTable, pageCount * 512);
			fontFile.Write(glyphData, glyphCount * sizeof(GlyphData));
		}
	}

	delete textureImporter;
	delete[] textureImage;
	delete[] glyphData;
	delete[] pageTable;

	return (result);
}


FontPicker::FontPicker() : Window("FontGenerator/Font")
{
}

FontPicker::~FontPicker()
{
	#if C4MACOS

		CFRelease(fontArray);

	#endif
}

#if C4WINDOWS

	FontPicker::FontWidget::FontWidget(const Vector2D& size, const char *name, const char *font) : TextWidget(size, name, font)
	{
	}

#elif C4MACOS

	FontPicker::FontWidget::FontWidget(const Vector2D& size, const char *name, const char *font, CTFontDescriptorRef desc) : TextWidget(size, name, font)
	{
		fontDescriptor = desc;
	}

#endif

FontPicker::FontWidget::~FontWidget()
{
}

#if C4WINDOWS

	int CALLBACK FontPicker::EnumFontsCallback(const LOGFONTA *logData, const TEXTMETRICA *physData, DWORD fontType, LPARAM data)
	{
		ListWidget *fontList = reinterpret_cast<ListWidget *>(data);

		const char *name = logData->lfFaceName;
		if (name[0] > '@')
		{
			fontList->InsertSortedListItem(new FontWidget(fontList->GetNaturalListItemSize(), name, fontList->GetFontName()));
		}

		return (1);
	}

#endif

void FontPicker::Preprocess(void)
{
	Window::Preprocess();

	selectButton = static_cast<PushButtonWidget *>(FindWidget("Select"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	fontList = static_cast<ListWidget *>(FindWidget("Font"));
	sizeBox = static_cast<EditTextWidget *>(FindWidget("Size"));

	#if C4WINDOWS

		LOGFONTA	fontData;

		boldBox = static_cast<CheckWidget *>(FindWidget("Bold"));
		italicBox = static_cast<CheckWidget *>(FindWidget("Italic"));

		fontData.lfCharSet = ANSI_CHARSET;
		fontData.lfFaceName[0] = 0;
		fontData.lfPitchAndFamily = 0;
		EnumFontFamiliesExA(GetDC(TheEngine->GetEngineWindow()), &fontData, &EnumFontsCallback, (LPARAM) fontList, 0);

	#elif C4MACOS

		FindWidget("Bold")->Hide();
		FindWidget("Italic")->Hide();

		int32 number = 1;
		CFStringRef optionKey = kCTFontCollectionRemoveDuplicatesOption;
		CFNumberRef optionValue = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &number);

		CFDictionaryRef optionsDict = CFDictionaryCreate(kCFAllocatorDefault, (const void **) &optionKey, (const void **) &optionValue, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		CTFontCollectionRef fontCollection = CTFontCollectionCreateFromAvailableFonts(optionsDict);

		CFRelease(optionsDict);
		CFRelease(optionValue);

		fontArray = CTFontCollectionCreateMatchingFontDescriptors(fontCollection);
		CFRelease(fontCollection);

		CFIndex fontCount = CFArrayGetCount(fontArray);
		for (CFIndex a = 0; a < fontCount; a++)
		{
			CTFontDescriptorRef fontDesc = (CTFontDescriptorRef) CFArrayGetValueAtIndex(fontArray, a);
			CFStringRef fontName = (CFStringRef) CTFontDescriptorCopyAttribute(fontDesc, kCTFontDisplayNameAttribute);
			if (fontName)
			{
				String<127>		name;

				CFStringGetCString(fontName, name, 127, kCFStringEncodingASCII);
				fontList->InsertSortedListItem(new FontWidget(fontList->GetNaturalListItemSize(), name, fontList->GetFontName(), fontDesc));

				CFRelease(fontName);
			}
		}

	#endif

	SetFocusWidget(fontList);
}

void FontPicker::Move(void)
{
	Window::Move();

	if ((fontList->GetFirstSelectedListItem()) && (sizeBox->GetText()[0] != 0))
	{
		selectButton->Enable();
	}
	else
	{
		selectButton->Disable();
	}
}

void FontPicker::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if ((widget == selectButton) || ((widget == fontList) && (sizeBox->GetText()[0] != 0)))
		{
			SystemFontData		fontData;

			int32 size = Text::StringToInteger(sizeBox->GetText());
			const FontWidget *fontWidget = static_cast<FontWidget *>(fontList->GetFirstSelectedListItem());

			#if C4WINDOWS

				fontData.fontName = fontWidget->GetText();

				unsigned_int32 flags = 0;

				if (boldBox->GetValue() != 0)
				{
					flags |= kFontImportBold;
				}

				if (italicBox->GetValue() != 0)
				{
					flags |= kFontImportItalic;
				}

				fontData.fontFlags = flags;

			#elif C4MACOS

				fontData.fontDescriptor = fontWidget->fontDescriptor;

			#endif

			TheInterfaceMgr->AddWidget(new FontGeneratorWindow(&fontData, size));
			delete this;
		}
		else if (widget == cancelButton)
		{
			delete this;
		}
	}
}


FontGenerator::FontGenerator() :
		Singleton<FontGenerator>(TheFontGenerator),
		stringTable("FontGenerator/strings"),
		generateFontCommandObserver(this, &FontGenerator::HandleGenerateFontCommand),
		generateFontCommand("gfont", &generateFontCommandObserver),
		generateFontMenuItem(stringTable.GetString(StringID('MCMD')), WidgetObserver<FontGenerator>(this, &FontGenerator::HandleGenerateFontMenuItem))
{
	TheEngine->AddCommand(&generateFontCommand);
	ThePluginMgr->AddToolMenuItem(&generateFontMenuItem);
}

FontGenerator::~FontGenerator()
{
	FontPicker *window = fontPicker;
	delete window;

	fontGeneratorList.Purge();
}

void FontGenerator::HandleGenerateFontMenuItem(Widget *menuItem, const WidgetEventData *eventData)
{
	HandleGenerateFontCommand(nullptr, nullptr);
}

void FontGenerator::HandleGenerateFontCommand(Command *command, const char *text)
{
	FontPicker *window = fontPicker;
	if (window)
	{
		TheInterfaceMgr->SetActiveWindow(window);
	}
	else
	{
		window = new FontPicker;
		fontPicker = window;
		TheInterfaceMgr->AddWidget(window);
	}
}

// ZYUQURM
