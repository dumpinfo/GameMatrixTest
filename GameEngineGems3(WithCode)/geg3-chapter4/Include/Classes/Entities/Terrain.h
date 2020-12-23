#pragma once
#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Stamp
	{
	public:
		Vec2 translation;
		float rotation;
		Vec2 scale;
		Vec4 color;
		Mat4 mat;
		std::list<Stamp*>::iterator it;
		float radius;

		Stamp();
		virtual ~Stamp();

		//void SetTranslation(const float x, const float y);
		//void SetRotation(const float angle);
		//void SetScale(const float x, const float y);
	};

	class StampLayer
	{
	public:
		StampLayer();
		virtual ~StampLayer();

		Texture* texture[3];
		std::list<Stamp*> stamps;
	};

	class TerrainLayer
	{
	public:
		TerrainLayer();
		virtual ~TerrainLayer();

		Texture* texture[3];
		Vec2 translation;
		float rotation;
		Vec2 scale;
		Vec3 slopeconstraints;
		Vec3 heightconstraints;
		float displacement;
		int texturelookupmode;
		int texturemappingmode;
	};

	class TerrainPatch : public Entity
	{
	public:
		int x,y;
		Vec2 position;
		Terrain* terrain;
		std::list<TerrainPatch*>::iterator it;
		Vec3 offset;
		Vec3 center;
		bool localaabbinvalidated;
		Surface* surface[5];

		TerrainPatch();
		virtual ~TerrainPatch();

		virtual void UpdateVertexHeights();
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		//virtual void UpdateOctreeNode();
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void UpdateAABB(const int mode);
	};

	class TerrainCache
	{
	public:
		int x,y,segmentx0,segmenty0,segmentx1,segmenty1;
		iVec4 buffersegments[4][4];
		std::vector<Buffer*> buffer;
		Vec2 renderposition;
		iVec2 gridposition[8];
		std::vector<bool> textureinvalidated;
		std::vector<bool> needsmipmapgen;

		TerrainCache(const int resolution, const int count, const int colorcomponents, Terrain* terrain);
		virtual ~TerrainCache();
	};

	#define MAX_TERRAIN_LAYERS 16
	#define MAX_STAMP_LAYERS 64

#ifdef __linux__
	#define VIRTUAL_TEXTURE_STAGES 7
#endif
#ifdef _WIN32
	#define VIRTUAL_TEXTURE_STAGES 7
#endif
#ifdef __ANDROID__
#define VIRTUAL_TEXTURE_STAGES 4
#endif
#ifdef __APPLE__
	#ifdef PLATFORM_IOS
		#define VIRTUAL_TEXTURE_STAGES 4
	#else
		#define VIRTUAL_TEXTURE_STAGES 7
	#endif
#endif

	class Terrain : public Entity//lua
	{
	public:
		Buffer* extraclipmapbuffer;
		//Shader* shader_clipmap[2][2][2][2][2][2][2][2][2][2][2][2];
		bool hires;
		int bpp;
		int resolution;
		int segmentresolution;
		Texture* heightmap;
		Texture* normalmap;
		Texture* offsetmap;
		Shader* shader_clipmap;
		//Buffer* heightbuffer;//for rendering to heightmap
		Buffer* normalbuffer;
		unsigned short* heightfield;
		unsigned char* displacementfield;
		char* attributes;
		unsigned char* offsetfield;
		std::vector<TerrainPatch*> patches;
		int segments;
		Surface* surface[5];
		std::vector<Vec3> instances;
		int nearsegmentsubdivision;
		Material* material[6];
		std::vector<TerrainPatch*> patchdrawqueue;
		std::vector<Mat4> patchdrawqueuematrix;
		std::vector<iVec2> patchdrawqueueoffset;
		float texturerange[VIRTUAL_TEXTURE_STAGES];
		float texturescale[256];
		TerrainLayer layer[MAX_TERRAIN_LAYERS];
		float texturerangemultiplier;
		float neartexturerange;
		bool heightfieldmodified;
		StampLayer stamplayers[MAX_STAMP_LAYERS];
		Shader* shader_stamp;
		Surface* stampsurface;
		Buffer* alphabuffer[MAX_TERRAIN_LAYERS/4];
		Texture* alphamap[MAX_TERRAIN_LAYERS/4];
		Shader* shader_updatenormals;
		float height;
		bool somepatchaabbsinvalidated;
		std::map<int,Shader*> clipmapshader;
		//std::vector<unsigned char> alphadata;
		Shader* shader_clear;
		Texture* texture_defaultnormal;
		Texture* texture_defaultdisplacement;
		Texture* texture_black;
		Surface* drawrectsurface;
		float texturestagedistance;
		float texturestagemultiplier;
		bool forcedrawall;
		std::vector<VegetationLayer*> vegetationlayers;
		Bank* offsetdata;
		Texture* vegetationmap;

		//Android data buffers for reload
		Bank* normaldata;
		Bank* alphadata[MAX_TERRAIN_LAYERS/4];

		Terrain();
		virtual ~Terrain();

		virtual void SetTextureStageDistance(float distance, float multiplier);//lua
		virtual void AndroidRestoreTextureData();
		virtual void Clear();
		virtual void SetLayerAlpha(const int slot, const int x, const int y, const bool alpha);//lua
		virtual bool GetLayerAlpha(const int slot, const int x, const int y);//lua
		virtual int GetLayerAtPoint(const int x, const int y);//lua
		virtual void ClearShaders();
		virtual void SetLayerAlpha(const int slot, const float alpha);//lua
		virtual void UpdateHeightmap(const int x, const int y, const int width, const int height);
		virtual Texture* GetHeightmap(const int mode=0);
		virtual void UpdateShape();
		virtual void SetScale(const float x, const float y, const float z);
		virtual void SetElevation(const int x, const int y, const float elevation, const bool update);//lua
		virtual float GetElevation(const float x, const float z);//lua
		virtual Vec3 GetNormal(const float x, const float z);//lua
		virtual Vec3 GetNormal(const int x, const int z);
		virtual float GetSlope(const float x, const float z);//lua
		virtual void UpdateAABB(const int mode);
		virtual void SetStampTexture(Texture* texture, const int index=0, const int slot=0);
		virtual Stamp* AddStamp(const float tx, const float ty, const float angle=0, const float sx=1, const float sy=1, const int slot=0, const float r=1, const float g=1, const float b=1, const float a=1);
		virtual void CalculateTextureRanges();
		virtual void SetLayerAlpha(const int slot, const int x, const int y, const float a0, const float a1, const float a2, const float a3);//lua
		virtual void SetLayerAlpha(const int slot, const int x, const int y, const float alpha);//lua
		virtual void SetLayerAlpha(const int slot, const float a0, const float a1, const float a2, const float a3);//lua
		//virtual float GetLayerAlpha(const int slot, const int x, const int y);
		virtual void SetLayerTexture(const int slot, Texture* texture, const int index=0);//lua
		virtual void SetLayerScale(const int slot, const float x, const float y);//lua
		virtual Vec2 GetLayerScale(const int slot);//lua
		virtual Texture* GetLayerTexture(const int slot=0, const int index=0);//lua
		virtual void Draw_(Camera* camera);
		virtual void UpdateVirtualTextures(Camera* camera);
		virtual bool LoadHeightmap(const std::string& path, const float scale=1.0, const int bpp=0);//lua
		virtual void SetHeight(const int x, const int y, const float height, const bool update=false);//lua
		virtual float GetHeight(const int x, const int y);//lua
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual void RefreshVirtualTextures();
		virtual void UpdateNormals();//lua
		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const float radius, const bool closest, const bool recursive, const int collisiontype);
		virtual Shader* GetClipmapShader(const bool diffuse0,const bool diffuse1,const bool diffuse2,const bool diffuse3,const bool normal0,const bool normal1,const bool normal2,const bool normal3,const bool displacement0,const bool displacement1,const bool displacement2,const bool displacement3);
		virtual void SetLayerSlopeConstraints(const int slot, const float minslope, const float maxslope, const float transition);//lua
		virtual void SetLayerHeightConstraints(const int slot, const float minheight, const float maxheight, const float transition);//lua
		virtual Vec3 GetLayerSlopeConstraints(const int slot);//lua
		virtual Vec3 GetLayerHeightConstraints(const int slot);//lua
		//virtual void UpdateNormals(const int x, const int y);
		//virtual void UpdateNormals(const int x, const int y, const int width, const int height);
		virtual int GetSectorsAtPoints(const int grid_x, const int grid_y, TerrainPatch** list);
		virtual void UpdatePatches(const int minx, const int miny, const int maxx, const int maxy);
		virtual std::string GetClassName();
		virtual VegetationLayer* AddVegetationLayer();//lua
		virtual void SetLayerDisplacement(const int slot, const float displacement);//lua
		virtual float GetLayerDisplacement(const int slot);//lua
		virtual void SetLayerTextureLookupMode(const int slot, const int mode);//lua
		virtual int GetLayerTextureLookupMode(const int slot);//lua
		virtual void SetLayerTextureMappingMode(const int slot, const int mode);//lua
		virtual int GetLayerTextureMappingMode(const int slot);//lua
		
		static Terrain* Create(const int size, const bool editable=false);//lua
	};
}
