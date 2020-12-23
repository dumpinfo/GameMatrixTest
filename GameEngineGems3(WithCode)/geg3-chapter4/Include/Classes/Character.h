#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Vec4;
	class Bank;
	
	class Character
	{
	public:
		int id;
		int x;
		int y;
		int width;
		int height;
		int spacing;
		int atlasx;
		int atlasy;
		Vec4 texcoords;
		Bank* data;
		
		Character();
		virtual ~Character();

		virtual Character* Copy();
	};
}
