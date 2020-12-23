 

#ifndef C4Polyhedron_h
#define C4Polyhedron_h


#include "C4Types.h"


namespace C4
{
	enum
	{
		kMaxPolyhedronVertexCount	= 28,
		kMaxPolyhedronEdgeCount		= 42,
		kMaxPolyhedronFaceCount		= 16,
		kMaxFaceEdgeCount			= 15
	};

	static_assert(kMaxPolyhedronVertexCount - kMaxPolyhedronEdgeCount + kMaxPolyhedronFaceCount == 2, "Max polyhedron parameters must satisfy Euler's Formula");


	struct Edge
	{
		unsigned_int8		vertexIndex[2];
		unsigned_int8		faceIndex[2];
	};


	struct Face
	{
		unsigned_int8		edgeCount;
		unsigned_int8		edgeIndex[kMaxFaceEdgeCount];
	};


	struct Convexity
	{
		unsigned_int8		planeCount;
		unsigned_int8		vertexCount;
		unsigned_int8		edgeCount;
		unsigned_int8		faceCount;

		Antivector4D		plane[kMaxPolyhedronFaceCount];

		Convexity() = default;
		Convexity(const Convexity& convexity);

		Convexity& operator =(const Convexity& convexity);
	};


	struct Polyhedron : Convexity
	{
		Point3D				vertex[kMaxPolyhedronVertexCount];
		Edge				edge[kMaxPolyhedronEdgeCount];
		Face				face[kMaxPolyhedronFaceCount];

		Polyhedron()
		{
			planeCount = 0;
			vertexCount = 0;
			edgeCount = 0;
			faceCount = 0;
		}

		Polyhedron(const Polyhedron& polyhedron);

		Polyhedron& operator =(const Polyhedron& polyhedron);

		int32 GetTriangleArray(Triangle *restrict triangle) const;

		#if C4DEBUG

			bool Validate(void) const;

		#endif
	};


	namespace Math
	{
		bool ClipPolyhedron(const Polyhedron *polyhedron, const Antivector4D& clippingPlane, Polyhedron *restrict result);
	}
}


#endif

// ZYUQURM
