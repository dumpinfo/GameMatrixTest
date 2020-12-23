#pragma once
#ifdef _WIN32
	#pragma warning(disable:4290)
	#pragma warning(disable:4996)
#endif

#include "../Leadwerks.h"

namespace Leadwerks
{
	//#define DYNAMIC_NAVMESH

	class Model;
	class Light;
	class SpotLight;
	class PointLight;
	class DirectionalLight;
	class NavMesh;
	class Simulation;
	class Body;
	class PickInfo;
	class OctreeNode;
	class Emitter;
    class CharacterController;
    class Joint;
	class Attractor;
	class Terrain;

	class World : public Object//lua
	{
	public:
		GraphicsDriver* graphicsdriver;
		Vec4 ambientlight;
		unsigned long renderiterator;
		Simulation* simulation;
		OctreeNode* octree;
		AABB aabb;
		std::list<Entity*> updateworldhookentities;
		std::list<Entity*> updatephysicshookentities;
		std::list<Entity*> postrenderhookentities;
		std::list<Entity*> entities;
		std::list<Model*> models;
		std::list<Camera*> cameras;
		std::list<Light*> lights;
		std::list<SpotLight*> spotlights;
		std::list<PointLight*> pointlights;
		std::list<DirectionalLight*> directionallights;
		std::list<Body*> bodies;
		std::list<Entity*> updatenavigationlist;
		std::list<Entity*> deletionlist;
		std::list<Emitter*> emitters;
		std::list<Brush*> brushes;
        std::list<Attractor*> attractors;
		std::list<CharacterController*> charactercontrollers;
        std::list<Joint*> joints;
		std::list<Vehicle*> vehicles;
		float viewrangedistance[4];
		NavMesh* navmesh;
        //bool pausestate;
		std::list<Entity*> octreenodeupdatelist;
		std::vector<Surface*> brushsurfaces;
		bool disablephysics;
		Vec3 gravity;
		Bank* databank;
		int stats_rendertime;
		int stats_updatetime;
		int stats_polysdrawn;
		int stats_batchesdrawn;
		int stats_drawcalls;
		int stats_shadowpolysdrawn;
		int stats_lightsdrawn;
		int stats_vegetationculling;
		Camera* projectedshadowcamera;
		std::vector<Entity*> clearvector;
		Vec3 projectedshadowrotationdirection;
		Terrain* terrain;
		std::list<World*>::iterator it;
		std::list<Entity*> entitieswithinvalidatedlighting;
		int lightquality;
		float size;
		float tessellationstrength;
		int stats_shadowsdrawn;
		std::vector<std::string> postfxpath;
		std::vector<uint64_t> postfxfileid;
		std::string skyboxpath;
		uint64_t skyboxfileid;
		bool watermode;
		float waterheight;
		Buffer* terrainbasemap;
		Material* watermaterial[2];
		Vec4 watercolor;
		Texture* watertexture[64];
		int waterquality;
		int terrainquality;
		int stats_bspnodesdrawn;
		int stats_brushesdrawn;
		int stats_vegetationinstances;

		World();
		virtual ~World();

		virtual void AddTable(const std::string& name);
		virtual void UpdateLighting();
		virtual void SetPhysicsDetail(const int mode);//lua
		virtual void ClearNavMesh();
		virtual bool BuildNavMesh(float maxedgelength = 12.0, float maxedgeerror = 1.3);//lua
        //virtual void Pause();
        //virtual void Play();
        //virtual bool Paused();
		virtual void SetGravity(const float x, const float y, const float z);//lua
		virtual Vec3 GetGravity();//lua
		virtual void UpdateOctree();
		virtual int CountEntities(const int objectid = 0);//lua
		virtual Entity* GetEntity(const int n, const int objectid = 0);//lua
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const float radius=0.0, const bool closest=false, const int collisiontype=0);//lua
		virtual bool Pick(const float x0, const float y0, const float z0, const float x1, const float y1, const float z1, PickInfo& pick, const float radius=0.0, const bool closest=false, const int collisiontype=0);//lua
		virtual void Update();//lua
		virtual void Render();//lua
		virtual void SetAmbientLight(const float l);//lua
		virtual void SetAmbientLight(const float r, const float g, const float b);//lua
		virtual void SetAmbientLight(const float r, const float g, const float b, const float a);//lua
		virtual Vec4 GetAmbientLight();
		virtual std::string Debug();
		virtual void ForEachVisibleEntityDo(Camera* camera, void callback(Camera* camera, Entity* entity, Object* extra), Object* extra = NULL);
		virtual void ForEachVisibleEntityDo(Camera* camera, const std::string& funcname, Object* extra=NULL);//lua
		virtual void ForEachEntityInAABBDo(const AABB& aabb, void callback(Entity* entity, Object* extra), Object* extra = NULL);
		virtual void ForEachEntityInAABBDo(const AABB& aabb, const std::string& funcname, Object* extra=NULL);//lua
		virtual void InvalidateNavMeshTiles(const AABB& aabb);
		virtual void Clear(const bool force=false);//lua
		virtual std::string GetClassName();
		virtual bool UpdateNavigation();
		virtual bool NavMeshFinished();
		//virtual void SetProjectedShadowVector(const Vec3& dir);
		virtual Entity* FindEntity(const std::string& name);//lua
		virtual void SetLightQuality(const int quality);//lua
		virtual int GetLightQuality();//lua
		virtual void SetSize(const float size);
		virtual float GetSize();
		virtual void SetTessellationQuality(const float tessfactor);//lua
		virtual float GetTessellationQuality();//lua
		virtual bool SerializePhysics(const std::string& filename);//lua
		virtual int GetMaxPhysicsThreads();//lua
		virtual int GetPhysicsThreadCount();//lua
		virtual void SetPhysicsThreadCount(const int threads);//lua
		virtual void SetWaterMode(const bool mode);//lua
		virtual void SetWaterHeight(const float height);//lua
		virtual void SetWaterColor(const float r, const float g, const float b, const float a=1.0f);//lua
		virtual bool GetWaterMode();//lua
		virtual float GetWaterHeight();//lua
		virtual Vec4 GetWaterColor();//lua
		virtual void SetTerrainQuality(const int quality);//lua
		static std::list<World*> list;
		static void ForEachEntityInAABBDoLuaCallback(Entity* entity, Object* extra);
		static void ForEachVisibleEntityDoLuaCallback(Camera* camera, Entity* entity, Object* extra);
		static World* Create();//lua
		static void SetCurrent(World* world);//lua
		static World* GetCurrent();//lua
		static void PickCallback(Entity* entity, Object* o);
	};

	extern World* ActiveWorld;
	//void SetWorld(World* world);
	//World* GetWorld();

}
