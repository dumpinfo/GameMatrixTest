#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2Uniform : public Uniform
	{
	public:
        
		OpenGL2Uniform();
        virtual ~OpenGL2Uniform();
		
		virtual bool SetInt(const int i);
		virtual bool SetFloat(const float f);
		virtual bool SetFloat(const float* f, const int count);
        virtual bool SetVec2(const Vec2& v);
		virtual bool SetVec2(const float* v, const int count);
		virtual bool SetVec3(const Vec3& v);
		virtual bool SetVec3(const float* v, const int count);
		virtual bool SetVec4(const Vec4& v);
		virtual bool SetVec4(const float* v, const int count);
		virtual bool SetMat3(const Mat3& mat);
		virtual bool SetMat4(const Mat4& mat);
		virtual bool SetMat4(const float* v, const int count);
		/*virtual int GetInt();
		virtual float GetFloat();
		virtual Vec2 GetVec2();
		virtual Vec3 GetVec3();
		virtual Vec4 GetVec4();
		virtual Mat3 GetMat3();
		virtual Mat4 GetMat4();*/
	};
}
