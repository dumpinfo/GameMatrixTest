#pragma once
#include "Leadwerks.h"

#ifdef _DLL
	#define DLLExport __declspec( dllexport )
#endif

#ifdef _STATICLIB
	#define DLLExport
#endif

#define BLITZSTRING char*

/*#ifdef __linux__
    #ifndef _ANDROID_
        #include "/usr/include/gtk-2.0/gdk/gdkx.h"
    #endif
#endif*/

namespace DLL
{
	extern "C"
	{
#ifdef __linux__
    #ifndef _ANDROID_
        void __attribute__ ((constructor)) LoadLib(void);
    #endif
#endif

#ifdef _DLL
        DLLExport BOOL WINAPI DllMain(__in  HINSTANCE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved);
#endif

#ifdef DEBUG
        DLLExport void ValidateObject(Leadwerks::Object* object);
		DLLExport void CheckType(Leadwerks::Object* object, int classid);
#endif

#ifdef __linux__
	#ifndef _ANDROID_
		DLLExport Display* GetWindowDisplay();
	#endif
#endif
		DLLExport void CasedPath(BLITZSTRING path, char* result, int sz);

		DLLExport int LeadwerksGetVersion();

		DLLExport void SetEntityCharacterControllerAngle(Leadwerks::Entity* entity, float angle);
		DLLExport float GetEntityCharacterControllerAngle(Leadwerks::Entity* entity);

		DLLExport int CheckStack(int in);

		DLLExport int GetLogText(char* s, int length);

        DLLExport void SetLogMode(int mode);

		//System
		DLLExport void SystemSetAppPath(BLITZSTRING s);
		DLLExport int SystemCountGraphicsModes();
		DLLExport void SystemGetGraphicsMode(int index, int* i);

		//Package
		DLLExport Leadwerks::Package* LoadPackage(BLITZSTRING path);
		DLLExport int CountPackageFiles(Leadwerks::Package* package);
		DLLExport void GetPackageFileName(Leadwerks::Package* package, int index, char* buf, int sz);

		//HTMLRenderer
		DLLExport void HTMLRendererSetMouseButton(Leadwerks::HTMLRenderer* htmlrenderer, int state, int button);
		DLLExport void HTMLRendererSetMousePosition(Leadwerks::HTMLRenderer* htmlrenderer, float x, float y, float z);
		DLLExport Leadwerks::HTMLRenderer* CreateHTMLRenderer(char* url, int width, int height);
		DLLExport void HTMLRendererNavigate(Leadwerks::HTMLRenderer* htmlrenderer, char* url);
		DLLExport void HTMLRendererSetSize(Leadwerks::HTMLRenderer* htmlrenderer, int width, int height);
		DLLExport void HTMLRendererGetURL(Leadwerks::HTMLRenderer* htmlrenderer, char* buf, int sz);
		DLLExport void HTMLRendererGetClickedURL(Leadwerks::HTMLRenderer* htmlrenderer, char* buf, int sz);
		DLLExport void HTMLRendererSetInput(Leadwerks::HTMLRenderer* htmlrenderer, float mx, float my, float mz, int button1, int button2, int button3);
		DLLExport Leadwerks::Texture* HTMLRendererGetTexture(Leadwerks::HTMLRenderer* htmlrenderer);
		DLLExport int HTMLRendererNeedsRedraw(Leadwerks::HTMLRenderer* htmlrenderer);
		DLLExport int HTMLRendererGetCursor(Leadwerks::HTMLRenderer* htmlrenderer);
		//DLLExport void HTMLRendererGetSize(Leadwerks::HTMLRenderer* htmlrenderer, int* size);

		//Steamworks API
		DLLExport int SteamworksSubscribeWorkshopFile(uint64_t fileid);
		DLLExport void SteamworksRunCallbacks();
		DLLExport void GetUserPublishedWorkshopFileAppID(int index, char* buf);
		DLLExport int SteamworksCountSubscribedWorkshopItems();
		DLLExport void SteamworksGetSubscribedWorkshopItem(uint64_t& fileid, int index);
		DLLExport void SteamworksGetWorkshopItemTimeUpdated(uint64_t fileid, uint64_t& time);
		DLLExport void SteamworksGetWorkshopItemTitle(uint64_t fileid, char* buf, int sz);
		DLLExport void SteamworksGetWorkshopItemLocalPath(uint64_t fileid, char* buf, int sz);
		DLLExport int SteamworksInstallPackage(uint64_t fileid, void installhook(const char* filepath));
		DLLExport int SteamworksUninstallPackage(BLITZSTRING path);
		DLLExport int SteamworksExtractWorkshopFiles(int force);
		DLLExport int SteamworksGetWorkshopFileDescription(uint64_t fileid, char* buf, int sz);
		DLLExport void SteamworksSetAchievement(BLITZSTRING name);
		DLLExport void SteamworksIncrementAchievementProgress(BLITZSTRING name);
		DLLExport int SteamworksGetAchievement(BLITZSTRING name);
		DLLExport int SteamGetAppID();
		DLLExport int SteamGetAppAge(uint64_t appid);
		DLLExport int SteamworksSubscribeAndDownloadWorkshopFile(uint64_t fileid, int callback(uint64_t fileid, float));
		DLLExport int SteamworksUnsubscribeWorkshopFile(uint64_t fileid, int callback(float));
		DLLExport int SteamworksDLCInstalled(uint64_t appid);
		DLLExport int SteamworksDLCSubscribed(uint64_t appid);
		DLLExport void SteamworksGetUserID(char* buf);
		DLLExport int CountUserPublishedWorkshopFiles(uint64_t consumerappid);
		DLLExport void GetUserPublishedWorkshopFileID(int index, char* buf);
		DLLExport void GetUserPublishedWorkshopFileTitle(int index, char* buf, int sz);
		DLLExport void GetUserPublishedFileTags(int index, char* buf, int sz);
		DLLExport int SteamworksUpdateWorkshopItemFile(uint64_t appid, uint64_t fileid, BLITZSTRING filename, BLITZSTRING previewfile, BLITZSTRING comment, int callback(float));
		DLLExport int SteamworksSyncSubscribedFiles(int callback(float), int downloadgames);
		DLLExport int SteamworksPublishScreenshot(BLITZSTRING path, BLITZSTRING title, BLITZSTRING desc, BLITZSTRING tags, int callback(float), char* buf);
		DLLExport int SteamworksPublishFile(BLITZSTRING path, BLITZSTRING title, BLITZSTRING desc, BLITZSTRING previewfile, int price, int callback(float), char* buf, BLITZSTRING tags, int privacy, uint64_t appid);
		DLLExport int SteamworksAddScreenshot(BLITZSTRING path, int width, int height);
		DLLExport int SteamworksInitialize();
		DLLExport Leadwerks::Bank* SteamworksLoadBank(BLITZSTRING path);
		DLLExport int SteamworksSaveFile(BLITZSTRING path, void* data, int size);
		DLLExport void SteamworksShutdown();
		DLLExport int SteamworksPublishVideo(BLITZSTRING vid, BLITZSTRING title, BLITZSTRING desc, char* buf);
		DLLExport void GetWorkshopCachePath(char* buf, int sz);

		//System
		DLLExport int GetSystemMemoryUsage();

		//Prefabs
		DLLExport Leadwerks::Entity* PrefabLoad(BLITZSTRING path, int flags, uint64_t fileid);

		//Terrain
		DLLExport void SetWorldTerrainQuality(Leadwerks::World* world, int quality);
		DLLExport void TerrainSetLayerTextureMappingMode(Leadwerks::Terrain* terrain, int slot, int mode);
		DLLExport int TerrainGetLayerTextureMappingMode(Leadwerks::Terrain* terrain, int slot);
		DLLExport void TerrainSetLayerTextureLookupMode(Leadwerks::Terrain* terrain, int slot, int mode);
		DLLExport int TerrainGetLayerTextureLookupMode(Leadwerks::Terrain* terrain, int slot);
		DLLExport Leadwerks::Texture* GetTerrainNormalMap(Leadwerks::Terrain* terrain);
		DLLExport Leadwerks::Texture* GetTerrainLayerAlphaMap(Leadwerks::Terrain* terrain, int slot);
		DLLExport Leadwerks::Texture* GetTerrainLayerTexture(Leadwerks::Terrain* terrain, int slot, int index);
		DLLExport void UpdateTerrainHeightmap(Leadwerks::Terrain* terrain,  int x, int y, int width, int height);
		DLLExport Leadwerks::Terrain* CreateTerrain(int sz, int editable);
		DLLExport int TerrainLoadHeightmap(Leadwerks::Terrain* terrain, BLITZSTRING path, int bitsperpixel);
		DLLExport void TerrainSetLayerTexture(Leadwerks::Terrain* terrain, int slot, Leadwerks::Texture* texture, int index);
		DLLExport Leadwerks::Texture* GetTerrainHeightmap(Leadwerks::Terrain* terrain, int mode);
		DLLExport void UpdateTerrainNormals(Leadwerks::Terrain* terrain);
		DLLExport void UpdateTerrainPatches(Leadwerks::Terrain* terrain, int minx, int miny, int maxx, int maxy);
		DLLExport void RefreshTerrainVirtualTextures(Leadwerks::Terrain* terrain);
		DLLExport void SetTerrainLayerScale(Leadwerks::Terrain* terrain, int slot, float x, float y);
		DLLExport void GetTerrainLayerScale(Leadwerks::Terrain* terrain, int slot, float* f);
		DLLExport void SetTerrainLayerSlopeConstraints(Leadwerks::Terrain* terrain, int slot, float minslope, float maxslope, float transition);
		DLLExport void SetTerrainLayerHeightConstraints(Leadwerks::Terrain* terrain, int slot, float minheight, float maxheight, float transition);
		DLLExport void GetTerrainLayerSlopeConstraints(Leadwerks::Terrain* terrain, int slot, float* f);
		DLLExport void GetTerrainLayerHeightConstraints(Leadwerks::Terrain* terrain, int slot, float* f);
		DLLExport void SetTerrainLayerAlphaMap(Leadwerks::Terrain* terrain, int slot, Leadwerks::Texture* alphamap);
		//DLLExport Leadwerks::Buffer* GetTerrainHeightmapBuffer(Leadwerks::Terrain* terrain);
		DLLExport void ClearTerrainClipmapShader(Leadwerks::Terrain* terrain);
		DLLExport void TerrainSetLayerDisplacement(Leadwerks::Terrain* terrain, int slot, float displacement);
		DLLExport float TerrainGetLayerDisplacement(Leadwerks::Terrain* terrain, int slot);

		//Shape
		DLLExport int SaveShape(Leadwerks::Shape* shape, char* path);
		DLLExport Leadwerks::Shape* ShapePolyMesh(Leadwerks::Model* model, float* matf);
		DLLExport Leadwerks::Shape* ShapeLoad(BLITZSTRING path, int flags);
        DLLExport Leadwerks::Shape* ShapeConvexDecomposition(Leadwerks::Surface* surface);
        DLLExport Leadwerks::Surface* ShapeGetSurface(Leadwerks::Shape* shape);
        DLLExport Leadwerks::Shape* ShapeBox(float x, float y, float z, float pitch, float yaw, float roll, float width, float height, float depth);
        DLLExport Leadwerks::Shape* ShapeCylinder(float x, float y, float z, float pitch, float yaw, float roll, float width, float height, float depth);
        DLLExport Leadwerks::Shape* ShapeCone(float x, float y, float z, float pitch, float yaw, float roll, float width, float height, float depth);
        DLLExport Leadwerks::Shape* ShapeSphere(float x, float y, float z, float pitch, float yaw, float roll, float width, float height, float depth);
		DLLExport Leadwerks::Shape* ShapeConvexHull(Leadwerks::Model* model, float* matf);
		DLLExport Leadwerks::Shape* ShapeShrinkWrap(Leadwerks::Model* model, float* matf);

		//DLLExport void WorldSetProjectedShadowVector(Leadwerks::World* world, float* f);
		DLLExport int WorldUpdateNavigation(Leadwerks::World* world);
		DLLExport int WorldNavMeshFinished(Leadwerks::World* world);
		DLLExport void WorldDisablePhysics(Leadwerks::World* world);

		//CharacterController
		//DLLExport Leadwerks::CharacterController* CharacterControllerCreate();
		//DLLExport void CharacterControllerSetRadius(Leadwerks::CharacterController* charactercontroller,float radius);
		//DLLExport float CharacterControllerGetRadius(Leadwerks::CharacterController* charactercontroller);
		//DLLExport void CharacterControllerSetHeight(Leadwerks::CharacterController* charactercontroller,float height);
		//DLLExport float CharacterControllerGetHeight(Leadwerks::CharacterController* charactercontroller);
		//DLLExport void CharacterControllerSetCrouchHeight(Leadwerks::CharacterController* charactercontroller, float crouchheight);
		//DLLExport float CharacterControllerGetCrouchHeight(Leadwerks::CharacterController* charactercontroller);
		//DLLExport void CharacterControllerSetStepHeight(Leadwerks::CharacterController* charactercontroller, float stepheight);
		//DLLExport float CharacterControllerGetStepHeight(Leadwerks::CharacterController* charactercontroller);
		//DLLExport void CharacterControllerSetMaxSlope(Leadwerks::CharacterController* charactercontroller, float maxslope);
		//DLLExport float CharacterControllerGetMaxSlope(Leadwerks::CharacterController* charactercontroller);

		//Brush
		DLLExport int BrushSlice(Leadwerks::Brush* brush, float nx, float ny, float nz, float d, Leadwerks::Brush* operandA, Leadwerks::Brush* operandB, float epsilon, Leadwerks::Face* sliceface);
		DLLExport void BrushResetTextureMapping(Leadwerks::Brush* brush);
		DLLExport int BrushIntersectsPoint(Leadwerks::Brush* brush, float x, float y, float z);
		DLLExport int BrushIntersectsBrush(Leadwerks::Brush* a, Leadwerks::Brush* b, Leadwerks::Face* face, float epsilon);
		DLLExport int BrushIntersectsModel(Leadwerks::Brush* brush, Leadwerks::Model* model);
		DLLExport void SetTextureLockMode(int mode);
		DLLExport void ShiftBrushTextureMapping(Leadwerks::Brush* brush, float x, float y, float z, int mode, int glb, float cx, float cy, float cz);
		DLLExport void ShiftBrushTextureMappingMat4(Leadwerks::Brush* brush, float* src, float* dst);
		DLLExport void RemapBrushesWithMaterial(Leadwerks::World* world, Leadwerks::Material* material);
        DLLExport void EntityBuildShape(Leadwerks::Entity* entity, int dynamic, int recursive);
		DLLExport void TranslateBrushSurfaces(Leadwerks::Brush* brush, float x, float y, float z);
		DLLExport void RotateBrushSurfaces(Leadwerks::Brush* brush, float x, float y, float z);
		DLLExport void ScaleBrushSurfaces(Leadwerks::Brush* brush, float x, float y, float z);
		DLLExport void BuildBrushTexCoords(Leadwerks::Brush* brush);
		DLLExport void ScaleBrush(Leadwerks::Brush* brush, float x, float y, float z);
		DLLExport Leadwerks::Brush* CreateBrush(Leadwerks::Entity* parent);
		DLLExport Leadwerks::Brush* BoxBrush(float width, float height, float depth, Leadwerks::Entity* parent);
		DLLExport Leadwerks::Brush* WedgeBrush(float width, float height, float depth, Leadwerks::Entity* parent);
		DLLExport Leadwerks::Brush* CylinderBrush(float width, float height, float depth, int sides, Leadwerks::Entity* parent);
		DLLExport Leadwerks::Brush* ConeBrush(float width, float height, float depth, int sides, Leadwerks::Entity* parent);
		DLLExport Leadwerks::Brush* SphereBrush(float width, float height, float depth, int sides, Leadwerks::Entity* parent);
		DLLExport int AddBrushVertex(Leadwerks::Brush* brush, float x, float y, float z);
		DLLExport Leadwerks::Face* AddBrushFace(Leadwerks::Brush* brush);
		DLLExport int CountBrushFaces(Leadwerks::Brush* brush);
		DLLExport Leadwerks::Face* GetBrushFace(Leadwerks::Brush* brush, int index);
		DLLExport void GetBrushVertexPosition(Leadwerks::Brush* brush, int v, float* position);
		DLLExport void SetBrushVertexPosition(Leadwerks::Brush* brush, int v, float x, float y, float z);
		DLLExport int CountBrushVertices(Leadwerks::Brush* brush);
		DLLExport void CenterBrush(Leadwerks::Brush* brush);
		//void UpdateBrush(Leadwerks::Brush* brush);
		DLLExport Leadwerks::ConvexHull* GetBrushConvexHull(Leadwerks::Brush* brush);
		DLLExport void BuildBrush(Leadwerks::Brush* brush);
		DLLExport int CountBrushEdges(Leadwerks::Brush* brush);
		DLLExport Leadwerks::Edge* GetBrushEdge(Leadwerks::Brush* brush,int index);
		DLLExport int CountBrushSelectedFaces(Leadwerks::Brush* brush);

		//Stop here

		//LensFlare
		DLLExport Leadwerks::LensFlare* LensFlareCreate(Leadwerks::Entity* parent);
		DLLExport void LensFlareSetRadius(Leadwerks::LensFlare* lensflare, float radius1, float radius2);
		DLLExport void LensFlareGetRadius(Leadwerks::LensFlare* lensflare, float* f);
		DLLExport void LensFlareSetRange(Leadwerks::LensFlare* lensflare, float range);
		DLLExport float LensFlareGetRange(Leadwerks::LensFlare* lensflare);
		DLLExport void LensFlareSetRotationSpeed(Leadwerks::LensFlare* lensflare, float rotationspeed);
		DLLExport float LensFlareGetRotationSpeed(Leadwerks::LensFlare* lensflare);

		//Listener
		DLLExport Leadwerks::Listener* ListenerCreate(Leadwerks::Entity* parent);

		//Edge
		DLLExport int GetEdgeVertex(Leadwerks::Edge* edge, int index);

		//ConvexHull
		DLLExport void BuildConvexHullAABB(Leadwerks::ConvexHull* convexhull);
		DLLExport void BuildConvexHullPlanes(Leadwerks::ConvexHull* convexhull);
		DLLExport void BuildBrushNormals(Leadwerks::Brush* brush);

		//Face
		DLLExport void FaceGetMaterialMappingScale(Leadwerks::Face* face, float* f);
		DLLExport Leadwerks::Texture* FaceGetLightmap(Leadwerks::Face* face, int index);
		DLLExport void FaceSetLightmap(Leadwerks::Face* face, Leadwerks::Texture* lightmap, int index);
		DLLExport void FaceGetVertexNormal(Leadwerks::Face* face, int v, float* f);
		DLLExport void FaceGetVertexTexCoords(Leadwerks::Face* face, int vert, int texcoordset, float* f);
		DLLExport void FaceSetVertexTexCoords(Leadwerks::Face* face, int vert, float u, float v, int texcoordset);
		DLLExport void SetFaceTextureMappingPlane(Leadwerks::Face* face, float x, float y, float z, float d, int index, int mode);
		DLLExport void GetFaceTextureMappingPlane(Leadwerks::Face* face, int axis, float* f, int mode);
		DLLExport void GetFacePlane(Leadwerks::Face* face, float* plane);
		DLLExport int AddFaceIndice(Leadwerks::Face* face, int v);
		DLLExport void SetFaceMappingPosition(Leadwerks::Face* face, float x, float y);
		DLLExport void SetFaceMappingRotation(Leadwerks::Face* face, float x, float y);
		DLLExport void SetFaceMappingScale(Leadwerks::Face* face, float x, float y);
		DLLExport void SetFaceMaterial(Leadwerks::Face* face, Leadwerks::Material* material);
		DLLExport Leadwerks::Material* GetFaceMaterial(Leadwerks::Face* face);
		DLLExport void SetFaceSmoothGroups(Leadwerks::Face* face, int smoothgroups);
		DLLExport int GetFaceSmoothGroups(Leadwerks::Face* face);
		DLLExport void HideFace(Leadwerks::Face* face);
		DLLExport void ShowFace(Leadwerks::Face* face);
		DLLExport int FaceHidden(Leadwerks::Face* face);
		DLLExport int CountFaceIndices(Leadwerks::Face* face);
		DLLExport int GetFaceIndiceVertex(Leadwerks::Face* face, int index);
		DLLExport void SetFaceSelectionState(Leadwerks::Face* face, int selectionstate);
		DLLExport int GetFaceSelectionState(Leadwerks::Face* face);
		DLLExport void GetFaceTextureMappingScale(Leadwerks::Face* face, float* f);
		DLLExport void GetFaceTextureMappingRotation(Leadwerks::Face* face, float* f);
		DLLExport void GetFaceTextureMappingTranslation(Leadwerks::Face* face, float* f);
		DLLExport void SetFaceTextureMappingTranslation(Leadwerks::Face* face, float x, float y);
		DLLExport void SetFaceTextureMappingRotation(Leadwerks::Face* face, float x, float y);
		DLLExport void SetFaceTextureMappingScale(Leadwerks::Face* face, float x, float y);

		//Transform
		DLLExport void TransformPointMatrix(float x, float y, float z, float* src, float* dst, float* result);
		DLLExport void TransformVectorMatrix(float x, float y, float z, float* src, float* dst, float* result);
		DLLExport void TransformPoint(float x, float y, float z, Leadwerks::Entity* src, Leadwerks::Entity* dst, float* result);
		DLLExport void TransformVector(float x, float y, float z, Leadwerks::Entity* src, Leadwerks::Entity* dst, float* result);
		DLLExport void TransformNormal(float x, float y, float z, Leadwerks::Entity* src, Leadwerks::Entity* dst, float* result);
		DLLExport void TransformAABB(float* aabb, Leadwerks::Entity* src, Leadwerks::Entity* dst, int slow, float* result);
		DLLExport void TransformPlane(float x, float y, float z, float d, Leadwerks::Entity* src, Leadwerks::Entity* dst, float* result);

		//Bank
		DLLExport int GetBankSize(Leadwerks::Bank* bank);
		DLLExport int PeekBankByte(Leadwerks::Bank* bank, int position);
		DLLExport char* GetBankBuffer(Leadwerks::Bank* bank);
		DLLExport Leadwerks::Bank* LoadBank(BLITZSTRING path, uint64_t fileid);

		//Object
        DLLExport void ReleaseObject(Leadwerks::Object* object);
        DLLExport void AddObjectRef(Leadwerks::Object* object);
        DLLExport int GetObjectRefCount(Leadwerks::Object* object);
		DLLExport void ObjectAddHook(Leadwerks::Object* object, int hookid, void* hook);
		DLLExport void ObjectRemoveHook(Leadwerks::Object* object, int hookid, void* hook);
		DLLExport void PrintObject(Leadwerks::Object* o);
		DLLExport void DeleteObject(Leadwerks::Object* o);
		DLLExport int GetObjectClass(Leadwerks::Object* o);
		DLLExport void SetObjectUserData(Leadwerks::Object* o, void* userdata);
		DLLExport void* GetObjectUserData(Leadwerks::Object* o);
		DLLExport void AddObjectHook(Leadwerks::Object* object, int hookid, void* hook);
		//DLLExport void RemoveObjectHook(Leadwerks::Object* object, int hookid, void* hook);

		//Interpreter
		DLLExport void InterpreterAllowLuaErrors();
		DLLExport int EvaluateInterpreterFile(BLITZSTRING path);
		DLLExport int EvaluateInterpreterString(BLITZSTRING source);
		DLLExport int InterpreterExecuteString(BLITZSTRING source);

		//Graphics driver
#ifdef LEADWERKS_3_1
		DLLExport Leadwerks::OpenGL4GraphicsDriver* CreateOpenGL4GraphicsDriver(Leadwerks::Context* sharedcontext);
#endif
		DLLExport int GetGraphicsDriverMaxTextureSamples(Leadwerks::GraphicsDriver* driver);
		//DLLExport Leadwerks::OpenGL1GraphicsDriver* CreateOpenGL1GraphicsDriver();
        //#ifdef OS_WINDOWS
        //    DLLExport Leadwerks::OpenGL3GraphicsDriver* CreateOpenGL3GraphicsDriver();
        //#endif
        //#ifdef MACOS
		//DLLExport Leadwerks::OpenGL2GraphicsDriver* CreateOpenGL2GraphicsDriver(Leadwerks::Context* sharedcontext);
        //#else
        //DLLExport Leadwerks::OpenGL2GraphicsDriver* CreateOpenGL2GraphicsDriver();
        //#endif
		DLLExport void ResetGraphicsDriver(Leadwerks::GraphicsDriver* graphicsdriver);
        DLLExport void SetGraphicsDriver(Leadwerks::GraphicsDriver* graphicsdriver);
		DLLExport Leadwerks::GraphicsDriver* GetGraphicsDriver();
		DLLExport int GetGraphicsDriverName(Leadwerks::GraphicsDriver* graphicsdriver, char* s, int length);
		DLLExport int GetGraphicsDriverVendor(Leadwerks::GraphicsDriver* graphicsdriver);
		DLLExport int GraphicsDriverSupported(Leadwerks::GraphicsDriver* graphicsdriver);
		DLLExport int BuffersSupported();
		DLLExport void GraphicsDriverReloadAutoShaders(Leadwerks::GraphicsDriver* graphicsdriver);
        //DLLExport int InitializeGraphicsDriver(Leadwerks::GraphicsDriver* graphicsdriver);
        #ifdef OS_MACOS
            DLLExport void SetGraphicsDriverSharedContext(Leadwerks::GraphicsDriver* graphicsdriver,Leadwerks::Context* context);
        #endif
        DLLExport int CountActiveTextures();
		DLLExport int CountActiveModels();

		//Timing
		DLLExport float UPS();
		DLLExport int Millisecs();
		DLLExport void UpdateTime();
		DLLExport void PauseTime();
		DLLExport void ResumeTime();

		//Network
		DLLExport Leadwerks::Server* CreateServer(int port);
		DLLExport Leadwerks::Message* UpdateServer(Leadwerks::Server* server, int timeout);
		DLLExport void DisconnectServer(Leadwerks::Server* server, Leadwerks::Peer* client, int force);
		DLLExport int SendServer(Leadwerks::Server* server, Leadwerks::Peer* client, int messageid, char* data, int size, int channel, int flags);

		//System
		DLLExport void SetSystemCallback(int id, char* funcptr);

		//File system
		DLLExport void SetDir(BLITZSTRING path);
		DLLExport int GetFileType(BLITZSTRING path, uint64_t fileid);

		//Light
		DLLExport void LightGetRange2(Leadwerks::Light* light, float* range);
		DLLExport void LightSetRange2(Leadwerks::Light* light, float x, float y);
		DLLExport Leadwerks::DirectionalLight* CreateDirectionalLight(Leadwerks::Entity* parent);
		DLLExport Leadwerks::PointLight* CreatePointLight(Leadwerks::Entity* parent);
		DLLExport Leadwerks::SpotLight* CreateSpotLight(Leadwerks::Entity* parent);
		DLLExport void SpotLightSetConeAngles(Leadwerks::SpotLight* spotlight, float outer, float inner);
		DLLExport void SpotLightGetConeAngles(Leadwerks::SpotLight* spotlight, float* result);
        DLLExport void LightSetRange(Leadwerks::Light* light, float range);
        DLLExport float LightGetRange(Leadwerks::Light* light);

		//Font
		DLLExport Leadwerks::Font* LoadFont(BLITZSTRING path, int size, int style, int family, int flags, uint64_t fileid);
		DLLExport Leadwerks::Font* LoadFontFromStream(Leadwerks::Stream* stream, int size, int style, int family);
		DLLExport Leadwerks::Texture* GetFontTexture(Leadwerks::Font* font);
		DLLExport void DrawFontText(Leadwerks::Font* font, BLITZSTRING text, int x, int y, float kerning);
		DLLExport int GetFontTextWidth(Leadwerks::Font* font, BLITZSTRING text);
		DLLExport int GetFontHeight(Leadwerks::Font* font);
		DLLExport void FreeFont(Leadwerks::Font* font);

		//Drawing
		DLLExport void SetViewport(int x, int y, int width, int height);
		DLLExport void SetColor(float r, float g, float b, float a);
        DLLExport void GetColor(float* color);
		DLLExport void SetClearColor(float r, float g, float b, float a);
		DLLExport void SetBlendMode(int blendmode);
		DLLExport void DrawLine(int x0, int y0, int x1, int y1);
		DLLExport void DrawRect(int x, int y, int width, int height, int style);
		DLLExport void DrawRectBatch(float* f, float* c, int count);
		DLLExport void DrawImage(Leadwerks::Texture* image, int x, int y, int width, int height);
		DLLExport void TileImage(Leadwerks::Texture* image);
		DLLExport void SetRotation(float rotation);
		DLLExport void SetTranslation(float x, float y);
		DLLExport void SetScale(float x, float y);

		//Window
		DLLExport Leadwerks::Window* CreateWindow(BLITZSTRING title, int x, int y, int width, int height, int style);
 #ifdef _WIN32
        DLLExport Leadwerks::Window* CreateCustomWindow(HWND hwnd);
 #endif
 #ifdef __linux__
    #ifndef _ANDROID_
        DLLExport Leadwerks::Window* CreateCustomWindow(XID xid);
    #endif
 #endif

        //#ifdef OS_MACOS
        //        DLLExport Leadwerks::Window* CreateCustomWindow(NSView nsview);
        //#endif
        DLLExport int GetWindowWidth(Leadwerks::Window* window);
		DLLExport int GetWindowHeight(Leadwerks::Window* window);
		DLLExport void SetWindowShape(Leadwerks::Window* window, int x, int y, int width, int height);
		DLLExport void FreeWindow(Leadwerks::Window* window);
		DLLExport int WindowKeyHit(Leadwerks::Window* window,int keycode);

		//Context
		DLLExport Leadwerks::Font* ContextGetFont(Leadwerks::Context* context);
		DLLExport void ContextSetFont(Leadwerks::Context* context, Leadwerks::Font* font);
		DLLExport void ContextDrawText(Leadwerks::Context* context, BLITZSTRING text, int x, int y);
		DLLExport void DrawContextStats(Leadwerks::Context* context, int x, int y);
		DLLExport Leadwerks::Context* CreateContext(Leadwerks::Window* window,int multisamplemode);
		DLLExport void SyncContext(Leadwerks::Context* context, int sync);
		//DLLExport Leadwerks::CustomContext* CreateCustomContext( char* extra, int GetWidth(char* extra), int GetHeight(char* extra), void MakeCurrent(char* extra), void Sync(char* extra,int sync), void Clear(void) );
#ifdef OS_MACOS
		//DLLExport Leadwerks::CustomContext* CreateCustomContext( char* extra, int (*GetWidth)(char*), int (*GetHeight) (char*), void (*MakeCurrent) (char*), void (*Sync) (char*,int sync), void Clear(char* extra, int) );
#endif

		//AssetReference
		DLLExport void AssetReset(Leadwerks::Asset* asset);
		DLLExport void AssetPathRemove(BLITZSTRING s, uint64_t fileid);
		DLLExport void ReloadAssets();
		DLLExport Leadwerks::Asset* FindAsset(BLITZSTRING s, uint64_t fileid);
		//DLLExport int ReloadAsset(Leadwerks::Asset* asset, int flags);
		DLLExport void SetAssetName(Leadwerks::Asset* asset, BLITZSTRING name);
		//DLLExport int CountAssetReferences();
		//DLLExport Leadwerks::AssetReference* GetAssetReference(int n);
		//DLLExport int GetAssetReferenceName(Leadwerks::AssetReference* assetreference);
		//DLLExport int CountAssetReferenceInstances(Leadwerks::AssetReference* assetreference);

		//Asset
		DLLExport void ReloadShader(BLITZSTRING path, int flags, uint64_t fileid);
		DLLExport Leadwerks::Asset* AssetInstance(Leadwerks::Asset* asset);
		DLLExport int GetAssetName(Leadwerks::Asset* asset, char* s, int length);
		DLLExport Leadwerks::AssetReference* GetAssetReference(Leadwerks::Asset* asset);
		DLLExport int ReloadAsset(Leadwerks::Asset* asset, int flags);
		DLLExport void GetAssetFileID(Leadwerks::Asset* asset, char* buf);

		//Mat4
		DLLExport void MultiplyMat4(float* mat0, float* mat1, float* result);
		DLLExport void GetMat4Scale(float* mat, float* result);
		DLLExport void Mat4FromRotation(float pitch, float yaw, float roll, float* result);

		//Shader
		//DLLExport void CompileShader(char* path);
		DLLExport Leadwerks::Shader* CreateShader();
		DLLExport Leadwerks::Shader* LoadShader(BLITZSTRING path, int flags, BLITZSTRING precode, uint64_t fileid);
		DLLExport void SetShader(Leadwerks::Shader* shader);
		//DLLExport int ReloadShader(Leadwerks::Shader* shader, int flags);
		DLLExport int SetShaderInt(Leadwerks::Shader* shader, BLITZSTRING name, int f);
		DLLExport int SetShaderFloat(Leadwerks::Shader* shader, BLITZSTRING name, float f);
		DLLExport int SetShaderVec2(Leadwerks::Shader* shader, BLITZSTRING name, float x, float y);
		DLLExport int SetShaderVec3(Leadwerks::Shader* shader, BLITZSTRING name, float x, float y, float z);
		DLLExport int SetShaderVec4(Leadwerks::Shader* shader, BLITZSTRING name, float x, float y, float z, float w);
		DLLExport int SetShaderMat4(Leadwerks::Shader* shader, BLITZSTRING name, float* mat);
		DLLExport void FreeShader(Leadwerks::Shader* shader);
		DLLExport int GetShaderSource(Leadwerks::Shader* shader, int shaderid, char* s, int length);
		DLLExport int GetShaderLog(Leadwerks::Shader* shader, char* s, int length);
		DLLExport void SetShaderSource(Leadwerks::Shader* shader, BLITZSTRING source, int shaderid);
		DLLExport int CompileShader(Leadwerks::Shader* shader, int shaderid);
		DLLExport int LinkShader(Leadwerks::Shader* shader);

		//Emitter
		DLLExport Leadwerks::Emitter* EmitterCreate(int particlecount, Leadwerks::Entity* parent);
		DLLExport void EmitterGetVelocity(Leadwerks::Emitter* emitter, int index, float* f);
		DLLExport void EmitterSetVelocity(Leadwerks::Emitter* emitter, float x,float y,float z,int index);
		DLLExport void EmitterSetAcceleration(Leadwerks::Emitter* emitter, float x,float y,float z);
		DLLExport void EmitterGetAcceleration(Leadwerks::Emitter* emitter, float* f);
		DLLExport void EmitterSetEmissionShape(Leadwerks::Emitter* emitter, int ShapeId);
		DLLExport int EmitterGetEmissionShape(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetDuration(Leadwerks::Emitter* emitter, int Duration);
		DLLExport int EmitterGetDuration(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetRotationSpeed(Leadwerks::Emitter* emitter, float speed);
		DLLExport float EmitterGetRotationSpeed(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetEmissionVolume(Leadwerks::Emitter* emitter, float x, float y, float z);
		DLLExport void EmitterGetEmissionVolume(Leadwerks::Emitter* emitter, float* f);
		DLLExport void EmitterSetReleaseQuantity(Leadwerks::Emitter* emitter,int quantity);
		DLLExport int EmitterGetReleaseQuantity(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetParticleCount(Leadwerks::Emitter* emitter, int ParticleCount);
		DLLExport int EmitterGetParticleCount(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetRotationByVelocityMode(Leadwerks::Emitter* emitter, int setByVel);
		DLLExport int EmitterGetRotationByVelocityMode(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetLoopMode(Leadwerks::Emitter* emitter, int loop);
		DLLExport int EmitterGetLoopMode(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetPosition(Leadwerks::Emitter* emitter, float x,float y,float z,int global);
		DLLExport void EmitterPause(Leadwerks::Emitter* emitter);
		DLLExport void EmitterPlay(Leadwerks::Emitter* emitter);
		DLLExport int EmitterGetPaused(Leadwerks::Emitter* emitter);
		DLLExport void EmitterAddAlphaControlPoint(Leadwerks::Emitter* emitter, float time, float alpha);
		DLLExport void EmitterAddScaleControlPoint(Leadwerks::Emitter* emitter, float time, float scale);
		DLLExport void EmitterSetViewMode(Leadwerks::Emitter* emitter,int viewmode);
		DLLExport int EmitterGetViewMode(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetAnimationRows(Leadwerks::Emitter* emitter, int Rows);
		DLLExport int EmitterGetAnimationRows(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetAnimationColumns(Leadwerks::Emitter* emitter, int Columns);
		DLLExport int EmitterGetAnimationColumns(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetAnimationCycles(Leadwerks::Emitter* emitter, int Cycles);
		DLLExport int EmitterGetAnimationCycles(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetFacingDirection(Leadwerks::Emitter* emitter,float x,float y,float z);
		DLLExport void EmitterGetFacingDirection(Leadwerks::Emitter* emitter, float* f);
        DLLExport void EmitterClearAlphaControlPoints(Leadwerks::Emitter* emitter);
		DLLExport void EmitterClearScaleControlPoints(Leadwerks::Emitter* emitter);
        DLLExport int EmitterCountAlphaControlPoints(Leadwerks::Emitter* emitter);
        DLLExport float EmitterGetAlphaControlPointTime(Leadwerks::Emitter* emitter, int index);
        DLLExport float EmitterGetAlphaControlPointValue(Leadwerks::Emitter* emitter, int index);
		DLLExport float EmitterGetScaleControlPointTime(Leadwerks::Emitter* emitter, int index);
        DLLExport float EmitterGetScaleControlPointValue(Leadwerks::Emitter* emitter, int index);
		DLLExport int EmitterCountScaleControlPoints(Leadwerks::Emitter* emitter);
		DLLExport void EmitterSetMaxScale(Leadwerks::Emitter* emitter, float x, float y);
		DLLExport void EmitterGetMaxScale(Leadwerks::Emitter* emitter, float* f);
        DLLExport void EmitterSetParticleColor(Leadwerks::Emitter* emitter, float r, float g, float b, float a, int mode);
        DLLExport void EmitterGetParticleColor(Leadwerks::Emitter* emitter, int mode, float* result);

		//Attractor
		DLLExport Leadwerks::Attractor* AttractorCreate(float Range,float Force, Leadwerks::Entity* parent);
		DLLExport float AttractorGetForce(Leadwerks::Attractor* attractor);
		DLLExport void AttractorSetForce(Leadwerks::Attractor* attractor, float force);
		DLLExport float AttractorGetRange(Leadwerks::Attractor* attractor);
		DLLExport void AttractorSetRange(Leadwerks::Attractor* attractor, float range);
		DLLExport int AttractorGetAlphaMode(Leadwerks::Attractor* attractor);
		DLLExport void AttractorSetAlphaMode(Leadwerks::Attractor* attractor,int mode);

		//Texture
		DLLExport void TextureSetLoadingMode(int mode);
		DLLExport void TextureSetTrilinearFilterMode(int trilinearfilter);
		DLLExport int TextureGetTrilinearFilterMode();
		DLLExport float TextureGetAnisotropy();
		DLLExport void TextureSetAnisotropy(float anisotropy);
		DLLExport float TextureGetMaxAnisotropy();
		DLLExport Leadwerks::Texture* GetTerrainBaseVirtualTexture(Leadwerks::Terrain* terrain);
		DLLExport void BuildTextureMipmaps(Leadwerks::Texture* texture);
		DLLExport int GetTextureTarget(Leadwerks::Texture* texture);
		DLLExport Leadwerks::Texture* LoadTexture(BLITZSTRING path, int flags, uint64_t fileid);
		DLLExport Leadwerks::Texture* CreateTexture(int width, int height, int format, int flags, int frames, int samples);
		//DLLExport void LockTexture(Leadwerks::Texture* texture, int miplevel, int framenumber);
		//DLLExport void UnlockTexture(Leadwerks::Texture* texture, int miplevel, int framenumber);
		//DLLExport void WriteTexturePixel(Leadwerks::Texture* texture, int x, int y, int r, int g, int b, int a, int miplevel, int framenumber);
		//DLLExport int ReadTexturePixel(Leadwerks::Texture* texture, int x, int y, int miplevel, int framenumber);
		DLLExport void GetTexturePixels(Leadwerks::Texture* texture, char* buf, int miplevel, int framenumber, int cubeface);
		DLLExport void SetTexturePixels(Leadwerks::Texture* texture, char* buf, int miplevel, int framenumber, int cubeface);
		DLLExport int GetTextureWidth(Leadwerks::Texture* texture, int miplevel);
		DLLExport int GetTextureHeight(Leadwerks::Texture* texture, int miplevel);
		DLLExport int GetTextureDepth(Leadwerks::Texture* texture, int miplevel);
        DLLExport void FreeTexture(Leadwerks::Texture* texture);
		DLLExport void SetTextureFilter(Leadwerks::Texture* texture, int filter);
		//DLLExport int TextureLocked(Leadwerks::Texture* texture, int miplevel, int framenumber);
		//DLLExport int ReloadTexture(Leadwerks::Texture* texture, int flags);
		DLLExport void BindTexture(Leadwerks::Texture* texture, int index);
		DLLExport char* GetTextureData(Leadwerks::Texture* texture, int miplevel, int framenumber);
		DLLExport int CountTextureMipmaps(Leadwerks::Texture* texture);
		DLLExport int GetTextureFormat(Leadwerks::Texture* texture);
		DLLExport int GetTextureMipmapSize(Leadwerks::Texture* texture, int mipmap);
		DLLExport void SetTextureClampMode(Leadwerks::Texture* texture, int x, int y, int z);
		DLLExport int GetTextureClampMode(Leadwerks::Texture* texture, int axis);
		DLLExport float GetTextureAnisotropy(Leadwerks::Texture* texture);
		DLLExport void SetTextureAnisotropy(Leadwerks::Texture* texture, float anisotropy);
		DLLExport int GetTextureFilterMode(Leadwerks::Texture* texture);

		//Material
		DLLExport void SetMaterialDecalMode(Leadwerks::Material* material, int rendermode);
		DLLExport int GetMaterialDecalMode(Leadwerks::Material* material);
		DLLExport Leadwerks::Material* MaterialLoadBestGuess(char* path, int flags);
		DLLExport int GetMaterialWidth(Leadwerks::Material* material);
		DLLExport int GetMaterialHeight(Leadwerks::Material* material);
		DLLExport void SetMaterialMappingScale(Leadwerks::Material* material, float x, float y, float z);
		DLLExport void GetMaterialMappingScale(Leadwerks::Material* material, float* f);
		DLLExport int GetMaterialDrawMode(Leadwerks::Material* material);
		DLLExport void SetMaterialDrawMode(Leadwerks::Material* material, int mode);
		DLLExport int MaterialGetAlwaysUseShader(Leadwerks::Material* material);
		DLLExport void MaterialSetAlwaysUseShader(Leadwerks::Material* material, int mode);
		DLLExport void MaterialSetLayer(Leadwerks::Material* material, int layer);
		DLLExport int MaterialGetLayer(Leadwerks::Material* material);
		DLLExport void SetMaterialDepthMaskMode(Leadwerks::Material* material, int mode);
		DLLExport int GetMaterialDepthMaskMode(Leadwerks::Material* material);
		DLLExport Leadwerks::Material* CreateMaterial();
		DLLExport void SetMaterialColor(Leadwerks::Material* material, float r, float g, float b, float a, int mode);
		DLLExport void GetMaterialColor(Leadwerks::Material* material, float* color, int mode);
		DLLExport int MaterialContainsValue(Leadwerks::Material* material, BLITZSTRING name);
		DLLExport void SetMaterialShader(Leadwerks::Material* material, Leadwerks::Shader* shader, int index);
		DLLExport Leadwerks::Material* LoadMaterial(BLITZSTRING path, int flags, uint64_t fileid);
		DLLExport Leadwerks::Texture* GetMaterialTexture(Leadwerks::Material* material, int index);
		DLLExport void SetMaterialTexture(Leadwerks::Material* material, Leadwerks::Texture* texture, int index);
		//DLLExport int ReloadMaterial(Leadwerks::Material* material, int flags);
		DLLExport Leadwerks::Shader* GetMaterialShader(Leadwerks::Material* material, int index);
        DLLExport void SetMaterialLighting(Leadwerks::Material* material,int mode);
		DLLExport int GetMaterialLightingMode(Leadwerks::Material* material);
		DLLExport void FreeMaterial(Leadwerks::Material* material);
		DLLExport void SetMaterialBlendMode(Leadwerks::Material* material, int mode);
		DLLExport int GetMaterialBlendMode(Leadwerks::Material* material);
		DLLExport void SetMaterialBackFaceCullMode(Leadwerks::Material* material, int mode);
		DLLExport int GetMaterialBackFaceCullMode(Leadwerks::Material* material);
		DLLExport void SetMaterialShadowMode(Leadwerks::Material* material, int mode);
		DLLExport int GetMaterialShadowMode(Leadwerks::Material* material);
		DLLExport void SetMaterialDepthTestMode(Leadwerks::Material* material, int mode);
		DLLExport int GetMaterialDepthTestMode(Leadwerks::Material* material);
		DLLExport void SetMaterialSortMode(Leadwerks::Material* material, int mode);
		DLLExport int GetMaterialSortMode(Leadwerks::Material* material);
		DLLExport void SetMaterialFloat(Leadwerks::Material* material, BLITZSTRING name, float x);
		DLLExport void SetMaterialVec2(Leadwerks::Material* material, BLITZSTRING name, float x, float y);
		DLLExport void SetMaterialVec3(Leadwerks::Material* material, BLITZSTRING name, float x, float y, float z);
		DLLExport void SetMaterialVec4(Leadwerks::Material* material, BLITZSTRING name, float x, float y, float z, float w);
		DLLExport float GetMaterialFloat(Leadwerks::Material* material, BLITZSTRING name);
		DLLExport void GetMaterialVec4(Leadwerks::Material* material, BLITZSTRING name, float* v);
		//DLLExport Leadwerks::Shader* ChooseMaterialShader(Leadwerks::Material* material);
		DLLExport void SetMaterialStippleMode(Leadwerks::Material* material, int mode);

		//Buffer
		DLLExport void GetBufferPixels(Leadwerks::Buffer* buffer, char* buf, int component);
		DLLExport void SetBuffer(Leadwerks::Buffer* buffer);
		DLLExport Leadwerks::Buffer* GetBuffer();
		DLLExport int GetBufferWidth(Leadwerks::Buffer* buffer);
		DLLExport int GetBufferHeight(Leadwerks::Buffer* buffer);
		DLLExport Leadwerks::Buffer* CreateBuffer(int width, int height, int colortextures, int depthtexture, int multisamplemode);
		DLLExport Leadwerks::Texture* GetBufferColorTexture(Leadwerks::Buffer* buffer, int index);
		DLLExport Leadwerks::Texture* GetBufferDepthTexture(Leadwerks::Buffer* buffer);
		DLLExport void ClearBuffer(Leadwerks::Buffer* buffer, int mode);
		DLLExport void FreeBuffer(Leadwerks::Buffer* buffer);
		DLLExport int SetBufferColorTexture(Leadwerks::Buffer* buffer,Leadwerks::Texture* texture, int index, int cubeface);

		//World
		DLLExport void SetWorldWaterMode(Leadwerks::World* world, int mode);
		DLLExport void SetWorldWaterColor(Leadwerks::World* world, float r, float g, float b, float a);
		DLLExport void SetWorldWaterHeight(Leadwerks::World* world, float height);
		DLLExport int GetWorldWaterMode(Leadwerks::World* world);
		DLLExport void GetWorldWaterColor(Leadwerks::World* world, float* f);
		DLLExport float GetWorldWaterHeight(Leadwerks::World* world);
		DLLExport void SetWorldTessellationQuality(Leadwerks::World* world, float tessfactor);
		DLLExport float GetWorldTessellationQuality(Leadwerks::World* world);
		DLLExport void SetWorldGravity(Leadwerks::World* world, float x, float y, float z);
		DLLExport void GetWorldGravity(Leadwerks::World* world, float* gravity);
		DLLExport void SetWorldSize(Leadwerks::World* world, float size);
		DLLExport float GetWorldSize(Leadwerks::World* world);
		DLLExport void SetWorldLightQuality(Leadwerks::World* world, int lightquality);
		DLLExport int GetWorldLightQuality(Leadwerks::World* world);
		DLLExport void SerializeNavTile(Leadwerks::World* world, int x, int y, Leadwerks::Stream* stream);
		DLLExport void DeserializeNavTile(Leadwerks::World* world, int x, int y, Leadwerks::Stream* stream);
		DLLExport int GetWorldNavTileSize(Leadwerks::World* world, int x, int y);
		DLLExport int GetWorldNavmeshTileCount(Leadwerks::World* world);
		DLLExport int BuildWorldNavTile(Leadwerks::World* world, int x, int y, float maxedgelength, float maxedgeerror);
		//, float agentradius, float agentheight);
		//DLLExport int BuildWorldNavMesh(Leadwerks::World* world, float maxedgelength, int callback(float progress));
		//DLLExport void WorldInvalidateProjectedShadowsInAABBCallback(Leadwerks::Entity* entity, Leadwerks::Object* extra);
		//DLLExport void WorldInvalidateProjectedShadowsInAABB(Leadwerks::World* world, float* f);
		DLLExport void ClearWorld(Leadwerks::World* world);
		DLLExport void ClearWorldNavMesh(Leadwerks::World* world);
		DLLExport Leadwerks::World* CreateWorld();
		DLLExport void GetWorldAABB(Leadwerks::World* world, float* aabb);
		DLLExport void RenderWorld(Leadwerks::World* world);
		DLLExport void WorldUpdate(Leadwerks::World* world);
		DLLExport void SetWorld(Leadwerks::World* world);
		DLLExport Leadwerks::World* GetWorld();
		DLLExport void FreeWorld(Leadwerks::World* world);
		DLLExport void SetWorldAmbientLight(Leadwerks::World* world, float r, float g, float b, float a);
		DLLExport void WorldGetAmbientLight(Leadwerks::World* world, float* f);
		DLLExport void WorldForEachEntityInAABBDo(Leadwerks::World* world, float* aabb, void callback(Leadwerks::Entity* entity, Leadwerks::Object* extra), Leadwerks::Object* extra);
		DLLExport int WorldPick(Leadwerks::World* world, float x0, float y0, float z0, float x1, float y1, float z1, float* pick,float radius,int closest,int collisiontype);

		//Sprite
		DLLExport Leadwerks::Sprite* SpriteCreate(Leadwerks::Entity* parent);
		DLLExport void SpriteSetSize(Leadwerks::Sprite* sprite, float width, float height);
		DLLExport void SpriteGetSize(Leadwerks::Sprite* sprite, float* f);
		DLLExport int SpriteGetViewMode(Leadwerks::Sprite* sprite);
		DLLExport void SpriteSetViewMode(Leadwerks::Sprite* sprite, int mode);

		//Entity
		DLLExport void ModelStripVertexColors(Leadwerks::Entity* entity);
		DLLExport void SetEntityEuler(Leadwerks::Entity* entity, float x, float y, float z);
		DLLExport void GetEntityQuaternion(Leadwerks::Entity* entity, int global, float* f);
		DLLExport void SetEntityQuaternion(Leadwerks::Entity* entity, float x, float y, float z, float w, int global);
		DLLExport void EntitySetAutoShape(Leadwerks::Entity* entity, int mode);
		DLLExport int BuildEntityShapeFromLimbs(Leadwerks::Entity* entity);
		DLLExport void EntitySetEditorViewMode(Leadwerks::Entity* entity, int mode);
		DLLExport void EntitySetAnimationName(Leadwerks::Entity* entity, int index, BLITZSTRING buffer);
		DLLExport int EntityGetAnimationName(Leadwerks::Entity* entity, int index, char* buffer, int length);
		//DLLExport int EntityGenerateConvexHull(Leadwerks::Entity* entity, int recursive);
		//DLLExport int EntityGenerateConvexDecomposition(Leadwerks::Entity* entity, int recursive);
		DLLExport void UpdateEntityBones(Leadwerks::Entity* entity);
        DLLExport void SetEntityPhysicsMode(Leadwerks::Entity* entity, int mode);
        DLLExport int GetEntityPhysicsMode(Leadwerks::Entity* entity);
		DLLExport void EntityRemoveAnimationSequence(Leadwerks::Entity* entity, int index);
		DLLExport void EntitySetPickRadius(Leadwerks::Entity* entity, float radius);
		DLLExport int EntityExtractAnimationSequence(Leadwerks::Entity* entity, int index, int startframe, int length);
		DLLExport void EntitySetPickMode(Leadwerks::Entity* entity, int pickmode);
		DLLExport int EntityGetPickMode(Leadwerks::Entity* entity);
		DLLExport int EntityGetPrefabPath(Leadwerks::Entity* entity, char* buffer, int length);
		DLLExport void EntityMoveToBack(Leadwerks::Entity* entity);
		DLLExport int EntityLoadAnimation(Leadwerks::Entity* entity, BLITZSTRING path);
		DLLExport void EntitySetAnimationFrame(Leadwerks::Entity* entity, float time, float blend, int index, int recursive);
		DLLExport void EntityGetAnimationFrame(Leadwerks::Entity* entity, int index, int sequence, float* f);
		DLLExport int EntityCountAnimations(Leadwerks::Entity* entity);
		DLLExport int EntityGetAnimationLength(Leadwerks::Entity* entity, int index, int recursive);
        DLLExport Leadwerks::Bone* EntityGetBone(Leadwerks::Entity* entity, int index);
		DLLExport void EntitySetCollisionType(Leadwerks::Entity* entity, int collisiontype);
		DLLExport int EntityGetCollisionType(Leadwerks::Entity* entity);
        DLLExport void EntitySetSweptCollisionMode(Leadwerks::Entity* entity, int mode);
        DLLExport int EntityGetSweptCollisionMode(Leadwerks::Entity* entity);
		DLLExport void EntitySetShape(Leadwerks::Entity* entity, Leadwerks::Shape* shape);
		DLLExport Leadwerks::Shape* EntityGetShape(Leadwerks::Entity* entity);
		DLLExport void EntityGetShapePosition(Leadwerks::Entity* entity, float* f);
		DLLExport void EntityGetShapeRotation(Leadwerks::Entity* entity, float* f);
		DLLExport void EntityGetShapeScale(Leadwerks::Entity* entity, float* f);
		//DLLExport void EntityBuildShape(Leadwerks::Entity* entity, int shapeid, float px, float py, float pz, float rx, float ry, float rz, float sx, float sy, float sz);
		//DLLExport int EntityGetShapeID(Leadwerks::Entity* entity);
		DLLExport void EntitySetNavigationMode(Leadwerks::Entity* entity, int mode);
		DLLExport int EntityGetNavigationMode(Leadwerks::Entity* entity);
		DLLExport void EntitySetMass(Leadwerks::Entity* entity, float mass);
		DLLExport float EntityGetMass(Leadwerks::Entity* entity);
		DLLExport void EntitySetViewRange(Leadwerks::Entity* entity, int viewrange);
		DLLExport int EntityGetViewRange(Leadwerks::Entity* entity);
		DLLExport int EntityPick(Leadwerks::Entity* entity, float x0, float y0, float z0, float x1, float y1, float z1, float* result, float radius, int closest, int recursive, int collisiontype);
		DLLExport float EntityGetDistanceToEntity(Leadwerks::Entity* entity0, Leadwerks::Entity* entity1);
		DLLExport void EntityDraw(Leadwerks::Entity* entity, Leadwerks::Camera* camera, int recursive, int drawsorted);
		DLLExport void SetEntityIntensity(Leadwerks::Entity* entity, float intensity, int mode, int recursive);
		DLLExport float GetEntityIntensity(Leadwerks::Entity* entity, int mode);
		//DLLExport void UpdateEntityParentRecursiveAABBAndOctreeNode(Leadwerks::Entity* entity);
		DLLExport Leadwerks::Entity* CopyEntity(Leadwerks::Entity* entity, int recursive);
		DLLExport Leadwerks::Entity* InstanceEntity(Leadwerks::Entity* entity, int recursive);
		DLLExport void SetEntityDrawPositionOverlayMode(Leadwerks::Entity* entity, int mode);
		DLLExport void SetEntitySelected(Leadwerks::Entity* entity, int selected);
		DLLExport int GetEntitySelected(Leadwerks::Entity* entity);
		DLLExport Leadwerks::Entity* GetEntityParent(Leadwerks::Entity* entity);
		DLLExport void SetEntityOcclusionCullingMode(Leadwerks::Entity* entity, int occlusioncullingmode);
		DLLExport void SetEntityViewRange(Leadwerks::Entity* entity, int viewrange);
		DLLExport void SetEntityShadowMode(Leadwerks::Entity* entity, int shadowmode);
		DLLExport void SetEntityKeyValue(Leadwerks::Entity* entity, BLITZSTRING name, BLITZSTRING value);
		DLLExport int GetEntityOcclusionCullingMode(Leadwerks::Entity* entity);
		DLLExport int GetEntityViewRange(Leadwerks::Entity* entity);
		DLLExport int GetEntityShadowMode(Leadwerks::Entity* entity);
		DLLExport void SetEntityColor(Leadwerks::Entity* entity, float r, float g, float b, float a, int mode, int recursive);
		DLLExport void GetEntityColor(Leadwerks::Entity* entity, float* color, int mode);
		DLLExport void SetEntityPosition(Leadwerks::Entity* entity,float x, float y, float z, int global);
		DLLExport void SetEntityRotation(Leadwerks::Entity* entity,float x, float y, float z, int global);
        DLLExport void TranslateEntity(Leadwerks::Entity* entity,float x, float y, float z, int global);
		DLLExport void SetEntityScale(Leadwerks::Entity* entity,float x, float y, float z);
		DLLExport void GetEntityPosition(Leadwerks::Entity* entity, float* position, int global);
		DLLExport void GetEntityRotation(Leadwerks::Entity* entity, float* rotation, int global);
		DLLExport void GetEntityScale(Leadwerks::Entity* entity, float* scale);
		DLLExport void GetEntityMatrix(Leadwerks::Entity* entity, float* mat, int global);
		DLLExport void GetEntityOriginalMatrix(Leadwerks::Entity* entity, float* mat, int global);
        DLLExport void SetEntityMatrix(Leadwerks::Entity* entity, float* mat, int global);
		DLLExport void MoveEntity(Leadwerks::Entity* entity,float x, float y, float z, int global);
		DLLExport void TurnEntity(Leadwerks::Entity* entity,float x, float y, float z, int global);
		DLLExport void SetEntityMaterial(Leadwerks::Entity* entity, Leadwerks::Material* material, int recursive);
		DLLExport Leadwerks::Material* GetEntityMaterial(Leadwerks::Entity* entity);
		DLLExport void GetEntityAABB(Leadwerks::Entity* entity, float* aabb, int mode);
		DLLExport void FreeEntity(Leadwerks::Entity* entity);
		DLLExport void HideEntity(Leadwerks::Entity* entity);
		DLLExport void ShowEntity(Leadwerks::Entity* entity);
		DLLExport int CountEntityChildren(Leadwerks::Entity* entity);
		DLLExport Leadwerks::Entity* GetEntityChild(Leadwerks::Entity* entity, int n);
		DLLExport void SetEntityParent(Leadwerks::Entity* entity, Leadwerks::Entity* parent, int global);
		DLLExport void AlignEntityToVector(Leadwerks::Entity* entity, float x, float y, float z, int axis, float rate, float roll);
		DLLExport int EntityHidden(Leadwerks::Entity* entity);
		DLLExport int EntityIsLimb(Leadwerks::Entity* entity);
		//DLLExport int SaveEntity(Leadwerks::Entity* entity, char* path);
		DLLExport int GetEntityKeyValue(Leadwerks::Entity* entity, BLITZSTRING name, BLITZSTRING defaultvalue, char* value, int length);
		DLLExport int CountEntityKeys(Leadwerks::Entity* entity);
		DLLExport int GetEntityKeyName(Leadwerks::Entity* entity, int index,char* buffer,int length);
		DLLExport void UpdateEntityAABB(Leadwerks::Entity* entity, int mode);
		//DLLExport int GetEntityKeyValue(int index,char* name, char* buffer,int length);

		//Pivot
		DLLExport Leadwerks::Pivot* CreatePivot(Leadwerks::Entity* parent);

		//Surface
		DLLExport void SetSurfaceVertexNormal(Leadwerks::Surface* surface, int index, float x, float y, float z);
		DLLExport void SurfaceUpdateSkinning(Leadwerks::Surface* surf, Leadwerks::Entity* entity);
		DLLExport void SetSurfaceVertexColor(Leadwerks::Surface* surface, int vertex, float r, float g, float b, float a);
		DLLExport void ClearSurface(Leadwerks::Surface* surf);
        DLLExport Leadwerks::Surface* SurfaceCreate();
        DLLExport int SurfaceGetVertexBoneIndice(Leadwerks::Surface* surface, int v, int index);
        DLLExport float SurfaceGetVertexBoneWeight(Leadwerks::Surface* surface, int v, int index);
		DLLExport int SurfacePick(Leadwerks::Surface* surface, float x0, float y0, float z0, float x1, float y1, float z1, float* result, float radius, int closest);
		DLLExport void UpdateSurfaceNormals(Leadwerks::Surface* surface, int mode, float distancetolerance, float angulartolerance);
		DLLExport void FlipSurfaceNormals(Leadwerks::Surface* surface);
		DLLExport Leadwerks::Material* GetSurfaceMaterial(Leadwerks::Surface* surface);
		DLLExport void SetSurfaceMaterial(Leadwerks::Surface* surface, Leadwerks::Material* material);
		DLLExport int GetSurfaceTriangleVertex(Leadwerks::Surface* surface, int index, int vertex);
		DLLExport int CountSurfaceVertices(Leadwerks::Surface* surface);
		DLLExport int CountSurfaceTriangles(Leadwerks::Surface* surface);
		DLLExport void SetSurfaceVertexPosition(Leadwerks::Surface* surface, int index, float x, float y, float z);
		DLLExport void GetSurfaceVertexPosition(Leadwerks::Surface* surface, int index, float* f);
		DLLExport void GetSurfaceVertexWeightedPosition(Leadwerks::Surface* surface, int index, float* f);
		DLLExport void GetSurfaceVertexNormal(Leadwerks::Surface* surface, int index, float* f);
		DLLExport void GetSurfaceVertexTexCoords(Leadwerks::Surface* surface, int index, float* f, int texcoordset);
		DLLExport void GetSurfaceVertexBinormal(Leadwerks::Surface* surface, int index, float* f);
		DLLExport void GetSurfaceVertexTangent(Leadwerks::Surface* surface, int index, float* f);
		DLLExport void GetSurfaceVertexColor(Leadwerks::Surface* surface, int index, float* f);
		DLLExport void SetSurfaceVertexBoneIndice(Leadwerks::Surface* surface, int vertex, int index, int boneindice);
		DLLExport void SetSurfaceVertexBoneWeight(Leadwerks::Surface* surface, int vertex, int index, float weight);
		DLLExport void GetSurfaceAABB(Leadwerks::Surface* surface, float* aabb);
		DLLExport int AddSurfaceVertex(Leadwerks::Surface* surface, float x, float y, float z, float nx, float ny, float nz, float u0, float v0, float u1, float v1, float r, float g, float b, float a);
		DLLExport int AddSurfaceIndice(Leadwerks::Surface* surface, int v);
		DLLExport void SetSurfaceDrawMode(Leadwerks::Surface* surface, int drawmode);
		DLLExport void UpdateSurfaceBinormalsAndTangents(Leadwerks::Surface* surface);
		DLLExport void Transform(float* mat);
		DLLExport void OptimizeSurface(Leadwerks::Surface* surface, float tolerance);
		DLLExport void UpdateSurfaceTexCoords(Leadwerks::Surface* surface, int mode, float* mat, float tilex, float tiley, int texcoordset);
		DLLExport void BuildSurfaceAABB(Leadwerks::Surface* surface);
		DLLExport void SurfaceAddTriangle(Leadwerks::Surface* surface,int a, int b, int c);
		DLLExport void SurfaceAddIndice(Leadwerks::Surface* surface, int i);

        //Bone
        DLLExport int BoneGetID(Leadwerks::Bone* bone);

		//Convex decomposition
		DLLExport Leadwerks::Model* BuildConvexDecomposition(Leadwerks::Entity* entity, int usercallback(float), int resolution, int depth, float concavity, int planeDownsampling, int convexhullDownsampling, float alpha, float beta, float gamma, int pca, int mode, int maxNumVerticesPerCH, float minVolumePerCH);

		//Model
		DLLExport void DeleteModelSurface(Leadwerks::Model* model, Leadwerks::Surface* surf);
		DLLExport Leadwerks::Model* ModelCylinder(int sides, Leadwerks::Entity* parent);
		DLLExport Leadwerks::Model*  CollapseModel(Leadwerks::Model* model);
		DLLExport Leadwerks::Model* CreateModel(Leadwerks::Entity* parent);
		DLLExport Leadwerks::Model* LoadModel(BLITZSTRING path, int flags, uint64_t fileid);
		DLLExport Leadwerks::Model* LoadModelFromStream(Leadwerks::Stream* stream, int flags);
		DLLExport Leadwerks::Model* CreateSphere(int segments, Leadwerks::Entity* parent);
		DLLExport Leadwerks::Model* CreateBox(float width, float height, float depth, Leadwerks::Entity* parent);
		DLLExport Leadwerks::Model* ModelCone(int segments,Leadwerks::Entity* parent);
		DLLExport void FlipModelNormals(Leadwerks::Model* model);
		DLLExport int CountModelSurfaces(Leadwerks::Model* model);
		DLLExport Leadwerks::Surface* GetModelSurface(Leadwerks::Model* model, int index);
		DLLExport Leadwerks::Surface* AddModelSurface(Leadwerks::Model* model);
        DLLExport Leadwerks::ModelFactory* GetModelFactory(Leadwerks::Model* model);
		//DLLExport Leadwerks::ModelReference* GetModelModelBase(Leadwerks::Model* model);
		DLLExport void ModelTranslateSurfaces(Leadwerks::Model* model, float x, float y, float z);
		DLLExport void ModelRotateSurfaces(Leadwerks::Model* model, float x, float y, float z);
		DLLExport void ModelScaleSurfaces(Leadwerks::Model* model, float x, float y, float z);

		//Decal
		DLLExport Leadwerks::Decal* CreateDecal(Leadwerks::Material* material, Leadwerks::Entity* parent);
		DLLExport void SetDecalRenderMode(Leadwerks::Decal* decal, int classid, int rendermode);
		DLLExport int GetDecalRenderMode(Leadwerks::Decal* decal, int classid);

		//ModelBase
		DLLExport int GetModelBasePath(Leadwerks::ModelFactory* modelfactory, char* s, int length);

		//Bank
		DLLExport Leadwerks::Bank* CreateBank(int size);
		DLLExport void PokeBankMem(Leadwerks::Bank* bank, int pos, char* buf, int size);

		//Bankstream
		DLLExport Leadwerks::BankStream* CreateBankStream(Leadwerks::Bank* bank, int takeownership);

		//Camera
		DLLExport int CameraSetRenderTarget(Leadwerks::Camera* camera, Leadwerks::Texture* texture);
		DLLExport Leadwerks::Texture* CameraGetRenderTarget(Leadwerks::Camera* camera);
		DLLExport void SetCameraSkybox(Leadwerks::Camera* camera, BLITZSTRING path, uint64_t fileid);
		DLLExport void CameraShowInvisibleMaterials(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraDrawSpritesMode(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraMultisampleMode(Leadwerks::Camera* camera, int mode);
		DLLExport int GetCameraMultisampleMode(Leadwerks::Camera* camera);
		DLLExport void UpdateCameraFrustum(Leadwerks::Camera* camera, float aspect);
		DLLExport void SetCameraGridColors(Leadwerks::Camera* camera, float* f);
		DLLExport void CameraSetRenderMode(Leadwerks::Camera* camera, int mode);
		DLLExport int CameraGetLightingMode(Leadwerks::Camera* camera);
        DLLExport void CameraSetDebugEntityBoxes(Leadwerks::Camera* camera, int mode);
        DLLExport int CameraGetDebugEntityBoxes(Leadwerks::Camera* camera);
        DLLExport void CameraSetDebugPhysicsMode(Leadwerks::Camera* camera, int mode);
        DLLExport void CameraSetDebugNavigationMode(Leadwerks::Camera* camera, int mode);
        DLLExport int CameraGetDebugPhysicsMode(Leadwerks::Camera* camera);
        DLLExport int CameraGetDebugNavigationMode(Leadwerks::Camera* camera);
		DLLExport void CameraDrawBatches(Leadwerks::Camera* camera);
		DLLExport void CameraDrawSortedObjects(Leadwerks::Camera* camera);
		DLLExport void SetCameraEditorOverlayMode(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraGridMode(Leadwerks::Camera* camera, int mode);
		DLLExport void ProjectCamera(Leadwerks::Camera* camera, float x, float y, float z, float* result);
		DLLExport void UnprojectCamera(Leadwerks::Camera* camera, float x, float y, float z, float* result);
		DLLExport void SetCameraGridSize(Leadwerks::Camera* camera, float size, int gridmajorlines);
		DLLExport int CameraPick(Leadwerks::Camera* camera,int x, int y, float* pick, float radius, int closest,int collisiontype);
		DLLExport Leadwerks::Camera* CreateCamera(Leadwerks::Entity* parent);
		DLLExport void SetCameraRange(Leadwerks::Camera* camera, float nearrange, float farrange);
		DLLExport void GetCameraRange(Leadwerks::Camera* camera, float* range);
		DLLExport void SetCameraProjectionMode(Leadwerks::Camera* camera, int projectionmode);
		DLLExport int GetCameraProjectionMode(Leadwerks::Camera* camera);
		DLLExport void SetCameraClearColor(Leadwerks::Camera* camera, float r, float g, float b, float a);
		DLLExport void GetCameraClearColor(Leadwerks::Camera* camera, float* color);
		DLLExport void SetCameraZoom(Leadwerks::Camera* camera, float zoom);
		DLLExport float GetCameraZoom(Leadwerks::Camera* camera);
		DLLExport void SetCameraDrawMode(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraTextureMode(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraLightingMode(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraColorMode(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraFOV(Leadwerks::Camera* camera, float fov);
		DLLExport float GetCameraFOV(Leadwerks::Camera* camera);
		DLLExport void SetCameraMotionBlurMode(Leadwerks::Camera* camera, int mode);
		DLLExport void SetCameraViewport(Leadwerks::Camera* camera, float x, float y, float width, float height);
		DLLExport void GetCameraViewport(Leadwerks::Camera* camera, float* viewport);
		DLLExport void CameraClearPostEffects(Leadwerks::Camera* camera);
		DLLExport int CameraAddPostEffect(Leadwerks::Camera* camera, BLITZSTRING fxfile, uint64_t fileid);
	}
}
