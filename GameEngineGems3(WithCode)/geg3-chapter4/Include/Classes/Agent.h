#include "../Leadwerks.h"

namespace Leadwerks
{
	class Agent
	{
	public:
		NavMesh* navmesh;
		int id;
		Vec3 position;
		//dtCrowdAgent* dtcrowdagent;
		std::list<Agent*>::iterator link;
		dtPolyRef currentpoly;
		dtCrowdAgentParams ap;
        Vec3 destination;
        bool destinationset;
        float maxspeed;
        
		Agent(NavMesh* navmesh, const Vec3& position);
		virtual ~Agent();
		
        virtual void Stop();
        virtual void SetPosition(const float x, const float y, const float z);
		virtual bool SetDestination(const float x, const float y, const float z);
	};
}
