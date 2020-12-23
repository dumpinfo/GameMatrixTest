 

#include "C4Polyhedron.h"


using namespace C4;


Convexity::Convexity(const Convexity& convexity)
{
	planeCount = convexity.planeCount;

	for (unsigned_machine a = 0; a < planeCount; a++)
	{
		plane[a] = convexity.plane[a];
	}
}

Convexity& Convexity::operator =(const Convexity& convexity)
{
	planeCount = convexity.planeCount;

	for (unsigned_machine a = 0; a < planeCount; a++)
	{
		plane[a] = convexity.plane[a];
	}

	return (*this);
}


Polyhedron::Polyhedron(const Polyhedron& polyhedron)
{
	planeCount = polyhedron.planeCount;
	vertexCount = polyhedron.vertexCount;
	edgeCount = polyhedron.edgeCount;
	faceCount = polyhedron.faceCount;

	for (unsigned_machine a = 0; a < planeCount; a++)
	{
		plane[a] = polyhedron.plane[a];
	}

	for (unsigned_machine a = 0; a < vertexCount; a++)
	{
		vertex[a] = polyhedron.vertex[a];
	}

	for (unsigned_machine a = 0; a < edgeCount; a++)
	{
		edge[a] = polyhedron.edge[a];
	}

	for (unsigned_machine a = 0; a < faceCount; a++)
	{
		face[a] = polyhedron.face[a];
	}
}

Polyhedron& Polyhedron::operator =(const Polyhedron& polyhedron)
{
	planeCount = polyhedron.planeCount;
	vertexCount = polyhedron.vertexCount;
	edgeCount = polyhedron.edgeCount;
	faceCount = polyhedron.faceCount;

	for (unsigned_machine a = 0; a < planeCount; a++)
	{
		plane[a] = polyhedron.plane[a];
	}

	for (unsigned_machine a = 0; a < vertexCount; a++)
	{
		vertex[a] = polyhedron.vertex[a];
	}

	for (unsigned_machine a = 0; a < edgeCount; a++)
	{
		edge[a] = polyhedron.edge[a];
	}

	for (unsigned_machine a = 0; a < faceCount; a++)
	{
		face[a] = polyhedron.face[a];
	}

	return (*this);
}

int32 Polyhedron::GetTriangleArray(Triangle *restrict triangle) const
{
	const Triangle *start = triangle;

	for (machine a = 0; a < faceCount; a++)
	{
		const Face *polyhedronFace = &face[a];

		const Edge *faceEdge = &edge[polyhedronFace->edgeIndex[0]];
		unsigned_int32 i0 = faceEdge->vertexIndex[0];

		int32 faceEdgeCount = polyhedronFace->edgeCount;
		for (machine b = 1; b < faceEdgeCount; b++)
		{ 
			faceEdge = &edge[polyhedronFace->edgeIndex[b]];
			unsigned_int32 i1 = faceEdge->vertexIndex[0];
			unsigned_int32 i2 = faceEdge->vertexIndex[1]; 

			if ((i1 != i0) && (i2 != i0)) 
			{
				if (faceEdge->faceIndex[0] == a)
				{ 
					triangle->Set(i0, i1, i2);
				} 
				else 
				{
					triangle->Set(i0, i2, i1);
				}
 
				triangle++;
			}
		}
	}

	return ((int32) (triangle - start));
}

#if C4DEBUG

	bool Polyhedron::Validate(void) const
	{
		for (unsigned_machine a = 0; a < edgeCount; a++)
		{
			if ((edge[a].vertexIndex[0] >= vertexCount) || (edge[a].vertexIndex[1] >= vertexCount))
			{
				return (false);
			}

			if ((edge[a].faceIndex[0] >= faceCount) || (edge[a].faceIndex[1] >= faceCount))
			{
				return (false);
			}
		}

		for (unsigned_machine a = 0; a < faceCount; a++)
		{
			unsigned_int32 count = face[a].edgeCount;
			for (unsigned_machine b = 0; b < count; b++)
			{
				if (face[a].edgeIndex[b] >= edgeCount)
				{
					return (false);
				}
			}
		}

		return (faceCount == planeCount);
	}

#endif


bool Math::ClipPolyhedron(const Polyhedron *polyhedron, const Antivector4D& clippingPlane, Polyhedron *restrict result)
{
	int8				vertexLocation[kMaxPolyhedronVertexCount];
	int8				edgeLocation[kMaxPolyhedronEdgeCount];

	unsigned_int8		vertexRemap[kMaxPolyhedronVertexCount];
	unsigned_int8		edgeRemap[kMaxPolyhedronEdgeCount];
	unsigned_int8		faceRemap[kMaxPolyhedronFaceCount];

	unsigned_int8		planeEdgeTable[kMaxFaceEdgeCount];

	int32 minLocation = 6;
	int32 maxLocation = 0;

	unsigned_int32 vertexCount = polyhedron->vertexCount;
	for (unsigned_machine a = 0; a < vertexCount; a++)
	{
		vertexRemap[a] = 0xFF;

		float d = clippingPlane ^ polyhedron->vertex[a];
		int8 location = (d > -kBoundaryEpsilon) + (d > kBoundaryEpsilon) * 2;
		vertexLocation[a] = location;

		minLocation = Min(minLocation, location);
		maxLocation = Max(maxLocation, location);
	}

	if (minLocation != 0)
	{
		*result = *polyhedron;
		return (true);
	}
	else if (maxLocation <= 1)
	{
		return (false);
	}

	unsigned_int32 edgeCount = polyhedron->edgeCount;
	for (unsigned_machine a = 0; a < edgeCount; a++)
	{
		edgeRemap[a] = 0xFF;

		const Edge *edge = &polyhedron->edge[a];
		edgeLocation[a] = (int8) (vertexLocation[edge->vertexIndex[0]] + vertexLocation[edge->vertexIndex[1]]);
	}

	unsigned_int32 resultFaceCount = 0;

	unsigned_int32 faceCount = polyhedron->faceCount;
	for (unsigned_machine a = 0; a < faceCount; a++)
	{
		faceRemap[a] = 0xFF;

		const Face *face = &polyhedron->face[a];
		unsigned_int32 faceEdgeCount = face->edgeCount;

		for (unsigned_machine b = 0; b < faceEdgeCount; b++)
		{
			if (edgeLocation[face->edgeIndex[b]] >= 3)
			{
				// Face has a vertex on the positive side of the plane.

				result->plane[resultFaceCount] = polyhedron->plane[a];
				faceRemap[a] = (unsigned_int8) resultFaceCount++;
				break;
			}
		}
	}

	unsigned_int32 resultVertexCount = 0;
	unsigned_int32 resultEdgeCount = 0;
	unsigned_int32 planeEdgeCount = 0;

	for (unsigned_machine a = 0; a < edgeCount; a++)
	{
		if (edgeLocation[a] >= 3)
		{
			// At least one endpoint lies on the positive side of the clipping plane.

			edgeRemap[a] = (unsigned_int8) resultEdgeCount;

			const Edge *edge = &polyhedron->edge[a];
			Edge *resultEdge = &result->edge[resultEdgeCount];
			resultEdgeCount++;

			resultEdge->faceIndex[0] = faceRemap[edge->faceIndex[0]];
			resultEdge->faceIndex[1] = faceRemap[edge->faceIndex[1]];

			unsigned_int32 vertexIndex1 = edge->vertexIndex[0];
			unsigned_int32 vertexIndex2 = edge->vertexIndex[1];

			if (vertexLocation[vertexIndex1] != 0)
			{
				// Vertex 1 is on the positive side of the clipping plane or lies in the clipping plane.

				unsigned_int8 remappedVertexIndex = vertexRemap[vertexIndex1];
				if (remappedVertexIndex == 0xFF)
				{
					remappedVertexIndex = resultVertexCount++;
					vertexRemap[vertexIndex1] = remappedVertexIndex;
					result->vertex[remappedVertexIndex] = polyhedron->vertex[vertexIndex1];
				}

				resultEdge->vertexIndex[0] = remappedVertexIndex;
			}
			else
			{
				// Vertex 1 is on the negative side, vertex 2 is on the positive side.

				const Point3D& p1 = polyhedron->vertex[vertexIndex2];
				const Point3D& p2 = polyhedron->vertex[vertexIndex1];
				Vector3D dp = p2 - p1;

				float d1 = clippingPlane ^ p1;
				float ddp = clippingPlane ^ dp;

				float t = d1 / ddp;
				result->vertex[resultVertexCount] = p1 - dp * t;
				resultEdge->vertexIndex[0] = (unsigned_int8) resultVertexCount++;
			}

			if (vertexLocation[vertexIndex2] != 0)
			{
				// Vertex 2 is on the positive side of the clipping plane or lies in the clipping plane.

				unsigned_int8 remappedVertexIndex = vertexRemap[vertexIndex2];
				if (remappedVertexIndex == 0xFF)
				{
					remappedVertexIndex = resultVertexCount++;
					vertexRemap[vertexIndex2] = remappedVertexIndex;
					result->vertex[remappedVertexIndex] = polyhedron->vertex[vertexIndex2];
				}

				resultEdge->vertexIndex[1] = remappedVertexIndex;
			}
			else
			{
				// Vertex 2 is on the negative side, vertex 1 is on the positive side.

				const Point3D& p1 = polyhedron->vertex[vertexIndex1];
				const Point3D& p2 = polyhedron->vertex[vertexIndex2];
				Vector3D dp = p2 - p1;

				float d1 = clippingPlane ^ p1;
				float ddp = clippingPlane ^ dp;

				float t = d1 / ddp;
				result->vertex[resultVertexCount] = p1 - dp * t;
				resultEdge->vertexIndex[1] = (unsigned_int8) resultVertexCount++;
			}
		}
	}

	for (unsigned_machine a = 0; a < faceCount; a++)
	{
		unsigned_int8 remappedFaceIndex = faceRemap[a];
		if (remappedFaceIndex != 0xFF)
		{
			unsigned_int8 newEdgeIndex = 0xFF;

			const Face *face = &polyhedron->face[a];
			unsigned_int32 faceEdgeCount = face->edgeCount;

			Face *resultFace = &result->face[remappedFaceIndex];
			unsigned_int32 resultFaceEdgeCount = 0;

			Edge *newEdge = nullptr;
			for (unsigned_machine b = 0; b < faceEdgeCount; b++)
			{
				unsigned_int8 edgeIndex = face->edgeIndex[b];

				int32 location = edgeLocation[edgeIndex];
				if (location >= 3)
				{
					if (location >= 4)
					{
						// Neither endpoint is on the negative side of the clipping plane.

						resultFace->edgeIndex[resultFaceEdgeCount++] = edgeRemap[edgeIndex];
					}
					else
					{
						// One endpoint is on positive side, and other is on negative side.

						if (!newEdge)
						{
							newEdgeIndex = resultEdgeCount;
							newEdge = &result->edge[resultEdgeCount];
							planeEdgeTable[planeEdgeCount++] = (unsigned_int8) resultEdgeCount;
							resultEdgeCount++;

							newEdge->faceIndex[0] = remappedFaceIndex;
							newEdge->faceIndex[1] = 0xFF;
							newEdge->vertexIndex[0] = 0xFF;
							newEdge->vertexIndex[1] = 0xFF;
						}

						const Edge *edge = &polyhedron->edge[edgeIndex];
						unsigned_int8 index = edge->vertexIndex[0];

						unsigned_int8 remappedEdgeIndex = edgeRemap[edgeIndex];
						const Edge *resultEdge = &result->edge[remappedEdgeIndex];

						if (edge->faceIndex[0] == a)
						{
							if (vertexLocation[index] != 0)
							{
								newEdge->vertexIndex[0] = resultEdge->vertexIndex[1];
								resultFace->edgeIndex[resultFaceEdgeCount++] = remappedEdgeIndex;
								resultFace->edgeIndex[resultFaceEdgeCount++] = newEdgeIndex;
							}
							else
							{
								newEdge->vertexIndex[1] = resultEdge->vertexIndex[0];
								resultFace->edgeIndex[resultFaceEdgeCount++] = remappedEdgeIndex;
							}
						}
						else
						{
							if (vertexLocation[index] != 0)
							{
								newEdge->vertexIndex[1] = resultEdge->vertexIndex[1];
								resultFace->edgeIndex[resultFaceEdgeCount++] = remappedEdgeIndex;
							}
							else
							{
								newEdge->vertexIndex[0] = resultEdge->vertexIndex[0];
								resultFace->edgeIndex[resultFaceEdgeCount++] = newEdgeIndex;
								resultFace->edgeIndex[resultFaceEdgeCount++] = remappedEdgeIndex;
							}
						}
					}
				}
			}

			if ((newEdge) && (Max(newEdge->vertexIndex[0], newEdge->vertexIndex[1]) == 0xFF))
			{
				// If we get here, then the inputs led to numerical precision problems.

				*result = *polyhedron;
				return (true);
			}

			resultFace->edgeCount = (unsigned_int8) resultFaceEdgeCount;
		}
	}

	if (planeEdgeCount > 2)
	{
		result->plane[resultFaceCount] = clippingPlane;

		Face *resultFace = &result->face[resultFaceCount];
		resultFace->edgeCount = (unsigned_int8) planeEdgeCount;

		for (unsigned_machine a = 0; a < planeEdgeCount; a++)
		{
			unsigned_int8 edgeIndex = planeEdgeTable[a];
			resultFace->edgeIndex[a] = edgeIndex;

			Edge *resultEdge = &result->edge[edgeIndex];
			resultEdge->faceIndex[1] = (unsigned_int8) resultFaceCount;
		}

		resultFaceCount++;
	}

	result->planeCount = (unsigned_int8) resultFaceCount;
	result->vertexCount = (unsigned_int8) resultVertexCount;
	result->edgeCount = (unsigned_int8) resultEdgeCount;
	result->faceCount = (unsigned_int8) resultFaceCount;
	return (true);
}

// ZYUQURM
