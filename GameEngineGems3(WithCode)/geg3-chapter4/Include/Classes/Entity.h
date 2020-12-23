#pragma once

#include "../Leadwerks.h"
#include <list>

namespace Leadwerks
{
	#define ENTITY_AABB_LOCAL 1
	#define ENTITY_AABB_GLOBAL 2
	#define ENTITY_AABB_RECURSIVE 4
	
	class Vec3;
	class Quat;
	class Mat4;
	class AABB;
	class Camera;
	class World;
	class Material;
	class Body;
	class PhysicsDriver;
	class Chunk;
	class Model;
	class Output;
    class NavTile;
	class Joint;
	class Bone;
    class CharacterController;
	class Source;
	class Sound;
	class Context;
	class Buffer;
	class Vehicle;

	// Classes
	class Entity : public Object//lua
	{
	public:
		GraphicsDriver* driver;
		unsigned long drawiterator;
		Body* body;
        float mass;
		std::list<Entity*>::iterator link;
		std::list<Entity*>::iterator parentlink;
		bool hidestate;
		int parentvectorindex;		
		Material* material;
		World* world;//lua
#ifdef _FLOAT64
		dVec3 position;
#else
		Vec3 position;//lua
#endif
		Vec3 rotation;//lua
		Quat quaternion;
		Vec3 scale;//lua
#ifdef _FLOAT64
		dMat4 mat;
		dMat4 originalmatrix;
		dMat4 lastrendermat;
#else
		Mat4 mat;//lua
        Mat4 originalmatrix;
		Mat4 lastrendermat;
#endif
		AABB lastnavaabb;
		bool updatenavigationneeded;
		bool addedtooctreenodeupdatelist;
		std::list<Entity*>::iterator octreenodeupdatelink;
		std::list<Entity*>::iterator updatenavigationlink;
		std::list<Entity*>::iterator octreenodelink;
		std::list<Entity*>::iterator updateworldhooklink;
		bool updateworldhookstate;
		std::list<Entity*>::iterator updatephysicshooklink;
		bool updatephysicshookstate;
		std::list<Entity*>::iterator postrenderhooklink;
		bool postrenderhookstate;
		OctreeNode* octreenode;
		long lastrendertick;
		Vec4 color[3];
		float intensity[3];
		AABB aabb;
		AABB localaabb;
		AABB recursiveaabb;
		bool recursiveaabbinvalidated;
		Entity* parent;//lua
		GraphicsDriver* graphicsdriver;
		PhysicsDriver* physicsdriver;
		std::list<Entity*> kids;
		Vec4 editcolor;
		void Update();
		void UpdatePhysics();
		std::map<std::string,std::string> keys;
		bool islimb;
		bool navigationmode;
		int collisiontype;
		int matrixlocked;
		bool occlusionmode;
		std::map<Camera*, std::map<Context*,OcclusionQuery*> > occlusionquery;
		//std::map<Camera*,OcclusionQuery*> occlusionquery;
		int viewrange;
        bool islight;
        //std::list<Light*> relevantlights;
		std::map<Light*,bool> relevantlights;
		bool uselighting;
		int shadowmode;
		bool selected;
		//Buffer* projectionshadowbuffer[2];
		//Model* projectionshadowmodel;
		bool drawpositionoverlaymode; 
		Component* component;
		//std::map<std::string,Component*> component;
		int scripthooks;
		std::list<Entity*>::iterator deletioniterator;
		bool scheduledfordeletion;
		int shapeid;
		Vec3 shapeposition;
		Vec3 shaperotation;
		Vec3 shapescale;
        std::vector<NavTile*> relevantnavtiles;
        bool sweptcollisionmode;
		std::list<Joint*> joints;
		std::vector<std::vector<float> > animations;
		std::vector<std::string> animationname;
		std::vector<Bone*> bones;
		bool isroot;
		Entity* fileroot;
		Mat4 originalinversematrix;
		Mat4 animationmatrix;
		std::vector<int> maxanimationlength;
		PrefabBase* prefabbase;
		int pickmode;
		float pickradius;
		int renderiterator;
		bool isprefab;
		std::string prefabpath;
		int scenechildindex;
        bool isrenderable;
        int countrenderableentities;
        int physicsmode;
        CharacterController* charactercontroller;
        std::list<CharacterController*> followingcharactercontrollers;
        float charactercontrollerrotationoffset;
		std::list<Source*> activesources;
		int countlights;
		Shape* shape;
		bool lightsortneeded;
		/*Camera* projectedshadowcamera;
		bool projectedshadowchanged;*/
		int worldclearvectorindex;
		//Mat4 projectedshadowmappingmatrix;
		ComponentBase* script;
		//float staticfriction;
		//float kineticfriction;
		//float elasticity;
		bool lightinginvalidated;
		std::list<Entity*>::iterator lightinginvalidatedlink;
		std::vector<float> bonematrices;
		bool hascollisionfunction;
		bool hasdrawfunction;
		bool hasdraweachfunction;
		bool massmatrixdefined;
		float massmatrix[6];
		std::map<Camera*, int> camerarenderiterator;
		bool isshapehint;
		Vehicle* vehicle;
		int occlusionquerystyle;
		bool editorviewonly;
		bool countocclusionsamples;
		AABB pickbox;
		Entity* owner;
		bool buoyancymode;
		Surface* occlusionquerysurface;

		Entity();
		virtual ~Entity();

		virtual double GetDistanceToEntity(Entity* entity);//lua
		virtual double GetDistance(Entity* entity);//lua
		virtual std::string GetClassName();
		virtual void InvalidateLighting();
		virtual bool ContainsFunction(const std::string& name);
		virtual void DrawKids(Camera* camera, const bool drawsorted);
		virtual void SortLights();
		virtual void IncrementLightCount(const int count);
		virtual void UpdateActiveSources();
		virtual void EmitSound(Sound* sound, float range=50, float volume=1, float pitch=1, bool loopmode=false);//lua
		virtual void EmitSound(Source* source);
		virtual bool GetCulled(Camera* camera=NULL);//lua
		virtual void SetOcclusionCullingMode(const bool mode, const bool recursive=false);//lua
		virtual bool GetOcclusionCullingMode();//lua
		virtual void AddHook(const int hookid, void* hook);
		virtual void SetCharacterControllerAngle(const float angle);//lua
		virtual float GetCharacterControllerAngle();//lua
        virtual int GetPhysicsMode();//lua
        virtual bool GetAirborne();//lua
        virtual bool GetCrouched();//lua
		virtual Mat4 GetPhysicsMatrix();//lua
        virtual bool GoToPoint(const float x, const float y, const float z, const float maxspeed=1.4, const float maxacceleration=1);//lua
        virtual bool GoToPoint(const Vec3& point, const float maxspeed=1.4, const float maxacceleration=1);//lua
        virtual bool Follow(Entity* entity, const float maxspeed=1.4, const float maxacceleration=1);//lua
		virtual void Stop();//lua
        virtual void SetInput(const float angle, const float move, const float strafe=0, const float jump=0, const bool crouch=false, const float maxaccel=1,const float maxdecel=0.5, const bool detailed=false);//lua
        virtual void SetPhysicsMode(const int mode);//lua
		virtual void RemoveAnimationSequence(const int index);//lua
        virtual void IncrementRenderableEntityCount(const int count);
		virtual void RemoveOctreeNode();
		virtual int ExtractAnimationSequence(const int index, const int startframe, const int length);//lua
		virtual void SetPickMode(const int mode, const bool recursive=false);//lua
		virtual int GetPickMode();//lua
		virtual void SetQuaternion(const Quat& q, const bool global=false);//lua
		virtual Quat GetQuaternion(const bool global=false);//lua
        virtual void CopyAnimations(Entity* entity);//lua
        virtual int LoadAnimation(const std::string& path);//lua
		virtual Mat4 GetAnimationFrame(const int index, const int sequence=0);//lua
		virtual int CountAnimations();//lua
		virtual int GetAnimationLength(const int index=0, const bool recursive=true);//lua
		virtual int GetAnimationLength(const std::string& name, const bool recursive = true);//lua
		//virtual int GetAnimationLength(const int index=0, const bool recursive=false);
        virtual Bone* GetBone(const int index);
		virtual void LockMatrix();//lua
		virtual void UnlockMatrix();//lua
		virtual void PopulateFileRootBoneArray(Entity* root);
		virtual Entity* GetParent();//lua
		virtual Entity* GetFileRoot();
		//virtual int AddAnimation();
		//virtual int AddAnimationFrame(const Vec3& position, const Vec3& rotation, const Vec3& scale, const int index=0);
		virtual void SetAnimationFrame(const double& time, const float blend=1.0, const int index=0, const bool recursive=true);//lua
		virtual void SetAnimationFrame(const double& time, const float blend, const std::string& sequence, const bool recursive=true);//lua
		virtual int FindAnimationSequence(const std::string& sequence);//lua
		virtual Component* GetComponent(const std::string& name);//lua
		virtual void SetFriction(const float staticfriction, const float kineticfriction);//lua
        //Joint functions
        /*virtual void SetJointType(const int jointtype);
        virtual int GetJointType();
        virtual void SetJointPosition(const float x, const float y, const float z);
        virtual Vec3 GetJointPosition();
        virtual void SetJointPin(const float x, const float y, const float z);
        virtual Vec3 GetJointPin();
        virtual void SetJointLimits(const float limit0, const float limit1);
        virtual Vec2 GetJointLimits();*/
        //virtual void SetJointStrength(const float strength);
        //virtual float GetJointStrength();
        //virtual void SetJointBreakForce(const float breakforce);
        //virtual float GetJointBreakForce();
        //virtual void UpdateProjectedShadowCameraOrientation();
		virtual float GetPickRadius();//lua
		virtual void SetPickRadius(const float radius);//lua
		virtual bool GetGravityMode();//lua
		virtual void SetGravityMode(const bool mode);//lua
		virtual void InterpolatePhysics(const double f);
        virtual void SetSweptCollisionMode(const bool mode);//lua
        virtual bool GetSweptCollisionMode();//lua
		virtual bool BuildShapeFromLimbs(Shape* shape, Entity* space, const bool dynamic);
		virtual void SetMass(const float mass);//lua
		virtual void SetMass(const float mass, const float cx, const float cy, const float cz, const float ixx, const float iyy, const float izz);//lua
		virtual float GetMass();//lua
		virtual int GetViewRange();//lua
		virtual void SetViewRange(const int range, const bool recursive=false);//lua
		virtual bool BuildShapeFromHints();
		virtual bool BuildShape(const bool dynamic = false, const bool recursive = true);//lua
		virtual void ResetScale();
		virtual void UpdateOcclusionQuerySurface();
		virtual bool OcclusionVolumeIntersectsPoint(const Vec3& p, const float radius);

		virtual void SetString(const std::string& name, const std::string& s);
		virtual std::string GetString(const std::string& name);
		virtual void SetObject(const std::string& name, Object* o);
		virtual Object* GetObject(const std::string& name);
		virtual bool GetBool(const std::string& name);
		virtual void SetFloat(const std::string& name, const float f);
		virtual float GetFloat(const std::string& name);

		virtual void CallOutputs(const std::string& name);
		virtual void AddOutput(const std::string& out, Entity* entity, const std::string& in);
		virtual bool SetScript(const std::string& path, const bool start=true, const uint64_t fileid=0);//lua
		//virtual void AttachScript(Script* script);
		virtual void SetSelected(const bool mode);
		virtual bool GetSelected();
		virtual void Collapse(Model* targetmodel=NULL);
        static void UpdateLightCallback(Entity* light, Object* extra);		
		//virtual bool Save(std::string path, const bool recursive=true);
		virtual void Serialize(Stream* stream);
		virtual void SerializeChildren(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual void DeserializeChildren(Stream* stream);
		virtual void ClearLighting();
        virtual void UpdateLighting();
		virtual OcclusionQuery* GetOcclusionQuery(Camera* camera);
		virtual bool OcclusionCulled(Camera* camera, const bool conservative=false);
		virtual void UpdateNavigation();
		//virtual bool BuildShape(const int shapeid=-1, const float px=0, const float py=0, const float pz=0, const float rx=0, const float ry=0, const float rz=0, const float sx=1, const float sy=1, const float sz=1);
		virtual void AddForce(const float x, const float y, const float z, const bool global = true);//lua
		virtual void AddForce(const Vec3& force, const bool global = true);//lua
		virtual void AddTorque(const Vec3& torque, const bool global = true);//lua
		virtual void AddTorque(const float x, const float y, const float z, const bool global = true);//lua

		//virtual void SetName(const std::string& name);
		//virtual std::string GetName();
		
		//virtual void UpdateParentRecursiveAABBAndOctreeNode();
		virtual void UpdateOctreeNode(const bool immediate=false);

		//Physics
		virtual Vec3 GetVelocity(const bool global = true);//lua
		virtual Vec3 GetPointVelocity(const Vec3& point);//lua
		virtual Vec3 GetPointVelocity(const float x, const float y, const float z);//lua
		virtual Vec3 GetOmega(const bool global = true);//lua
		virtual void SetVelocity(const Vec3& velocity, const bool global=true);//lua
		virtual void SetOmega(const Vec3& omega, const bool global=true);//lua

		virtual bool Pick(const Vec3& p0, const Vec3& p1, PickInfo& pick, const float radius = 0.0, const bool closest = false, const bool recursive=false, const int collisiontype=0);//lua
		
		//virtual void SetPosition(const dVec3& position, const bool global=false);
		virtual void SetPosition(const dVec3& position, const bool global = false);//lua
		//virtual void SetPosition(const double x, const double y, const double z, const bool global = false);
		virtual void SetPosition(const Vec3& position, const bool global=false);//lua
		virtual void SetPosition(const float x, const float y, const float z, const bool global=false);//lua
		//virtual void SetPosition(const double x, const double y, const double z, const bool global=false);
		virtual void Translate(const float x, const float y, const float z, const bool global=false);//lua
		virtual void Translate(const Vec3& translation, const bool global);//lua

		virtual void OnCollision(Entity* entity, const Vec3& position, const Vec3& normal, const float speed);//lua
		
		virtual void Turn(const Vec3& rotation, const bool global = false);//lua
		virtual void Turn(const float x, const float y, const float z, const bool global=false);//lua
		//virtual bool Create();
		
		//Script
		virtual bool CallFunction(const std::string& name, Object* extra=NULL);
		virtual bool CallFunction(const std::string& name, Entity* entity, const Vec3& position, const Vec3& normal, const float speed);
		
		virtual std::string Debug();//lua
		//virtual void SetScript(Script* script);
		
		//void SetRotation(const Vec3& rotation);
		//void SetRotation(const Quat& rotation);
		//void SetRotation(const float pitch, const float yaw, const float roll);
		virtual void SetRotation(const Vec3& rotation, const bool global=false);//lua
		virtual void SetRotation(const Quat& rotation, const bool global=false);//lua
		virtual void SetRotation(const float pitch, const float yaw, const float roll, const bool global=false);//lua
		
		virtual void SetScale(const float x);//lua
		virtual void SetScale(const Vec3& scale);//lua
		virtual void SetScale(const float x, const float y, const float z);//lua
		
		virtual Vec3 GetPosition(const bool global = false);//lua
		//virtual dVec3 GetPositiond(const bool global = false);
		virtual Vec3 GetRotation(const bool global = false);//lua
		virtual Vec3 GetScale(const bool global = false);//lua
		virtual Mat4 GetMatrix(const bool global=true);//lua
		
		virtual void SetCollisionType(const int collisiontype);//lua
		virtual int GetCollisionType();//lua
		
		virtual void SetColor(const float r, const float g, const float b);//lua
		virtual void SetColor(const float r, const float g, const float b, const float a, const int mode=COLOR_DIFFUSE, const bool recursive=false);//lua
		virtual void SetColor(const Vec4& color, const int mode=COLOR_DIFFUSE, const bool recursive=false);//lua
		virtual Vec4 GetColor(const int mode=0);//lua

		virtual void SetIntensity(const float intensity, const int mode=COLOR_DIFFUSE, const bool recursive=false);//lua
		virtual float GetIntensity(const int mode=COLOR_DIFFUSE);//lua
		
		virtual void Move(const Vec3& position, const bool global = false);//lua
		virtual void Move(const float x, const float y, const float z, const bool global=false);//lua
		
		virtual void AlignToVector(const float x, const float y, const float z, const int axis = 2, const float rate = 1.0, const float roll = 0.0);//lua
		virtual void AlignToVector(const Vec3& v, const int axis = 2, const float rate = 1.0, const float roll = 0.0);//lua
		virtual void Point(Entity* entity, const int axis = 2, const float rate = 1.0, const float roll = 0.0);//lua
		
		virtual void SetMatrix(const Mat4& mat, const bool global = true);//lua
		
		virtual AABB GetAABB(const int mode = ENTITY_AABB_GLOBAL);//lua
		//void Turn(const Vec3& rotation, const bool global = false);
		//void Turn(const float pitch, const float yaw, const float roll, const bool global = false);
		//void Translate(const Vec3& translation, const bool global = false);
		//void Translate(const float x, const float y, const float x,, const bool global = false);
		//void Move(const Vec3& movement, const bool global = false);
		//void Move(const float x, const float y, const float x,, const bool global = false);
		//void AlignToVector(const Vec3& vector, const int axis = AXIS_X, const float rate = 1.0, const float roll = 0.0);
		//void AlignToVector(const float x, const float y, const float x, const int axis = AXIS_X, const float rate = 1.0, const float roll = 0.0);
		//void Point(const float x, const float y, const float x, const int axis = AXIS_X, const float rate = 1.0, const float roll = 0.0);
		//void Point(const Entity& entity, const int axis = AXIS_X, const float rate = 1.0, const float roll = 0.0);
		//void Point(const Vec3& position, const int axis = AXIS_X, const float rate = 1.0, const float roll = 0.0);
		virtual void InvalidateRecursiveAABB();
		
		virtual void UpdateAABB(const int mode);//lua
		virtual void CopyTo(Entity* entity,const int mode);//lua
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction = true) = 0;//lua
		virtual Entity* Instance(const bool recursive=true, const bool callstartfunction=true)=0;//lua
		virtual void CopyKids(Entity* parent, const int mode, const bool callstartfunction);//lua
		virtual void DrawShadow(Camera* camera, const bool recursive);
		
		//virtual void Push();
		
		Entity* FindChild(const std::string& name);//lua
		
		virtual int CountKeys();//lua
		virtual std::string GetKeyName(const int index);//lua
		std::string GetKeyValue(const std::string& name, const std::string& defaultvalue="");//lua
		void SetKeyValue(const std::string& name,const std::string& value);//lua
		void ClearKeys();//lua

		virtual void Hide();//lua
		virtual void Show();//lua
		
		//Physics
		bool Active();//lua

		virtual void UpdateMatrix();
		virtual void UpdateLocalOrientation();
		virtual void UpdateGlobalOrientation();
		virtual void UpdateKids();
		
		//void DoUpdateMatrixStuff();
		virtual void SetMaterial(Material* material, const bool recursive = false);//lua
		virtual bool SetMaterial(const std::string& file, const bool recursive = false);//lua
		virtual Material* GetMaterial();//lua
		virtual int CountChildren();//lua
		virtual Entity* GetChild(const int index);//lua
		virtual void SetParent(Entity* parent, const bool global=true);//lua
		virtual bool Hidden();//lua
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void PhysicsSetRotation(const float pitch, const float yaw, const float roll, const float smoothness = 0.5);//lua
		virtual void PhysicsSetRotation(const Quat& q, const float smoothness = 0.5);//lua
		virtual void PhysicsSetPosition(const float x, const float y, const float z, const float smoothness = 0.5);//lua
		
        virtual void AddPointForce(const float fx, const float fy, const float fz, const float px, const float py, const float pz, const bool global=true);//lua
        virtual void AddPointForce(const Vec3& force, const Vec3& position, const bool global=true);//lua
        
		virtual void SetNavigationMode(const bool mode, const bool recursive=false);//lua
		virtual bool GetNavigationMode();//lua
		
		virtual void FixBoneRoots();

        virtual void SetShape(Shape*);//lua
		virtual void SetAutoShape(const int mode);
        virtual void SetShape(Shape*, const Mat4& mat);//lua
        virtual Shape* GetShape();//lua
        
		virtual void SetShadowMode(const int shadowmode, const bool recursive=false);//lua
		virtual int GetShadowMode();//lua
		//virtual void UpdateProjectedShadow();

		virtual void AddBuoyancyForce(const float nx, const float ny, const float nz, const float d, const float fluidDensity=1.0, const float fluidViscosity=0.1);//lua
		
		//virtual bool GenerateConvexHull(const bool recursive = true);
		//virtual bool GenerateConvexDecomposition(const bool recursive);
		virtual void GatherSurfaces(std::vector<Surface*>* surfaces, Entity* space, const bool dynamic, const bool recursive);

		virtual std::string GetAnimationName(const int index);
		virtual void SetAnimationName(const int index, const std::string& name);

		virtual void AddToWorldTable(const bool add);

		virtual Surface* GetOcclusionQuerySurface();
		virtual Mat4 GetOcclusionQueryMatrix();

		virtual void SetPickBox(const float x0, const float y0, const float z0, const float x1, const float y1, const float z1);
		virtual AABB GetPickBox();
		virtual void BuildPickBox(const bool recursive);

		static  int DrawHook;
		static  int DrawEachHook;
		static  int UpdateMatrixHook;
		static  int UpdateWorldHook;
		static  int UpdatePhysicsHook;
		static  int CollisionHook;
		static  int PostRenderHook;
		
		//static void UpdateProjectedShadowCallback(Entity* entity, Object* extra);

		static std::list<Entity*> EventQueueEntities;
		static Vec3 CalcAverageOmega(Quat q, Quat QB, const double& dt);//lua
		static bool CompareLights(Light* first, Light* second);

		static const int NearViewRange;//lua
		static const int MediumViewRange;//lua
		static const int FarViewRange;//lua
		static const int MaxViewRange;//lua
		static const int InfiniteViewRange;//lua

		static const int LocalAABB;//lua
		static const int GlobalAABB;//lua
		static const int RecursiveAABB;//lua
        
        static const int RigidBodyPhysics;//lua
        static const int CharacterPhysics;//lua
		static const int KinematicPhysics;//lua
        static const int SpherePick;//lua
        static const int PolygonPick;//lua
		static const int BoxPick;//lua
		static const int CapsulePick;//lua

		bool fastphysics;
		static ConvexHull* pickboxhull;
		static Shape* pickcapsuleshape;
	};
	
	//class Entity_wrap : public Entity, public luabind::wrap_base {};
	
	//Transform commands
	Vec3 TFormPoint(const Vec3& point, Entity* src=NULL, Entity* dst=NULL);
	Vec3 TFormPoint(const float x, const float y, const float z, Entity* src=NULL, Entity* dst=NULL);
	Vec3 TFormVector(const float x, const float y, const float z, Entity* src=NULL, Entity* dst=NULL);
	Vec3 TFormVector(const Vec3& vector, Entity* src=NULL, Entity* dst=NULL);
	Vec3 TFormNormal(const float x, const float y, const float z, Entity* src=NULL, Entity* dst=NULL);
	Quat TFormRotation(const Quat& rotation, Entity* src=NULL, Entity* dst=NULL);
	Vec3 TFormRotation(const float pitch, const float yaw, const float roll, Entity* src, Entity* dst);
	Quat TFormQuaternion(const Quat& quaternion, Entity* src, Entity* dst);	
}
