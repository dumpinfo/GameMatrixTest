#pragma once
#pragma warning(disable:4290)
#pragma warning(disable:4996)

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Directory
	{
		public:
		vector<std::string> files;
		int CountFiles();
		std::string GetFile(const int n);
	};
}
