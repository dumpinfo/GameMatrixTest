#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class ConvexHull;
	class Face;
	class Model;
	
	class Brush : public Entity//lua
	{
	public:
		ConvexHull* convexhull;
		//Surface* surface[2];
		float maxsmoothangle;
		//Shape* shape;
		std::list<Brush*>::iterator link;
		vector<Surface*> surfaces;
		bool surfacesbuilt;

		Brush();
		virtual ~Brush();
		
		virtual void BuildTexCoords();
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual void CopyTo(Brush* Brush,const int mode);
		//virtual bool BuildShape(const int shapeid);
		//virtual void SetShape(Shape* shape, const Mat4& mat);
		virtual int GetClass();
		virtual void UpdateAABB(const int mode);
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual Face* GetFace(const int index);
		virtual void SetMaterial(Material* material, const bool recursive=false);
		virtual void Center();
		virtual void Fix();
		virtual void SetVertexPosition(const int v, const float x, const float y, const float z);
		virtual Vec3 GetVertexPosition(const int v);
		virtual int AddVertex(const float x, const float y, const float z);
		virtual int AddVertex(const Vec3& position);
		virtual Face* AddFace();
		virtual void Build();
		virtual int CountFaces();
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const float radius, const bool closest, const bool recursive=false, const int collisiontype=0);
		virtual void Scale(const float x, const float y, const float z);
		virtual void TranslateSurfaces(const float x, const float y, const float z);
		virtual void RotateSurfaces(const float x, const float y, const float z);
		virtual void ScaleSurfaces(const float x, const float y, const float z);
		//virtual void BuildSurface(const int index);
		virtual void BuildNormals();
        virtual bool BuildShape(const bool dynamic=false, const bool recursive=false);
        virtual std::string GetClassName();
		virtual void UpdateMatrix();
		virtual void ResetTextureMapping();
		virtual void SetMatrix(const Mat4& mat);
		virtual void ShiftTextureMapping(const float x, const float y, const float z, const int mode, const bool glb, const float cx, const float cy, const float cz);
		virtual void ShiftTextureMapping(const Mat4& src, const Mat4& dst);
		virtual bool IntersectsBrush(Brush* brush, Face* face = NULL, const float epsilon=0.0f);
		virtual bool IntersectsModel(Model* model);
		virtual bool IntersectsPoint(const Vec3& point, const float d=0.0f);
		virtual bool Slice(Plane& plane, Brush* operandA, Brush* operandB, const float epsilon = 0.001, Face* sliceface=NULL);
		virtual bool BuildEndCap(Plane& plane, std::vector<int>& splitvertices0, Brush* operandA, Face* sliceface = NULL);
		virtual int FindVertex(const Vec3& position, const float epsilon=0.001);
		virtual void ClearSurfaces();
		virtual Surface* FindSurface(Material* material);

		static std::vector<Brush*> adjacentbrushes;
		static AABB adjacentbrushesaabb;
        
		static bool TextureLockMode;

		static void GetAdjacentBrushes(Entity* entity, Object* extra);
		static Brush* Create(Entity* parent=NULL);
		static Brush* Box(const float width=1, const float height=1, const float depth=1, Entity* parent=NULL);
		static Brush* Wedge(const float width=1, const float height=1, const float depth=1, Entity* parent=NULL);
		static Brush* Cylinder(const float width=1, const float height=1, const float depth=1, const int sides=16, Entity* parent=NULL);
		static Brush* Cone(const float width=1, const float height=1, const float depth=1, const int sides=16, Entity* parent=NULL);
		static Brush* Sphere(const float width=1, const float height=1, const float depth=1, const int sides=16, Entity* parent=NULL);
	};
}
