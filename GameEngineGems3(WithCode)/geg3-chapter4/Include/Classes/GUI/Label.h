#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Widget;

	class Label : public Widget//lua
	{
	public:
		virtual std::string GetClassName();
		static Label* Create(const std::string& text, int x, const int y, const int width, const int height, Widget* parent = NULL, const int style = 0);//lua
		virtual void Draw(Context* context);
		void OnMouseEnter(int x, int y);
	};
}