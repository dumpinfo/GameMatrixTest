#pragma once

#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class Loader
	{
	public:
		std::string extension;
		
		virtual bool Load(AssetReference* assetreference, Stream* stream)=0;
	};
}
