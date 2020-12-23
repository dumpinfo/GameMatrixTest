#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks3D
{
	class OpenGLES2SurfaceReference : public SurfaceReference
	{
		public:
		bool dynamic;
		
		OpenGLES2SurfaceReference();
		//virtual ~OpenGLES2SurfaceReference();
		
		virtual SurfaceReference* Copy();
		/*
		virtual int CountVertices();
		virtual int CountTriangles();
		virtual int CountIndices();
		virtual int AddTriangle(const int a, const int b, const int c);
		virtual int AddIndice(const int i);
		virtual int AddVertex(const float x,const float y,const float z,const float nx,const float ny,const float nz,const float u0,const float v0,const float u1,const float v1,const float r,const float g,const float b,const float a);
		virtual int GetTriangleVertex(const int t, const int c);
		virtual void SetTriangleVertex(const int t, const int c, const int i);
		
		virtual Vec3 GetVertexPosition(const int v);
		virtual Vec3 GetVertexNormal(const int v);
		virtual Vec3 GetVertexTangent(const int v);
		virtual Vec3 GetVertexBinormal(const int v);
		virtual Vec2 GetVertexTexCoords(const int v, const int texcoordset);
		virtual Vec4 GetVertexColor(const int v);
		
		virtual void Lock();
		virtual void Unlock();
		virtual void Enable(const int flags=0);*/
		virtual void Draw();
		//virtual void Disable();
	};
}
