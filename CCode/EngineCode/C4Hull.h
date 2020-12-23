 

#ifndef C4Hull_h
#define C4Hull_h


#include "C4Bounding.h"


namespace C4
{
	class HullTriangle;


	class HullVertex : public ListElement<HullVertex>
	{
		public:

			Point3D			position;
			unsigned_int32	vertexIndex;

			HullVertex(const Point3D& p, unsigned_int32 i);
	};


	class HullEdge : public GraphEdge<HullTriangle, HullEdge>
	{
		public:

			int32			edgeIndex[2];

			HullEdge(HullTriangle *t1, HullTriangle *t2, int32 i1, int32 i2);
	};


	class HullTriangle : public GraphElement<HullTriangle, HullEdge>, public ListElement<HullTriangle>
	{
		public:

			HullVertex		*vertex[3];
			Antivector4D	plane;

			HullTriangle(HullVertex *v1, HullVertex *v2, HullVertex *v3);

			using ListElement<HullTriangle>::Previous;
			using ListElement<HullTriangle>::Next;

			void Negate(void);
	};


	struct Triangle;


	namespace Math
	{
		C4API int32 ComputeConvexHull(int32 vertexCount, const Point3D *vertex, const Box3D& boundingBox, unsigned_int16 *hullIndex);
	}
}


#endif

// ZYUQURM
