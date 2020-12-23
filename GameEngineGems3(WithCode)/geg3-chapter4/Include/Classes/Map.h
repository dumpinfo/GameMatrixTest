#include "../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class Brush;
	class Model;

	class Map//lua
	{
	public:
		Entity* toplevelentity;
		int version;
		Stream* stream;
		std::map<int,std::string> stringtable;
		std::map<int,Entity*> entityidmap;
		std::map<Entity*,int> entityscriptposition;
		std::map<Entity*,int> entityscriptconnectionsposition;
		std::vector<Entity*> entities;
		std::vector<Texture*> lightmaps;
		std::vector<Texture*> lightvectormaps;
		bool loadasprefab;
		std::vector<Model*> mergedobjects;
		std::string path;
		std::map<OctreeNode*,std::vector<Model*> > nodemergedobjects;
		uint64_t fileid;

		Map();
		virtual ~Map();
		
		virtual bool Read(const int flags);
		virtual Entity* ReadEntity();
		virtual std::string ReadString();
		virtual Entity* ReadObject(Entity* parent, const int childindex, const int prefabindex);
		virtual Entity* ReadBrush();
		virtual Entity* ReadJoint();
		//virtual Entity* ReadModel();
        virtual Entity* ReadCamera();
		virtual Entity* ReadEmitter();
		virtual Entity* ReadAttractor();
		virtual Model* FindMergedModel(Entity* entity, const bool walkable);
		virtual Entity* FindEntity(const std::string& name);

		static bool CompareEntity(Entity* first, Entity* second);

		//std::string path;
		//std::vector<Entity*> entities;
		
		//Scene();
		//virtual ~Scene();
		
		//virtual int CountEntities();
		//virtual Entity* GetEntity(const int index);
		
		static const int LoadScripts;//lua
		static const int DeferStartScriptFunction;
		static std::string LoadMapHookName;
		//static bool Save(Stream* stream);
		//static bool Save(const std::string& path);
		static bool Load(Stream* stream, void hook(Entity* entity, Object* extra), Object* extra, const int flags, const std::string& path, const uint64_t fileid);
		static bool Load(const std::string& path, void hook(Entity* entity, Object* extra), Object* extra = NULL, const int flags = Map::LoadScripts, const uint64_t fileid = 0);
		static bool Load(const std::string& path, const std::string& hookname, Object* extra = NULL, const int flags = Map::LoadScripts, const uint64_t fileid = 0);//lua
		static bool Load(const std::string& path, const int flags = Map::LoadScripts, const uint64_t fileid=0);//lua
		static Entity* GetPrefabChildIndex(Entity* parent, const int prefabchildindex);
		static void LoadMapHook(Entity* entity, Object* extra);
	};
}
