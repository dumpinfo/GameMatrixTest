#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Window;
	class Buffer;
	
	class Context : public Buffer//lua
	{
		public:
		Window* window;
		GraphicsDriver* graphicsdriver;
		
		bool keydownstate[256];
		bool keyhitstate[256];
		bool mousedownstate[5];
		bool mousehitstate[5];
		int mousex,mousey,mousez;		
		std::list<OcclusionQuery*> occlusionqueries;

		Context();
		virtual ~Context();
		
		virtual std::string GetClassName();
		virtual std::string Debug();
		//virtual int GetWidth()=0;
		//virtual int GetHeight()=0;
		//virtual void MakeCurrent()=0;
		
		/*virtual bool KeyDown(const int keycode);
		virtual bool KeyHit(const int keycode);
		virtual void FlushKeys();
		
		virtual bool MouseDown(const int button=MOUSE_LEFT);
		virtual bool MouseHit(const int button=MOUSE_RIGHT);
		virtual int MouseX();
		virtual int MouseY();
		virtual int MouseZ();
		virtual void FlushMouse();*/
		
		virtual bool Screenshot(const std::string& path="");//lua
        virtual Window* GetWindow();//lua
		virtual void Sync(const bool sync=true)=0;//lua
		virtual bool SetColorTexture(Texture* texture, const int i=0, const int cubeface=0, const int miplevel=0);
		virtual bool SetDepthTexture(Texture* texture, const int cubeface=0);
		virtual void SetAntialias(const bool mode)=0;
		
		static Context* Create(Window* window,const int multisamplemode=0);//lua
		static Context* GetCurrent();//lua
		static void SetCurrent(Context* context);//lua
		
		virtual void DrawShadowText(const std::string& text, const float x, const float y);
		virtual void DrawStats(const float x, const float y, const bool extra=false);//lua
		virtual void DrawRect(const float x, const float y, const float width, const float height, const int style=0);//lua
		virtual void DrawLine(const float x0, const float y0, const float x1, const float y1, const bool drawlastpixel=false);//lua
		virtual void Plot(const float x, const float y);//lua
		virtual void DrawImage(Texture* texture, const float x, const float y, const float width, const float height);//lua
		virtual void DrawImage(Texture* texture, const float x, const float y);//lua
        virtual Vec4 GetColor();//lua
		virtual void SetTranslation(const float x, const float y);//lua
		virtual void SetRotation(const float rotation);//lua
		virtual void SetScale(const float x, const float y);//lua
        virtual void SetFont(Font* font);//lua
        virtual Font* GetFont();//lua
        virtual void DrawText(const std::string& text, const float x, const float y, const float kerning=1.0);//lua
        virtual void SetShader(Shader* shader);//lua
        virtual Shader* GetShader();//lua
        virtual void SetBlendMode(const int blendmode);//lua
        virtual int GetBlendMode();//lua
        
        //int CurrentBlendMode;
        //Vec4 CurrentColor;
	};
}
