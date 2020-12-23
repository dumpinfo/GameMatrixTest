#pragma once
#pragma warning(disable:4290)
#pragma warning(disable:4996)

#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	
	class Mipmap
	{
		public:
		
		//Attributes
		//int width;
		//int height;
		Bank* pixels;
		
		Mipmap();
		~Mipmap();

		//Methods
		//virtual int ReadPixel(const int x, const int y)=0;
		//virtual int WritePixel(const int x, const int y, const bool send)=0;
		//virtual void Send(const int x, const int y, const int width, const int height)=0;
		//virtual void Retrieve(const int x, const int y, const int width, const int height)=0;
		
	};
	
}
