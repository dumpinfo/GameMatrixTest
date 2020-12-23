#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks3D
{
	class NewtonDynamicsBodyState
	{
	public:
		Mat4 mat;
		Vec3 scale;
		bool active;
		Vec3 velocity;
		Vec3 omega;
		float mass;
	};
}
