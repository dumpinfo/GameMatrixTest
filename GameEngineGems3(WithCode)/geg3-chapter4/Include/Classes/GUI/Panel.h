#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Widget;

	class Panel : public Widget//lua
	{
	public:
		virtual std::string GetClassName();
		static Panel* Create(const int x, const int y, const int width, const int height, Widget* parent = NULL, const int style = 0);//lua
		virtual void Draw(Context* context);
		void OnMouseEnter(int x, int y);
	};
}