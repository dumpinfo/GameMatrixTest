#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class FileSystem;

	class Initializer
	{
		Initializer();
		static Initializer* _initializer;
	};
}
