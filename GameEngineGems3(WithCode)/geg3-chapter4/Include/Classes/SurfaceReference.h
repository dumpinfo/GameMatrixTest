#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Surface;
	class ModelReference;
	
	#define SURFACE_TRIANGLES 1
	#define SURFACE_LINES 2
	#define SURFACE_POINTS 3
	#define SURFACE_QUADS 4
	
	#define SURFACE_POSITION 0
	#define SURFACE_NORMAL 1	
	#define SURFACE_TEXCOORDS0 2
	#define SURFACE_TEXCOORDS1 3
	#define SURFACE_COLOR 5
	#define SURFACE_BINORMAL 6
	#define SURFACE_TANGENT 7
	#define SURFACE_BONEWEIGHTS 8
	#define SURFACE_BONEINDICES 9
	
	#define SURFACE_SOLID 2
	
	class SurfaceReference : public Object
	{
	public:
		Shape* shape;
		bool aabbinvalidated;
		GraphicsDriver* graphicsdriver;
		int instancecount;
		Material* material;
		int mode;
		ModelReference* modelreference;
		VertexArray* positionarray;
		VertexArray* normalarray;
		VertexArray* texcoordsarray[2];
		VertexArray* colorarray;
		VertexArray* binormalarray;
		VertexArray* tangentarray;
		VertexArray* boneindicesarray;
		VertexArray* boneweightsarray;
		VertexArray* animatedpositionarray;
		VertexArray* animatednormalarray;
		VertexArray* animatedbinormalarray;
		VertexArray* animatedtangentarray;

		IndiceArray* indicearray;
		AABB aabb;
		bool updateaabbneeded;
		bool updatetangentsandbinormalsneeded;
		bool updatetreeneeded;
		//SurfaceTreeNode* tree;
		vector<Vertex> vertex;
		std::list<SurfaceReference*>::iterator link;
		int bytesperindice;
		Batch* batch;
		
		//Constructor
		SurfaceReference();
		virtual ~SurfaceReference();
		
		//Procedures
		virtual void CopyTo(SurfaceReference* surfacereference);
		virtual int CountFaces();
		virtual int CountIndicesPerFace();
		virtual Vec3 GetVertexBinormal(const int index);
		virtual Vec3 GetVertexTangent(const int index);
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual Shape* CreateShapePolyMesh(const bool optimize = false);
		virtual bool Pick(const Vec3& p0, const Vec3& p1, Pick& pick, const Vec3& radius = 0.0, bool closest = false);
		virtual void Update(const bool force = false);
		virtual void FlipNormals();
		virtual void Clear();
		virtual void SetMaterial(Material* material);
		virtual int CountVertices();
		virtual void Lock();
		virtual void Unlock();
		virtual void UpdateAABB();
		virtual SurfaceReference* Copy()=0;
		virtual int CountTriangles();
		virtual int CountIndices(); 
		virtual void UpdateNormals(const bool soft, const float distancetolerance, const float angulartolerance=180.0);
		virtual int AddTriangle(const int a, const int b, const int c);
		virtual int AddIndice(const int i);
		virtual int AddVertex(const float x,const float y,const float z,const float nx,const float ny,const float nz,const float u0,const float v0,const float u1,const float v1,const float r,const float g,const float b,const float a);
		virtual void SetVertexPosition(const int v, const float x,const float y,const float z);
		virtual void SetVertexNormal(const int v, const float x,const float y,const float z);
		virtual void SetVertexTangent(const int v, const float x,const float y,const float z);
		virtual void SetVertexBinormal(const int v, const float x,const float y,const float z);
		virtual void SetVertexTexCoords(const int v, const float x,const float y,const int texcoordset);
		virtual void SetVertexColor(const int v, const float r,const float g,const float b,const float a);
		virtual Vec3 GetVertexPosition(const int v);
		virtual Vec3 GetVertexNormal(const int v);
		virtual Vec2 GetVertexTexCoords(const int v, const int texcoordset);
		virtual Vec4 GetVertexColor(const int v);
		virtual bool UpdateTangentsAndBinormals();
		virtual void UpdateTree();
		virtual int GetTriangleVertex(const int t, const int c);
		virtual Vec3 GetTriangleNormal(const int t);
		virtual int GetIndiceVertex(const int i);
		//virtual void SetTriangleVertex(const int t, const int c, const int i)=0;
		virtual Surface* Instance();
		virtual void Enable(const int flags=0);
		virtual void Draw()=0;
		virtual void Disable();
		virtual SurfaceReference* Extract(const AABB& aabb);
		virtual void Facet();
		virtual void SetTriangleVertex(const int t, const int c, const int v);
		virtual void SetIndiceVertex(const int i, const int v);
		virtual void Optimize(const float tolerance=0.01);
		virtual void UpdateTexCoords(const int mode, const Mat4& mat=Mat4(), const float tilex=1, const float tiley=1, const int texcoordset=0);
		virtual void Transform(const Mat4& mat);
		virtual void Unweld();
		virtual int GetVertexBoneIndice(const int vertex, const int index);
		virtual float GetVertexBoneWeight(const int vertex, const int index);
		virtual void UpdateSkinning(Entity* entity);
        virtual void SetVertexBoneWeight(const int vertex, const int index, const float weight);
        virtual void SetVertexBoneIndice(const int vertex, const int index, const int indice);
        
		static float OptimizeTolerance;
		static float UpdateNormalsLinearTolerance;
		static float UpdateNormalsAngularTolerance;
		static bool UpdateNormalsCompare(Vec3 v0, Vec3 v1);
		static bool OptimizeCompare(Vertex v0, Vertex v1);
	};
	
	extern const int TEXCOORDS_PLANE;
	extern const int TEXCOORDS_BOX;
	extern const int TEXCOORDS_SPHERE;
	extern const int TEXCOORDS_CYLINDER;
	extern std::list<SurfaceReference*> SurfaceReference_list;
	void ExtractSurfaceReferenceCallback(OctreeNode* node, const AABB& aabb, const char* extra);
}
