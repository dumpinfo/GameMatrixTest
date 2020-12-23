#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class NavGeom
	{
	public:
		Surface* surface;
		Mat4 mat;
		
		NavGeom();
		NavGeom(Surface* surface, const Mat4& mat);
		virtual ~NavGeom();
		
	};
}
