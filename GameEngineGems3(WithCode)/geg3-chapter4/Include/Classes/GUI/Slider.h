#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Widget;

	class Slider : public Widget//lua
	{
	public:
		virtual std::string GetClassName();
		static Slider* Create(int x, const int y, const int width, const int height, Widget* parent = NULL, const int style = 0);//lua
		virtual void Draw(Context* context);
		void OnMouseDown(const int x, const int y, const int button);
		void OnMouseUp(const int x, const int y, const int button);
		void OnMouseMove(const int x, const int y);
	};
}