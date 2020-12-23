#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class Fluid
	{
	public:
		float viscosity;
		
		Fluid();
		virtual ~Fluid();
		
	}
	
	Fluid* CreateFluid(const int molecules, const float viscosity=1.0)
}
