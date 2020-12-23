 

#ifndef C4Topology_h
#define C4Topology_h


#include "C4Types.h"


namespace C4
{
	const float kCollinearEdgeEpsilon = 0.9999F;
	const float kCoplanarFaceEpsilon = 0.9999F;
	const float kCollapsePlaneEpsilon = 0.001F;


	class TopoEdge;
	class TopoFace;
	class TopoMesh;
	class Mesh;


	class TopoVertex : public GraphElement<TopoVertex, TopoEdge>, public Memory<TopoMesh>
	{
		private:

			unsigned_int32	vertexIndex;

		public:

			TopoVertex(unsigned_int32 index);
			~TopoVertex();

			unsigned_int32 GetIndex(void) const
			{
				return (vertexIndex);
			}

			void SetIndex(unsigned_int32 index)
			{
				vertexIndex = index;
			}

			bool FindCollinearBoundaryEdges(const Point3D *vertexArray, TopoEdge *(& result)[2]) const;
	};


	class TopoEdge : public GraphEdge<TopoVertex, TopoEdge>, public ListElement<TopoEdge>, public Memory<TopoMesh>
	{
		private:

			TopoFace		*topoFace;
			int32			edgeIndex;
			float			collapseCost;

		public:

			TopoEdge(TopoFace *face, int32 index1, int32 index2);
			~TopoEdge();

			using ListElement<TopoEdge>::Previous;
			using ListElement<TopoEdge>::Next;

			TopoFace *GetFace(void) const
			{
				return (topoFace);
			}

			int32 GetIndex(void) const
			{
				return (edgeIndex);
			}

			float GetCollapseCost(void) const
			{
				return (collapseCost);
			}

			void SetCollapseCost(float cost)
			{
				collapseCost = cost;
			}

			bool CalculateCollapseCost(float collapseThreshold, const Point3D *vertexArray);
	};


	class TopoFace : public ListElement<TopoFace>, public Memory<TopoMesh>
	{
		friend class TopoEdge;

		private:

			TopoVertex		*topoVertex[3];
			TopoEdge		*topoEdge[3];

		public:

			TopoFace(TopoVertex *v1, TopoVertex *v2, TopoVertex *v3);
			~TopoFace();

			TopoVertex *GetVertex(int32 index) const
			{ 
				return (topoVertex[index]);
			}
 
			void SetVertex(int32 index, TopoVertex *vertex)
			{ 
				topoVertex[index] = vertex;
			}
 
			TopoEdge *GetEdge(int32 index) const
			{ 
				return (topoEdge[index]); 
			}

			int32 GetVertexIndex(const TopoVertex *vertex) const;
			Vector3D CalculateNormal(const Point3D *vertexArray) const; 
	};


	class TopoMesh : public Memory<TopoMesh>
	{
		private:

			const Mesh						*geometryMesh;

			Graph<TopoVertex, TopoEdge>		meshGraph;
			List<TopoFace>					faceList;

			static void InsertCollapseEdge(TopoEdge *collapseEdge, List<TopoEdge> *edgeList);
			static void CollapseEdge(TopoVertex *collapseVertex, TopoVertex *keepVertex, TopoFace *collapseFace);
			void CollapseEdge(TopoVertex *collapseVertex, TopoVertex *keepVertex, TopoFace *collapseFace1, TopoFace *collapseFace2, List<TopoEdge> *edgeList);

		public:

			TopoMesh(const Mesh *mesh, int32 primitiveCount = -1);
			~TopoMesh();

			TopoFace *GetFirstFace(void) const
			{
				return (faceList.First());
			}

			bool SimplifyBoundaryEdges(const Point3D *vertexArray);
			void OptimizeMesh(const Point3D *vertexArray, float collapseThreshold);

			void CompactMesh(int32 *vertexCount, int32 *faceCount, unsigned_int32 *remapTable);
	};
}


#endif

// ZYUQURM
