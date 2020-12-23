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


#ifndef C4FontGenerator_h
#define C4FontGenerator_h


#include "C4Plugins.h"
#include "C4Configuration.h"


extern "C"
{
	C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
}


namespace C4
{
	enum
	{
		kUnicodeBlockCount		= 149
	};


	enum
	{
		kWidgetGlyph			= 'GLPH'
	};


	class FontGeneratorWindow;


	struct UnicodeBlock
	{
		unsigned_int16		firstCode;
		unsigned_int16		lastCode;
		const char			*blockName;
	};


	struct GlyphMetric
	{
		int32		width;
		int32		height;
		int32		xoffset;
		int32		yoffset;
	};


	struct ShadowGlyphMetric : GlyphMetric
	{
		int32		shadowWidth;
		int32		shadowHeight;
		Integer2D	shadowOrigin;
	};


	struct SystemFontData
	{
		#if C4WINDOWS

			const char				*fontName;
			unsigned_int32			fontFlags;

		#elif C4MACOS

			CTFontDescriptorRef		fontDescriptor;

		#endif
	};


	class RangesResource : public Resource<RangesResource>
	{
		friend class Resource<RangesResource>;

		private:

			static ResourceDescriptor	descriptor;

			~RangesResource();

		public:

			RangesResource(const char *name, ResourceCatalog *catalog);
	};


	class SystemFont
	{
		private:

			#if C4WINDOWS

				HDC				deviceContext;
				HGDIOBJ			defaultFont;
				HFONT			fontObject;
				GLYPHSET		*glyphSet;

			#elif C4MACOS

				CTFontRef		coreTextFont; 
				CGContextRef	graphicsContext;

				int32			contextWidth; 
				int32			contextRowBytes;
				CGSize			spaceWidth; 

				CGGlyph			*glyphTable;
				unsigned_int8	*imageStorage; 

			#endif 
 
			int32				fontHeight;
			int32				fontAscent;

			bool				validPage[256]; 

		public:

			SystemFont(const SystemFontData *fontData, int32 size);
			~SystemFont();

			int32 GetFontHeight(void) const
			{
				return (fontHeight);
			}

			int32 GetFontAscent(void) const
			{
				return (fontAscent);
			}

			bool ValidPage(unsigned_int32 page) const
			{
				return (validPage[page]);
			}

			bool ValidBlock(const UnicodeBlock *block) const;
			bool ValidGlyph(unsigned_int32 code) const;

			bool GetGlyphImage(unsigned_int32 code, GlyphMetric *metric, Color2C **image = nullptr) const;
	};


	class GlyphWidget final : public ImageWidget, public ListElement<GlyphWidget>
	{
		private:

			struct GlyphVertex
			{
				Point2D		position;
				Color4C		color;
			};

			unsigned_int32				glyphCode;

			const Color2C				*glyphImage;
			TextureHeader				textureHeader;

			static SharedVertexBuffer	vertexBuffer;
			static SharedVertexBuffer	indexBuffer;

			List<Attribute>				attributeList;
			DiffuseAttribute			diffuseAttribute;

			Renderable					backgroundRenderable;

			bool CalculateBoundingBox(Box2D *box) const override;

		public:

			GlyphWidget(const Vector2D& size, unsigned_int32 code, Color2C *image, int32 glyphWidth, int32 glyphHeight);
			~GlyphWidget();

			using ListElement<GlyphWidget>::Previous;
			using ListElement<GlyphWidget>::Next;

			unsigned_int32 GetGlyphCode(void) const
			{
				return (glyphCode);
			}

			void Select(void)
			{
				SetWidgetState(GetWidgetState() | kWidgetHilited);
			}

			void Unselect(void)
			{
				SetWidgetState(GetWidgetState() & ~kWidgetHilited);
			}

			void Include(void)
			{
				SetWidgetColor(K::black);
			}

			void Exclude(void)
			{
				SetWidgetColor(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F));
			}

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class FontGeneratorWindow : public Window, public ListElement<FontGeneratorWindow>
	{
		private:

			unsigned_int32			glyphPage;
			bool					*glyphIncluded;

			unsigned_int32			selectionBegin;
			unsigned_int32			selectionEnd;

			bool					rebuildFlag;
			List<GlyphWidget>		glyphList;

			ResourceName			outputFontName;
			int32					fontSpacing;
			int32					fontLeading;

			bool					shadowFlag;
			int32					shadowBlurRadius;
			int32					shadowDarkness;
			int32					shadowOffsetX;
			int32					shadowOffsetY;

			SystemFont				systemFont;

			PushButtonWidget		*generateButton;
			PushButtonWidget		*cancelButton;
			PushButtonWidget		*loadButton;
			PushButtonWidget		*saveButton;
			PushButtonWidget		*includeButton;
			PushButtonWidget		*excludeButton;

			TextWidget				*rangeText;
			TextWidget				*selectionText;

			ListWidget				*blockList;
			Widget					*tableGroup;

			ConfigurationWidget							*configurationWidget;
			ConfigurationObserver<FontGeneratorWindow>	configurationObserver;
			WidgetObserver<FontGeneratorWindow>			glyphObserver;

			int32					usedBlockCount;
			unsigned_int8			blockRemapTable[kUnicodeBlockCount];

			static const UnicodeBlock unicodeBlockTable[kUnicodeBlockCount];

			bool GetShadowGlyphImage(unsigned_int32 code, ShadowGlyphMetric *metric, Color2C **image = nullptr) const;

			void BuildGlyphTable(void);
			void SetSelection(unsigned_int32 begin, unsigned_int32 end);

			void HandleGlyphEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleConfigurationEvent(SettingInterface *settingInterface);

			static void LoadRangesPicked(FilePicker *picker, void *cookie);
			static void SaveRangesPicked(FilePicker *picker, void *cookie);

			EngineResult GenerateFont(void);

		public:

			FontGeneratorWindow(const SystemFontData *fontData, int32 size);
			~FontGeneratorWindow();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Move(void) override;

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class FontPicker : public Window, public LinkTarget<FontPicker>
	{
		private:

			class FontWidget final : public TextWidget
			{
				public:

					#if C4WINDOWS

						FontWidget(const Vector2D& size, const char *name, const char *font);

					#elif C4MACOS

						CTFontDescriptorRef		fontDescriptor;

						FontWidget(const Vector2D& size, const char *name, const char *font, CTFontDescriptorRef desc);

					#endif

					~FontWidget();
			};

			PushButtonWidget		*selectButton;
			PushButtonWidget		*cancelButton;

			ListWidget				*fontList;
			EditTextWidget			*sizeBox;

			#if C4WINDOWS

				CheckWidget			*boldBox;
				CheckWidget			*italicBox;

				static int CALLBACK EnumFontsCallback(const LOGFONTA *logData, const TEXTMETRICA *physData, DWORD fontType, LPARAM data);

			#elif C4MACOS

				CFArrayRef			fontArray;

			#endif

		public:

			FontPicker();
			~FontPicker();

			void Preprocess(void) override;
			void Move(void) override;

			void HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData) override;
	};


	class FontGenerator : public Plugin, public Singleton<FontGenerator>
	{
		private:

			StringTable							stringTable;

			CommandObserver<FontGenerator>		generateFontCommandObserver;
			Command								generateFontCommand;
			MenuItemWidget						generateFontMenuItem;

			Link<FontPicker>					fontPicker;
			List<FontGeneratorWindow>			fontGeneratorList;

			void HandleGenerateFontMenuItem(Widget *menuItem, const WidgetEventData *eventData);
			void HandleGenerateFontCommand(Command *command, const char *text);

		public:

			FontGenerator();
			~FontGenerator();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}

			void AddFontGeneratorWindow(FontGeneratorWindow *window)
			{
				fontGeneratorList.Append(window);
			}
	};


	extern FontGenerator *TheFontGenerator;
}


#endif

// ZYUQURM
