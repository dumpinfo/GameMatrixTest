#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Surface;
	
	class Batch
	{
	public:
		int count;
		//std::vector<Mat4> instances;
		std::vector<Entity*> entities;
		std::vector<bool> selectionstate;
		Surface* surface;
		std::vector<float> instancematrices;

		Batch();
		virtual ~Batch();
		
		virtual void DrawInstance(Camera* camera, const Mat4& mat, const Vec4& color, Entity* entity);
		//virtual void DrawInstance(Camera* camera, const dMat4& mat, const Vec4& color, Entity* entity);
		//virtual void AddInstance(Camera* camera, const dMat4& mat, const Vec4& color, Entity* entity)
		//virtual void AddInstance(Camera* camera, const int num=1);
		virtual void AddInstance(Camera* camera, const Mat4& mat, const Vec4& color, Entity* entity);
		virtual void Clear();
		virtual void Draw(Camera* camera, const int start=0)=0;
	};
}
