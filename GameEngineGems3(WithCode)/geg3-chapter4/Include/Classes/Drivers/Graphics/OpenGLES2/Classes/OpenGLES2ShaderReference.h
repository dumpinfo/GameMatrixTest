#pragma once

#include "../OpenGLES2GraphicsDriver.h"

namespace Leadwerks3D
{
	class OpenGLES2Uniform;
    
	class OpenGLES2ShaderReference : public ShaderReference
	{
    public:
		std::map<std::string,OpenGLES2Uniform> uniforms;
		unsigned int object[5];
		unsigned int program;
		//map<std::string,int> attributes;
        
        OpenGLES2Uniform uniform_entitymatrix;
        
		virtual std::string GetClassName();

		OpenGLES2ShaderReference();
		virtual ~OpenGLES2ShaderReference();
		
        virtual Uniform GetUniform(const std::string& name);
		virtual AssetReference* Copy();
		//virtual int GetAttributeLocation(const std::string& name);
		virtual bool Compile(const int sourceid);
		virtual bool Link();
		virtual void Enable();
		virtual void Disable();
		virtual bool Reload(const int flags=0);
		virtual void Reset();
		virtual Shader* Instance();
		//virtual Uniform* GetUniform(std::string name, const bool warning=true);
		virtual bool SetFloat(const std::string& name, const float f);
        virtual bool SetFloat(const std::string& name, const float* f, const int count);
		virtual bool SetVec2(const std::string& name, const Vec2& v);
		virtual bool SetVec2(const std::string& name, const float* v, const int count);
		virtual bool SetVec3(const std::string& name, const Vec3& v);
        virtual bool SetVec3(const std::string& name, const float* v, const int count);
		virtual bool SetVec4(const std::string& name, const Vec4& v);
        virtual bool SetVec4(const std::string& name, const float* v, const int count);
		virtual bool SetMat3(const std::string& name, const Mat3& m);
		virtual bool SetMat4(const std::string& name, const Mat4& m);
		virtual bool SetInt(const std::string& name, const int i);
		virtual int GetInt(const std::string& name);
		virtual float GetFloat(const std::string& name);
		virtual Vec2 GetVec2(const std::string& name);
		virtual Vec3 GetVec3(const std::string& name);
		virtual Vec4 GetVec4(const std::string& name);
		virtual Mat3 GetMat3(const std::string& name);
		virtual Mat4 GetMat4(const std::string& name);
	};
}
