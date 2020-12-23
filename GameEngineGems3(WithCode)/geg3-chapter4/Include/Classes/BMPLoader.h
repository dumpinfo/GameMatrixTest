#pragma once

#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class BMPLoader : public Loader
	{
	public:
		virtual bool Load(AssetReference* assetreference, Stream* stream);
	};
}
