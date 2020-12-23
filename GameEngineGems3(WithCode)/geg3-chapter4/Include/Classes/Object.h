#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class wrap_base;

	/*class Thing {};
	class Thing_wrap : public Thing, public luabind::wrap_base {};
	Thing* GetThing();
	extern Thing* thing_;*/

	class Object//lua
	{
	public:
		//bool isvalid;
		//bool useaspointer;
		//int classid;
        //std::vector<Object*>::iterator collectionlink;
		int classid;
		void* userdata;
		//std::string classname;
		//luabind::object* o;
		//std::map<int,void*>* hooks;
		std::multimap<int,void*>* hooks;
		bool persistent;
		bool collected;
		unsigned long refcount;

		Object();
		virtual ~Object();

		//virtual void* GetHook(const int hookid);
		virtual int GetClass();//lua
		virtual void SetPersistence(const bool mode);
        virtual std::string GetClassName();//lua
		/*virtual std::string ToString();
         virtual int ToInt();
         virtual int Compare(Object& object);
         */
		//virtual void SetHook(const int hookid, void* hook);
		virtual void AddHook(const int hookid, void* hook);
		virtual void RemoveHook(const int hookid, void* hook);

		//virtual void RemoveHook(const int hookid, void* hook);
		virtual void Push(const bool takeownership=false);
        virtual bool UseAsPointer();
		//virtual operator<(Object* o);
		//virtual operator>(Object* o);
		virtual void SetUserData(void* userdata);//lua
		virtual void* GetUserData();//lua
		virtual void Print_();
		virtual std::string Debug();//lua
		virtual std::string ToString();//lua
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		//virtual void Free();
		virtual unsigned long Release();//lua
        virtual unsigned long AddRef();//lua
        virtual unsigned long GetRefCount();//lua

		static int DeleteHook;
		static std::vector<Object*> Garbage;
		static std::string GetAddress(Object* o);//lua
		static Object* CreateAndDeserialize(Stream* stream);

		static const int PivotClass;//lua
		static const int ModelClass;//lua
		static const int CameraClass;//lua
		static const int DirectionalLightClass;//lua
		static const int SpotLightClass;//lua
		static const int PointLightClass;//lua
		static const int ListenerClass;//lua
		static const int Vec2Class;//lua
		static const int Vec3Class;//lua
		static const int Vec4Class;//lua
		static const int Mat4Class;//lua
		static const int SurfaceClass;//lua
		static const int BrushClass;//lua
		static const int EmitterClass;//lua
		static const int AttractorClass;//lua
		static const int JointClass;//lua
		static const int LensFlareClass;//lua
		static const int BoneClass;//lua
		static const int TerrainClass;//lua
		static const int TerrainPatchClass;//lua
		static const int BufferClass;//lua
		static const int ContextClass;//lua
		static const int ShaderClass;//lua
		static const int ComponentClass;//lua
		static const int TextureClass;//lua
		static const int MaterialClass;//lua
		static const int SpriteClass;//lua
		static const int DecalClass;//lua
		static const int VegetationLayerClass;//lua
	};

	//class Object_wrap : public Object, public luabind::wrap_base {};
}
