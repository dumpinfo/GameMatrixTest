#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Face;
	class Edge;
	class GraphicsDriver;
	class Material;
	class PickInfo;

	class ConvexHull : public Object
	{
	public:
		GraphicsDriver* graphicsdriver;
		AABB aabb;
		vector<Face*> face;
		vector<Edge*> edge;
		vector<Vec3> vertices;
		Surface* surface[2];
		std::vector<Surface*> visiblesurfaces;
		float maxsmoothangle;
		int countselectedfaces;

		ConvexHull();
		virtual ~ConvexHull();
		
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const float radius, const bool closest, bool shadowtest);
		//virtual void BuildNormals();
		virtual void ClearVisibleSurfaces();
		virtual void BuildVisibleSurfaces();
		virtual int FindVisibleSurfaceIndex(Material* material);
		virtual ConvexHull* Copy();
		virtual void Translate(const float x, const float y, const float z);
		virtual void Rotate(const float x, const float y, const float z);
		virtual void Scale(const float x, const float y, const float z);
		virtual Face* GetFace(const int index);
		virtual int CountVertices();
		virtual int AddVertex(const float x,const float y,const float z);
		virtual Face* AddFace();
		virtual Edge* FindEdge(const int a, const int b);
		virtual void BuildAABB();
		virtual void BuildPlanes();
		virtual void Build();
		virtual int CountFaces();
		virtual void SetVertexPosition(const int v, const float x,const float y,const float z, const bool build=true);
		virtual bool IntersectsPoint(const Vec3& p);
		virtual bool IntersectsPoint(const Vec3& p,const float d);
		virtual bool IntersectsPoint(const float x,const float y,const float z, const float radius);
		virtual bool IntersectsRay(const Vec3& p0,const Vec3& p1);
		virtual bool IntersectsAABB(AABB& aabb);
		virtual bool ContainsAABB(AABB& aabb);
		virtual bool IntersectsConvexHull(ConvexHull& convexhull);
		virtual void SetVertex(const int v, const float x, const float y, const float z);
		virtual void SetVertex(const int v, const Vec3& position);
		virtual Vec3 GetVertexPosition(const int v);
		virtual void BuildSurfaces(const Mat4& mat);
		virtual void BuildTexCoords(const Mat4& mat);
		virtual void FixErrors();
		virtual void BuildEdges();
		virtual bool IntersectsConvexHull(ConvexHull* convexhull);
		virtual bool IntersectsPlane(Plane& plane, const float epsilon=0.0f);

		static ConvexHull* Create();
		static ConvexHull* Box(const float width=1, const float height=1, const float depth=1);
		static ConvexHull* Wedge(const float width=1, const float height=1, const float depth=1);
		static ConvexHull* Cylinder(const float width=1, const float height=1, const float depth=1, const int sides=16);
		static ConvexHull* Cone(const float width=1, const float height=1, const float depth=1, const int sides=16);
		static ConvexHull* Sphere(const float width=1, const float height=1, const float depth=1, const int sides=16);
	};
}
