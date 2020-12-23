 

#include "C4Hull.h"


using namespace C4;


namespace C4
{
	namespace Math
	{
		const float kHullEpsilon = 0.002F;

		void FindFirstHullEdge(int32 vertexCount, const HullVertex *hullVertex, machine *i1, machine *i2);
		void AugmentConvexHull(List<HullTriangle> *triangleList, HullTriangle *baseTriangle, HullVertex *newVertex, const Point3D& position);
		void ConnectHullTriangles(Graph<HullTriangle, HullEdge> *hullGraph, List<HullTriangle> *triangleList);
		void RemoveInteriorHullVertices(Graph<HullTriangle, HullEdge> *hullGraph, List<HullVertex> *vertexList);
	}
}


HullVertex::HullVertex(const Point3D& p, unsigned_int32 i)
{
	position = p;
	vertexIndex = i;
}


HullEdge::HullEdge(HullTriangle *t1, HullTriangle *t2, int32 i1, int32 i2) : GraphEdge<HullTriangle, HullEdge>(t1, t2)
{
	edgeIndex[0] = i1;
	edgeIndex[1] = i2;
}


HullTriangle::HullTriangle(HullVertex *v1, HullVertex *v2, HullVertex *v3)
{
	vertex[0] = v1;
	vertex[1] = v2;
	vertex[2] = v3;

	Vector3D normal = (v2->position - v1->position) % (v3->position - v1->position);
	plane.Set(normal * InverseMag(normal), v1->position);
}

void HullTriangle::Negate(void)
{
	HullVertex *v = vertex[1];
	vertex[1] = vertex[2];
	vertex[2] = v;

	plane = -plane;
}


void Math::FindFirstHullEdge(int32 vertexCount, const HullVertex *hullVertex, machine *i1, machine *i2)
{
	machine xminIndex = 0;
	machine xmaxIndex = 0;
	machine yminIndex = 0;
	machine ymaxIndex = 0;
	machine zminIndex = 0;
	machine zmaxIndex = 0;

	const Point3D& p = hullVertex[0].position;
	float xmin = p.x;
	float xmax = p.x;
	float ymin = p.y;
	float ymax = p.y;
	float zmin = p.z;
	float zmax = p.z;

	for (machine a = 1; a < vertexCount; a++)
	{
		const Point3D& q = hullVertex[a].position;

		if (q.x < xmin)
		{
			xmin = q.x;
			xminIndex = a;
		}
		else if (q.x > xmax)
		{
			xmax = q.x;
			xmaxIndex = a;
		}

		if (q.y < ymin)
		{
			ymin = q.y;
			yminIndex = a;
		}
		else if (q.y > ymax)
		{
			ymax = q.y;
			ymaxIndex = a;
		}

		if (q.z < zmin)
		{
			zmin = q.z;
			zminIndex = a; 
		}
		else if (q.z > zmax)
		{ 
			zmax = q.z;
			zmaxIndex = a; 
		}
	}
 
	float dx = xmax - xmin;
	float dy = ymax - ymin; 
	float dz = zmax - zmin; 

	if ((dx > dy) && (dx > dz))
	{
		*i1 = xminIndex; 
		*i2 = xmaxIndex;
	}
	else if (dy > dz)
	{
		*i1 = yminIndex;
		*i2 = ymaxIndex;
	}
	else
	{
		*i1 = zminIndex;
		*i2 = zmaxIndex;
	}
}

void Math::AugmentConvexHull(List<HullTriangle> *triangleList, HullTriangle *baseTriangle, HullVertex *newVertex, const Point3D& position)
{
	const HullEdge *edge = baseTriangle->GetFirstOutgoingEdge();
	while (edge)
	{
		HullTriangle *ht = edge->GetFinishElement();
		if (!((ht->plane ^ position) > kHullEpsilon))
		{
			int32 i1 = edge->edgeIndex[1];
			int32 i2 = IncMod<3>(i1);

			HullTriangle *newTriangle = new HullTriangle(ht->vertex[i2], ht->vertex[i1], newVertex);
			triangleList->Append(newTriangle);
			new HullEdge(newTriangle, ht, 0, i1);
		}

		edge = edge->GetNextOutgoingEdge();
	}

	edge = baseTriangle->GetFirstIncomingEdge();
	while (edge)
	{
		HullTriangle *ht = edge->GetStartElement();
		if (!((ht->plane ^ position) > kHullEpsilon))
		{
			int32 i1 = edge->edgeIndex[0];
			int32 i2 = IncMod<3>(i1);

			HullTriangle *newTriangle = new HullTriangle(ht->vertex[i2], ht->vertex[i1], newVertex);
			triangleList->Append(newTriangle);
			new HullEdge(ht, newTriangle, i1, 0);
		}

		edge = edge->GetNextIncomingEdge();
	}

	delete baseTriangle;
}

void Math::ConnectHullTriangles(Graph<HullTriangle, HullEdge> *hullGraph, List<HullTriangle> *triangleList)
{
	HullTriangle *triangle = triangleList->First();
	while (triangle)
	{
		hullGraph->AddElement(triangle);

		const HullVertex *hv = triangle->vertex[0];
		HullTriangle *ht = triangleList->First();
		do
		{
			if (ht != triangle)
			{
				if (ht->vertex[1] == hv)
				{
					new HullEdge(ht, triangle, 1, 2);
					break;
				}
			}

			ht = ht->Next();
		} while (ht);

		triangle = triangle->Next();
	}
}

void Math::RemoveInteriorHullVertices(Graph<HullTriangle, HullEdge> *hullGraph, List<HullVertex> *vertexList)
{
	HullVertex *vertex = vertexList->First();
	while (vertex)
	{
		HullVertex *nextVertex = vertex->Next();

		const HullTriangle *triangle = hullGraph->GetFirstElement();
		do
		{
			if ((triangle->plane ^ vertex->position) > kHullEpsilon)
			{
				goto next;
			}

			triangle = triangle->GetNextElement();
		} while (triangle);

		vertexList->Remove(vertex);

		next:
		vertex = nextVertex;
	}
}

int32 Math::ComputeConvexHull(int32 vertexCount, const Point3D *vertex, const Box3D& boundingBox, unsigned_int16 *hullIndex)
{
	if (vertexCount > 4)
	{
		machine		i1, i2;

		Buffer buffer(vertexCount * (sizeof(HullVertex) + 1));
		HullVertex *hullVertex = buffer.GetPtr<HullVertex>();

		float dx = boundingBox.max.x - boundingBox.min.x;
		float dy = boundingBox.max.y - boundingBox.min.y;
		float dz = boundingBox.max.z - boundingBox.min.z;

		float sx = (Fabs(dx) > K::min_float) ? 2.0F / dx : 1.0F;
		float sy = (Fabs(dy) > K::min_float) ? 2.0F / dy : 1.0F;
		float sz = (Fabs(dz) > K::min_float) ? 2.0F / dz : 1.0F;

		Vector3D scale(sx, sy, sz);
		Vector3D center = (boundingBox.min + boundingBox.max) * 0.5F;

		for (machine a = 0; a < vertexCount; a++)
		{
			new(&hullVertex[a]) HullVertex((vertex[a] & scale) - center, a);
		}

		FindFirstHullEdge(vertexCount, hullVertex, &i1, &i2);

		const Point3D& v1 = hullVertex[i1].position;
		const Point3D& v2 = hullVertex[i2].position;
		Vector3D tangent = Normalize(v2 - v1);

		machine i3 = -1;
		float maxDistance = kHullEpsilon;

		for (machine a = 0; a < vertexCount; a++)
		{
			if ((a != i1) && (a != i2))
			{
				Vector3D dv = hullVertex[a].position - v1;
				float distance = dv * tangent;
				distance = dv * dv - distance * distance;

				if (distance > maxDistance)
				{
					i3 = a;
					maxDistance = distance;
				}
			}
		}

		if (i3 >= 0)
		{
			Graph<HullTriangle, HullEdge>	hullGraph;
			List<HullVertex>				vertexList;
			List<HullTriangle>				inactiveList;

			HullTriangle *t1 = new HullTriangle(&hullVertex[i1], &hullVertex[i2], &hullVertex[i3]);
			hullGraph.AddElement(t1);

			machine i4 = -1;
			maxDistance = 0.0F;
			float maxAbsDistance = kHullEpsilon;

			for (machine a = 0; a < vertexCount; a++)
			{
				if ((a != i1) && (a != i2) && (a != i3))
				{
					HullVertex *hv = &hullVertex[a];
					vertexList.Append(hv);

					float distance = t1->plane ^ hv->position;
					float absDistance = Fabs(distance);
					if (absDistance > maxAbsDistance)
					{
						i4 = a;
						maxDistance = distance;
						maxAbsDistance = absDistance;
					}
				}
			}

			if (i4 >= 0)
			{
				HullVertex *apex = &hullVertex[i4];
				vertexList.Remove(apex);

				if (maxDistance > 0.0F)
				{
					t1->Negate();
				}

				HullTriangle *t2 = new HullTriangle(t1->vertex[0], t1->vertex[2], apex);
				hullGraph.AddElement(t2);

				HullTriangle *t3 = new HullTriangle(t1->vertex[2], t1->vertex[1], apex);
				hullGraph.AddElement(t3);

				HullTriangle *t4 = new HullTriangle(t1->vertex[1], t1->vertex[0], apex);
				hullGraph.AddElement(t4);

				new HullEdge(t1, t2, 2, 0);
				new HullEdge(t1, t3, 1, 0);
				new HullEdge(t1, t4, 0, 0);

				new HullEdge(t2, t3, 1, 2);
				new HullEdge(t3, t4, 1, 2);
				new HullEdge(t4, t2, 1, 2);

				RemoveInteriorHullVertices(&hullGraph, &vertexList);

				bool *hullFlag = reinterpret_cast<bool *>(hullVertex + vertexCount);
				MemoryMgr::ClearMemory(hullFlag, vertexCount);

				for (;;)
				{
					HullTriangle *ht = hullGraph.GetFirstElement();
					if (!ht)
					{
						break;
					}

					HullVertex *maxVertex = nullptr;
					maxDistance = kHullEpsilon;

					HullVertex *hv = vertexList.First();
					while (hv)
					{
						float distance = ht->plane ^ hv->position;
						if (distance > maxDistance)
						{
							maxDistance = distance;
							maxVertex = hv;
						}

						hv = hv->Next();
					}

					if (maxVertex)
					{
						List<HullTriangle>	triangleList;

						vertexList.Remove(maxVertex);
						const Point3D& position = maxVertex->position;
						AugmentConvexHull(&triangleList, ht, maxVertex, position);

						ht = hullGraph.GetFirstElement();
						while (ht)
						{
							HullTriangle *next = ht->GetNextElement();

							if ((ht->plane ^ position) > kHullEpsilon)
							{
								AugmentConvexHull(&triangleList, ht, maxVertex, position);
							}

							ht = next;
						}

						ConnectHullTriangles(&hullGraph, &triangleList);
						triangleList.RemoveAll();

						RemoveInteriorHullVertices(&hullGraph, &vertexList);
					}
					else
					{
						hullFlag[ht->vertex[0]->vertexIndex] = true;
						hullFlag[ht->vertex[1]->vertexIndex] = true;
						hullFlag[ht->vertex[2]->vertexIndex] = true;

						hullGraph.DetachElement(ht);
						inactiveList.Append(ht);
					}
				}

				int32 count = 0;
				for (machine a = 0; a < vertexCount; a++)
				{
					if (hullFlag[a])
					{
						hullIndex[count] = (unsigned_int16) hullVertex[a].vertexIndex;
						count++;
					}
				}

				vertexList.RemoveAll();
				return (count);
			}

			vertexList.RemoveAll();
		}
	}

	for (machine a = 0; a < vertexCount; a++)
	{
		hullIndex[a] = (unsigned_int16) a;
	}

	return (vertexCount);
}

// ZYUQURM
