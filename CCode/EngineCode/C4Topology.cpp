 

#include "C4Topology.h"
#include "C4Mesh.h"


using namespace C4;


namespace C4
{
	template <> Heap Memory<TopoMesh>::heap("TopoMesh");
	template class Memory<TopoMesh>;
}


TopoVertex::TopoVertex(unsigned_int32 index)
{
	vertexIndex = index;
}

TopoVertex::~TopoVertex()
{
}

bool TopoVertex::FindCollinearBoundaryEdges(const Point3D *vertexArray, TopoEdge *(& result)[2]) const
{
	TopoEdge *edge1 = GetFirstIncomingEdge();
	while (edge1)
	{
		TopoVertex *vertex1 = edge1->GetStartElement();
		if (!FindOutgoingEdge(vertex1))
		{
			TopoEdge *edge2 = GetFirstOutgoingEdge();
			while (edge2)
			{
				TopoVertex *vertex2 = edge2->GetFinishElement();
				if (!FindIncomingEdge(vertex2))
				{
					const Point3D& center = vertexArray[vertexIndex];
					const Point3D& p1 = vertexArray[vertex1->GetIndex()];
					const Point3D& p2 = vertexArray[vertex2->GetIndex()];

					Vector3D dp1 = center - p1;
					Vector3D dp2 = p2 - center;
					float m1 = SquaredMag(dp1);
					float m2 = SquaredMag(dp2);

					if (dp1 * dp2 * InverseSqrt(m1) * InverseSqrt(m2) > kCollinearEdgeEpsilon)
					{
						edge1->SetCollapseCost(m1);
						edge2->SetCollapseCost(m2);
						result[0] = edge1;
						result[1] = edge2;
						return (true);
					}

					break;
				}

				edge2 = edge2->GetNextOutgoingEdge();
			}
		}

		edge1 = edge1->GetNextIncomingEdge();
	}

	return (false);
}


TopoEdge::TopoEdge(TopoFace *face, int32 index1, int32 index2) : GraphEdge<TopoVertex, TopoEdge>(face->GetVertex(index1), face->GetVertex(index2))
{
	topoFace = face;
	edgeIndex = index1;
	face->topoEdge[index1] = this;
}

TopoEdge::~TopoEdge()
{
}

bool TopoEdge::CalculateCollapseCost(float collapseThreshold, const Point3D *vertexArray)
{
	const TopoVertex *vertex1 = GetStartElement();
	const TopoVertex *vertex2 = GetFinishElement();

	if (vertex1->FindNextOutgoingEdge(vertex2, this))
	{
		return (false);
	}

	const TopoEdge *opposingEdge = vertex1->FindIncomingEdge(vertex2);
	if (opposingEdge)
	{
		if (vertex1->FindNextIncomingEdge(vertex2, opposingEdge))
		{
			return (false);
		}

		const TopoFace *face1 = topoFace;
		const TopoFace *face2 = opposingEdge->GetFace();
 
		Vector3D normal1 = face1->CalculateNormal(vertexArray);
		Vector3D normal2 = face2->CalculateNormal(vertexArray);
		Vector3D averageNormal = normal1 + normal2; 

		const TopoEdge *edge = vertex1->GetFirstIncomingEdge(); 
		while (edge)
		{
			if (!vertex1->FindOutgoingEdge(edge->GetStartElement())) 
			{
				return (false); 
			} 

			const TopoFace *face = edge->GetFace();
			if ((face != face1) && (face != face2))
			{ 
				averageNormal += face->CalculateNormal(vertexArray);
			}

			edge = edge->GetNextIncomingEdge();
		}

		normal1.Normalize();
		normal2.Normalize();

		const Point3D& p1 = vertexArray[vertex1->GetIndex()];
		const Point3D& p2 = vertexArray[vertex2->GetIndex()];
		Vector3D dp = p2 - p1;

		Antivector4D plane(Normalize(averageNormal % dp), p2);
		float d = 1.0F;

		edge = vertex1->GetFirstIncomingEdge();
		while (edge)
		{
			const TopoFace *face = edge->GetFace();
			if ((face != face1) && (face != face2))
			{
				Vector3D normal = face->CalculateNormal(vertexArray);
				bool positive = false;
				bool negative = false;

				for (machine a = 0; a < 3; a++)
				{
					const TopoVertex *vertex = face->GetVertex(a);
					if (vertex != vertex1)
					{
						float e = plane ^ vertexArray[vertex->GetIndex()];
						if (e > kCollapsePlaneEpsilon)
						{
							positive = true;
						}
						else if (e < -kCollapsePlaneEpsilon)
						{
							negative = true;
						}
					}
				}

				int32 i1 = face->GetVertexIndex(vertex1);
				int32 i2 = IncMod<3>(i1);
				int32 i3 = IncMod<3>(i2);

				const Point3D& q0 = vertexArray[vertex2->GetIndex()];
				const Point3D& q1 = vertexArray[face->GetVertex(i2)->GetIndex()];
				const Point3D& q2 = vertexArray[face->GetVertex(i3)->GetIndex()];

				if (positive)
				{
					float oldArea = normal1 * normal;
					d = Fmin(d, oldArea * InverseMag(normal));

					float newArea = normal1 * ((q1 - q0) % (q2 - q0));
					if (newArea / oldArea < 0.125F)
					{
						return (false);
					}
				}

				if (negative)
				{
					float oldArea = normal2 * normal;
					d = Fmin(d, oldArea * InverseMag(normal));

					float newArea = normal2 * ((q1 - q0) % (q2 - q0));
					if (newArea / oldArea < 0.125F)
					{
						return (false);
					}
				}
			}

			edge = edge->GetNextIncomingEdge();
		}

		if (d > 0.5F)
		{
			d = 2.0F - d;
			float m = Magnitude(dp) * (d * d);
			if (m < collapseThreshold)
			{
				collapseCost = m * (float) vertex1->GetOutgoingEdgeCount();
				return (true);
			}
		}
	}

	return (false);
}


TopoFace::TopoFace(TopoVertex *v1, TopoVertex *v2, TopoVertex *v3)
{
	topoVertex[0] = v1;
	topoVertex[1] = v2;
	topoVertex[2] = v3;
}

TopoFace::~TopoFace()
{
	for (machine a = 0; a < 3; a++)
	{
		delete topoEdge[a];
	}
}

int32 TopoFace::GetVertexIndex(const TopoVertex *vertex) const
{
	for (machine a = 0; a < 3; a++)
	{
		if (topoVertex[a] == vertex)
		{
			return (a);
		}
	}

	return (-1);
}

Vector3D TopoFace::CalculateNormal(const Point3D *vertexArray) const
{
	const Point3D& p0 = vertexArray[topoVertex[0]->GetIndex()];
	const Point3D& p1 = vertexArray[topoVertex[1]->GetIndex()];
	const Point3D& p2 = vertexArray[topoVertex[2]->GetIndex()];

	return ((p1 - p0) % (p2 - p0));
}


TopoMesh::TopoMesh(const Mesh *mesh, int32 primitiveCount)
{
	geometryMesh = mesh;

	int32 vertexCount = mesh->GetVertexCount();
	TopoVertex **vertexTable = new TopoVertex *[vertexCount];

	for (machine a = 0; a < vertexCount; a++)
	{
		TopoVertex *vertex = new TopoVertex(a);
		vertexTable[a] = vertex;
		meshGraph.AddElement(vertex);
	}

	if (primitiveCount < 0)
	{
		primitiveCount = mesh->GetPrimitiveCount();
	}

	const Triangle *triangle = mesh->GetArray<Triangle>(kArrayPrimitive);
	for (machine a = 0; a < primitiveCount; a++)
	{
		TopoVertex *v1 = vertexTable[triangle->index[0]];
		TopoVertex *v2 = vertexTable[triangle->index[1]];
		TopoVertex *v3 = vertexTable[triangle->index[2]];

		TopoFace *face = new TopoFace(v1, v2, v3);
		faceList.Append(face);

		new TopoEdge(face, 0, 1);
		new TopoEdge(face, 1, 2);
		new TopoEdge(face, 2, 0);

		triangle++;
	}

	delete[] vertexTable;
}

TopoMesh::~TopoMesh()
{
}

void TopoMesh::InsertCollapseEdge(TopoEdge *collapseEdge, List<TopoEdge> *edgeList)
{
	TopoEdge *edge = edgeList->First();
	if (edge)
	{
		float cost = collapseEdge->GetCollapseCost();
		do
		{
			if (edge->GetCollapseCost() > cost)
			{
				edgeList->InsertBefore(collapseEdge, edge);
				return;
			}

			edge = edge->Next();
		} while (edge);
	}

	edgeList->Append(collapseEdge);
}

void TopoMesh::CollapseEdge(TopoVertex *collapseVertex, TopoVertex *keepVertex, TopoFace *collapseFace)
{
	TopoEdge *edge = collapseVertex->GetFirstOutgoingEdge();
	while (edge)
	{
		TopoFace *face = edge->GetFace();
		if (face != collapseFace)
		{
			int32 index1 = face->GetVertexIndex(collapseVertex);
			int32 index2 = IncMod<3>(index1);

			face->SetVertex(index1, keepVertex);
			new TopoEdge(face, index1, index2);
		}

		edge = edge->GetNextOutgoingEdge();
	}

	edge = collapseVertex->GetFirstIncomingEdge();
	while (edge)
	{
		TopoFace *face = edge->GetFace();
		if (face != collapseFace)
		{
			int32 index2 = face->GetVertexIndex(collapseVertex);
			if (index2 < 0)
			{
				index2 = face->GetVertexIndex(keepVertex);
			}

			int32 index1 = DecMod<3>(index2);

			face->SetVertex(index2, keepVertex);
			new TopoEdge(face, index1, index2);
		}

		edge = edge->GetNextIncomingEdge();
	}

	delete collapseFace;
	delete collapseVertex;
}

void TopoMesh::CollapseEdge(TopoVertex *collapseVertex, TopoVertex *keepVertex, TopoFace *collapseFace1, TopoFace *collapseFace2, List<TopoEdge> *edgeList)
{
	int32 count = 0;
	TopoEdge *edge = collapseVertex->GetFirstOutgoingEdge();
	while (edge)
	{
		TopoFace *face = edge->GetFace();
		if ((face != collapseFace1) && (face != collapseFace2))
		{
			int32 index1 = face->GetVertexIndex(collapseVertex);
			int32 index2 = IncMod<3>(index1);

			face->SetVertex(index1, keepVertex);
			new TopoEdge(face, index1, index2);
		}

		count++;
		edge = edge->GetNextOutgoingEdge();
	}

	edge = collapseVertex->GetFirstIncomingEdge();
	while (edge)
	{
		TopoFace *face = edge->GetFace();
		if ((face != collapseFace1) && (face != collapseFace2))
		{
			int32 index2 = face->GetVertexIndex(collapseVertex);
			if (index2 < 0)
			{
				index2 = face->GetVertexIndex(keepVertex);
			}

			int32 index1 = DecMod<3>(index2);

			face->SetVertex(index2, keepVertex);
			new TopoEdge(face, index1, index2);
		}

		edge = edge->GetNextIncomingEdge();
	}

	delete collapseFace1;
	delete collapseFace2;
	delete collapseVertex;

	edge = keepVertex->GetFirstOutgoingEdge();
	while (edge)
	{
		edgeList->Append(edge);

		int32 i2 = IncMod<3>(edge->GetIndex());
		int32 i3 = IncMod<3>(i2);

		const TopoFace *face = edge->GetFace();
		TopoEdge *e2 = face->GetEdge(i2);
		TopoEdge *e3 = face->GetEdge(i3);
		edgeList->Append(e2);
		edgeList->Append(e3);

		TopoEdge *e4 = e2->GetStartElement()->FindIncomingEdge(e2->GetFinishElement());
		if (e4)
		{
			edgeList->Append(e4);
		}

		edge = edge->GetNextOutgoingEdge();
	}
}

bool TopoMesh::SimplifyBoundaryEdges(const Point3D *vertexArray)
{
	bool collapse = false;

	TopoVertex *vertex = meshGraph.GetFirstElement();
	while (vertex)
	{
		TopoEdge	*collinearEdge[2];

		TopoVertex *nextVertex = vertex->GetNextElement();

		if (vertex->FindCollinearBoundaryEdges(vertexArray, collinearEdge))
		{
			int32 edgeCount = vertex->GetOutgoingEdgeCount();
			if (edgeCount == 2)
			{
				if (collinearEdge[0]->GetCollapseCost() < collinearEdge[1]->GetCollapseCost())
				{
					CollapseEdge(collinearEdge[0]->GetFinishElement(), collinearEdge[0]->GetStartElement(), collinearEdge[0]->GetFace());
				}
				else
				{
					CollapseEdge(collinearEdge[1]->GetStartElement(), collinearEdge[1]->GetFinishElement(), collinearEdge[1]->GetFace());
				}

				collapse = true;
			}
			else if (edgeCount > 2)
			{
				TopoFace *collapseFace0 = collinearEdge[0]->GetFace();
				TopoFace *collapseFace1 = collinearEdge[1]->GetFace();

				int32 index0 = collapseFace0->GetVertexIndex(vertex);
				int32 index1 = collapseFace1->GetVertexIndex(vertex);

				const TopoEdge *interiorEdge0 = vertex->FindIncomingEdge(collapseFace0->GetVertex(IncMod<3>(index0)));
				const TopoEdge *interiorEdge1 = vertex->FindOutgoingEdge(collapseFace1->GetVertex(DecMod<3>(index1)));
				if ((interiorEdge0) && (interiorEdge1))
				{
					const TopoFace *interiorFace0 = interiorEdge0->GetFace();
					const TopoFace *interiorFace1 = interiorEdge1->GetFace();

					bool collapse0 = (Normalize(collapseFace0->CalculateNormal(vertexArray)) * Normalize(interiorFace0->CalculateNormal(vertexArray)) > kCoplanarFaceEpsilon);
					bool collapse1 = (Normalize(collapseFace1->CalculateNormal(vertexArray)) * Normalize(interiorFace1->CalculateNormal(vertexArray)) > kCoplanarFaceEpsilon);
					if ((collapse0 & collapse1) && (collinearEdge[0]->GetCollapseCost() > collinearEdge[1]->GetCollapseCost()))
					{
						collapse0 = false;
					}

					if (collapse0)
					{
						CollapseEdge(collinearEdge[0]->GetFinishElement(), collinearEdge[0]->GetStartElement(), collapseFace0);
						collapse = true;
					}
					else if (collapse1)
					{
						CollapseEdge(collinearEdge[1]->GetStartElement(), collinearEdge[1]->GetFinishElement(), collapseFace1);
						collapse = true;
					}
				}
			}
		}

		vertex = nextVertex;
	}

	return (collapse);
}

void TopoMesh::OptimizeMesh(const Point3D *vertexArray, float collapseThreshold)
{
	List<TopoEdge>	edgeList;
	List<TopoEdge>	newList;

	const TopoFace *face = faceList.First();
	while (face)
	{
		for (machine a = 0; a < 3; a++)
		{
			TopoEdge *edge = face->GetEdge(a);
			if (edge->CalculateCollapseCost(collapseThreshold, vertexArray))
			{
				InsertCollapseEdge(edge, &edgeList);
			}
		}

		face = face->Next();
	}

	for (machine a = 0;; a++)
	{
		TopoEdge *edge = edgeList.First();
		if (!edge)
		{
			break;
		}

		TopoVertex *collapseVertex = edge->GetStartElement();
		TopoVertex *keepVertex = edge->GetFinishElement();
		CollapseEdge(collapseVertex, keepVertex, edge->GetFace(), collapseVertex->FindIncomingEdge(keepVertex)->GetFace(), &newList);

		for (;;)
		{
			TopoEdge *newEdge = newList.First();
			if (!newEdge)
			{
				break;
			}

			if (newEdge->CalculateCollapseCost(collapseThreshold, vertexArray))
			{
				InsertCollapseEdge(newEdge, &edgeList);
			}
			else
			{
				newList.Remove(newEdge);
			}
		}
	}

	edgeList.RemoveAll();
}

void TopoMesh::CompactMesh(int32 *vertexCount, int32 *faceCount, unsigned_int32 *remapTable)
{
	int32 count = 0;

	TopoVertex *vertex = meshGraph.GetFirstElement();
	while (vertex)
	{
		remapTable[count] = vertex->GetIndex();
		vertex->SetIndex(count);
		count++;

		vertex = vertex->GetNextElement();
	}

	*vertexCount = count;
	*faceCount = faceList.GetElementCount();
}

// ZYUQURM
