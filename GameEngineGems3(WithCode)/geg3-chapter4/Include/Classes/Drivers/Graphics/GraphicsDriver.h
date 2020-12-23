#pragma once

#include "../../../Leadwerks.h"
#include "../../Font.h"

#undef DrawText

#define VENDOR_AMD 1
#define VENDOR_NVIDIA 2
#define VENDOR_INTEL 3
#define VENDOR_UNKNOWN 4

namespace Leadwerks
{
	class Window;
	class Texture;
	class Context;
	class Buffer;
	class Font;
	class GUIDriver;
	class Object;
    class PointLight;
    
	class GraphicsDriver : public Driver//lua
	{
	public:
		Font* defaultfont;
		Material* drawphysicsmaterial;
		int countactivetextures;
		int countactivemodels;
		GUIDriver* guidriver;
		Material* defaultmaterial;
		Material* defaultmeshmaterial;
		Shader* defaultshader;
		Buffer* currentbuffer;
		Buffer* backbuffer;
		Shader* shader[32];
		Context* currentcontext;
		int64_t vidmemusage;
		int drawblend;
		//Vec4 drawcolor;
		Vec3 drawtranslation;
		Vec3 drawrotation;
		Vec3 drawscale;
		Mat4 drawmatrix;
		Vec4 clearcolor;
		bool dontdeletesharedcontext;
        Context* dummycontext;
		bool occlusioncullingsupported;
		Surface* gridsurface[2];
		Surface* gridoriginsurface;
		Material* gridmaterial;
		Mat4 projectionmatrix;
		Mat4 cameramatrix;
		Mat4 projectioncameramatrix;
		Material* overlayselectionmaterial;
		Surface* drawmatrixsurface[2];
		Font* currentfont;
        int gbufferlightingsamples;
		bool tessellationsupported;
		int glversion;
		bool useprojectedshadows;
		bool useshadowmaps;
		Shader* defaultshadowshader;
		Vec4 drawcolor;
		int maxtexturesize;
		float maxanisotropy;
		Vec4 clipplane[6];
		
		GraphicsDriver();
		virtual ~GraphicsDriver();
		
		virtual Uniform* CreateUniform()=0;
		virtual void Reset();
        virtual PointLight* CreatePointLight()=0;
		virtual SpotLight* CreateSpotLight()=0;
		virtual DirectionalLight* CreateDirectionalLight()=0;
		virtual bool Initialize()=0;
		virtual int CountActiveTextures();
		virtual int CountActiveModels();
		virtual void ReloadAutoShaders()=0;
		//virtual Shader* ChooseShader(MaterialReference* materialreference, const int shaderindex=SHADER_DEFAULT)=0;
		virtual bool BuffersSupported()=0;
		virtual int GetMaxBufferColorTextures()=0;
		virtual int GetMaxTextureUnits()=0;
		virtual int GetMaxContextSamples()=0;
		virtual int GetMaxTextureSamples()=0;
		virtual void SetViewport(const int x, const int y, const int width, const int height)=0;
		virtual void SetClearColor(const float r, const float g, const float b, const float a);
		virtual void SetClearColor(const float r, const float g, const float b);
		virtual Vec4 GetClearColor();
		virtual int GetVidMemUsage();
		virtual Material* GetDefaultMaterial()=0;
		virtual Shader* GetDefaultShader()=0;
		virtual bool Supported()=0;
		virtual int GetVendor()=0;
		virtual std::string GetName();
		virtual std::string GetDeviceName()=0;
		virtual Texture* CreateTexture()=0;//const int width, const int height, const int format, const int flags, const int frames)=0;
		virtual Context* CreateContext(Window* window,const int multisamplemode)=0;
		virtual Texture* CreateTexture(const int width, const int height, const int format, const int flags, const int frames)=0;
		virtual Surface* CreateSurface()=0;
		virtual Shader* CreateShader()=0;
		virtual void SetContext(Context* context)=0;
		virtual Camera* CreateCamera(Entity* parent)=0;
		virtual Material* CreateMaterial()=0;
		virtual Buffer* CreateBuffer(const int width, const int height, const int colorcomponents, const int depthbuffer, const int multisamplemode)=0;
		virtual VertexArray* CreateVertexArray(const int target,const int type,const int coordinates)=0;
		virtual IndiceArray* CreateIndiceArray(const int mode, const int type)=0;
		virtual void SetBlendMode(const int mode);//lua
        virtual OcclusionQuery* CreateOcclusionQuery()=0;
		virtual Shader* GetDefaultShadowShader();
		virtual void DrawRectBatch(const float* f, const float* c, const int count)=0;

		//Drawing functions
		virtual void DrawText(const std::string& text, const float x, const float y, Font* font)=0;
		virtual void DrawRect(const float x, const float y, const float width, const float height, const int style=0)=0;
        virtual void BeginChars(const float x, const float y, const int mode)=0;
        virtual void EndChars()=0;
		virtual void DrawChar(const float x, const float y, const float width, const float height, const Vec4& coords)=0;
		virtual void DrawLine(const float x0, const float y0, const float x1, const float y1, const bool drawlastpixel=false)=0;
		virtual void DrawPixel(const float x, const float y)=0;
		virtual void DrawImage(Texture* texture, const float x, const float y, const float width=0, const float height=0)=0;
		virtual void TileImage(Texture* texture)=0;
		virtual void SetColor(const float r, const float g, const float b);
		virtual void SetColor(const float r, const float g, const float b, const float a);
		virtual void SetShader(Shader* shader)=0;
		virtual void SetTranslation(const float x, const float y);
		virtual void SetRotation(const float rotation);
		virtual void SetScale(const float x, const float y);
		
		//Static members
		static int SHADER_EDIT;
		static int SHADER_LIGHT_DIRECTIONAL;
		static int SHADER_LIGHT_POINT;
		static int SHADER_LIGHT_SPOT;
		static int SHADER_DRAW_TEXT;
		static int SHADER_DRAW_TEXT_LCD;
		static int SHADER_DRAW_IMAGE;
		static int SHADER_DRAW_IMAGE_CUBEMAP;
		static int SHADER_DRAW_PRIMITIVE;
		static int SHADER_OCCLUSION_QUERY;
		static int SHADER_DRAW_IMAGE_VOLUME;
		static int WireframeShader;
		static int SolidShader;
		static int TexturedShader;		
		static int GridShader;
		
		//Static functions
		static GraphicsDriver* GetCurrent();//lua
		static void SetCurrent(GraphicsDriver* graphicsdriver);//lua
		static GraphicsDriver* current;
	};
}
