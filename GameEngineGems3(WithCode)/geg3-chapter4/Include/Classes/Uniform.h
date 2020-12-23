#include "../Leadwerks.h"

namespace Leadwerks
{
	class Uniform : public Object
	{
	public:
		int index;
		int type;
		int size;
		std::string name;
		bool setonce;
		float value_float;
		Vec2 value_vec2;
		Vec3 value_vec3;
		Vec4 value_vec4;
		Mat3 value_mat3;
		Mat4 value_mat4;

		Uniform();
		virtual ~Uniform();
		
		virtual bool SetInt(const int i)=0;
		virtual bool SetFloat(const float f)=0;
        virtual bool SetFloat(const float* f, const int count)=0;
		virtual bool SetVec2(const Vec2& v)=0;
		virtual bool SetVec2(const float* v, const int count)=0;
		virtual bool SetVec3(const Vec3& v)=0;
		virtual bool SetVec3(const float* v, const int count)=0;
		virtual bool SetVec4(const Vec4& v)=0;
		virtual bool SetVec4(const float* v, const int count)=0;
		virtual bool SetMat3(const Mat3& m)=0;
		//virtual bool SetMat3(const Mat3& m, const int count)=0;
		virtual bool SetMat4(const Mat4& m)=0;
		virtual bool SetMat4(const float* v, const int count)=0;
	};
}
