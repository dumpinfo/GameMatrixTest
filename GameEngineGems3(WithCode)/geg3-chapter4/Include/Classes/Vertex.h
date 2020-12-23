#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	class Vec2;
	class Vec3;
	class Vec4;
	
	class Vertex
	{
	public:
		Vec3 position;
		Vec3 normal;
		Vec2 texcoords[2];
		Vec4 color;
		Vec3 binormal;
		Vec3 tangent;
        Vec4 boneweights;
        iVec4 boneindices;
		int index;

		Vertex();
		Vertex(const Vec3& position, const Vec3& normal=Vec3(0), const Vec2& texcoords0=Vec2(0), const Vec2& texcoords1=Vec2(0), const Vec4& color=Vec4(1));

		bool operator<(const Vertex vertex);
	};
	
	bool operator<(const Vertex& v0, const Vertex& v1);
}
