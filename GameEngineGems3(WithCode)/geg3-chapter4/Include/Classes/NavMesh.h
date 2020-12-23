#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class NavPath;
	class NavTile;
	class World;
	class Agent;
	struct NavMeshDebugger;
	
	class NavMesh : public Object
	{
	public:
		int countinvalidtiles;
		std::list<Agent*> agents;
		float maxslope;
		float height;
		float stepheight;
		AABB aabb;
		float radius;
		rcContext* ctx;
		rcHeightfield* solid;
		rcConfig cfg;
		rcCompactHeightfield* chf;
		rcContourSet* cset;
		rcPolyMesh* pmesh;
		rcPolyMeshDetail* dmesh;
		std::vector<std::vector<NavTile*> > navtile;
		int tilecount;
		int tilesize;
		World* world;
		dtNavMesh* dtnavmesh;
		dtNavMeshQuery* navquery;
		//dtTiledNavMesh* navTileMesh;
		float voxelsize;
		std::list<NavPath*> paths;
		NavMeshDebugger* navmeshdebugger;
		dtCrowd* crowd;
		Thread* thread;
		std::list<NavTile*> invalidtiles;
		std::list<NavTile*> processingtiles;
		
		NavMesh();
		//NavMesh(const float height=1.8, const float radius=0.5, const float stepheight=0.5, const float voxelsize=0.3, const int tilesize=32, const int tilecount=16);
		virtual ~NavMesh();
        
		virtual bool BuildTile(const int x, const int y, float maxedgelength, float maxedgeerror);
		virtual void Clear();
		virtual bool Build(float maxedgelength, int callback(float progress));
        virtual bool Update();
		virtual void UpdateCrowd(const float step);
		//virtual bool BeginTile(const int x, const int z);
		//virtual void AddTileGeometry(Surface* surf, Mat4 mat);
		//virtual bool FinishTile();
		virtual void InvalidateTiles(const AABB& aabb);
		virtual bool UpdateTiles();
		virtual bool FindPath(const Vec3& p0, const Vec3& p1, std::vector<Vec3>& points);
		//virtual NavPath* FindPath(const Vec3& p0, const Vec3& p1);
        
        static void AddEntityGeometryToNavTile(Entity* entity, Object* extra);
        static Object* UpdateNavMesh(Object* o);
        static NavMesh* Create(World* world, const float width, const float height, const float depth, const float posx=0, const float posy=0, const float posz=0, const float agentradius=0.4, const float agentheight=1.8, const float agentstepheight=0.501);
	};
	//DWORD WINAPI UpdateNavMesh( LPVOID lpParam );
}
