#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Widget;

	class Button : public Widget//lua
	{
	public:
		virtual std::string GetClassName();
		static Button* Create(const std::string& text, int x, const int y, const int width, const int height, Widget* parent = NULL, const int style = 0);//lua
		virtual void Draw(Context* context);
		virtual void OnMouseDown(const int x, const int y, const int button);
		virtual void OnMouseUp(const int x, const int y, const int button);

		static const int AlignLeft;//lua
		static const int AlignRight;//lua
	};
}