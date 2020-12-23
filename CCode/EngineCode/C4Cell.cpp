 

#include "C4Cell.h"


using namespace C4;


namespace C4
{
	const float kCellFraction = 0.75F;


	template <> Heap Memory<Bond>::heap("Bond", MemoryMgr::CalculatePoolSize(64, sizeof(Bond)));
	template class Memory<Bond>;

	template <> Heap Memory<Cell>::heap("Cell", MemoryMgr::CalculatePoolSize(256, sizeof(Cell)));
	template class Memory<Cell>;
}


C4::Cell::Cell()
{
	subcell[0] = nullptr;
	subcell[1] = nullptr;
	subcell[2] = nullptr;
	subcell[3] = nullptr;
}

C4::Cell::Cell(CellGraph *graph, Cell *superCell, int32 index) : Site(index)
{
	superCell->subcell[index] = this;
	new Bond(superCell, this);
	graph->AddElement(this);

	subcell[0] = nullptr;
	subcell[1] = nullptr;
	subcell[2] = nullptr;
	subcell[3] = nullptr;
}

C4::Cell::~Cell()
{
	const Bond *bond = GetFirstIncomingEdge();
	if (bond)
	{
		static_cast<Cell *>(bond->GetStartElement())->subcell[GetCellIndex()] = nullptr;
	}
}


CellGraph::CellGraph(Site *site)
{
	superSite = site;
}

CellGraph::~CellGraph()
{
}

void CellGraph::Activate(const Box3D& box, float size)
{
	cellSize = size;
	inverseCellSize = 1.0F / size;

	int32 width = (int32) Ceil((box.max.x - box.min.x) * inverseCellSize);
	int32 height = (int32) Ceil((box.max.y - box.min.y) * inverseCellSize);
	maxCellCoord.Set(MaxZero(width - 1), MaxZero(height - 1));

	int32 logWidth = 32 - Cntlz(width - 1);
	int32 logHeight = 32 - Cntlz(height - 1);
	int32 count = Max(Max(logWidth, logHeight), 1);
	levelCount = count;

	rootCellSize = (float) (1 << (count - 1)) * size;
	rootCell.SetWorldBoundingBox(box.min, Point3D(box.min.x + rootCellSize * 2.0F, box.min.y + rootCellSize * 2.0F, box.max.z));

	new Bond(superSite, &rootCell);
}

C4::Cell *CellGraph::UpdateCell(Cell *superCell, int32 i, int32 j, float size)
{
	int32 index = (j & 1) * 2 + (i & 1);
	Cell *cell = superCell->subcell[index];
	if (!cell)
	{
		cell = new Cell(this, superCell, index);
		const Point3D& rootBoxMin = rootCell.GetWorldBoundingBox().min;
		Point3D p(rootBoxMin.x + (float) i * size, rootBoxMin.y + (float) j * size, rootBoxMin.z);
		cell->SetWorldBoundingBox(p, Point3D(p.x + size, p.y + size, rootCell.GetWorldBoundingBox().max.z));
	}

	return (cell);
}

void CellGraph::InsertSite(Site *site)
{
	if (rootCell.GetFirstIncomingEdge())
	{
		const Box3D& siteBox = site->GetWorldBoundingBox();
		Vector2D size = siteBox.max.GetPoint2D() - siteBox.min.GetPoint2D();

		float siteSize = Fmax(size.x, size.y); 
		float levelCellSize = rootCellSize;
		if (siteSize < levelCellSize * kCellFraction)
		{ 
			Vector2D offset = siteBox.min.GetPoint2D() - rootCell.GetWorldBoundingBox().min.GetPoint2D();
 
			int32 x1 = (int32) (offset.x * inverseCellSize);
			int32 y1 = (int32) (offset.y * inverseCellSize);
			int32 x2 = (int32) ((offset.x + size.x) * inverseCellSize); 
			int32 y2 = (int32) ((offset.y + size.y) * inverseCellSize);
 
			if (((unsigned_int32) x1 <= (unsigned_int32) maxCellCoord.x) && ((unsigned_int32) x2 <= (unsigned_int32) maxCellCoord.x) && ((unsigned_int32) y1 <= (unsigned_int32) maxCellCoord.y) && ((unsigned_int32) y2 <= (unsigned_int32) maxCellCoord.y)) 
			{
				int32	i1, i2, j1, j2;
				Cell	*superCell[4];
				Cell	*subcell[4]; 

				superCell[0] = &rootCell;
				superCell[1] = &rootCell;
				superCell[2] = &rootCell;
				superCell[3] = &rootCell;

				for (machine level = levelCount - 1; level >= 0; level--)
				{
					i1 = x1 >> level;
					i2 = x2 >> level;
					j1 = y1 >> level;
					j2 = y2 >> level;

					Cell *cell = UpdateCell(superCell[0], i1, j1, levelCellSize);
					subcell[0] = cell;

					if (i2 == i1)
					{
						subcell[1] = cell;
					}
					else
					{
						subcell[1] = UpdateCell(superCell[1], i2, j1, levelCellSize);
					}

					if (j2 == j1)
					{
						subcell[2] = subcell[0];
						subcell[3] = subcell[1];
					}
					else
					{
						cell = UpdateCell(superCell[2], i1, j2, levelCellSize);
						subcell[2] = cell;

						if (i2 == i1)
						{
							subcell[3] = cell;
						}
						else
						{
							subcell[3] = UpdateCell(superCell[3], i2, j2, levelCellSize);
						}
					}

					levelCellSize *= 0.5F;
					if (siteSize > levelCellSize * kCellFraction)
					{
						break;
					}

					superCell[0] = subcell[0];
					superCell[1] = subcell[1];
					superCell[2] = subcell[2];
					superCell[3] = subcell[3];
				}

				new Bond(subcell[0], site);
				if (i2 != i1)
				{
					new Bond(subcell[1], site);
				}

				if (j2 != j1)
				{
					new Bond(subcell[2], site);
					if (i2 != i1)
					{
						new Bond(subcell[3], site);
					}
				}

				return;
			}
		}
	}

	new Bond(superSite, site);
}

void CellGraph::RemoveSite(Site *site)
{
	for (;;)
	{
		Bond *bond = site->GetFirstIncomingEdge();
		if (!bond)
		{
			break;
		}

		Site *predecessor = bond->GetStartElement();
		delete bond;

		if (predecessor != superSite)
		{
			while (predecessor != &rootCell)
			{
				if (predecessor->GetFirstOutgoingEdge())
				{
					break;
				}

				bond = predecessor->GetFirstIncomingEdge();
				Site *cell = bond->GetStartElement();
				delete predecessor;
				predecessor = cell;
			}
		}
	}
}


CellGraphSite::CellGraphSite() : CellGraph(this)
{
}

CellGraphSite::~CellGraphSite()
{
}

// ZYUQURM
