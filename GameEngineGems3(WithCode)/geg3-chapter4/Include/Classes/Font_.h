#pragma once

#include "../Leadwerks3D.h"
//#include "Character.h"

#undef DrawText

namespace Leadwerks3D
{
    class FontReference;
	class Asset;

	class Font : public Asset
	{
	public:
		
		Font();	
		virtual ~Font();
		
		virtual Texture* GetTexture();
		virtual void DrawText(const std::string& text, const int x, const int y, const int kerning=0);
		virtual int GetTextWidth(const std::string& text);
		virtual int GetHeight();
        
        static const int Pixel;
        static const int Smooth;
        static const int LCD;
        static const int English;
        
        static Font* Load(const std::string& path, const int size, const int style = Smooth, const int family = English, const int flags=0);        
        static Font* Load(Stream* stream, const int size, const int style = Smooth, const int family = English, const int flags=0);        
	};
}
