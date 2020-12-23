#include "../../Leadwerks.h"

namespace Leadwerks
{
	class Decal : public Entity//lua
	{
	public:
		Surface* surface;
		std::map<int, bool> rendermode;

		Decal();
		virtual ~Decal();

		virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
		virtual void UpdateAABB(const int mode);
		virtual int GetClass();
		virtual Entity* Instance(const bool recursive, const bool callstartfunction);
		virtual Entity* Copy(const bool recursive, const bool callstartfunction);
		virtual std::string GetClassName();
		virtual void SetRenderMode(const int objecttype, const bool mode);//lua
		virtual bool GetRenderMode(const int objecttype);//lua

		static Decal* Create(Material* material, Entity* parent = NULL);//lua
	};
}