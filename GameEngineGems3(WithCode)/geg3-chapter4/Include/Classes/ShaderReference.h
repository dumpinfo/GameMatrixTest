#include "../Leadwerks.h"

namespace Leadwerks
{
	class Uniform;

	class ShaderReference : public AssetReference
	{
	public:
		std::string source[5];
		std::string errortext;
		
		//Common uniforms
		Uniform uniform_entitymatrix;
		
		ShaderReference();
		virtual ~ShaderReference();		
		
		virtual Uniform GetUniform(const std::string& name)=0;
		//virtual bool Reload(const int flags=0);
		virtual void InitializeUniforms();
		virtual Asset* Instance()=0;
		//virtual Uniform* GetUniform(std::string name, const bool warning=true)=0;
		virtual void Enable()=0;
		virtual void Disable()=0;
		virtual void Reset();
		virtual bool Compile(const int sourceid)=0;
		virtual bool Link()=0;
		virtual void SetSource(const std::string& source, const int sourceid);
		virtual std::string GetSource(const int sourceid);
		/*virtual SetUniformFloat(const std::string& name, const float f)=0;
		virtual SetUniformFloat(const std::string& name, const float f, const int count)=0;
		virtual SetUniformInt(const std::string& name, const int i)=0;
		virtual SetUniformInt(const std::string& name, const int i, const int count)=0;
		virtual SetUniformVec2(const std::string& name, const Vec2& f)=0;
		virtual SetUniformVec2(const std::string& name, const char* v, const int count)=0;
		virtual SetUniformVec3(const std::string& name, const Vec3& f)=0;
		virtual SetUniformVec3(const std::string& name, const char* v, const int count)=0;*/
		virtual bool SetInt(const std::string& name, const int i)=0;
		virtual bool SetFloat(const std::string& name, const float f)=0;
        virtual bool SetFloat(const std::string& name, const float* f, const int count)=0;
		virtual bool SetVec2(const std::string& name, const Vec2& v)=0;
		virtual bool SetVec2(const std::string& name, const float* v, const int count)=0;
		virtual bool SetVec3(const std::string& name, const Vec3& v)=0;
		virtual bool SetVec3(const std::string& name, const float* v, const int count)=0;
		virtual bool SetVec4(const std::string& name, const Vec4& v)=0;
		virtual bool SetVec4(const std::string& name, const float* v, const int count)=0;
		virtual bool SetMat3(const std::string& name, const Mat3& f)=0;
		virtual bool SetMat4(const std::string& name, const Mat4& f)=0;
		virtual bool SetMat4(const float* v, const int count)=0;
		virtual int GetInt(const std::string& name)=0;
		virtual float GetFloat(const std::string& name)=0;
		virtual Vec2 GetVec2(const std::string& name)=0;
		virtual Vec3 GetVec3(const std::string& name)=0;
		virtual Vec4 GetVec4(const std::string& name)=0;
		virtual Mat3 GetMat3(const std::string& name)=0;
		virtual Mat4 GetMat4(const std::string& name)=0;
		/*virtual SetUniformVec4(const std::string& name, const char* v, const int count)=0;
		virtual SetUniformMat3(const std::string& name, const Mat3& f)=0;
		virtual SetUniformMat3(const std::string& name, const char* m, const int count)=0;
		virtual SetUniformMat4(const std::string& name, const Mat4& f)=0;
		virtual SetUniformMat4(const std::string& name, const char* m, const int count)=0;*/
	};
}
