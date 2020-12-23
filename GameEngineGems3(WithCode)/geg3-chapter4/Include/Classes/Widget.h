#pragma once

#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class Widget
	{
	public:
		iVec2 position;
		iVec2 size;
		Widget* parent;
		int state;
		Texture* image[9];
		
		Widget();
		virtual ~Widget();
		
		virtual void SetShape(const int x, const int y, const int width, const int height);
		virtual void Draw();
		virtual int GetState();
		virtual void SetState(const int state);
	};
}
