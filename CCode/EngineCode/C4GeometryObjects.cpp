 

#include "C4GeometryObjects.h"
#include "C4Primitives.h"
#include "C4Topology.h"
#include "C4Hull.h"
#include "C4Terrain.h"
#include "C4Water.h"
#include "C4Models.h"
#include "C4Configuration.h"


using namespace C4;


namespace
{
	enum
	{
		kBuildMeshWeld			= 1 << 0,
		kBuildMeshOptimize		= 1 << 1,
		kBuildMeshColor			= 1 << 2
	};
}


namespace C4
{
	struct BooleanEdge : public ListElement<BooleanEdge>
	{
		unsigned_int32		validFlags;
		Point3D				endpoint[2];

		BooleanEdge(int32 index, const Point3D& p);
		~BooleanEdge();
	};

	struct BooleanLoop : public ListElement<BooleanLoop>
	{
		int32		vertexCount;
		bool		*reflex;
		bool		*active;
		Point3D		*vertex;

		BooleanLoop(int32 count);
		~BooleanLoop();

		int32 GetActiveVertexCount(void) const;

		int32 GetNextActiveVertex(int32 index) const;
		int32 GetPrevActiveVertex(int32 index) const;

		bool ClassifyVertex(int32 index, const Vector3D& normal);
		int32 GetDecompStart(int32 *finish) const;
	};


	template <> Heap Memory<GeometryOctree>::heap("GeometryOctree", MemoryMgr::CalculatePoolSize(128, sizeof(GeometryOctree)));
	template class Memory<GeometryOctree>;
}


BooleanEdge::BooleanEdge(int32 index, const Point3D& p)
{
	validFlags = 1 << index;
	endpoint[index] = p;
}

BooleanEdge::~BooleanEdge()
{
}


BooleanLoop::BooleanLoop(int32 count)
{
	vertexCount = count;

	reflex = new bool[count * (2 + sizeof(Point3D))];
	active = reflex + count;
	vertex = reinterpret_cast<Point3D *>(active + count);

	for (machine a = 0; a < count; a++)
	{
		active[a] = true;
	}
}

BooleanLoop::~BooleanLoop()
{
	delete[] reflex;
}

int32 BooleanLoop::GetActiveVertexCount(void) const
{
	int32 count = 0;
	for (machine a = 0; a < vertexCount; a++)
	{
		count += active[a];
	}

	return (count);
}
 
int32 BooleanLoop::GetNextActiveVertex(int32 index) const
{
	for (;;) 
	{
		if (++index == vertexCount) 
		{
			index = 0;
		} 

		if (active[index]) 
		{ 
			return (index);
		}
	}
} 

int32 BooleanLoop::GetPrevActiveVertex(int32 index) const
{
	for (;;)
	{
		if (--index == -1)
		{
			index = vertexCount - 1;
		}

		if (active[index])
		{
			return (index);
		}
	}
}

bool BooleanLoop::ClassifyVertex(int32 index, const Vector3D& normal)
{
	int32 next = GetNextActiveVertex(index);
	int32 prev = GetPrevActiveVertex(index);

	const Point3D& p0 = vertex[prev];
	const Point3D& p1 = vertex[index];
	const Point3D& p2 = vertex[next];

	bool b = (normal % (p1 - p0) * (p2 - p1) < 0.0F);
	reflex[index] = b;
	return (b);
}

int32 BooleanLoop::GetDecompStart(int32 *finish) const
{
	for (machine start = 0;; start++)
	{
		for (machine a = start; a < vertexCount; a++)
		{
			if ((active[a]) && (reflex[a]))
			{
				start = a;
				goto found;
			}
		}

		break;

		found:
		int32 next = GetNextActiveVertex(start);
		if (!reflex[next])
		{
			*finish = next;
			return (start);
		}
	}

	return (-1);
}


GeometryOctree::GeometryOctree()
{
	for (machine a = 0; a < 8; a++)
	{
		subnode[a] = nullptr;
	}
}

GeometryOctree::GeometryOctree(const Box3D& bounds) : GeometryOctree()
{
	octreeCenter = (bounds.min + bounds.max) * 0.5F;
	octreeSize = (bounds.max - bounds.min) * 0.5F;
}

GeometryOctree::GeometryOctree(const GeometryOctree *octree, int32 subnodeIndex) : GeometryOctree()
{
	const Vector3D& size = octree->GetSize();
	float x = size.x * 0.5F;
	float y = size.y * 0.5F;
	float z = size.z * 0.5F;
	octreeSize.Set(x, y, z);

	if (!(subnodeIndex & kOctantX))
	{
		x = -x;
	}

	if (!(subnodeIndex & kOctantY))
	{
		y = -y;
	}

	if (!(subnodeIndex & kOctantZ))
	{
		z = -z;
	}

	const Point3D& center = octree->GetCenter();
	octreeCenter.Set(center.x + x, center.y + y, center.z + z);
}

GeometryOctree::~GeometryOctree()
{
	for (machine a = 0; a < 8; a++)
	{
		delete subnode[a];
	}
}

int32 GeometryOctree::ClassifyPoint(const Vector3D& p) const
{
	int32 index = (p.x > octreeCenter.x) ? kOctantX : 0;

	if (p.y > octreeCenter.y)
	{
		index |= kOctantY;
	}

	if (p.z > octreeCenter.z)
	{
		index |= kOctantZ;
	}

	return (index);
}

GeometryOctree *GeometryOctree::FindNodeContainingTriangle(const Vector3D& p1, const Vector3D& p2, const Vector3D& p3, int32 maxDepth)
{
	if (maxDepth > 1)
	{
		int32 subnodeIndex1 = ClassifyPoint(p1);
		int32 subnodeIndex2 = ClassifyPoint(p2);
		int32 subnodeIndex3 = ClassifyPoint(p3);
		if ((subnodeIndex1 == subnodeIndex2) && (subnodeIndex1 == subnodeIndex3))
		{
			GeometryOctree *octree = GetSubnode(subnodeIndex1);
			if (!octree)
			{
				octree = new GeometryOctree(this, subnodeIndex1);
				SetSubnode(subnodeIndex1, octree);
			}

			return (octree->FindNodeContainingTriangle(p1, p2, p3, maxDepth - 1));
		}
	}

	return (this);
}


GeometryObject::GeometryObject(GeometryType type) :
		Object(kObjectGeometry),
		staticVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		staticIndexBuffer(kVertexBufferIndex | kVertexBufferStatic)
{
	geometryType = type;
	Initialize();

	geometryLevelCount = 0;
	geometryLevel = nullptr;
}

GeometryObject::GeometryObject(GeometryType type, int32 levelCount) :
		Object(kObjectGeometry),
		staticVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		staticIndexBuffer(kVertexBufferIndex | kVertexBufferStatic)
{
	geometryType = type;
	Initialize();

	geometryLevelCount = levelCount;
	geometryLevel = new Mesh[levelCount];
}

GeometryObject::~GeometryObject()
{
	if (!(geometryObjectFlags & kGeometryObjectStaticSurfaces))
	{
		delete[] surfaceData;
	}

	delete[] convexHullIndexArray;
	delete[] reinterpret_cast<char *>(collisionOctree);
	delete[] geometryLevel;
}

void GeometryObject::Initialize(void)
{
	geometryFlags = kGeometryCastShadows;
	geometryEffectFlags = 0;

	geometryDetailBias = 0.0F;
	shaderDetailBias = 0.0F;

	geometryObjectFlags = 0;

	collisionExclusionMask = kCollisionSoundPath;
	collisionLevel = 0;

	surfaceCount = 0;
	surfaceData = nullptr;

	collisionOctree = nullptr;
	convexHullIndexArray = nullptr;
}

int32 GeometryObject::Release(void)
{
	if ((geometryObjectFlags & kGeometryObjectPrototype) && (GetReferenceCount() == 2))
	{
		ResetVertexBuffers();
	}

	return (Object::Release());
}

GeometryObject *GeometryObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kGeometryGeneric:

			return (new GenericGeometryObject);

		case kGeometryPrimitive:

			return (PrimitiveGeometryObject::Create(++data, unpackFlags));

		case kGeometryTerrain:

			if ((++data).GetType() == 0)
			{
				return (new TerrainGeometryObject);
			}

			return (new TerrainLevelGeometryObject);

		case kGeometryWater:

			return (new WaterGeometryObject);

		case kGeometryHorizonWater:

			return (new HorizonWaterGeometryObject);
	}

	return (nullptr);
}

void GeometryObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << geometryType;
}

void GeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << geometryFlags;

	data << ChunkHeader('EFLG', 4);
	data << geometryEffectFlags;

	if (geometryDetailBias != 0.0F)
	{
		data << ChunkHeader('BIAS', 4);
		data << geometryDetailBias;
	}

	if (shaderDetailBias != 0.0F)
	{
		data << ChunkHeader('SHDB', 4);
		data << shaderDetailBias;
	}

	data << ChunkHeader('CDAT', 8);
	data << collisionExclusionMask;
	data << collisionLevel;

	PackHandle handle = data.BeginChunk('GLEV');

	data << geometryLevelCount;
	for (machine a = 0; a < geometryLevelCount; a++)
	{
		geometryLevel[a].Pack(data, packFlags);
	}

	data.EndChunk(handle);

	if (surfaceData)
	{
		data << ChunkHeader('SURF', sizeof(SurfaceData) * surfaceCount + 4);
		data << surfaceCount;

		for (machine a = 0; a < surfaceCount; a++)
		{
			data << surfaceData[a];
		}
	}

	if (collisionOctree)
	{
		data << ChunkHeader('CTRE', 4 + collisionOctreeSize);
		data << collisionOctreeSize;
		data.WriteData(collisionOctree, collisionOctreeSize);
	}

	if (convexHullIndexArray)
	{
		int32 size = ((convexHullVertexCount + 1) & ~1) * 2;
		data << ChunkHeader('HULL', 4 + size);
		data << convexHullVertexCount;
		data.WriteData(convexHullIndexArray, size);
	}

	data << TerminatorChunk;
}

void GeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<GeometryObject>(data, unpackFlags);
}

bool GeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> geometryFlags;

			#if C4LEGACY

				if ((data.GetVersion() < 60) && (!(geometryFlags & (1 << 11))))
				{
					geometryFlags |= kGeometryCastShadows;
				}

				geometryFlags &= ~((1 << 11) | (1 << 29));

			#endif

			return (true);

		case 'EFLG':

			data >> geometryEffectFlags;
			return (true);

		case 'BIAS':

			data >> geometryDetailBias;
			return (true);

		case 'SHDB':

			data >> shaderDetailBias;
			return (true);

		case 'CDAT':

			data >> collisionExclusionMask;
			data >> collisionLevel;
			return (true);

		case 'GLEV':

			data >> geometryLevelCount;
			if (geometryLevelCount != 0)
			{
				geometryLevel = new Mesh[geometryLevelCount];
				for (machine a = 0; a < geometryLevelCount; a++)
				{
					geometryLevel[a].Unpack(data, unpackFlags);
				}
			}

			return (true);

		case 'SURF':
		{
			int32	count;

			data >> count;

			if (!(geometryObjectFlags & kGeometryObjectStaticSurfaces))
			{
				if (!(unpackFlags & kUnpackEditor))
				{
					break;
				}

				surfaceCount = count;
				if (count != 0)
				{
					surfaceData = new SurfaceData[surfaceCount];
				}
			}

			for (machine a = 0; a < count; a++)
			{
				data >> surfaceData[a];
			}

			return (true);
		}

		case 'CTRE':

			data >> collisionOctreeSize;
			collisionOctree = reinterpret_cast<CollisionOctree *>(new char[collisionOctreeSize]);
			data.ReadData(collisionOctree, collisionOctreeSize);
			return (true);

		case 'HULL':
		{
			data >> convexHullVertexCount;
			int32 count = (convexHullVertexCount + 1) & ~1;
			convexHullIndexArray = new unsigned_int16[count];
			data.ReadArray(count, convexHullIndexArray);
			return (true);
		}
	}

	return (false);
}

void *GeometryObject::BeginSettingsUnpack(void)
{
	geometryDetailBias = 0.0F;
	shaderDetailBias = 0.0F;

	if (!(geometryObjectFlags & kGeometryObjectStaticSurfaces))
	{
		delete[] surfaceData;
		surfaceData = nullptr;
	}

	delete[] convexHullIndexArray;
	convexHullIndexArray = nullptr;

	delete[] reinterpret_cast<char *>(collisionOctree);
	collisionOctree = nullptr;

	delete[] geometryLevel;
	geometryLevel = nullptr;

	return (nullptr);
}

int32 GeometryObject::GetCategoryCount(void) const
{
	return (2);
}

Type GeometryObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectGeometry));
		return (kObjectGeometry);
	}

	if (index == 1)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID('COLL'));
		return ('COLL');
	}

	return (0);
}

int32 GeometryObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectGeometry)
	{
		return (16);
	}

	if (category == 'COLL')
	{
		return (14);
	}

	return (0);
}

Setting *GeometryObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (category == kObjectGeometry)
	{
		if (index == 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND'));
			return (new HeadingSetting('REND', title));
		}

		if (index == 1)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			int32 value = 0;

			if (geometryFlags & kGeometryRemotePortal)
			{
				value = 1;
			}
			else if (geometryFlags & kGeometryRenderEffectPass)
			{
				if (geometryEffectFlags & kGeometryEffectOpaque)
				{
					value = (geometryEffectFlags & kGeometryEffectAccumulate) ? 3 : 2;
				}
				else
				{
					value = (geometryEffectFlags & kGeometryEffectAccumulate) ? 5 : 4;
				}
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'MODE'));
			MenuSetting *menu = new MenuSetting('MODE', value, title, 6);

			menu->SetMenuItemString(0, table->GetString(StringID(kObjectGeometry, 'REND', 'MODE', 'NRML')));
			menu->SetMenuItemString(1, table->GetString(StringID(kObjectGeometry, 'REND', 'MODE', 'COVR')));
			menu->SetMenuItemString(2, table->GetString(StringID(kObjectGeometry, 'REND', 'MODE', 'OBEF')));
			menu->SetMenuItemString(3, table->GetString(StringID(kObjectGeometry, 'REND', 'MODE', 'OAEF')));
			menu->SetMenuItemString(4, table->GetString(StringID(kObjectGeometry, 'REND', 'MODE', 'TBEF')));
			menu->SetMenuItemString(5, table->GetString(StringID(kObjectGeometry, 'REND', 'MODE', 'TAEF')));

			return (menu);
		}

		if (index == 2)
		{
			if ((flags & kConfigurationScript) || (geometryType == kGeometryWater))
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'SHAD'));
			return (new BooleanSetting('SHAD', ((geometryFlags & kGeometryCastShadows) != 0), title));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'RFOG'));
			return (new BooleanSetting('RFOG', ((geometryFlags & kGeometryFogInhibit) == 0), title));
		}

		if (index == 4)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'CUBE'));
			return (new BooleanSetting('CUBE', ((geometryFlags & kGeometryCubeLightInhibit) == 0), title));
		}

		if (index == 5)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'AMBT'));
			return (new BooleanSetting('AMBT', ((geometryFlags & kGeometryAmbientOnly) != 0), title));
		}

		if (index == 6)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'DCAL'));
			return (new BooleanSetting('DCAL', ((geometryFlags & kGeometryRenderDecal) != 0), title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'IVIS'));
			return (new BooleanSetting('IVIS', ((geometryFlags & kGeometryInvisible) != 0), title));
		}

		if (index == 8)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'ISPC'));
			return (new BooleanSetting('ISPC', ((geometryFlags & kGeometryInstancePaintSpace) != 0), title));
		}

		if (index == 9)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'BLUR'));
			return (new BooleanSetting('BLUR', ((geometryFlags & kGeometryMotionBlurInhibit) == 0), title));
		}

		if (index == 10)
		{
			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'MARK'));
			return (new BooleanSetting('MARK', ((geometryFlags & kGeometryMarkingInhibit) == 0), title));
		}

		if (index == 11)
		{
			const char *title = table->GetString(StringID(kObjectGeometry, 'REND', 'FULL'));
			return (new BooleanSetting('FULL', ((geometryFlags & kGeometryMarkingFullPolygon) != 0), title));
		}

		if (index == 12)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'DETL'));
			return (new HeadingSetting('DETL', title));
		}

		if (index == 13)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'DETL', 'SDET'));
			return (new BooleanSetting('SDET', ((geometryFlags & kGeometryShaderDetailEnable) != 0), title));
		}

		if (index == 14)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'DETL', 'SHDB'));
			return (new TextSetting('SHDB', shaderDetailBias, title));
		}

		if (index == 15)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kObjectGeometry, 'DETL', 'BIAS'));
			return (new TextSetting('BIAS', geometryDetailBias, title));
		}
	}
	else if (category == 'COLL')
	{
		if (index == 0)
		{
			const char *title = table->GetString(StringID('COLL', 'COLL'));
			return (new HeadingSetting('COLL', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID('COLL', 'COLL', 'DTCT'));
			return (new BooleanSetting('DTCT', (collisionExclusionMask != kCollisionExcludeAll), title));
		}

		if (index == 2)
		{
			if ((geometryObjectFlags & kGeometryObjectConvexPrimitive) == 0)
			{
				const char *title = table->GetString(StringID('COLL', 'COLL', 'HULL'));
				return (new BooleanSetting('HULL', ((geometryFlags & kGeometryConvexHull) != 0), title));
			}
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID('COLL', 'COLL', 'LEVL'));
			return (new IntegerSetting('LEVL', collisionLevel, title, 0, 3, 1));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID('COLL', 'CFLG'));
			return (new HeadingSetting('CFLG', title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID('COLL', 'CFLG', 'BODY'));
			return (new BooleanSetting('BODY', ((collisionExclusionMask & kCollisionRigidBody) == 0), title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID('COLL', 'CFLG', 'CHAR'));
			return (new BooleanSetting('CHAR', ((collisionExclusionMask & kCollisionCharacter) == 0), title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID('COLL', 'CFLG', 'PROJ'));
			return (new BooleanSetting('PROJ', ((collisionExclusionMask & kCollisionProjectile) == 0), title));
		}

		if (index == 8)
		{
			const char *title = table->GetString(StringID('COLL', 'CFLG', 'VHCL'));
			return (new BooleanSetting('VHCL', ((collisionExclusionMask & kCollisionVehicle) == 0), title));
		}

		if (index == 9)
		{
			const char *title = table->GetString(StringID('COLL', 'CFLG', 'CAMR'));
			return (new BooleanSetting('CAMR', ((collisionExclusionMask & kCollisionCamera) == 0), title));
		}

		if (index == 10)
		{
			const char *title = table->GetString(StringID('COLL', 'CFLG', 'INTR'));
			return (new BooleanSetting('INTR', ((collisionExclusionMask & kCollisionInteraction) == 0), title));
		}

		if (index == 11)
		{
			const char *title = table->GetString(StringID('COLL', 'OFLG'));
			return (new HeadingSetting('OFLG', title));
		}

		if (index == 12)
		{
			const char *title = table->GetString(StringID('COLL', 'OFLG', 'SITE'));
			return (new BooleanSetting('SITE', ((collisionExclusionMask & kCollisionSightPath) == 0), title));
		}

		if (index == 13)
		{
			const char *title = table->GetString(StringID('COLL', 'OFLG', 'SOND'));
			return (new BooleanSetting('SOND', ((collisionExclusionMask & kCollisionSoundPath) == 0), title));
		}
	}

	return (nullptr);
}

void GeometryObject::SetCategorySetting(Type category, const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (category == kObjectGeometry)
	{
		if (identifier == 'MODE')
		{
			geometryFlags &= ~(kGeometryRemotePortal | kGeometryRenderEffectPass);
			geometryEffectFlags &= ~(kGeometryEffectAccumulate | kGeometryEffectOpaque);

			int32 value = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
			if (value == 1)
			{
				geometryFlags |= kGeometryRemotePortal;
			}
			else if (value >= 2)
			{
				geometryFlags |= kGeometryRenderEffectPass;

				if (value == 2)
				{
					geometryEffectFlags |= kGeometryEffectOpaque;
				}
				else if (value == 3)
				{
					geometryEffectFlags |= kGeometryEffectAccumulate | kGeometryEffectOpaque;
				}
				else if (value == 5)
				{
					geometryEffectFlags |= kGeometryEffectAccumulate;
				}
			}
		}
		else if (identifier == 'SHAD')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryCastShadows;
			}
			else
			{
				geometryFlags &= ~kGeometryCastShadows;
			}
		}
		else if (identifier == 'RFOG')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryFogInhibit;
			}
			else
			{
				geometryFlags &= ~kGeometryFogInhibit;
			}
		}
		else if (identifier == 'CUBE')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryCubeLightInhibit;
			}
			else
			{
				geometryFlags &= ~kGeometryCubeLightInhibit;
			}
		}
		else if (identifier == 'AMBT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryAmbientOnly;
			}
			else
			{
				geometryFlags &= ~kGeometryAmbientOnly;
			}
		}
		else if (identifier == 'DCAL')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryRenderDecal;
			}
			else
			{
				geometryFlags &= ~kGeometryRenderDecal;
			}
		}
		else if (identifier == 'IVIS')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryInvisible;
			}
			else
			{
				geometryFlags &= ~kGeometryInvisible;
			}
		}
		else if (identifier == 'ISPC')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryInstancePaintSpace;
			}
			else
			{
				geometryFlags &= ~kGeometryInstancePaintSpace;
			}
		}
		else if (identifier == 'BLUR')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryMotionBlurInhibit;
			}
			else
			{
				geometryFlags &= ~kGeometryMotionBlurInhibit;
			}
		}
		else if (identifier == 'MARK')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryMarkingInhibit;
			}
			else
			{
				geometryFlags &= ~kGeometryMarkingInhibit;
			}
		}
		else if (identifier == 'FULL')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryMarkingFullPolygon;
			}
			else
			{
				geometryFlags &= ~kGeometryMarkingFullPolygon;
			}
		}
		else if (identifier == 'SDET')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				geometryFlags |= kGeometryShaderDetailEnable;
			}
			else
			{
				geometryFlags &= ~kGeometryShaderDetailEnable;
			}
		}
		else if (identifier == 'SHDB')
		{
			shaderDetailBias = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'BIAS')
		{
			geometryDetailBias = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
	}
	else if (category == 'COLL')
	{
		if (identifier == 'DTCT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask &= kCollisionCharacter | kCollisionProjectile | kCollisionVehicle | kCollisionCamera | kCollisionInteraction | kCollisionSightPath | kCollisionSoundPath;
			}
			else
			{
				collisionExclusionMask = kCollisionExcludeAll;
			}
		}
		else if (identifier == 'HULL')
		{
			if ((geometryObjectFlags & kGeometryObjectConvexPrimitive) == 0)
			{
				if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
				{
					geometryFlags |= kGeometryConvexHull;
				}
				else
				{
					geometryFlags &= ~kGeometryConvexHull;
				}
			}
		}
		else if (identifier == 'LEVL')
		{
			SetCollisionLevel(static_cast<const IntegerSetting *>(setting)->GetIntegerValue());
		}
		else if (identifier == 'BODY')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionRigidBody;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionRigidBody;
			}
		}
		else if (identifier == 'CHAR')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionCharacter;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionCharacter;
			}
		}
		else if (identifier == 'PROJ')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionProjectile;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionProjectile;
			}
		}
		else if (identifier == 'VHCL')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionVehicle;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionVehicle;
			}
		}
		else if (identifier == 'CAMR')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionCamera;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionCamera;
			}
		}
		else if (identifier == 'INTR')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionInteraction;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionInteraction;
			}
		}
		else if (identifier == 'SITE')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionSightPath;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionSightPath;
			}
		}
		else if (identifier == 'SOND')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				collisionExclusionMask |= kCollisionSoundPath;
			}
			else if (collisionExclusionMask != kCollisionExcludeAll)
			{
				collisionExclusionMask &= ~kCollisionSoundPath;
			}
		}
	}
}

void GeometryObject::Preprocess(unsigned_int32 dynamicFlags)
{
	unsigned_int32 objectFlags = geometryObjectFlags;
	if (!(objectFlags & kGeometryObjectPreprocessed))
	{
		geometryObjectFlags = (unsigned_int16) (objectFlags | kGeometryObjectPreprocessed);
		dynamicArrayFlags = (unsigned_int16) dynamicFlags;

		unsigned_int32 bufferSize = 0;
		unsigned_int32 maxVertexSize = 0;

		Mesh *mesh = geometryLevel;
		int32 levelCount = geometryLevelCount;
		for (machine a = 0; a < levelCount; a++)
		{
			unsigned_int32 vertexSize = 0;

			const unsigned_int8 *arrayIndex = mesh->GetAttributeArrayIndex();
			int32 arrayCount = mesh->GetAttributeArrayCount();
			for (machine b = 0; b < arrayCount; b++)
			{
				int32 index = arrayIndex[b];
				if ((dynamicFlags & (1 << index)) == 0)
				{
					mesh->attributeOffset[b] = bufferSize + vertexSize;
					vertexSize += mesh->GetArrayDescriptor(index)->elementSize;
				}
			}

			bufferSize += mesh->GetVertexCount() * vertexSize;
			maxVertexSize = Max(maxVertexSize, vertexSize);
			mesh++;
		}

		staticVertexStride = maxVertexSize;
		staticVertexBuffer.Establish(bufferSize);
		BuildStaticVertexBuffer(&staticVertexBuffer);

		mesh = geometryLevel;
		if (mesh->GetArray(kArrayPrimitive))
		{
			unsigned_int32 indexSize = 0;
			for (machine a = 0; a < levelCount; a++)
			{
				mesh->primitiveOffset = indexSize;
				const ArrayDescriptor *desc = mesh->GetArrayDescriptor(kArrayPrimitive);
				indexSize += ((desc->elementCount + 1) & ~1) * desc->elementSize;
				mesh++;
			}

			staticIndexBuffer.Establish(indexSize);
			BuildStaticIndexBuffer(&staticIndexBuffer);
		}
	}
}

void GeometryObject::Neutralize(void)
{
	geometryObjectFlags &= ~kGeometryObjectPreprocessed;
}

void GeometryObject::ResetVertexBuffers(void)
{
	staticIndexBuffer.Establish(0);
	staticVertexBuffer.Establish(0);

	geometryObjectFlags &= ~kGeometryObjectPreprocessed;
}

void GeometryObject::BuildStaticVertexBuffer(VertexBuffer *vertexBuffer)
{
	volatile unsigned_int32 *restrict buffer = vertexBuffer->BeginUpdateSync<unsigned_int32>();

	int32 levelCount = geometryLevelCount;
	const Mesh *mesh = geometryLevel;
	unsigned_int32 dynamicFlags = dynamicArrayFlags;

	for (machine a = 0; a < levelCount; a++)
	{
		unsigned_int8			componentCount[kMaxAttributeArrayCount];
		const unsigned_int32	*staticArray[kMaxAttributeArrayCount];

		int32 staticCount = 0;

		int32 arrayCount = mesh->GetAttributeArrayCount();
		const unsigned_int8 *arrayIndex = mesh->GetAttributeArrayIndex();
		for (machine b = 0; b < arrayCount; b++)
		{
			int32 index = arrayIndex[b];
			if ((dynamicFlags & (1 << index)) == 0)
			{
				const ArrayBundle *bundle = mesh->GetArrayBundle(index);
				componentCount[staticCount] = (unsigned_int8) bundle->descriptor.componentCount;
				staticArray[staticCount] = static_cast<unsigned_int32 *>(bundle->pointer);
				staticCount++;
			}
		}

		int32 vertexCount = mesh->GetVertexCount();
		for (machine b = 0; b < vertexCount; b++)
		{
			for (machine c = 0; c < staticCount; c++)
			{
				int32 count = componentCount[c];
				const unsigned_int32 *source = staticArray[c] + b * count;
				for (machine d = 0; d < count; d++)
				{
					buffer[d] = source[d];
				}

				buffer += count;
			}
		}

		mesh++;
	}

	vertexBuffer->EndUpdateSync();
}

void GeometryObject::BuildStaticIndexBuffer(VertexBuffer *indexBuffer)
{
	volatile unsigned_int16 *restrict buffer = indexBuffer->BeginUpdateSync<unsigned_int16>();

	const Mesh *mesh = geometryLevel;
	int32 levelCount = geometryLevelCount;
	for (machine a = 0; a < levelCount; a++)
	{
		const ArrayBundle *bundle = mesh->GetArrayBundle(kArrayPrimitive);

		const unsigned_int16 *source = static_cast<unsigned_int16 *>(bundle->pointer);
		volatile unsigned_int16 *restrict destin = buffer + mesh->GetPrimitiveIndexOffset() / 2;

		int32 count = (bundle->descriptor.elementCount * bundle->descriptor.elementSize) >> 1;
		for (machine b = 0; b < count; b++)
		{
			destin[b] = source[b];
		}

		mesh++;
	}

	indexBuffer->EndUpdateSync();
}

void GeometryObject::SetGeometryLevelCount(int32 levelCount)
{
	delete[] geometryLevel;

	geometryLevelCount = levelCount;
	if (levelCount != 0)
	{
		geometryLevel = new Mesh[levelCount];
	}
	else
	{
		geometryLevel = nullptr;
	}

	delete[] reinterpret_cast<char *>(collisionOctree);
	collisionOctree = nullptr;

	delete[] convexHullIndexArray;
	convexHullIndexArray = nullptr;

	collisionLevel = Min(collisionLevel, levelCount - 1);
}

void GeometryObject::SetSurfaceCount(int32 count)
{
	delete[] surfaceData;

	surfaceCount = count;
	if (count != 0)
	{
		surfaceData = new SurfaceData[count];
	}
	else
	{
		surfaceData = nullptr;
	}
}

void GeometryObject::SetStaticSurfaceData(int32 count, SurfaceData *data, bool init)
{
	surfaceCount = count;
	surfaceData = data;

	geometryObjectFlags |= kGeometryObjectStaticSurfaces;

	if (init)
	{
		for (machine a = 0; a < count; a++)
		{
			data->surfaceFlags = 0;
			data->materialIndex = 0;
			data++;
		}
	}
}

unsigned_int32 GeometryObject::GetCompressedOctreeSize(const GeometryOctree *geometryOctree)
{
	unsigned_int32 size = sizeof(CollisionOctree) + ((geometryOctree->GetIndexCount() + 3) & ~3) * 2 + 4;
	for (machine a = 0; a < 8; a++)
	{
		const GeometryOctree *node = geometryOctree->GetSubnode(a);
		if (node)
		{
			size += GetCompressedOctreeSize(node);
		}
	}

	return (size);
}

char *GeometryObject::CompressOctree(const GeometryOctree *geometryOctree, CollisionOctree *collisionOctree)
{
	const Point3D& center = geometryOctree->GetCenter();
	const Vector3D& size = geometryOctree->GetSize();

	collisionOctree->collisionBounds.min = center - size;
	collisionOctree->collisionBounds.max = center + size;

	int32 count = geometryOctree->GetIndexCount();
	collisionOctree->elementCount = (unsigned_int16) count;
	collisionOctree->offsetAlign = 8;

	const unsigned_int32 *indexArray = geometryOctree->GetIndexArray();
	unsigned_int16 *compressedArray = collisionOctree->GetIndexArray();
	for (machine a = 0; a < count; a++)
	{
		compressedArray[a] = (unsigned_int16) indexArray[a];
	}

	// Make sure that sizeof(CollisionOctree) + roundedCount * 2 is a multiple of 8.

	int32 roundedCount = ((count + 3) & ~3) + 2;
	for (machine a = count; a < roundedCount; a++)
	{
		compressedArray[a] = 0;
	}

	char *nodeBase = reinterpret_cast<char *>(collisionOctree);
	char *subnodeBase = nodeBase + sizeof(CollisionOctree) + roundedCount * 2;

	for (machine a = 0; a < 8; a++)
	{
		const GeometryOctree *node = geometryOctree->GetSubnode(a);
		if (node)
		{
			unsigned_int32 offset = (unsigned_int32) (subnodeBase - nodeBase) / 8;
			if (offset < 65536)
			{
				collisionOctree->subnodeOffset[a] = (unsigned_int16) offset;
				subnodeBase = CompressOctree(node, reinterpret_cast<CollisionOctree *>(subnodeBase));
			}
			else
			{
				collisionOctree->subnodeOffset[a] = 0;
			}
		}
		else
		{
			collisionOctree->subnodeOffset[a] = 0;
		}
	}

	return (subnodeBase);
}

GeometryOctree *GeometryObject::BuildCollisionOctree(const Mesh *level, const Box3D& boundingBox)
{
	GeometryOctree *octree = new GeometryOctree(boundingBox);

	const Point3D *vertexArray = level->GetArray<Point3D>(kArrayPosition);
	const Triangle *triangle = level->GetArray<Triangle>(kArrayPrimitive);
	int32 triangleCount = level->GetPrimitiveCount();

	for (machine index = 0; index < triangleCount; index++)
	{
		const Point3D& p1 = vertexArray[triangle->index[0]];
		const Point3D& p2 = vertexArray[triangle->index[1]];
		const Point3D& p3 = vertexArray[triangle->index[2]];

		octree->FindNodeContainingTriangle(p1, p2, p3)->AddIndex(index);
		triangle++;
	}

	return (octree);
}

void GeometryObject::BuildCollisionData(void)
{
	delete[] reinterpret_cast<char *>(collisionOctree);
	collisionOctree = nullptr;

	delete[] convexHullIndexArray;
	convexHullIndexArray = nullptr;

	if (collisionExclusionMask != kCollisionExcludeAll)
	{
		Box3D	boundingBox;

		const Mesh *mesh = &geometryLevel[collisionLevel];
		const Point3D *vertex = mesh->GetArray<Point3D>(kArrayPosition);
		if ((!vertex) || (mesh->GetSkinWeightData()))
		{
			return;
		}

		int32 vertexCount = mesh->GetVertexCount();
		boundingBox.Calculate(vertexCount, vertex);

		GeometryOctree *triangleRoot = BuildCollisionOctree(mesh, boundingBox);
		collisionOctreeSize = GetCompressedOctreeSize(triangleRoot);

		if (collisionOctreeSize != 0)
		{
			collisionOctree = reinterpret_cast<CollisionOctree *>(new char[collisionOctreeSize]);
			CompressOctree(triangleRoot, collisionOctree);
		}

		delete triangleRoot;

		if (geometryFlags & kGeometryConvexHull)
		{
			convexHullIndexArray = new unsigned_int16[(vertexCount + 1) & ~1];
			int32 count = Math::ComputeConvexHull(vertexCount, vertex, boundingBox, convexHullIndexArray);

			convexHullVertexCount = count;
			if ((count & 1) != 0)
			{
				convexHullIndexArray[count - 1] = 0;
			}
		}
	}
}

void GeometryObject::ScaleCollisionOctree(CollisionOctree *octree, float factor)
{
	octree->collisionBounds.Scale(factor);

	for (machine a = 0; a < 8; a++)
	{
		if (octree->subnodeOffset[a])
		{
			ScaleCollisionOctree(octree->GetSubnode(a), factor);
		}
	}
}

void GeometryObject::ScaleCollisionData(float factor)
{
	if (collisionOctree)
	{
		ScaleCollisionOctree(collisionOctree, factor);
	}
}

void GeometryObject::OffsetCollisionOctree(CollisionOctree *octree, const Vector3D& dv)
{
	octree->collisionBounds.Offset(dv);

	for (machine a = 0; a < 8; a++)
	{
		if (octree->subnodeOffset[a])
		{
			OffsetCollisionOctree(octree->GetSubnode(a), dv);
		}
	}
}

void GeometryObject::OffsetCollisionData(const Vector3D& dv)
{
	if (collisionOctree)
	{
		OffsetCollisionOctree(collisionOctree, dv);
	}
}

const Point3D& GeometryObject::CalculateConvexHullSupportPoint(const Point3D *vertex, const Vector3D& direction) const
{
	int32 vertexCount = convexHullVertexCount;
	const unsigned_int16 *hullIndex = convexHullIndexArray;

	unsigned_int32 maxIndex = hullIndex[0];

	#if C4SIMD

		vec_float d = VecLoadUnaligned(&direction.x);
		vec_float maxDistance = VecDot3D(VecLoadUnaligned(&vertex[maxIndex].x), d);

		for (machine a = 1; a < vertexCount; a++)
		{
			unsigned_int32 i = hullIndex[a];
			vec_float f = VecDot3D(VecLoadUnaligned(&vertex[i].x), d);
			if (VecCmpgtScalar(f, maxDistance))
			{
				maxIndex = i;
				maxDistance = f;
			}
		}

	#else

		float maxDistance = vertex[maxIndex] * direction;

		for (machine a = 1; a < vertexCount; a++)
		{
			unsigned_int32 i = hullIndex[a];
			float f = vertex[i] * direction;
			if (f > maxDistance)
			{
				maxIndex = i;
				maxDistance = f;
			}
		}

	#endif

	return (vertex[maxIndex]);
}

void GeometryObject::CalculateConvexHullSupportPointArray(const Point3D *vertex, int32 count, const Vector3D *direction, Point3D *support) const
{
	int32 vertexCount = convexHullVertexCount;
	const unsigned_int16 *hullIndex = convexHullIndexArray;

	for (machine k = 0; k < count; k++)
	{
		#if C4SIMD

			vec_float d = VecLoadUnaligned(&direction[k].x);
			vec_float maxVertex = VecLoadUnaligned(&vertex[hullIndex[0]].x);
			vec_float maxDistance = VecDot3D(maxVertex, d);

			for (machine a = 1; a < vertexCount; a++)
			{
				vec_float v = VecLoadUnaligned(&vertex[hullIndex[a]].x);
				vec_float f = VecDot3D(v, d);
				vec_float mask = VecSmearX(VecMaskCmpgt(f, maxDistance));
				maxVertex = VecSelect(maxVertex, v, mask);
				maxDistance = VecSelect(maxDistance, f, mask);
			}

			VecStore3D(maxVertex, &support[k].x);

		#else

			const Vector3D& d = direction[k];
			unsigned_int32 maxIndex = hullIndex[0];
			float maxDistance = vertex[maxIndex] * d;

			for (machine a = 1; a < vertexCount; a++)
			{
				unsigned_int32 i = hullIndex[a];
				float f = vertex[i] * d;
				if (f > maxDistance)
				{
					maxIndex = i;
					maxDistance = f;
				}
			}

			support[k] = vertex[maxIndex];

		#endif
	}
}

int32 GeometryObject::GetMaxCollisionLevel(void) const
{
	return (geometryLevelCount - 1);
}

bool GeometryObject::ClipSegmentToCollisionBounds(const Box3D& bounds, float radius, Point3D& p1, Point3D& p2)
{
	radius += 0.03125F;

	float dx = p2.x - p1.x;
	float xmin = bounds.min.x - radius;
	if (p1.x < xmin)
	{
		if (p2.x < xmin)
		{
			return (false);
		}

		if (Fabs(dx) > K::min_float)
		{
			float t = (xmin - p1.x) / dx;

			p1.x = xmin;
			dx = p2.x - xmin;

			p1.y += t * (p2.y - p1.y);
			p1.z += t * (p2.z - p1.z);
		}
	}
	else if (p2.x < xmin)
	{
		if (Fabs(dx) > K::min_float)
		{
			float t = (p2.x - xmin) / dx;

			p2.x = xmin;
			dx = xmin - p1.x;

			p2.y += t * (p1.y - p2.y);
			p2.z += t * (p1.z - p2.z);
		}
	}

	float xmax = bounds.max.x + radius;
	if (p1.x > xmax)
	{
		if (p2.x > xmax)
		{
			return (false);
		}

		if (Fabs(dx) > K::min_float)
		{
			float t = (xmax - p1.x) / dx;

			p1.x = xmax;
			p1.y += t * (p2.y - p1.y);
			p1.z += t * (p2.z - p1.z);
		}
	}
	else if (p2.x > xmax)
	{
		if (Fabs(dx) > K::min_float)
		{
			float t = (p2.x - xmax) / dx;

			p2.x = xmax;
			p2.y += t * (p1.y - p2.y);
			p2.z += t * (p1.z - p2.z);
		}
	}

	float dy = p2.y - p1.y;
	float ymin = bounds.min.y - radius;
	if (p1.y < ymin)
	{
		if (p2.y < ymin)
		{
			return (false);
		}

		if (Fabs(dy) > K::min_float)
		{
			float t = (ymin - p1.y) / dy;

			p1.y = ymin;
			dy = p2.y - ymin;

			p1.x += t * (p2.x - p1.x);
			p1.z += t * (p2.z - p1.z);
		}
	}
	else if (p2.y < ymin)
	{
		if (Fabs(dy) > K::min_float)
		{
			float t = (p2.y - ymin) / dy;

			p2.y = ymin;
			dy = ymin - p1.y;

			p2.x += t * (p1.x - p2.x);
			p2.z += t * (p1.z - p2.z);
		}
	}

	float ymax = bounds.max.y + radius;
	if (p1.y > ymax)
	{
		if (p2.y > ymax)
		{
			return (false);
		}

		if (Fabs(dy) > K::min_float)
		{
			float t = (ymax - p1.y) / dy;

			p1.y = ymax;
			p1.x += t * (p2.x - p1.x);
			p1.z += t * (p2.z - p1.z);
		}
	}
	else if (p2.y > ymax)
	{
		if (Fabs(dy) > K::min_float)
		{
			float t = (p2.y - ymax) / dy;

			p2.y = ymax;
			p2.x += t * (p1.x - p2.x);
			p2.z += t * (p1.z - p2.z);
		}
	}

	float dz = p2.z - p1.z;
	float zmin = bounds.min.z - radius;
	if (p1.z < zmin)
	{
		if (p2.z < zmin)
		{
			return (false);
		}

		if (Fabs(dz) > K::min_float)
		{
			float t = (zmin - p1.z) / dz;

			p1.z = zmin;
			dz = p2.z - zmin;

			p1.x += t * (p2.x - p1.x);
			p1.y += t * (p2.y - p1.y);
		}
	}
	else if (p2.z < zmin)
	{
		if (Fabs(dz) > K::min_float)
		{
			float t = (p2.z - zmin) / dz;

			p2.z = zmin;
			dz = zmin - p1.z;

			p2.x += t * (p1.x - p2.x);
			p2.y += t * (p1.y - p2.y);
		}
	}

	float zmax = bounds.max.z + radius;
	if (p1.z > zmax)
	{
		if (p2.z > zmax)
		{
			return (false);
		}

		if (Fabs(dz) > K::min_float)
		{
			float t = (zmax - p1.z) / dz;

			p1.z = zmax;
			p1.x += t * (p2.x - p1.x);
			p1.y += t * (p2.y - p1.y);
		}
	}
	else if (p2.z > zmax)
	{
		if (Fabs(dz) > K::min_float)
		{
			float t = (p2.z - zmax) / dz;

			p2.z = zmax;
			p2.x += t * (p1.x - p2.x);
			p2.y += t * (p1.y - p2.y);
		}
	}

	return (true);
}

bool GeometryObject::DetectSegmentIntersection(const CollisionOctree *octree, const Mesh *level, const Point3D& p1, const Point3D& p2, GeometryHitData *geometryHitData)
{
	bool result = false;
	float smax = 1.0F;

	const Point3D *vertexArray = level->GetArray<Point3D>(kArrayPosition);
	const Triangle *triangleArray = level->GetArray<Triangle>(kArrayPrimitive);

	int32 count = octree->elementCount;
	const unsigned_int16 *triangleIndex = octree->GetIndexArray();
	for (machine i = 0; i < count; i++)
	{
		Point3D		position;
		Vector3D	normal;
		float		param;

		unsigned_int32 index = triangleIndex[i];
		const Triangle *triangle = &triangleArray[index];
		const Point3D& v1 = vertexArray[triangle->index[0]];
		const Point3D& v2 = vertexArray[triangle->index[1]];
		const Point3D& v3 = vertexArray[triangle->index[2]];

		if ((Math::IntersectSegmentAndTriangle(p1, p2, v1, v2, v3, &position, &normal, &param)) && (param < smax))
		{
			smax = param;
			result = true;

			geometryHitData->position = position;
			geometryHitData->normal = normal;
			geometryHitData->triangleIndex = index;
		}
	}

	const Point3D *intersectPoint = (result) ? &geometryHitData->position : &p2;

	for (machine a = 0; a < 8; a++)
	{
		if (octree->subnodeOffset[a] != 0)
		{
			Point3D q1 = p1;
			Point3D q2 = *intersectPoint;

			const CollisionOctree *suboctree = octree->GetSubnode(a);
			if (ClipSegmentToCollisionBounds(suboctree->collisionBounds, 0.0F, q1, q2))
			{
				if (DetectSegmentIntersection(suboctree, level, q1, q2, geometryHitData))
				{
					intersectPoint = &geometryHitData->position;
					result = true;
				}
			}
		}
	}

	return (result);
}

bool GeometryObject::DetectSegmentEdgeIntersection(const Bivector4D& segmentLine, const Bivector4D& edgeLine, const Point3D& p1, const Vector3D& v1, float r2, float& smax, GeometryHitData *geometryHitData)
{
	const Vector3D& dp = segmentLine.GetTangent();

	float w = segmentLine ^ edgeLine;
	Vector3D u = dp % edgeLine.GetTangent();
	if (w * w < u * u * r2)
	{
		float e2 = SquaredMag(edgeLine.GetTangent());
		Vector3D t = edgeLine.GetTangent() * InverseSqrt(e2);

		Point3D p0 = p1 - v1;
		float p0_dot_t = p0 * t;
		float dp_dot_t = dp * t;

		float b = p0 * dp - p0_dot_t * dp_dot_t;
		if (b < 0.0F)
		{
			float a = dp * dp - dp_dot_t * dp_dot_t;
			float c = p0 * p0 - p0_dot_t * p0_dot_t - r2;
			float D = b * b - a * c;
			if (D > 0.0F)
			{
				float s = (-b - Sqrt(D)) / a;
				if ((s > 0.0F) && (s < smax))
				{
					Point3D p = p0 + dp * s;
					float h = p * t;
					if ((h > 0.0F) && (h * h < e2))
					{
						smax = s;
						geometryHitData->position = p + v1;
						geometryHitData->normal = p - ProjectOnto(p, t);
						return (true);
					}
				}
			}
		}
	}

	return (false);
}

bool GeometryObject::DetectSegmentVertexIntersection(const Bivector4D& segmentLine, const Point3D& p1, const Vector3D& v1, float r2, float a, float ainv, float& smax, GeometryHitData *geometryHitData)
{
	const Vector3D& dp = segmentLine.GetTangent();
	Point3D p0 = p1 - v1;

	float b = p0 * dp;
	if (b < 0.0F)
	{
		float c = p0 * p0 - r2;
		float D = b * b - a * c;
		if (D > 0.0F)
		{
			float s = (-b - Sqrt(D)) * ainv;
			if ((s > 0.0F) && (s < smax))
			{
				smax = s;
				geometryHitData->position = p1 + dp * s;
				geometryHitData->normal = geometryHitData->position - v1;
				return (true);
			}
		}
	}

	return (false);
}

bool GeometryObject::DetectSegmentIntersection(const CollisionOctree *octree, const Mesh *level, const Point3D& p1, const Point3D& p2, float radius, GeometryHitData *geometryHitData)
{
	bool result = false;
	float smax = 1.0F;

	const Point3D *vertexArray = level->GetArray<Point3D>(kArrayPosition);
	const Triangle *triangleArray = level->GetArray<Triangle>(kArrayPrimitive);

	Bivector4D segmentLine(p1, p2);

	int32 count = octree->elementCount;
	const unsigned_int16 *triangleIndex = octree->GetIndexArray();
	for (machine i = 0; i < count; i++)
	{
		unsigned_int32 index = triangleIndex[i];
		const Triangle *triangle = &triangleArray[index];
		const Point3D& v1 = vertexArray[triangle->index[0]];
		const Point3D& v2 = vertexArray[triangle->index[1]];
		const Point3D& v3 = vertexArray[triangle->index[2]];

		Antivector4D plane(v1, v2, v3);
		plane.Standardize();

		float d1 = plane ^ p1;
		float d2 = plane ^ p2;

		if ((Fmin(d1, d2) < radius) && (Fmax(d1, d2) > -radius))
		{
			if (!(d1 < radius))
			{
				const Antivector3D& n = plane.GetAntivector3D();
				Vector3D dp = n * radius;
				Point3D s1 = p1 - dp;
				Point3D s2 = p2 - dp;

				Point3D m = (v1 + v2) * 0.5F;
				Vector3D q1 = s1 - m;
				Vector3D q2 = s2 - m;

				Vector3D edge = v2 - v1;
				Vector3D line = q1 % q2;
				if (!(line * edge > 0.0F))
				{
					m = (v2 + v3) * 0.5F;
					q1 = s1 - m;
					q2 = s2 - m;

					edge = v3 - v2;
					line = q1 % q2;
					if (!(line * edge > 0.0F))
					{
						m = (v3 + v1) * 0.5F;
						q1 = s1 - m;
						q2 = s2 - m;

						edge = v1 - v3;
						line = q1 % q2;
						if (!(line * edge > 0.0F))
						{
							float s = (d1 - radius) / (d1 - d2);
							if (s < smax)
							{
								smax = s;
								result = true;

								geometryHitData->position = p1 + segmentLine.GetTangent() * s;
								geometryHitData->normal = n;
								geometryHitData->triangleIndex = index;
							}
						}
					}
				}
			}

			float r2 = radius * radius;

			if (DetectSegmentEdgeIntersection(segmentLine, Bivector4D(v1, v2), p1, v1, r2, smax, geometryHitData))
			{
				result = true;
				geometryHitData->triangleIndex = index;
			}

			if (DetectSegmentEdgeIntersection(segmentLine, Bivector4D(v2, v3), p1, v2, r2, smax, geometryHitData))
			{
				result = true;
				geometryHitData->triangleIndex = index;
			}

			if (DetectSegmentEdgeIntersection(segmentLine, Bivector4D(v3, v1), p1, v3, r2, smax, geometryHitData))
			{
				result = true;
				geometryHitData->triangleIndex = index;
			}

			const Vector3D& dp = segmentLine.GetTangent();
			float a = dp * dp;
			float ainv = 1.0F / a;

			if (DetectSegmentVertexIntersection(segmentLine, p1, v1, r2, a, ainv, smax, geometryHitData))
			{
				result = true;
				geometryHitData->triangleIndex = index;
			}

			if (DetectSegmentVertexIntersection(segmentLine, p1, v2, r2, a, ainv, smax, geometryHitData))
			{
				result = true;
				geometryHitData->triangleIndex = index;
			}

			if (DetectSegmentVertexIntersection(segmentLine, p1, v3, r2, a, ainv, smax, geometryHitData))
			{
				result = true;
				geometryHitData->triangleIndex = index;
			}
		}
	}

	const Point3D *intersectPoint = (result) ? &geometryHitData->position : &p2;

	for (machine a = 0; a < 8; a++)
	{
		if (octree->subnodeOffset[a] != 0)
		{
			Point3D q1 = p1;
			Point3D q2 = *intersectPoint;

			const CollisionOctree *suboctree = octree->GetSubnode(a);
			if (ClipSegmentToCollisionBounds(suboctree->collisionBounds, radius, q1, q2))
			{
				if (DetectSegmentIntersection(suboctree, level, q1, q2, radius, geometryHitData))
				{
					intersectPoint = &geometryHitData->position;
					result = true;
				}
			}
		}
	}

	return (result);
}

bool GeometryObject::DetectCollision(const Point3D& p1, const Point3D& p2, float radius, GeometryHitData *geometryHitData) const
{
	if (collisionOctree)
	{
		Point3D q1 = p1;
		Point3D q2 = p2;

		if (ClipSegmentToCollisionBounds(collisionOctree->collisionBounds, radius, q1, q2))
		{
			bool	result;

			const Mesh *mesh = &geometryLevel[collisionLevel];

			if (radius == 0.0F)
			{
				result = DetectSegmentIntersection(collisionOctree, mesh, q1, q2, geometryHitData);
			}
			else
			{
				result = DetectSegmentIntersection(collisionOctree, mesh, q1, q2, radius, geometryHitData);
			}

			if (result)
			{
				Vector3D dp = p2 - p1;
				geometryHitData->param = (geometryHitData->position - p1) * dp / (dp * dp);
				return (true);
			}
		}
	}

	return (false);
}

bool GeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (false);
}

bool GeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return (false);
}


GenericGeometryObject::GenericGeometryObject() : GeometryObject(kGeometryGeneric)
{
}

GenericGeometryObject::GenericGeometryObject(int32 levelCount) : GeometryObject(kGeometryGeneric, levelCount)
{
}

GenericGeometryObject::GenericGeometryObject(const Geometry *geometry) : GeometryObject(kGeometryGeneric)
{
	const GeometryObject *object = geometry->GetObject();
	SetGeometryFlags(object->GetGeometryFlags());

	int32 surfaceCount = object->GetSurfaceCount();
	if (surfaceCount != 0)
	{
		SetSurfaceCount(surfaceCount);
		for (machine a = 0; a < surfaceCount; a++)
		{
			*GetSurfaceData(a) = *object->GetSurfaceData(a);
		}
	}

	int32 levelCount = object->GetGeometryLevelCount();
	SetGeometryLevelCount(levelCount);

	for (machine a = 0; a < levelCount; a++)
	{
		Mesh	tempMesh[2];

		Mesh *level = object->GetGeometryLevel(a);
		if (level->GetArray<Vector3D>(kArrayNormal))
		{
			if (level->GetArray<Point2D>(kArrayTexcoord))
			{
				tempMesh[0].CopyMesh(level);
			}
			else
			{
				tempMesh[0].BuildTexcoordArray(level, geometry, this);
			}
		}
		else
		{
			if (level->GetArray<Point2D>(kArrayTexcoord))
			{
				tempMesh[0].BuildNormalArray(level);
			}
			else
			{
				tempMesh[1].BuildTexcoordArray(level, geometry, this);
				tempMesh[0].BuildNormalArray(&tempMesh[1]);
			}
		}

		GetGeometryLevel(a)->BuildTangentArray(&tempMesh[0]);
	}

	unsigned_int32 mask = object->GetCollisionExclusionMask();
	SetCollisionExclusionMask(mask);

	UpdateBounds();
	BuildCollisionData();
}

GenericGeometryObject::GenericGeometryObject(int32 levelCount, const List<GeometrySurface> *const *surfaceListTable, int32 surfaceCount, const ImmutableArray<int32>& materialIndexArray, const SkinData *const *skinDataTable, const MorphData *const *morphDataTable) : GeometryObject(kGeometryGeneric, levelCount)
{
	SetSurfaceCount(surfaceCount);
	for (machine a = 0; a < surfaceCount; a++)
	{
		SurfaceData *data = GetSurfaceData(a);

		data->surfaceFlags = 0;
		data->materialIndex = (unsigned_int16) materialIndexArray[a];

		data->textureAlignData[0].alignMode = kTextureAlignNatural;
		data->textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
		data->textureAlignData[1].alignMode = kTextureAlignNatural;
		data->textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
	}

	for (machine level = 0; level < levelCount; level++)
	{
		BuildMesh(level, 0, surfaceListTable[level], materialIndexArray.GetElementCount(), (skinDataTable) ? skinDataTable[level] : nullptr, (morphDataTable) ? morphDataTable[level] : nullptr);
	}

	UpdateBounds();
	BuildCollisionData();
}

GenericGeometryObject::GenericGeometryObject(int32 geometryCount, const Geometry *const *geometryArray, const ImmutableArray<MaterialObject *>& materialArray, const Transformable *transformable) : GeometryObject(kGeometryGeneric)
{
	ArrayDescriptor		desc[kMaxGeometryTexcoordCount + 8];

	const GeometryObject *object = geometryArray[0]->GetObject();
	const Mesh *mesh = object->GetGeometryLevel(0);

	int32 levelCount = object->GetGeometryLevelCount();
	unsigned_int32 geometryFlags = object->GetGeometryFlags();
	unsigned_int32 collisionMask = object->GetCollisionExclusionMask();

	int32 colorCount = (mesh->GetArray(kArrayColor) != nullptr);

	int32 texcoordCount = 1;
	for (machine a = 1; a < kMaxGeometryTexcoordCount; a++)
	{
		if (mesh->GetArray(kArrayTexcoord0 + a))
		{
			texcoordCount = a;
		}
	}

	int32 boneCount = 0;
	bool skinFlag = (mesh->GetSkinWeightData() != nullptr);
	if (skinFlag)
	{
		boneCount = mesh->GetArrayDescriptor(kArrayBoneHash)->elementCount;
		if ((boneCount == 0) || (mesh->GetArrayDescriptor(kArrayInverseBindTransform)->elementCount != boneCount))
		{
			skinFlag = false;
		}
	}

	int32 morphCount = mesh->GetMorphTargetCount();
	bool morphFlag = ((morphCount != 0) && (mesh->GetMorphTargetData() != nullptr) && (mesh->GetArrayDescriptor(kArrayMorphHash)->elementCount == morphCount));

	for (machine a = 1; a < geometryCount; a++)
	{
		object = geometryArray[a]->GetObject();
		mesh = object->GetGeometryLevel(0);

		levelCount = Max(levelCount, object->GetGeometryLevelCount());
		geometryFlags &= object->GetGeometryFlags();
		collisionMask &= object->GetCollisionExclusionMask();

		if (mesh->GetArray(kArrayColor))
		{
			colorCount = 1;
		}

		for (machine b = 1; b < kMaxGeometryTexcoordCount; b++)
		{
			if (mesh->GetArray(kArrayTexcoord0 + b))
			{
				texcoordCount = Max(texcoordCount, b);
			}
		}

		skinFlag &= ((mesh->GetArrayDescriptor(kArrayBoneHash)->elementCount == boneCount) && (mesh->GetArrayDescriptor(kArrayInverseBindTransform)->elementCount == boneCount));
		morphFlag &= ((mesh->GetMorphTargetCount() == morphCount) && (mesh->GetArrayDescriptor(kArrayMorphHash)->elementCount == morphCount));
	}

	if (!morphFlag)
	{
		morphCount = 0;
	}

	SetGeometryFlags(geometryFlags);
	SetCollisionExclusionMask(collisionMask);

	SetGeometryLevelCount(levelCount);
	Mesh *finalMesh = new Mesh[levelCount];

	for (machine level = 0; level < levelCount; level++)
	{
		Mesh	outputMesh;

		int32 outputVertexCount = 0;
		int32 outputPrimitiveCount = 0;
		int32 outputSurfaceCount = 0;
		unsigned_int32 outputSkinSize = 0;

		for (machine a = 0; a < geometryCount; a++)
		{
			object = geometryArray[a]->GetObject();
			outputSurfaceCount += Max(object->GetSurfaceCount(), 1);

			const Mesh *inputMesh = object->GetGeometryLevel(Min(level, object->GetGeometryLevelCount() - 1));
			outputVertexCount += inputMesh->GetVertexCount();
			outputPrimitiveCount += inputMesh->GetPrimitiveCount();

			if (skinFlag)
			{
				outputSkinSize += inputMesh->GetSkinWeightSize();
			}
		}

		if (level == 0)
		{
			SetSurfaceCount(outputSurfaceCount);
		}

		desc[0].identifier = kArrayPosition;
		desc[0].elementCount = outputVertexCount;
		desc[0].elementSize = sizeof(Point3D);
		desc[0].componentCount = 3;

		desc[1].identifier = kArrayNormal;
		desc[1].elementCount = outputVertexCount;
		desc[1].elementSize = sizeof(Vector3D);
		desc[1].componentCount = 3;

		int32 arrayCount = 2;

		if (colorCount != 0)
		{
			desc[arrayCount].identifier = kArrayColor;
			desc[arrayCount].elementCount = outputVertexCount;
			desc[arrayCount].elementSize = 4;
			desc[arrayCount].componentCount = 1;
			arrayCount++;
		}

		for (machine a = 0; a < texcoordCount; a++)
		{
			desc[arrayCount].identifier = kArrayTexcoord0 + a;
			desc[arrayCount].elementCount = outputVertexCount;
			desc[arrayCount].elementSize = sizeof(Point2D);
			desc[arrayCount].componentCount = 2;
			arrayCount++;
		}

		desc[arrayCount].identifier = kArraySurfaceIndex;
		desc[arrayCount].elementCount = outputVertexCount;
		desc[arrayCount].elementSize = 2;
		desc[arrayCount].componentCount = 1;
		arrayCount++;

		desc[arrayCount].identifier = kArrayPrimitive;
		desc[arrayCount].elementCount = outputPrimitiveCount;
		desc[arrayCount].elementSize = sizeof(Triangle);
		desc[arrayCount].componentCount = 1;
		arrayCount++;

		if (skinFlag)
		{
			desc[arrayCount].identifier = kArrayBoneHash;
			desc[arrayCount].elementCount = boneCount;
			desc[arrayCount].elementSize = 4;
			desc[arrayCount].componentCount = 1;
			arrayCount++;

			desc[arrayCount].identifier = kArrayInverseBindTransform;
			desc[arrayCount].elementCount = boneCount;
			desc[arrayCount].elementSize = sizeof(Transform4D);
			desc[arrayCount].componentCount = 16;
			arrayCount++;
		}

		if (morphFlag)
		{
			desc[arrayCount].identifier = kArrayMorphHash;
			desc[arrayCount].elementCount = morphCount;
			desc[arrayCount].elementSize = 4;
			desc[arrayCount].componentCount = 1;
			arrayCount++;
		}

		outputMesh.AllocateStorage(outputVertexCount, arrayCount, desc, outputSkinSize, morphCount);

		int32 vertexOffset = 0;
		int32 primitiveOffset = 0;
		int32 surfaceOffset = 0;
		unsigned_int32 skinOffset = 0;

		for (machine a = 0; a < geometryCount; a++)
		{
			const Geometry *geometry = geometryArray[a];
			object = geometry->GetObject();

			const Mesh *inputMesh = object->GetGeometryLevel(Min(level, object->GetGeometryLevelCount() - 1));
			int32 inputVertexCount = inputMesh->GetVertexCount();

			const Point3D *inputPosition = inputMesh->GetArray<Point3D>(kArrayPosition);
			const Vector3D *inputNormal = inputMesh->GetArray<Vector3D>(kArrayNormal);
			Point3D *outputPosition = outputMesh.GetArray<Point3D>(kArrayPosition) + vertexOffset;
			Vector3D *outputNormal = outputMesh.GetArray<Vector3D>(kArrayNormal) + vertexOffset;

			if (geometry == transformable)
			{
				MemoryMgr::CopyMemory(inputPosition, outputPosition, inputVertexCount * sizeof(Point3D));
				MemoryMgr::CopyMemory(inputNormal, outputNormal, inputVertexCount * sizeof(Vector3D));
			}
			else
			{
				Transform4D transform = transformable->GetInverseWorldTransform() * geometry->GetWorldTransform();
				Transform4D inverse = geometry->GetInverseWorldTransform() * transformable->GetWorldTransform();

				for (machine b = 0; b < inputVertexCount; b++)
				{
					outputPosition[b] = transform * inputPosition[b];
					outputNormal[b] = inputNormal[b] * inverse;
				}
			}

			if (colorCount != 0)
			{
				const Color4C *inputColor = inputMesh->GetArray<Color4C>(kArrayColor);
				Color4C *outputColor = outputMesh.GetArray<Color4C>(kArrayColor) + vertexOffset;

				if (inputColor)
				{
					MemoryMgr::CopyMemory(inputColor, outputColor, inputVertexCount * sizeof(Color4C));
				}
				else
				{
					MemoryMgr::FillMemory(outputColor, inputVertexCount * sizeof(Color4C), 0xFF);
				}
			}

			for (machine b = 0; b < texcoordCount; b++)
			{
				const Point2D *inputTexcoord = inputMesh->GetArray<Point2D>(kArrayTexcoord0 + b);
				Point2D *outputTexcoord = outputMesh.GetArray<Point2D>(kArrayTexcoord0 + b) + vertexOffset;

				if (inputTexcoord)
				{
					MemoryMgr::CopyMemory(inputTexcoord, outputTexcoord, inputVertexCount * sizeof(Point2D));
				}
				else
				{
					MemoryMgr::ClearMemory(outputTexcoord, inputVertexCount * sizeof(Point2D));
				}
			}

			const unsigned_int16 *inputSurfaceIndex = inputMesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
			unsigned_int16 *outputSurfaceIndex = outputMesh.GetArray<unsigned_int16>(kArraySurfaceIndex) + vertexOffset;

			if (inputSurfaceIndex)
			{
				for (machine b = 0; b < inputVertexCount; b++)
				{
					outputSurfaceIndex[b] = (unsigned_int16) (inputSurfaceIndex[b] + surfaceOffset);
				}
			}
			else
			{
				for (machine b = 0; b < inputVertexCount; b++)
				{
					outputSurfaceIndex[b] = (unsigned_int16) surfaceOffset;
				}
			}

			const Triangle *inputTriangle = inputMesh->GetArray<Triangle>(kArrayPrimitive);
			Triangle *outputTriangle = outputMesh.GetArray<Triangle>(kArrayPrimitive) + primitiveOffset;

			int32 inputPrimitiveCount = inputMesh->GetPrimitiveCount();
			for (machine b = 0; b < inputPrimitiveCount; b++)
			{
				outputTriangle[b].Set(inputTriangle[b].index[0] + vertexOffset, inputTriangle[b].index[1] + vertexOffset, inputTriangle[b].index[2] + vertexOffset);
			}

			if (skinFlag)
			{
				if (a == 0)
				{
					const unsigned_int32 *inputBoneHash = inputMesh->GetArray<unsigned_int32>(kArrayBoneHash);
					unsigned_int32 *outputBoneHash = outputMesh.GetArray<unsigned_int32>(kArrayBoneHash);
					MemoryMgr::CopyMemory(inputBoneHash, outputBoneHash, boneCount * 4);

					const Transform4D *inputTransform = inputMesh->GetArray<Transform4D>(kArrayInverseBindTransform);
					Transform4D *outputTransform = outputMesh.GetArray<Transform4D>(kArrayInverseBindTransform);
					MemoryMgr::CopyMemory(inputTransform, outputTransform, boneCount * sizeof(Transform4D));
				}

				unsigned_int32 inputSkinSize = inputMesh->GetSkinWeightSize();
				MemoryMgr::CopyMemory(inputMesh->GetSkinWeightData(), reinterpret_cast<char *>(outputMesh.GetSkinWeightData()) + skinOffset, inputSkinSize);

				skinOffset += inputSkinSize;
			}

			if (morphFlag)
			{
				if (a == 0)
				{
					const unsigned_int32 *inputMorphHash = inputMesh->GetArray<unsigned_int32>(kArrayMorphHash);
					unsigned_int32 *outputMorphHash = outputMesh.GetArray<unsigned_int32>(kArrayMorphHash);
					MemoryMgr::CopyMemory(inputMorphHash, outputMorphHash, morphCount * 4);
				}

				MorphAttrib *outputMorphAttrib = outputMesh.GetMorphTargetData() + vertexOffset;
				for (machine m = 0; m < morphCount; m++)
				{
					MemoryMgr::CopyMemory(inputMesh->GetMorphTargetData(), outputMorphAttrib, inputVertexCount * sizeof(MorphAttrib));
					outputMorphAttrib += outputVertexCount;
				}
			}

			vertexOffset += inputVertexCount;
			primitiveOffset += inputPrimitiveCount;
			surfaceOffset += Max(object->GetSurfaceCount(), 1);
		}

		outputMesh.WeldMesh(0.001F);
		finalMesh[level].BuildTangentArray(&outputMesh);

		if (morphFlag)
		{
			for (machine m = 0; m < morphCount; m++)
			{
				finalMesh[level].CalculateMorphTangentArray(m);
			}
		}
	}

	int32 surfaceOffset = 0;
	for (machine a = 0; a < geometryCount; a++)
	{
		const Geometry *geometry = geometryArray[a];
		object = geometry->GetObject();

		int32 inputSurfaceCount = object->GetSurfaceCount();
		if (inputSurfaceCount != 0)
		{
			for (machine b = 0; b < inputSurfaceCount; b++)
			{
				SurfaceData *outputSurfaceData = GetSurfaceData(surfaceOffset++);

				const SurfaceData *inputSurfaceData = object->GetSurfaceData(b);
				outputSurfaceData->surfaceFlags = inputSurfaceData->surfaceFlags;
				outputSurfaceData->materialIndex = (unsigned_int16) MaxZero(materialArray.FindElement(geometry->GetMaterialObject(inputSurfaceData->materialIndex)));

				if (geometry == transformable)
				{
					outputSurfaceData->textureAlignData[0] = inputSurfaceData->textureAlignData[0];
					outputSurfaceData->textureAlignData[1] = inputSurfaceData->textureAlignData[1];
				}
				else
				{
					Transform4D inverseTransform = geometry->GetInverseWorldTransform() * transformable->GetWorldTransform();
					for (machine c = 0; c < 2; c++)
					{
						TextureAlignMode mode = inputSurfaceData->textureAlignData[c].alignMode;
						const Antivector4D& plane = inputSurfaceData->textureAlignData[c].alignPlane;

						outputSurfaceData->textureAlignData[c].alignMode = mode;
						if ((mode == kTextureAlignObjectPlane) || (mode == kTextureAlignGlobalObjectPlane))
						{
							outputSurfaceData->textureAlignData[c].alignPlane = plane * inverseTransform;
						}
						else
						{
							outputSurfaceData->textureAlignData[c].alignPlane = plane;
						}
					}
				}
			}
		}
		else
		{
			SurfaceData *outputSurfaceData = GetSurfaceData(surfaceOffset++);

			outputSurfaceData->surfaceFlags = 0;
			outputSurfaceData->materialIndex = (unsigned_int16) MaxZero(materialArray.FindElement(geometry->GetMaterialObject(0)));

			outputSurfaceData->textureAlignData[0].alignMode = kTextureAlignNatural;
			outputSurfaceData->textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
			outputSurfaceData->textureAlignData[1].alignMode = kTextureAlignNatural;
			outputSurfaceData->textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
		}
	}

	for (machine level = 0; level < levelCount; level++)
	{
		GetGeometryLevel(level)->BuildSegmentArray(&finalMesh[level], GetSurfaceCount(), GetSurfaceData());
	}

	delete[] finalMesh;

	UpdateBounds();
	BuildCollisionData();
}

GenericGeometryObject::GenericGeometryObject(BooleanOperation operation, const Geometry *geometry1, const Geometry *geometry2, const ImmutableArray<MaterialObject *>& materialArray) : GeometryObject(kGeometryGeneric)
{
	Array<SurfaceData> surfaceData(8);

	const GeometryObject *object1 = geometry1->GetObject();
	const GeometryObject *object2 = geometry2->GetObject();

	SetGeometryFlags(object1->GetGeometryFlags() & object2->GetGeometryFlags());
	int32 levelCount = Max(object1->GetGeometryLevelCount(), object2->GetGeometryLevelCount());
	SetGeometryLevelCount(levelCount);

	for (machine level = 0; level < levelCount; level++)
	{
		Mesh					tempMesh[2];
		List<GeometrySurface>	resultList;

		tempMesh[0].CopyMesh(object1->GetGeometryLevel(Min(level, object1->GetGeometryLevelCount() - 1)));
		tempMesh[1].CopyMesh(object2->GetGeometryLevel(Min(level, object2->GetGeometryLevelCount() - 1)));
		tempMesh[1].TransformMesh(geometry1->GetInverseWorldTransform() * geometry2->GetWorldTransform());

		if (operation == kBooleanUnion)
		{
			tempMesh[1].InvertMesh();
		}

		Array<SurfaceData> *surfaceDataArray = (level == 0) ? &surfaceData : nullptr;
		unsigned_int32 buildFlags = IntersectMeshes(&tempMesh[0], &tempMesh[1], &resultList, geometry1, surfaceDataArray, materialArray);
		int32 primarySurfaceCount = resultList.GetElementCount();

		if (operation == kBooleanUnion)
		{
			tempMesh[0].InvertMesh();
			tempMesh[1].InvertMesh();
		}

		buildFlags |= IntersectMeshes(&tempMesh[1], &tempMesh[0], &resultList, geometry2, surfaceDataArray, materialArray);

		if (level == 0)
		{
			int32 surfaceCount = surfaceData.GetElementCount();
			if (surfaceCount != 0)
			{
				Transform4D inverseTransform = geometry2->GetInverseWorldTransform() * geometry1->GetWorldTransform();

				SetSurfaceCount(surfaceCount);
				for (machine a = 0; a < surfaceCount; a++)
				{
					const SurfaceData *inputSurfaceData = &surfaceData[a];
					SurfaceData *outputSurfaceData = GetSurfaceData(a);

					if (a < primarySurfaceCount)
					{
						*GetSurfaceData(a) = *inputSurfaceData;
					}
					else
					{
						outputSurfaceData->surfaceFlags = inputSurfaceData->surfaceFlags;
						outputSurfaceData->materialIndex = inputSurfaceData->materialIndex;

						for (machine b = 0; b < 2; b++)
						{
							TextureAlignMode mode = inputSurfaceData->textureAlignData[b].alignMode;
							const Antivector4D& plane = inputSurfaceData->textureAlignData[b].alignPlane;

							outputSurfaceData->textureAlignData[b].alignMode = mode;
							if ((mode == kTextureAlignObjectPlane) || (mode == kTextureAlignGlobalObjectPlane))
							{
								outputSurfaceData->textureAlignData[b].alignPlane = plane * inverseTransform;
							}
							else
							{
								outputSurfaceData->textureAlignData[b].alignPlane = plane;
							}
						}
					}
				}
			}
		}

		BuildMesh(level, buildFlags | (kBuildMeshWeld | kBuildMeshOptimize), &resultList, materialArray.GetElementCount());
	}

	UpdateBounds();
	BuildCollisionData();
}

GenericGeometryObject::~GenericGeometryObject()
{
}

void GenericGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GeometryObject::Pack(data, packFlags);

	data << ChunkHeader('BSPH', sizeof(Point3D) + 4);
	data << boundingSphere.GetCenter();
	data << boundingSphere.GetRadius();

	data << ChunkHeader('BBOX', sizeof(Box3D));
	data << boundingBox;

	data << TerminatorChunk;
}

void GenericGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<GenericGeometryObject>(data, unpackFlags);
}

bool GenericGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BSPH':
		{
			Point3D		center;
			float		radius;

			data >> center;
			data >> radius;

			boundingSphere.SetCenter(center);
			boundingSphere.SetRadius(radius);
			return (true);
		}

		case 'BBOX':

			data >> boundingBox;
			return (true);
	}

	return (false);
}

int32 GenericGeometryObject::GetObjectSize(float *size) const
{
	Vector3D boundsSize = boundingBox.max - boundingBox.min;
	size[0] = boundsSize.x;
	size[1] = boundsSize.y;
	size[2] = boundsSize.z;
	return (3);
}

void GenericGeometryObject::SetObjectSize(const float *size)
{
	Vector3D scale = boundingBox.max - boundingBox.min;
	scale.x = (scale.x > 0.0F) ? size[0] / scale.x : 1.0F;
	scale.y = (scale.y > 0.0F) ? size[1] / scale.y : 1.0F;
	scale.z = (scale.z > 0.0F) ? size[2] / scale.z : 1.0F;

	int32 levelCount = GetGeometryLevelCount();
	for (machine a = 0; a < levelCount; a++)
	{
		GetGeometryLevel(a)->ScaleMesh(scale);
	}

	boundingBox.Scale(scale);
	SetBoundingSphere((boundingBox.min + boundingBox.max) * 0.5F, Magnitude(boundingBox.max - boundingBox.min) * K::sqrt_2_over_2);
}

void GenericGeometryObject::BuildMesh(int32 level, unsigned_int32 flags, const List<GeometrySurface> *surfaceList, int32 materialCount, const SkinData *skinData, const MorphData *morphData)
{
	ArrayDescriptor		desc[kMaxGeometryTexcoordCount + 8];
	Point2D				*texcoord[kMaxGeometryTexcoordCount];
	Mesh				outputMesh[2];

	int32 vertexCount = 0;
	int32 texcoordCount = 1;
	unsigned_int32 surfaceFlags = ~0;

	const GeometrySurface *surface = surfaceList->First();
	while (surface)
	{
		texcoordCount = Max(texcoordCount, surface->texcoordCount);
		surfaceFlags &= surface->surfaceFlags;

		int32 count = surface->polygonList.GetElementCount();
		vertexCount += count * 3;

		surface = surface->Next();
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	int32 arrayCount = 1;
	if (surfaceFlags & kSurfaceValidNormals)
	{
		desc[1].identifier = kArrayNormal;
		desc[1].elementCount = vertexCount;
		desc[1].elementSize = sizeof(Vector3D);
		desc[1].componentCount = 3;

		arrayCount = 2;
	}

	if (surfaceFlags & kSurfaceValidTangents)
	{
		desc[arrayCount].identifier = kArrayTangent;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = sizeof(Vector4D);
		desc[arrayCount].componentCount = 4;
		arrayCount++;
	}

	if ((flags & kBuildMeshColor) || (surfaceFlags & kSurfaceValidColors))
	{
		desc[arrayCount].identifier = kArrayColor;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	for (machine a = 0; a < texcoordCount; a++)
	{
		desc[arrayCount].identifier = kArrayTexcoord0 + a;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = sizeof(Point2D);
		desc[arrayCount].componentCount = 2;
		arrayCount++;
	}

	int32 surfaceCount = GetSurfaceCount();
	if (surfaceCount > 1)
	{
		desc[arrayCount].identifier = kArraySurfaceIndex;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 2;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	unsigned_int32 skinSize = 0;
	if (skinData)
	{
		desc[arrayCount].identifier = kArrayBoneHash;
		desc[arrayCount].elementCount = skinData->boneCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;

		desc[arrayCount].identifier = kArrayInverseBindTransform;
		desc[arrayCount].elementCount = skinData->boneCount;
		desc[arrayCount].elementSize = sizeof(Transform4D);
		desc[arrayCount].componentCount = 16;
		arrayCount++;

		surface = surfaceList->First();
		while (surface)
		{
			const GeometryPolygon *polygon = surface->polygonList.First();
			while (polygon)
			{
				const GeometryVertex *v = polygon->vertexList.First();
				for (machine a = 0; a < 3; a++)
				{
					skinSize += skinData->skinWeightTable[v->index]->GetSize();
					v = v->Next();
				}

				polygon = polygon->Next();
			}

			surface = surface->Next();
		}
	}

	int32 morphCount = 0;
	if (morphData)
	{
		morphCount = morphData->morphCount;

		desc[arrayCount].identifier = kArrayMorphHash;
		desc[arrayCount].elementCount = morphCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	outputMesh[0].AllocateStorage(vertexCount, arrayCount, desc, skinSize, morphCount);

	Point3D *position = outputMesh[0].GetArray<Point3D>(kArrayPosition);
	Vector3D *normal = outputMesh[0].GetArray<Vector3D>(kArrayNormal);
	Vector4D *tangent = outputMesh[0].GetArray<Vector4D>(kArrayTangent);
	Color4C *color = outputMesh[0].GetArray<Color4C>(kArrayColor);

	for (machine a = 0; a < texcoordCount; a++)
	{
		texcoord[a] = outputMesh[0].GetArray<Point2D>(kArrayTexcoord0 + a);
	}

	unsigned_int16 *surfaceIndex = outputMesh[0].GetArray<unsigned_int16>(kArraySurfaceIndex);
	SkinWeight *skinWeight = outputMesh[0].GetSkinWeightData();
	MorphAttrib *morphAttrib = outputMesh[0].GetMorphTargetData();

	surfaceCount = 0;
	surface = surfaceList->First();
	while (surface)
	{
		const GeometryPolygon *polygon = surface->polygonList.First();
		while (polygon)
		{
			const GeometryVertex	*v[3];

			v[0] = polygon->vertexList.First();
			v[1] = v[0]->Next();
			v[2] = v[1]->Next();

			position[0] = v[0]->position;
			position[1] = v[1]->position;
			position[2] = v[2]->position;
			position += 3;

			if (surfaceFlags & kSurfaceValidNormals)
			{
				normal[0] = v[0]->normal;
				normal[1] = v[1]->normal;
				normal[2] = v[2]->normal;
				normal += 3;
			}

			if (surfaceFlags & kSurfaceValidTangents)
			{
				tangent[0] = v[0]->tangent;
				tangent[1] = v[1]->tangent;
				tangent[2] = v[2]->tangent;
				tangent += 3;
			}

			if (color)
			{
				for (machine a = 0; a < 3; a++)
				{
					const ColorRGBA& c = v[a]->color;
					color->Set((unsigned_int32) (c.red * 255.0F), (unsigned_int32) (c.green * 255.0F), (unsigned_int32) (c.blue * 255.0F), (unsigned_int32) (c.alpha * 255.0F));
					color++;
				}
			}

			for (machine a = 0; a < texcoordCount; a++)
			{
				texcoord[a][0] = v[0]->texcoord[a];
				texcoord[a][1] = v[1]->texcoord[a];
				texcoord[a][2] = v[2]->texcoord[a];
				texcoord[a] += 3;
			}

			if (surfaceIndex)
			{
				surfaceIndex[0] = (unsigned_int16) surfaceCount;
				surfaceIndex[1] = (unsigned_int16) surfaceCount;
				surfaceIndex[2] = (unsigned_int16) surfaceCount;
				surfaceIndex += 3;
			}

			if (skinData)
			{
				for (machine a = 0; a < 3; a++)
				{
					const SkinWeight *sw = skinData->skinWeightTable[v[a]->index];

					int32 boneCount = sw->boneCount;
					skinWeight->boneCount = boneCount;

					BoneWeight *boneWeight = skinWeight->boneWeight;
					const BoneWeight *bw = sw->boneWeight;
					for (machine b = 0; b < boneCount; b++)
					{
						*boneWeight = *bw;
						boneWeight++;
						bw++;
					}

					skinWeight = reinterpret_cast<SkinWeight *>(boneWeight);
				}
			}

			if (morphData)
			{
				unsigned_int32 i1 = v[0]->index;
				unsigned_int32 i2 = v[1]->index;
				unsigned_int32 i3 = v[2]->index;

				MorphAttrib *attrib = morphAttrib;

				for (machine m = 0; m < morphCount; m++)
				{
					const Point3D *morphPosition = morphData->morphPositionTable[m];
					attrib[0].position = morphPosition[i1];
					attrib[1].position = morphPosition[i2];
					attrib[2].position = morphPosition[i3];

					const Vector3D *morphNormal = morphData->morphNormalTable[m];
					if (morphNormal)
					{
						attrib[0].normal = morphNormal[i1];
						attrib[1].normal = morphNormal[i2];
						attrib[2].normal = morphNormal[i3];
					}

					attrib += vertexCount;
				}

				morphAttrib += 3;
			}

			polygon = polygon->Next();
		}

		surfaceCount++;
		surface = surface->Next();
	}

	if (skinData)
	{
		int32 boneCount = skinData->boneCount;
		MemoryMgr::CopyMemory(skinData->boneHashArray, outputMesh[0].GetArray(kArrayBoneHash), boneCount * 4);
		MemoryMgr::CopyMemory(skinData->inverseBindTransformArray, outputMesh[0].GetArray(kArrayInverseBindTransform), boneCount * sizeof(Transform4D));
	}

	if (morphData)
	{
		MemoryMgr::CopyMemory(morphData->morphHashArray, outputMesh[0].GetArray(kArrayMorphHash), morphData->morphCount * 4);
	}

	if (vertexCount != 0)
	{
		if (flags & kBuildMeshWeld)
		{
			outputMesh[0].WeldMesh(0.001F);
		}

		if (flags & kBuildMeshOptimize)
		{
			outputMesh[0].MendMesh(0.001F, 0.001F, 0.001F);
		}

		outputMesh[1].UnifyMesh(&outputMesh[0]);

		unsigned_int32 x = 1;
		if ((flags & kBuildMeshOptimize) && (surfaceCount > 1))
		{
			outputMesh[0].SimplifyBoundaryEdges(&outputMesh[1]);
			x = 0;
		}

		if (!(surfaceFlags & kSurfaceValidNormals))
		{
			outputMesh[x ^ 1].BuildNormalArray(&outputMesh[x]);
			x ^= 1;
		}

		Mesh *finalMesh = GetGeometryLevel(level);

		if (materialCount > 1)
		{
			outputMesh[x ^ 1].BuildTangentArray(&outputMesh[x]);
			finalMesh->BuildSegmentArray(&outputMesh[x ^ 1], surfaceCount, GetSurfaceData());
		}
		else
		{
			finalMesh->BuildTangentArray(&outputMesh[x]);
		}

		if (morphData)
		{
			for (machine m = 0; m < morphCount; m++)
			{
				if (!morphData->morphNormalTable[m])
				{
					finalMesh->CalculateMorphNormalArray(m);
				}

				finalMesh->CalculateMorphTangentArray(m);

				if (morphData->relativeArray[m])
				{
					finalMesh->SubtractMorphBase(m, morphData->baseIndexArray[m]);
				}
			}
		}
	}
	else
	{
		GetGeometryLevel(level)->CopyMesh(&outputMesh[0]);
	}
}

unsigned_int32 GenericGeometryObject::IntersectMeshes(const Mesh *targetMesh, const Mesh *auxMesh, List<GeometrySurface> *resultList, const Geometry *targetGeometry, Array<SurfaceData> *surfaceDataArray, const ImmutableArray<MaterialObject *>& materialArray)
{
	Point3D		polygonVertex[3];
	Vector3D	polygonNormal[3];
	ColorRGBA	polygonColor[3];
	Point2D		polygonTexcoord[3];

	float auxVolume = auxMesh->CalculateVolume();

	int32 primitiveCount = targetMesh->GetPrimitiveCount();
	const Triangle *triangle = targetMesh->GetArray<Triangle>(kArrayPrimitive);

	const Point3D *vertex = targetMesh->GetArray<Point3D>(kArrayPosition);
	const Vector3D *normal = targetMesh->GetArray<Vector3D>(kArrayNormal);
	const Point2D *texcoord = targetMesh->GetArray<Point2D>(kArrayTexcoord);

	const Color4C *color = nullptr;
	unsigned_int32 buildFlags = 0;

	const ArrayBundle *colorBundle = targetMesh->GetArrayBundle(kArrayColor);
	if ((colorBundle) && (colorBundle->descriptor.componentCount == 1))
	{
		color = targetMesh->GetArray<Color4C>(kArrayColor);
		buildFlags = kBuildMeshColor;
	}
	else
	{
		for (machine a = 0; a < 3; a++)
		{
			polygonColor[a].Set(0.0F, 0.0F, 0.0F, 0.0F);
		}
	}

	const unsigned_int16 *surfaceIndex = targetMesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (surfaceIndex)
	{
		for (machine primitiveIndex = 0; primitiveIndex < primitiveCount;)
		{
			unsigned_int32 inputSurfaceIndex = surfaceIndex[triangle->index[0]];

			GeometrySurface *surface = new GeometrySurface;
			surface->surfaceFlags = kSurfaceValidNormals;
			resultList->Append(surface);

			for (; primitiveIndex < primitiveCount; primitiveIndex++)
			{
				if (surfaceIndex[triangle->index[0]] != inputSurfaceIndex)
				{
					break;
				}

				if (color)
				{
					for (machine a = 0; a < 3; a++)
					{
						unsigned_int32 i = triangle->index[a];
						polygonVertex[a] = vertex[i];
						polygonNormal[a] = normal[i];
						polygonTexcoord[a] = texcoord[i];

						const Color4C& c = color[i];
						polygonColor[a].Set((float) c.GetRed() * K::one_over_255, (float) c.GetGreen() * K::one_over_255, (float) c.GetBlue() * K::one_over_255, (float) c.GetAlpha() * K::one_over_255);
					}
				}
				else
				{
					for (machine a = 0; a < 3; a++)
					{
						unsigned_int32 i = triangle->index[a];
						polygonVertex[a] = vertex[i];
						polygonNormal[a] = normal[i];
						polygonTexcoord[a] = texcoord[i];
					}
				}

				IntersectPolygonAndMesh(polygonVertex, polygonNormal, polygonColor, polygonTexcoord, auxMesh, auxVolume, &surface->polygonList);
				triangle++;
			}

			if (surfaceDataArray)
			{
				int32 surfaceCount = surfaceDataArray->GetElementCount();
				surfaceDataArray->SetElementCount(surfaceCount + 1);

				SurfaceData *outputSurfaceData = &(*surfaceDataArray)[surfaceCount];
				const SurfaceData *inputSurfaceData = targetGeometry->GetObject()->GetSurfaceData(inputSurfaceIndex);

				outputSurfaceData->surfaceFlags = inputSurfaceData->surfaceFlags;
				outputSurfaceData->materialIndex = (unsigned_int16) MaxZero(materialArray.FindElement(targetGeometry->GetMaterialObject(inputSurfaceData->materialIndex)));

				outputSurfaceData->textureAlignData[0] = inputSurfaceData->textureAlignData[0];
				outputSurfaceData->textureAlignData[1] = inputSurfaceData->textureAlignData[1];
			}
		}
	}
	else
	{
		GeometrySurface *surface = new GeometrySurface;
		surface->surfaceFlags = kSurfaceValidNormals;
		resultList->Append(surface);

		for (machine primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++)
		{
			if (color)
			{
				for (machine a = 0; a < 3; a++)
				{
					unsigned_int32 i = triangle->index[a];
					polygonVertex[a] = vertex[i];
					polygonNormal[a] = normal[i];
					polygonTexcoord[a] = texcoord[i];

					const Color4C& c = color[i];
					polygonColor[a].Set((float) c.GetRed() * K::one_over_255, (float) c.GetGreen() * K::one_over_255, (float) c.GetBlue() * K::one_over_255, (float) c.GetAlpha() * K::one_over_255);
				}
			}
			else
			{
				for (machine a = 0; a < 3; a++)
				{
					unsigned_int32 i = triangle->index[a];
					polygonVertex[a] = vertex[i];
					polygonNormal[a] = normal[i];
					polygonTexcoord[a] = texcoord[i];
				}
			}

			IntersectPolygonAndMesh(polygonVertex, polygonNormal, polygonColor, polygonTexcoord, auxMesh, auxVolume, &surface->polygonList);
			triangle++;
		}

		if (surfaceDataArray)
		{
			int32 surfaceCount = surfaceDataArray->GetElementCount();
			surfaceDataArray->SetElementCount(surfaceCount + 1);

			SurfaceData *outputSurfaceData = &(*surfaceDataArray)[surfaceCount];
			outputSurfaceData->surfaceFlags = 0;
			outputSurfaceData->materialIndex = (unsigned_int16) MaxZero(materialArray.FindElement(targetGeometry->GetMaterialObject(0)));

			outputSurfaceData->textureAlignData[0].alignMode = kTextureAlignNatural;
			outputSurfaceData->textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
			outputSurfaceData->textureAlignData[1].alignMode = kTextureAlignNatural;
			outputSurfaceData->textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
		}
	}

	return (buildFlags);
}

void GenericGeometryObject::IntersectPolygonAndMesh(const Point3D *polygonVertex, const Vector3D *polygonNormal, const ColorRGBA *polygonColor, const Point2D *polygonTexcoord, const Mesh *mesh, float geometryVolume, List<GeometryPolygon> *resultList)
{
	List<BooleanLoop>		positiveLoopList;
	List<BooleanLoop>		negativeLoopList;
	List<GeometryPolygon>	inputList[2];
	int32					vertexCount[2];
	const Point3D			*vertexTable[2];

	Antivector4D plane(polygonVertex[0], polygonVertex[1], polygonVertex[2]);
	plane.Standardize();
	BuildBooleanLoops(plane, mesh, &positiveLoopList, &negativeLoopList);

	int32 positiveLoopCount = positiveLoopList.GetElementCount();
	int32 negativeLoopCount = negativeLoopList.GetElementCount();

	if (positiveLoopCount + negativeLoopCount == 0)
	{
		if (geometryVolume < 0.0F)
		{
			GeometryPolygon *gp = new GeometryPolygon;
			resultList->Append(gp);

			for (machine a = 0; a < 3; a++)
			{
				GeometryVertex *gv = new GeometryVertex;
				gp->vertexList.Append(gv);

				gv->position = polygonVertex[a];
				gv->normal = polygonNormal[a];
				gv->color = polygonColor[a];
				gv->texcoord[0] = polygonTexcoord[a];
			}
		}

		return;
	}

	if (positiveLoopCount != 0)
	{
		int32 maxLoopVertexCount = 0;
		const BooleanLoop *loop = positiveLoopList.First();
		do
		{
			maxLoopVertexCount = Max(maxLoopVertexCount, loop->vertexCount);
			loop = loop->Next();
		} while (loop);

		int32 resultCount = 3 + maxLoopVertexCount;

		Buffer buffer(resultCount * (sizeof(Point3D) + sizeof(Vector3D) + sizeof(ColorRGBA) + sizeof(Point2D)) + (resultCount - 2) * sizeof(Triangle));
		Point3D *resultVertex = buffer.GetPtr<Point3D>();
		Vector3D *resultNormal = resultVertex + resultCount;
		ColorRGBA *resultColor = reinterpret_cast<ColorRGBA *>(resultNormal + resultCount);
		Point2D *resultTexcoord = reinterpret_cast<Point2D *>(resultColor + resultCount);
		Triangle *resultTriangle = reinterpret_cast<Triangle *>(resultTexcoord + resultCount);

		vertexCount[0] = 3;
		vertexTable[0] = polygonVertex;

		loop = positiveLoopList.First();
		do
		{
			vertexCount[1] = loop->vertexCount;
			vertexTable[1] = loop->vertex;

			Math::IntersectConvexPolygons(vertexCount, vertexTable, plane.GetAntivector3D(), &resultCount, resultVertex);
			if (resultCount >= 3)
			{
				CalculatePolygonAttributes(polygonVertex, polygonNormal, polygonColor, polygonTexcoord, resultCount, resultVertex, resultNormal, resultColor, resultTexcoord);
				int32 triangleCount = Math::TriangulatePolygon(resultCount, resultVertex, plane.GetAntivector3D(), resultTriangle);

				for (machine a = 0; a < triangleCount; a++)
				{
					const Triangle *t = &resultTriangle[a];

					GeometryPolygon *gp = new GeometryPolygon;
					for (machine b = 0; b < 3; b++)
					{
						int32 index = t->index[b];

						GeometryVertex *gv = new GeometryVertex;
						gv->position = resultVertex[index];
						gv->normal = resultNormal[index];
						gv->color = resultColor[index];
						gv->texcoord[0] = resultTexcoord[index];
						gp->vertexList.Append(gv);
					}

					inputList[0].Append(gp);
				}
			}

			loop = loop->Next();
		} while (loop);
	}

	int32 parity = 0;

	if (negativeLoopCount != 0)
	{
		if (geometryVolume < 0.0F)
		{
			for (;;)
			{
				GeometryPolygon *gp = inputList[0].First();
				if (!gp)
				{
					break;
				}

				resultList->Append(gp);
			}

			GeometryPolygon *gp = new GeometryPolygon;
			inputList[0].Append(gp);

			for (machine a = 0; a < 3; a++)
			{
				GeometryVertex *gv = new GeometryVertex;
				gp->vertexList.Append(gv);

				gv->position = polygonVertex[a];
				gv->normal = polygonNormal[a];
				gv->color = polygonColor[a];
				gv->texcoord[0] = polygonTexcoord[a];
			}
		}

		const BooleanLoop *loop = negativeLoopList.First();
		do
		{
			int32 subtractCount = loop->vertexCount;
			vertexCount[1] = subtractCount;
			vertexTable[1] = loop->vertex;

			for (;;)
			{
				GeometryPolygon *inputPolygon = inputList[parity].First();
				if (!inputPolygon)
				{
					break;
				}

				int32 positiveCount = inputPolygon->vertexList.GetElementCount();
				int32 triangleCount = positiveCount + subtractCount;
				int32 resultCount = triangleCount + subtractCount;

				Buffer buffer((positiveCount + resultCount) * sizeof(Point3D) + resultCount * (sizeof(Vector3D) + sizeof(ColorRGBA) + sizeof(Point2D)) + triangleCount * sizeof(Triangle));
				Point3D *positiveVertex = buffer.GetPtr<Point3D>();
				Point3D *resultVertex = positiveVertex + positiveCount;
				Vector3D *resultNormal = resultVertex + resultCount;
				ColorRGBA *resultColor = reinterpret_cast<ColorRGBA *>(resultNormal + resultCount);
				Point2D *resultTexcoord = reinterpret_cast<Point2D *>(resultColor + resultCount);
				Triangle *resultTriangle = reinterpret_cast<Triangle *>(resultTexcoord + resultCount);

				const GeometryVertex *inputVertex = inputPolygon->vertexList.First();
				for (machine a = 0; a < positiveCount; a++)
				{
					positiveVertex[a] = inputVertex->position;
					inputVertex = inputVertex->Next();
				}

				vertexCount[0] = positiveCount;
				vertexTable[0] = positiveVertex;

				if (Math::SubtractConvexPolygons(vertexCount, vertexTable, plane.GetAntivector3D(), &resultCount, &triangleCount, resultVertex, resultTriangle))
				{
					CalculatePolygonAttributes(polygonVertex, polygonNormal, polygonColor, polygonTexcoord, resultCount, resultVertex, resultNormal, resultColor, resultTexcoord);

					for (machine a = 0; a < triangleCount; a++)
					{
						const Triangle *t = &resultTriangle[a];

						GeometryPolygon *gp = new GeometryPolygon;
						for (machine b = 0; b < 3; b++)
						{
							int32 index = t->index[b];

							GeometryVertex *gv = new GeometryVertex;
							gv->position = resultVertex[index];
							gv->normal = resultNormal[index];
							gv->color = resultColor[index];
							gv->texcoord[0] = resultTexcoord[index];
							gp->vertexList.Append(gv);
						}

						inputList[parity ^ 1].Append(gp);
					}

					delete inputPolygon;
				}
				else
				{
					inputList[parity ^ 1].Append(inputPolygon);
				}
			}

			parity ^= 1;
			loop = loop->Next();
		} while (loop);
	}

	for (;;)
	{
		GeometryPolygon *gp = inputList[parity].First();
		if (!gp)
		{
			break;
		}

		resultList->Append(gp);
	}
}

void GenericGeometryObject::BuildBooleanLoops(const Antivector4D& plane, const Mesh *mesh, List<BooleanLoop> *positiveList, List<BooleanLoop> *negativeList)
{
	List<BooleanEdge>	edgeList;

	int32 primitiveCount = mesh->GetPrimitiveCount();
	const Triangle *triangle = mesh->GetArray<Triangle>(kArrayPrimitive);
	const Point3D *vertex = mesh->GetArray<Point3D>(kArrayPosition);

	for (machine a = 0; a < primitiveCount; a++)
	{
		BooleanEdge *edge = nullptr;

		unsigned_int32 index1 = triangle->index[2];
		for (machine b = 0; b < 3; b++)
		{
			unsigned_int32 index2 = triangle->index[b];

			const Point3D& p1 = vertex[index1];
			const Point3D& p2 = vertex[index2];

			float d1 = plane ^ p1;
			float d2 = plane ^ p2;

			int32 c1 = (d1 > 0.0F) ? 1 : -1;
			int32 c2 = (d2 > 0.0F) ? 1 : -1;

			if (c1 > c2)
			{
				float t = d2 / (d1 - d2);
				Point3D q = p2 - (p1 - p2) * t;

				if (edge)
				{
					if ((edge->validFlags & 1) == 0)
					{
						edge->validFlags = 3;
						edge->endpoint[0] = q;
						break;
					}
				}
				else
				{
					edge = new BooleanEdge(0, q);
					edgeList.Append(edge);
				}
			}
			else if (c2 > c1)
			{
				float t = d1 / (d2 - d1);
				Point3D q = p1 - (p2 - p1) * t;

				if (edge)
				{
					if ((edge->validFlags & 2) == 0)
					{
						edge->validFlags = 3;
						edge->endpoint[1] = q;
						break;
					}
				}
				else
				{
					edge = new BooleanEdge(1, q);
					edgeList.Append(edge);
				}
			}

			index1 = index2;
		}

		if (edge)
		{
			if (edge->validFlags != 3)
			{
				delete edge;
			}
			else
			{
				const BooleanEdge *be = edgeList.First();
				while (be != edge)
				{
					float dp1 = SquaredMag(be->endpoint[0] - edge->endpoint[0]);
					float dp2 = SquaredMag(be->endpoint[1] - edge->endpoint[1]);
					if ((dp1 < kWeldEpsilonSquared) && (dp2 < kWeldEpsilonSquared))
					{
						delete edge;
						break;
					}

					be = be->Next();
				}
			}
		}

		triangle++;
	}

	for (;;)
	{
		List<BooleanEdge>	loopEdgeList;

		BooleanEdge *edge = edgeList.First();
		if (!edge)
		{
			break;
		}

		loopEdgeList.Append(edge);
		int32 edgeCount = 1;

		for (;;)
		{
			BooleanEdge *next = edgeList.First();
			while (next)
			{
				if (SquaredMag(next->endpoint[0] - edge->endpoint[1]) < kWeldEpsilonSquared)
				{
					edge = next;
					loopEdgeList.Append(next);
					edgeCount++;
					break;
				}

				next = next->Next();
			}

			if (!next)
			{
				break;
			}
		}

		if (edgeCount < 3)
		{
			continue;
		}

		BooleanEdge *e3 = loopEdgeList.First();
		BooleanEdge *e2 = loopEdgeList.Last();
		BooleanEdge *e1 = e2->Previous();
		do
		{
			Vector3D dp1 = e2->endpoint[0] - e1->endpoint[0];
			Vector3D dp2 = e3->endpoint[0] - e2->endpoint[0];
			if (dp1 * dp2 * InverseMag(dp1) * InverseMag(dp2) > kCollinearEdgeEpsilon)
			{
				delete e2;
				edgeCount--;
			}
			else
			{
				e1 = e2;
			}

			e2 = e3;
			e3 = e3->Next();
		} while (e3);

		if (edgeCount < 3)
		{
			continue;
		}

		BooleanLoop *loop = new BooleanLoop(edgeCount);

		edge = loopEdgeList.First();
		for (machine a = 0; a < edgeCount; a++)
		{
			loop->vertex[a] = edge->endpoint[0];
			edge = edge->Next();
		}

		const Vector3D& normal = plane.GetAntivector3D();
		if (Math::GetPolygonArea(edgeCount, loop->vertex, normal) >= 0.0F)
		{
			ConvexDecomposeLoop(normal, loop, positiveList);
		}
		else
		{
			ConvexDecomposeLoop(-normal, loop, negativeList);
		}
	}
}

void GenericGeometryObject::ConvexDecomposeLoop(const Vector3D& normal, BooleanLoop *inputLoop, List<BooleanLoop> *outputList)
{
	int32 reflexCount = 0;
	int32 vertexCount = inputLoop->vertexCount;
	for (machine a = 0; a < vertexCount; a++)
	{
		reflexCount += inputLoop->ClassifyVertex(a, normal);
	}

	if (reflexCount == 0)
	{
		outputList->Append(inputLoop);
		return;
	}

	const Point3D *vertex = inputLoop->vertex;

	for (;;)
	{
		int32	finish;

		int32 start = inputLoop->GetDecompStart(&finish);
		if (start == -1)
		{
			int32 outputCount = inputLoop->GetActiveVertexCount();
			BooleanLoop *outputLoop = new BooleanLoop(outputCount);
			outputList->Append(outputLoop);

			outputCount = 0;
			for (machine a = 0; a < vertexCount; a++)
			{
				if (inputLoop->active[a])
				{
					outputLoop->vertex[outputCount++] = vertex[a];
				}
			}

			break;
		}

		int32 startNext = finish;
		int32 outputCount = 2;

		for (;;)
		{
			int32 finishPrev = finish;
			finish = inputLoop->GetNextActiveVertex(finish);
			if (inputLoop->reflex[finish])
			{
				outputCount++;
				break;
			}

			const Point3D& p0 = vertex[finishPrev];
			const Point3D& p1 = vertex[finish];
			const Point3D& p2 = vertex[start];
			const Point3D& p3 = vertex[startNext];

			if ((normal % (p1 - p0) * (p2 - p1) < 0.0F) || (normal % (p2 - p1) * (p3 - p2) < 0.0F))
			{
				finish = finishPrev;
				break;
			}

			outputCount++;
		}

		if (outputCount >= 3)
		{
			BooleanLoop *outputLoop = new BooleanLoop(outputCount);
			outputList->Append(outputLoop);

			outputCount = 0;
			for (machine a = start;;)
			{
				outputLoop->vertex[outputCount++] = vertex[a];
				if (a == finish)
				{
					break;
				}

				if (a != start)
				{
					inputLoop->active[a] = false;
				}

				a = inputLoop->GetNextActiveVertex(a);
			}
		}
		else
		{
			inputLoop->active[finish] = false;
		}

		inputLoop->ClassifyVertex(start, normal);
		inputLoop->ClassifyVertex(finish, normal);
	}

	delete inputLoop;
}

void GenericGeometryObject::CalculatePolygonAttributes(const Point3D *polygonVertex, const Vector3D *polygonNormal, const ColorRGBA *polygonColor, const Point2D *polygonTexcoord, int32 vertexCount, const Point3D *vertex, Vector3D *normal, ColorRGBA *color, Point2D *texcoord)
{
	for (machine a = 0; a < vertexCount; a++)
	{
		float	w1, w2, w3;

		const Point3D& p = vertex[a];
		for (machine b = 0; b < 3; b++)
		{
			if (polygonVertex[b] == p)
			{
				normal[a] = polygonNormal[b];
				color[a] = polygonColor[b];
				texcoord[a] = polygonTexcoord[b];
				goto next;
			}
		}

		Math::CalculateBarycentricCoordinates(polygonVertex[0], polygonVertex[1], polygonVertex[2], p, &w1, &w2, &w3);

		normal[a] = Normalize(polygonNormal[0] * w1 + polygonNormal[1] * w2 + polygonNormal[2] * w3);
		color[a] = polygonColor[0] * w1 + polygonColor[1] * w2 + polygonColor[2] * w3;
		texcoord[a] = polygonTexcoord[0] * w1 + polygonTexcoord[1] * w2 + polygonTexcoord[2] * w3;

		next:;
	}
}

bool GenericGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (boundingBox.ExteriorSphere(center, radius));
}

bool GenericGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return (boundingBox.ExteriorSweptSphere(p1, p2, radius));
}

void GenericGeometryObject::UpdateBounds(void)
{
	const Mesh *geometryMesh = GetGeometryLevel(0);
	int32 vertexCount = geometryMesh->GetVertexCount();

	BoundingSphere *sphere = GetBoundingSphere();

	if (vertexCount != 0)
	{
		const Point3D *vertex = geometryMesh->GetArray<Point3D>(kArrayPosition);

		sphere->Calculate(vertexCount, vertex);
		boundingBox.Calculate(vertexCount, vertex);
	}
	else
	{
		sphere->SetCenter(Zero3D);
		sphere->SetRadius(0.0F);

		boundingBox.Set(Zero3D, Zero3D);
	}
}

void GenericGeometryObject::Rebuild(const Geometry *geometry)
{
	bool morph = false;
	bool skin = false;

	const Controller *controller = geometry->GetController();
	if (controller)
	{
		ControllerType type = controller->GetControllerType();
		if (type == kControllerMorph)
		{
			morph = true;
		}
		else if (type == kControllerSkin)
		{
			morph = true;
			skin = true;
		}
	}

	int32 levelCount = GetGeometryLevelCount();
	for (machine level = 0; level < levelCount; level++)
	{
		Mesh	tempMesh;

		Mesh *geometryMesh = GetGeometryLevel(level);
		if (((!morph) && (geometryMesh->GetMorphTargetCount() != 0)) || ((!skin) && (geometryMesh->GetSkinWeightSize() != 0)))
		{
			tempMesh.CopyRigidMesh(geometryMesh);
			geometryMesh->CopyRigidMesh(&tempMesh);
		}

		geometryMesh->CalculateTangentArray();
	}
}

// ZYUQURM
