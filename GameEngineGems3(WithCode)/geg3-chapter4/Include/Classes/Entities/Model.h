#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{	
	class Model : public Entity//lua
	{
	public:
		std::list<Model*>::iterator link;
		ModelFactory* factory;
        std::string path;
        bool collapsedfrombrushes;//lua
		Entity* projectedshadowowner;
		bool walkable;//for merged CSG brushes

		Model();
		Model(Entity* parent);
		virtual ~Model();
		
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const float radius = 0.0, const bool closest = false, const bool recursive=false, const int collisiontype=0);
		vector<Surface*> surfaces;
		Surface* AddSurface();//lua
		//virtual bool Create();
		virtual void FlipNormals();
		virtual int CountSurfaces();//lua
		virtual std::string GetClassName();
		virtual Surface* GetSurface(const int n);//lua
		//virtual void Draw(Camera* camera, const bool recursive);
		virtual void SetMaterial(Material* material, const bool recursive = false);//lua
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive=true, const bool callstartfunction=true);
		virtual void CopyTo(Model* Model,const int mode);
		virtual void UpdateAABB(const int mode);//lua
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		//Surface* GetSurface();
		virtual void Collapse(Model* targetmodel=NULL);
		virtual Surface* FindSurface(Material* material, Texture* lightmap=NULL);//lua
        //virtual void BuildShape();
        virtual void DrawShadow(Camera* camera, const bool recursive);
		virtual void DrawSurfaces(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void SetAutoShape(const int mode);

		virtual void TranslateSurfaces(const float x, const float y, const float z);		
		virtual void RotateSurfaces(const float x, const float y, const float z);
		virtual void ScaleSurfaces(const float x, const float y, const float z);
		//virtual bool PickEdge(const Vec3& p0, const Vec3& p1, const float distance);
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void ResetScale();

        static Model* Box(Entity* parent = NULL); //lua
        static Model* Box(const float width, Entity* parent = NULL); //lua
        static Model* Box(const float width, const float height, const float depth, Entity* parent = NULL);//lua       
		static Model* Sphere(const int segments=16, Entity* parent=NULL);//lua
		static Model* Cone(const int segments=16,Entity* parent=NULL);//lua
		static Model* Cylinder(const int sides=16, Entity* parent=NULL);//lua

		//static Model* Box(const int segmentsx, const int segmentsy, const int segmentsz, Entity* parent = NULL);
		//static Model* Cylinder(const int radiussegments, const int heightsegments, Entity* parent=NULL);
		//static Model* Plane(const int segmentsx, const int segmentsy, Entity* parent=NULL);
		
		static Model* Create(Entity* parent = NULL);//lua
		static Model* Load(const std::string& path, const int flags = 0, const uint64_t fileid=0);//lua
		static Model* Load(Stream* stream, const int flags=0);
	};
	
	//class Model_wrap : public Entity, public luabind::wrap_base {};
	
	
}
