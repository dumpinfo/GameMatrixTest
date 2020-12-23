#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{

	
	class MaterialValue;
	class Texture;
	class Material;
	
    #define BLEND_SOLID 0
    #define BLEND_ALPHA 1
    #define BLEND_SHADE 2
    #define BLEND_LIGHT 3
    #define BLEND_MOD2X 4
	
	extern const int SHADER_DEFAULT;
	extern const int SHADER_SHADOW;
	extern const int SHADER_EDIT;
    
    #define MATERIAL_EDITOR 1
    
	class Material : public Asset//lua
	{
	public:
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
		bool caulk;
		Vec3 mappingscale;
		Mat4 mappingmatrix;
		int decalmode;

		Material();
		virtual ~Material();
		
		virtual Vec3 GetMappingScale();
		virtual void SetMappingScale(const float x, const float y, const float z);
		virtual int GetWidth();
		virtual int GetHeight();
		virtual int GetDepth();
		virtual std::string GetClassName();
		virtual void SetLayer(const int layer);
		virtual int GetLayer();
		virtual void SetFloat(const std::string& name, const float f);//lua
		virtual void SetVec2(const std::string& name, const float x, const float y);//lua
		virtual void SetVec3(const std::string& name, const float x, const float y, const float z);//lua
		virtual void SetVec4(const std::string& name, const float x, const float y, const float z, const float w);//lua
		virtual void SetVec2(const std::string& name, const Vec2& v);//lua
		virtual void SetVec3(const std::string& name, const Vec3& v);//lua
		virtual void SetVec4(const std::string& name, const Vec4& v);//lua
		virtual void SetBlendMode(const int mode);//lua
		virtual int GetBlendMode();//lua
		virtual void SetColor(const Vec4& color, const int mode=COLOR_DIFFUSE);//lua
		virtual void SetColor(const float r, const float g, const float b);//lua
		virtual void SetColor(const float r, const float g, const float b, const float a, const int mode=COLOR_DIFFUSE);//lua
		virtual Vec4 GetColor(const int mode = COLOR_DIFFUSE);//lua
		virtual void SetShader(Shader* shader, const int index=SHADER_DEFAULT);//lua
		virtual bool SetShader(const std::string& path, const int index=SHADER_DEFAULT);//lua
		virtual void SetTexture(Texture* texture, const int index = 0);//lua
		virtual bool SetTexture(const std::string& path, const int index);//lua
		//virtual bool SetTexture(const std::string& path, const int index = 0, const int loadflags=0);
		virtual Texture* GetTexture(const int index=0);//lua
		virtual void Enable(const int flags=0, const int shaderindex = SHADER_DEFAULT);
		virtual void Disable()=0;
		virtual bool ContainsValue(const std::string& name);
		virtual Shader* GetShader(const int index=SHADER_DEFAULT);//lua
		virtual void SetBackFaceCullMode(const bool mode);//lua
		virtual bool GetBackFaceCullMode();//lua
		virtual void SetSortMode(const bool mode);//lua
        virtual void SetLightingMode(const int mode);//lua
		virtual int GetLightingMode();//lua
		virtual bool GetSortMode();//lua
		virtual void SetDepthTestMode(const bool mode);//lua
		virtual int GetDepthTestMode();//lua
		virtual void SetDepthMaskMode(const bool mode);//lua
		virtual bool GetDepthMaskMode();//lua
		virtual void SetShadowMode(const bool mode);//lua
		virtual bool GetShadowMode();//lua
		virtual float GetFloat(const std::string& name);//lua
		virtual Vec2 GetVec2(const std::string& name);//lua
		virtual Vec3 GetVec3(const std::string& name);//lua
		virtual Vec4 GetVec4(const std::string& name);//lua
		virtual Mat3 GetMat3(const std::string& name);//lua
		virtual Mat4 GetMat4(const std::string& name);//lua
		//virtual void SetVec4(const std::string& name, const float x, const float y, const float z, const float w);
		virtual bool Reload(const int flags=0);
		virtual void Reset();
		virtual void SetDecalMode(const int rendermode);//lua
		virtual int GetDecalMode();//lua

		static float DefaultMappingScale;
        static Material* Create();//lua
		static Material* Load(const std::string& path, const int flags = 0, const uint64_t fileid=0);//lua
		static Material* LoadBestGuess(const std::string& path, const int flags=0);
	};
}
