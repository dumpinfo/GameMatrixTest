#pragma once
#include "../../../Leadwerks.h"

namespace Leadwerks
{
	class DirectionalLight : public Light//lua
	{
	public:
		std::list<DirectionalLight*>::iterator link;
		float shadowstagerange[4];//lua
		float shadowstagearea[4];
		Mat4 shadowstageprojectionmatrix[4];
		Mat4 shadowstagecameramatrix[4];
		Vec3 shadowstagecameraposition[4];
		Camera* currentrendercamera;

		DirectionalLight();
		virtual ~DirectionalLight();
		
		virtual void UpdateMatrix();
		virtual Entity* Copy(const bool recursive = true, const bool callstartfunction=true);
		virtual Entity* Instance(const bool recursive = true, const bool callstartfunction=true);
		
		virtual std::string GetClassName();
		
		static DirectionalLight* Create(Entity* parent=NULL);//lua
		//static const int CascadedShadowMapStages;
	};
}
