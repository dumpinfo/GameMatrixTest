#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks
{
		
	class Entity;
	
	class OpenGLES2Camera : public Camera
	{
		public:
		
		OpenGLES2Camera();
		OpenGLES2Camera(Entity* parent);
		virtual ~OpenGLES2Camera();
		
        virtual void SetGridSize(const float size, const int majorlines);
        virtual void DrawEditorOverlay();
        virtual void DrawGrid();
        virtual void DrawOcclusionQueries();
		virtual void SetMotionBlurMode(const bool mode);
		virtual void SetDrawMode(const int mode);
		virtual void Render(Entity* entity = NULL);
		virtual void DrawAABB(const AABB& aabb, const bool wireframe);
	};
}
