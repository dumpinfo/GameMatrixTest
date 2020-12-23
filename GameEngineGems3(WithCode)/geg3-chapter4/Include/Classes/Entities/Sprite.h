#pragma once

#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Model;

	class Sprite : public Model//lua
	{
	public:
		Vec2 size;
		float angle;
		//Surface* surface;
		int viewmode;

		Sprite();
		virtual ~Sprite();
		
		virtual void SetSize(const float width, const float height);//lua
		virtual Vec2 GetSize();//lua
		virtual float GetAngle();//lua
		virtual void SetAngle(const float angle);//lua
		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		virtual std::string GetClassName();
		virtual void CopyTo(Entity* entity, const int mode);
		virtual void SetViewMode(const int mode);//lua
		virtual int GetViewMode();//lua
		virtual void UpdateAABB(const int mode);

		static Sprite* Create(Entity* parent=NULL);//lua
	};
}
