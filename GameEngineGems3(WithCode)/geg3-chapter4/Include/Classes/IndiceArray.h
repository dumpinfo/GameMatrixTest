#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class IndiceArray
	{
	public:
		int lockmode;
		Bank* data;
		//int mode;//GL_TRIANGLES
		//int type;//GL_UNSIGNED_SHORT
		bool dynamic;
		bool altered;
		int memusage;
		GraphicsDriver* driver;

		IndiceArray();
		virtual ~IndiceArray();
		
		virtual IndiceArray* Copy()=0;
		virtual void Clear();
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual void Lock()=0;
		virtual void Unlock()=0;
		virtual void Enable()=0;
		virtual void Disable()=0;
	};
}
