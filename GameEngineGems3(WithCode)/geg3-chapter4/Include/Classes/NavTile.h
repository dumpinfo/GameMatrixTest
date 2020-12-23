#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	struct NavMeshDebugger;
	
	class NavTile : public Object
	{
	public:
		int x,z;
		AABB aabb;
		Bank* data;
		rcHeightfield* solid;
		rcConfig cfg;
		rcContext* ctx;
		rcCompactHeightfield* chf;
		rcContourSet* cset;
		rcPolyMesh* pmesh;
		rcPolyMeshDetail* dmesh;
		NavMesh* navmesh;
		NavMeshDebugger* navmeshdebugger;
		bool valid;
		Bank* geometrydata;
		Bank* areadata;
		unsigned char* navData;
		int navDataSize;
		Thread* thread;
		std::vector<Surface*> geometrysurfaces;
		std::vector<Mat4> geometrymatrices;
		NavTile* nexttile;
		std::list<NavTile*>::iterator invalidlink;
		int updatestate;
		dtTileRef tileref;
		std::list<NavGeom> geometry;
		bool invalidated;
        dtNavMeshCreateParams params;

		NavTile();
		virtual ~NavTile();
		
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual void InitParams();
        virtual void Invalidate();
		virtual bool Finalize();
		virtual void Clear();
		virtual NavTile* Copy();
		virtual int GetDataSerializedSize();
		virtual bool Initialize();
		virtual void AddSurface(Surface* surface, Mat4 mat);
		virtual bool Build();
	};
}
