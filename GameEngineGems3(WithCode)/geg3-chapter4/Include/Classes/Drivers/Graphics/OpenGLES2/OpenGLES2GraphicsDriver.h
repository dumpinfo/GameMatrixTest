#pragma once

#include "../../../../Leadwerks.h"
//#include "../../../../OpenGL/OpenGL.h"

#include "Classes/OpenGLES2VertexArray.h"
#include "Classes/OpenGLES2IndiceArray.h"
#include "Classes/OpenGLES2Texture.h"
#include "Classes/OpenGLES2Context.h"
#include "Classes/OpenGLES2Surface.h"
#include "Classes/OpenGLES2Uniform.h"
#include "Classes/OpenGLES2Shader.h"
#include "Classes/OpenGLES2OcclusionQuery.h"
#include "Classes/OpenGLES2Camera.h"
#include "Classes/OpenGLES2Buffer.h"
#include "Classes/OpenGLES2Model.h"
#include "Classes/OpenGLES2Batch.h"
#include "Classes/OpenGLES2Material.h"

#undef DrawText

namespace Leadwerks
{
	class Texture;
	class OpenGLSurface;
	class Font;
	
	class OpenGLES2GraphicsDriver : public GraphicsDriver//lua
	{
	public:
		map<std::string,std::string> openglextensions;
		Shader* shader_ambientlight;
		Shader* shader_directionallight;
		OpenGLES2Context* dummycontext;
		//Mat4 projectionmatrix;
		//Mat4 cameramatrix;
		//Mat4 projectioncameramatrix;
		OpenGLES2Context* window;
		bool unloadshaderonendchars;
        static GLuint backframebufferobject;
		
		#ifdef _WIN32
			HGLRC currenthrc;
		#endif
		
		bool dummycontextcreationfailed;
		int vendor;
		Shader* postshader[2][2];
		Shader* defaultmodelshader[2][2][2][2][2][2][2][2][2];
		Shader* DrawImageShader;
		Shader* shader_drawimagecubemap;
		Shader* DrawShader;
		Shader* default_diffuse;
		Shader* default_diffuse_normal;
		Shader* default_diffuse_normal_specular;
		Shader* default_diffuse_normal_specular_reflection;
		bool polygonstipplepatterninitialized__;
		GLubyte polygonstipplepattern__[32][32];
		int maxcontextsamples;
		int maxtexturesamples;
		
		OpenGLES2GraphicsDriver();
		
		virtual SpotLight* CreateSpotLight();
		virtual PointLight* CreatePointLight();
		virtual DirectionalLight* CreateDirectionalLight();
		virtual void Reset();
		virtual void ReloadAutoShaders();
		virtual bool glExtensionSupported(const std::string& name);
		virtual OcclusionQuery* CreateOcclusionQuery();
		virtual bool Initialize();
		//virtual Shader* ChooseShader(Material* material, const int shaderindex=SHADER_DEFAULT);
		virtual bool BuffersSupported();
		virtual int GetMaxBufferColorTextures();
		virtual int GetMaxTextureUnits();
		virtual int GetMaxContextSamples();
		virtual int GetMaxTextureSamples();
		virtual void SetViewport(const int x, const int y, const int width, const int height);
		virtual int glGetVersion();
		virtual int glGetShaderVersion();
		virtual void EnableDummyContext();
		virtual void EnableCurrentContext();
		virtual bool CreateDummyContext();
		virtual Material* GetDefaultMaterial();
		virtual Shader* GetDefaultShader();
		virtual bool Supported();
		virtual int GetVendor();
		virtual std::string GetDeviceName();
		Surface* drawrectsurface;
		Surface* drawlinesurface;
		Surface* drawpixelsurface;
		virtual VertexArray* CreateVertexArray(const int target,const int type,const int coordinates);
		virtual IndiceArray* CreateIndiceArray(const int mode, const int type);
		//virtual void SetVerticalSync(const bool mode);
		virtual Texture* CreateTexture();//const int width, const int height, const int format, const int flags, const int frames);
		virtual OpenGLES2Context* CreateContext(Window* window,const int multisamplemode);
		virtual Texture* CreateTexture(const int width, const int height, const int format, const int flags, const int frames);
		//virtual Texture* LoadTexture(std::string path,const int flags);
		//virtual Shader* LoadShader(const std::string& path);
		virtual Surface* CreateSurface();
		virtual Material* CreateMaterial();
		virtual void SetContext(Context* context);
		virtual Camera* CreateCamera(Entity* parent);
		virtual Buffer* CreateBuffer(const int width, const int height, const int colorcomponents, const int depthbuffer, const int multisamplemode);
		virtual Shader* CreateShader();
		virtual void SetShader(Shader* shader);
		//virtual void SetStippleMode(const bool mode);
		virtual void SetBlendMode(const int mode);
		virtual void DrawText(const std::string& text, const float x, const float y, Font* font);
		virtual void DrawRect(const float x, const float y, const float width, const float height, const int style=0);
		virtual void DrawLine(const float x0, const float y0, const float x1, const float y1, const bool drawlastpixel=false);
		virtual void DrawPixel(const float x, const float y);
        virtual void BeginChars(const float x, const float y, const int mode);
        virtual void EndChars();        
		virtual void DrawChar(const float x, const float y, const float width, const float height, const Vec4& coords);
		virtual void DrawImage(Texture* texture, const float x, const float y, const float width=0, const float height=0);
		virtual void TileImage(Texture* texture);
        virtual Model* CreateModel();
		virtual Uniform* CreateUniform();

		static OpenGLES2GraphicsDriver* Create();
	};
//	void glCheckError();
//	GraphicsDriver* OpenGLGraphicsDriver();
	OpenGLES2GraphicsDriver* CreateOpenGLES2GraphicsDriver();
}
