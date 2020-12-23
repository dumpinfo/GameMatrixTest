#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class MaterialValue
	{
	public:
		std::string name;
		Mat4 value;
		virtual void Update( Shader* shader );
		int size;
		
		MaterialValue();
	};
}
