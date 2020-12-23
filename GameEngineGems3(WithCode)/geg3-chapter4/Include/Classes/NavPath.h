#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class NavMesh;

	class NavPath
	{
	public:
		NavMesh* navmesh;
		Surface* surface;
		std::vector<Vec3> points;
		std::list<NavPath*>::iterator link;
		
		NavPath();
		virtual ~NavPath();
		
	};
}
