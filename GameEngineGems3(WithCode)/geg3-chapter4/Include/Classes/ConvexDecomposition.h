#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Surface;

	class ConvexDecomposition
	{
	public:
		Surface* surface;

		void AddEntity(Entity* entity, Entity* root);
		void AddSurface(Surface* surface,Mat4& src,Mat4& dst);
		std::vector<Surface*> Build(Entity* entity, int usercallback(float) = NULL, int resolution = 1000000, int depth = 20, float concavity = 0.0001, int planeDownsampling = 4, int convexhullDownsampling = 4, float alpha = 0.05, float beta = 0.05, float gamma = 0.0001, int pca = 0, int mode = 0, int maxNumVerticesPerCH = 256, float minVolumePerCH = 0.0f);
		std::vector<Surface*> Build(Surface* surface, int usercallback(float) = NULL, int resolution = 1000000, int depth = 20, float concavity = 0.0001, int planeDownsampling = 4, int convexhullDownsampling = 4, float alpha = 0.05, float beta = 0.05, float gamma = 0.0001, int pca = 0, int mode = 0, int maxNumVerticesPerCH = 256, float minVolumePerCH = 0.0f);
	};
}