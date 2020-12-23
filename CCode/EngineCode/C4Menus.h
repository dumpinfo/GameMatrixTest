 

#ifndef C4Menus_h
#define C4Menus_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4Widgets.h"


namespace C4
{
	enum
	{
		kShortcutUnmodified		= 1 << 0,
		kShortcutShift			= 1 << 1
	};


	//# \enum	PopupMenuFlags

	enum
	{
		kPopupMenuRenderPlain	= 1 << 0		//## The popup menu background is not rendered.
	};


	//# \enum	MenuStyle

	enum
	{
		kMenuPopup,					//## A popup menu associated with a $@PopupMenuWidget@$.
		kMenuPulldown,				//## A pulldown menu associated with a $@MenuBarWidget@$.
		kMenuContextual				//## A contextual menu that can be displayed anywhere.
	};


	enum : WidgetType
	{
		kWidgetMenu					= 'MENU',
		kWidgetMenuItem				= 'ITEM',
		kWidgetPopupMenu			= 'POPM',
		kWidgetPulldownMenu			= 'PULM',
		kWidgetMenuButton			= 'MBUT',
		kWidgetMenuBar				= 'MBAR',
		kWidgetMenuFrame			= 'MFRM'
	};


	enum : EventType
	{
		kEventWidgetSelect			= 'WGSL',
		kEventWidgetCancel			= 'WGCL'
	};


	class MenuItemWidget;
	class Menu;


	struct MenuEventData : WidgetEventData
	{
		int32				menuSelection;
		MenuItemWidget		*menuItemWidget;

		MenuEventData() = default;

		MenuEventData(EventType type, int32 selection = kWidgetValueNone, MenuItemWidget *widget = nullptr) : WidgetEventData(type)
		{
			menuSelection = selection;
			menuItemWidget = widget;
		}
	};


	class Shortcut
	{
		public:

			enum
			{
				kMaxShortcutLength = 31
			};

			typedef String<kMaxShortcutLength>	ShortcutString;

		private:

			unsigned_int32	keyCode;
			unsigned_int32	shortcutFlags;

		public:

			Shortcut()
			{
				keyCode = 0;
				shortcutFlags = 0;
			}

			Shortcut(unsigned_int32 code, unsigned_int32 flags = 0) 
			{
				keyCode = code;
				shortcutFlags = flags; 
			}
 
			Shortcut(const Shortcut& shortcut)
			{
				keyCode = shortcut.keyCode; 
				shortcutFlags = shortcut.shortcutFlags;
			} 
 
			unsigned_int32 GetKeyCode(void) const
			{
				return (keyCode);
			} 

			unsigned_int32 GetShortcutFlags(void) const
			{
				return (shortcutFlags);
			}

			bool GetShortcutText(ShortcutString& text) const;
	};


	class MenuItemWidget final : public Widget, public ListElement<MenuItemWidget>
	{
		private:

			ObserverType		menuItemObserver;
			Shortcut			menuItemShortcut;

			TextWidget			*textWidget;
			TextWidget			*shortcutWidget;
			TextWidget			*bulletWidget;
			LineWidget			*lineWidget;

		public:

			MenuItemWidget();
			MenuItemWidget(const MenuItemWidget& menuItemWidget);

			C4API MenuItemWidget(const char *text);
			C4API MenuItemWidget(const char *text, const ObserverType& observer);
			C4API MenuItemWidget(const char *text, const ObserverType& observer, const Shortcut& shortcut);
			C4API MenuItemWidget(int32 lineStyle);
			C4API ~MenuItemWidget();

			using ListElement<MenuItemWidget>::Previous;
			using ListElement<MenuItemWidget>::Next;

			const Shortcut *GetShortcut(void) const
			{
				return (&menuItemShortcut);
			}

			TextWidget *GetTextWidget(void) const
			{
				return (textWidget);
			}

			TextWidget *GetShortcutWidget(void) const
			{
				return (shortcutWidget);
			}

			TextWidget *GetBulletWidget(void) const
			{
				return (bulletWidget);
			}

			LineWidget *GetLineWidget(void) const
			{
				return (lineWidget);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			C4API void ShowBullet(void);
			C4API void HideBullet(void);
	};


	//# \class	PopupMenuWidget		The interface widget that displays a popup menu.
	//
	//# The $PopupMenuWidget$ class represents an interface widget that displays a popup menu.
	//
	//# \def	class PopupMenuWidget final : public TextWidget
	//
	//# \ctor	PopupMenuWidget(const Vector2D& size, const char *font);
	//
	//# \param	size	The size of the text box, in pixels.
	//# \param	font	The name of the font in which the text is displayed.
	//
	//# \desc
	//# The $PopupMenuWidget$ class displays a popup menu.
	//#
	//# The default widget color controls the color of the text through the $@TextWidget@$ base class.
	//# The $kWidgetColorButton$ color type is also supported by the popup menu widget, and it controls the color of the button.
	//
	//# \base	TextWidget		The text displayed in the popup menu is based on the $TextWidget$ class.


	class PopupMenuWidget final : public TextWidget
	{
		friend class WidgetReg<PopupMenuWidget>;

		private:

			struct MenuVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			unsigned_int32						popupMenuFlags;
			int32								popupMenuSelection;

			float								itemSpacing;
			ColorRGBA							buttonColor;

			List<MenuItemWidget>				menuItemList;
			WidgetObserver<PopupMenuWidget>		menuObserver;

			VertexBuffer						vertexBuffer;
			List<Attribute>						attributeList;
			DiffuseAttribute					diffuseAttribute;
			Renderable							menuRenderable;

			PopupMenuWidget();
			PopupMenuWidget(const PopupMenuWidget& popupMenuWidget);

			Widget *Replicate(void) const override;

			void HandleMenuEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API PopupMenuWidget(const Vector2D& size, const char *font);
			C4API ~PopupMenuWidget();

			unsigned_int32 GetPopupMenuFlags(void) const
			{
				return (popupMenuFlags);
			}

			void SetPopupMenuFlags(unsigned_int32 flags)
			{
				popupMenuFlags = flags;
			}

			int32 GetSelection(void) const
			{
				return (popupMenuSelection);
			}

			float GetItemSpacing(void) const
			{
				return (itemSpacing);
			}

			void SetItemSpacing(float spacing)
			{
				itemSpacing = spacing;
			}

			int32 GetMenuItemCount(void) const
			{
				return (menuItemList.GetElementCount());
			}

			MenuItemWidget *GetSelectedMenuItem(void) const
			{
				return (menuItemList[popupMenuSelection]);
			}

			MenuItemWidget *GetFirstMenuItem(void) const
			{
				return (menuItemList.First());
			}

			MenuItemWidget *GetLastMenuItem(void) const
			{
				return (menuItemList.Last());
			}

			void AppendMenuItem(MenuItemWidget *widget)
			{
				menuItemList.Append(widget);
			}

			void RemoveMenuItem(MenuItemWidget *widget)
			{
				menuItemList.Remove(widget);
			}

			void PurgeMenuItems(void)
			{
				menuItemList.Purge();
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;

			C4API void SetSelection(int32 selection, bool post = false);

			void Preprocess(void) override;
			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
	};


	class PulldownMenuWidget final : public TextWidget, public ListElement<PulldownMenuWidget>
	{
		private:

			float						itemSpacing;
			ResourceName				fontName;

			List<MenuItemWidget>		menuItemList;

			PulldownMenuWidget();
			PulldownMenuWidget(const PulldownMenuWidget& pulldownMenuWidget);

			Widget *Replicate(void) const override;

		public:

			C4API PulldownMenuWidget(const char *text, float spacing = 13.0F, const char *font = "font/Gui");
			C4API ~PulldownMenuWidget();

			using ListElement<PulldownMenuWidget>::Previous;
			using ListElement<PulldownMenuWidget>::Next;

			float GetItemSpacing(void) const
			{
				return (itemSpacing);
			}

			void SetItemSpacing(float spacing)
			{
				itemSpacing = spacing;
			}

			const ResourceName& GetFontName(void) const
			{
				return (fontName);
			}

			void SetFont(const char *font)
			{
				fontName = font;
			}

			MenuItemWidget *GetFirstMenuItem(void) const
			{
				return (menuItemList.First());
			}

			MenuItemWidget *GetLastMenuItem(void) const
			{
				return (menuItemList.Last());
			}

			void PrependMenuItem(MenuItemWidget *widget)
			{
				menuItemList.Prepend(widget);
			}

			void AppendMenuItem(MenuItemWidget *widget)
			{
				menuItemList.Append(widget);
			}

			void InsertMenuItemBefore(MenuItemWidget *widget, MenuItemWidget *before)
			{
				menuItemList.InsertBefore(widget, before);
			}

			void InsertMenuItemAfter(MenuItemWidget *widget, MenuItemWidget *after)
			{
				menuItemList.InsertAfter(widget, after);
			}

			void RemoveMenuItem(MenuItemWidget *widget)
			{
				menuItemList.Remove(widget);
			}

			void PurgeMenuItems(void)
			{
				menuItemList.Purge();
			}

			Menu *DisplayMenu(const Point3D& position);

			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
	};


	class MenuButtonWidget final : public GuiButtonWidget
	{
		private:

			Vector2D							hiliteTexcoordOffset;
			Vector2D							menuPositionOffset;

			Link<Widget>						menuWidget;
			WidgetObserver<MenuButtonWidget>	menuObserver;

			MenuButtonWidget();
			MenuButtonWidget(const MenuButtonWidget& menuButtonWidget);

			Widget *Replicate(void) const override;

			void HandleMenuEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API MenuButtonWidget(const Vector2D& size, const Point2D& minTex, const Point2D& maxTex);
			C4API ~MenuButtonWidget();

			const Vector2D& GetHiliteTexcoordOffset(void) const
			{
				return (hiliteTexcoordOffset);
			}

			void SetHiliteTexcoordOffset(const Vector2D& offset)
			{
				hiliteTexcoordOffset = offset;
			}

			const Vector2D& GetMenuPositionOffset(void) const
			{
				return (menuPositionOffset);
			}

			void SetMenuPositionOffset(const Vector2D& offset)
			{
				menuPositionOffset = offset;
			}

			PulldownMenuWidget *GetMenu(void) const
			{
				return (static_cast<PulldownMenuWidget *>(menuWidget.GetTarget()));
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void SetMenu(PulldownMenuWidget *widget);

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
	};


	//# \class	MenuBarWidget		The interface widget that displays a menu bar.
	//
	//# The $MenuBarWidget$ class represents an interface widget that displays a menu bar.
	//
	//# \def	class MenuBarWidget final : public RenderableWidget
	//
	//# \ctor	MenuBarWidget(const Vector2D& size, const char *font);
	//
	//# \param	size	The size of the menu bar widget, in pixels.
	//# \param	font	The name of the font in which menu titles are displayed.
	//
	//# \desc
	//# The $MenuBarWidget$ class displays a menu bar that allows the user to choose commands from pull-down menus.
	//#
	//# The default widget color corresponds to the $kWidgetColorBackground$ color type and determines the background color
	//# of the menu bar. The $kWidgetColorHilite$ color type is also supported by the menu bar widget, and it determines the
	//# color that highlights the selected menu when the user clicks in the menu bar. If a highlight color has not been
	//# explicitly specified, then the Interface Manager's global highlight color is used.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.


	class MenuBarWidget final : public RenderableWidget
	{
		friend class WidgetReg<MenuBarWidget>;

		private:

			enum
			{
				kMenuBarUpdatePlacement		= 1 << 0
			};

			struct MenuVertex
			{
				Point2D		position;
				ColorRGBA	color;
				Point2D		texcoord;
			};

			float							menuSpacing;
			float							menuOffset;

			unsigned_int32					colorOverrideFlags;
			ColorRGBA						hiliteColor;

			ResourceName					fontName;

			bool							preprocessFlag;
			unsigned_int16					menuBarUpdateFlags;
			Range<float>					hiliteRange;

			List<PulldownMenuWidget>		menuList;
			WidgetObserver<MenuBarWidget>	menuObserver;

			ColorRGBA						dynamicBackgroundColor;
			ColorRGBA						dynamicHiliteColor;

			VertexBuffer					vertexBuffer;

			MenuBarWidget();
			MenuBarWidget(const MenuBarWidget& menuBarWidget);

			void SetMenuBarUpdateFlags(unsigned_int32 flags)
			{
				menuBarUpdateFlags |= flags;
				Invalidate();
			}

			Widget *Replicate(void) const override;

			void SetDefaultHiliteColor(void);

			void HandleStructureUpdate(void) override;

			void HandleMenuEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API MenuBarWidget(const Vector2D& size, const char *font);
			C4API ~MenuBarWidget();

			float GetMenuSpacing(void) const
			{
				return (menuSpacing);
			}

			void SetMenuSpacing(float spacing)
			{
				menuSpacing = spacing;
			}

			float GetMenuOffset(void) const
			{
				return (menuOffset);
			}

			void SetMenuOffset(float offset)
			{
				menuOffset = offset;
			}

			const ResourceName& GetFontName(void) const
			{
				return (fontName);
			}

			void SetFont(const char *font)
			{
				fontName = font;
			}

			PulldownMenuWidget *GetFirstMenu(void) const
			{
				return (menuList.First());
			}

			PulldownMenuWidget *GetLastMenu(void) const
			{
				return (menuList.Last());
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			C4API void PrependMenu(PulldownMenuWidget *widget);
			C4API void AppendMenu(PulldownMenuWidget *widget);
			C4API void InsertMenuBefore(PulldownMenuWidget *widget, PulldownMenuWidget *before);
			C4API void InsertMenuAfter(PulldownMenuWidget *widget, PulldownMenuWidget *after);
			C4API void RemoveMenu(PulldownMenuWidget *widget);

			void Preprocess(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
	};


	class MenuFrameWidget final : public RenderableWidget
	{
		private:

			struct FrameVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			VertexBuffer				vertexBuffer;
			static SharedVertexBuffer	indexBuffer;

			List<Attribute>				attributeList;
			DiffuseAttribute			diffuseAttribute;

			void Initialize(void);

		public:

			MenuFrameWidget();
			MenuFrameWidget(const Vector2D& size);
			~MenuFrameWidget();

			void Preprocess(void) override;
			void Build(void) override;
	};


	//# \class	Menu		The root widget for menus.
	//
	//# A $Menu$ object serves as the root widget for menus.
	//
	//# \def	class Menu : public RootWidget
	//
	//# \ctor	Menu(int32 style, const List<MenuItemWidget> *list, int32 selection = -1, float minWidth = 0.0F, float spacing = 13.0F, const char *font = "font/Gui");
	//
	//# \param	style		The menu style. See below for possible values.
	//# \param	list		The list of menu items to display in the menu.
	//# \param	selection	The initial selection.
	//# \param	minWidth	The minimum width at which to display the menu.
	//# \param	spacing		The vertical distance from one menu item to the next.
	//# \param	font		The name of the font in which menu items are displayed.
	//
	//# \desc
	//# The $style$ parameter can be one of the following constants.
	//
	//# \table	MenuStyle
	//
	//# \base	RootWidget		Menus serve as a root widget container.


	class Menu : public RootWidget
	{
		private:

			enum
			{
				kMinMenuOpenTime = 250
			};

			Link<Widget>				menuBar;
			Link<Widget>				pulldownMenu;

			const List<MenuItemWidget>	*itemList;

			int32						menuStyle;
			float						minMenuWidth;
			ResourceName				fontName;

			float						itemSpacing;
			float						itemPadding;
			float						menuPadding;

			int32						itemCount;
			int32						itemSelection;
			int32						initialSelection;

			int32						openTime;

			MenuFrameWidget				frameWidget;
			QuadWidget					quadWidget;

			void SetSelection(int32 selection);

		public:

			C4API Menu(int32 style, const List<MenuItemWidget> *list, int32 selection = -1, float minWidth = 0.0F, float spacing = 13.0F, const char *font = "font/Gui");
			C4API ~Menu();

			MenuBarWidget *GetMenuBar(void) const
			{
				return (static_cast<MenuBarWidget *>(menuBar.GetTarget()));
			}

			void SetMenuBar(MenuBarWidget *widget)
			{
				menuBar = widget;
			}

			PulldownMenuWidget *GetPulldownMenu(void) const
			{
				return (static_cast<PulldownMenuWidget *>(pulldownMenu.GetTarget()));
			}

			void SetPulldownMenu(PulldownMenuWidget *widget)
			{
				pulldownMenu = widget;
			}

			int32 GetMenuStyle(void) const
			{
				return (menuStyle);
			}

			float GetItemSpacing(void) const
			{
				return (itemSpacing);
			}

			float GetMenuPadding(void) const
			{
				return (menuPadding);
			}

			void Preprocess(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;
			bool HandleKeyboardEvent(const KeyboardEventData *eventData) override;
			void TrackTask(WidgetPart widgetPart, const Point3D& mousePosition) override;
	};
}


#endif

// ZYUQURM
