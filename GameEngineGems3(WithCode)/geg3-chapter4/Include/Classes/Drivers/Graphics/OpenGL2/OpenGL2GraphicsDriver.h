#pragma once

#include "../../../../Leadwerks.h"

#undef DrawText

namespace Leadwerks
{
	class Texture;
	class OpenGLSurface;
	class Font;
	class OpenGL2Context;

	//Rename this "GraphicsModule"
	class OpenGL2GraphicsDriver : public GraphicsDriver
	{
	public:
		int maxtextureunits;
		map<std::string,std::string> openglextensions;
		Context* dummycontext;
		OpenGL2Context* window;

#ifdef _WIN32
			HGLRC currenthrc;
#endif

		bool dummycontextcreationfailed;
		int vendor;
		Shader* defaultmodelshader[2][2][2][2][2][2][2][2][2][3];
		bool polygonstipplepatterninitialized__;
		GLubyte polygonstipplepattern__[32][32];
		int maxcontextsamples;
		int maxtexturesamples;
		bool unloadshaderonendchars;
		Shader* shader_grid;
		//Shader* shader[16];

		OpenGL2GraphicsDriver();
		virtual ~OpenGL2GraphicsDriver();

		virtual Uniform* CreateUniform();
		virtual void Reset();
		virtual void ReloadAutoShaders();
        virtual PointLight* CreatePointLight();
		virtual SpotLight* CreateSpotLight();
		virtual DirectionalLight* CreateDirectionalLight();
		virtual bool glExtensionSupported(const std::string& name);
		virtual OcclusionQuery* CreateOcclusionQuery();
		virtual bool Initialize();
		//virtual Shader* ChooseShader(MaterialReference* materialreference, const int shaderindex);
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
		virtual Context* CreateContext(Window* window,const int multisamplemode);
		virtual Texture* CreateTexture(const int width, const int height, const int format, const int flags, const int frames);
		//virtual Texture* LoadTexture(std::string path,const int flags);
		//virtual Shader* LoadShader(const std::string& path);
		virtual Surface* CreateSurface();
		virtual Material* CreateMaterial();
		virtual void SetContext(Context* context);
		virtual Camera* CreateCamera(Entity* parent);
		//virtual Material* CreateMaterial();
		virtual Buffer* CreateBuffer(const int width, const int height, const int colorcomponents, const int depthbuffer, const int multisamplemode);
		virtual Shader* CreateShader();
		virtual void SetShader(Shader* shader);
		//virtual void SetStippleMode(const bool mode);
		//virtual void SetBlendMode(const int mode);
		virtual void DrawText(const std::string& text, const float x, const float y, Font* font);
		virtual void DrawRect(const float x, const float y, const float width, const float height, const int style=0);
		virtual void DrawLine(const float x0, const float y0, const float x1, const float y1, const bool drawlastpixel=false);
		virtual void DrawPixel(const float x, const float y);
        virtual void BeginChars(const float x, const float y, const int mode);
        virtual void EndChars();
		virtual void DrawChar(const float x, const float y, const float width, const float height, const Vec4& coords);
		virtual void DrawImage(Texture* texture, const float x, const float y, const float width=0, const float height=0);
		virtual void TileImage(Texture* texture);
        
		static OpenGL2GraphicsDriver* Create();//lua
		//static int SHADER_DRAWPRIMITIVE;
		//static int SHADER_DRAWIMAGE;
		//static int SHADER_DRAWIMAGECUBEMAP;
		//static int SHADER_DRAWTEXT;
		//static int SHADER_DRAWLCDTEXT;
	};

//	void glCheckError();
//	GraphicsDriver* OpenGLGraphicsDriver();
	OpenGL2GraphicsDriver* CreateOpenGL2GraphicsDriver();
}
