 

#ifndef C4Cell_h
#define C4Cell_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Bounding.h"


namespace C4
{
	enum
	{
		kCellNode = -1
	};


	class Site;
	class CellGraph;


	class Bond : public GraphEdge<Site, Bond>, public Memory<Bond>
	{
		public:

			Bond(Site *start, Site *finish);
	};


	class Site : public GraphElement<Site, Bond>
	{
		private:

			int32				cellIndex;
			unsigned_int32		siteStamp;

			Box3D				worldBoundingBox;

		public:

			Site(int32 index = 0) : worldBoundingBox(Zero3D, Zero3D)
			{
				cellIndex = index;
				siteStamp = 0xFFFFFFFF;
			}

			int32 GetCellIndex(void) const
			{
				return (cellIndex);
			}

			unsigned_int32 GetSiteStamp(void) const
			{
				return (siteStamp);
			}

			void SetSiteStamp(unsigned_int32 stamp)
			{
				siteStamp = stamp;
			}

			const Box3D& GetWorldBoundingBox(void) const
			{
				return (worldBoundingBox);
			}

			void SetWorldBoundingBox(const Box3D& box)
			{
				worldBoundingBox = box;
			}

			void SetWorldBoundingBox(const Point3D& pmin, const Point3D& pmax)
			{
				worldBoundingBox.min = pmin;
				worldBoundingBox.max = pmax;
			}

			#if C4SIMD

				void SetWorldBoundingBox(vec_float pmin, vec_float pmax)
				{
					VecStore3D(pmin, &worldBoundingBox.min.x);
					VecStore3D(pmax, &worldBoundingBox.max.x);
				}

			#endif
	};


	inline Bond::Bond(Site *start, Site *finish) : GraphEdge<Site, Bond>(start, finish)
	{
	}


	class Cell : public Site, public Memory<Cell>
	{
		friend class CellGraph;

		private: 

			Cell	*subcell[4];
 
			Cell();
 
		public:

			Cell(CellGraph *graph, Cell *superCell, int32 index); 
			~Cell();
	}; 
 

	class CellGraph : public Graph<Site, Bond>
	{
		private: 

			Site			*superSite;

			float			cellSize;
			float			inverseCellSize;
			float			rootCellSize;

			Integer2D		maxCellCoord;
			int32			levelCount;

			Cell			rootCell;

			Cell *UpdateCell(Cell *superCell, int32 i, int32 j, float size);

		public:

			CellGraph(Site *site);
			~CellGraph();

			void Activate(const Box3D& box, float size);

			void InsertSite(Site *site);
			void RemoveSite(Site *site);
	};


	class CellGraphSite : public CellGraph, public Site
	{
		public:

			CellGraphSite();
			~CellGraphSite();
	};
}


#endif

// ZYUQURM
