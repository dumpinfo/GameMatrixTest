#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Texture;
	class Character;
	class AssetReference;

	class FontReference : public AssetReference
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

		FontReference();	
		virtual ~FontReference();
		
		virtual std::string GetClassName();
		virtual Character* LoadChar(const int c, const bool force = false);
		virtual bool BuildAtlas();
		virtual void DrawText(const std::string& text, const int x, const int y, const int kerning=0);
		virtual int GetTextWidth(const std::string& text);
		virtual bool Reload(const int flags=0);
		virtual bool Reload(Stream* stream, const int flags=0);
		virtual Asset* Instance();
		virtual AssetReference* Copy();
		virtual void Reset();
	};
}
