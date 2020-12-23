#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL4IndiceArray : public OpenGL2IndiceArray
	{
	public:
		OpenGL4IndiceArray();
		OpenGL4IndiceArray(const int mode, const int type);
	};
}
