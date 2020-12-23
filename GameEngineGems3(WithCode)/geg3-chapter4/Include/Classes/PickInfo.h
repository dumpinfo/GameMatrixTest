#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class Surface;

	class PickInfo : public Object//lua
	{
	public:
		Vec3 position;//lua
		Vec3 normal;//lua
		Entity* entity;//lua
		Surface* surface;//lua
		Face* face;//lua
		int triangle;//lua
		int tests;
		float distance;

		PickInfo();//lua

		virtual std::string GetClassName();
		virtual std::string Debug();

        static const int Sphere;
        static const int Polygon;
	};
}
