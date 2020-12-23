#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL4Camera : public OpenGL2Camera
	{
	public:
		Shader* shader_ambient[7];
		Shader* shader_directional[8][2][3];
		Shader* shader_point[8][2][3];
		Shader* shader_spot[8][2][3];
		Shader* shader_gi_point;
		Shader* shader_gi_spot;
		Shader* shader_gi_directional;
		Shader* shader_decal;
		Surface* surface_pointlight;
		Surface* surface_spotlight;
		Surface* surface_decal;
		Texture* texture_bfn;
		Surface* skysurface;
		Material* skymaterial;
		
		OpenGL4Camera();
		virtual ~OpenGL4Camera();

		virtual bool DrawDecals();
		virtual void DrawSkybox();
		virtual void UpdateBuffers(const int width, const int height);
		virtual void Render(Entity* entity);
		virtual void RenderLights(Texture* colortexture0, Texture* colortexture1, Texture* colortexture2, Texture* depthtexture, Buffer* dstbuffer);
		virtual void RenderReflection();
		virtual void DrawWater(Buffer* lightbuffer);
		virtual void SetupProjection(Buffer* buffer);
	};
}
