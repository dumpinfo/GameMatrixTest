#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Bank;

	class VertexArray
	{
	public:
		Bank* data;
		int target;
		int type;
		int coordinates;
		int lockcount;
		bool dynamic;
		bool altered;
		int lockmode;
		int memusage;
		GraphicsDriver* driver;
		
		VertexArray();
		virtual ~VertexArray();
		
		virtual VertexArray* Copy()=0;
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		virtual void Clear();
		virtual void Lock()=0;
		virtual void Unlock()=0;
		virtual void Enable()=0;
		virtual void Disable()=0;
	};
}
