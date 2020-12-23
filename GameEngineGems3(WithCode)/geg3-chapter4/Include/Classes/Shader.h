#include "../Leadwerks.h"

namespace Leadwerks
{
	class Uniform;

	class Shader : public Asset//lua
	{
	public:
		std::string source[5];
		std::string errortext;
		std::string precode;
		bool animated;
		bool instanced;

		//Common uniforms
		Uniform* uniform_currenttime;
		Uniform* uniform_buffersize;
		Uniform* uniform_drawmatrix;
        Uniform* uniform_drawcolor;
		Uniform* uniform_projectionmatrix;
		Uniform* uniform_cameramatrix;
		Uniform* uniform_projectioncameramatrix;
		Uniform* uniform_cameradrawmode;
		Uniform* uniform_cameraprojectionmode;
        Uniform* uniform_lighting_ambient;
		Uniform* uniform_cameraposition;
		Uniform* uniform_camerarange;
		Uniform* uniform_camerazoom;
		Uniform* uniform_cameratheta;
		Uniform* uniform_prevprojectioncameramatrix;
		Uniform* uniform_camerainversematrix;
		Uniform* uniform_cameranormalmatrix;
		Uniform* uniform_camerainversenormalmatrix;		
		Uniform* uniform_entitymatrix;
		Uniform* uniform_ambientlight;
		Uniform* uniform_projectedshadowmappingmatrix;
		Uniform* uniform_isbackbuffer;
		Uniform* uniform_tessstrength;
		Uniform* uniform_texture[32];
		Uniform* uniform_clipplane[6];

		//Lighting
		Uniform* uniform_lightrange; 
		Uniform* uniform_lightingconeanglescos;                      
		Uniform* uniform_lightdirection;
		Uniform* uniform_lightposition;
		Uniform* uniform_lightcolor;
		Uniform* uniform_lightspecular;

		Shader();
		virtual ~Shader();		
		
        virtual std::string GetClassName();
        virtual std::string GetLog();//lua
		virtual Uniform* GetUniform(const std::string& name)=0;
		//virtual bool Reload(const int flags=0);
		virtual void InitializeUniforms();
		//virtual Uniform* GetUniform(std::string name, const bool warning=true)=0;
		virtual void Enable()=0;//lua
		virtual void Disable()=0;//lua
		virtual void Reset();
		virtual bool Compile(const int sourceid)=0;//lua
		virtual bool Link()=0;//lua
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
		virtual bool SetInt(const std::string& name, const int i)=0;//lua
		virtual bool SetFloat(const std::string& name, const float f)=0;//lua
        virtual bool SetFloat(const std::string& name, const float* f, const int count)=0;//lua
		virtual bool SetVec2(const std::string& name, const Vec2& v)=0;//lua
		virtual bool SetVec2(const std::string& name, const float* v, const int count)=0;//lua
		virtual bool SetVec3(const std::string& name, const Vec3& v)=0;//lua
		virtual bool SetVec3(const std::string& name, const float* v, const int count)=0;//lua
		virtual bool SetVec4(const std::string& name, const Vec4& v)=0;//lua
		virtual bool SetVec4(const std::string& name, const float* v, const int count)=0;//lua
		virtual bool SetMat3(const std::string& name, const Mat3& f)=0;//lua
		virtual bool SetMat4(const std::string& name, const Mat4& f)=0;//lua
		virtual bool SetMat4(const std::string& name, const float* f, const int count) = 0;
		virtual int GetInt(const std::string& name) = 0;//lua
		virtual float GetFloat(const std::string& name)=0;//lua
		virtual Vec2 GetVec2(const std::string& name)=0;//lua
		virtual Vec3 GetVec3(const std::string& name)=0;//lua
		virtual Vec4 GetVec4(const std::string& name)=0;//lua
		virtual Mat3 GetMat3(const std::string& name)=0;//lua
		virtual Mat4 GetMat4(const std::string& name)=0;//lua
		/*virtual SetUniformVec4(const std::string& name, const char* v, const int count)=0;
		virtual SetUniformMat3(const std::string& name, const Mat3& f)=0;
		virtual SetUniformMat3(const std::string& name, const char* m, const int count)=0;
		virtual SetUniformMat4(const std::string& name, const Mat4& f)=0;
		virtual SetUniformMat4(const std::string& name, const char* m, const int count)=0;*/
         
		static Shader* Create();//lua
		static Shader* Load(const std::string& path, const int flags = 0, const std::string& precode = "", const uint64_t fileid=0);//lua
        static Shader* GetCurrent();
        
        static Shader* Current;
        static const int Vertex;
        static const int Pixel;
        static const int Geometry;
        static const int Evaluation;
        static const int Control;
	};
}
