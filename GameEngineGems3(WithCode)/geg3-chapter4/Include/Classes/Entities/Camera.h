#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class TerrainCache;
	class LensFlare;
	class Decal;

	#define CLEAR_COLOR 1
	#define CLEAR_DEPTH 2

	#define CAMERA_PROJECTION_PERSPECTIVE 1
	#define CAMERA_PROJECTION_ORTHOGRAPHIC 2

	#define DRAW_POINTS 1
	#define DRAW_WIREFRAME 2
	#define DRAW_SOLID 4
	#define DRAW_EDIT 8

	class PickInfo;
	class WaterPlane;

	class Camera : public Entity//lua
	{
	public:
		std::list<SortedObject> sortedobjects;
		Surface* aabbsurface[2];
		std::list<Camera*>::iterator link;
		float theta;
		ConvexHull* frustum;
		Vec4 viewport;
		bool viewportdefined;
		Vec2 range;
		float zoom;
		float aspect;
		Mat4 projectionmatrix;
		Mat4 projectioncameramatrix;
		Mat4 prevprojectioncameramatrix;
		Mat4 inversematrix;
		int entitiestested;
		int clearmode;
		Vec4 clearcolor;
		int projectionmode;
		int drawmode;
		int rendermode;
		bool motionblurenabled;
		unsigned long renderiterator;
		Texture* colortable;
		int lightingmode;
		int texturemode;
		int colormode;
		bool gridmode;
		int gridmajorlines;
		Vec4 gridcolororigin;
		Vec4 gridcolormajor;
		Vec4 gridcolorminor;
		float gridsize;
		std::vector<Batch*> batches;
		bool drawphysicsmode;
		int batchesdrawn;
		int passesdrawn;
		float localfrustumpoints[8][8];
		float frustumpoints[8][8];
		int trianglesdrawn;
		bool drawentityboxesmode;
		int octreenodesdrawn;
		bool drawoctreemode;
		std::vector<OcclusionQuery*> queuedocclusionqueries[2];
		std::list<OcclusionQuery*> occlusionqueries;
		int countocclusionqueries;
		int depthdrawmode;
		int colordrawmode;
		std::vector<Vec3> drawvertexqueue[2];
		std::vector<Vec2> drawobjectcenterqueue[2];
		bool editoroverlaymode;
        bool drawnavigationmode;
        std::list<Entity*> updateprojectedshadowlist;
		/*bool isshadowcamera;
		Entity* shadowcaster;
		Entity* projectedshadowowner;*/
		Buffer* gbuffer;//lua
		std::vector<Object*> posteffects;
		bool gbufferrequired;
		Buffer* auxillarycolorbuffer[2];
		int multisamplemode;
		bool HDRmode;
		TerrainCache* terraincache;
		std::list<SpotLight*> spotlightdrawqueue;
		std::list<PointLight*> pointlightdrawqueue;
		std::list<Decal*> decaldrawqueue;
		bool drawsprites;
		Asset* skybox;
		bool drawinvisiblematerials;
		std::list<LensFlare*> drawnlensflares;
		int vrrenderstage;
		Camera* reflectioncamera;
		Buffer* waterbuffer;
		Surface* watersurface;
		//int terraindetail;
		WaterPlane* waterplane;
		Buffer* decalbuffer;
		Buffer* renderbuffer;
		Texture* rendertarget;
		bool isreflection;

		Camera();
		virtual ~Camera();

		static void DrawEntity(Camera* camera, Entity* entity, Object* extra);
		static Camera* Create(Entity* parent = NULL);//lua

		virtual void SetMultisampleMode(const int mode);//lua
		virtual int GetMultisampleMode();//lua
		virtual void SetHDRMode(const bool mode);
		virtual bool GetHDRMode();
		virtual void ReleaseBuffers();
		virtual void ClearPostEffects();//lua
		virtual void UpdateBuffers(const int width, const int height);
		virtual int AddPostEffect(const std::string& path, const uint64_t fileid=0);//lua
		virtual void RemovePostEffect(const int index);//lua
		virtual void PostProcess(Buffer* backbuffer, const int startindex);
		virtual void UpdateMatrix();
		virtual void UpdateAABB(const int mode);
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual float GetZoom();//lua
		virtual void SetRenderMode(const int rendermode);//lua
		virtual int GetRenderMode();//lua
		virtual int GetLightingMode();//lua
		virtual void SetGridSize(const float size, const int majorlines)=0;
        virtual void SetDebugEntityBoxesMode(const bool mode);//lua
        virtual bool GetDebugEntityBoxesMode();//lua
        virtual void SetDebugPhysicsMode(const bool mode);//lua
        virtual void SetDebugNavigationMode(const bool mode);//lua
        virtual bool GetDebugNavigationMode();//lua
        virtual bool GetDebugPhysicsMode();//lua
        virtual void DrawNavMeshes();
		virtual void DrawEditorOverlay()=0;
		virtual void ResetStats();
		virtual void DrawSortedObjects();
		virtual void DrawGrid()=0;
		virtual void Write(Stream* stream);
		virtual std::string GetClassName();
		virtual void DrawOcclusionQueries();
		virtual void DrawAABB(const AABB& aabb, const bool wireframe)=0;
		virtual bool CullsEntity(Entity* entity);//lua
		virtual void DrawBatches();
		virtual void DetermineVisibleEntities();
		virtual void UpdateFrustum();
		virtual bool Pick(const int x, const int y, PickInfo& pick, const float radius=0.0, const bool closest=false, const int collisiontype=0);//lua
		virtual void SetColorTable(Texture* texture);
		virtual Vec3 Project(const Vec3& p);//lua
		virtual Vec3 UnProject(const Vec3& p);//lua
		virtual void SetViewport(const float x, const float y, const float width, const float height);
		virtual Vec4 GetViewport();
		virtual void SetMotionBlurMode(const bool mode);
		virtual void SetLightingMode(const int mode);//lua
		virtual void SetTextureMode(const int mode);//lua
		virtual void SetColorMode(const int mode);//lua
		virtual void SetGridMode(const bool mode);
		//virtual void SetGridMajorLines(const int lines);
		//virtual void SetGridColor(const float originred,const float originblue,const float origingreen,const float majorred,const float majorblue,const float majorgreen,const float minorred,const float minorblue,const float minorgreen);
		virtual void SetDrawMode(const int colordrawmode);//lua
		virtual void SetDrawMode(const int colordrawmode, const int depthdrawmode);
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual void CopyTo(Camera* camera,const int mode);
		virtual Vec2 GetRange();//lua
		virtual void SetRange(const float nearrange, const float farrange);//lua
		virtual void SetProjectionMode(const int projectionmode);//lua
		virtual int GetProjectionMode();//lua
		virtual void SetClearColor(const float r, const float g, const float b, const float a=1.0);//lua
		virtual void SetClearColor(const Vec4& color);//lua
		virtual Vec4 GetClearColor();//lua
		virtual void Render(Entity* entity = NULL)=0;
		virtual void SetZoom(const float zoom);//lua
		virtual void SetFOV(const float fov);//lua
		virtual float GetFOV();//lua
		virtual void SetSkybox(Asset* skybox);//lua
		virtual bool SetSkybox(const std::string& path, const uint64_t fileid=0);//lua
		virtual Asset* GetSkybox();//lua
		virtual bool SetRenderTarget(Texture* texture);//lua
		virtual Texture* GetRenderTarget();//lua

		static const int Wireframe;
		static const int Solid;
		static const int Textured;
		static const int TexturedAndLighting;
		static const int Shadow;
		static int HOOK_RENDER;
		static int HOOK_CULL;
		static int LAYER_DEFAULT;
		static int LAYER_SKY;
		static int LAYER_BACKGROUND;
		static int LAYER_FOREGROUND;
		static int LAYER_WATER;
		static int LAYER_TRANSPARENCY;
		static int RenderHook;
		static int CullHook;
		static const int Orthographic;//lua
		static const int Perspective;//lua
	};
	extern Camera* ActiveCamera;
}
