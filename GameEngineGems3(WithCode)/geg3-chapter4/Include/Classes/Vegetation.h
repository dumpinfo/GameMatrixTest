#include "../Leadwerks.h"

namespace Leadwerks
{
	class VegetationCell;
	class NewtonDynamicsShape;

	class VegetationLayer : public Entity
	{
	public:
		int seed;
		int orientation;
		int collisiontype;
		//int cellresolution;
		int resolution;
		int variationmapresolution;
		Shader* shader;
		float density;
		std::vector<bool> filter;
		Texture* filtermap;
		Texture* variationmap;
		std::vector<Surface*> surfaces;
		int variationresolution;
		std::vector<float> variationmatrices;
		Model* model;
		std::vector< std::vector<VegetationCell*> > cells;
		//int cellcount;
		std::vector<VegetationCell*> renderedcells;
		std::vector<VegetationCell*> nearcells;
		Terrain* terrain;
		float billboarddistance;
		Surface* billboardsurface;
		Shader* shader_frustumcull;
		Surface* instancesurface;
		Texture* instancetexture;
		GLuint buffertexture;
		unsigned int maxinstancesdrawn;
		Vec2 sloperange;
		Vec2 heightrange;
		float viewrange;
		Buffer* billboardbuffer[2];
		std::vector<Material*> modelmaterial;
		std::vector<Material*> billboardmaterial;
		std::map<Camera*,GLuint> billboardfeedbackbuffer;
		std::map<Camera*,GLuint> modelfeedbackbuffer;
		std::map<Camera*, std::map<Context*, GLuint> > billboardocclusionquery;
        std::map<Camera*, std::map<Context*, GLuint> > modelocclusionquery;
		Vec2 scalerange;
		Vec2 colorrange;
		int shadowmode;
		float billboardtransition;
		bool billboardsenabled;
		std::map <Camera*,Vec2> gridoffset;
		int visiblegridsize;

		VegetationLayer();
		VegetationLayer(Terrain* terrain);
		virtual ~VegetationLayer();

		virtual bool IntersectsAABB(const AABB& aabb, const float padding = 0.1);
		virtual Vec4 GetInstanceColor(const int x, const int z);//lua
		virtual int GetInstancesInAABB(const AABB& aabb, std::vector<Mat4>& instances, const float padding=0.1);
		virtual bool BuildShape(const bool dynamic = false, const bool recursive = true);//lua
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction = true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction = true);
		virtual void SetShadowMode(const int shadowmode);//lua
		virtual int GetShadowMode();//lua
		virtual void SetScale(const float minscale, const float maxscale);//lua
		virtual void SetColor(const float minbrightness, const float maxbrightness);//lua
		virtual void SetBillboardDistance(const float distance);//lua
		virtual void SetViewRange(const float range);//lua
		virtual void SetSlopeConstraints(const float minslope, const float maxslope);//lua
		virtual void SetHeightConstraints(const float minheight, const float maxheight);//lua
		virtual void BuildVariationMatrices();
		virtual Mat4 GetInstanceMatrix(const int x, const int z);//lua
		virtual void SetModel(Model* model);//lua
		virtual bool SetModel(const std::string& path);//lua
		virtual void PrePass(Camera* camera);
		virtual void Draw(Camera* camera);
		virtual void SetDensity(const float density);//lua
		virtual void RenderBillboard();
		virtual std::string GetClassName();

		//----------------------------------------------------------
		//Physics
		//----------------------------------------------------------
		NewtonMesh* newtonmesh;		
		NewtonDynamicsShape* instanceshape;
		Surface* collisionsurface;
		std::vector<int> indices;
		std::vector<float> facenormals;
		std::vector<float> vertexpositions;

		std::vector<int> collisionsurfaceindicecounts[32];
		std::vector<int> collisionsurfaceindices[32];
		std::vector<float> collisionsurfacevertices[32];
		std::vector<Mat4> instances[32];

		virtual void DrawPhysics(Camera* camera);

		NewtonCollision* newtoncollision;
		static void CollideCallback(NewtonUserMeshCollisionCollideDesc* const collideDescData, const void* const continueCollisionHandle);
		static dFloat RayHitCallback(NewtonUserMeshCollisionRayHitDesc* const lineDescData);
		static int AABBTest(void* const userData, const dFloat* const boxP0, const dFloat* const boxP1);
		static int GetFacesInAABB(void* const userData, const dFloat* const p0, const dFloat* const p1, const dFloat** const vertexArray, int* const vertexCount, int* const vertexStrideInBytes, const int* const indexList, int maxIndexCount, const int* const userDataLis);
		//static void SerializationCallback(void* const userData, NewtonSerializeCallback function, void* const serializeHandle);
		//static void DestroyCallback(void* const userData);
		//static void GetInfoCallback(void* const userData, NewtonCollisionInfoRecord* const infoRecord);
		//----------------------------------------------------------
		//
		//----------------------------------------------------------

		static World* billboardworld;
		static Camera* billboardcamera;
	};

	/*class VegetationCell : public Entity
	{
	public:
		iVec2 position;
		VegetationLayer* layer;

		VegetationCell();
		virtual ~VegetationCell();

		virtual void UpdateAABB(const int mode);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction = true);
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction = true);
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
	};*/
}
