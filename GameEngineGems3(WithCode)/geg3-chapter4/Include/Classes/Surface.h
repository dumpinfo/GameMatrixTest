#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Surface;
	class ModelReference;
    class PickInfo;
	
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
	
#ifdef OS_IOS
    #define POSITIONARRAYSIZE 16
#else
    #define POSITIONARRAYSIZE 12
#endif
    
	class Surface : public Object//lua
	{
	public:
		Shape* shape;
		bool aabbinvalidated;
		GraphicsDriver* graphicsdriver;
		int instancecount;
		Material* material;
		int mode;
		Texture* lightmap[2];
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
		std::list<Surface*>::iterator link;
		int bytesperindice;
		Batch* batch[2];
		int pointsize;
		int decalmode;

		Surface();
		virtual ~Surface();
		
		virtual void SetLightmap(Texture* texture, const int index);
		virtual int GetClass();
		virtual std::string GetClassName();
        virtual AABB GetAABB();//lua
        virtual Material* GetMaterial();//lua
        virtual void Add(Surface* surface, const Mat4& mat);//lua
		virtual void Add(Surface* surface, const Mat4& src, const Mat4& dst, const bool simple=false);
		virtual void CopyTo(Surface* surface);
		virtual int CountFaces();
		virtual int CountIndicesPerFace();
		virtual Vec3 GetVertexBinormal(const int index);//lua
		virtual Vec3 GetVertexTangent(const int index);//lua
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual Shape* CreateShapePolyMesh(const bool optimize = false);
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const Vec3& radius = 0.0, bool closest = false);//lua
		virtual void Update(const bool force = false);//lua
		virtual void FlipNormals();//lua
		virtual void Clear();
		virtual void SetMaterial(Material* material);//lua
		virtual int CountVertices();//lua
		virtual void Lock();
		virtual void Unlock();
		virtual void UpdateAABB();//lua
		virtual Surface* Copy()=0;
		virtual int CountTriangles();//lua
		virtual int CountIndices(); //lua
		virtual void UpdateNormals(const bool soft=false, const float distancetolerance=0.01, const float angulartolerance=180.0);//lua
		virtual int AddTriangle(const int a, const int b, const int c);//lua
		virtual int AddIndice(const int i);//lua
		virtual int AddVertex(const float x,const float y,const float z,const float nx=0,const float ny=0,const float nz=0,const float u0=0,const float v0=0,const float u1=0,const float v1=0,const float r=1,const float g=1,const float b=1,const float a=1);//lua
        virtual int AddVertex(const Vec3& position, const Vec3& normal=Vec3(0), const Vec2& texcoords0=Vec2(0), const Vec2& texcoords1=Vec2(0), const Vec4& color=Vec4(1));//lua
		virtual void SetVertexPosition(const int v, const float x,const float y,const float z);//lua
		virtual void SetVertexNormal(const int v, const float x,const float y,const float z);//lua
        virtual void SetVertexNormal(const int v, const Vec3& normal);//lua
		virtual void SetVertexPosition(const int v, const Vec3& position);//lua
		virtual void SetVertexTexCoords(const int v, const Vec2& texcoords, const int texcoordset=0);//lua
		virtual void SetVertexColor(const int v, const Vec4& color);//lua
		virtual void SetVertexTangent(const int v, const float x,const float y,const float z);//lua
		virtual void SetVertexBinormal(const int v, const float x,const float y,const float z);//lua
		virtual void SetVertexTexCoords(const int v, const float x,const float y,const int texcoordset=0);//lua
		virtual void SetVertexColor(const int v, const float r,const float g,const float b,const float a);//lua
		virtual Vec3 GetVertexPosition(const int v);//lua
		virtual Vec3 GetVertexNormal(const int v);//lua
		virtual Vec2 GetVertexTexCoords(const int v, const int texcoordset=0);//lua
		virtual Vec4 GetVertexColor(const int v);//lua
		virtual bool UpdateTangentsAndBinormals();//lua
		virtual void UpdateTree();
		virtual int GetTriangleVertex(const int t, const int c);//lua
		virtual Vec3 GetTriangleNormal(const int t);//lua
		virtual int GetIndiceVertex(const int i);//lua
		//virtual void SetTriangleVertex(const int t, const int c, const int i)=0;
		//virtual Surface* Instance();
		virtual void Enable(const int flags = 0, const int shaderindex=0);
		virtual void Draw(const int instance_count = 0) = 0;
		virtual void Disable();
		virtual Surface* Extract(const AABB& aabb);//lua
		virtual void Facet();
		virtual void SetTriangleVertex(const int t, const int c, const int v);
		virtual void SetIndiceVertex(const int i, const int v);
		virtual void Optimize(const float tolerance=0.01);
		virtual void UpdateTexCoords(const int mode, const Mat4& mat=Mat4(), const float tilex=1, const float tiley=1, const int texcoordset=0);
		virtual void Transform(const Mat4& mat);
		virtual void Unweld();
		virtual int GetVertexBoneIndice(const int vertex, const int index);
		virtual float GetVertexBoneWeight(const int vertex, const int index);
		virtual void UpdateSkinning(Entity* entity, const bool vertexweighting);
        virtual void SetVertexBoneWeight(const int vertex, const int index, const float weight);
        virtual void SetVertexBoneIndice(const int vertex, const int index, const int indice);
        virtual void Translate(const float x, const float y, const float z);
        virtual void Rotate(const float x, const float y, const float z);
        virtual void Scale(const float x, const float y, const float z);
		virtual Vec3 GetVertexWeightedPosition(const int v);

        static Surface* Create();//lua
        static void ExtractCallback(OctreeNode* node, const AABB& aabb, const char* extra);
        
		static float OptimizeTolerance;
		static float UpdateNormalsLinearTolerance;
		static float UpdateNormalsAngularTolerance;
		static bool UpdateNormalsCompare(Vec3 v0, Vec3 v1);
		static bool OptimizeCompare(Vertex v0, Vertex v1);

		static Surface* Plane(const int xsegs=1, const int zsegs=1);//lua
    };
	
	extern const int TEXCOORDS_PLANE;
	extern const int TEXCOORDS_BOX;
	extern const int TEXCOORDS_SPHERE;
	extern const int TEXCOORDS_CYLINDER;
	extern std::list<Surface*> Surface_list;
}
