#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
		
	class Entity;
	
	class OpenGL2Camera : public Camera
	{
		public:
		
		Surface* gridsurface[2];

		OpenGL2Camera();
		OpenGL2Camera(Entity* parent);
		virtual ~OpenGL2Camera();
		
		virtual void SetGridSize(const float size, const int majorlines);
		//virtual void DrawBatches();
		virtual void DrawEditorOverlay();
		virtual void DrawOcclusionQueries();
		virtual void SetMotionBlurMode(const bool mode);
		virtual void Render(Entity* entity = NULL);
		virtual void DrawGrid();
		virtual void DrawAABB(const AABB& aabb, const bool wireframe);
		virtual void RenderTerrain();
	};
}
