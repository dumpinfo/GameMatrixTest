 

#include "C4Shapes.h"
#include "C4Simulation.h"
#include "C4Physics.h"
#include "C4Configuration.h"


using namespace C4;


namespace
{
	enum
	{
		kMaxShapeVertexCount			= 34,
		kMaxShapePolygonVertexCount		= 8
	};
}


const unsigned_int8 BoxShapeObject::boxEdge[12][2] =
{
	{0, 1}, {2, 3}, {0, 2}, {1, 3}, {4, 5}, {6, 7}, {4, 6}, {5, 7}, {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

const unsigned_int8 BoxShapeObject::boxPolygon[30] =
{
	4, 1, 3, 7, 5,
	4, 2, 0, 4, 6,
	4, 3, 2, 6, 7,
	4, 0, 1, 5, 4,
	4, 4, 5, 7, 6,
	4, 2, 3, 1, 0
};

const ShapeMesh BoxShapeObject::boxMesh =
{
	8, 12, 6,
	boxEdge, boxPolygon
};


const unsigned_int8 PyramidShapeObject::pyramidEdge[8][2] =
{
	{0, 1}, {2, 3}, {0, 2}, {1, 3}, {0, 4}, {1, 4}, {2, 4}, {3, 4}
};

const unsigned_int8 PyramidShapeObject::pyramidPolygon[21] =
{
	3, 1, 3, 4,
	3, 2, 0, 4,
	3, 3, 2, 4,
	3, 0, 1, 4,
	4, 2, 3, 1, 0
};

const ShapeMesh PyramidShapeObject::pyramidMesh =
{
	5, 8, 5,
	pyramidEdge, pyramidPolygon
};


const unsigned_int8 CylinderShapeObject::cylinderEdge[24][2] =
{
	{0, 1}, {8, 9}, {0, 8},
	{1, 2}, {9, 10}, {1, 9},
	{2, 3}, {10, 11}, {2, 10},
	{3, 4}, {11, 12}, {3, 11},
	{4, 5}, {12, 13}, {4, 12},
	{5, 6}, {13, 14}, {5, 13},
	{6, 7}, {14, 15}, {6, 14},
	{7, 0}, {15, 8}, {7, 15}
};

const unsigned_int8 CylinderShapeObject::cylinderPolygon[58] =
{
	4, 0, 1, 9, 8,
	4, 1, 2, 10, 9,
	4, 2, 3, 11, 10,
	4, 3, 4, 12, 11,
	4, 4, 5, 13, 12,
	4, 5, 6, 14, 13,
	4, 6, 7, 15, 14,
	4, 7, 0, 8, 15,
	8, 7, 6, 5, 4, 3, 2, 1, 0,
	8, 8, 9, 10, 11, 12, 13, 14, 15
};

const ShapeMesh CylinderShapeObject::cylinderMesh =
{
	16, 24, 10,
	cylinderEdge, cylinderPolygon
};


const unsigned_int8 ConeShapeObject::coneEdge[16][2] =
{
	{0, 1}, {0, 8},
	{1, 2}, {1, 8},
	{2, 3}, {2, 8},
	{3, 4}, {3, 8}, 
	{4, 5}, {4, 8},
	{5, 6}, {5, 8},
	{6, 7}, {6, 8}, 
	{7, 0}, {7, 8}
}; 

const unsigned_int8 ConeShapeObject::conePolygon[41] =
{ 
	3, 0, 1, 8,
	3, 1, 2, 8, 
	3, 2, 3, 8, 
	3, 3, 4, 8,
	3, 4, 5, 8,
	3, 5, 6, 8,
	3, 6, 7, 8, 
	3, 7, 0, 8,
	8, 7, 6, 5, 4, 3, 2, 1, 0
};

const ShapeMesh ConeShapeObject::coneMesh =
{
	9, 16, 9,
	coneEdge, conePolygon
};


const unsigned_int8 SphereShapeObject::sphereEdge[56][2] =
{
	{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 0},
	{8, 9}, {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14}, {14, 15}, {15, 8},
	{16, 17}, {17, 18}, {18, 19}, {19, 20}, {20, 21}, {21, 22}, {22, 23}, {23, 16},
	{24, 8}, {8, 0}, {0, 16}, {16, 25},
	{24, 9}, {9, 1}, {1, 17}, {17, 25},
	{24, 10}, {10, 2}, {2, 18}, {18, 25},
	{24, 11}, {11, 3}, {3, 19}, {19, 25},
	{24, 12}, {12, 4}, {4, 20}, {20, 25},
	{24, 13}, {13, 5}, {5, 21}, {21, 25},
	{24, 14}, {14, 6}, {6, 22}, {22, 25},
	{24, 15}, {15, 7}, {7, 23}, {23, 25}
};

const unsigned_int8 SphereShapeObject::spherePolygon[144] =
{
	4, 0, 1, 9, 8,
	4, 1, 2, 10, 9,
	4, 2, 3, 11, 10,
	4, 3, 4, 12, 11,
	4, 4, 5, 13, 12,
	4, 5, 6, 14, 13,
	4, 6, 7, 15, 14,
	4, 7, 0, 8, 15,
	4, 0, 16, 17, 1,
	4, 1, 17, 18, 2,
	4, 2, 18, 19, 3,
	4, 3, 19, 20, 4,
	4, 4, 20, 21, 5,
	4, 5, 21, 22, 6,
	4, 6, 22, 23, 7,
	4, 7, 23, 16, 0,
	3, 8, 9, 24,
	3, 9, 10, 24,
	3, 10, 11, 24,
	3, 11, 12, 24,
	3, 12, 13, 24,
	3, 13, 14, 24,
	3, 14, 15, 24,
	3, 15, 8, 24,
	3, 16, 25, 17,
	3, 17, 25, 18,
	3, 18, 25, 19,
	3, 19, 25, 20,
	3, 20, 25, 21,
	3, 21, 25, 22,
	3, 22, 25, 23,
	3, 23, 25, 16
};

const ShapeMesh SphereShapeObject::sphereMesh =
{
	26, 56, 32,
	sphereEdge, spherePolygon
};


const unsigned_int8 DomeShapeObject::domeEdge[32][2] =
{
	{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 0},
	{8, 9}, {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14}, {14, 15}, {15, 8},
	{16, 8}, {8, 0},
	{16, 9}, {9, 1},
	{16, 10}, {10, 2},
	{16, 11}, {11, 3},
	{16, 12}, {12, 4},
	{16, 13}, {13, 5},
	{16, 14}, {14, 6},
	{16, 15}, {15, 7}
};

const unsigned_int8 DomeShapeObject::domePolygon[81] =
{
	4, 0, 1, 9, 8,
	4, 1, 2, 10, 9,
	4, 2, 3, 11, 10,
	4, 3, 4, 12, 11,
	4, 4, 5, 13, 12,
	4, 5, 6, 14, 13,
	4, 6, 7, 15, 14,
	4, 7, 0, 8, 15,
	3, 8, 9, 16,
	3, 9, 10, 16,
	3, 10, 11, 16,
	3, 11, 12, 16,
	3, 12, 13, 16,
	3, 13, 14, 16,
	3, 14, 15, 16,
	3, 15, 8, 16,
	8, 7, 6, 5, 4, 3, 2, 1, 0
};

const ShapeMesh DomeShapeObject::domeMesh =
{
	17, 32, 17,
	domeEdge, domePolygon
};


const unsigned_int8 CapsuleShapeObject::capsuleEdge[72][2] =
{
	{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 0},
	{8, 9}, {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14}, {14, 15}, {15, 8},
	{16, 17}, {17, 18}, {18, 19}, {19, 20}, {20, 21}, {21, 22}, {22, 23}, {23, 16},
	{24, 25}, {25, 26}, {26, 27}, {27, 28}, {28, 29}, {29, 30}, {30, 31}, {31, 24},
	{32, 16}, {16, 8}, {8, 0}, {0, 24}, {24, 33},
	{32, 17}, {17, 9}, {9, 1}, {1, 25}, {25, 33},
	{32, 18}, {18, 10}, {10, 2}, {2, 26}, {26, 33},
	{32, 19}, {19, 11}, {11, 3}, {3, 27}, {27, 33},
	{32, 20}, {20, 12}, {12, 4}, {4, 28}, {28, 33},
	{32, 21}, {21, 13}, {13, 5}, {5, 29}, {29, 33},
	{32, 22}, {22, 14}, {14, 6}, {6, 30}, {30, 33},
	{32, 23}, {23, 15}, {15, 7}, {7, 31}, {31, 33}
};

const unsigned_int8 CapsuleShapeObject::capsulePolygon[184] =
{
	4, 0, 1, 9, 8,
	4, 1, 2, 10, 9,
	4, 2, 3, 11, 10,
	4, 3, 4, 12, 11,
	4, 4, 5, 13, 12,
	4, 5, 6, 14, 13,
	4, 6, 7, 15, 14,
	4, 7, 0, 8, 15,
	4, 8, 9, 17, 16,
	4, 9, 10, 18, 17,
	4, 10, 11, 19, 18,
	4, 11, 12, 20, 19,
	4, 12, 13, 21, 20,
	4, 13, 14, 22, 21,
	4, 14, 15, 23, 22,
	4, 15, 8, 16, 23,
	4, 0, 24, 25, 1,
	4, 1, 25, 26, 2,
	4, 2, 26, 27, 3,
	4, 3, 27, 28, 4,
	4, 4, 28, 29, 5,
	4, 5, 29, 30, 6,
	4, 6, 30, 31, 7,
	4, 7, 31, 24, 0,
	3, 16, 17, 32,
	3, 17, 18, 32,
	3, 18, 19, 32,
	3, 19, 20, 32,
	3, 20, 21, 32,
	3, 21, 22, 32,
	3, 22, 23, 32,
	3, 23, 16, 32,
	3, 24, 33, 25,
	3, 25, 33, 26,
	3, 26, 33, 27,
	3, 27, 33, 28,
	3, 28, 33, 29,
	3, 29, 33, 30,
	3, 30, 33, 31,
	3, 31, 33, 24
};

const ShapeMesh CapsuleShapeObject::capsuleMesh =
{
	34, 72, 40,
	capsuleEdge, capsulePolygon
};


const unsigned_int8 TruncatedPyramidShapeObject::truncatedPyramidEdge[12][2] =
{
	{0, 1}, {2, 3}, {0, 2}, {1, 3}, {4, 5}, {6, 7}, {4, 6}, {5, 7}, {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

const unsigned_int8 TruncatedPyramidShapeObject::truncatedPyramidPolygon[30] =
{
	4, 1, 3, 7, 5,
	4, 2, 0, 4, 6,
	4, 3, 2, 6, 7,
	4, 0, 1, 5, 4,
	4, 4, 5, 7, 6,
	4, 2, 3, 1, 0
};

const ShapeMesh TruncatedPyramidShapeObject::truncatedPyramidMesh =
{
	8, 12, 6,
	truncatedPyramidEdge, truncatedPyramidPolygon
};


const unsigned_int8 TruncatedConeShapeObject::truncatedConeEdge[24][2] =
{
	{0, 1}, {8, 9}, {0, 8},
	{1, 2}, {9, 10}, {1, 9},
	{2, 3}, {10, 11}, {2, 10},
	{3, 4}, {11, 12}, {3, 11},
	{4, 5}, {12, 13}, {4, 12},
	{5, 6}, {13, 14}, {5, 13},
	{6, 7}, {14, 15}, {6, 14},
	{7, 0}, {15, 8}, {7, 15}
};

const unsigned_int8 TruncatedConeShapeObject::truncatedConePolygon[58] =
{
	4, 0, 1, 9, 8,
	4, 1, 2, 10, 9,
	4, 2, 3, 11, 10,
	4, 3, 4, 12, 11,
	4, 4, 5, 13, 12,
	4, 5, 6, 14, 13,
	4, 6, 7, 15, 14,
	4, 7, 0, 8, 15,
	8, 7, 6, 5, 4, 3, 2, 1, 0,
	8, 8, 9, 10, 11, 12, 13, 14, 15
};

const ShapeMesh TruncatedConeShapeObject::truncatedConeMesh =
{
	16, 24, 10,
	truncatedConeEdge, truncatedConePolygon
};


const unsigned_int8 TruncatedDomeShapeObject::truncatedDomeEdge[40][2] =
{
	{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 0},
	{8, 9}, {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14}, {14, 15}, {15, 8},
	{16, 17}, {17, 18}, {18, 19}, {19, 20}, {20, 21}, {21, 22}, {22, 23}, {23, 16},
	{0, 8}, {8, 16},
	{1, 9}, {9, 17},
	{2, 10}, {10, 18},
	{3, 11}, {11, 19},
	{4, 12}, {12, 20},
	{5, 13}, {13, 21},
	{6, 14}, {14, 22},
	{7, 15}, {15, 23}
};

const unsigned_int8 TruncatedDomeShapeObject::truncatedDomePolygon[98] =
{
	4, 0, 1, 9, 8,
	4, 1, 2, 10, 9,
	4, 2, 3, 11, 10,
	4, 3, 4, 12, 11,
	4, 4, 5, 13, 12,
	4, 5, 6, 14, 13,
	4, 6, 7, 15, 14,
	4, 7, 0, 8, 15,
	4, 8, 9, 17, 16,
	4, 9, 10, 18, 17,
	4, 10, 11, 19, 18,
	4, 11, 12, 20, 19,
	4, 12, 13, 21, 20,
	4, 13, 14, 22, 21,
	4, 14, 15, 23, 22,
	4, 15, 16, 16, 23,
	8, 7, 6, 5, 4, 3, 2, 1, 0,
	8, 16, 17, 18, 19, 20, 21, 22, 23
};

const ShapeMesh TruncatedDomeShapeObject::truncatedDomeMesh =
{
	24, 40, 18,
	truncatedDomeEdge, truncatedDomePolygon
};


const ConstVector3D Shape::penetrationDirectionTable[kPenetrationDirectionCount] =
{
	{1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {-1.0F, 0.0F, 0.0F}, {0.0F, -1.0F, 0.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, -1.0F},
	{0.70710678F, 0.70710678F, 0.0F}, {-0.70710678F, 0.70710678F, 0.0F}, {-0.70710678F, -0.70710678F, 0.0F}, {0.70710678F, -0.70710678F, 0.0F},
	{0.70710678F, 0.0F, 0.70710678F}, {-0.70710678F, 0.0F, 0.70710678F}, {-0.70710678F, 0.0F, -0.70710678F}, {0.70710678F, 0.0F, -0.70710678F},
	{0.0F, 0.70710678F, 0.70710678F}, {0.0F, -0.70710678F, 0.70710678F}, {0.0F, -0.70710678F, -0.70710678F}, {0.0F, 0.70710678F, -0.70710678F},
	{0.57735F, 0.57735F, 0.57735F}, {-0.57735F, 0.57735F, 0.57735F}, {-0.57735F, -0.57735F, 0.57735F}, {0.57735F, -0.57735F, 0.57735F},
	{0.57735F, 0.57735F, -0.57735F}, {-0.57735F, 0.57735F, -0.57735F}, {-0.57735F, -0.57735F, -0.57735F}, {0.57735F, -0.57735F, -0.57735F}
};


unsigned_int32 (*const Shape::simplexMinFunc[4])(const Point3D *, Point3D *) =
{
	&CalculateZeroSimplexMinimum, &CalculateOneSimplexMinimum, &CalculateTwoSimplexMinimum, &CalculateThreeSimplexMinimum
};


ShapeObject::ShapeObject(ShapeType type, const ShapeMesh *mesh, const Point3D *vertex, Volume *volume) :
		Object(kObjectShape),
		VolumeObject(volume)
{
	shapeType = type;
	shapeMesh = mesh;
	shapeVertex = vertex;

	shapeFlags = 0;
	shapeDensity = 1.0F;
	shapeSubstance = kSubstanceNone;

	shapeVolume = 0.0F;
	shapeShrinkSize = 0.0F;
}

ShapeObject::~ShapeObject()
{
}

ShapeObject *ShapeObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kShapeBox:

			return (new BoxShapeObject);

		case kShapePyramid:

			return (new PyramidShapeObject);

		case kShapeCylinder:

			return (new CylinderShapeObject);

		case kShapeCone:

			return (new ConeShapeObject);

		case kShapeSphere:

			return (new SphereShapeObject);

		case kShapeDome:

			return (new DomeShapeObject);

		case kShapeCapsule:

			return (new CapsuleShapeObject);

		case kShapeTruncatedPyramid:

			return (new TruncatedPyramidShapeObject);

		case kShapeTruncatedCone:

			return (new TruncatedConeShapeObject);

		case kShapeTruncatedDome:

			return (new TruncatedDomeShapeObject);
	}

	return (nullptr);
}

void ShapeObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << shapeType;
}

void ShapeObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << shapeFlags;

	data << ChunkHeader('DENS', 4);
	data << shapeDensity;

	data << ChunkHeader('SBST', 4);
	data << shapeSubstance;

	data << TerminatorChunk;

	PackVolume(data, packFlags);
}

void ShapeObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<ShapeObject>(data, unpackFlags);
	UnpackVolume(data, unpackFlags);
}

bool ShapeObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> shapeFlags;
			return (true);

		case 'DENS':

			data >> shapeDensity;
			return (true);

		case 'SBST':

			data >> shapeSubstance;
			return (true);
	}

	return (false);
}

int32 ShapeObject::GetCategoryCount(void) const
{
	return (1);
}

Type ShapeObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectShape));
		return (kObjectShape);
	}

	return (0);
}

int32 ShapeObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectShape)
	{
		return (4);
	}

	return (0);
}

Setting *ShapeObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectShape)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectShape, 'SHAP'));
			return (new HeadingSetting(kObjectShape, title));
		}

		if (index == 1)
		{
			String<> title = table->GetString(StringID(kObjectShape, 'SHAP', 'DENS'));
			title += " (V = ";
			title += Text::FloatToString(CalculateVolume());
			title += " m\xC2\xB3)";
			return (new TextSetting('DENS', shapeDensity, title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kObjectShape, 'SHAP', 'COLL'));
			return (new BooleanSetting('COLL', ((shapeFlags & kShapeCollisionInhibit) != 0), title));
		}

		if (index == 3)
		{
			int32 count = 1;
			int32 selection = 0;

			const Substance *substance = MaterialObject::GetFirstRegisteredSubstance();
			while (substance)
			{
				if (substance->GetSubstanceType() == shapeSubstance)
				{
					selection = count;
				}

				substance = substance->Next();
				count++;
			}

			const char *title = table->GetString(StringID(kObjectShape, 'SHAP', 'SBST'));
			MenuSetting *menu = new MenuSetting('SBST', selection, title, count);
			menu->SetMenuItemString(0, table->GetString(StringID(kObjectShape, 'SHAP', 'SBST', 'NONE')));

			substance = MaterialObject::GetFirstRegisteredSubstance();
			for (machine a = 1; a < count; a++)
			{
				menu->SetMenuItemString(a, substance->GetSubstanceName());
				substance = substance->Next();
			}

			return (menu);
		}
	}

	return (nullptr);
}

void ShapeObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectShape)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'DENS')
		{
			shapeDensity = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'COLL')
		{
			if ((static_cast<const BooleanSetting *>(setting)->GetBooleanValue()) || (shapeDensity < 0.0F))
			{
				shapeFlags |= kShapeCollisionInhibit;
			}
			else
			{
				shapeFlags &= ~kShapeCollisionInhibit;
			}
		}
		else if (identifier == 'SBST')
		{
			int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
			if (selection == 0)
			{
				shapeSubstance = kSubstanceNone;
			}
			else
			{
				const Substance *substance = MaterialObject::GetFirstRegisteredSubstance();
				while (substance)
				{
					if (--selection == 0)
					{
						shapeSubstance = substance->GetSubstanceType();
						break;
					}

					substance = substance->Next();
				}
			}
		}
	}
}

int32 ShapeObject::GetObjectSize(float *size) const
{
	return (GetVolumeObjectSize(size));
}

void ShapeObject::SetObjectSize(const float *size)
{
	SetVolumeObjectSize(size);
}

void ShapeObject::Preprocess(void)
{
	shapeVolume = CalculateVolume();
}

bool ShapeObject::InteriorPoint(const Point3D& p) const
{
	int32 polygonCount = shapeMesh->polygonCount;
	const unsigned_int8 *polygon = shapeMesh->polygonArray;

	for (machine a = 0; a < polygonCount; a++)
	{
		int32 polygonVertexCount = polygon[0];

		int32 i1 = polygon[polygonVertexCount];
		int32 i2 = polygon[1];
		int32 i3 = polygon[2];

		const Point3D& v1 = shapeVertex[i1];
		const Point3D& v2 = shapeVertex[i2];
		const Point3D& v3 = shapeVertex[i3];

		Vector3D normal = (v2 - v1) % (v3 - v1);
		if (normal * p > normal * v1)
		{
			return (false);
		}
	}

	return (true);
}

bool ShapeObject::DetectSegmentIntersection(const Point3D& p1, const Point3D& p2, float radius, ShapeHitData *shapeHitData) const
{
	Bivector4D segmentLine(p1, p2);

	int32 polygonCount = shapeMesh->polygonCount;
	const unsigned_int8 *polygon = shapeMesh->polygonArray;

	for (machine i = 0; i < polygonCount; i++)
	{
		int32 polygonVertexCount = polygon[0];

		const Point3D& v1 = shapeVertex[polygon[polygonVertexCount]];
		const Point3D& v2 = shapeVertex[polygon[1]];
		const Point3D& v3 = shapeVertex[polygon[2]];

		Bivector4D edgeLine(v1, v2);
		Antivector4D plane = edgeLine ^ v3;
		plane.Standardize();

		float d1 = plane ^ p1;
		float d2 = plane ^ p2;

		if ((!(d1 < radius)) && (d2 < radius))
		{
			Bivector4D line = Translate(segmentLine, plane.GetAntivector3D() * -radius);
			if ((!((line ^ edgeLine) > 0.0F)) && (!((line ^ Bivector4D(v2, v3)) > 0.0F)))
			{
				const Point3D *u1 = &v3;

				for (machine c = 3; c <= polygonVertexCount; c++)
				{
					const Point3D *u2 = &shapeVertex[polygon[c]];
					if ((line ^ Bivector4D(*u1, *u2)) > 0.0F)
					{
						goto next;
					}

					u1 = u2;
				}

				float s = (d1 - radius) / (d1 - d2);
				shapeHitData->position = p1 + segmentLine.GetTangent() * s;
				shapeHitData->normal = plane.GetAntivector3D();
				shapeHitData->param = s;
				return (true);
			}
		}

		next:
		polygon += polygonVertexCount + 1;
	}

	if (radius > 0.0F)
	{
		bool result = false;
		float smax = 1.0F;

		int32 edgeCount = shapeMesh->edgeCount;
		const unsigned_int8 (*edge)[2] = shapeMesh->edgeArray;

		float r2 = radius * radius;

		const Vector3D& dp = segmentLine.GetTangent();
		float dp2 = dp * dp;
		float inverseLength = InverseSqrt(dp2);

		for (machine i = 0; i < edgeCount; i++)
		{
			const Point3D& v1 = shapeVertex[edge[i][0]];
			const Point3D& v2 = shapeVertex[edge[i][1]];

			Vector3D dv = v2 - v1;
			float e2 = dv * dv;

			Vector3D t = dv * InverseSqrt(e2);
			Vector3D p0 = p1 - v1;

			float p0_dot_t = p0 * t;
			float dp_dot_t = dp * t * inverseLength;

			float b = (p0 * dp) * inverseLength - p0_dot_t * dp_dot_t;
			if (b < 0.0F)
			{
				float a = 1.0F - dp_dot_t * dp_dot_t;
				float c = p0 * p0 - p0_dot_t * p0_dot_t - r2;
				float D = b * b - a * c;
				if (D > 0.0F)
				{
					float s = (-b - Sqrt(D)) * inverseLength / a;
					if ((s > 0.0F) && (s < smax))
					{
						Vector3D p = p0 + dp * s;

						float h = dv * p;
						if ((h > 0.0F) && (h < e2))
						{
							shapeHitData->position = v1 + p;
							shapeHitData->normal = p - ProjectOnto(p, t);
							shapeHitData->param = s;
							result = true;
							smax = s;
						}
					}
				}
			}
		}

		if (!result)
		{
			int32 vertexCount = shapeMesh->vertexCount;
			float ainv = inverseLength * inverseLength;

			for (machine i = 0; i < vertexCount; i++)
			{
				const Point3D& v1 = shapeVertex[i];
				Vector3D p0 = p1 - v1;

				float b = p0 * dp;
				if (b < 0.0F)
				{
					float c = p0 * p0 - r2;
					float D = b * b - dp2 * c;
					if (D > 0.0F)
					{
						float s = (-b - Sqrt(D)) * ainv;
						if ((s > 0.0F) && (s < smax))
						{
							shapeHitData->position = p1 + dp * s;
							shapeHitData->normal = shapeHitData->position - v1;
							shapeHitData->param = s;
							result = true;
							smax = s;
						}
					}
				}
			}
		}

		return (result);
	}

	return (false);
}

float ShapeObject::CalculateSubmergedVolume(const Antivector4D& plane, Point3D *centroid) const
{
	float volume = 0.0F;
	centroid->Set(0.0F, 0.0F, 0.0F);

	Point3D cm = CalculateCenterOfMass();
	Point3D apex = cm - plane.GetAntivector3D() * (plane ^ cm);

	int32 polygonCount = shapeMesh->polygonCount;
	const unsigned_int8 *polygon = shapeMesh->polygonArray;

	for (machine a = 0; a < polygonCount; a++)
	{
		const Point3D *vertex[kMaxShapePolygonVertexCount];
		int8 location[kMaxShapePolygonVertexCount];
		Point3D result[kMaxShapePolygonVertexCount + 1];

		int32 vertexCount = polygon[0];
		polygon++;

		int32 positive = 0;
		for (machine b = 0; b < vertexCount; b++)
		{
			const Point3D& v = shapeVertex[polygon[b]];
			vertex[b] = &v;

			float d = plane ^ v;
			if (d > kBoundaryEpsilon)
			{
				location[b] = kPolygonInterior;
				positive++;
			}
			else if (d < -kBoundaryEpsilon)
			{
				location[b] = kPolygonExterior;
			}
			else
			{
				location[b] = kPolygonBoundary;
			}
		}

		polygon += vertexCount;

		if (positive == vertexCount)
		{
			Point3D p = apex + *vertex[0];
			Vector3D v3 = *vertex[0] - apex;

			for (machine b = 2; b < vertexCount; b++)
			{
				Vector3D v1 = *vertex[b - 1] - *vertex[0];
				Vector3D v2 = *vertex[b] - *vertex[0];
				float vol = v1 % v2 * v3;

				volume += vol;
				*centroid += (p + *vertex[b - 1] + *vertex[b]) * vol;
			}
		}
		else if (positive > 0)
		{
			int32 resultCount = 0;
			int32 previous = vertexCount - 1;
			for (machine b = 0; b < vertexCount; b++)
			{
				int8 loc = location[b];
				if (loc == kPolygonExterior)
				{
					if (location[previous] == kPolygonInterior)
					{
						const Point3D& v1 = *vertex[previous];
						const Point3D& v2 = *vertex[b];
						Vector3D dv = v2 - v1;

						float t = (plane ^ v2) / (plane ^ dv);
						result[resultCount++] = v2 - dv * t;
					}
				}
				else
				{
					const Point3D& v1 = *vertex[b];
					if ((loc == kPolygonInterior) && (location[previous] == kPolygonExterior))
					{
						const Point3D& v2 = *vertex[previous];
						Vector3D dv = v2 - v1;

						float t = (plane ^ v2) / (plane ^ dv);
						result[resultCount++] = v2 - dv * t;
					}

					result[resultCount++] = v1;
				}

				previous = b;
			}

			Point3D p = apex + result[0];
			Vector3D v3 = result[0] - apex;

			for (machine b = 2; b < resultCount; b++)
			{
				Vector3D v1 = result[b - 1] - result[0];
				Vector3D v2 = result[b] - result[0];
				float vol = v1 % v2 * v3;

				volume += vol;
				*centroid += (p + result[b - 1] + result[b]) * vol;
			}
		}
	}

	*centroid *= 0.25F / volume;
	return (Fmin(volume * K::one_over_6, shapeVolume));
}


BoxShapeObject::BoxShapeObject() : ShapeObject(kShapeBox, &boxMesh, boxVertex, this)
{
}

BoxShapeObject::BoxShapeObject(const Vector3D& size) :
		ShapeObject(kShapeBox, &boxMesh, boxVertex, this),
		BoxVolume(size)
{
}

BoxShapeObject::~BoxShapeObject()
{
}

void BoxShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector3D& size = GetBoxSize();
	SetShapeShrinkSize(Fmin(Fmin(size.x, size.y, size.z) * 0.25F, kMaxShapeShrinkSize));

	boxVertex[0].Set(0.0F, 0.0F, 0.0F);
	boxVertex[1].Set(size.x, 0.0F, 0.0F);
	boxVertex[2].Set(0.0F, size.y, 0.0F);
	boxVertex[3].Set(size.x, size.y, 0.0F);
	boxVertex[4].Set(0.0F, 0.0F, size.z);
	boxVertex[5].Set(size.x, 0.0F, size.z);
	boxVertex[6].Set(0.0F, size.y, size.z);
	boxVertex[7].Set(size.x, size.y, size.z);
}

float BoxShapeObject::CalculateVolume(void) const
{
	const Vector3D& size = GetBoxSize();
	return (size.x * size.y * size.z);
}

Point3D BoxShapeObject::CalculateCenterOfMass(void) const
{
	const Vector3D& size = GetBoxSize();
	return (Point3D(size.x * 0.5F, size.y * 0.5F, size.z * 0.5F));
}

InertiaTensor BoxShapeObject::CalculateInertiaTensor(void) const
{
	const Vector3D& size = GetBoxSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;
	float z2 = size.z * size.z;

	float k = size.x * size.y * size.z * (GetShapeDensity() * K::one_over_12);
	return (InertiaTensor((y2 + z2) * k, (x2 + z2) * k, (x2 + y2) * k));
}

Point3D BoxShapeObject::GetInitialSupportPoint(void) const
{
	float d = GetShapeShrinkSize();
	return (Point3D(d, d, d));
}

Point3D BoxShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	#if C4SIMD

		Point3D		result;

		vec_float size = VecLoadUnaligned(&GetBoxSize().x);
		vec_float d = VecLoadSmearScalar(&GetShapeShrinkSize());

		vec_float mask = VecMaskCmpgt(VecLoadUnaligned(&direction.x), VecFloatGetZero());
		vec_float p = VecSelect(d, VecSub(size, d), mask);

		VecStore3D(p, &result.x);
		return (result);

	#else

		const Vector3D& size = GetBoxSize();
		float d = GetShapeShrinkSize();

		float x = (direction.x > 0.0F) ? size.x - d : d;
		float y = (direction.y > 0.0F) ? size.y - d : d;
		float z = (direction.z > 0.0F) ? size.z - d : d;

		return (Point3D(x, y, z));

	#endif
}

void BoxShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	#if C4SIMD

		vec_float size = VecLoadUnaligned(&GetBoxSize().x);
		vec_float d = VecLoadSmearScalar(&GetShapeShrinkSize());

		vec_float mask = VecMaskCmpgt(VecLoadUnaligned(&direction.x), VecFloatGetZero());
		vec_float p = VecSelect(d, VecSub(size, d), mask);

		VecStore3D(p, &support[0].x);
		VecStore3D(VecSub(size, p), &support[1].x);

	#else

		const Vector3D& size = GetBoxSize();
		float d = GetShapeShrinkSize();

		float x = (direction.x > 0.0F) ? size.x - d : d;
		float y = (direction.y > 0.0F) ? size.y - d : d;
		float z = (direction.z > 0.0F) ? size.z - d : d;

		support[0].Set(x, y, z);
		support[1].Set(size.x - x, size.y - y, size.z - z);

	#endif
}

void BoxShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	#if C4SIMD

		vec_float zero = VecFloatGetZero();
		vec_float size = VecLoadUnaligned(&GetBoxSize().x);

		for (machine a = 0; a < count; a++)
		{
			vec_float mask = VecMaskCmpgt(VecLoadUnaligned(&direction->x), zero);
			VecStore3D(VecAnd(size, mask), &support->x);

			direction++;
			support++;
		}

	#else

		const Vector3D& size = GetBoxSize();

		for (machine a = 0; a < count; a++)
		{
			support->x = (direction->x > 0.0F) ? size.x : 0.0F;
			support->y = (direction->y > 0.0F) ? size.y : 0.0F;
			support->z = (direction->z > 0.0F) ? size.z : 0.0F;

			direction++;
			support++;
		}

	#endif
}


PyramidShapeObject::PyramidShapeObject() : ShapeObject(kShapePyramid, &pyramidMesh, pyramidVertex, this)
{
}

PyramidShapeObject::PyramidShapeObject(const Vector2D& size, float height) :
		ShapeObject(kShapePyramid, &pyramidMesh, pyramidVertex, this),
		PyramidVolume(size, height)
{
}

PyramidShapeObject::~PyramidShapeObject()
{
}

void PyramidShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();

	float d = Fmin(Fmin(size.x, size.y, height) * 0.25F, kMaxShapeShrinkSize);
	SetShapeShrinkSize(d);

	float rx = size.x * 0.5F;
	float ry = size.y * 0.5F;
	float h2 = height * height;
	float fx = Sqrt(rx * rx + h2);
	float fy = Sqrt(ry * ry + h2);
	float g = d / height;
	pyramidShrinkSize.Set(g * (rx + fx), g * (ry + fy), Fmax(d / rx * fx, d / ry * fy));

	pyramidVertex[0].Set(0.0F, 0.0F, 0.0F);
	pyramidVertex[1].Set(size.x, 0.0F, 0.0F);
	pyramidVertex[2].Set(0.0F, size.y, 0.0F);
	pyramidVertex[3].Set(size.x, size.y, 0.0F);
	pyramidVertex[4].Set(rx, ry, height);
}

float PyramidShapeObject::CalculateVolume(void) const
{
	const Vector2D& size = GetPyramidSize();
	return (size.x * size.y * GetPyramidHeight() * K::one_over_3);
}

Point3D PyramidShapeObject::CalculateCenterOfMass(void) const
{
	const Vector2D& size = GetPyramidSize();
	return (Point3D(size.x * 0.5F, size.y * 0.5F, GetPyramidHeight() * 0.25F));
}

InertiaTensor PyramidShapeObject::CalculateInertiaTensor(void) const
{
	const Vector2D& size = GetPyramidSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;

	float height = GetPyramidHeight();
	float h2 = height * height;

	float k1 = size.x * size.y * height * (GetShapeDensity() * K::one_over_60);
	float k2 = k1 * 0.75F;
	return (InertiaTensor(y2 * k1 + h2 * k2, x2 * k1 + h2 * k2, (x2 + y2) * k1));
}

Point3D PyramidShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(pyramidShrinkSize.x, pyramidShrinkSize.y, GetShapeShrinkSize()));
}

Point3D PyramidShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();

	float x = (direction.x > 0.0F) ? size.x - pyramidShrinkSize.x : pyramidShrinkSize.x;
	float y = (direction.y > 0.0F) ? size.y - pyramidShrinkSize.y : pyramidShrinkSize.y;
	Point3D apex(size.x * 0.5F, size.y * 0.5F, height - pyramidShrinkSize.z);

	float d = GetShapeShrinkSize();
	float t = x * direction.x + y * direction.y + d * direction.z;
	float u = apex * direction;

	if (u > t)
	{
		return (apex);
	}

	return (Point3D(x, y, d));
}

void PyramidShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();

	float x = (direction.x > 0.0F) ? size.x - pyramidShrinkSize.x : pyramidShrinkSize.x;
	float y = (direction.y > 0.0F) ? size.y - pyramidShrinkSize.y : pyramidShrinkSize.y;
	Point3D apex(size.x * 0.5F, size.y * 0.5F, height - pyramidShrinkSize.z);

	float d = GetShapeShrinkSize();
	float t = x * direction.x + y * direction.y + d * direction.z;
	float u = apex * direction;

	if (u > t)
	{
		support[0] = apex;
		support[1].Set(size.x - x, size.y - y, d);
	}
	else
	{
		support[0].Set(x, y, d);
		support[1] = apex;
	}
}

void PyramidShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();

	Point3D apex(size.x * 0.5F, size.y * 0.5F, height);

	for (machine a = 0; a < count; a++)
	{
		float x = (direction->x > 0.0F) ? size.x : 0.0F;
		float y = (direction->y > 0.0F) ? size.y : 0.0F;

		float t = x * direction->x + y * direction->y;
		float u = apex * *direction;

		if (u > t)
		{
			*support = apex;
		}
		else
		{
			support->Set(x, y, 0.0F);
		}

		direction++;
		support++;
	}
}


CylinderShapeObject::CylinderShapeObject() : ShapeObject(kShapeCylinder, &cylinderMesh, cylinderVertex, this)
{
}

CylinderShapeObject::CylinderShapeObject(const Vector2D& size, float height) :
		ShapeObject(kShapeCylinder, &cylinderMesh, cylinderVertex, this),
		CylinderVolume(size, height)
{
}

CylinderShapeObject::~CylinderShapeObject()
{
}

void CylinderShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector2D& size = GetCylinderSize();
	float height = GetCylinderHeight();

	SetShapeShrinkSize(Fmin(Fmin(size.x, size.y) * 0.5F, height * 0.25F, kMaxShapeShrinkSize));

	float hx = size.x * K::sqrt_2_over_2;
	float hy = size.y * K::sqrt_2_over_2;

	cylinderVertex[0].Set(size.x, 0.0F, 0.0F);
	cylinderVertex[1].Set(hx, hy, 0.0F);
	cylinderVertex[2].Set(0.0F, size.y, 0.0F);
	cylinderVertex[3].Set(-hx, hy, 0.0F);
	cylinderVertex[4].Set(-size.x, 0.0F, 0.0F);
	cylinderVertex[5].Set(-hx, -hy, 0.0F);
	cylinderVertex[6].Set(0.0F, -size.y, 0.0F);
	cylinderVertex[7].Set(hx, -hy, 0.0F);
	cylinderVertex[8].Set(size.x, 0.0F, height);
	cylinderVertex[9].Set(hx, hy, height);
	cylinderVertex[10].Set(0.0F, size.y, height);
	cylinderVertex[11].Set(-hx, hy, height);
	cylinderVertex[12].Set(-size.x, 0.0F, height);
	cylinderVertex[13].Set(-hx, -hy, height);
	cylinderVertex[14].Set(0.0F, -size.y, height);
	cylinderVertex[15].Set(hx, -hy, height);
}

float CylinderShapeObject::CalculateVolume(void) const
{
	const Vector2D& size = GetCylinderSize();
	return (size.x * size.y * GetCylinderHeight() * K::tau_over_2);
}

Point3D CylinderShapeObject::CalculateCenterOfMass(void) const
{
	return (Point3D(0.0F, 0.0F, GetCylinderHeight() * 0.5F));
}

InertiaTensor CylinderShapeObject::CalculateInertiaTensor(void) const
{
	const Vector2D& size = GetCylinderSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;

	float height = GetCylinderHeight();
	float h2 = height * height;

	float k = size.x * size.y * height * (GetShapeDensity() * K::tau_over_24);
	return (InertiaTensor((y2 * 3.0F + h2) * k, (x2 * 3.0F + h2) * k, (x2 + y2) * (k * 3.0F)));
}

Point3D CylinderShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(0.0F, 0.0F, GetShapeShrinkSize()));
}

Point3D CylinderShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	const Vector2D& size = GetCylinderSize();
	float height = GetCylinderHeight();
	float d = GetShapeShrinkSize();

	float sx = size.x - d;
	float sy = size.y - d;

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;
	float z = (direction.z > 0.0F) ? height - d : d;

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		return (Point3D(x * r, y * r, z));
	}

	return (Point3D(0.0F, 0.0F, z));
}

void CylinderShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	const Vector2D& size = GetCylinderSize();
	float height = GetCylinderHeight();
	float d = GetShapeShrinkSize();

	float sx = size.x - d;
	float sy = size.y - d;
	float sz = height - d;

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;
	float r = x * direction.x + y * direction.y;

	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		x *= r;
		y *= r;

		float z = (direction.z > 0.0F) ? sz : d;
		support[0].Set(x, y, z);
		support[1].Set(-x, -y, height - z);
	}
	else
	{
		if (direction.z > 0.0F)
		{
			support[0].Set(0.0F, 0.0F, sz);
			support[1].Set(0.0F, 0.0F, d);
		}
		else
		{
			support[0].Set(0.0F, 0.0F, d);
			support[1].Set(0.0F, 0.0F, sz);
		}
	}
}

void CylinderShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	const Vector2D& size = GetCylinderSize();
	float height = GetCylinderHeight();

	for (machine a = 0; a < count; a++)
	{
		float x = size.x * size.x * direction->x;
		float y = size.y * size.y * direction->y;
		float z = (direction->z > 0.0F) ? height : 0.0F;

		float r = x * direction->x + y * direction->y;
		if (r > K::min_float)
		{
			r = InverseSqrt(r);
			support->Set(x * r, y * r, z);
		}
		else
		{
			support->Set(0.0F, 0.0F, z);
		}

		direction++;
		support++;
	}
}


ConeShapeObject::ConeShapeObject() : ShapeObject(kShapeCone, &coneMesh, coneVertex, this)
{
}

ConeShapeObject::ConeShapeObject(const Vector2D& size, float height) :
		ShapeObject(kShapeCone, &coneMesh, coneVertex, this),
		ConeVolume(size, height)
{
}

ConeShapeObject::~ConeShapeObject()
{
}

void ConeShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();

	float d = Fmin(Fmin(size.x, size.y) * 0.5F, height * 0.25F, kMaxShapeShrinkSize);
	SetShapeShrinkSize(d);

	float rx = size.x;
	float ry = size.y;
	float h2 = height * height;
	float fx = Sqrt(rx * rx + h2);
	float fy = Sqrt(ry * ry + h2);
	float g = d / height;
	coneShrinkSize.Set(g * (rx + fx), g * (ry + fy), Fmax(d / rx * fx, d / ry * fy));

	float hx = rx * K::sqrt_2_over_2;
	float hy = ry * K::sqrt_2_over_2;

	coneVertex[0].Set(rx, 0.0F, 0.0F);
	coneVertex[1].Set(hx, hy, 0.0F);
	coneVertex[2].Set(0.0F, ry, 0.0F);
	coneVertex[3].Set(-hx, hy, 0.0F);
	coneVertex[4].Set(-rx, 0.0F, 0.0F);
	coneVertex[5].Set(-hx, -hy, 0.0F);
	coneVertex[6].Set(0.0F, -ry, 0.0F);
	coneVertex[7].Set(hx, -hy, 0.0F);
	coneVertex[8].Set(0.0F, 0.0F, height);
}

float ConeShapeObject::CalculateVolume(void) const
{
	const Vector2D& size = GetConeSize();
	return (size.x * size.y * GetConeHeight() * K::tau_over_6);
}

Point3D ConeShapeObject::CalculateCenterOfMass(void) const
{
	return (Point3D(0.0F, 0.0F, GetConeHeight() * 0.25F));
}

InertiaTensor ConeShapeObject::CalculateInertiaTensor(void) const
{
	const Vector2D& size = GetConeSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;

	float height = GetConeHeight();
	float h2 = height * height;

	float k = size.x * size.y * height * (GetShapeDensity() * K::tau_over_40);
	return (InertiaTensor((y2 + h2 * 0.25F) * k, (x2 + h2 * 0.25F) * k, (x2 + y2) * k));
}

Point3D ConeShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(0.0F, 0.0F, GetShapeShrinkSize()));
}

Point3D ConeShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();

	float sx = size.x - coneShrinkSize.x;
	float sy = size.y - coneShrinkSize.y;

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;
	float r = x * direction.x + y * direction.y;

	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		x *= r;
		y *= r;

		float h = height - coneShrinkSize.z;
		float d = GetShapeShrinkSize();

		float t = x * direction.x + y * direction.y + d * direction.z;
		float u = h * direction.z;

		if (u > t)
		{
			return (Point3D(0.0F, 0.0F, h));
		}

		return (Point3D(x, y, d));
	}

	if (direction.z > 0.0F)
	{
		return (Point3D(0.0F, 0.0F, height - coneShrinkSize.z));
	}

	return (Point3D(0.0F, 0.0F, GetShapeShrinkSize()));
}

void ConeShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();

	float sx = size.x - coneShrinkSize.x;
	float sy = size.y - coneShrinkSize.y;
	float h = height - coneShrinkSize.z;
	float d = GetShapeShrinkSize();

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;
	float r = x * direction.x + y * direction.y;

	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		x *= r;
		y *= r;

		float t = x * direction.x + y * direction.y + d * direction.z;
		float u = h * direction.z;

		if (u > t)
		{
			support[0].Set(0.0F, 0.0F, h);
			support[1].Set(-x, -y, d);
		}
		else
		{
			support[0].Set(x, y, d);
			support[1].Set(0.0F, 0.0F, h);
		}
	}
	else
	{
		if (direction.z > 0.0F)
		{
			support[0].Set(0.0F, 0.0F, h);
			support[1].Set(0.0F, 0.0F, d);
		}
		else
		{
			support[0].Set(0.0F, 0.0F, d);
			support[1].Set(0.0F, 0.0F, h);
		}
	}
}

void ConeShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();

	for (machine a = 0; a < count; a++)
	{
		float x = size.x * size.x * direction->x;
		float y = size.y * size.y * direction->y;
		float r = x * direction->x + y * direction->y;

		if (r > K::min_float)
		{
			r = InverseSqrt(r);
			x *= r;
			y *= r;

			float t = x * direction->x + y * direction->y;
			float u = height * direction->z;

			if (u > t)
			{
				support->Set(0.0F, 0.0F, height);
			}
			else
			{
				support->Set(x, y, 0.0F);
			}
		}
		else
		{
			if (direction->z > 0.0F)
			{
				support->Set(0.0F, 0.0F, height);
			}
			else
			{
				support->Set(0.0F, 0.0F, 0.0F);
			}
		}

		direction++;
		support++;
	}
}


SphereShapeObject::SphereShapeObject() : ShapeObject(kShapeSphere, &sphereMesh, sphereVertex, this)
{
}

SphereShapeObject::SphereShapeObject(const Vector3D& size) :
		ShapeObject(kShapeSphere, &sphereMesh, sphereVertex, this),
		SphereVolume(size)
{
}

SphereShapeObject::~SphereShapeObject()
{
}

void SphereShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector3D& size = GetSphereSize();
	sphericalFlag = ((Fabs(size.y - size.x) < 0.001F) && (Fabs(size.z - size.x) < 0.001F));

	SetShapeShrinkSize(Fmin(Fmin(size.x, size.y, size.z) * 0.25F, kMaxShapeShrinkSize));

	float hx = size.x * K::sqrt_2_over_2;
	float hy = size.y * K::sqrt_2_over_2;
	float hz = size.z * K::sqrt_2_over_2;

	sphereVertex[0].Set(size.x, 0.0F, 0.0F);
	sphereVertex[1].Set(hx, hy, 0.0F);
	sphereVertex[2].Set(0.0F, size.y, 0.0F);
	sphereVertex[3].Set(-hx, hy, 0.0F);
	sphereVertex[4].Set(-size.x, 0.0F, 0.0F);
	sphereVertex[5].Set(-hx, -hy, 0.0F);
	sphereVertex[6].Set(0.0F, -size.y, 0.0F);
	sphereVertex[7].Set(hx, -hy, 0.0F);

	float dx = hx;
	float dy = hy;
	hx *= K::sqrt_2_over_2;
	hy *= K::sqrt_2_over_2;

	sphereVertex[8].Set(dx, 0.0F, hz);
	sphereVertex[9].Set(hx, hy, hz);
	sphereVertex[10].Set(0.0F, dy, hz);
	sphereVertex[11].Set(-hx, hy, hz);
	sphereVertex[12].Set(-dx, 0.0F, hz);
	sphereVertex[13].Set(-hx, -hy, hz);
	sphereVertex[14].Set(0.0F, -dy, hz);
	sphereVertex[15].Set(hx, -hy, hz);

	sphereVertex[16].Set(dx, 0.0F, -hz);
	sphereVertex[17].Set(hx, hy, -hz);
	sphereVertex[18].Set(0.0F, dy, -hz);
	sphereVertex[19].Set(-hx, hy, -hz);
	sphereVertex[20].Set(-dx, 0.0F, -hz);
	sphereVertex[21].Set(-hx, -hy, -hz);
	sphereVertex[22].Set(0.0F, -dy, -hz);
	sphereVertex[23].Set(hx, -hy, -hz);

	sphereVertex[24].Set(0.0F, 0.0F, size.z);
	sphereVertex[25].Set(0.0F, 0.0F, -size.z);
}

float SphereShapeObject::CalculateVolume(void) const
{
	const Vector3D& size = GetSphereSize();
	return (size.x * size.y * size.z * K::two_tau_over_3);
}

Point3D SphereShapeObject::CalculateCenterOfMass(void) const
{
	return (Point3D(0.0F, 0.0F, 0.0F));
}

InertiaTensor SphereShapeObject::CalculateInertiaTensor(void) const
{
	const float two_tau_over_15 = 0.83775804095727819692337156887453F;

	const Vector3D& size = GetSphereSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;
	float z2 = size.z * size.z;

	float k = size.x * size.y * size.z * (two_tau_over_15 * GetShapeDensity());
	return (InertiaTensor((y2 + z2) * k, (x2 + z2) * k, (x2 + y2) * k));
}

Point3D SphereShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(GetSphereSize().x - GetShapeShrinkSize(), 0.0F, 0.0F));
}

Point3D SphereShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	#if C4SIMD

		Point3D		result;

		vec_float size = VecLoadUnaligned(&GetSphereSize().x);
		vec_float s = VecSub(size, VecLoadSmearScalar(&GetShapeShrinkSize()));

		vec_float dir = VecLoadUnaligned(&direction.x);
		vec_float p = VecMul(VecMul(s, s), dir);
		p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

		VecStore3D(p, &result.x);
		return (result);

	#else

		const Vector3D& size = GetSphereSize();
		float d = GetShapeShrinkSize();

		float sx = size.x - d;
		float sy = size.y - d;
		float sz = size.z - d;

		float x = sx * sx * direction.x;
		float y = sy * sy * direction.y;
		float z = sz * sz * direction.z;
		float r = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

		return (Point3D(x * r, y * r, z * r));

	#endif
}

void SphereShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	#if C4SIMD

		vec_float size = VecLoadUnaligned(&GetSphereSize().x);
		vec_float s = VecSub(size, VecLoadSmearScalar(&GetShapeShrinkSize()));

		vec_float dir = VecLoadUnaligned(&direction.x);
		vec_float p = VecMul(VecMul(s, s), dir);
		p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

		VecStore3D(p, &support[0].x);
		VecStore3D(VecSub(VecFloatGetZero(), p), &support[1].x);

	#else

		const Vector3D& size = GetSphereSize();
		float d = GetShapeShrinkSize();

		float sx = size.x - d;
		float sy = size.y - d;
		float sz = size.z - d;

		float x = sx * sx * direction.x;
		float y = sy * sy * direction.y;
		float z = sz * sz * direction.z;
		float r = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);
		x *= r;
		y *= r;
		z *= r;

		support[0].Set(x, y, z);
		support[1].Set(-x, -y, -z);

	#endif
}

void SphereShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	#if C4SIMD

		vec_float size = VecLoadUnaligned(&GetSphereSize().x);
		vec_float s2 = VecMul(size, size);

		for (machine a = 0; a < count; a++)
		{
			vec_float dir = VecLoadUnaligned(&direction->x);
			vec_float p = VecMul(s2, dir);
			p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

			VecStore3D(p, &support->x);

			direction++;
			support++;
		}

	#else

		const Vector3D& size = GetSphereSize();
		Vector3D s2 = size & size;

		for (machine a = 0; a < count; a++)
		{
			float x = s2.x * direction->x;
			float y = s2.y * direction->y;
			float z = s2.z * direction->z;
			float r = InverseSqrt(x * direction->x + y * direction->y + z * direction->z);

			support->Set(x * r, y * r, z * r);

			direction++;
			support++;
		}

	#endif
}

bool SphereShapeObject::DetectSegmentIntersection(const Point3D& p1, const Point3D& p2, float radius, ShapeHitData *shapeHitData) const
{
	if (sphericalFlag)
	{
		float r = radius + GetSphereSize().x;
		Vector3D dp = p2 - p1;

		float a = dp * dp;
		float b = p1 * dp;
		float c = p1 * p1 - r * r;
		float D = b * b - a * c;
		if (D > 0.0F)
		{
			float t = -b - Sqrt(D);
			if ((t > 0.0F) && (t < a))
			{
				t /= a;

				shapeHitData->position = p1 + dp * t;
				shapeHitData->normal = shapeHitData->position;
				shapeHitData->param = t;
				return (true);
			}
		}

		return (false);
	}

	return (ShapeObject::DetectSegmentIntersection(p1, p2, radius, shapeHitData));
}

float SphereShapeObject::CalculateSubmergedVolume(const Antivector4D& plane, Point3D *centroid) const
{
	if (sphericalFlag)
	{
		float d = plane.w;
		float r = GetSphereSize().x;

		if (d < -r)
		{
			return (0.0F);
		}

		if (d > r)
		{
			centroid->Set(0.0F, 0.0F, 0.0F);
			return (r * r * r * K::two_tau_over_3);
		}

		float d2 = d * d;
		float r2 = r * r;
		float volume = r2 * (r * K::tau_over_3 + d * K::tau_over_2) - d2 * d * K::tau_over_6;

		float z = (r2 * d2 * K::tau_over_4 - (r2 * r2 + d2 * d2) * K::tau_over_8) / volume;
		*centroid = Zero3D - plane.GetAntivector3D() * z;

		return (volume);
	}

	return (ShapeObject::CalculateSubmergedVolume(plane, centroid));
}


DomeShapeObject::DomeShapeObject() : ShapeObject(kShapeDome, &domeMesh, domeVertex, this)
{
}

DomeShapeObject::DomeShapeObject(const Vector3D& size) :
		ShapeObject(kShapeDome, &domeMesh, domeVertex, this),
		DomeVolume(size)
{
}

DomeShapeObject::~DomeShapeObject()
{
}

void DomeShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector3D& size = GetDomeSize();
	SetShapeShrinkSize(Fmin(Fmin(size.x, size.y, size.z) * 0.25F, kMaxShapeShrinkSize));

	float hx = size.x * K::sqrt_2_over_2;
	float hy = size.y * K::sqrt_2_over_2;
	float hz = size.z * K::sqrt_2_over_2;

	domeVertex[0].Set(size.x, 0.0F, 0.0F);
	domeVertex[1].Set(hx, hy, 0.0F);
	domeVertex[2].Set(0.0F, size.y, 0.0F);
	domeVertex[3].Set(-hx, hy, 0.0F);
	domeVertex[4].Set(-size.x, 0.0F, 0.0F);
	domeVertex[5].Set(-hx, -hy, 0.0F);
	domeVertex[6].Set(0.0F, -size.y, 0.0F);
	domeVertex[7].Set(hx, -hy, 0.0F);

	float dx = hx;
	float dy = hy;
	hx *= K::sqrt_2_over_2;
	hy *= K::sqrt_2_over_2;

	domeVertex[8].Set(dx, 0.0F, hz);
	domeVertex[9].Set(hx, hy, hz);
	domeVertex[10].Set(0.0F, dy, hz);
	domeVertex[11].Set(-hx, hy, hz);
	domeVertex[12].Set(-dx, 0.0F, hz);
	domeVertex[13].Set(-hx, -hy, hz);
	domeVertex[14].Set(0.0F, -dy, hz);
	domeVertex[15].Set(hx, -hy, hz);

	domeVertex[16].Set(0.0F, 0.0F, size.z);
}

float DomeShapeObject::CalculateVolume(void) const
{
	const Vector3D& size = GetDomeSize();
	return (size.x * size.y * size.z * K::tau_over_3);
}

Point3D DomeShapeObject::CalculateCenterOfMass(void) const
{
	return (Point3D(0.0F, 0.0F, GetDomeSize().z * 0.375F));
}

InertiaTensor DomeShapeObject::CalculateInertiaTensor(void) const
{
	const float tau_over_15 = 0.41887902047863909846168578443727F;

	const Vector3D& size = GetDomeSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;
	float z2 = size.z * size.z;

	float k = size.x * size.y * size.z * (tau_over_15 * GetShapeDensity());
	return (InertiaTensor((y2 + z2) * k - z2 * k * 0.703125F, (x2 + z2) * k - z2 * k * 0.703125F, (x2 + y2) * k));
}

Point3D DomeShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(0.0F, 0.0F, GetShapeShrinkSize()));
}

Point3D DomeShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	const Vector3D& size = GetDomeSize();
	float d = GetShapeShrinkSize();

	float sx = size.x - d;
	float sy = size.y - d;

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;

	if (direction.z > 0.0F)
	{
		float sz = size.z - d * 2.0F;

		float z = sz * sz * direction.z;
		float r = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

		return (Point3D(x * r, y * r, z * r + d));
	}

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		return (Point3D(x * r, y * r, d));
	}

	return (Point3D(0.0F, 0.0F, d));
}

void DomeShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	const Vector3D& size = GetDomeSize();
	float d = GetShapeShrinkSize();

	float sx = size.x - d;
	float sy = size.y - d;
	float sz = size.z - d * 2.0F;

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;

	float r1 = x * direction.x + y * direction.y;
	if (r1 > K::min_float)
	{
		r1 = InverseSqrt(r1);
		float z = sz * sz * direction.z;
		float r2 = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

		if (direction.z > 0.0F)
		{
			support[0].Set(x * r2, y * r2, z * r2 + d);
			support[1].Set(-x * r1, -y * r1, d);
		}
		else
		{
			support[0].Set(x * r1, y * r1, d);
			support[1].Set(-x * r2, -y * r2, d - z * r2);
		}
	}
	else
	{
		if (direction.z > 0.0F)
		{
			support[0].Set(0.0F, 0.0F, sz + d);
			support[1].Set(0.0F, 0.0F, d);
		}
		else
		{
			support[0].Set(0.0F, 0.0F, d);
			support[1].Set(0.0F, 0.0F, sz + d);
		}
	}
}

void DomeShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	const Vector3D& size = GetDomeSize();

	for (machine a = 0; a < count; a++)
	{
		float x = size.x * size.x * direction->x;
		float y = size.y * size.y * direction->y;

		if (direction->z > 0.0F)
		{
			float z = size.z * size.z * direction->z;
			float r = InverseSqrt(x * direction->x + y * direction->y + z * direction->z);

			support->Set(x * r, y * r, z * r);
		}
		else
		{
			float r = x * direction->x + y * direction->y;
			if (r > K::min_float)
			{
				r = InverseSqrt(r);
				support->Set(x * r, y * r, 0.0F);
			}
			else
			{
				support->Set(0.0F, 0.0F, 0.0F);
			}
		}

		direction++;
		support++;
	}
}


CapsuleShapeObject::CapsuleShapeObject() : ShapeObject(kShapeCapsule, &capsuleMesh, capsuleVertex, this)
{
}

CapsuleShapeObject::CapsuleShapeObject(const Vector3D& size, float height) :
		ShapeObject(kShapeCapsule, &capsuleMesh, capsuleVertex, this),
		CapsuleVolume(size, height)
{
}

CapsuleShapeObject::~CapsuleShapeObject()
{
}

void CapsuleShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector3D& size = GetCapsuleSize();
	float height = GetCapsuleHeight();

	SetShapeShrinkSize(Fmin(Fmin(size.x, size.y, size.z, height) * 0.25F, kMaxShapeShrinkSize));

	float hx = size.x * K::sqrt_2_over_2;
	float hy = size.y * K::sqrt_2_over_2;
	float hz = size.z * K::sqrt_2_over_2;

	capsuleVertex[0].Set(size.x, 0.0F, 0.0F);
	capsuleVertex[1].Set(hx, hy, 0.0F);
	capsuleVertex[2].Set(0.0F, size.y, 0.0F);
	capsuleVertex[3].Set(-hx, hy, 0.0F);
	capsuleVertex[4].Set(-size.x, 0.0F, 0.0F);
	capsuleVertex[5].Set(-hx, -hy, 0.0F);
	capsuleVertex[6].Set(0.0F, -size.y, 0.0F);
	capsuleVertex[7].Set(hx, -hy, 0.0F);

	capsuleVertex[8].Set(size.x, 0.0F, height);
	capsuleVertex[9].Set(hx, hy, height);
	capsuleVertex[10].Set(0.0F, size.y, height);
	capsuleVertex[11].Set(-hx, hy, height);
	capsuleVertex[12].Set(-size.x, 0.0F, height);
	capsuleVertex[13].Set(-hx, -hy, height);
	capsuleVertex[14].Set(0.0F, -size.y, height);
	capsuleVertex[15].Set(hx, -hy, height);

	float dx = hx;
	float dy = hy;
	hx *= K::sqrt_2_over_2;
	hy *= K::sqrt_2_over_2;

	float h1 = height + hz;
	capsuleVertex[16].Set(dx, 0.0F, h1);
	capsuleVertex[17].Set(hx, hy, h1);
	capsuleVertex[18].Set(0.0F, dy, h1);
	capsuleVertex[19].Set(-hx, hy, h1);
	capsuleVertex[20].Set(-dx, 0.0F, h1);
	capsuleVertex[21].Set(-hx, -hy, h1);
	capsuleVertex[22].Set(0.0F, -dy, h1);
	capsuleVertex[23].Set(hx, -hy, h1);

	float h2 = -hz;
	capsuleVertex[24].Set(dx, 0.0F, h2);
	capsuleVertex[25].Set(hx, hy, h2);
	capsuleVertex[26].Set(0.0F, dy, h2);
	capsuleVertex[27].Set(-hx, hy, h2);
	capsuleVertex[28].Set(-dx, 0.0F, h2);
	capsuleVertex[29].Set(-hx, -hy, h2);
	capsuleVertex[30].Set(0.0F, -dy, h2);
	capsuleVertex[31].Set(hx, -hy, h2);

	capsuleVertex[32].Set(0.0F, 0.0F, height + size.z);
	capsuleVertex[33].Set(0.0F, 0.0F, -size.z);
}

float CapsuleShapeObject::CalculateVolume(void) const
{
	const Vector3D& size = GetCapsuleSize();
	float rxry = size.x * size.y;
	return (rxry * size.z * K::two_tau_over_3 + rxry * GetCapsuleHeight() * K::tau_over_2);
}

Point3D CapsuleShapeObject::CalculateCenterOfMass(void) const
{
	return (Point3D(0.0F, 0.0F, GetCapsuleHeight() * 0.5F));
}

InertiaTensor CapsuleShapeObject::CalculateInertiaTensor(void) const
{
	const Vector3D& size = GetCapsuleSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;
	float z2 = size.z * size.z;

	float height = GetCapsuleHeight();
	float h2 = height * height;

	float rxry = size.x * size.y;
	float k1 = rxry * height * (GetShapeDensity() * K::tau_over_24);
	float k2 = rxry * size.z * (GetShapeDensity() * K::tau_over_3);
	float k3 = height * size.z * 0.75F + h2 * 0.5F;
	return (InertiaTensor((y2 * 3.0F + h2) * k1 + ((y2 + z2) * 0.4F + k3) * k2, (x2 * 3.0F + h2) * k1 + ((x2 + z2) * 0.4F + k3) * k2, (x2 + y2) * (k1 * 3.0F + k2 * 0.4F)));
}

Point3D CapsuleShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(GetCapsuleSize().x - GetShapeShrinkSize(), 0.0F, 0.0F));
}

Point3D CapsuleShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	#if C4SIMD

		Point3D		result;

		vec_float size = VecLoadUnaligned(&GetCapsuleSize().x);
		vec_float s = VecSub(size, VecLoadSmearScalar(&GetShapeShrinkSize()));

		vec_float dir = VecLoadUnaligned(&direction.x);
		vec_float p = VecMul(VecMul(s, s), dir);
		p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

		vec_float z = VecSmearZ(p);
		z = VecSelect(z, VecAdd(z, VecLoadScalar(&GetCapsuleHeight())), VecMaskCmpgt(VecSmearZ(dir), VecFloatGetZero()));

		VecStoreX(p, &result.x, 0);
		VecStoreY(p, &result.x, 1);
		VecStoreX(z, &result.z);
		return (result);

	#else

		const Vector3D& size = GetCapsuleSize();
		float d = GetShapeShrinkSize();

		float sx = size.x - d;
		float sy = size.y - d;
		float sz = size.z - d;

		float x = sx * sx * direction.x;
		float y = sy * sy * direction.y;
		float z = sz * sz * direction.z;
		float r = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

		z *= r;
		if (direction.z > 0.0F)
		{
			z += GetCapsuleHeight();
		}

		return (Point3D(x * r, y * r, z));

	#endif
}

void CapsuleShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	#if C4SIMD

		vec_float size = VecLoadUnaligned(&GetCapsuleSize().x);
		vec_float s = VecSub(size, VecLoadSmearScalar(&GetShapeShrinkSize()));

		vec_float dir = VecLoadUnaligned(&direction.x);
		vec_float p = VecMul(VecMul(s, s), dir);
		p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

		vec_float z = VecSmearZ(p);

		if (direction.z > 0.0F)
		{
			VecStoreX(p, &support[0].x, 0);
			VecStoreY(p, &support[0].x, 1);
			VecStoreX(VecAdd(z, VecLoadScalar(&GetCapsuleHeight())), &support[0].z);

			VecStore3D(VecNegate(p), &support[1].x);
		}
		else
		{
			VecStore3D(p, &support[0].x);

			p = VecNegate(p);
			VecStoreX(p, &support[1].x, 0);
			VecStoreY(p, &support[1].x, 1);
			VecStoreX(VecSub(VecLoadScalar(&GetCapsuleHeight()), z), &support[1].z);
		}

	#else

		const Vector3D& size = GetCapsuleSize();
		float height = GetCapsuleHeight();
		float d = GetShapeShrinkSize();

		float sx = size.x - d;
		float sy = size.y - d;
		float sz = size.z - d;

		float x = sx * sx * direction.x;
		float y = sy * sy * direction.y;
		float z = sz * sz * direction.z;
		float r = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);
		x *= r;
		y *= r;
		z *= r;

		if (direction.z > 0.0F)
		{
			support[0].Set(x, y, z + height);
			support[1].Set(-x, -y, -z);
		}
		else
		{
			support[0].Set(x, y, z);
			support[1].Set(-x, -y, height - z);
		}

	#endif
}

void CapsuleShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	#if C4SIMD

		vec_float height = VecLoadScalar(&GetCapsuleHeight());
		vec_float size = VecLoadUnaligned(&GetCapsuleSize().x);
		vec_float s2 = VecMul(size, size);

		for (machine a = 0; a < count; a++)
		{
			vec_float dir = VecLoadUnaligned(&direction->x);
			vec_float p = VecMul(s2, dir);
			p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

			vec_float z = VecSmearZ(p);
			z = VecSelect(z, VecAdd(z, height), VecMaskCmpgt(VecSmearZ(dir), VecFloatGetZero()));

			VecStoreX(p, &support->x, 0);
			VecStoreY(p, &support->x, 1);
			VecStoreX(z, &support->z);

			direction++;
			support++;
		}

	#else

		float height = GetCapsuleHeight();
		const Vector3D& size = GetCapsuleSize();
		Vector3D s2 = size & size;

		for (machine a = 0; a < count; a++)
		{
			float x = s2.x * direction->x;
			float y = s2.y * direction->y;
			float z = s2.z * direction->z;
			float r = InverseSqrt(x * direction->x + y * direction->y + z * direction->z);

			z *= r;
			if (direction->z > 0.0F)
			{
				z += height;
			}

			support->Set(x * r, y * r, z);

			direction++;
			support++;
		}

	#endif
}


TruncatedPyramidShapeObject::TruncatedPyramidShapeObject() : ShapeObject(kShapeTruncatedPyramid, &truncatedPyramidMesh, truncatedPyramidVertex, this)
{
}

TruncatedPyramidShapeObject::TruncatedPyramidShapeObject(const Vector2D& size, float height, float ratio) :
		ShapeObject(kShapeTruncatedPyramid, &truncatedPyramidMesh, truncatedPyramidVertex, this),
		TruncatedPyramidVolume(size, height, ratio)
{
}

TruncatedPyramidShapeObject::~TruncatedPyramidShapeObject()
{
}

void TruncatedPyramidShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();
	float ratio = GetPyramidRatio();

	float d = Fmin(Fmin(Fmin(size.x, size.y) * ratio, height) * 0.25F, kMaxShapeShrinkSize);
	SetShapeShrinkSize(d);

	float rx = size.x * 0.5F;
	float ry = size.y * 0.5F;
	float m1 = 1.0F - ratio;
	float m2 = 1.0F + ratio;
	float h2 = height * height / (m1 * m1);
	float fx = Sqrt(rx * rx + h2);
	float fy = Sqrt(ry * ry + h2);
	float g = d * m1 / height;
	pyramidShrinkSize.Set(g * (rx + fx), g * (ry + fy));

	truncatedPyramidVertex[0].Set(0.0F, 0.0F, 0.0F);
	truncatedPyramidVertex[1].Set(size.x, 0.0F, 0.0F);
	truncatedPyramidVertex[2].Set(0.0F, size.y, 0.0F);
	truncatedPyramidVertex[3].Set(size.x, size.y, 0.0F);
	truncatedPyramidVertex[4].Set(rx * m1, ry * m1, height);
	truncatedPyramidVertex[5].Set(rx * m2, ry * m1, height);
	truncatedPyramidVertex[6].Set(rx * m1, ry * m2, height);
	truncatedPyramidVertex[7].Set(rx * m2, ry * m2, height);
}

float TruncatedPyramidShapeObject::CalculateVolume(void) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();
	float ratio = GetPyramidRatio();

	return (size.x * size.y * height * ((ratio * ratio + ratio + 1.0F) * K::one_over_3));
}

Point3D TruncatedPyramidShapeObject::CalculateCenterOfMass(void) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();
	float ratio = GetPyramidRatio();
	float r2 = ratio * ratio;

	float z = (r2 * 0.75F + ratio * 0.5F + 0.25F) / (r2 + ratio + 1.0F);
	return (Point3D(size.x * 0.5F, size.y * 0.5F, z * height));
}

InertiaTensor TruncatedPyramidShapeObject::CalculateInertiaTensor(void) const
{
	const Vector2D& size = GetPyramidSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;

	float height = GetPyramidHeight();
	float h2 = height * height;

	float ratio = GetPyramidRatio();
	float r2 = ratio * ratio;
	float m1 = r2 + ratio + 1.0F;
	float m2 = r2 * r2 + r2 * ratio + m1;
	float m3 = r2 * r2 + r2 * ratio * 4.0F + r2 * 10.0F + ratio * 4.0F + 1.0F;

	float k1 = size.x * size.y * height * (GetShapeDensity() * K::one_over_60);
	float k2 = k1 * h2 * 0.75F * m3 / m1;
	k1 *= m2;

	return (InertiaTensor(y2 * k1 + k2, x2 * k1 + k2, (x2 + y2) * k1));
}

Point3D TruncatedPyramidShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(pyramidShrinkSize.x, pyramidShrinkSize.y, GetShapeShrinkSize()));
}

Point3D TruncatedPyramidShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();
	float ratio = GetPyramidRatio();
	float m1 = 0.5F - 0.5F * ratio;
	float m2 = 0.5F + 0.5F * ratio;

	float x1 = (direction.x > 0.0F) ? size.x - pyramidShrinkSize.x : pyramidShrinkSize.x;
	float y1 = (direction.y > 0.0F) ? size.y - pyramidShrinkSize.y : pyramidShrinkSize.y;

	float x2 = (direction.x > 0.0F) ? size.x * m2 - pyramidShrinkSize.x : size.x * m1 + pyramidShrinkSize.x;
	float y2 = (direction.y > 0.0F) ? size.y * m2 - pyramidShrinkSize.y : size.y * m1 + pyramidShrinkSize.y;

	float d = GetShapeShrinkSize();
	float h = height - d;

	float t = x1 * direction.x + y1 * direction.y + d * direction.z;
	float u = x2 * direction.x + y2 * direction.y + h * direction.z;

	if (t > u)
	{
		return (Point3D(x1, y1, d));
	}

	return (Point3D(x2, y2, h));
}

void TruncatedPyramidShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();
	float ratio = GetPyramidRatio();
	float m1 = 0.5F - 0.5F * ratio;
	float m2 = 0.5F + 0.5F * ratio;

	float x1 = (direction.x > 0.0F) ? size.x - pyramidShrinkSize.x : pyramidShrinkSize.x;
	float y1 = (direction.y > 0.0F) ? size.y - pyramidShrinkSize.y : pyramidShrinkSize.y;

	float x2 = (direction.x > 0.0F) ? size.x * m2 - pyramidShrinkSize.x : size.x * m1 + pyramidShrinkSize.x;
	float y2 = (direction.y > 0.0F) ? size.y * m2 - pyramidShrinkSize.y : size.y * m1 + pyramidShrinkSize.y;

	float d = GetShapeShrinkSize();
	float h = height - d;

	float t = x1 * direction.x + y1 * direction.y + d * direction.z;
	float u = x2 * direction.x + y2 * direction.y + h * direction.z;

	if (t > u)
	{
		support[0].Set(x1, y1, d);
		support[1].Set(-x2, -y2, h);
	}
	else
	{
		support[0].Set(x2, y2, h);
		support[1].Set(-x1, -y1, d);
	}
}

void TruncatedPyramidShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	const Vector2D& size = GetPyramidSize();
	float height = GetPyramidHeight();
	float ratio = GetPyramidRatio();
	float m1 = 0.5F - 0.5F * ratio;
	float m2 = 0.5F + 0.5F * ratio;

	for (machine a = 0; a < count; a++)
	{
		float x1 = (direction->x > 0.0F) ? size.x : 0.0F;
		float y1 = (direction->y > 0.0F) ? size.y : 0.0F;

		float x2 = (direction->x > 0.0F) ? size.x * m2 : size.x * m1;
		float y2 = (direction->y > 0.0F) ? size.y * m2 : size.y * m1;

		float t = x1 * direction->x + y1 * direction->y;
		float u = x2 * direction->x + y2 * direction->y + height * direction->z;

		if (t > u)
		{
			support->Set(x1, y1, 0.0F);
		}
		else
		{
			support->Set(x2, y2, height);
		}

		direction++;
		support++;
	}
}


TruncatedConeShapeObject::TruncatedConeShapeObject() : ShapeObject(kShapeTruncatedCone, &truncatedConeMesh, truncatedConeVertex, this)
{
}

TruncatedConeShapeObject::TruncatedConeShapeObject(const Vector2D& size, float height, float ratio) :
		ShapeObject(kShapeTruncatedCone, &truncatedConeMesh, truncatedConeVertex, this),
		TruncatedConeVolume(size, height, ratio)
{
}

TruncatedConeShapeObject::~TruncatedConeShapeObject()
{
}

void TruncatedConeShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();
	float ratio = GetConeRatio();

	float d = Fmin(Fmin(size.x, size.y) * (ratio * 0.5F), height * 0.25F, kMaxShapeShrinkSize);
	SetShapeShrinkSize(d);

	float rx = size.x;
	float ry = size.y;
	float m1 = 1.0F - ratio;
	float h2 = height * height / (m1 * m1);
	float fx = Sqrt(rx * rx + h2);
	float fy = Sqrt(ry * ry + h2);
	float g = d * m1 / height;
	coneShrinkSize.Set(g * (rx + fx), g * (ry + fy));

	float hx = rx * K::sqrt_2_over_2;
	float hy = ry * K::sqrt_2_over_2;

	truncatedConeVertex[0].Set(rx, 0.0F, 0.0F);
	truncatedConeVertex[1].Set(hx, hy, 0.0F);
	truncatedConeVertex[2].Set(0.0F, ry, 0.0F);
	truncatedConeVertex[3].Set(-hx, hy, 0.0F);
	truncatedConeVertex[4].Set(-rx, 0.0F, 0.0F);
	truncatedConeVertex[5].Set(-hx, -hy, 0.0F);
	truncatedConeVertex[6].Set(0.0F, -ry, 0.0F);
	truncatedConeVertex[7].Set(hx, -hy, 0.0F);

	hx *= ratio;
	hy *= ratio;

	truncatedConeVertex[8].Set(rx * ratio, 0.0F, height);
	truncatedConeVertex[9].Set(hx, hy, height);
	truncatedConeVertex[10].Set(0.0F, ry * ratio, height);
	truncatedConeVertex[11].Set(-hx, hy, height);
	truncatedConeVertex[12].Set(-rx * ratio, 0.0F, height);
	truncatedConeVertex[13].Set(-hx, -hy, height);
	truncatedConeVertex[14].Set(0.0F, -ry * ratio, height);
	truncatedConeVertex[15].Set(hx, -hy, height);
}

float TruncatedConeShapeObject::CalculateVolume(void) const
{
	const Vector2D& size = GetConeSize();
	float ratio = GetConeRatio();

	return (size.x * size.y * (GetConeHeight() * K::tau_over_6) * (ratio * ratio + ratio + 1.0F));
}

Point3D TruncatedConeShapeObject::CalculateCenterOfMass(void) const
{
	float height = GetConeHeight();
	float ratio = GetConeRatio();
	float r2 = ratio * ratio;

	float z = (r2 * 0.75F + ratio * 0.5F + 0.25F) / (r2 + ratio + 1.0F);
	return (Point3D(0.0F, 0.0F, z * height));
}

InertiaTensor TruncatedConeShapeObject::CalculateInertiaTensor(void) const
{
	const Vector2D& size = GetConeSize();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;

	float height = GetConeHeight();
	float h2 = height * height;

	float ratio = GetConeRatio();
	float r2 = ratio * ratio;
	float m1 = r2 + ratio + 1.0F;
	float m2 = r2 * r2 + r2 * ratio + m1;
	float m3 = r2 * r2 * 0.25F + r2 * ratio + r2 * 2.5F + ratio + 0.25F;

	float k1 = size.x * size.y * height * (GetShapeDensity() * K::tau_over_40);
	float k2 = k1 * h2 * m3 / m1;
	k1 *= m2;

	return (InertiaTensor(y2 * k1 + k2, x2 * k1 + k2, (x2 + y2) * k1));
}

Point3D TruncatedConeShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(0.0F, 0.0F, GetShapeShrinkSize()));
}

Point3D TruncatedConeShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();
	float ratio = GetConeRatio();

	float sx = size.x - coneShrinkSize.x;
	float sy = size.y - coneShrinkSize.y;

	float x1 = sx * sx * direction.x;
	float y1 = sy * sy * direction.y;
	float r = x1 * direction.x + y1 * direction.y;

	float d = GetShapeShrinkSize();
	float h = height - d;

	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		x1 *= r;
		y1 *= r;

		float x2 = x1 * ratio;
		float y2 = y1 * ratio;

		float t = x1 * direction.x + y1 * direction.y + d * direction.z;
		float u = x2 * direction.x + y2 * direction.y + h * direction.z;

		if (t > u)
		{
			return (Point3D(x1, y1, d));
		}

		return (Point3D(x2, y2, h));
	}

	if (direction.z > 0.0F)
	{
		return (Point3D(0.0F, 0.0F, h));
	}

	return (Point3D(0.0F, 0.0F, d));
}

void TruncatedConeShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();
	float ratio = GetConeRatio();

	float sx = size.x - coneShrinkSize.x;
	float sy = size.y - coneShrinkSize.y;

	float x1 = sx * sx * direction.x;
	float y1 = sy * sy * direction.y;
	float r = x1 * direction.x + y1 * direction.y;

	float d = GetShapeShrinkSize();
	float h = height - d;

	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		x1 *= r;
		y1 *= r;

		float x2 = x1 * ratio;
		float y2 = y1 * ratio;

		float t = x1 * direction.x + y1 * direction.y + d * direction.z;
		float u = x2 * direction.x + y2 * direction.y + h * direction.z;

		if (t > u)
		{
			support[0].Set(x1, y1, d);
			support[1].Set(-x2, -y2, h);
		}
		else
		{
			support[0].Set(x2, y2, h);
			support[1].Set(-x1, -y1, d);
		}
	}
	else
	{
		if (direction.z > 0.0F)
		{
			support[0].Set(0.0F, 0.0F, h);
			support[1].Set(0.0F, 0.0F, d);
		}
		else
		{
			support[0].Set(0.0F, 0.0F, d);
			support[1].Set(0.0F, 0.0F, h);
		}
	}
}

void TruncatedConeShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	const Vector2D& size = GetConeSize();
	float height = GetConeHeight();
	float ratio = GetConeRatio();

	for (machine a = 0; a < count; a++)
	{
		float x1 = size.x * size.x * direction->x;
		float y1 = size.y * size.y * direction->y;
		float r = x1 * direction->x + y1 * direction->y;

		if (r > K::min_float)
		{
			r = InverseSqrt(r);
			x1 *= r;
			y1 *= r;

			float x2 = x1 * ratio;
			float y2 = y1 * ratio;

			float t = x1 * direction->x + y1 * direction->y;
			float u = x2 * direction->x + y2 * direction->y + height * direction->z;

			if (t > u)
			{
				support->Set(x1, y1, 0.0F);
			}
			else
			{
				support->Set(x2, y2, height);
			}
		}
		else
		{
			if (direction->z > 0.0F)
			{
				support->Set(0.0F, 0.0F, height);
			}
			else
			{
				support->Set(0.0F, 0.0F, 0.0F);
			}
		}

		direction++;
		support++;
	}
}


TruncatedDomeShapeObject::TruncatedDomeShapeObject() : ShapeObject(kShapeTruncatedDome, &truncatedDomeMesh, truncatedDomeVertex, this)
{
}

TruncatedDomeShapeObject::TruncatedDomeShapeObject(const Vector2D& size, float height, float ratio) :
		ShapeObject(kShapeTruncatedDome, &truncatedDomeMesh, truncatedDomeVertex, this),
		TruncatedDomeVolume(size, height, ratio)
{
}

TruncatedDomeShapeObject::~TruncatedDomeShapeObject()
{
}

void TruncatedDomeShapeObject::Preprocess(void)
{
	ShapeObject::Preprocess();

	const Vector2D& size = GetDomeSize().GetVector2D();
	float height = GetDomeHeight();
	float ratio = GetDomeRatio();

	SetShapeShrinkSize(Fmin(Fmin(size.x, size.y) * (ratio * 0.5F), height * 0.25F, kMaxShapeShrinkSize));

	float hx = size.x * K::sqrt_2_over_2;
	float hy = size.y * K::sqrt_2_over_2;

	truncatedDomeVertex[0].Set(size.x, 0.0F, 0.0F);
	truncatedDomeVertex[1].Set(hx, hy, 0.0F);
	truncatedDomeVertex[2].Set(0.0F, size.y, 0.0F);
	truncatedDomeVertex[3].Set(-hx, hy, 0.0F);
	truncatedDomeVertex[4].Set(-size.x, 0.0F, 0.0F);
	truncatedDomeVertex[5].Set(-hx, -hy, 0.0F);
	truncatedDomeVertex[6].Set(0.0F, -size.y, 0.0F);
	truncatedDomeVertex[7].Set(hx, -hy, 0.0F);

	float s = 0.5F * Sqrt(ratio * ratio + 3.0F);
	float z = height * 0.5F;
	float mx = hx * s;
	float my = hy * s;

	truncatedDomeVertex[8].Set(size.x * s, 0.0F, z);
	truncatedDomeVertex[9].Set(mx, my, z);
	truncatedDomeVertex[10].Set(0.0F, size.y * s, z);
	truncatedDomeVertex[11].Set(-mx, my, z);
	truncatedDomeVertex[12].Set(-size.x * s, 0.0F, z);
	truncatedDomeVertex[13].Set(-mx, -my, z);
	truncatedDomeVertex[14].Set(0.0F, -size.y * s, z);
	truncatedDomeVertex[15].Set(mx, -my, z);

	hx *= ratio;
	hy *= ratio;

	truncatedDomeVertex[16].Set(size.x * ratio, 0.0F, height);
	truncatedDomeVertex[17].Set(hx, hy, height);
	truncatedDomeVertex[18].Set(0.0F, size.y * ratio, height);
	truncatedDomeVertex[19].Set(-hx, hy, height);
	truncatedDomeVertex[20].Set(-size.x * ratio, 0.0F, height);
	truncatedDomeVertex[21].Set(-hx, -hy, height);
	truncatedDomeVertex[22].Set(0.0F, -size.y * ratio, height);
	truncatedDomeVertex[23].Set(hx, -hy, height);
}

float TruncatedDomeShapeObject::CalculateVolume(void) const
{
	const Vector2D& size = GetDomeSize().GetVector2D();
	float ratio = GetDomeRatio();

	return (size.x * size.y * (GetDomeHeight() * K::tau_over_6) * (ratio * ratio + 2.0F));
}

Point3D TruncatedDomeShapeObject::CalculateCenterOfMass(void) const
{
	float height = GetDomeHeight();
	float ratio = GetDomeRatio();
	float r2 = ratio * ratio;

	float z = (1.0F - r2 * r2) * 0.75F / ((1.0F - r2) * (r2 + 2.0F));
	return (Point3D(0.0F, 0.0F, z * height));
}

InertiaTensor TruncatedDomeShapeObject::CalculateInertiaTensor(void) const
{
	const float tau_over_480 = 0.01308996938995747182692768076366F;

	const Vector2D& size = GetDomeSize().GetVector2D();
	float x2 = size.x * size.x;
	float y2 = size.y * size.y;

	float height = GetDomeHeight();
	float h2 = height * height;

	float ratio = GetDomeRatio();
	float r2 = ratio * ratio;
	float r4 = r2 * r2;
	float r6 = r4 * r2;

	float m1 = h2 * (19.0F + r2 * 19.0F - r4 * 35.0F - r6 * 3.0F) / (1.0F - r2);
	float m2 = 64.0F + r2 * 64.0F + r4 * 40.0F + r6 * 12.0F;
	float m3 = r4 * 12.0F + r2 * 16.0F + 32.0F;

	float k = size.x * size.y * height * (GetShapeDensity() * tau_over_480) / (r2 + 2.0F);
	return (InertiaTensor((m1 + y2 * m2) * k, (m1 + x2 * m2) * k, (x2 + y2) * k * m3));
}

Point3D TruncatedDomeShapeObject::GetInitialSupportPoint(void) const
{
	return (Point3D(0.0F, 0.0F, GetShapeShrinkSize()));
}

Point3D TruncatedDomeShapeObject::CalculateSupportPoint(const Vector3D& direction) const
{
	const Vector3D& size = GetDomeSize();
	float d = GetShapeShrinkSize();

	float sx = size.x - d;
	float sy = size.y - d;

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		if (direction.z > 0.0F)
		{
			float sz = size.z - d * 2.0F;
			float h = GetDomeHeight() - d;

			float z = sz * sz * direction.z;
			float f = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

			z = z * f + d;
			if (z < h)
			{
				return (Point3D(x * f, y * f, z));
			}

			r = InverseSqrt(r) * GetDomeRatio();
			return (Point3D(x * r, y * r, h));
		}

		r = InverseSqrt(r);
		return (Point3D(x * r, y * r, d));
	}

	if (direction.z > 0.0F)
	{
		return (Point3D(0.0F, 0.0F, GetDomeHeight() - d));
	}

	return (Point3D(0.0F, 0.0F, d));
}

void TruncatedDomeShapeObject::CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const
{
	const Vector3D& size = GetDomeSize();
	float d = GetShapeShrinkSize();
	float h = GetDomeHeight() - d;

	float sx = size.x - d;
	float sy = size.y - d;

	float x = sx * sx * direction.x;
	float y = sy * sy * direction.y;

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		r = InverseSqrt(r);

		float sz = size.z - d * 2.0F;
		float z = sz * sz * direction.z;
		float f = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

		if (direction.z > 0.0F)
		{
			z = z * f + d;
			if (z < h)
			{
				support[0].Set(x * f, y * f, z);
			}
			else
			{
				float g = r * GetDomeRatio();
				support[0].Set(x * g, y * g, h);
			}

			support[1].Set(-x * r, -y * r, d);
		}
		else
		{
			support[0].Set(x * r, y * r, d);

			z = -z * f + d;
			if (z < h)
			{
				support[1].Set(-x * f, -y * f, z);
			}
			else
			{
				float g = r * GetDomeRatio();
				support[1].Set(-x * g, -y * g, h);
			}
		}
	}
	else
	{
		if (direction.z > 0.0F)
		{
			support[0].Set(0.0F, 0.0F, h);
			support[1].Set(0.0F, 0.0F, d);
		}
		else
		{
			support[0].Set(0.0F, 0.0F, d);
			support[1].Set(0.0F, 0.0F, h);
		}
	}
}

void TruncatedDomeShapeObject::CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	const Vector3D& size = GetDomeSize();
	float height = GetDomeHeight();
	float ratio = GetDomeRatio();
	Vector3D s2 = size & size;

	for (machine a = 0; a < count; a++)
	{
		float x = s2.x * direction->x;
		float y = s2.y * direction->y;

		float r = x * direction->x + y * direction->y;
		if (r > K::min_float)
		{
			if (direction->z > 0.0F)
			{
				float z = s2.z * direction->z;
				float f = InverseSqrt(x * direction->x + y * direction->y + z * direction->z);

				z *= f;
				if (z < height)
				{
					support->Set(x * f, y * f, z);
				}
				else
				{
					r = InverseSqrt(r) * ratio;
					support->Set(x * r, y * r, height);
				}
			}
			else
			{
				r = InverseSqrt(r);
				support->Set(x * r, y * r, 0.0F);
			}
		}
		else
		{
			support->Set(0.0F, 0.0F, (direction->z > 0.0F) ? height : 0.0F);
		}

		direction++;
		support++;
	}
}


Shape::Shape(ShapeType type) : Node(kNodeShape)
{
	shapeType = type;
}

Shape::Shape(const Shape& shape) : Node(shape)
{
	shapeType = shape.shapeType;
}

Shape::~Shape()
{
}

Shape *Shape::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kShapeBox:

			return (new BoxShape);

		case kShapePyramid:

			return (new PyramidShape);

		case kShapeCylinder:

			return (new CylinderShape);

		case kShapeCone:

			return (new ConeShape);

		case kShapeSphere:

			return (new SphereShape);

		case kShapeDome:

			return (new DomeShape);

		case kShapeCapsule:

			return (new CapsuleShape);

		case kShapeTruncatedPyramid:

			return (new TruncatedPyramidShape);

		case kShapeTruncatedCone:

			return (new TruncatedConeShape);

		case kShapeTruncatedDome:

			return (new TruncatedDomeShape);
	}

	return (nullptr);
}

void Shape::PackType(Packer& data) const
{
	Node::PackType(data);
	data << shapeType;
}

void Shape::Preprocess(void)
{
	Node::Preprocess();

	if (!GetManipulator())
	{
		GetObject()->Preprocess();

		const Node *super = GetSuperNode();
		const Controller *controller = super->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody) && (controller->Asleep()))
		{
			Box3D	boundingBox;

			CalculateBoundingBox(&boundingBox);
			shapeCollisionBox = Transform(boundingBox, GetWorldTransform());
		}
	}
}

void Shape::CalculateCollisionBox(const Transform4D& transform1, const Transform4D& transform2)
{
	Box3D	boundingBox;

	CalculateBoundingBox(&boundingBox);
	shapeCollisionBox = Union(Transform(boundingBox, transform1 * GetNodeTransform()), Transform(boundingBox, transform2 * GetNodeTransform()));
}

unsigned_int32 Shape::CalculateZeroSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	*p = simplex[0];
	return (0x0001);
}

unsigned_int32 Shape::CalculateOneSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	#if C4SIMD

		vec_float q1 = VecLoadUnaligned(&simplex[0].x);
		vec_float q2 = VecLoadUnaligned(&simplex[1].x);
		vec_float v = VecSub(q2, q1);

		const vec_float zero = VecFloatGetZero();

		if (!VecCmpltScalar(VecDot3D(v, q1), zero))
		{
			VecStore3D(q1, &p->x);
			return (0x0001);
		}

		if (!VecCmpgtScalar(VecDot3D(v, q2), zero))
		{
			VecStore3D(q2, &p->x);
			return (0x0002);
		}

		vec_float f = VecDiv(VecProjectOnto3D(q1, v), VecSmearX(VecDot3D(v, v)));
		VecStore3D(VecSub(q1, f), &p->x);
		return (0x0003);

	#else

		const Point3D& q1 = simplex[0];
		const Point3D& q2 = simplex[1];
		Vector3D v = q2 - q1;

		if (!(v * q1 < 0.0F))
		{
			// Origin is in Voronoi region for vertex q1.

			*p = q1;
			return (0x0001);
		}

		if (!(v * q2 > 0.0F))
		{
			// Origin is in Voronoi region for vertex q2.

			*p = q2;
			return (0x0002);
		}

		// Origin is in Voronoi region for line segment interior.

		*p = q1 - ProjectOnto(q1, v) / SquaredMag(v);
		return (0x0003);

	#endif
}

unsigned_int32 Shape::CalculateTwoSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	#if C4SIMD

		vec_float q1 = VecLoadUnaligned(&simplex[0].x);
		vec_float q2 = VecLoadUnaligned(&simplex[1].x);
		vec_float q3 = VecLoadUnaligned(&simplex[2].x);
		vec_float v1 = VecSub(q2, q1);
		vec_float v2 = VecSub(q3, q2);
		vec_float v3 = VecSub(q1, q3);

		const vec_float zero = VecFloatGetZero();

		bool m1 = !VecCmpltScalar(VecDot3D(v1, q1), zero);
		bool p1 = !VecCmpgtScalar(VecDot3D(v1, q2), zero);
		bool m2 = !VecCmpltScalar(VecDot3D(v2, q2), zero);
		bool p2 = !VecCmpgtScalar(VecDot3D(v2, q3), zero);
		bool m3 = !VecCmpltScalar(VecDot3D(v3, q3), zero);
		bool p3 = !VecCmpgtScalar(VecDot3D(v3, q1), zero);

		if (m1 & p3)
		{
			VecStore3D(q1, &p->x);
			return (0x0001);
		}

		if (m2 & p1)
		{
			VecStore3D(q2, &p->x);
			return (0x0002);
		}

		if (m3 & p2)
		{
			VecStore3D(q3, &p->x);
			return (0x0004);
		}

		vec_float n = VecCross3D(v3, v1);
		bool d1 = !VecCmpltScalar(VecDot3D(VecCross3D(n, v1), q1), zero);
		bool d2 = !VecCmpltScalar(VecDot3D(VecCross3D(n, v2), q2), zero);
		bool d3 = !VecCmpltScalar(VecDot3D(VecCross3D(n, v3), q3), zero);

		if ((d1) && (!m1) && (!p1))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q1, v1), VecSmearX(VecDot3D(v1, v1)));
			VecStore3D(VecSub(q1, f), &p->x);
			return (0x0003);
		}

		if ((d2) && (!m2) && (!p2))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q2, v2), VecSmearX(VecDot3D(v2, v2)));
			VecStore3D(VecSub(q2, f), &p->x);
			return (0x0006);
		}

		if ((d3) && (!m3) && (!p3))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q3, v3), VecSmearX(VecDot3D(v3, v3)));
			VecStore3D(VecSub(q3, f), &p->x);
			return (0x0005);
		}

		vec_float f = VecDiv(VecProjectOnto3D(q1, n), VecSmearX(VecDot3D(n, n)));
		VecStore3D(f, &p->x);
		return (0x0007);

	#else

		const Point3D& q1 = simplex[0];
		const Point3D& q2 = simplex[1];
		const Point3D& q3 = simplex[2];
		Vector3D v1 = q2 - q1;
		Vector3D v2 = q3 - q2;
		Vector3D v3 = q1 - q3;

		bool m1 = !(v1 * q1 < 0.0F);
		bool p1 = !(v1 * q2 > 0.0F);
		bool m2 = !(v2 * q2 < 0.0F);
		bool p2 = !(v2 * q3 > 0.0F);
		bool m3 = !(v3 * q3 < 0.0F);
		bool p3 = !(v3 * q1 > 0.0F);

		if (m1 & p3)
		{
			// Origin is in Voronoi region for vertex q1.

			*p = q1;
			return (0x0001);
		}

		if (m2 & p1)
		{
			// Origin is in Voronoi region for vertex q2.

			*p = q2;
			return (0x0002);
		}

		if (m3 & p2)
		{
			// Origin is in Voronoi region for vertex q3.

			*p = q3;
			return (0x0004);
		}

		Vector3D n = v3 % v1;
		bool d1 = !((n % v1) * q1 < 0.0F);
		bool d2 = !((n % v2) * q2 < 0.0F);
		bool d3 = !((n % v3) * q3 < 0.0F);

		if ((d1) && (!m1) && (!p1))
		{
			// Origin is in Voronoi region for edge v1.

			*p = q1 - ProjectOnto(q1, v1) / SquaredMag(v1);
			return (0x0003);
		}

		if ((d2) && (!m2) && (!p2))
		{
			// Origin is in Voronoi region for edge v2.

			*p = q2 - ProjectOnto(q2, v2) / SquaredMag(v2);
			return (0x0006);
		}

		if ((d3) && (!m3) && (!p3))
		{
			// Origin is in Voronoi region for edge v3.

			*p = q3 - ProjectOnto(q3, v3) / SquaredMag(v3);
			return (0x0005);
		}

		// Origin is in Voronoi region for triangle interior.

		*p = ProjectOnto(q1, n) / SquaredMag(n);
		return (0x0007);

	#endif
}

unsigned_int32 Shape::CalculateThreeSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	#if C4SIMD

		vec_float q1 = VecLoadUnaligned(&simplex[0].x);
		vec_float q2 = VecLoadUnaligned(&simplex[1].x);
		vec_float q3 = VecLoadUnaligned(&simplex[2].x);
		vec_float q4 = VecLoadUnaligned(&simplex[3].x);
		vec_float v12 = VecSub(q2, q1);
		vec_float v23 = VecSub(q3, q2);
		vec_float v31 = VecSub(q1, q3);
		vec_float v14 = VecSub(q4, q1);
		vec_float v24 = VecSub(q4, q2);
		vec_float v34 = VecSub(q4, q3);

		const vec_float zero = VecFloatGetZero();

		bool m12 = !VecCmpltScalar(VecDot3D(v12, q1), zero);
		bool p12 = !VecCmpgtScalar(VecDot3D(v12, q2), zero);
		bool m23 = !VecCmpltScalar(VecDot3D(v23, q2), zero);
		bool p23 = !VecCmpgtScalar(VecDot3D(v23, q3), zero);
		bool m31 = !VecCmpltScalar(VecDot3D(v31, q3), zero);
		bool p31 = !VecCmpgtScalar(VecDot3D(v31, q1), zero);
		bool m14 = !VecCmpltScalar(VecDot3D(v14, q1), zero);
		bool p14 = !VecCmpgtScalar(VecDot3D(v14, q4), zero);
		bool m24 = !VecCmpltScalar(VecDot3D(v24, q2), zero);
		bool p24 = !VecCmpgtScalar(VecDot3D(v24, q4), zero);
		bool m34 = !VecCmpltScalar(VecDot3D(v34, q3), zero);
		bool p34 = !VecCmpgtScalar(VecDot3D(v34, q4), zero);

		if (m12 & p31 & m14)
		{
			VecStore3D(q1, &p->x);
			return (0x0001);
		}

		if (m23 & p12 & m24)
		{
			VecStore3D(q2, &p->x);
			return (0x0002);
		}

		if (m31 & p23 & m34)
		{
			VecStore3D(q3, &p->x);
			return (0x0004);
		}

		if (p14 & p24 & p34)
		{
			VecStore3D(q4, &p->x);
			return (0x0008);
		}

		vec_float n1 = VecCross3D(v12, v31);
		vec_float n2 = VecCross3D(v12, v14);
		vec_float n3 = VecCross3D(v23, v24);
		vec_float n4 = VecCross3D(v31, v34);

		bool d11 = !VecCmpltScalar(VecDot3D(VecCross3D(v12, n1), q1), zero);
		bool d12 = !VecCmpltScalar(VecDot3D(VecCross3D(v23, n1), q2), zero);
		bool d13 = !VecCmpltScalar(VecDot3D(VecCross3D(v31, n1), q3), zero);
		bool d21 = !VecCmpltScalar(VecDot3D(VecCross3D(n2, v12), q1), zero);
		bool d22 = !VecCmpltScalar(VecDot3D(VecCross3D(n2, v24), q2), zero);
		bool d24 = !VecCmpltScalar(VecDot3D(VecCross3D(v14, n2), q4), zero);
		bool d32 = !VecCmpltScalar(VecDot3D(VecCross3D(n3, v23), q2), zero);
		bool d33 = !VecCmpltScalar(VecDot3D(VecCross3D(n3, v34), q3), zero);
		bool d34 = !VecCmpltScalar(VecDot3D(VecCross3D(v24, n3), q4), zero);
		bool d43 = !VecCmpltScalar(VecDot3D(VecCross3D(n4, v31), q3), zero);
		bool d41 = !VecCmpltScalar(VecDot3D(VecCross3D(n4, v14), q1), zero);
		bool d44 = !VecCmpltScalar(VecDot3D(VecCross3D(v34, n4), q4), zero);

		if ((!m12) && (!p12) && (d11) && (d21))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q1, v12), VecSmearX(VecDot3D(v12, v12)));
			VecStore3D(VecSub(q1, f), &p->x);
			return (0x0003);
		}

		if ((!m23) && (!p23) && (d12) && (d32))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q2, v23), VecSmearX(VecDot3D(v23, v23)));
			VecStore3D(VecSub(q2, f), &p->x);
			return (0x0006);
		}

		if ((!m31) && (!p31) && (d13) && (d43))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q3, v31), VecSmearX(VecDot3D(v31, v31)));
			VecStore3D(VecSub(q3, f), &p->x);
			return (0x0005);
		}

		if ((!m14) && (!p14) && (d24) && (d41))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q1, v14), VecSmearX(VecDot3D(v14, v14)));
			VecStore3D(VecSub(q1, f), &p->x);
			return (0x0009);
		}

		if ((!m24) && (!p24) && (d22) && (d34))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q2, v24), VecSmearX(VecDot3D(v24, v24)));
			VecStore3D(VecSub(q2, f), &p->x);
			return (0x000A);
		}

		if ((!m34) && (!p34) && (d33) && (d44))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q3, v34), VecSmearX(VecDot3D(v34, v34)));
			VecStore3D(VecSub(q3, f), &p->x);
			return (0x000C);
		}

		bool f1 = !VecCmpltScalar(VecMulScalar(VecDot3D(n1, q1), VecDot3D(n1, v14)), zero);
		bool f2 = !VecCmpgtScalar(VecMulScalar(VecDot3D(n2, q2), VecDot3D(n2, v31)), zero);
		bool f3 = !VecCmpgtScalar(VecMulScalar(VecDot3D(n3, q3), VecDot3D(n3, v12)), zero);
		bool f4 = !VecCmpgtScalar(VecMulScalar(VecDot3D(n4, q4), VecDot3D(n4, v23)), zero);

		if ((f1) && (!d11) && (!d12) && (!d13))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q1, n1), VecSmearX(VecDot3D(n1, n1)));
			VecStore3D(f, &p->x);
			return (0x0007);
		}

		if ((f2) && (!d21) && (!d22) && (!d24))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q2, n2), VecSmearX(VecDot3D(n2, n2)));
			VecStore3D(f, &p->x);
			return (0x000B);
		}

		if ((f3) && (!d32) && (!d33) && (!d34))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q3, n3), VecSmearX(VecDot3D(n3, n3)));
			VecStore3D(f, &p->x);
			return (0x000E);
		}

		if ((f4) && (!d43) && (!d41) && (!d44))
		{
			vec_float f = VecDiv(VecProjectOnto3D(q4, n4), VecSmearX(VecDot3D(n4, n4)));
			VecStore3D(f, &p->x);
			return (0x000D);
		}

		VecStore3D(zero, &p->x);
		return (0x000F);

	#else

		const Point3D& q1 = simplex[0];
		const Point3D& q2 = simplex[1];
		const Point3D& q3 = simplex[2];
		const Point3D& q4 = simplex[3];
		Vector3D v12 = q2 - q1;
		Vector3D v23 = q3 - q2;
		Vector3D v31 = q1 - q3;
		Vector3D v14 = q4 - q1;
		Vector3D v24 = q4 - q2;
		Vector3D v34 = q4 - q3;

		bool m12 = !(v12 * q1 < 0.0F);
		bool p12 = !(v12 * q2 > 0.0F);
		bool m23 = !(v23 * q2 < 0.0F);
		bool p23 = !(v23 * q3 > 0.0F);
		bool m31 = !(v31 * q3 < 0.0F);
		bool p31 = !(v31 * q1 > 0.0F);
		bool m14 = !(v14 * q1 < 0.0F);
		bool p14 = !(v14 * q4 > 0.0F);
		bool m24 = !(v24 * q2 < 0.0F);
		bool p24 = !(v24 * q4 > 0.0F);
		bool m34 = !(v34 * q3 < 0.0F);
		bool p34 = !(v34 * q4 > 0.0F);

		if (m12 & p31 & m14)
		{
			// Origin is in Voronoi region for vertex q1.

			*p = q1;
			return (0x0001);
		}

		if (m23 & p12 & m24)
		{
			// Origin is in Voronoi region for vertex q2.

			*p = q2;
			return (0x0002);
		}

		if (m31 & p23 & m34)
		{
			// Origin is in Voronoi region for vertex q3.

			*p = q3;
			return (0x0004);
		}

		if (p14 & p24 & p34)
		{
			// Origin is in Voronoi region for vertex q4.

			*p = q4;
			return (0x0008);
		}

		Vector3D n1 = v12 % v31;
		Vector3D n2 = v12 % v14;
		Vector3D n3 = v23 % v24;
		Vector3D n4 = v31 % v34;

		bool d11 = !((v12 % n1) * q1 < 0.0F);
		bool d12 = !((v23 % n1) * q2 < 0.0F);
		bool d13 = !((v31 % n1) * q3 < 0.0F);
		bool d21 = !((n2 % v12) * q1 < 0.0F);
		bool d22 = !((n2 % v24) * q2 < 0.0F);
		bool d24 = !((v14 % n2) * q4 < 0.0F);
		bool d32 = !((n3 % v23) * q2 < 0.0F);
		bool d33 = !((n3 % v34) * q3 < 0.0F);
		bool d34 = !((v24 % n3) * q4 < 0.0F);
		bool d43 = !((n4 % v31) * q3 < 0.0F);
		bool d41 = !((n4 % v14) * q1 < 0.0F);
		bool d44 = !((v34 % n4) * q4 < 0.0F);

		if ((!m12) && (!p12) && (d11) && (d21))
		{
			// Origin is in Voronoi region for edge v12.

			*p = q1 - ProjectOnto(q1, v12) / SquaredMag(v12);
			return (0x0003);
		}

		if ((!m23) && (!p23) && (d12) && (d32))
		{
			// Origin is in Voronoi region for edge v23.

			*p = q2 - ProjectOnto(q2, v23) / SquaredMag(v23);
			return (0x0006);
		}

		if ((!m31) && (!p31) && (d13) && (d43))
		{
			// Origin is in Voronoi region for edge v31.

			*p = q3 - ProjectOnto(q3, v31) / SquaredMag(v31);
			return (0x0005);
		}

		if ((!m14) && (!p14) && (d24) && (d41))
		{
			// Origin is in Voronoi region for edge v14.

			*p = q1 - ProjectOnto(q1, v14) / SquaredMag(v14);
			return (0x0009);
		}

		if ((!m24) && (!p24) && (d22) && (d34))
		{
			// Origin is in Voronoi region for edge v24.

			*p = q2 - ProjectOnto(q2, v24) / SquaredMag(v24);
			return (0x000A);
		}

		if ((!m34) && (!p34) && (d33) && (d44))
		{
			// Origin is in Voronoi region for edge v34.

			*p = q3 - ProjectOnto(q3, v34) / SquaredMag(v34);
			return (0x000C);
		}

		bool f1 = !((n1 * q1) * (n1 * v14) < 0.0F);
		bool f2 = !((n2 * q2) * (n2 * v31) > 0.0F);
		bool f3 = !((n3 * q3) * (n3 * v12) > 0.0F);
		bool f4 = !((n4 * q4) * (n4 * v23) > 0.0F);

		if ((f1) && (!d11) && (!d12) && (!d13))
		{
			// Origin is in Voronoi region for face f1.

			*p = ProjectOnto(q1, n1) / SquaredMag(n1);
			return (0x0007);
		}

		if ((f2) && (!d21) && (!d22) && (!d24))
		{
			// Origin is in Voronoi region for face f2.

			*p = ProjectOnto(q2, n2) / SquaredMag(n2);
			return (0x000B);
		}

		if ((f3) && (!d32) && (!d33) && (!d34))
		{
			// Origin is in Voronoi region for face f3.

			*p = ProjectOnto(q3, n3) / SquaredMag(n3);
			return (0x000E);
		}

		if ((f4) && (!d43) && (!d41) && (!d44))
		{
			// Origin is in Voronoi region for face f4.

			*p = ProjectOnto(q4, n4) / SquaredMag(n4);
			return (0x000D);
		}

		// Origin is in tetrahedron interior.

		p->Set(0.0F, 0.0F, 0.0F);
		return (0x000F);

	#endif
}

bool Shape::StaticIntersectShape(const Shape *betaShape, RigidBodyContact *rigidBodyContact, IntersectionData *intersectionData) const
{
	int32		simplexVertexCount;
	Point3D		alphaVertex[2][4];
	Point3D		betaVertex[2][4];
	Point3D		deltaVertex[2][4];
	Point3D		prevMinPoint;

	const RigidBodyController *alphaBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());
	const RigidBodyController *betaBody = static_cast<RigidBodyController *>(betaShape->GetSuperNode()->GetController());

	Transform4D alphaTransform = alphaBody->GetFinalTransform() * GetNodeTransform();
	Transform4D betaTransform = betaBody->GetFinalTransform() * betaShape->GetNodeTransform();
	Transform4D alphaInverseTransform = Adjugate(alphaTransform);
	Transform4D betaInverseTransform = Adjugate(betaTransform);

	const ShapeObject *alphaObject = GetObject();
	const ShapeObject *betaObject = betaShape->GetObject();

	if (rigidBodyContact)
	{
		simplexVertexCount = rigidBodyContact->cachedSimplexVertexCount;
		if (simplexVertexCount != 0)
		{
			for (machine a = 0; a < simplexVertexCount; a++)
			{
				alphaVertex[0][a] = rigidBodyContact->cachedAlphaVertex[a];
				betaVertex[0][a] = rigidBodyContact->cachedBetaVertex[a];
				deltaVertex[0][a] = alphaTransform * alphaVertex[0][a] - betaTransform * betaVertex[0][a];
			}

			prevMinPoint = deltaVertex[0][0];
			goto start;
		}
	}

	simplexVertexCount = 1;
	alphaVertex[0][0] = alphaObject->GetInitialSupportPoint();
	betaVertex[0][0] = betaObject->GetInitialSupportPoint();
	deltaVertex[0][0] = alphaTransform * alphaVertex[0][0] - betaTransform * betaVertex[0][0];
	prevMinPoint = deltaVertex[0][0];

	start:
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		alphaVertex[simplexParity][simplexVertexCount] = alphaObject->CalculateSupportPoint(alphaInverseTransform * -direction);
		betaVertex[simplexParity][simplexVertexCount] = betaObject->CalculateSupportPoint(betaInverseTransform * direction);
		deltaVertex[simplexParity][simplexVertexCount] = alphaTransform * alphaVertex[simplexParity][simplexVertexCount] - betaTransform * betaVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				alphaVertex[simplexParity ^ 1][simplexVertexCount] = alphaVertex[simplexParity][index];
				betaVertex[simplexParity ^ 1][simplexVertexCount] = betaVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;

			if (simplexVertexCount == 1)
			{
				c1 = alphaTransform * alphaVertex[simplexParity][0];
				c2 = betaTransform * betaVertex[simplexParity][0];
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = alphaTransform * (alphaVertex[simplexParity][0] * w1 + alphaVertex[simplexParity][1] * w2);
				c2 = betaTransform * (betaVertex[simplexParity][0] * w1 + betaVertex[simplexParity][1] * w2);
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = alphaTransform * (alphaVertex[simplexParity][0] * w1 + alphaVertex[simplexParity][1] * w2 + alphaVertex[simplexParity][2] * w3);
				c2 = betaTransform * (betaVertex[simplexParity][0] * w1 + betaVertex[simplexParity][1] * w2 + betaVertex[simplexParity][2] * w3);
			}

			if (rigidBodyContact)
			{
				rigidBodyContact->cachedSimplexVertexCount = simplexVertexCount;
				for (machine a = 0; a < simplexVertexCount; a++)
				{
					rigidBodyContact->cachedAlphaVertex[a] = alphaVertex[simplexParity][a];
					rigidBodyContact->cachedBetaVertex[a] = betaVertex[simplexParity][a];
				}
			}

			float shrink1 = alphaObject->GetShapeShrinkSize();
			float shrink2 = betaObject->GetShapeShrinkSize();

			Vector3D normal = c2 - c1;
			float m2 = SquaredMag(normal);
			if (m2 > kShapeSeparationEpsilon)
			{
				float shrinkSum = shrink1 + shrink2;
				if (m2 < shrinkSum * shrinkSum)
				{
					normal *= InverseSqrt(m2);

					intersectionData->alphaContact = c1 + normal * shrink1;
					intersectionData->betaContact = c2 - normal * shrink2;
					intersectionData->contactNormal = normal;
					intersectionData->contactParam = 1.0F;
					return (true);
				}

				return (false);
			}

			break;
		}

		prevMinPoint = minPoint;
	}

	if (rigidBodyContact)
	{
		rigidBodyContact->cachedSimplexVertexCount = 0;
	}

	return (FindMinimumShapePenetration(betaShape, alphaTransform, alphaInverseTransform, betaTransform, betaInverseTransform, intersectionData));
}

bool Shape::StaticIntersectShape(const Shape *betaShape, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const
{
	Point3D		alphaVertex[2][4];
	Point3D		betaVertex[2][4];
	Point3D		deltaVertex[2][4];

	const RigidBodyController *alphaBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());
	const RigidBodyController *betaBody = static_cast<RigidBodyController *>(betaShape->GetSuperNode()->GetController());

	Transform4D alphaTransform = alphaBody->GetFinalTransform() * GetNodeTransform();
	Transform4D betaTransform = betaBody->GetFinalTransform() * betaShape->GetNodeTransform();
	alphaTransform[3] -= worldOffset;
	Transform4D alphaInverseTransform = Adjugate(alphaTransform);
	Transform4D betaInverseTransform = Adjugate(betaTransform);

	const ShapeObject *alphaObject = GetObject();
	const ShapeObject *betaObject = betaShape->GetObject();

	alphaVertex[0][0] = alphaObject->GetInitialSupportPoint();
	betaVertex[0][0] = betaObject->GetInitialSupportPoint();
	deltaVertex[0][0] = alphaTransform * alphaVertex[0][0] - betaTransform * betaVertex[0][0];
	Point3D prevMinPoint = deltaVertex[0][0];

	int32 simplexVertexCount = 1;
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		alphaVertex[simplexParity][simplexVertexCount] = alphaObject->CalculateSupportPoint(alphaInverseTransform * -direction);
		betaVertex[simplexParity][simplexVertexCount] = betaObject->CalculateSupportPoint(betaInverseTransform * direction);
		deltaVertex[simplexParity][simplexVertexCount] = alphaTransform * alphaVertex[simplexParity][simplexVertexCount] - betaTransform * betaVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				alphaVertex[simplexParity ^ 1][simplexVertexCount] = alphaVertex[simplexParity][index];
				betaVertex[simplexParity ^ 1][simplexVertexCount] = betaVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;
			int32		dim1, dim2;

			if (simplexVertexCount == 1)
			{
				c1 = alphaTransform * alphaVertex[simplexParity][0];
				c2 = betaTransform * betaVertex[simplexParity][0];

				dim1 = 0;
				dim2 = 0;
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = alphaTransform * (alphaVertex[simplexParity][0] * w1 + alphaVertex[simplexParity][1] * w2);
				c2 = betaTransform * (betaVertex[simplexParity][0] * w1 + betaVertex[simplexParity][1] * w2);

				dim1 = (SquaredMag(alphaVertex[simplexParity][0] - alphaVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(betaVertex[simplexParity][0] - betaVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = alphaTransform * (alphaVertex[simplexParity][0] * w1 + alphaVertex[simplexParity][1] * w2 + alphaVertex[simplexParity][2] * w3);
				c2 = betaTransform * (betaVertex[simplexParity][0] * w1 + betaVertex[simplexParity][1] * w2 + betaVertex[simplexParity][2] * w3);

				dim1 = (SquaredMag(alphaVertex[simplexParity][0] - alphaVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(alphaVertex[simplexParity][0] - alphaVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(betaVertex[simplexParity][0] - betaVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(betaVertex[simplexParity][0] - betaVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
			}

			Vector3D normal = Normalize(c2 - c1);

			intersectionData->alphaContact = c1 + normal * alphaObject->GetShapeShrinkSize() + worldOffset;
			intersectionData->betaContact = c2 - normal * betaObject->GetShapeShrinkSize();
			intersectionData->contactNormal = normal;
			intersectionData->contactParam = 1.0F;
			*dimension1 = dim1;
			*dimension2 = dim2;
			return (false);
		}

		prevMinPoint = minPoint;
	}

	return (true);
}

bool Shape::FindMinimumShapePenetration(const Shape *betaShape, const Transform4D& alphaTransform, const Transform4D& alphaInverseTransform, const Transform4D& betaTransform, const Transform4D& betaInverseTransform, IntersectionData *intersectionData) const
{
	int32		dim1, dim2;
	Vector3D	alphaDirection[kPenetrationDirectionCount + 12];
	Vector3D	betaDirection[kPenetrationDirectionCount + 12];
	Point3D		alphaSupport[kPenetrationDirectionCount + 12];
	Point3D		betaSupport[kPenetrationDirectionCount + 12];
	Point3D		deltaSupport[kPenetrationDirectionCount + 12];

	for (machine a = 0; a < kPenetrationDirectionCount; a++)
	{
		alphaDirection[a] = alphaInverseTransform * penetrationDirectionTable[a];
		betaDirection[a] = -(betaInverseTransform * penetrationDirectionTable[a]);
	}

	alphaDirection[kPenetrationDirectionCount].Set(1.0F, 0.0F, 0.0F);
	alphaDirection[kPenetrationDirectionCount + 1].Set(0.0F, 1.0F, 0.0F);
	alphaDirection[kPenetrationDirectionCount + 2].Set(0.0F, 0.0F, 1.0F);
	alphaDirection[kPenetrationDirectionCount + 3].Set(-1.0F, 0.0F, 0.0F);
	alphaDirection[kPenetrationDirectionCount + 4].Set(0.0F, -1.0F, 0.0F);
	alphaDirection[kPenetrationDirectionCount + 5].Set(0.0F, 0.0F, -1.0F);
	alphaDirection[kPenetrationDirectionCount + 6] = alphaInverseTransform * betaTransform[0];
	alphaDirection[kPenetrationDirectionCount + 7] = alphaInverseTransform * betaTransform[1];
	alphaDirection[kPenetrationDirectionCount + 8] = alphaInverseTransform * betaTransform[2];
	alphaDirection[kPenetrationDirectionCount + 9] = -alphaDirection[kPenetrationDirectionCount + 6];
	alphaDirection[kPenetrationDirectionCount + 10] = -alphaDirection[kPenetrationDirectionCount + 7];
	alphaDirection[kPenetrationDirectionCount + 11] = -alphaDirection[kPenetrationDirectionCount + 8];

	betaDirection[kPenetrationDirectionCount + 3] = betaInverseTransform * alphaTransform[0];
	betaDirection[kPenetrationDirectionCount + 4] = betaInverseTransform * alphaTransform[1];
	betaDirection[kPenetrationDirectionCount + 5] = betaInverseTransform * alphaTransform[2];
	betaDirection[kPenetrationDirectionCount] = -betaDirection[kPenetrationDirectionCount + 3];
	betaDirection[kPenetrationDirectionCount + 1] = -betaDirection[kPenetrationDirectionCount + 4];
	betaDirection[kPenetrationDirectionCount + 2] = -betaDirection[kPenetrationDirectionCount + 5];
	betaDirection[kPenetrationDirectionCount + 6].Set(-1.0F, 0.0F, 0.0F);
	betaDirection[kPenetrationDirectionCount + 7].Set(0.0F, -1.0F, 0.0F);
	betaDirection[kPenetrationDirectionCount + 8].Set(0.0F, 0.0F, -1.0F);
	betaDirection[kPenetrationDirectionCount + 9].Set(1.0F, 0.0F, 0.0F);
	betaDirection[kPenetrationDirectionCount + 10].Set(0.0F, 1.0F, 0.0F);
	betaDirection[kPenetrationDirectionCount + 11].Set(0.0F, 0.0F, 1.0F);

	GetObject()->CalculateSupportPointArray(kPenetrationDirectionCount + 12, alphaDirection, alphaSupport);
	betaShape->GetObject()->CalculateSupportPointArray(kPenetrationDirectionCount + 12, betaDirection, betaSupport);

	alphaSupport[0] = alphaTransform * alphaSupport[0];
	betaSupport[0] = betaTransform * betaSupport[0];
	deltaSupport[0] = alphaSupport[0] - betaSupport[0];

	float minDelta = Fabs(deltaSupport[0] * penetrationDirectionTable[0]);
	int32 minIndex = 0;

	for (machine a = 1; a < kPenetrationDirectionCount; a++)
	{
		alphaSupport[a] = alphaTransform * alphaSupport[a];
		betaSupport[a] = betaTransform * betaSupport[a];
		deltaSupport[a] = alphaSupport[a] - betaSupport[a];

		float d = Fabs(deltaSupport[a] * penetrationDirectionTable[a]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}
	}

	int32 column = 0;
	for (machine a = kPenetrationDirectionCount; a < kPenetrationDirectionCount + 6; a++)
	{
		alphaSupport[a] = alphaTransform * alphaSupport[a];
		betaSupport[a] = betaTransform * betaSupport[a];
		deltaSupport[a] = alphaSupport[a] - betaSupport[a];

		float d = Fabs(deltaSupport[a] * alphaTransform[column]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}

		column = IncMod<3>(column);
	}

	for (machine a = kPenetrationDirectionCount + 6; a < kPenetrationDirectionCount + 12; a++)
	{
		alphaSupport[a] = alphaTransform * alphaSupport[a];
		betaSupport[a] = betaTransform * betaSupport[a];
		deltaSupport[a] = alphaSupport[a] - betaSupport[a];

		float d = Fabs(deltaSupport[a] * betaTransform[column]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}

		column = IncMod<3>(column);
	}

	Vector3D direction = alphaTransform * alphaDirection[minIndex];
	Vector3D delta = ProjectOnto(deltaSupport[minIndex], direction) * 2.0F;

	if (!StaticIntersectShape(betaShape, intersectionData, &dim1, &dim2, delta))
	{
		if (dim1 < dim2)
		{
			delta = intersectionData->betaContact - intersectionData->alphaContact;
			intersectionData->betaContact = intersectionData->alphaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}
		else if (dim2 < dim1)
		{
			delta = intersectionData->alphaContact - intersectionData->betaContact;
			intersectionData->alphaContact = intersectionData->betaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}

		return (true);
	}

	return (false);
}

bool Shape::DynamicIntersectShape(const Shape *betaShape, const Vector3D& alphaDisplacement, const Vector3D& betaDisplacement, IntersectionData *intersectionData) const
{
	Vector3D displacement = alphaDisplacement - betaDisplacement;
	if (SquaredMag(displacement) > kIntersectionDisplacementEpsilon)
	{
		Point3D		alphaVertex[2][4];
		Point3D		betaVertex[2][4];
		Point3D		deltaVertex[2][4];

		const RigidBodyController *alphaBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());
		const RigidBodyController *betaBody = static_cast<RigidBodyController *>(betaShape->GetSuperNode()->GetController());

		Transform4D alphaTransform = alphaBody->GetFinalTransform() * GetNodeTransform();
		Transform4D betaTransform = betaBody->GetFinalTransform() * betaShape->GetNodeTransform();
		Transform4D alphaInverseTransform = Adjugate(alphaTransform);
		Transform4D betaInverseTransform = Adjugate(betaTransform);

		const ShapeObject *alphaObject = GetObject();
		const ShapeObject *betaObject = betaShape->GetObject();

		float shrink1 = alphaObject->GetShapeShrinkSize();
		float shrink2 = betaObject->GetShapeShrinkSize();
		float shrinkSum = shrink1 + shrink2;
		float minAdvance = shrinkSum * InverseMag(displacement) * 0.5F;
		float reverseAdvance = minAdvance * 0.5F;

		float tmin = 0.0F;
		Vector3D ds(0.0F, 0.0F, 0.0F);

		Point3D initialAlphaVertex = alphaObject->CalculateSupportPoint(alphaInverseTransform * displacement);
		Point3D initialBetaVertex = betaObject->CalculateSupportPoint(betaInverseTransform * -displacement);
		Vector3D initialDeltaVertex = alphaTransform * initialAlphaVertex - betaTransform * initialBetaVertex - displacement;

		start:
		alphaVertex[0][0] = initialAlphaVertex;
		betaVertex[0][0] = initialBetaVertex;
		deltaVertex[0][0] = initialDeltaVertex + ds;
		Point3D prevMinPoint = deltaVertex[0][0];

		int32 simplexVertexCount = 1;
		for (unsigned_int32 simplexParity = 0;;)
		{
			Point3D		minPoint;

			const Vector3D& direction = prevMinPoint;

			alphaVertex[simplexParity][simplexVertexCount] = alphaObject->CalculateSupportPoint(alphaInverseTransform * -direction);
			betaVertex[simplexParity][simplexVertexCount] = betaObject->CalculateSupportPoint(betaInverseTransform * direction);
			deltaVertex[simplexParity][simplexVertexCount] = alphaTransform * alphaVertex[simplexParity][simplexVertexCount] - betaTransform * betaVertex[simplexParity][simplexVertexCount] - displacement;

			float f1 = direction * deltaVertex[simplexParity][simplexVertexCount];
			float f2 = -(direction * displacement);
			if (f1 > (tmin + minAdvance) * f2)
			{
				if (f2 < f1 * K::min_float)
				{
					break;
				}

				if (f2 > Magnitude(direction) * kConservativeAdvancementEpsilon)
				{
					if (f1 > f2)
					{
						return (false);
					}

					tmin = f1 / f2 - reverseAdvance;
					ds = displacement * tmin;
					goto start;
				}
			}

			deltaVertex[simplexParity][simplexVertexCount] += ds;

			for (machine a = 0; a < simplexVertexCount; a++)
			{
				if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
				{
					simplexVertexCount--;
					break;
				}
			}

			unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
			if (mask == 0x0F)
			{
				break;
			}

			simplexVertexCount = 0;
			for (machine index = 0;; index++)
			{
				if (mask & 1)
				{
					alphaVertex[simplexParity ^ 1][simplexVertexCount] = alphaVertex[simplexParity][index];
					betaVertex[simplexParity ^ 1][simplexVertexCount] = betaVertex[simplexParity][index];
					deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
					simplexVertexCount++;
				}

				if ((mask >>= 1) == 0)
				{
					break;
				}
			}

			simplexParity ^= 1;

			if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
			{
				Point3D		c1, c2;

				if (simplexVertexCount == 1)
				{
					c1 = alphaTransform * alphaVertex[simplexParity][0] + (tmin - 1.0F) * alphaDisplacement;
					c2 = betaTransform * betaVertex[simplexParity][0] + (tmin - 1.0F) * betaDisplacement;
				}
				else if (simplexVertexCount == 2)
				{
					const Point3D& p1 = deltaVertex[simplexParity][0];
					const Point3D& p2 = deltaVertex[simplexParity][1];
					float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
					float w2 = 1.0F - w1;

					c1 = alphaTransform * (alphaVertex[simplexParity][0] * w1 + alphaVertex[simplexParity][1] * w2) + (tmin - 1.0F) * alphaDisplacement;
					c2 = betaTransform * (betaVertex[simplexParity][0] * w1 + betaVertex[simplexParity][1] * w2) + (tmin - 1.0F) * betaDisplacement;
				}
				else
				{
					float	w1, w2, w3;

					Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

					c1 = alphaTransform * (alphaVertex[simplexParity][0] * w1 + alphaVertex[simplexParity][1] * w2 + alphaVertex[simplexParity][2] * w3) + (tmin - 1.0F) * alphaDisplacement;
					c2 = betaTransform * (betaVertex[simplexParity][0] * w1 + betaVertex[simplexParity][1] * w2 + betaVertex[simplexParity][2] * w3) + (tmin - 1.0F) * betaDisplacement;
				}

				Vector3D normal = c2 - c1;
				float m2 = SquaredMag(normal);
				if (m2 > kShapeSeparationEpsilon)
				{
					if (m2 < shrinkSum * shrinkSum)
					{
						normal *= InverseSqrt(m2);

						intersectionData->alphaContact = c1 + normal * shrink1;
						intersectionData->betaContact = c2 - normal * shrink2;
						intersectionData->contactNormal = normal;
						intersectionData->contactParam = tmin;
						return (true);
					}

					return (false);
				}

				break;
			}

			prevMinPoint = minPoint;
		}
	}

	return (StaticIntersectShape(betaShape, nullptr, intersectionData));
}

bool Shape::StaticIntersectPrimitive(const PrimitiveGeometry *geometry, GeometryContact *geometryContact, IntersectionData *intersectionData) const
{
	int32		simplexVertexCount;
	Point3D		shapeVertex[2][4];
	Point3D		geometryVertex[2][4];
	Point3D		deltaVertex[2][4];
	Point3D		prevMinPoint;

	const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());

	Transform4D shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
	const Transform4D& geometryTransform = geometry->GetWorldTransform();
	Matrix3D shapeInverseMatrix3D = Adjugate3D(shapeTransform);
	const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

	const ShapeObject *shapeObject = GetObject();
	const PrimitiveGeometryObject *geometryObject = geometry->GetObject();

	if (geometryContact)
	{
		simplexVertexCount = geometryContact->cachedSimplexVertexCount;
		if (simplexVertexCount != 0)
		{
			for (machine a = 0; a < simplexVertexCount; a++)
			{
				shapeVertex[0][a] = geometryContact->cachedAlphaVertex[a];
				geometryVertex[0][a] = geometryContact->cachedBetaVertex[a];
				deltaVertex[0][a] = shapeTransform * shapeVertex[0][a] - geometryTransform * geometryVertex[0][a];
			}

			prevMinPoint = deltaVertex[0][0];
			goto start;
		}
	}

	simplexVertexCount = 1;
	shapeVertex[0][0] = shapeObject->GetInitialSupportPoint();
	geometryVertex[0][0] = geometryObject->GetInitialPrimitiveSupportPoint();
	deltaVertex[0][0] = shapeTransform * shapeVertex[0][0] - geometryTransform * geometryVertex[0][0];
	prevMinPoint = deltaVertex[0][0];

	start:
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * -direction);
		geometryVertex[simplexParity][simplexVertexCount] = geometryObject->CalculatePrimitiveSupportPoint(geometryInverseTransform * direction);
		deltaVertex[simplexParity][simplexVertexCount] = shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryTransform * geometryVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
				geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;

			if (simplexVertexCount == 1)
			{
				c1 = shapeTransform * shapeVertex[simplexParity][0];
				c2 = geometryTransform * geometryVertex[simplexParity][0];
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2);
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3);
			}

			if (geometryContact)
			{
				geometryContact->cachedSimplexVertexCount = simplexVertexCount;
				for (machine a = 0; a < simplexVertexCount; a++)
				{
					geometryContact->cachedAlphaVertex[a] = shapeVertex[simplexParity][a];
					geometryContact->cachedBetaVertex[a] = geometryVertex[simplexParity][a];
				}
			}

			float shrink = shapeObject->GetShapeShrinkSize();

			Vector3D normal = c2 - c1;
			float m2 = SquaredMag(normal);
			if (m2 > kShapeSeparationEpsilon)
			{
				if (m2 < shrink * shrink)
				{
					normal *= InverseSqrt(m2);

					intersectionData->alphaContact = c1 + normal * shrink;
					intersectionData->betaContact = c2;
					intersectionData->contactNormal = normal;
					intersectionData->contactParam = 1.0F;
					intersectionData->triangleIndex = 0;
					return (true);
				}

				return (false);
			}

			break;
		}

		prevMinPoint = minPoint;
	}

	if (geometryContact)
	{
		geometryContact->cachedSimplexVertexCount = 0;
	}

	return (FindMinimumPrimitivePenetration(geometry, shapeTransform, shapeInverseMatrix3D, intersectionData));
}

bool Shape::StaticIntersectPrimitive(const PrimitiveGeometry *geometry, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const
{
	Point3D		shapeVertex[2][4];
	Point3D		geometryVertex[2][4];
	Point3D		deltaVertex[2][4];

	const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());

	Transform4D shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
	const Transform4D& geometryTransform = geometry->GetWorldTransform();
	shapeTransform[3] -= worldOffset;
	Matrix3D shapeInverseMatrix3D = Adjugate3D(shapeTransform);
	const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

	const ShapeObject *shapeObject = GetObject();
	const PrimitiveGeometryObject *geometryObject = geometry->GetObject();

	shapeVertex[0][0] = shapeObject->GetInitialSupportPoint();
	geometryVertex[0][0] = geometryObject->GetInitialPrimitiveSupportPoint();
	deltaVertex[0][0] = shapeTransform * shapeVertex[0][0] - geometryTransform * geometryVertex[0][0];
	Point3D prevMinPoint = deltaVertex[0][0];

	int32 simplexVertexCount = 1;
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * -direction);
		geometryVertex[simplexParity][simplexVertexCount] = geometryObject->CalculatePrimitiveSupportPoint(geometryInverseTransform * direction);
		deltaVertex[simplexParity][simplexVertexCount] = shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryTransform * geometryVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
				geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;
			int32		dim1, dim2;

			if (simplexVertexCount == 1)
			{
				c1 = shapeTransform * shapeVertex[simplexParity][0];
				c2 = geometryTransform * geometryVertex[simplexParity][0];

				dim1 = 0;
				dim2 = 0;
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2);

				dim1 = (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3);

				dim1 = (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
			}

			Vector3D normal = Normalize(c2 - c1);

			intersectionData->alphaContact = c1 + normal * shapeObject->GetShapeShrinkSize() + worldOffset;
			intersectionData->betaContact = c2;
			intersectionData->contactNormal = normal;
			intersectionData->contactParam = 1.0F;
			intersectionData->triangleIndex = 0;
			*dimension1 = dim1;
			*dimension2 = dim2;
			return (false);
		}

		prevMinPoint = minPoint;
	}

	return (true);
}

bool Shape::FindMinimumPrimitivePenetration(const PrimitiveGeometry *geometry, const Transform4D& shapeTransform, const Matrix3D& shapeInverseMatrix3D, IntersectionData *intersectionData) const
{
	int32		dim1, dim2;
	Vector3D	shapeDirection[kPenetrationDirectionCount + 12];
	Vector3D	geometryDirection[kPenetrationDirectionCount + 12];
	Point3D		shapeSupport[kPenetrationDirectionCount + 12];
	Point3D		geometrySupport[kPenetrationDirectionCount + 12];
	Point3D		deltaSupport[kPenetrationDirectionCount + 12];

	const Transform4D& geometryTransform = geometry->GetWorldTransform();
	const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

	for (machine a = 0; a < kPenetrationDirectionCount; a++)
	{
		shapeDirection[a] = shapeInverseMatrix3D * penetrationDirectionTable[a];
		geometryDirection[a] = -(geometryInverseTransform * penetrationDirectionTable[a]);
	}

	shapeDirection[kPenetrationDirectionCount].Set(1.0F, 0.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 1].Set(0.0F, 1.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 2].Set(0.0F, 0.0F, 1.0F);
	shapeDirection[kPenetrationDirectionCount + 3].Set(-1.0F, 0.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 4].Set(0.0F, -1.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 5].Set(0.0F, 0.0F, -1.0F);
	shapeDirection[kPenetrationDirectionCount + 6] = shapeInverseMatrix3D * geometryTransform[0];
	shapeDirection[kPenetrationDirectionCount + 7] = shapeInverseMatrix3D * geometryTransform[1];
	shapeDirection[kPenetrationDirectionCount + 8] = shapeInverseMatrix3D * geometryTransform[2];
	shapeDirection[kPenetrationDirectionCount + 9] = -shapeDirection[kPenetrationDirectionCount + 6];
	shapeDirection[kPenetrationDirectionCount + 10] = -shapeDirection[kPenetrationDirectionCount + 7];
	shapeDirection[kPenetrationDirectionCount + 11] = -shapeDirection[kPenetrationDirectionCount + 8];

	geometryDirection[kPenetrationDirectionCount + 3] = geometryInverseTransform * shapeTransform[0];
	geometryDirection[kPenetrationDirectionCount + 4] = geometryInverseTransform * shapeTransform[1];
	geometryDirection[kPenetrationDirectionCount + 5] = geometryInverseTransform * shapeTransform[2];
	geometryDirection[kPenetrationDirectionCount] = -geometryDirection[kPenetrationDirectionCount + 3];
	geometryDirection[kPenetrationDirectionCount + 1] = -geometryDirection[kPenetrationDirectionCount + 4];
	geometryDirection[kPenetrationDirectionCount + 2] = -geometryDirection[kPenetrationDirectionCount + 5];
	geometryDirection[kPenetrationDirectionCount + 6].Set(-1.0F, 0.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 7].Set(0.0F, -1.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 8].Set(0.0F, 0.0F, -1.0F);
	geometryDirection[kPenetrationDirectionCount + 9].Set(1.0F, 0.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 10].Set(0.0F, 1.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 11].Set(0.0F, 0.0F, 1.0F);

	GetObject()->CalculateSupportPointArray(kPenetrationDirectionCount + 12, shapeDirection, shapeSupport);
	geometry->GetObject()->CalculatePrimitiveSupportPointArray(kPenetrationDirectionCount + 12, geometryDirection, geometrySupport);

	shapeSupport[0] = shapeTransform * shapeSupport[0];
	geometrySupport[0] = geometryTransform * geometrySupport[0];
	deltaSupport[0] = shapeSupport[0] - geometrySupport[0];

	float minDelta = Fabs(deltaSupport[0] * penetrationDirectionTable[0]);
	int32 minIndex = 0;

	for (machine a = 1; a < kPenetrationDirectionCount; a++)
	{
		shapeSupport[a] = shapeTransform * shapeSupport[a];
		geometrySupport[a] = geometryTransform * geometrySupport[a];
		deltaSupport[a] = shapeSupport[a] - geometrySupport[a];

		float d = Fabs(deltaSupport[a] * penetrationDirectionTable[a]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}
	}

	int32 column = 0;
	for (machine a = kPenetrationDirectionCount; a < kPenetrationDirectionCount + 6; a++)
	{
		shapeSupport[a] = shapeTransform * shapeSupport[a];
		geometrySupport[a] = geometryTransform * geometrySupport[a];
		deltaSupport[a] = shapeSupport[a] - geometrySupport[a];

		float d = Fabs(deltaSupport[a] * shapeTransform[column]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}

		column = IncMod<3>(column);
	}

	for (machine a = kPenetrationDirectionCount + 6; a < kPenetrationDirectionCount + 12; a++)
	{
		shapeSupport[a] = shapeTransform * shapeSupport[a];
		geometrySupport[a] = geometryTransform * geometrySupport[a];
		deltaSupport[a] = shapeSupport[a] - geometrySupport[a];

		float d = Fabs(deltaSupport[a] * geometryTransform[column]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}

		column = IncMod<3>(column);
	}

	Vector3D direction = shapeTransform * shapeDirection[minIndex];
	Vector3D delta = ProjectOnto(deltaSupport[minIndex], direction) * 2.0F;

	if (!StaticIntersectPrimitive(geometry, intersectionData, &dim1, &dim2, delta))
	{
		if (dim1 < dim2)
		{
			delta = intersectionData->betaContact - intersectionData->alphaContact;
			intersectionData->betaContact = intersectionData->alphaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}
		else if (dim2 < dim1)
		{
			delta = intersectionData->alphaContact - intersectionData->betaContact;
			intersectionData->alphaContact = intersectionData->betaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}

		return (true);
	}

	return (false);
}

bool Shape::DynamicIntersectPrimitive(const PrimitiveGeometry *geometry, const Vector3D& displacement, IntersectionData *intersectionData) const
{
	if (SquaredMag(displacement) > kIntersectionDisplacementEpsilon)
	{
		Point3D		shapeVertex[2][4];
		Point3D		geometryVertex[2][4];
		Point3D		deltaVertex[2][4];

		const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());

		Transform4D shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
		shapeTransform[3] -= displacement;
		const Transform4D& geometryTransform = geometry->GetWorldTransform();
		Matrix3D shapeInverseMatrix3D = Adjugate3D(shapeTransform);
		const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

		const ShapeObject *shapeObject = GetObject();
		const PrimitiveGeometryObject *geometryObject = geometry->GetObject();

		float shrink = shapeObject->GetShapeShrinkSize();
		float minAdvance = shrink * InverseMag(displacement) * 0.5F;
		float reverseAdvance = minAdvance * 0.5F;

		float tmin = 0.0F;
		Vector3D ds(0.0F, 0.0F, 0.0F);

		Point3D initialShapeVertex = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * displacement);
		Point3D initialGeometryVertex = geometryObject->CalculatePrimitiveSupportPoint(geometryInverseTransform * -displacement);
		Vector3D initialDeltaVertex = shapeTransform * initialShapeVertex - geometryTransform * initialGeometryVertex;

		start:
		shapeVertex[0][0] = initialShapeVertex;
		geometryVertex[0][0] = initialGeometryVertex;
		deltaVertex[0][0] = initialDeltaVertex + ds;
		Point3D prevMinPoint = deltaVertex[0][0];

		int32 simplexVertexCount = 1;
		for (unsigned_int32 simplexParity = 0;;)
		{
			Point3D		minPoint;

			const Vector3D& direction = prevMinPoint;

			shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * -direction);
			geometryVertex[simplexParity][simplexVertexCount] = geometryObject->CalculatePrimitiveSupportPoint(geometryInverseTransform * direction);
			deltaVertex[simplexParity][simplexVertexCount] = shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryTransform * geometryVertex[simplexParity][simplexVertexCount];

			float f1 = direction * deltaVertex[simplexParity][simplexVertexCount];
			float f2 = -(direction * displacement);
			if (f1 > (tmin + minAdvance) * f2)
			{
				if (f2 < f1 * K::min_float)
				{
					break;
				}

				if (f2 > Magnitude(direction) * kConservativeAdvancementEpsilon)
				{
					if (f1 > f2)
					{
						return (false);
					}

					tmin = f1 / f2 - reverseAdvance;
					ds = displacement * tmin;
					goto start;
				}
			}

			deltaVertex[simplexParity][simplexVertexCount] += ds;

			for (machine a = 0; a < simplexVertexCount; a++)
			{
				if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
				{
					simplexVertexCount--;
					break;
				}
			}

			unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
			if (mask == 0x0F)
			{
				break;
			}

			simplexVertexCount = 0;
			for (machine index = 0;; index++)
			{
				if (mask & 1)
				{
					shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
					geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
					deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
					simplexVertexCount++;
				}

				if ((mask >>= 1) == 0)
				{
					break;
				}
			}

			simplexParity ^= 1;

			if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
			{
				Point3D		c1, c2;

				if (simplexVertexCount == 1)
				{
					c1 = shapeTransform * shapeVertex[simplexParity][0] + ds;
					c2 = geometryTransform * geometryVertex[simplexParity][0];
				}
				else if (simplexVertexCount == 2)
				{
					const Point3D& p1 = deltaVertex[simplexParity][0];
					const Point3D& p2 = deltaVertex[simplexParity][1];
					float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
					float w2 = 1.0F - w1;

					c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2) + ds;
					c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2);
				}
				else
				{
					float	w1, w2, w3;

					Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

					c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3) + ds;
					c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3);
				}

				Vector3D normal = c2 - c1;
				float m2 = SquaredMag(normal);
				if (m2 > kShapeSeparationEpsilon)
				{
					if (m2 < shrink * shrink)
					{
						normal *= InverseSqrt(m2);

						intersectionData->alphaContact = c1 + normal * shrink;
						intersectionData->betaContact = c2;
						intersectionData->contactNormal = normal;
						intersectionData->contactParam = tmin;
						intersectionData->triangleIndex = 0;
						return (true);
					}

					return (false);
				}

				break;
			}

			prevMinPoint = minPoint;
		}
	}

	return (StaticIntersectPrimitive(geometry, nullptr, intersectionData));
}

bool Shape::StaticIntersectConvexHull(const Geometry *geometry, GeometryContact *geometryContact, IntersectionData *intersectionData) const
{
	int32		simplexVertexCount;
	Point3D		shapeVertex[2][4];
	Point3D		geometryVertex[2][4];
	Point3D		deltaVertex[2][4];
	Point3D		prevMinPoint;

	const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());

	Transform4D shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
	const Transform4D& geometryTransform = geometry->GetWorldTransform();
	Matrix3D shapeInverseMatrix3D = Adjugate3D(shapeTransform);
	const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

	const ShapeObject *shapeObject = GetObject();
	const GeometryObject *geometryObject = geometry->GetObject();
	const Point3D *convexHullVertex = geometryObject->GetConvexHullVertexArray();

	if (geometryContact)
	{
		simplexVertexCount = geometryContact->cachedSimplexVertexCount;
		if (simplexVertexCount != 0)
		{
			for (machine a = 0; a < simplexVertexCount; a++)
			{
				shapeVertex[0][a] = geometryContact->cachedAlphaVertex[a];
				geometryVertex[0][a] = geometryContact->cachedBetaVertex[a];
				deltaVertex[0][a] = shapeTransform * shapeVertex[0][a] - geometryTransform * geometryVertex[0][a];
			}

			prevMinPoint = deltaVertex[0][0];
			goto start;
		}
	}

	simplexVertexCount = 1;
	shapeVertex[0][0] = shapeObject->GetInitialSupportPoint();
	geometryVertex[0][0] = geometryObject->GetInitialConvexHullSupportPoint(convexHullVertex);
	deltaVertex[0][0] = shapeTransform * shapeVertex[0][0] - geometryTransform * geometryVertex[0][0];
	prevMinPoint = deltaVertex[0][0];

	start:
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * -direction);
		geometryVertex[simplexParity][simplexVertexCount] = geometryObject->CalculateConvexHullSupportPoint(convexHullVertex, geometryInverseTransform * direction);
		deltaVertex[simplexParity][simplexVertexCount] = shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryTransform * geometryVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
				geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;

			if (simplexVertexCount == 1)
			{
				c1 = shapeTransform * shapeVertex[simplexParity][0];
				c2 = geometryTransform * geometryVertex[simplexParity][0];
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2);
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3);
			}

			if (geometryContact)
			{
				geometryContact->cachedSimplexVertexCount = simplexVertexCount;
				for (machine a = 0; a < simplexVertexCount; a++)
				{
					geometryContact->cachedAlphaVertex[a] = shapeVertex[simplexParity][a];
					geometryContact->cachedBetaVertex[a] = geometryVertex[simplexParity][a];
				}
			}

			float shrink = shapeObject->GetShapeShrinkSize();

			Vector3D normal = c2 - c1;
			float m2 = SquaredMag(normal);
			if (m2 > kShapeSeparationEpsilon)
			{
				if (m2 < shrink * shrink)
				{
					normal *= InverseSqrt(m2);

					intersectionData->alphaContact = c1 + normal * shrink;
					intersectionData->betaContact = c2;
					intersectionData->contactNormal = normal;
					intersectionData->contactParam = 1.0F;
					intersectionData->triangleIndex = 0;
					return (true);
				}

				return (false);
			}

			break;
		}

		prevMinPoint = minPoint;
	}

	if (geometryContact)
	{
		geometryContact->cachedSimplexVertexCount = 0;
	}

	return (FindMinimumConvexHullPenetration(geometry, shapeTransform, shapeInverseMatrix3D, intersectionData));
}

bool Shape::StaticIntersectConvexHull(const Geometry *geometry, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const
{
	Point3D		shapeVertex[2][4];
	Point3D		geometryVertex[2][4];
	Point3D		deltaVertex[2][4];

	const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());

	Transform4D shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
	const Transform4D& geometryTransform = geometry->GetWorldTransform();
	shapeTransform[3] -= worldOffset;
	Matrix3D shapeInverseMatrix3D = Adjugate3D(shapeTransform);
	const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

	const ShapeObject *shapeObject = GetObject();
	const GeometryObject *geometryObject = geometry->GetObject();
	const Point3D *convexHullVertex = geometryObject->GetConvexHullVertexArray();

	shapeVertex[0][0] = shapeObject->GetInitialSupportPoint();
	geometryVertex[0][0] = geometryObject->GetInitialConvexHullSupportPoint(convexHullVertex);
	deltaVertex[0][0] = shapeTransform * shapeVertex[0][0] - geometryTransform * geometryVertex[0][0];
	Point3D prevMinPoint = deltaVertex[0][0];

	int32 simplexVertexCount = 1;
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * -direction);
		geometryVertex[simplexParity][simplexVertexCount] = geometryObject->CalculateConvexHullSupportPoint(convexHullVertex, geometryInverseTransform * direction);
		deltaVertex[simplexParity][simplexVertexCount] = shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryTransform * geometryVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
				geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;
			int32		dim1, dim2;

			if (simplexVertexCount == 1)
			{
				c1 = shapeTransform * shapeVertex[simplexParity][0];
				c2 = geometryTransform * geometryVertex[simplexParity][0];

				dim1 = 0;
				dim2 = 0;
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2);

				dim1 = (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3);
				c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3);

				dim1 = (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
			}

			Vector3D normal = Normalize(c2 - c1);

			intersectionData->alphaContact = c1 + normal * shapeObject->GetShapeShrinkSize() + worldOffset;
			intersectionData->betaContact = c2;
			intersectionData->contactNormal = normal;
			intersectionData->contactParam = 1.0F;
			intersectionData->triangleIndex = 0;
			*dimension1 = dim1;
			*dimension2 = dim2;
			return (false);
		}

		prevMinPoint = minPoint;
	}

	return (true);
}

bool Shape::FindMinimumConvexHullPenetration(const Geometry *geometry, const Transform4D& shapeTransform, const Matrix3D& shapeInverseMatrix3D, IntersectionData *intersectionData) const
{
	int32		dim1, dim2;
	Vector3D	shapeDirection[kPenetrationDirectionCount + 12];
	Vector3D	geometryDirection[kPenetrationDirectionCount + 12];
	Point3D		shapeSupport[kPenetrationDirectionCount + 12];
	Point3D		geometrySupport[kPenetrationDirectionCount + 12];
	Point3D		deltaSupport[kPenetrationDirectionCount + 12];

	const Transform4D& geometryTransform = geometry->GetWorldTransform();
	const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

	for (machine a = 0; a < kPenetrationDirectionCount; a++)
	{
		shapeDirection[a] = shapeInverseMatrix3D * penetrationDirectionTable[a];
		geometryDirection[a] = -(geometryInverseTransform * penetrationDirectionTable[a]);
	}

	shapeDirection[kPenetrationDirectionCount].Set(1.0F, 0.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 1].Set(0.0F, 1.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 2].Set(0.0F, 0.0F, 1.0F);
	shapeDirection[kPenetrationDirectionCount + 3].Set(-1.0F, 0.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 4].Set(0.0F, -1.0F, 0.0F);
	shapeDirection[kPenetrationDirectionCount + 5].Set(0.0F, 0.0F, -1.0F);
	shapeDirection[kPenetrationDirectionCount + 6] = shapeInverseMatrix3D * geometryTransform[0];
	shapeDirection[kPenetrationDirectionCount + 7] = shapeInverseMatrix3D * geometryTransform[1];
	shapeDirection[kPenetrationDirectionCount + 8] = shapeInverseMatrix3D * geometryTransform[2];
	shapeDirection[kPenetrationDirectionCount + 9] = -shapeDirection[kPenetrationDirectionCount + 6];
	shapeDirection[kPenetrationDirectionCount + 10] = -shapeDirection[kPenetrationDirectionCount + 7];
	shapeDirection[kPenetrationDirectionCount + 11] = -shapeDirection[kPenetrationDirectionCount + 8];

	geometryDirection[kPenetrationDirectionCount + 3] = geometryInverseTransform * shapeTransform[0];
	geometryDirection[kPenetrationDirectionCount + 4] = geometryInverseTransform * shapeTransform[1];
	geometryDirection[kPenetrationDirectionCount + 5] = geometryInverseTransform * shapeTransform[2];
	geometryDirection[kPenetrationDirectionCount] = -geometryDirection[kPenetrationDirectionCount + 3];
	geometryDirection[kPenetrationDirectionCount + 1] = -geometryDirection[kPenetrationDirectionCount + 4];
	geometryDirection[kPenetrationDirectionCount + 2] = -geometryDirection[kPenetrationDirectionCount + 5];
	geometryDirection[kPenetrationDirectionCount + 6].Set(-1.0F, 0.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 7].Set(0.0F, -1.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 8].Set(0.0F, 0.0F, -1.0F);
	geometryDirection[kPenetrationDirectionCount + 9].Set(1.0F, 0.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 10].Set(0.0F, 1.0F, 0.0F);
	geometryDirection[kPenetrationDirectionCount + 11].Set(0.0F, 0.0F, 1.0F);

	GetObject()->CalculateSupportPointArray(kPenetrationDirectionCount + 12, shapeDirection, shapeSupport);

	const GeometryObject *geometryObject = geometry->GetObject();
	const Point3D *convexHullVertex = geometryObject->GetConvexHullVertexArray();
	geometryObject->CalculateConvexHullSupportPointArray(convexHullVertex, kPenetrationDirectionCount + 12, geometryDirection, geometrySupport);

	shapeSupport[0] = shapeTransform * shapeSupport[0];
	geometrySupport[0] = geometryTransform * geometrySupport[0];
	deltaSupport[0] = shapeSupport[0] - geometrySupport[0];

	float minDelta = Fabs(deltaSupport[0] * penetrationDirectionTable[0]);
	int32 minIndex = 0;

	for (machine a = 1; a < kPenetrationDirectionCount; a++)
	{
		shapeSupport[a] = shapeTransform * shapeSupport[a];
		geometrySupport[a] = geometryTransform * geometrySupport[a];
		deltaSupport[a] = shapeSupport[a] - geometrySupport[a];

		float d = Fabs(deltaSupport[a] * penetrationDirectionTable[a]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}
	}

	int32 column = 0;
	for (machine a = kPenetrationDirectionCount; a < kPenetrationDirectionCount + 6; a++)
	{
		shapeSupport[a] = shapeTransform * shapeSupport[a];
		geometrySupport[a] = geometryTransform * geometrySupport[a];
		deltaSupport[a] = shapeSupport[a] - geometrySupport[a];

		float d = Fabs(deltaSupport[a] * shapeTransform[column]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}

		column = IncMod<3>(column);
	}

	for (machine a = kPenetrationDirectionCount + 6; a < kPenetrationDirectionCount + 12; a++)
	{
		shapeSupport[a] = shapeTransform * shapeSupport[a];
		geometrySupport[a] = geometryTransform * geometrySupport[a];
		deltaSupport[a] = shapeSupport[a] - geometrySupport[a];

		float d = Fabs(deltaSupport[a] * geometryTransform[column]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}

		column = IncMod<3>(column);
	}

	Vector3D direction = shapeTransform * shapeDirection[minIndex];
	Vector3D delta = ProjectOnto(deltaSupport[minIndex], direction) * 2.0F;

	if (!StaticIntersectConvexHull(geometry, intersectionData, &dim1, &dim2, delta))
	{
		if (dim1 < dim2)
		{
			delta = intersectionData->betaContact - intersectionData->alphaContact;
			intersectionData->betaContact = intersectionData->alphaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}
		else if (dim2 < dim1)
		{
			delta = intersectionData->alphaContact - intersectionData->betaContact;
			intersectionData->alphaContact = intersectionData->betaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}

		return (true);
	}

	return (false);
}

bool Shape::DynamicIntersectConvexHull(const Geometry *geometry, const Vector3D& displacement, IntersectionData *intersectionData) const
{
	if (SquaredMag(displacement) > kIntersectionDisplacementEpsilon)
	{
		Point3D		shapeVertex[2][4];
		Point3D		geometryVertex[2][4];
		Point3D		deltaVertex[2][4];

		const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());

		Transform4D shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
		shapeTransform[3] -= displacement;
		const Transform4D& geometryTransform = geometry->GetWorldTransform();
		Matrix3D shapeInverseMatrix3D = Adjugate3D(shapeTransform);
		const Transform4D& geometryInverseTransform = geometry->GetInverseWorldTransform();

		const ShapeObject *shapeObject = GetObject();
		const GeometryObject *geometryObject = geometry->GetObject();
		const Point3D *convexHullVertex = geometryObject->GetConvexHullVertexArray();

		float shrink = shapeObject->GetShapeShrinkSize();
		float minAdvance = shrink * InverseMag(displacement) * 0.5F;
		float reverseAdvance = minAdvance * 0.5F;

		float tmin = 0.0F;
		Vector3D ds(0.0F, 0.0F, 0.0F);

		Point3D initialShapeVertex = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * displacement);
		Point3D initialGeometryVertex = geometryObject->CalculateConvexHullSupportPoint(convexHullVertex, geometryInverseTransform * -displacement);
		Vector3D initialDeltaVertex = shapeTransform * initialShapeVertex - geometryTransform * initialGeometryVertex;

		start:
		shapeVertex[0][0] = initialShapeVertex;
		geometryVertex[0][0] = initialGeometryVertex;
		deltaVertex[0][0] = initialDeltaVertex + ds;
		Point3D prevMinPoint = deltaVertex[0][0];

		int32 simplexVertexCount = 1;
		for (unsigned_int32 simplexParity = 0;;)
		{
			Point3D		minPoint;

			const Vector3D& direction = prevMinPoint;

			shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(shapeInverseMatrix3D * -direction);
			geometryVertex[simplexParity][simplexVertexCount] = geometryObject->CalculateConvexHullSupportPoint(convexHullVertex, geometryInverseTransform * direction);
			deltaVertex[simplexParity][simplexVertexCount] = shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryTransform * geometryVertex[simplexParity][simplexVertexCount];

			float f1 = direction * deltaVertex[simplexParity][simplexVertexCount];
			float f2 = -(direction * displacement);
			if (f1 > (tmin + minAdvance) * f2)
			{
				if (f2 < f1 * K::min_float)
				{
					break;
				}

				if (f2 > Magnitude(direction) * kConservativeAdvancementEpsilon)
				{
					if (f1 > f2)
					{
						return (false);
					}

					tmin = f1 / f2 - reverseAdvance;
					ds = displacement * tmin;
					goto start;
				}
			}

			deltaVertex[simplexParity][simplexVertexCount] += ds;

			for (machine a = 0; a < simplexVertexCount; a++)
			{
				if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
				{
					simplexVertexCount--;
					break;
				}
			}

			unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
			if (mask == 0x0F)
			{
				break;
			}

			simplexVertexCount = 0;
			for (machine index = 0;; index++)
			{
				if (mask & 1)
				{
					shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
					geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
					deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
					simplexVertexCount++;
				}

				if ((mask >>= 1) == 0)
				{
					break;
				}
			}

			simplexParity ^= 1;

			if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
			{
				Point3D		c1, c2;

				if (simplexVertexCount == 1)
				{
					c1 = shapeTransform * shapeVertex[simplexParity][0] + ds;
					c2 = geometryTransform * geometryVertex[simplexParity][0];
				}
				else if (simplexVertexCount == 2)
				{
					const Point3D& p1 = deltaVertex[simplexParity][0];
					const Point3D& p2 = deltaVertex[simplexParity][1];
					float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
					float w2 = 1.0F - w1;

					c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2) + ds;
					c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2);
				}
				else
				{
					float	w1, w2, w3;

					Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

					c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3) + ds;
					c2 = geometryTransform * (geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3);
				}

				Vector3D normal = c2 - c1;
				float m2 = SquaredMag(normal);
				if (m2 > kShapeSeparationEpsilon)
				{
					if (m2 < shrink * shrink)
					{
						normal *= InverseSqrt(m2);

						intersectionData->alphaContact = c1 + normal * shrink;
						intersectionData->betaContact = c2;
						intersectionData->contactNormal = normal;
						intersectionData->contactParam = tmin;
						intersectionData->triangleIndex = 0;
						return (true);
					}

					return (false);
				}

				break;
			}

			prevMinPoint = minPoint;
		}
	}

	return (StaticIntersectConvexHull(geometry, nullptr, intersectionData));
}

unsigned_int32 Shape::CalculateTriangleSupportPoint(const Point3D *vertex, const Vector3D& direction)
{
	#if C4SIMD

		vec_float u = VecLoadUnaligned(&direction.x);

		vec_float d1 = VecDot3D(VecLoadUnaligned(&vertex[0].x), u);
		vec_float d2 = VecDot3D(VecLoadUnaligned(&vertex[1].x), u);
		vec_float d3 = VecDot3D(VecLoadUnaligned(&vertex[2].x), u);

		bool m1 = VecCmpgtScalar(d2, d1);
		bool m2 = VecCmpgtScalar(d3, d1);
		bool m3 = VecCmpgtScalar(d3, d2);

	#else

		float d1 = vertex[0] * direction;
		float d2 = vertex[1] * direction;
		float d3 = vertex[2] * direction;

		bool m1 = (d2 > d1);
		bool m2 = (d3 > d1);
		bool m3 = (d3 > d2);

	#endif

	return ((m1 | m2) + (m2 & m3));
}

void Shape::CalculateTriangleSupportPointArray(const Point3D *vertex, int32 count, const Vector3D *direction, unsigned_int8 *supportIndex)
{
	#if C4SIMD

		vec_float v1 = VecLoadUnaligned(&vertex[0].x);
		vec_float v2 = VecLoadUnaligned(&vertex[1].x);
		vec_float v3 = VecLoadUnaligned(&vertex[2].x);

		for (machine a = 0; a < count; a++)
		{
			vec_float u = VecLoadUnaligned(&direction[a].x);

			vec_float d1 = VecDot3D(v1, u);
			vec_float d2 = VecDot3D(v2, u);
			vec_float d3 = VecDot3D(v3, u);

			bool m1 = VecCmpgtScalar(d2, d1);
			bool m2 = VecCmpgtScalar(d3, d1);
			bool m3 = VecCmpgtScalar(d3, d2);

			supportIndex[a] = (unsigned_int8) ((m1 | m2) + (m2 & m3));
		}

	#else

		for (machine a = 0; a < count; a++)
		{
			const Vector3D& u = direction[a];

			float d1 = vertex[0] * u;
			float d2 = vertex[1] * u;
			float d3 = vertex[2] * u;

			bool m1 = (d2 > d1);
			bool m2 = (d3 > d1);
			bool m3 = (d3 > d2);

			supportIndex[a] = (unsigned_int8) ((m1 | m2) + (m2 & m3));
		}

	#endif
}

bool Shape::StaticIntersectTriangle(const TriangleIntersectionParams *params, IntersectionData *intersectionData) const
{
	Point3D			support[2];
	Point3D			shapeVertex[2][4];
	Point3D			geometryVertex[2][4];
	Point3D			deltaVertex[2][4];
	unsigned_int8	supportIndex[2][4];

	const ShapeObject *shapeObject = GetObject();
	float shrink = shapeObject->GetShapeShrinkSize();

	shapeObject->CalculateOpposingSupportPoints(params->shapeInverseMatrix3D * params->triangleNormal, shapeVertex[0]);
	support[0] = params->shapeTransform * shapeVertex[0][0];
	support[1] = params->shapeTransform * shapeVertex[0][1];
	deltaVertex[0][0] = support[1] - params->triangleVertex[0];

	if (((support[0] - params->triangleVertex[0]) * params->triangleNormal < -shrink) || (deltaVertex[0][0] * params->triangleNormal > shrink))
	{
		return (false);
	}

	Point3D prevMinPoint = deltaVertex[0][0];
	shapeVertex[0][0] = shapeVertex[0][1];
	geometryVertex[0][0] = params->triangleVertex[0];
	supportIndex[0][0] = 0;

	int32 simplexVertexCount = 1;
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		unsigned_int32 vertexIndex = CalculateTriangleSupportPoint(params->triangleVertex, direction);
		supportIndex[simplexParity][simplexVertexCount] = (unsigned_int8) vertexIndex;
		geometryVertex[simplexParity][simplexVertexCount] = params->triangleVertex[vertexIndex];

		shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(params->shapeInverseMatrix3D * -direction);
		deltaVertex[simplexParity][simplexVertexCount] = params->shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
				geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				supportIndex[simplexParity ^ 1][simplexVertexCount] = supportIndex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;

			if (simplexVertexCount == 1)
			{
				c1 = params->shapeTransform * shapeVertex[simplexParity][0];
				c2 = geometryVertex[simplexParity][0];
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = params->shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2);
				c2 = geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2;
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = params->shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3);
				c2 = geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3;
			}

			Vector3D normal = c2 - c1;
			float m2 = SquaredMag(normal);
			if (m2 > kShapeSeparationEpsilon)
			{
				if (m2 < shrink * shrink)
				{
					normal *= InverseSqrt(m2);

					intersectionData->alphaContact = c1 + normal * shrink;
					intersectionData->betaContact = c2;
					intersectionData->contactNormal = normal;
					intersectionData->contactParam = 1.0F;
					return (true);
				}

				return (false);
			}

			break;
		}

		prevMinPoint = minPoint;
	}

	return (FindMinimumTrianglePenetration(params, intersectionData));
}

bool Shape::StaticIntersectTriangle(const TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const
{
	Point3D			shapeVertex[2][4];
	Point3D			geometryVertex[2][4];
	Point3D			deltaVertex[2][4];
	unsigned_int8	supportIndex[2][4];

	Transform4D shapeTransform(params->shapeTransform[0], params->shapeTransform[1], params->shapeTransform[2], params->shapeTransform.GetTranslation() - worldOffset);
	const ShapeObject *shapeObject = GetObject();

	shapeVertex[0][0] = shapeObject->GetInitialSupportPoint();
	geometryVertex[0][0] = params->triangleVertex[0];
	deltaVertex[0][0] = shapeTransform * shapeVertex[0][0] - geometryVertex[0][0];
	Point3D prevMinPoint = deltaVertex[0][0];
	supportIndex[0][0] = 0;

	int32 simplexVertexCount = 1;
	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		unsigned_int32 vertexIndex = CalculateTriangleSupportPoint(params->triangleVertex, direction);
		supportIndex[simplexParity][simplexVertexCount] = (unsigned_int8) vertexIndex;
		geometryVertex[simplexParity][simplexVertexCount] = params->triangleVertex[vertexIndex];

		shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(params->shapeInverseMatrix3D * -direction);
		deltaVertex[simplexParity][simplexVertexCount] = shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - geometryVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
				geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				supportIndex[simplexParity ^ 1][simplexVertexCount] = supportIndex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
		{
			Point3D		c1, c2;
			int32		dim1, dim2;

			if (simplexVertexCount == 1)
			{
				c1 = shapeTransform * shapeVertex[simplexParity][0];
				c2 = geometryVertex[simplexParity][0];

				dim1 = 0;
				dim2 = 0;
			}
			else if (simplexVertexCount == 2)
			{
				const Point3D& p1 = deltaVertex[simplexParity][0];
				const Point3D& p2 = deltaVertex[simplexParity][1];
				float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
				float w2 = 1.0F - w1;

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2);
				c2 = geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2;

				dim1 = (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][1]) > kSimplexDimensionEpsilon);
			}
			else
			{
				float	w1, w2, w3;

				Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

				c1 = shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3);
				c2 = geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3;

				dim1 = (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(shapeVertex[simplexParity][0] - shapeVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
				dim2 = (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][1]) > kSimplexDimensionEpsilon) + (SquaredMag(geometryVertex[simplexParity][0] - geometryVertex[simplexParity][2]) > kSimplexDimensionEpsilon);
			}

			Vector3D normal = Normalize(c2 - c1);

			intersectionData->alphaContact = c1 + normal * shapeObject->GetShapeShrinkSize() + worldOffset;
			intersectionData->betaContact = c2;
			intersectionData->contactNormal = normal;
			intersectionData->contactParam = 1.0F;
			intersectionData->triangleIndex = 0;
			*dimension1 = dim1;
			*dimension2 = dim2;
			return (false);
		}

		prevMinPoint = minPoint;
	}

	return (true);
}

bool Shape::FindMinimumTrianglePenetration(const TriangleIntersectionParams *params, IntersectionData *intersectionData) const
{
	int32			dim1, dim2;
	Vector3D		shapeDirection[kPenetrationDirectionCount + 10];
	Vector3D		geometryDirection[kPenetrationDirectionCount + 10];
	Point3D			shapeSupport[kPenetrationDirectionCount + 10];
	unsigned_int8	geometrySupportIndex[kPenetrationDirectionCount + 10];
	Point3D			deltaSupport[kPenetrationDirectionCount + 10];

	Vector3D edgeNormal1 = (params->triangleVertex[1] - params->triangleVertex[0]) % params->triangleNormal;
	Vector3D edgeNormal2 = (params->triangleVertex[2] - params->triangleVertex[1]) % params->triangleNormal;
	Vector3D edgeNormal3 = (params->triangleVertex[0] - params->triangleVertex[2]) % params->triangleNormal;
	edgeNormal1.Normalize();
	edgeNormal2.Normalize();
	edgeNormal3.Normalize();

	shapeDirection[0] = params->shapeInverseMatrix3D * -params->triangleNormal;
	shapeDirection[1] = params->shapeInverseMatrix3D * -edgeNormal1;
	shapeDirection[2] = params->shapeInverseMatrix3D * -edgeNormal2;
	shapeDirection[3] = params->shapeInverseMatrix3D * -edgeNormal3;

	geometryDirection[0] = params->triangleNormal;
	geometryDirection[1] = edgeNormal1;
	geometryDirection[2] = edgeNormal2;
	geometryDirection[3] = edgeNormal3;

	if (params->triangleNormal.x < 0.0F)
	{
		shapeDirection[4].Set(1.0F, 0.0F, 0.0F);
		geometryDirection[4] = -params->shapeTransform[0];
	}
	else
	{
		shapeDirection[4].Set(-1.0F, 0.0F, 0.0F);
		geometryDirection[4] = params->shapeTransform[0];
	}

	if (params->triangleNormal.y < 0.0F)
	{
		shapeDirection[5].Set(0.0F, 1.0F, 0.0F);
		geometryDirection[5] = -params->shapeTransform[1];
	}
	else
	{
		shapeDirection[5].Set(0.0F, -1.0F, 0.0F);
		geometryDirection[5] = params->shapeTransform[1];
	}

	if (params->triangleNormal.z < 0.0F)
	{
		shapeDirection[6].Set(0.0F, 0.0F, 1.0F);
		geometryDirection[6] = -params->shapeTransform[2];
	}
	else
	{
		shapeDirection[6].Set(0.0F, 0.0F, -1.0F);
		geometryDirection[6] = params->shapeTransform[2];
	}

	int32 directionCount = 7;

	for (machine a = 0; a < kPenetrationDirectionCount; a++)
	{
		const Vector3D& penetrationDirection = penetrationDirectionTable[a];
		if (penetrationDirection * params->triangleNormal < 0.0F)
		{
			shapeDirection[directionCount] = params->shapeInverseMatrix3D * penetrationDirection;
			geometryDirection[directionCount] = -penetrationDirection;
			directionCount++;
		}
	}

	GetObject()->CalculateSupportPointArray(directionCount, shapeDirection, shapeSupport);
	CalculateTriangleSupportPointArray(params->triangleVertex, directionCount, geometryDirection, geometrySupportIndex);

	shapeSupport[0] = params->shapeTransform * shapeSupport[0];
	deltaSupport[0] = shapeSupport[0] - params->triangleVertex[geometrySupportIndex[0]];

	float minDelta = Fabs(deltaSupport[0] * geometryDirection[0]);
	int32 minIndex = 0;

	for (machine a = 1; a < directionCount; a++)
	{
		shapeSupport[a] = params->shapeTransform * shapeSupport[a];
		deltaSupport[a] = shapeSupport[a] - params->triangleVertex[geometrySupportIndex[a]];

		float d = Fabs(deltaSupport[a] * geometryDirection[a]);
		if (d < minDelta)
		{
			minDelta = d;
			minIndex = a;
		}
	}

	Vector3D direction = params->shapeTransform * shapeDirection[minIndex];
	Vector3D delta = ProjectOnto(deltaSupport[minIndex], direction) * 2.0F;

	if (!StaticIntersectTriangle(params, intersectionData, &dim1, &dim2, delta))
	{
		if (dim1 < dim2)
		{
			delta = intersectionData->betaContact - intersectionData->alphaContact;
			intersectionData->betaContact = intersectionData->alphaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}
		else if (dim2 < dim1)
		{
			delta = intersectionData->alphaContact - intersectionData->betaContact;
			intersectionData->alphaContact = intersectionData->betaContact + ProjectOnto(delta, direction);
			intersectionData->contactNormal = Normalize(ProjectOnto(intersectionData->contactNormal, direction));
		}

		return (true);
	}

	return (false);
}

bool Shape::DynamicIntersectTriangle(const Vector3D& displacement, const TriangleIntersectionParams *params, IntersectionData *intersectionData) const
{
	Point3D			shapeVertex[2][4];
	Point3D			geometryVertex[2][4];
	Point3D			deltaVertex[2][4];
	unsigned_int8	supportIndex[2][4];

	if (params->triangleNormal * displacement > 0.0F)
	{
		return (false);
	}

	const ShapeObject *shapeObject = GetObject();

	float shrink = shapeObject->GetShapeShrinkSize();
	float minAdvance = shrink * InverseMag(displacement) * 0.5F;
	float reverseAdvance = minAdvance * 0.5F;

	float tmin = 0.0F;
	Vector3D ds(0.0F, 0.0F, 0.0F);

	Point3D initialShapeVertex = shapeObject->CalculateSupportPoint(params->shapeInverseMatrix3D * -params->triangleNormal);
	Vector3D initialDeltaVertex = params->shapeTransform * initialShapeVertex - displacement - params->triangleVertex[0];
	if (initialDeltaVertex * params->triangleNormal > -shrink)
	{
		start:
		deltaVertex[0][0] = initialDeltaVertex + ds;
		Point3D prevMinPoint = deltaVertex[0][0];

		shapeVertex[0][0] = initialShapeVertex;
		geometryVertex[0][0] = params->triangleVertex[0];
		supportIndex[0][0] = 0;

		int32 simplexVertexCount = 1;
		for (unsigned_int32 simplexParity = 0;;)
		{
			Point3D		minPoint;

			const Vector3D& direction = prevMinPoint;

			int32 vertexIndex = CalculateTriangleSupportPoint(params->triangleVertex, direction);
			supportIndex[simplexParity][simplexVertexCount] = (unsigned_int8) vertexIndex;
			geometryVertex[simplexParity][simplexVertexCount] = params->triangleVertex[vertexIndex];

			shapeVertex[simplexParity][simplexVertexCount] = shapeObject->CalculateSupportPoint(params->shapeInverseMatrix3D * -direction);
			deltaVertex[simplexParity][simplexVertexCount] = params->shapeTransform * shapeVertex[simplexParity][simplexVertexCount] - displacement - geometryVertex[simplexParity][simplexVertexCount];

			float f1 = direction * deltaVertex[simplexParity][simplexVertexCount];
			float f2 = -(direction * displacement);
			if (f1 > (tmin + minAdvance) * f2)
			{
				if (f2 < f1 * K::min_float)
				{
					break;
				}

				if (f2 > Magnitude(direction) * kConservativeAdvancementEpsilon)
				{
					if (f1 > f2 * intersectionData->contactParam)
					{
						return (false);
					}

					tmin = f1 / f2 - reverseAdvance;
					ds = displacement * tmin;
					goto start;
				}
			}

			deltaVertex[simplexParity][simplexVertexCount] += ds;

			for (machine a = 0; a < simplexVertexCount; a++)
			{
				if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < kSimplexVertexEpsilon)
				{
					simplexVertexCount--;
					break;
				}
			}

			unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
			if (mask == 0x0F)
			{
				break;
			}

			simplexVertexCount = 0;
			for (machine index = 0;; index++)
			{
				if (mask & 1)
				{
					shapeVertex[simplexParity ^ 1][simplexVertexCount] = shapeVertex[simplexParity][index];
					geometryVertex[simplexParity ^ 1][simplexVertexCount] = geometryVertex[simplexParity][index];
					deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
					supportIndex[simplexParity ^ 1][simplexVertexCount] = supportIndex[simplexParity][index];
					simplexVertexCount++;
				}

				if ((mask >>= 1) == 0)
				{
					break;
				}
			}

			simplexParity ^= 1;

			if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - kSupportPointTolerance))
			{
				Point3D			c1, c2;
				Vector3D		edgeNormal;
				const Vector3D	*geometryNormal;

				ds -= displacement;
				const Vector3D *normalArray = params->geometryMesh->GetArray<Vector3D>(kArrayNormal);

				if (simplexVertexCount == 1)
				{
					c1 = params->shapeTransform * shapeVertex[simplexParity][0] + ds;
					c2 = geometryVertex[simplexParity][0];

					edgeNormal = *params->geometryTransform * normalArray[params->triangle.index[supportIndex[simplexParity][0]]];
					geometryNormal = &edgeNormal;
				}
				else if (simplexVertexCount == 2)
				{
					const Point3D& p1 = deltaVertex[simplexParity][0];
					const Point3D& p2 = deltaVertex[simplexParity][1];
					float w1 = FmaxZero(Fmin(p1 * (minPoint - p2) / (p1 * p1 - p1 * p2), 1.0F));
					float w2 = 1.0F - w1;

					c1 = params->shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2) + ds;
					c2 = geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2;

					edgeNormal = *params->geometryTransform * (normalArray[params->triangle.index[supportIndex[simplexParity][0]]] * w1 + normalArray[params->triangle.index[supportIndex[simplexParity][1]]] * w2);
					edgeNormal.Normalize();
					geometryNormal = &edgeNormal;
				}
				else
				{
					float	w1, w2, w3;

					Math::CalculateBarycentricCoordinates(deltaVertex[simplexParity][0], deltaVertex[simplexParity][1], deltaVertex[simplexParity][2], minPoint, &w1, &w2, &w3);

					c1 = params->shapeTransform * (shapeVertex[simplexParity][0] * w1 + shapeVertex[simplexParity][1] * w2 + shapeVertex[simplexParity][2] * w3) + ds;
					c2 = geometryVertex[simplexParity][0] * w1 + geometryVertex[simplexParity][1] * w2 + geometryVertex[simplexParity][2] * w3;
					geometryNormal = &params->triangleNormal;
				}

				Vector3D normal = c2 - c1;
				float m2 = SquaredMag(normal);
				if (m2 < shrink * shrink)
				{
					if (m2 > kShapeSeparationEpsilon)
					{
						c1 += normal * (InverseSqrt(m2) * shrink);
						intersectionData->alphaContact = c2 + ProjectOnto(c1 - c2, *geometryNormal);
					}
					else
					{
						intersectionData->alphaContact = c1 - *geometryNormal * shrink;
					}

					intersectionData->betaContact = c2;
					intersectionData->contactNormal = -*geometryNormal;
					intersectionData->contactParam = tmin;
					return (true);
				}

				return (false);
			}

			prevMinPoint = minPoint;
		}
	}

	return (StaticIntersectTriangle(params, intersectionData));
}

int32 Shape::StaticIntersectGeometryOctree(const CollisionOctree *octree, const Box3D& collisionBox, TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 contactCount) const
{
	if (octree->collisionBounds.Intersection(collisionBox))
	{
		const Mesh *geometryMesh = params->geometryMesh;
		const Point3D *geometryVertex = geometryMesh->GetArray<Point3D>(kArrayPosition);
		const Triangle *geometryTriangle = geometryMesh->GetArray<Triangle>(kArrayPrimitive);

		int32 triangleCount = octree->elementCount;
		const unsigned_int16 *triangleIndex = octree->GetIndexArray();

		#if C4SIMD

			vec_float bmin = VecLoadUnaligned(&shapeCollisionBox.min.x);
			vec_float bmax = VecLoadUnaligned(&shapeCollisionBox.max.x);

		#endif

		for (machine a = 0; a < triangleCount; a++)
		{
			unsigned_int32 index = triangleIndex[a];
			const Triangle *triangle = &geometryTriangle[index];

			#if C4SIMD

				vec_float c1 = VecLoad(&(*params->geometryTransform)(0,0));
				vec_float c2 = VecLoad(&(*params->geometryTransform)(0,0), 4);
				vec_float c3 = VecLoad(&(*params->geometryTransform)(0,0), 8);
				vec_float c4 = VecLoad(&(*params->geometryTransform)(0,0), 12);

				vec_float v1 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[0]].x));
				vec_float v2 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[1]].x));
				vec_float v3 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[2]].x));

				vec_float vmin = VecMin(VecMin(v1, v2), v3);
				vec_float vmax = VecMax(VecMax(v1, v2), v3);
				if ((VecCmpgtAny3D(vmin, bmax)) || (VecCmpltAny3D(vmax, bmin)))
				{
					continue;
				}

				VecStore3D(v1, &params->triangleVertex[0].x);
				VecStore3D(v2, &params->triangleVertex[1].x);
				VecStore3D(v3, &params->triangleVertex[2].x);

			#else

				params->triangleVertex[0] = *params->geometryTransform * geometryVertex[triangle->index[0]];
				params->triangleVertex[1] = *params->geometryTransform * geometryVertex[triangle->index[1]];
				params->triangleVertex[2] = *params->geometryTransform * geometryVertex[triangle->index[2]];

				float xmin = Fmin(params->triangleVertex[0].x, params->triangleVertex[1].x, params->triangleVertex[2].x);
				float xmax = Fmax(params->triangleVertex[0].x, params->triangleVertex[1].x, params->triangleVertex[2].x);
				float ymin = Fmin(params->triangleVertex[0].y, params->triangleVertex[1].y, params->triangleVertex[2].y);
				float ymax = Fmax(params->triangleVertex[0].y, params->triangleVertex[1].y, params->triangleVertex[2].y);
				float zmin = Fmin(params->triangleVertex[0].z, params->triangleVertex[1].z, params->triangleVertex[2].z);
				float zmax = Fmax(params->triangleVertex[0].z, params->triangleVertex[1].z, params->triangleVertex[2].z);

				if (!shapeCollisionBox.Intersection(Box3D(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax))))
				{
					continue;
				}

			#endif

			params->triangle = *triangle;
			params->triangleNormal = Normalize((params->triangleVertex[1] - params->triangleVertex[0]) % (params->triangleVertex[2] - params->triangleVertex[0]));

			if (StaticIntersectTriangle(params, &intersectionData[contactCount]))
			{
				intersectionData[contactCount].triangleIndex = index;
				if (++contactCount == kMaxIntersectionContactCount)
				{
					return (kMaxIntersectionContactCount);
				}
			}
		}

		for (machine a = 0; a < 8; a++)
		{
			if (octree->subnodeOffset[a] != 0)
			{
				contactCount = StaticIntersectGeometryOctree(octree->GetSubnode(a), collisionBox, params, intersectionData, contactCount);
				if (contactCount == kMaxIntersectionContactCount)
				{
					return (kMaxIntersectionContactCount);
				}
			}
		}
	}

	return (contactCount);
}

int32 Shape::DynamicIntersectGeometryOctree(const Vector3D& displacement, const CollisionOctree *octree, const Box3D& collisionBox, TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 contactCount) const
{
	if (octree->collisionBounds.Intersection(collisionBox))
	{
		const Mesh *geometryMesh = params->geometryMesh;
		const Point3D *geometryVertex = geometryMesh->GetArray<Point3D>(kArrayPosition);
		const Triangle *geometryTriangle = geometryMesh->GetArray<Triangle>(kArrayPrimitive);

		int32 triangleCount = octree->elementCount;
		const unsigned_int16 *triangleIndex = octree->GetIndexArray();

		#if C4SIMD

			vec_float bmin = VecLoadUnaligned(&shapeCollisionBox.min.x);
			vec_float bmax = VecLoadUnaligned(&shapeCollisionBox.max.x);

		#endif

		for (machine a = 0; a < triangleCount; a++)
		{
			unsigned_int32 index = triangleIndex[a];
			const Triangle *triangle = &geometryTriangle[index];

			#if C4SIMD

				vec_float c1 = VecLoad(&(*params->geometryTransform)(0,0));
				vec_float c2 = VecLoad(&(*params->geometryTransform)(0,0), 4);
				vec_float c3 = VecLoad(&(*params->geometryTransform)(0,0), 8);
				vec_float c4 = VecLoad(&(*params->geometryTransform)(0,0), 12);

				vec_float v1 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[0]].x));
				vec_float v2 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[1]].x));
				vec_float v3 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[2]].x));

				vec_float vmin = VecMin(VecMin(v1, v2), v3);
				vec_float vmax = VecMax(VecMax(v1, v2), v3);
				if ((VecCmpgtAny3D(vmin, bmax)) || (VecCmpltAny3D(vmax, bmin)))
				{
					continue;
				}

				VecStore3D(v1, &params->triangleVertex[0].x);
				VecStore3D(v2, &params->triangleVertex[1].x);
				VecStore3D(v3, &params->triangleVertex[2].x);

			#else

				params->triangleVertex[0] = *params->geometryTransform * geometryVertex[triangle->index[0]];
				params->triangleVertex[1] = *params->geometryTransform * geometryVertex[triangle->index[1]];
				params->triangleVertex[2] = *params->geometryTransform * geometryVertex[triangle->index[2]];

				float xmin = Fmin(params->triangleVertex[0].x, params->triangleVertex[1].x, params->triangleVertex[2].x);
				float xmax = Fmax(params->triangleVertex[0].x, params->triangleVertex[1].x, params->triangleVertex[2].x);
				float ymin = Fmin(params->triangleVertex[0].y, params->triangleVertex[1].y, params->triangleVertex[2].y);
				float ymax = Fmax(params->triangleVertex[0].y, params->triangleVertex[1].y, params->triangleVertex[2].y);
				float zmin = Fmin(params->triangleVertex[0].z, params->triangleVertex[1].z, params->triangleVertex[2].z);
				float zmax = Fmax(params->triangleVertex[0].z, params->triangleVertex[1].z, params->triangleVertex[2].z);

				if (!shapeCollisionBox.Intersection(Box3D(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax))))
				{
					continue;
				}

			#endif

			params->triangle = *triangle;
			params->triangleNormal = Normalize((params->triangleVertex[1] - params->triangleVertex[0]) % (params->triangleVertex[2] - params->triangleVertex[0]));

			if (DynamicIntersectTriangle(displacement, params, &intersectionData[0]))
			{
				intersectionData[0].triangleIndex = index;
				contactCount = 1;
			}
		}

		for (machine a = 0; a < 8; a++)
		{
			if (octree->subnodeOffset[a] != 0)
			{
				contactCount = DynamicIntersectGeometryOctree(displacement, octree->GetSubnode(a), collisionBox, params, intersectionData, contactCount);
				if (contactCount == kMaxIntersectionContactCount)
				{
					return (kMaxIntersectionContactCount);
				}
			}
		}
	}

	return (contactCount);
}

int32 Shape::MixedIntersectGeometryOctree(const Vector3D& displacement, const CollisionOctree *octree, const Box3D& collisionBox, int32 contactTriangleCount, const unsigned_int32 *contactTriangleIndex, TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 contactCount) const
{
	if (octree->collisionBounds.Intersection(collisionBox))
	{
		const Mesh *geometryMesh = params->geometryMesh;
		const Point3D *geometryVertex = geometryMesh->GetArray<Point3D>(kArrayPosition);
		const Triangle *geometryTriangle = geometryMesh->GetArray<Triangle>(kArrayPrimitive);

		int32 triangleCount = octree->elementCount;
		const unsigned_int16 *triangleIndex = octree->GetIndexArray();

		#if C4SIMD

			vec_float bmin = VecLoadUnaligned(&shapeCollisionBox.min.x);
			vec_float bmax = VecLoadUnaligned(&shapeCollisionBox.max.x);

		#endif

		for (machine a = 0; a < triangleCount; a++)
		{
			unsigned_int32 index = triangleIndex[a];
			const Triangle *triangle = &geometryTriangle[index];

			#if C4SIMD

				vec_float c1 = VecLoad(&(*params->geometryTransform)(0,0));
				vec_float c2 = VecLoad(&(*params->geometryTransform)(0,0), 4);
				vec_float c3 = VecLoad(&(*params->geometryTransform)(0,0), 8);
				vec_float c4 = VecLoad(&(*params->geometryTransform)(0,0), 12);

				vec_float v1 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[0]].x));
				vec_float v2 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[1]].x));
				vec_float v3 = VecTransformPoint3D(c1, c2, c3, c4, VecLoadUnaligned(&geometryVertex[triangle->index[2]].x));

				vec_float vmin = VecMin(VecMin(v1, v2), v3);
				vec_float vmax = VecMax(VecMax(v1, v2), v3);
				if ((VecCmpgtAny3D(vmin, bmax)) || (VecCmpltAny3D(vmax, bmin)))
				{
					continue;
				}

				VecStore3D(v1, &params->triangleVertex[0].x);
				VecStore3D(v2, &params->triangleVertex[1].x);
				VecStore3D(v3, &params->triangleVertex[2].x);

			#else

				params->triangleVertex[0] = *params->geometryTransform * geometryVertex[triangle->index[0]];
				params->triangleVertex[1] = *params->geometryTransform * geometryVertex[triangle->index[1]];
				params->triangleVertex[2] = *params->geometryTransform * geometryVertex[triangle->index[2]];

				float xmin = Fmin(params->triangleVertex[0].x, params->triangleVertex[1].x, params->triangleVertex[2].x);
				float xmax = Fmax(params->triangleVertex[0].x, params->triangleVertex[1].x, params->triangleVertex[2].x);
				float ymin = Fmin(params->triangleVertex[0].y, params->triangleVertex[1].y, params->triangleVertex[2].y);
				float ymax = Fmax(params->triangleVertex[0].y, params->triangleVertex[1].y, params->triangleVertex[2].y);
				float zmin = Fmin(params->triangleVertex[0].z, params->triangleVertex[1].z, params->triangleVertex[2].z);
				float zmax = Fmax(params->triangleVertex[0].z, params->triangleVertex[1].z, params->triangleVertex[2].z);

				if (!shapeCollisionBox.Intersection(Box3D(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax))))
				{
					continue;
				}

			#endif

			params->triangle = *triangle;
			params->triangleNormal = Normalize((params->triangleVertex[1] - params->triangleVertex[0]) % (params->triangleVertex[2] - params->triangleVertex[0]));

			for (machine b = 0; b < contactTriangleCount; b++)
			{
				if (contactTriangleIndex[b] == index)
				{
					if (StaticIntersectTriangle(params, &intersectionData[contactCount]))
					{
						intersectionData[contactCount].triangleIndex = index;

						if (++contactCount == kMaxIntersectionContactCount)
						{
							return (kMaxIntersectionContactCount);
						}
					}

					goto next;
				}
			}

			if (DynamicIntersectTriangle(displacement, params, &intersectionData[contactCount]))
			{
				intersectionData[contactCount].triangleIndex = index;

				if (++contactCount == kMaxIntersectionContactCount)
				{
					return (kMaxIntersectionContactCount);
				}
			}

			next:;
		}

		for (machine a = 0; a < 8; a++)
		{
			if (octree->subnodeOffset[a] != 0)
			{
				contactCount = MixedIntersectGeometryOctree(displacement, octree->GetSubnode(a), collisionBox, contactTriangleCount, contactTriangleIndex, params, intersectionData, contactCount);
				if (contactCount == kMaxIntersectionContactCount)
				{
					return (kMaxIntersectionContactCount);
				}
			}
		}
	}

	return (contactCount);
}

int32 Shape::StaticIntersectGeometry(const Geometry *geometry, IntersectionData *intersectionData) const
{
	TriangleIntersectionParams		params;

	const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());
	params.shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
	params.shapeInverseMatrix3D = Adjugate3D(params.shapeTransform);

	const GeometryObject *geometryObject = geometry->GetObject();
	params.geometryObject = geometryObject;
	params.geometryMesh = geometryObject->GetGeometryLevel(geometryObject->GetCollisionLevel());
	params.geometryTransform = &geometry->GetWorldTransform();

	const CollisionOctree *octree = geometryObject->GetCollisionOctree();
	Box3D collisionBox = Transform(shapeCollisionBox, geometry->GetInverseWorldTransform());

	return (StaticIntersectGeometryOctree(octree, collisionBox, &params, intersectionData, 0));
}

int32 Shape::DynamicIntersectGeometry(const Geometry *geometry, const Vector3D& displacement, IntersectionData *intersectionData) const
{
	if (SquaredMag(displacement) > kIntersectionDisplacementEpsilon)
	{
		TriangleIntersectionParams		params;

		const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());
		params.shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
		params.shapeInverseMatrix3D = Adjugate3D(params.shapeTransform);

		const GeometryObject *geometryObject = geometry->GetObject();
		params.geometryObject = geometryObject;
		params.geometryMesh = geometryObject->GetGeometryLevel(geometryObject->GetCollisionLevel());
		params.geometryTransform = &geometry->GetWorldTransform();

		const CollisionOctree *octree = geometryObject->GetCollisionOctree();
		Box3D collisionBox = Transform(shapeCollisionBox, geometry->GetInverseWorldTransform());

		intersectionData[0].contactParam = 1.0F;
		return (DynamicIntersectGeometryOctree(displacement, octree, collisionBox, &params, intersectionData, 0));
	}

	return (StaticIntersectGeometry(geometry, intersectionData));
}

int32 Shape::MixedIntersectGeometry(const Geometry *geometry, const Vector3D& displacement, int32 contactTriangleCount, const unsigned_int32 *contactTriangleIndex, IntersectionData *intersectionData) const
{
	if (SquaredMag(displacement) > kIntersectionDisplacementEpsilon)
	{
		TriangleIntersectionParams		params;

		const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetSuperNode()->GetController());
		params.shapeTransform = rigidBody->GetFinalTransform() * GetNodeTransform();
		params.shapeInverseMatrix3D = Adjugate3D(params.shapeTransform);

		const GeometryObject *geometryObject = geometry->GetObject();
		params.geometryObject = geometryObject;
		params.geometryMesh = geometryObject->GetGeometryLevel(geometryObject->GetCollisionLevel());
		params.geometryTransform = &geometry->GetWorldTransform();

		const CollisionOctree *octree = geometryObject->GetCollisionOctree();
		Box3D collisionBox = Transform(shapeCollisionBox, geometry->GetInverseWorldTransform());

		for (machine a = 0; a < kMaxIntersectionContactCount; a++)
		{
			intersectionData[a].contactParam = 1.0F;
		}

		return (MixedIntersectGeometryOctree(displacement, octree, collisionBox, contactTriangleCount, contactTriangleIndex, &params, intersectionData, 0));
	}

	return (StaticIntersectGeometry(geometry, intersectionData));
}


BoxShape::BoxShape() : Shape(kShapeBox)
{
}

BoxShape::BoxShape(const Vector3D& size) : Shape(kShapeBox)
{
	SetNewObject(new BoxShapeObject(size));
}

BoxShape::BoxShape(const BoxShape& boxShape) : Shape(boxShape)
{
}

BoxShape::~BoxShape()
{
}

Node *BoxShape::Replicate(void) const
{
	return (new BoxShape(*this));
}

bool BoxShape::CalculateBoundingBox(Box3D *box) const
{
	const Vector3D& size = GetObject()->GetBoxSize();

	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(size.x, size.y, size.z);
	return (true);
}


PyramidShape::PyramidShape() : Shape(kShapePyramid)
{
}

PyramidShape::PyramidShape(const Vector2D& size, float height) : Shape(kShapePyramid)
{
	SetNewObject(new PyramidShapeObject(size, height));
}

PyramidShape::PyramidShape(const PyramidShape& pyramidShape) : Shape(pyramidShape)
{
}

PyramidShape::~PyramidShape()
{
}

Node *PyramidShape::Replicate(void) const
{
	return (new PyramidShape(*this));
}

bool PyramidShape::CalculateBoundingBox(Box3D *box) const
{
	const PyramidShapeObject *object = GetObject();
	const Vector2D& size = object->GetPyramidSize();
	float height = object->GetPyramidHeight();

	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(size.x, size.y, height);
	return (true);
}


CylinderShape::CylinderShape() : Shape(kShapeCylinder)
{
}

CylinderShape::CylinderShape(const Vector2D& size, float height) : Shape(kShapeCylinder)
{
	SetNewObject(new CylinderShapeObject(size, height));
}

CylinderShape::CylinderShape(const CylinderShape& cylinderShape) : Shape(cylinderShape)
{
}

CylinderShape::~CylinderShape()
{
}

Node *CylinderShape::Replicate(void) const
{
	return (new CylinderShape(*this));
}

bool CylinderShape::CalculateBoundingBox(Box3D *box) const
{
	const CylinderShapeObject *object = GetObject();
	const Vector2D& size = object->GetCylinderSize();
	float height = object->GetCylinderHeight();

	box->min.Set(-size.x, -size.y, 0.0F);
	box->max.Set(size.x, size.y, height);
	return (true);
}


ConeShape::ConeShape() : Shape(kShapeCone)
{
}

ConeShape::ConeShape(const Vector2D& size, float height) : Shape(kShapeCone)
{
	SetNewObject(new ConeShapeObject(size, height));
}

ConeShape::ConeShape(const ConeShape& coneShape) : Shape(coneShape)
{
}

ConeShape::~ConeShape()
{
}

Node *ConeShape::Replicate(void) const
{
	return (new ConeShape(*this));
}

bool ConeShape::CalculateBoundingBox(Box3D *box) const
{
	const ConeShapeObject *object = GetObject();
	const Vector2D& size = object->GetConeSize();
	float height = object->GetConeHeight();

	box->min.Set(-size.x, -size.y, 0.0F);
	box->max.Set(size.x, size.y, height);
	return (true);
}


SphereShape::SphereShape() : Shape(kShapeSphere)
{
}

SphereShape::SphereShape(const Vector3D& size) : Shape(kShapeSphere)
{
	SetNewObject(new SphereShapeObject(size));
}

SphereShape::SphereShape(const SphereShape& sphereShape) : Shape(sphereShape)
{
}

SphereShape::~SphereShape()
{
}

Node *SphereShape::Replicate(void) const
{
	return (new SphereShape(*this));
}

bool SphereShape::CalculateBoundingBox(Box3D *box) const
{
	const Vector3D& size = GetObject()->GetSphereSize();

	box->min.Set(-size.x, -size.y, -size.z);
	box->max.Set(size.x, size.y, size.z);
	return (true);
}


DomeShape::DomeShape() : Shape(kShapeDome)
{
}

DomeShape::DomeShape(const Vector3D& size) : Shape(kShapeDome)
{
	SetNewObject(new DomeShapeObject(size));
}

DomeShape::DomeShape(const DomeShape& domeShape) : Shape(domeShape)
{
}

DomeShape::~DomeShape()
{
}

Node *DomeShape::Replicate(void) const
{
	return (new DomeShape(*this));
}

bool DomeShape::CalculateBoundingBox(Box3D *box) const
{
	const Vector3D& size = GetObject()->GetDomeSize();

	box->min.Set(-size.x, -size.y, 0.0F);
	box->max.Set(size.x, size.y, size.z);
	return (true);
}


CapsuleShape::CapsuleShape() : Shape(kShapeCapsule)
{
}

CapsuleShape::CapsuleShape(const Vector3D& size, float height) : Shape(kShapeCapsule)
{
	SetNewObject(new CapsuleShapeObject(size, height));
}

CapsuleShape::CapsuleShape(const CapsuleShape& capsuleShape) : Shape(capsuleShape)
{
}

CapsuleShape::~CapsuleShape()
{
}

Node *CapsuleShape::Replicate(void) const
{
	return (new CapsuleShape(*this));
}

bool CapsuleShape::CalculateBoundingBox(Box3D *box) const
{
	const CapsuleShapeObject *object = GetObject();
	const Vector3D& size = object->GetCapsuleSize();
	float height = object->GetCapsuleHeight();

	box->min.Set(-size.x, -size.y, -size.z);
	box->max.Set(size.x, size.y, height + size.z);
	return (true);
}


TruncatedPyramidShape::TruncatedPyramidShape() : Shape(kShapeTruncatedPyramid)
{
}

TruncatedPyramidShape::TruncatedPyramidShape(const Vector2D& size, float height, float ratio) : Shape(kShapeTruncatedPyramid)
{
	SetNewObject(new TruncatedPyramidShapeObject(size, height, ratio));
}

TruncatedPyramidShape::TruncatedPyramidShape(const TruncatedPyramidShape& truncatedPyramidShape) : Shape(truncatedPyramidShape)
{
}

TruncatedPyramidShape::~TruncatedPyramidShape()
{
}

Node *TruncatedPyramidShape::Replicate(void) const
{
	return (new TruncatedPyramidShape(*this));
}

bool TruncatedPyramidShape::CalculateBoundingBox(Box3D *box) const
{
	const TruncatedPyramidShapeObject *object = GetObject();
	const Vector2D& size = object->GetPyramidSize();
	float height = object->GetPyramidHeight();

	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(size.x, size.y, height);
	return (true);
}


TruncatedConeShape::TruncatedConeShape() : Shape(kShapeTruncatedCone)
{
}

TruncatedConeShape::TruncatedConeShape(const Vector2D& size, float height, float ratio) : Shape(kShapeTruncatedCone)
{
	SetNewObject(new TruncatedConeShapeObject(size, height, ratio));
}

TruncatedConeShape::TruncatedConeShape(const TruncatedConeShape& truncatedConeShape) : Shape(truncatedConeShape)
{
}

TruncatedConeShape::~TruncatedConeShape()
{
}

Node *TruncatedConeShape::Replicate(void) const
{
	return (new TruncatedConeShape(*this));
}

bool TruncatedConeShape::CalculateBoundingBox(Box3D *box) const
{
	const TruncatedConeShapeObject *object = GetObject();
	const Vector2D& size = object->GetConeSize();
	float height = object->GetConeHeight();

	box->min.Set(-size.x, -size.y, 0.0F);
	box->max.Set(size.x, size.y, height);
	return (true);
}


TruncatedDomeShape::TruncatedDomeShape() : Shape(kShapeTruncatedDome)
{
}

TruncatedDomeShape::TruncatedDomeShape(const Vector2D& size, float height, float ratio) : Shape(kShapeTruncatedDome)
{
	SetNewObject(new TruncatedDomeShapeObject(size, height, ratio));
}

TruncatedDomeShape::TruncatedDomeShape(const TruncatedDomeShape& truncatedDomeShape) : Shape(truncatedDomeShape)
{
}

TruncatedDomeShape::~TruncatedDomeShape()
{
}

Node *TruncatedDomeShape::Replicate(void) const
{
	return (new TruncatedDomeShape(*this));
}

bool TruncatedDomeShape::CalculateBoundingBox(Box3D *box) const
{
	const TruncatedDomeShapeObject *object = GetObject();
	const Vector2D& size = object->GetDomeSize().GetVector2D();
	float height = object->GetDomeHeight();

	box->min.Set(-size.x, -size.y, 0.0F);
	box->max.Set(size.x, size.y, height);
	return (true);
}

// ZYUQURM
