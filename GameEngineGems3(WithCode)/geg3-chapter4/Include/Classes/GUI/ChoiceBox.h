#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Widget;

	class ChoiceBox : public Widget//lua
	{
	public:
		Texture* image_arrow_right;
		Texture* image_arrow_left;
		int hoveredarrow;
		virtual std::string GetClassName();
		static ChoiceBox* Create(int x, const int y, const int width, const int height, Widget* parent = NULL, const int style = 0);//lua
		virtual void Draw(Context* context);
		void OnMouseDown(const int x, const int y, const int button);
		void OnMouseMove(const int x, const int y);
		void OnMouseLeave(const int x, const int y);
	};
}