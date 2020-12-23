#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Body;
	
	class Collision//lua
	{
	public:
		Body* body[2];
		Vec3 position;
		Vec3 normal;
		float speed;
		
		virtual std::string GetClassName();

		Collision();

		static void SetResponse(const int collisionTypeA, const int collisionTypeB, const int response);//lua

		static const int None;//lua
		static const int Collide;//lua
		static const int Prop;//lua
		static const int Scene;//lua
		static const int Character;//lua
		static const int Trigger;//lua
		static const int Debris;//lua
		static const int Projectile;//lua
		static const int LineOfSight;//lua
		static const int Fluid;//lua
	};
}
