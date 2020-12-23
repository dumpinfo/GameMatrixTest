#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{

	
	class MaterialValue;
	class Texture;
	class Material;
	
	class MaterialReference : public AssetReference
	{
	public:
		
		//Attributes
		Vec4 color[3];
		int blendmode;
		int layer;
		bool castshadows;
		bool cullbackfaces;
		bool depthtest;
		bool zsort;
		bool depthmask;
		int lightingmode;
		Shader* shader[3];
		bool shaderautochosen[3];
		std::map<std::string,MaterialValue> values;//moving this behind the texture[] array causes random errors in editor?!
		Texture* texture[32];
		Mat4 texturematrix[32];
		int testvalue;
		bool stipplemode;
		int drawmode;
		int texturemode;
		bool alphamask;
		bool alwaysuseshader;

		MaterialReference();
		virtual ~MaterialReference();
		
		//Procedures
		virtual void Print_();
		virtual void Enable(const int flags=0, const int shaderindex=SHADER_DEFAULT);
		virtual float GetFloat(const std::string& name);
		virtual Vec2 GetVec2(const std::string& name);
		virtual Vec3 GetVec3(const std::string& name);
		virtual Vec4 GetVec4(const std::string& name);
		virtual Mat3 GetMat3(const std::string& name);
		virtual Mat4 GetMat4(const std::string& name);
		virtual void SetFloat(const std::string& name, const float f);
		virtual void SetVec2(const std::string& name, const float x, const float y);
		virtual void SetVec3(const std::string& name, const float x, const float y, const float z);
		//virtual void SetVec4(const std::string& name, const float x, const float y, const float z, const float w);
		virtual void SetVec4( std::string name,  float x,  float y,  float z,  float w);
		virtual void SetColor(const float r,const float g,const float b,const float a, const int index=COLOR_DIFFUSE);
		virtual Vec4 GetColor(const int mode = COLOR_DIFFUSE);
		virtual Material* Instance();
		virtual bool ContainsValue(const std::string& name);
		virtual void SetShader(Shader* shader, const int index=SHADER_DEFAULT);
		virtual void SetTexture(Texture* texture, const int index);
		virtual Texture* GetTexture(const int index);
		virtual void Disable()=0;
        virtual void SetLightingMode(const int mode);
		virtual int GetLightingMode();
		virtual bool Reload(const int flags=0);
		virtual void Reset();
	};
}
