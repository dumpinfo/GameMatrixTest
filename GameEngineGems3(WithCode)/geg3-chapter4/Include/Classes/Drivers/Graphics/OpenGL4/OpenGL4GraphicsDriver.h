#pragma once

#include "../OpenGL2/OpenGL2GraphicsDriver.h"
#include "../../../../Leadwerks.h"
#include "Classes/OpenGL4VertexArray.h"
#include "Classes/OpenGL4IndiceArray.h"
#include "Classes/OpenGL4Texture.h"
#include "Classes/OpenGL4Context.h"
#include "Classes/OpenGL4Shader.h"
#include "Classes/OpenGL4Camera.h"
#include "Classes/OpenGL4Buffer.h"
#include "Classes/OpenGL4Uniform.h"
#include "Classes/OpenGL4Batch.h"
#include "Classes/OpenGL4Material.h"
#include "Classes/OpenGL4SpotLight.h"
#include "Classes/OpenGL4PointLight.h"
#include "Classes/OpenGL4DirectionalLight.h"

namespace Leadwerks
{
	class Texture;
	class OpenGLSurface;
	class Font;
	class OpenGL2GraphicsDriver;

	class OpenGL4GraphicsDriver : public OpenGL2GraphicsDriver
	{
	public:
		GLuint matrixbuffer;
		GLuint buffer_bonematrices;
		GLuint buffer_instancematrices;

		OpenGL4GraphicsDriver();
		virtual ~OpenGL4GraphicsDriver();

		virtual PointLight* CreatePointLight();
		virtual SpotLight* CreateSpotLight();
		virtual DirectionalLight* CreateDirectionalLight();
		virtual Uniform* CreateUniform();
		virtual bool Initialize();
		virtual Buffer* CreateBuffer(const int width, const int height, const int colorcomponents, const int depthbuffer, const int multisamplemode);
		virtual VertexArray* CreateVertexArray(const int target,const int type,const int coordinates);
		virtual IndiceArray* CreateIndiceArray(const int mode, const int type);
		virtual Context* CreateContext(Window* window,const int multisamplemode);
		virtual Camera* CreateCamera(Entity* parent);
		virtual Shader* CreateShader();
		virtual Surface* CreateSurface();
		virtual int GetMaxTextureSamples();
		virtual void DrawRectBatch(const float* f, const float* c, const int count);
	};
}
