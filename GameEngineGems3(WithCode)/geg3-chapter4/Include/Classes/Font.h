#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Texture;
	class Character;

	class Font : public Asset//lua
	{
	public:
		Texture* texture;
		FT_Face face;
        int height;
		bool faceinitialized;
		int textureatlasx;
		int textureatlasy;
		int textureatlasmaxy;
		std::map<int,Character*> character;
		int style;
		int size;
		bool vertical;
		int family;

		Font();
		virtual ~Font();

		virtual std::string GetClassName();
		virtual Character* LoadChar(const int c, const bool force = false);
		virtual bool BuildAtlas();
		virtual void DrawText(const std::string& text, const float x, const float y, const float kerning=0);//lua
		virtual int GetTextWidth(const std::string& text);//lua
        virtual int GetHeight();//lua
        virtual Texture* GetTexture();
		virtual bool Reload(const int flags=0);
		virtual bool Reload(Stream* stream, const int flags=0);
		virtual Asset* Copy();
		virtual void Reset();

        static const int Pixel;//lua
        static const int Smooth;//lua
        static const int LCD;//lua
        static const int English;//lua

		static Font* Load(const std::string& path, const int size, const int style = Font::Smooth, const int family = Font::English, const int flags = 0, const uint64_t fileid=0);//lua
		static Font* Load(Stream* stream, const int size, const int style = Font::Smooth, const int family = Font::English, const int flags=0);//lua
	};
}
