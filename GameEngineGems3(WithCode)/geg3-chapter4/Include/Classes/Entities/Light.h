#pragma once
#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Light : public Camera//lua
	{
	public:
		std::list<Light*>::iterator link;
		std::list<Light*>::iterator lightoctreenodelink;
        std::map<Entity*,bool> relevantstaticentities;
		std::map<Entity*,bool> relevantdynamicentities;
		//std::list<Entity*> relevantstaticentities;
		//std::list<Entity*> relevantdynamicentities;
		Vec2 coneangles;
        bool useposition;
        float sortdistance;
		int shadowmapsize;
		Buffer* staticshadowbuffer[6];
		Buffer* shadowbuffer[6];
		Vec2 shadowmapoffset[4];
		bool shadowmapinvalidated[3];
		int shadowpassmode;
		bool lightinginvalidated;
		float shadowsoftness;
		bool frustumupdateneeded;

		Light();
		virtual ~Light();

		virtual void InvalidateLighting();
		virtual void RemoveOctreeNode();
		virtual void Show();
		virtual std::string GetClassName();
        virtual void SetRange(const float range);//lua
		virtual void SetRange(const float nearrange, const float farrange);//lua
        virtual void UpdateOctreeNode(const bool immediate=false);
		virtual void ClearLighting();
		virtual void UpdateLighting();
		virtual void SetMatrix(const Mat4& mat, const bool global=true);
		virtual void UpdateMatrix();
		virtual void SetShadowMode(const int mode, const bool recursive=false);//lua
		virtual void SetShadowMapSize(const int size);
		virtual int GetShadowmapSize();
		virtual void SetGridSize(const float a, const int b);
		virtual void DrawEditorOverlay();
		virtual void DrawGrid();
		virtual void DrawAABB(const AABB& aabb, const bool recursive);
		virtual void Render(Entity* entity);
		virtual void SetShadowOffset(const float muloffset, const float addoffset,const int index);//lua
		virtual void InvalidateShadowMap(const int shadowmode);
		virtual Vec2 GetShadowOffset(const int index);//lua
		virtual void Render();
		virtual void SetShadowSoftness(const float softness);
		virtual float GetShadowSoftness();

		static int Dynamic;//lua
		static int Static;//lua
		static int Buffered;//lua
		static bool FreezeChanges;

		static void UpdateLightingCallback(Entity* entity, Object* extra);
	};
}
