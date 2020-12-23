#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class NewtonDynamicsFluid : public Fluid
	{
	public:
		float viscosity;
		
		Fluid();
		virtual ~Fluid();
		
		void ProcessCollision(const Vec3& position, const Vec3& normal, const float force)
		{
			if (isfluidparticle)
			{
				//Adhesion to other particles
				d = this->position.DistanceToPoint(position)
				d = Min(2.0,Abs(d))
				d = 1.0 - d/2.0
				force += d * (this->position - position).Normalize();
			}
			else
			{
				//Repulsion from solid objects
				force += normal * force
			}
		}
	}
	
	Fluid* CreateFluid(const int molecules, const float viscosity=1.0)
}
