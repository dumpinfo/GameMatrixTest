#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class GUI;

	class WidgetItem : public Object
	{
	public:
		std::string name;
	};

	class Widget : public Object//lua
	{
	public:
		std::string text;
		iVec2 position;
		iVec2 globalposition;
		iVec2 size;
		int value;
		int range[2];
		Vec4 backgroundcolor;
		Vec4 textcolor;
		Widget* parent;
		std::list<Widget*> kids;
		std::list<Widget*>::iterator parentlink;
		Font* font;
		std::string tooltip;
		iVec4 cliparea;
		GUI* gui;
		bool buttonpressed;
		Texture* backgroundimage;
		Texture* backgroundimagepressed;
		int style;
		Sound* sound;
		std::vector<WidgetItem> items;
		int selecteditem;
		bool hidden;

		Widget();
		virtual ~Widget();

		virtual void Hide();//lua
		virtual void Show();//lua
		virtual int GetSelectedItem();//lua
		virtual std::string GetItemText(const int index);//lua
		virtual void SetFont(Font* font);//lua
		virtual void SelectItem(const int index);//lua
		virtual int AddItem(const std::string& text);//lua
		virtual int SelectedItem();//lua
		virtual void SetValue(const int value);//lua
		virtual int GetValue();//lua
		virtual void SetColor(const float r, const float g, const float b, const float a, const int mode = 0);//lua
		virtual void SetPosition(const int x, const int y);//lua
		virtual void UpdateLayout();
		virtual iVec2 GetPosition(const bool global);//lua
		virtual int GetWidth();//lua
		virtual int GetHeight();//lua
		virtual bool ContainsPoint(int x, int y);
		virtual void SetParent(Widget* parent);
		virtual Widget* FindWidgetAtPosition(int x, int y);
		virtual void SetText(std::string text);//lua
		virtual void OnMouseMove(int x, int y);
		virtual void OnMouseDown(int x, int y, int button);
		virtual void OnMouseEnter(int x, int y);
		virtual void OnMouseLeave(int x, int y);
		virtual void OnMouseUp(int x, int y, int button);
		virtual void OnKeyDown(int keycode, int modifier);
		virtual void OnKeyUp(int keycode, int modifier);
		virtual void OnMouseScroll(int scroll);
		virtual void Draw(Context* context);
		virtual void DrawKids(Context* context);
		virtual void SetRange(const int start, const int stop);
		virtual iVec2 GetRange();
	};
}