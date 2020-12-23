 

#include "C4Mesh.h"
#include "C4Topology.h"
#include "C4GeometryObjects.h"


using namespace C4;


namespace C4
{
	class IndexTable
	{
		private:

			enum
			{
				kIndexBucketCount = 512
			};

			Array<unsigned_int32, 16>	indexBucket[kIndexBucketCount];

		public:

			IndexTable();
			~IndexTable();

			const ImmutableArray<unsigned_int32>& GetIndexBucket(unsigned_int32 hash) const
			{
				return (indexBucket[hash & (kIndexBucketCount - 1)]);
			}

			void AddIndex(unsigned_int32 hash, unsigned_int32 index)
			{
				indexBucket[hash & (kIndexBucketCount - 1)].AddElement(index);
			}

			void Clear(void);
	};


	class IndexGrid
	{
		private:

			enum
			{
				kIndexGridCount = 8
			};

			Point3D		pmin;
			Vector3D	scale;

			Array<unsigned_int32, 16>	indexGrid[kIndexGridCount][kIndexGridCount][kIndexGridCount];

		public:

			IndexGrid(int32 vertexCount, const Point3D *position);
			~IndexGrid();

			const ImmutableArray<unsigned_int32>& GetIndexGrid(int32 i, int32 j, int32 k) const
			{
				return (indexGrid[k][j][i]);
			}

			void AddIndex(const Point3D& position, unsigned_int32 index);
			void CalculateGridRange(const Point3D& position, float radius, Range<int32> (& range)[3]) const;
	};


	template <> Heap EngineMemory<GeometryVertex>::heap("GeometryVertex");
	template class EngineMemory<GeometryVertex>;
}


IndexTable::IndexTable()
{
}

IndexTable::~IndexTable()
{
}

void IndexTable::Clear(void)
{
	for (machine a = 0; a < kIndexBucketCount; a++)
	{
		indexBucket[a].Clear();
	}
}


IndexGrid::IndexGrid(int32 vertexCount, const Point3D *position)
{
	Box3D	bounds;

	bounds.Calculate(vertexCount, position);
	pmin = bounds.min;

	Vector3D size = bounds.max - bounds.min;
	scale.x = (size.x > K::min_float) ? float(kIndexGridCount) / size.x : 0.0F;
	scale.y = (size.y > K::min_float) ? float(kIndexGridCount) / size.y : 0.0F; 
	scale.z = (size.z > K::min_float) ? float(kIndexGridCount) / size.z : 0.0F;
}
 
IndexGrid::~IndexGrid()
{ 
}

void IndexGrid::AddIndex(const Point3D& position, unsigned_int32 index) 
{
	Vector3D v = (position - pmin) & scale; 
 
	int32 i = MaxZero(Min(int32(v.x), kIndexGridCount - 1));
	int32 j = MaxZero(Min(int32(v.y), kIndexGridCount - 1));
	int32 k = MaxZero(Min(int32(v.z), kIndexGridCount - 1));
 
	indexGrid[k][j][i].AddElement(index);
}

void IndexGrid::CalculateGridRange(const Point3D& position, float radius, Range<int32> (& range)[3]) const
{
	Vector3D v = position - pmin;

	range[0].min = MaxZero(Min(int32((v.x - radius) * scale.x), kIndexGridCount - 1));
	range[0].max = MaxZero(Min(int32((v.x + radius) * scale.x), kIndexGridCount - 1));
	range[1].min = MaxZero(Min(int32((v.y - radius) * scale.y), kIndexGridCount - 1));
	range[1].max = MaxZero(Min(int32((v.y + radius) * scale.y), kIndexGridCount - 1));
	range[2].min = MaxZero(Min(int32((v.z - radius) * scale.z), kIndexGridCount - 1));
	range[2].max = MaxZero(Min(int32((v.z + radius) * scale.z), kIndexGridCount - 1));
}


GeometryVertex::GeometryVertex()
{
	normal.Set(0.0F, 0.0F, 1.0F);
	tangent.Set(1.0F, 0.0F, 0.0F, 1.0F);
	color.Set(1.0F, 1.0F, 1.0F, 1.0F);

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		texcoord[a].Set(0.0F, 0.0F);
	}
}

GeometryVertex::~GeometryVertex()
{
}


GeometryPolygon::GeometryPolygon()
{
}

GeometryPolygon::~GeometryPolygon()
{
}


GeometrySurface::GeometrySurface(unsigned_int32 flags, unsigned_int32 material, int32 texcoord)
{
	surfaceFlags = flags;
	materialIndex = material;
	texcoordCount = texcoord;
}

GeometrySurface::~GeometrySurface()
{
}


Mesh::Mesh()
{
	meshStorage = nullptr;
}

Mesh::~Mesh()
{
	delete[] meshStorage;
}

inline bool Mesh::TangentsSimilar(const Vector4D& t1, const Vector4D& t2)
{
	return ((t1.w == t2.w) && (t1.GetVector3D() * t2.GetVector3D() > 0.0F));
}

GeometryArrayType Mesh::GetArrayType(const ArrayDescriptor *desc, int32 *count)
{
	int32 elementCount = desc->elementCount;

	switch (desc->identifier)
	{
		case kArrayColor0:
		case kArrayColor1:
		case kArrayColor2:

			if (desc->componentCount == 1)
			{
				*count = elementCount * 4;
				return (kGeometryUnsignedInt8);
			}

			break;

		case kArraySurfaceIndex:

		#if C4LEGACY

			case 25:

		#endif

			*count = elementCount;
			return (kGeometryUnsignedInt16);

		case kArrayBoneHash:

			*count = elementCount;
			return ((desc->elementSize == 4) ? kGeometryUnsignedInt32 : kGeometryUnsignedInt16);

		case kArrayPrimitive:

			*count = elementCount;
			return ((desc->elementSize == sizeof(Triangle)) ? kGeometryTriangle : kGeometryQuad);

		case kArraySegment:

			*count = elementCount;
			return (kGeometrySegment);

		#if C4LEGACY

			case 23:

				*count = elementCount;
				return (kGeometryEdge);

		#endif
	}

	*count = elementCount * desc->componentCount;
	return (kGeometryFloat);
}

void Mesh::Pack(Packer& data, unsigned_int32 packFlags) const
{
	int32 meshArrayCount = 0;
	unsigned_int32 size = 12;
	for (machine index = 0; index < kMaxGeometryArrayCount; index++)
	{
		const ArrayBundle *bundle = &arrayBundle[index];
		if ((bundle->pointer) && (bundle->descriptor.elementCount != 0) && (index <= kArraySegment))
		{
			meshArrayCount++;
			size += sizeof(ArrayDescriptor);
			size += (bundle->descriptor.elementCount * bundle->descriptor.elementSize + 3) & ~3;
		}
	}

	data << ChunkHeader('DATA', size);
	data << meshVertexCount;
	data << meshArrayCount;
	data << skinWeightSize;
	data << morphTargetCount;

	for (machine index = 0; index < kMaxGeometryArrayCount; index++)
	{
		const ArrayBundle *bundle = &arrayBundle[index];
		if ((bundle->pointer) && (bundle->descriptor.elementCount != 0) && (index <= kArraySegment))
		{
			data << bundle->descriptor;
		}
	}

	for (machine index = 0; index < kMaxGeometryArrayCount; index++)
	{
		const ArrayBundle *bundle = &arrayBundle[index];
		const void *array = bundle->pointer;
		if ((array) && (bundle->descriptor.elementCount != 0) && (index <= kArraySegment))
		{
			static const unsigned_int16 zero = 0;

			int32	count;

			GeometryArrayType type = GetArrayType(&bundle->descriptor, &count);
			switch (type)
			{
				case kGeometryFloat:

					data.WriteArray(count, static_cast<const float *>(array));
					break;

				case kGeometryUnsignedInt8:

					data.WriteArray((count + 3) & ~3, static_cast<const unsigned_int8 *>(array));
					break;

				case kGeometryUnsignedInt16:

					data.WriteArray((count + 1) & ~1, static_cast<const unsigned_int16 *>(array));
					break;

				case kGeometryUnsignedInt32:

					data.WriteArray(count, static_cast<const unsigned_int32 *>(array));
					break;

				case kGeometryTriangle:

					data.WriteArray(count, static_cast<const Triangle *>(array));
					if (count & 1)
					{
						data.WriteData(&zero, 2);
					}

					break;

				case kGeometryQuad:

					data.WriteArray(count, static_cast<const Quad *>(array));
					break;

				case kGeometrySegment:

					data.WriteArray(count, static_cast<const SegmentData *>(array));
					break;

				case kGeometryEdge:

					data.WriteArray(count * 4, static_cast<const unsigned_int16 *>(array));
					break;
			}
		}
	}

	if (skinWeightData)
	{
		data << ChunkHeader('SKIN', skinWeightSize);
		data.WriteData(skinWeightData, skinWeightSize);
	}

	if (morphTargetCount != 0)
	{
		int32 count = morphTargetCount * meshVertexCount;
		data << ChunkHeader('MRPH', count * sizeof(MorphAttrib));
		data.WriteArray(count, morphTargetData);
	}

	data << TerminatorChunk;
}

void Mesh::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Mesh>(data, unpackFlags);
}

bool Mesh::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

		#if C4LEGACY

			case 'ARAY':

		#endif

		{
			int32				vertexCount;
			int32				arrayCount;
			unsigned_int32		skinSize;
			int32				morphCount;
			ArrayDescriptor		arrayDesc[kMaxGeometryArrayCount];

			data >> vertexCount;
			data >> arrayCount;
			data >> skinSize;

			#if C4LEGACY

				if (chunkHeader->chunkType == 'DATA')
				{

			#endif

					data >> morphCount;

			#if C4LEGACY

				}
				else
				{
					morphCount = 0;
				}

			#endif

			data.ReadArray(arrayCount, arrayDesc);

			#if C4LEGACY

				if (data.GetVersion() <= 70)
				{
					static const int8 remap[10] =
					{
						-1, kArrayBoneHash, kArraySurfaceIndex, -1, kArrayPrimitive, kArrayInverseBindTransform, kArraySegment, -1, -1, -1
					};

					for (machine a = 0; a < arrayCount; a++)
					{
						int32 identifier = arrayDesc[a].identifier;
						if (identifier >= 16)
						{
							arrayDesc[a].identifier = remap[identifier - 16];
						}
					}
				}

			#endif

			AllocateStorage(vertexCount, arrayCount, arrayDesc, skinSize, morphCount);

			for (machine a = 0; a < arrayCount; a++)
			{
				int32	count;

				const ArrayDescriptor *desc = &arrayDesc[a];
				int32 index = desc->identifier;

				#if C4LEGACY

					if (index >= 0)
					{

				#endif

						void *array = arrayBundle[index].pointer;

						GeometryArrayType type = GetArrayType(desc, &count);
						switch (type)
						{
							case kGeometryFloat:

								data.ReadArray(count, static_cast<float *>(array));
								break;

							case kGeometryUnsignedInt8:

								data.ReadArray((count + 3) & ~3, static_cast<unsigned_int8 *>(array));
								break;

							case kGeometryUnsignedInt16:

								data.ReadArray((count + 1) & ~1, static_cast<unsigned_int16 *>(array));
								break;

							case kGeometryUnsignedInt32:

								data.ReadArray(count, static_cast<unsigned_int32 *>(array));
								break;

							case kGeometryTriangle:

								data.ReadArray(count, static_cast<Triangle *>(array));
								if (count & 1)
								{
									data += 2;
								}

								break;

							case kGeometryQuad:

								data.ReadArray(count, static_cast<Quad *>(array));
								break;

							case kGeometrySegment:

								data.ReadArray(count, static_cast<SegmentData *>(array));
								break;

							case kGeometryEdge:

								data.ReadArray(count * 4, static_cast<unsigned_int16 *>(array));
								break;
						}

				#if C4LEGACY

					}
					else
					{
						data.Skip(data.GetPointer(), (desc->elementCount * desc->elementSize + 3) & ~3);
					}

				#endif
			}

			return (true);
		}

		case 'SKIN':
		{
			SkinWeight *skinWeight = skinWeightData;

			int32 vertexCount = meshVertexCount;
			for (machine a = 0; a < vertexCount; a++)
			{
				data >> skinWeight->boneCount;
				int32 boneCount = skinWeight->boneCount;

				BoneWeight *boneWeight = skinWeight->boneWeight;
				for (machine b = 0; b < boneCount; b++, boneWeight++)
				{
					data >> *boneWeight;
				}

				skinWeight = reinterpret_cast<SkinWeight *>(boneWeight);
			}

			return (true);
		}

		case 'MRPH':

			data.ReadArray(morphTargetCount * meshVertexCount, morphTargetData);
			return (true);
	}

	return (false);
}

void *Mesh::BeginSettingsUnpack(void)
{
	delete[] meshStorage;
	meshStorage = nullptr;

	return (nullptr);
}

void Mesh::AllocateStorage(int32 vertexCount, int32 arrayCount, const ArrayDescriptor *arrayDesc, unsigned_int32 skinSize, int32 morphCount)
{
	delete[] meshStorage;
	meshStorage = nullptr;

	meshVertexCount = vertexCount;
	primitiveOffset = 0;

	skinWeightSize = skinSize;
	skinWeightData = nullptr;

	morphTargetCount = morphCount;
	morphTargetData = nullptr;

	unsigned_int32 size = 0;
	for (machine a = 0; a < arrayCount; a++)
	{
		const ArrayDescriptor *desc = &arrayDesc[a];

		#if C4LEGACY

			if (desc->identifier >= 0)
			{

		#endif

				size += (desc->elementCount * desc->elementSize + 15) & ~15;

		#if C4LEGACY

			}

		#endif
	}

	for (machine index = 0; index < kMaxGeometryArrayCount; index++)
	{
		arrayBundle[index].descriptor.elementCount = 0;
		arrayBundle[index].pointer = nullptr;
	}

	int32 attributeCount = 0;

	size += skinSize + morphCount * vertexCount * sizeof(MorphAttrib);
	if (size != 0)
	{
		char *pointer = new char[size];
		meshStorage = pointer;

		for (machine a = 0; a < arrayCount; a++)
		{
			const ArrayDescriptor *desc = &arrayDesc[a];

			int32 count = desc->elementCount;
			if (count != 0)
			{
				int32 index = desc->identifier;

				#if C4LEGACY

					if (index >= 0)
					{

				#endif

						arrayBundle[index].descriptor = *desc;
						arrayBundle[index].pointer = pointer;

						unsigned_int32 logicalSize = count * desc->elementSize;
						unsigned_int32 physicalSize = (logicalSize + 15) & ~15;
						for (unsigned_machine b = logicalSize; b < physicalSize; b++)
						{
							pointer[b] = 0;
						}

						pointer += physicalSize;

						if (index < kMaxAttributeArrayCount)
						{
							attributeArrayIndex[attributeCount] = (unsigned_int8) index;
							attributeOffset[attributeCount] = 0;
							attributeCount++;
						}

				#if C4LEGACY

					}

				#endif
			}
		}

		if (skinSize != 0)
		{
			skinWeightData = reinterpret_cast<SkinWeight *>(pointer);
			pointer += skinSize;
		}

		if (morphCount != 0)
		{
			morphTargetData = reinterpret_cast<MorphAttrib *>(pointer);
		}
	}

	attributeArrayCount = attributeCount;
}

void Mesh::AllocateStorage(const Mesh *inputMesh, int32 arrayCount, const ArrayDescriptor *arrayDesc, unsigned_int32 skinSize, int32 morphCount)
{
	bool	copyFlag[kMaxGeometryArrayCount];

	delete[] meshStorage;
	meshStorage = nullptr;

	meshVertexCount = inputMesh->GetVertexCount();
	primitiveOffset = 0;

	skinWeightSize = 0;
	skinWeightData = nullptr;

	morphTargetCount = 0;
	morphTargetData = nullptr;

	unsigned_int32 size = 0;
	for (machine a = 0; a < arrayCount; a++)
	{
		const ArrayDescriptor *desc = &arrayDesc[a];
		size += (desc->elementCount * desc->elementSize + 15) & ~15;
	}

	for (machine index = 0; index < kMaxGeometryArrayCount; index++)
	{
		copyFlag[index] = false;

		const ArrayBundle *bundle = inputMesh->GetArrayBundle(index);
		if ((bundle->pointer) && (bundle->descriptor.elementCount != 0))
		{
			for (machine a = 0; a < arrayCount; a++)
			{
				if (arrayDesc[a].identifier == index)
				{
					goto next;
				}
			}

			copyFlag[index] = true;
			size += (bundle->GetArraySize() + 15) & ~15;
		}

		next:;
	}

	for (machine index = 0; index < kMaxGeometryArrayCount; index++)
	{
		arrayBundle[index].descriptor.elementCount = 0;
		arrayBundle[index].pointer = nullptr;
	}

	int32 attributeCount = 0;

	if (skinSize == 0)
	{
		size += inputMesh->GetSkinWeightSize();
	}
	else
	{
		size += skinSize;
	}

	if (morphCount == 0)
	{
		size += inputMesh->GetMorphTargetCount() * meshVertexCount * sizeof(MorphAttrib);
	}
	else
	{
		size += morphCount * meshVertexCount * sizeof(MorphAttrib);
	}

	if (size != 0)
	{
		char *pointer = new char[size];
		meshStorage = pointer;

		for (machine a = 0; a < arrayCount; a++)
		{
			const ArrayDescriptor *desc = &arrayDesc[a];
			int32 count = desc->elementCount;
			if (count != 0)
			{
				int32 index = desc->identifier;

				arrayBundle[index].descriptor = *desc;
				arrayBundle[index].pointer = pointer;

				unsigned_int32 logicalSize = count * desc->elementSize;
				unsigned_int32 physicalSize = (logicalSize + 15) & ~15;
				for (unsigned_machine b = logicalSize; b < physicalSize; b++)
				{
					pointer[b] = 0;
				}

				pointer += physicalSize;

				if (index < kMaxAttributeArrayCount)
				{
					attributeArrayIndex[attributeCount] = (unsigned_int8) index;
					attributeOffset[attributeCount] = 0;
					attributeCount++;
				}
			}
		}

		for (machine index = 0; index < kMaxGeometryArrayCount; index++)
		{
			const ArrayBundle *bundle = inputMesh->GetArrayBundle(index);
			if (copyFlag[index])
			{
				arrayBundle[index].descriptor = bundle->descriptor;
				arrayBundle[index].pointer = pointer;

				unsigned_int32 logicalSize = bundle->GetArraySize();
				unsigned_int32 physicalSize = (logicalSize + 15) & ~15;

				MemoryMgr::CopyMemory(bundle->pointer, pointer, logicalSize);
				for (unsigned_machine a = logicalSize; a < physicalSize; a++)
				{
					pointer[a] = 0;
				}

				pointer += physicalSize;

				if (index < kMaxAttributeArrayCount)
				{
					attributeArrayIndex[attributeCount] = (unsigned_int8) index;
					attributeOffset[attributeCount] = 0;
					attributeCount++;
				}
			}
		}

		if (skinSize != 0)
		{
			skinWeightSize = skinSize;
			skinWeightData = reinterpret_cast<SkinWeight *>(pointer);
			pointer += skinSize;
		}
		else
		{
			skinSize = inputMesh->GetSkinWeightSize();
			if (skinSize != 0)
			{
				skinWeightSize = skinSize;
				skinWeightData = reinterpret_cast<SkinWeight *>(pointer);
				MemoryMgr::CopyMemory(inputMesh->GetSkinWeightData(), skinWeightData, skinSize);
				pointer += skinSize;
			}
		}

		if (morphCount != 0)
		{
			morphTargetCount = morphCount;
			morphTargetData = reinterpret_cast<MorphAttrib *>(pointer);
		}
		else
		{
			morphCount = inputMesh->GetMorphTargetCount();
			if (morphCount != 0)
			{
				morphTargetCount = morphCount;
				morphTargetData = reinterpret_cast<MorphAttrib *>(pointer);
				MemoryMgr::CopyMemory(inputMesh->GetMorphTargetData(), morphTargetData, morphCount * meshVertexCount * sizeof(MorphAttrib));
			}
		}
	}

	attributeArrayCount = attributeCount;
}

void Mesh::CopyMesh(const Mesh *inputMesh, unsigned_int32 exclusionMask)
{
	ArrayDescriptor		desc[kMaxGeometryArrayCount];

	int32 arrayCount = 0;
	for (machine a = 0; a < kMaxGeometryArrayCount; a++)
	{
		if ((exclusionMask & 1) == 0)
		{
			const ArrayBundle *bundle = inputMesh->GetArrayBundle(a);
			if (bundle->pointer)
			{
				desc[arrayCount] = bundle->descriptor;
				arrayCount++;
			}
		}

		exclusionMask >>= 1;
	}

	int32 vertexCount = inputMesh->GetVertexCount();
	unsigned_int32 skinSize = inputMesh->GetSkinWeightSize();
	int32 morphCount = inputMesh->GetMorphTargetCount();
	AllocateStorage(vertexCount, arrayCount, desc, skinSize, morphCount);

	for (machine a = 0; a < arrayCount; a++)
	{
		int32 index = desc[a].identifier;
		const ArrayBundle *bundle = inputMesh->GetArrayBundle(index);
		MemoryMgr::CopyMemory(bundle->pointer, arrayBundle[index].pointer, bundle->GetArraySize());
	}

	if (skinSize != 0)
	{
		MemoryMgr::CopyMemory(inputMesh->GetSkinWeightData(), skinWeightData, skinSize);
	}

	if (morphCount != 0)
	{
		MemoryMgr::CopyMemory(inputMesh->GetMorphTargetData(), morphTargetData, morphCount * vertexCount * sizeof(MorphAttrib));
	}
}

void Mesh::CopyRigidMesh(const Mesh *inputMesh, unsigned_int32 exclusionMask)
{
	ArrayDescriptor		desc[kMaxGeometryArrayCount];

	exclusionMask |= (1 << kArrayMorphHash) | (1 << kArrayBoneHash) | (1 << kArrayInverseBindTransform);

	int32 arrayCount = 0;
	for (machine a = 0; a < kMaxGeometryArrayCount; a++)
	{
		if ((exclusionMask & 1) == 0)
		{
			const ArrayBundle *bundle = inputMesh->GetArrayBundle(a);
			if (bundle->pointer)
			{
				desc[arrayCount] = bundle->descriptor;
				arrayCount++;
			}
		}

		exclusionMask >>= 1;
	}

	int32 vertexCount = inputMesh->GetVertexCount();
	AllocateStorage(vertexCount, arrayCount, desc);

	for (machine a = 0; a < arrayCount; a++)
	{
		int32 index = desc[a].identifier;
		const ArrayBundle *bundle = inputMesh->GetArrayBundle(index);
		MemoryMgr::CopyMemory(bundle->pointer, arrayBundle[index].pointer, bundle->GetArraySize());
	}
}

void Mesh::TransformMesh(const Transform4D& transform)
{
	Transform4D inverse = Inverse(transform);
	int32 vertexCount = GetVertexCount();

	Point3D *position = GetArray<Point3D>(kArrayPosition);
	for (machine a = 0; a < vertexCount; a++)
	{
		*position = transform * *position;
		position++;
	}

	Vector3D *normal = GetArray<Vector3D>(kArrayNormal);
	if (normal)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			*normal = Normalize(*normal * inverse);
			normal++;
		}
	}

	void *tangent = GetArray(kArrayTangent);
	if (tangent)
	{
		if (GetArrayDescriptor(kArrayTangent)->componentCount == 4)
		{
			Vector4D *tangent4D = static_cast<Vector4D *>(tangent);
			for (machine a = 0; a < vertexCount; a++)
			{
				tangent4D->GetVector3D() = transform * tangent4D->GetVector3D();
				tangent4D++;
			}
		}
		else
		{
			Vector3D *tangent3D = static_cast<Vector3D *>(tangent);
			for (machine a = 0; a < vertexCount; a++)
			{
				*tangent3D = transform * *tangent3D;
				tangent3D++;
			}
		}
	}

	int32 morphCount = morphTargetCount;
	MorphAttrib *morph = morphTargetData;

	for (machine m = 0; m < morphCount; m++)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			morph->position = transform * morph->position;
			morph->normal = morph->normal * inverse;
			morph->tangent = transform * morph->tangent;
			morph++;
		}
	}
}

void Mesh::TranslateMesh(const Vector3D& translation)
{
	int32 vertexCount = GetVertexCount();
	Point3D *position = GetArray<Point3D>(kArrayPosition);
	for (machine a = 0; a < vertexCount; a++)
	{
		*position += translation;
		position++;
	}

	int32 morphCount = morphTargetCount;
	MorphAttrib *morph = morphTargetData;

	for (machine m = 0; m < morphCount; m++)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			morph->position += translation;
			morph++;
		}
	}
}

void Mesh::ScaleMesh(const Vector3D& scale)
{
	Vector3D inverse(1.0F / scale.x, 1.0F / scale.y, 1.0F / scale.z);

	int32 vertexCount = GetVertexCount();
	Point3D *position = GetArray<Point3D>(kArrayPosition);

	Vector3D *normal = GetArray<Vector3D>(kArrayNormal);
	if (normal)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			*position &= scale;
			*normal = Normalize(*normal & inverse);
			position++;
			normal++;
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			*position &= scale;
			position++;
		}
	}

	int32 morphCount = morphTargetCount;
	MorphAttrib *morph = morphTargetData;

	for (machine m = 0; m < morphCount; m++)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			morph->position &= scale;
			morph->normal = Normalize(morph->normal & inverse);
			morph++;
		}
	}
}

void Mesh::InvertMesh(void)
{
	int32 vertexCount = GetVertexCount();
	Vector3D *normal = GetArray<Vector3D>(kArrayNormal);
	if (normal)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			*normal = -*normal;
			normal++;
		}
	}

	Vector4D *tangent = GetArray<Vector4D>(kArrayTangent);
	if ((tangent) && (GetArrayDescriptor(kArrayTangent)->componentCount == 4))
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			tangent->w = -tangent->w;
			tangent++;
		}
	}

	int32 morphCount = morphTargetCount;
	MorphAttrib *morph = morphTargetData;

	for (machine m = 0; m < morphCount; m++)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			morph->normal = -morph->normal;
			morph++;
		}
	}

	int32 primitiveCount = GetPrimitiveCount();
	Triangle *triangle = GetArray<Triangle>(kArrayPrimitive);
	for (machine a = 0; a < primitiveCount; a++)
	{
		int32 t = triangle->index[1];
		triangle->index[1] = triangle->index[2];
		triangle->index[2] = (unsigned_int16) t;
		triangle++;
	}
}

void Mesh::WeldMesh(float epsilon)
{
	const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (surfaceIndex)
	{
		float e2 = epsilon * epsilon;

		int32 vertexCount = GetVertexCount();
		Point3D *position = GetArray<Point3D>(kArrayPosition);
		IndexGrid *indexGrid = new IndexGrid(vertexCount, position);

		for (machine a = 0; a < vertexCount; a++)
		{
			Range<int32>	range[3];

			const Point3D& posi = position[a];
			unsigned_int32 si = surfaceIndex[a];

			indexGrid->CalculateGridRange(posi, epsilon, range);

			for (machine k = range[2].min; k < range[2].max; k++)
			{
				for (machine j = range[1].min; j < range[1].max; j++)
				{
					for (machine i = range[0].min; i < range[0].max; i++)
					{
						for (unsigned_int32 index : indexGrid->GetIndexGrid(i, j, k))
						{
							if ((surfaceIndex[index] != si) && (SquaredMag(position[index] - posi) < e2))
							{
								position[a] = position[index];
								goto next;
							}
						}
					}
				}
			}

			indexGrid->AddIndex(posi, a);
			next:;
		}

		delete indexGrid;
	}
}

void Mesh::MendMesh(float vertexEpsilon, float normalEpsilon, float texcoordEpsilon)
{
	float e2 = vertexEpsilon * vertexEpsilon;
	normalEpsilon = 1.0F - normalEpsilon;

	int32 vertexCount = GetVertexCount();
	Point3D *position = GetArray<Point3D>(kArrayPosition);
	IndexGrid *indexGrid = new IndexGrid(vertexCount, position);

	Vector3D *normal = GetArray<Vector3D>(kArrayNormal);
	Vector4D *tangent = GetArray<Vector4D>(kArrayTangent);
	Point2D *texcoord = GetArray<Point2D>(kArrayTexcoord);
	const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);

	if (normal)
	{
		if (tangent)
		{
			for (machine a = 0; a < vertexCount; a++)
			{
				Range<int32>	range[3];

				const Point3D& posi = position[a];
				const Vector3D& nrml = normal[a];
				const Vector4D& tang = tangent[a];
				const Point2D& texc = texcoord[a];

				indexGrid->CalculateGridRange(posi, vertexEpsilon, range);

				for (machine k = range[2].min; k < range[2].max; k++)
				{
					for (machine j = range[1].min; j < range[1].max; j++)
					{
						for (machine i = range[0].min; i < range[0].max; i++)
						{
							for (unsigned_int32 index : indexGrid->GetIndexGrid(i, j, k))
							{
								if ((!surfaceIndex) || (surfaceIndex[index] == surfaceIndex[a]))
								{
									if ((SquaredMag(position[index] - posi) < e2) && (normal[index] * nrml > normalEpsilon) && (TangentsSimilar(tangent[index], tang)))
									{
										if ((Fabs(texcoord[index].x - texc.x) < texcoordEpsilon) && (Fabs(texcoord[index].y - texc.y) < texcoordEpsilon))
										{
											position[a] = position[index];
											normal[a] = normal[index];
											texcoord[a] = texcoord[index];
											goto next1;
										}
									}
								}
							}
						}
					}
				}

				indexGrid->AddIndex(posi, a);
				next1:;
			}
		}
		else
		{
			for (machine a = 0; a < vertexCount; a++)
			{
				Range<int32>	range[3];

				const Point3D& posi = position[a];
				const Vector3D& nrml = normal[a];
				const Point2D& texc = texcoord[a];

				indexGrid->CalculateGridRange(posi, vertexEpsilon, range);

				for (machine k = range[2].min; k < range[2].max; k++)
				{
					for (machine j = range[1].min; j < range[1].max; j++)
					{
						for (machine i = range[0].min; i < range[0].max; i++)
						{
							for (unsigned_int32 index : indexGrid->GetIndexGrid(i, j, k))
							{
								if ((!surfaceIndex) || (surfaceIndex[index] == surfaceIndex[a]))
								{
									if ((SquaredMag(position[index] - posi) < e2) && (normal[index] * nrml > normalEpsilon))
									{
										if ((Fabs(texcoord[index].x - texc.x) < texcoordEpsilon) && (Fabs(texcoord[index].y - texc.y) < texcoordEpsilon))
										{
											position[a] = position[index];
											normal[a] = normal[index];
											texcoord[a] = texcoord[index];
											goto next2;
										}
									}
								}
							}
						}
					}
				}

				indexGrid->AddIndex(posi, a);
				next2:;
			}
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			Range<int32>	range[3];

			const Point3D& posi = position[a];
			const Point2D& texc = texcoord[a];

			indexGrid->CalculateGridRange(posi, vertexEpsilon, range);

			for (machine k = range[2].min; k < range[2].max; k++)
			{
				for (machine j = range[1].min; j < range[1].max; j++)
				{
					for (machine i = range[0].min; i < range[0].max; i++)
					{
						for (unsigned_int32 index : indexGrid->GetIndexGrid(i, j, k))
						{
							if ((!surfaceIndex) || (surfaceIndex[index] == surfaceIndex[a]))
							{
								if (SquaredMag(position[index] - posi) < e2)
								{
									if ((Fabs(texcoord[index].x - texc.x) < texcoordEpsilon) && (Fabs(texcoord[index].y - texc.y) < texcoordEpsilon))
									{
										position[a] = position[index];
										texcoord[a] = texcoord[index];
										goto next4;
									}
								}
							}
						}
					}
				}
			}

			indexGrid->AddIndex(posi, a);
			next4:;
		}
	}

	delete indexGrid;
}

void Mesh::UnifyMesh(const Mesh *inputMesh)
{
	ArrayDescriptor		desc[kMaxGeometryTexcoordCount + 8];
	const Point2D		*inputTexcoord[kMaxGeometryTexcoordCount];
	Point2D				*outputTexcoord[kMaxGeometryTexcoordCount];

	IndexTable *indexTable = new IndexTable;

	int32 vertexCount = inputMesh->GetVertexCount();
	const Point3D *inputPosition = inputMesh->GetArray<Point3D>(kArrayPosition);

	Buffer buffer(vertexCount * 4);
	unsigned_int32 *remapTable = buffer.GetPtr<unsigned_int32>();

	const Color4C *inputColor = inputMesh->GetArray<Color4C>(kArrayColor);
	const Vector3D *inputNormal = inputMesh->GetArray<Vector3D>(kArrayNormal);

	const Vector4D *inputTangent = inputMesh->GetArray<Vector4D>(kArrayTangent);
	if ((inputTangent) && (inputMesh->GetArrayDescriptor(kArrayTangent)->componentCount != 4))
	{
		inputTangent = nullptr;
	}

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		inputTexcoord[a] = inputMesh->GetArray<Point2D>(kArrayTexcoord0 + a);
	}

	int32 unifiedCount = 0;
	unsigned_int32 skinSize = 0;
	const SkinWeight **skinWeightTable = nullptr;

	const SkinWeight *skinWeight = inputMesh->GetSkinWeightData();
	if (skinWeight)
	{
		skinWeightTable = new const SkinWeight *[vertexCount];

		for (machine a = 0; a < vertexCount; a++)
		{
			skinWeightTable[a] = skinWeight;
			skinWeight = reinterpret_cast<const SkinWeight *>(skinWeight->boneWeight + skinWeight->boneCount);
		}
	}

	const unsigned_int16 *inputSurfaceIndex = inputMesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (inputSurfaceIndex)
	{
		unsigned_int32 currentSurface = inputSurfaceIndex[0];

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 surface = inputSurfaceIndex[a];
			if (surface != currentSurface)
			{
				currentSurface = surface;
				indexTable->Clear();
			}

			const Point3D& position = inputPosition[a];
			unsigned_int32 hash = Hash(position);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if (inputPosition[index] != position)
				{
					continue;
				}

				if (inputTexcoord[0][index] != inputTexcoord[0][a])
				{
					continue;
				}

				if ((inputTexcoord[1]) && (inputTexcoord[1][index] != inputTexcoord[1][a]))
				{
					continue;
				}

				if ((inputNormal) && (inputNormal[index] != inputNormal[a]))
				{
					continue;
				}

				if ((inputColor) && (inputColor[index] != inputColor[a]))
				{
					continue;
				}

				if ((inputTangent) && (!TangentsSimilar(inputTangent[index], inputTangent[a])))
				{
					continue;
				}

				remapTable[a] = remapTable[index];
				goto next1;
			}

			remapTable[a] = unifiedCount++;
			indexTable->AddIndex(hash, a);

			if (skinWeightTable)
			{
				skinSize += skinWeightTable[a]->GetSize();
			}

			next1:;
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& position = inputPosition[a];
			unsigned_int32 hash = Hash(position);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if (inputPosition[index] != position)
				{
					continue;
				}

				if (inputTexcoord[0][index] != inputTexcoord[0][a])
				{
					continue;
				}

				if ((inputTexcoord[1]) && (inputTexcoord[1][index] != inputTexcoord[1][a]))
				{
					continue;
				}

				if ((inputNormal) && (inputNormal[index] != inputNormal[a]))
				{
					continue;
				}

				if ((inputColor) && (inputColor[index] != inputColor[a]))
				{
					continue;
				}

				if ((inputTangent) && (!TangentsSimilar(inputTangent[index], inputTangent[a])))
				{
					continue;
				}

				remapTable[a] = remapTable[index];
				goto next2;
			}

			remapTable[a] = unifiedCount++;
			indexTable->AddIndex(hash, a);

			if (skinWeightTable)
			{
				skinSize += skinWeightTable[a]->GetSize();
			}

			next2:;
		}
	}

	delete indexTable;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = unifiedCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	int32 arrayCount = 1;
	if (inputNormal)
	{
		desc[1].identifier = kArrayNormal;
		desc[1].elementCount = unifiedCount;
		desc[1].elementSize = sizeof(Vector3D);
		desc[1].componentCount = 3;

		arrayCount = 2;
	}

	if (inputColor)
	{
		desc[arrayCount].identifier = kArrayColor;
		desc[arrayCount].elementCount = unifiedCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	desc[arrayCount].identifier = kArrayTexcoord0;
	desc[arrayCount].elementCount = unifiedCount;
	desc[arrayCount].elementSize = sizeof(Point2D);
	desc[arrayCount].componentCount = 2;
	arrayCount++;

	if (inputTexcoord[1])
	{
		desc[arrayCount].identifier = kArrayTexcoord1;
		desc[arrayCount].elementCount = unifiedCount;
		desc[arrayCount].elementSize = sizeof(Point2D);
		desc[arrayCount].componentCount = 2;
		arrayCount++;
	}

	if (inputSurfaceIndex)
	{
		desc[arrayCount].identifier = kArraySurfaceIndex;
		desc[arrayCount].elementCount = unifiedCount;
		desc[arrayCount].elementSize = 2;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	const Triangle *inputTriangle = inputMesh->GetArray<Triangle>(kArrayPrimitive);
	if (inputTriangle)
	{
		int32 inputPrimitiveCount = inputMesh->GetPrimitiveCount();
		int32 outputPrimitiveCount = 0;

		for (machine a = 0; a < inputPrimitiveCount; a++)
		{
			unsigned_int32 i1 = remapTable[inputTriangle[a].index[0]];
			unsigned_int32 i2 = remapTable[inputTriangle[a].index[1]];
			unsigned_int32 i3 = remapTable[inputTriangle[a].index[2]];

			if ((i1 != i2) && (i1 != i3) && (i2 != i3))
			{
				outputPrimitiveCount++;
			}
		}

		desc[arrayCount].elementCount = outputPrimitiveCount;
	}
	else
	{
		desc[arrayCount].elementCount = vertexCount / 3;
	}

	desc[arrayCount].identifier = kArrayPrimitive;
	desc[arrayCount].elementSize = sizeof(Triangle);
	desc[arrayCount].componentCount = 1;
	arrayCount++;

	const ArrayBundle *boneHashBundle = inputMesh->GetArrayBundle(kArrayBoneHash);
	if (boneHashBundle->pointer)
	{
		desc[arrayCount] = boneHashBundle->descriptor;
		arrayCount++;
	}

	const ArrayBundle *inverseBindTransformBundle = inputMesh->GetArrayBundle(kArrayInverseBindTransform);
	if (inverseBindTransformBundle->pointer)
	{
		desc[arrayCount] = inverseBindTransformBundle->descriptor;
		arrayCount++;
	}

	const ArrayBundle *morphHashBundle = inputMesh->GetArrayBundle(kArrayMorphHash);
	if (morphHashBundle->pointer)
	{
		desc[arrayCount] = morphHashBundle->descriptor;
		arrayCount++;
	}

	int32 morphCount = inputMesh->GetMorphTargetCount();
	AllocateStorage(unifiedCount, arrayCount, desc, skinSize, morphCount);

	Point3D *outputPosition = GetArray<Point3D>(kArrayPosition);
	Vector3D *outputNormal = GetArray<Vector3D>(kArrayNormal);
	Color4C *outputColor = GetArray<Color4C>(kArrayColor);

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		outputTexcoord[a] = GetArray<Point2D>(kArrayTexcoord0 + a);
	}

	unsigned_int16 *outputSurfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);

	SkinWeight *sw = GetSkinWeightData();
	unifiedCount = 0;

	for (machine a = 0; a < vertexCount; a++)
	{
		unsigned_int32 index = remapTable[a];
		if (index == unifiedCount)
		{
			outputPosition[unifiedCount] = inputPosition[a];
			outputTexcoord[0][unifiedCount] = inputTexcoord[0][a];

			if (outputTexcoord[1])
			{
				outputTexcoord[1][unifiedCount] = inputTexcoord[1][a];
			}

			if (outputNormal)
			{
				outputNormal[unifiedCount] = inputNormal[a];
			}

			if (outputColor)
			{
				outputColor[unifiedCount] = inputColor[a];
			}

			if (outputSurfaceIndex)
			{
				outputSurfaceIndex[unifiedCount] = inputSurfaceIndex[a];
			}

			if (sw)
			{
				skinWeight = skinWeightTable[a];
				int32 boneCount = skinWeight->boneCount;

				sw->boneCount = boneCount;
				for (machine b = 0; b < boneCount; b++)
				{
					sw->boneWeight[b] = skinWeight->boneWeight[b];
				}

				sw = reinterpret_cast<SkinWeight *>(sw->boneWeight + boneCount);
			}

			unifiedCount++;
		}
	}

	const MorphAttrib *morphAttrib = inputMesh->GetMorphTargetData();
	MorphAttrib *morph = GetMorphTargetData();

	for (machine m = 0; m < morphCount; m++)
	{
		unifiedCount = 0;

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index == unifiedCount)
			{
				morph[unifiedCount] = morphAttrib[a];
				unifiedCount++;
			}
		}

		morphAttrib += vertexCount;
		morph += unifiedCount;
	}

	Triangle *outputTriangle = GetArray<Triangle>(kArrayPrimitive);
	if (inputTriangle)
	{
		int32 inputPrimitiveCount = inputMesh->GetPrimitiveCount();
		int32 outputPrimitiveCount = 0;

		for (machine a = 0; a < inputPrimitiveCount; a++)
		{
			unsigned_int32 i1 = remapTable[inputTriangle[a].index[0]];
			unsigned_int32 i2 = remapTable[inputTriangle[a].index[1]];
			unsigned_int32 i3 = remapTable[inputTriangle[a].index[2]];

			if ((i1 != i2) && (i1 != i3) && (i2 != i3))
			{
				outputTriangle[outputPrimitiveCount].Set(i1, i2, i3);
				outputPrimitiveCount++;
			}
		}
	}
	else
	{
		unsigned_int32 index = 0;
		int32 primitiveCount = GetPrimitiveCount();
		for (machine a = 0; a < primitiveCount; a++)
		{
			outputTriangle[a].Set(remapTable[index], remapTable[index + 1], remapTable[index + 2]);
			index += 3;
		}
	}

	const void *pointer = boneHashBundle->pointer;
	if (pointer)
	{
		MemoryMgr::CopyMemory(pointer, GetArray(kArrayBoneHash), boneHashBundle->GetArraySize());
	}

	pointer = inverseBindTransformBundle->pointer;
	if (pointer)
	{
		MemoryMgr::CopyMemory(pointer, GetArray(kArrayInverseBindTransform), inverseBindTransformBundle->GetArraySize());
	}

	pointer = morphHashBundle->pointer;
	if (pointer)
	{
		MemoryMgr::CopyMemory(pointer, GetArray(kArrayMorphHash), morphHashBundle->GetArraySize());
	}

	delete[] skinWeightTable;
}

bool Mesh::GetTexcoordTransform(const TextureAlignData *alignData, const Transformable *transformable, Antivector4D *plane)
{
	switch (alignData->alignMode)
	{
		case kTextureAlignNatural:

			*plane = alignData->alignPlane;
			return (false);

		case kTextureAlignObjectPlane:

			*plane = alignData->alignPlane;
			break;

		case kTextureAlignWorldPlane:

			*plane = alignData->alignPlane * transformable->GetWorldTransform();
			break;

		case kTextureAlignGlobalObjectPlane:

			*plane = alignData->alignPlane;
			plane->w -= *plane ^ transformable->GetInverseWorldTransform().GetTranslation().GetVector3D();
			break;
	}

	return (true);
}

bool Mesh::GenerateTexcoords(const Transformable *transformable, const GeometryObject *object)
{
	Point2D *texcoord = GetArray<Point2D>(kArrayTexcoord);
	if (texcoord)
	{
		int32 surfaceCount = object->GetSurfaceCount();

		unsigned_int32 size = (sizeof(Antivector4D) * 2 + 2) * surfaceCount;
		Buffer buffer(size);

		Antivector4D *planeS = buffer.GetPtr<Antivector4D>();
		Antivector4D *planeT = planeS + surfaceCount;
		bool *generateS = reinterpret_cast<bool *>(planeT + surfaceCount);
		bool *generateT = generateS + surfaceCount;

		int32 genCount = 0;
		for (machine a = 0; a < surfaceCount; a++)
		{
			const SurfaceData *surfaceData = object->GetSurfaceData(a);

			bool gen = GetTexcoordTransform(&surfaceData->textureAlignData[0], transformable, &planeS[a]);
			generateS[a] = gen;
			genCount += gen;

			gen = GetTexcoordTransform(&surfaceData->textureAlignData[1], transformable, &planeT[a]);
			generateT[a] = gen;
			genCount += gen;
		}

		if (genCount != 0)
		{
			int32 vertexCount = meshVertexCount;
			const Point3D *vertex = GetArray<Point3D>(kArrayPosition);

			const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
			if (surfaceIndex)
			{
				for (machine a = 0; a < vertexCount; a++)
				{
					unsigned_int32 index = surfaceIndex[a];

					if (generateS[index])
					{
						texcoord[a].x = planeS[index] ^ vertex[a];
					}

					if (generateT[index])
					{
						texcoord[a].y = planeT[index] ^ vertex[a];
					}
				}
			}
			else
			{
				bool sgen = generateS[0];
				bool tgen = generateT[0];

				if (sgen & tgen)
				{
					for (machine a = 0; a < vertexCount; a++)
					{
						texcoord[a].x = planeS[0] ^ vertex[a];
						texcoord[a].y = planeT[0] ^ vertex[a];
					}
				}
				else if (sgen)
				{
					for (machine a = 0; a < vertexCount; a++)
					{
						texcoord[a].x = planeS[0] ^ vertex[a];
					}
				}
				else if (tgen)
				{
					for (machine a = 0; a < vertexCount; a++)
					{
						texcoord[a].y = planeT[0] ^ vertex[a];
					}
				}
			}

			return (true);
		}
	}

	return (false);
}

bool Mesh::TransformTexcoords(const Transformable *transformable, const GeometryObject *object)
{
	int32 surfaceCount = object->GetSurfaceCount();

	unsigned_int32 size = (sizeof(Antivector4D) * 2 + 2) * surfaceCount;
	Buffer buffer(size);

	Antivector4D *planeS = buffer.GetPtr<Antivector4D>();
	Antivector4D *planeT = planeS + surfaceCount;
	bool *transformS = reinterpret_cast<bool *>(planeT + surfaceCount);
	bool *transformT = transformS + surfaceCount;

	int32 transformCount = 0;
	for (machine a = 0; a < surfaceCount; a++)
	{
		const SurfaceData *surfaceData = object->GetSurfaceData(a);

		const Antivector4D& splane = surfaceData->textureAlignData[0].alignPlane;
		const Antivector4D& tplane = surfaceData->textureAlignData[1].alignPlane;

		if (surfaceData->textureAlignData[0].alignMode == kTextureAlignNatural)
		{
			if ((splane.x == 0.0F) && (tplane.x == 0.0F))
			{
				planeS[a].Set(splane.y, splane.z, 0.0F, splane.w);
			}
			else if ((splane.y == 0.0F) && (tplane.y == 0.0F))
			{
				planeS[a].Set(splane.x, -splane.z, 0.0F, splane.w);
			}
			else
			{
				planeS[a].Set(splane.x, splane.y, 0.0F, splane.w);
			}

			transformS[a] = true;
			transformCount++;
		}
		else
		{
			transformS[a] = false;
		}

		if (surfaceData->textureAlignData[1].alignMode == kTextureAlignNatural)
		{
			if ((splane.x == 0.0F) && (tplane.x == 0.0F))
			{
				planeT[a].Set(tplane.y, tplane.z, 0.0F, tplane.w);
			}
			else if ((splane.y == 0.0F) && (tplane.y == 0.0F))
			{
				planeT[a].Set(tplane.x, -tplane.z, 0.0F, tplane.w);
			}
			else
			{
				planeT[a].Set(tplane.x, tplane.y, 0.0F, tplane.w);
			}

			transformT[a] = true;
			transformCount++;
		}
		else
		{
			transformT[a] = false;
		}
	}

	if (transformCount != 0)
	{
		int32 vertexCount = meshVertexCount;
		Point2D *texcoord = GetArray<Point2D>(kArrayTexcoord);

		const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
		if (surfaceIndex)
		{
			for (machine a = 0; a < vertexCount; a++)
			{
				unsigned_int32 index = surfaceIndex[a];

				if (transformS[index])
				{
					texcoord[a].x = planeS[index] ^ texcoord[a];
				}

				if (transformT[index])
				{
					texcoord[a].y = planeT[index] ^ texcoord[a];
				}
			}
		}
		else
		{
			bool sxfrm = transformS[0];
			bool txfrm = transformT[0];

			if (sxfrm & txfrm)
			{
				for (machine a = 0; a < vertexCount; a++)
				{
					texcoord[a].x = planeS[0] ^ texcoord[a];
					texcoord[a].y = planeT[0] ^ texcoord[a];
				}
			}
			else if (sxfrm)
			{
				for (machine a = 0; a < vertexCount; a++)
				{
					texcoord[a].x = planeS[0] ^ texcoord[a];
				}
			}
			else if (txfrm)
			{
				for (machine a = 0; a < vertexCount; a++)
				{
					texcoord[a].y = planeT[0] ^ texcoord[a];
				}
			}
		}

		return (true);
	}

	return (false);
}

void Mesh::BuildNormalArray(const Mesh *inputMesh)
{
	ArrayDescriptor		desc;

	int32 vertexCount = inputMesh->GetVertexCount();

	desc.identifier = kArrayNormal;
	desc.elementCount = vertexCount;
	desc.elementSize = sizeof(Vector3D);
	desc.componentCount = 3;

	AllocateStorage(inputMesh, 1, &desc);
	CalculateNormalArray();
}

void Mesh::CalculateNormalArray(void)
{
	IndexTable *indexTable = new IndexTable;

	int32 vertexCount = GetVertexCount();
	const Point3D *position = GetArray<Point3D>(kArrayPosition);

	Buffer buffer(vertexCount * 4);
	unsigned_int32 *remapTable = buffer.GetPtr<unsigned_int32>();

	const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (surfaceIndex)
	{
		unsigned_int32 currentSurface = surfaceIndex[0];

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 surface = surfaceIndex[a];
			if (surface != currentSurface)
			{
				currentSurface = surface;
				indexTable->Clear();
			}

			const Point3D& p = position[a];
			unsigned_int32 hash = Hash(p);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if (position[index] == p)
				{
					remapTable[a] = index;
					goto next1;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next1:;
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& p = position[a];
			unsigned_int32 hash = Hash(p);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if (position[index] == p)
				{
					remapTable[a] = index;
					goto next2;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next2:;
		}
	}

	delete indexTable;

	const Triangle *triangle = GetArray<Triangle>(kArrayPrimitive);
	int32 triangleCount = GetPrimitiveCount();

	Vector3D *normal = GetArray<Vector3D>(kArrayNormal);
	MemoryMgr::ClearMemory(normal, vertexCount * sizeof(Vector3D));

	for (machine a = 0; a < triangleCount; a++)
	{
		int32 i1 = remapTable[triangle->index[0]];
		int32 i2 = remapTable[triangle->index[1]];
		int32 i3 = remapTable[triangle->index[2]];

		const Point3D& v1 = position[i1];
		const Point3D& v2 = position[i2];
		const Point3D& v3 = position[i3];

		Vector3D nrml = (v2 - v1) % (v3 - v1);

		normal[i1] += nrml;
		normal[i2] += nrml;
		normal[i3] += nrml;

		triangle++;
	}

	for (machine a = 0; a < vertexCount; a++)
	{
		unsigned_int32 index = remapTable[a];

		if (index == a)
		{
			normal[a].Normalize();
		}
		else
		{
			normal[a] = normal[index];
		}
	}
}

void Mesh::BuildTangentArray(const Mesh *inputMesh)
{
	ArrayDescriptor		desc;

	int32 vertexCount = inputMesh->GetVertexCount();

	desc.identifier = kArrayTangent;
	desc.elementCount = vertexCount;
	desc.elementSize = sizeof(Vector4D);
	desc.componentCount = 4;

	AllocateStorage(inputMesh, 1, &desc);
	CalculateTangentArray();
}

void Mesh::CalculateTangentArray(void)
{
	IndexTable *indexTable = new IndexTable;

	int32 vertexCount = GetVertexCount();
	const Point3D *position = GetArray<Point3D>(kArrayPosition);
	const Vector3D *normal = GetArray<Vector3D>(kArrayNormal);

	Buffer buffer(vertexCount * (4 + sizeof(Vector3D) * 2));
	unsigned_int32 *remapTable = buffer.GetPtr<unsigned_int32>();

	const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (surfaceIndex)
	{
		int32 currentSurface = surfaceIndex[0];

		for (machine a = 0; a < vertexCount; a++)
		{
			int32 surface = surfaceIndex[a];
			if (surface != currentSurface)
			{
				currentSurface = surface;
				indexTable->Clear();
			}

			const Point3D& p = position[a];
			const Vector3D& n = normal[a];
			unsigned_int32 hash = Hash(p) + Hash(n);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if ((position[index] == p) && (normal[index] == n))
				{
					remapTable[a] = index;
					goto next1;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next1:;
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& p = position[a];
			const Vector3D& n = normal[a];
			unsigned_int32 hash = Hash(p) + Hash(n);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if ((position[index] == p) && (normal[index] == n))
				{
					remapTable[a] = index;
					goto next2;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next2:;
		}
	}

	delete indexTable;

	Vector3D *tangent = reinterpret_cast<Vector3D *>(remapTable + vertexCount);
	Vector3D *bitangent = tangent + vertexCount;
	MemoryMgr::ClearMemory(tangent, vertexCount * (sizeof(Vector3D) * 2));

	const Point2D *texcoord = GetArray<Point2D>(kArrayTexcoord);
	const Triangle *triangle = GetArray<Triangle>(kArrayPrimitive);

	int32 primitiveCount = GetPrimitiveCount();
	for (machine a = 0; a < primitiveCount; a++)
	{
		int32 i1 = triangle->index[0];
		int32 i2 = triangle->index[1];
		int32 i3 = triangle->index[2];

		const Point3D& v1 = position[i1];
		const Point3D& v2 = position[i2];
		const Point3D& v3 = position[i3];

		const Point2D& w1 = texcoord[i1];
		const Point2D& w2 = texcoord[i2];
		const Point2D& w3 = texcoord[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		Vector3D sdir(t2 * x1 - t1 * x2, t2 * y1 - t1 * y2, t2 * z1 - t1 * z2);
		Vector3D tdir(s1 * x2 - s2 * x1, s1 * y2 - s2 * y1, s1 * z2 - s2 * z1);

		float r = s1 * t2 - s2 * t1;
		if (Fabs(r) > K::min_float)
		{
			r = 1.0F / r;
			sdir *= r;
			tdir *= r;
		}
		else
		{
			Vector3D nrml = Normalize((v2 - v1) % (v3 - v1));

			if (fabs(s1 - s2) < K::min_float)
			{
				tdir.Set(x1, y1, z1);
				sdir = tdir % nrml;
			}
			else
			{
				sdir.Set(x1, y1, z1);
				tdir = nrml % sdir;
			}
		}

		tangent[i1] += sdir;
		tangent[i2] += sdir;
		tangent[i3] += sdir;

		bitangent[i1] += tdir;
		bitangent[i2] += tdir;
		bitangent[i3] += tdir;

		triangle++;
	}

	if (GetArrayDescriptor(kArrayTangent)->componentCount == 4)
	{
		Vector4D *tangentArray = GetArray<Vector4D>(kArrayTangent);

		for (machine a = 0; a < vertexCount; a++)
		{
			const Vector3D& nrml = normal[a];
			const Vector3D& tang = tangent[a];
			tangentArray[a].GetVector3D() = Normalize(tang - nrml * (nrml * tang));
			tangentArray[a].w = (nrml % tang * bitangent[a] < 0.0F) ? -1.0F : 1.0F;
		}

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index != a)
			{
				const Vector4D& t1 = tangentArray[index];
				const Vector4D& t2 = tangentArray[a];

				if (t1.w == t2.w)
				{
					Vector3D sum = t1.GetVector3D() + t2.GetVector3D();
					if (t2.GetVector3D() * sum * InverseMag(sum) > K::sqrt_2_over_2)
					{
						tangentArray[index].GetVector3D() = sum;
						continue;
					}
				}

				remapTable[a] = a;
			}
		}

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index == a)
			{
				tangentArray[a].GetVector3D().Normalize();
			}
			else
			{
				tangentArray[a] = tangentArray[index];
			}
		}
	}
	else
	{
		Vector3D *tangentArray = GetArray<Vector3D>(kArrayTangent);

		for (machine a = 0; a < vertexCount; a++)
		{
			const Vector3D& nrml = normal[a];
			const Vector3D& tang = tangent[a];
			tangentArray[a] = Normalize(tang - nrml * (nrml * tang));
		}

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index != a)
			{
				const Vector3D& t1 = tangentArray[index];
				const Vector3D& t2 = tangentArray[a];

				Vector3D sum = t1 + t2;
				if (t2 * sum * InverseMag(sum) > K::sqrt_2_over_2)
				{
					tangentArray[index] = sum;
					continue;
				}

				remapTable[a] = a;
			}
		}

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index == a)
			{
				tangentArray[a].Normalize();
			}
			else
			{
				tangentArray[a] = tangentArray[index];
			}
		}
	}
}

void Mesh::CalculateMorphNormalArray(int32 morph)
{
	IndexTable *indexTable = new IndexTable;

	int32 vertexCount = GetVertexCount();
	MorphAttrib *attrib = morphTargetData + vertexCount * morph;

	Buffer buffer(vertexCount * 4);
	unsigned_int32 *remapTable = buffer.GetPtr<unsigned_int32>();

	const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (surfaceIndex)
	{
		unsigned_int32 currentSurface = surfaceIndex[0];

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 surface = surfaceIndex[a];
			if (surface != currentSurface)
			{
				currentSurface = surface;
				indexTable->Clear();
			}

			const Point3D& p = attrib[a].position;
			unsigned_int32 hash = Hash(p);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if (attrib[index].position == p)
				{
					remapTable[a] = index;
					goto next1;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next1:;
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& p = attrib[a].position;
			unsigned_int32 hash = Hash(p);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if (attrib[index].position == p)
				{
					remapTable[a] = index;
					goto next2;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next2:;
		}
	}

	delete indexTable;

	const Triangle *triangle = GetArray<Triangle>(kArrayPrimitive);
	int32 triangleCount = GetPrimitiveCount();

	for (machine a = 0; a < vertexCount; a++)
	{
		attrib[a].normal.Set(0.0F, 0.0F, 0.0F);
	}

	for (machine a = 0; a < triangleCount; a++)
	{
		int32 i1 = remapTable[triangle->index[0]];
		int32 i2 = remapTable[triangle->index[1]];
		int32 i3 = remapTable[triangle->index[2]];

		const Point3D& v1 = attrib[i1].position;
		const Point3D& v2 = attrib[i2].position;
		const Point3D& v3 = attrib[i3].position;

		Vector3D nrml = (v2 - v1) % (v3 - v1);

		attrib[i1].normal += nrml;
		attrib[i2].normal += nrml;
		attrib[i3].normal += nrml;

		triangle++;
	}

	for (machine a = 0; a < vertexCount; a++)
	{
		unsigned_int32 index = remapTable[a];

		if (index == a)
		{
			attrib[a].normal.Normalize();
		}
		else
		{
			attrib[a].normal = attrib[index].normal;
		}
	}
}

void Mesh::CalculateMorphTangentArray(int32 morph)
{
	IndexTable *indexTable = new IndexTable;

	int32 vertexCount = GetVertexCount();
	MorphAttrib *attrib = morphTargetData + vertexCount * morph;

	Buffer buffer(vertexCount * (4 + sizeof(Vector3D)));
	unsigned_int32 *remapTable = buffer.GetPtr<unsigned_int32>();

	const unsigned_int16 *surfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (surfaceIndex)
	{
		int32 currentSurface = surfaceIndex[0];

		for (machine a = 0; a < vertexCount; a++)
		{
			int32 surface = surfaceIndex[a];
			if (surface != currentSurface)
			{
				currentSurface = surface;
				indexTable->Clear();
			}

			const Point3D& p = attrib[a].position;
			const Vector3D& n = attrib[a].normal;
			unsigned_int32 hash = Hash(p) + Hash(n);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if ((attrib[index].position == p) && (attrib[index].normal == n))
				{
					remapTable[a] = index;
					goto next1;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next1:;
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& p = attrib[a].position;
			const Vector3D& n = attrib[a].normal;
			unsigned_int32 hash = Hash(p) + Hash(n);

			const ImmutableArray<unsigned_int32>& bucket = indexTable->GetIndexBucket(hash);
			for (unsigned_int32 index : bucket)
			{
				if ((attrib[index].position == p) && (attrib[index].normal == n))
				{
					remapTable[a] = index;
					goto next2;
				}
			}

			remapTable[a] = a;
			indexTable->AddIndex(hash, a);

			next2:;
		}
	}

	delete indexTable;

	Vector3D *tangent = reinterpret_cast<Vector3D *>(remapTable + vertexCount);
	MemoryMgr::ClearMemory(tangent, vertexCount * sizeof(Vector3D));

	const Point2D *texcoord = GetArray<Point2D>(kArrayTexcoord);
	const Triangle *triangle = GetArray<Triangle>(kArrayPrimitive);

	int32 primitiveCount = GetPrimitiveCount();
	for (machine a = 0; a < primitiveCount; a++)
	{
		int32 i1 = triangle->index[0];
		int32 i2 = triangle->index[1];
		int32 i3 = triangle->index[2];

		const Point3D& v1 = attrib[i1].position;
		const Point3D& v2 = attrib[i2].position;
		const Point3D& v3 = attrib[i3].position;

		const Point2D& w1 = texcoord[i1];
		const Point2D& w2 = texcoord[i2];
		const Point2D& w3 = texcoord[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		Vector3D sdir(t2 * x1 - t1 * x2, t2 * y1 - t1 * y2, t2 * z1 - t1 * z2);

		float r = s1 * t2 - s2 * t1;
		if (Fabs(r) > K::min_float)
		{
			sdir /= r;
		}
		else
		{
			Vector3D nrml = Normalize((v2 - v1) % (v3 - v1));

			if (fabs(s1 - s2) < K::min_float)
			{
				sdir = Vector3D(x1, y1, z1) % nrml;
			}
			else
			{
				sdir.Set(x1, y1, z1);
			}
		}

		tangent[i1] += sdir;
		tangent[i2] += sdir;
		tangent[i3] += sdir;

		triangle++;
	}

	if (GetArrayDescriptor(kArrayTangent)->componentCount == 4)
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			const Vector3D& nrml = attrib[a].normal;
			const Vector3D& tang = tangent[a];
			attrib[a].tangent = Normalize(tang - nrml * (nrml * tang));
		}

		const Vector4D *tangentArray = GetArray<Vector4D>(kArrayTangent);

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index != a)
			{
				if (tangentArray[index].w == tangentArray[a].w)
				{
					const Vector4D& t1 = attrib[index].tangent;
					const Vector4D& t2 = attrib[a].tangent;

					Vector3D sum = t1.GetVector3D() + t2.GetVector3D();
					if (t2.GetVector3D() * sum * InverseMag(sum) > K::sqrt_2_over_2)
					{
						attrib[index].tangent = sum;
						continue;
					}
				}

				remapTable[a] = a;
			}
		}

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index == a)
			{
				attrib[a].tangent.Normalize();
			}
			else
			{
				attrib[a].tangent = attrib[index].tangent;
			}
		}
	}
	else
	{
		for (machine a = 0; a < vertexCount; a++)
		{
			const Vector3D& nrml = attrib[a].normal;
			const Vector3D& tang = tangent[a];
			attrib[a].tangent = Normalize(tang - nrml * (nrml * tang));
		}

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index != a)
			{
				const Vector3D& t1 = attrib[index].tangent;
				const Vector3D& t2 = attrib[a].tangent;

				Vector3D sum = t1 + t2;
				if (t2 * sum * InverseMag(sum) > K::sqrt_2_over_2)
				{
					attrib[index].tangent = sum;
					continue;
				}

				remapTable[a] = a;
			}
		}

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = remapTable[a];
			if (index == a)
			{
				attrib[a].tangent.Normalize();
			}
			else
			{
				attrib[a].tangent = attrib[index].tangent;
			}
		}
	}
}

void Mesh::SubtractMorphBase(int32 morph, int32 base)
{
	int32 vertexCount = GetVertexCount();
	MorphAttrib *attrib = morphTargetData + vertexCount * morph;

	if (base < 0)
	{
		const Point3D *position = GetArray<Point3D>(kArrayPosition);
		const Vector3D *normal = GetArray<Vector3D>(kArrayNormal);

		if (GetArrayDescriptor(kArrayTangent)->componentCount == 4)
		{
			const Vector4D *tangent = GetArray<Vector4D>(kArrayTangent);

			for (machine a = 0; a < vertexCount; a++)
			{
				attrib[a].position -= position[a];
				attrib[a].normal -= normal[a];
				attrib[a].tangent -= tangent[a].GetVector3D();
			}
		}
		else
		{
			const Vector3D *tangent = GetArray<Vector3D>(kArrayTangent);

			for (machine a = 0; a < vertexCount; a++)
			{
				attrib[a].position -= position[a];
				attrib[a].normal -= normal[a];
				attrib[a].tangent -= tangent[a];
			}
		}
	}
	else
	{
		const MorphAttrib *baseAttrib = morphTargetData + vertexCount * base;

		for (machine a = 0; a < vertexCount; a++)
		{
			attrib[a].position -= baseAttrib[a].position;
			attrib[a].normal -= baseAttrib[a].normal;
			attrib[a].tangent -= baseAttrib[a].tangent;
		}
	}
}

void Mesh::BuildSegmentArray(const Mesh *inputMesh, int32 surfaceCount, const SurfaceData *surfaceData)
{
	ArrayDescriptor		desc[3];

	const unsigned_int16 *surfaceIndex = inputMesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
	if ((surfaceCount > 1) && (surfaceIndex))
	{
		Buffer buffer(surfaceCount * 16);
		unsigned_int32 *surfaceRemapTable = buffer.GetPtr<unsigned_int32>();
		unsigned_int32 *surfaceBackmapTable = surfaceRemapTable + surfaceCount;

		for (machine a = 0; a < surfaceCount; a++)
		{
			int32 index = surfaceData[a].materialIndex;
			unsigned_int32 count = 0;

			for (machine b = 0; b < a; b++)
			{
				count += (surfaceData[b].materialIndex <= index);
			}

			for (machine b = a + 1; b < surfaceCount; b++)
			{
				count += (surfaceData[b].materialIndex < index);
			}

			surfaceRemapTable[a] = count;
			surfaceBackmapTable[count] = a;
		}

		int32 segmentCount = 1;
		unsigned_int32 prevMaterialIndex = surfaceData[surfaceBackmapTable[0]].materialIndex;
		for (machine a = 1; a < surfaceCount; a++)
		{
			unsigned_int32 materialIndex = surfaceData[surfaceBackmapTable[a]].materialIndex;
			if (materialIndex != prevMaterialIndex)
			{
				segmentCount++;
				prevMaterialIndex = materialIndex;
			}
		}

		if (segmentCount > 1)
		{
			int32 primitiveCount = inputMesh->GetPrimitiveCount();

			desc[0].identifier = kArrayPrimitive;
			desc[0].elementCount = primitiveCount;
			desc[0].elementSize = sizeof(Triangle);
			desc[0].componentCount = 1;

			desc[1].identifier = kArraySegment;
			desc[1].elementCount = segmentCount;
			desc[1].elementSize = sizeof(SegmentData);
			desc[1].componentCount = 1;

			AllocateStorage(inputMesh, 2, desc);

			int32 *triangleCountTable = reinterpret_cast<int32 *>(surfaceBackmapTable + surfaceCount);
			for (machine a = 0; a < surfaceCount; a++)
			{
				triangleCountTable[a] = 0;
			}

			const Triangle *inputTriangle = inputMesh->GetArray<Triangle>(kArrayPrimitive);
			for (machine a = 0; a < primitiveCount; a++)
			{
				triangleCountTable[surfaceIndex[inputTriangle[a].index[0]]]++;
			}

			SegmentData *segmentData = GetArray<SegmentData>(kArraySegment);
			prevMaterialIndex = surfaceData[surfaceBackmapTable[0]].materialIndex;

			int32 *triangleStartTable = triangleCountTable + surfaceCount;
			triangleStartTable[surfaceBackmapTable[0]] = 0;

			int32 segmentPrimitiveStart = 0;
			int32 segmentPrimitiveCount = triangleCountTable[surfaceBackmapTable[0]];
			int32 triangleStart = segmentPrimitiveCount;

			for (machine a = 1; a < surfaceCount; a++)
			{
				unsigned_int32 index = surfaceBackmapTable[a];
				triangleStartTable[index] = triangleStart;

				unsigned_int32 materialIndex = surfaceData[surfaceBackmapTable[a]].materialIndex;
				if (materialIndex != prevMaterialIndex)
				{
					segmentData->materialIndex = prevMaterialIndex;
					segmentData->primitiveStart = segmentPrimitiveStart;
					segmentData->primitiveCount = segmentPrimitiveCount;
					segmentData++;

					prevMaterialIndex = materialIndex;
					segmentPrimitiveStart += segmentPrimitiveCount;
					segmentPrimitiveCount = 0;
				}

				int32 count = triangleCountTable[index];
				triangleStart += count;
				segmentPrimitiveCount += count;
			}

			segmentData->materialIndex = prevMaterialIndex;
			segmentData->primitiveStart = segmentPrimitiveStart;
			segmentData->primitiveCount = segmentPrimitiveCount;

			for (machine a = 0; a < surfaceCount; a++)
			{
				triangleCountTable[a] = 0;
			}

			Triangle *outputTriangle = GetArray<Triangle>(kArrayPrimitive);
			unsigned_int32 *primitiveRemapTable = new unsigned_int32[primitiveCount];

			for (machine a = 0; a < primitiveCount; a++)
			{
				const Triangle& triangle = inputTriangle[a];
				unsigned_int32 index = surfaceIndex[triangle.index[0]];

				int32 count = triangleCountTable[index];
				unsigned_int32 i = triangleStartTable[index] + count;

				outputTriangle[i] = triangle;
				primitiveRemapTable[a] = i;

				triangleCountTable[index] = count + 1;
			}

			delete[] primitiveRemapTable;
			return;
		}
	}

	desc[0].identifier = kArraySegment;
	desc[0].elementCount = 0;
	desc[0].elementSize = sizeof(SegmentData);
	desc[0].componentCount = 1;

	AllocateStorage(inputMesh, 1, desc);
}

void Mesh::BuildTexcoordArray(const Mesh *inputMesh, const Transformable *transformable, const GeometryObject *object)
{
	ArrayDescriptor		desc;

	int32 vertexCount = inputMesh->GetVertexCount();

	desc.identifier = kArrayTexcoord;
	desc.elementCount = vertexCount;
	desc.elementSize = sizeof(Point2D);
	desc.componentCount = 2;

	AllocateStorage(inputMesh, 1, &desc);
	GenerateTexcoords(transformable, object);
}

void Mesh::SimplifyBoundaryEdges(const Mesh *inputMesh)
{
	int32				vertexCount;
	int32				triangleCount;
	Point2D				*inputTexcoord[kMaxGeometryTexcoordCount];
	ArrayDescriptor		desc[kMaxGeometryTexcoordCount + 4];

	TopoMesh topoMesh(inputMesh);

	const Point3D *inputPosition = inputMesh->GetArray<Point3D>(kArrayPosition);
	while (topoMesh.SimplifyBoundaryEdges(inputPosition)) {}

	Buffer buffer(inputMesh->GetVertexCount() * 4);
	unsigned_int32 *remapTable = buffer.GetPtr<unsigned_int32>();
	topoMesh.CompactMesh(&vertexCount, &triangleCount, remapTable);

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	int32 arrayCount = 1;
	const Vector3D *inputNormal = inputMesh->GetArray<Vector3D>(kArrayNormal);
	if (inputNormal)
	{
		desc[1].identifier = kArrayNormal;
		desc[1].elementCount = vertexCount;
		desc[1].elementSize = sizeof(Vector3D);
		desc[1].componentCount = 3;

		arrayCount = 2;
	}

	const Color4C *inputColor = inputMesh->GetArray<Color4C>(kArrayColor);
	if (inputColor)
	{
		desc[arrayCount].identifier = kArrayColor;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		inputTexcoord[a] = inputMesh->GetArray<Point2D>(kArrayTexcoord0 + a);
		if (inputTexcoord[a])
		{
			desc[arrayCount].identifier = kArrayTexcoord0 + a;
			desc[arrayCount].elementCount = vertexCount;
			desc[arrayCount].elementSize = sizeof(Point2D);
			desc[arrayCount].componentCount = 2;
			arrayCount++;
		}
	}

	const unsigned_int16 *inputSurfaceIndex = inputMesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (inputSurfaceIndex)
	{
		desc[arrayCount].identifier = kArraySurfaceIndex;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 2;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	desc[arrayCount].identifier = kArrayPrimitive;
	desc[arrayCount].elementCount = triangleCount;
	desc[arrayCount].elementSize = sizeof(Triangle);
	desc[arrayCount].componentCount = 1;
	arrayCount++;

	AllocateStorage(vertexCount, arrayCount, desc);

	Point3D *outputPosition = GetArray<Point3D>(kArrayPosition);
	for (machine a = 0; a < vertexCount; a++)
	{
		outputPosition[a] = inputPosition[remapTable[a]];
	}

	if (inputNormal)
	{
		Vector3D *outputNormal = GetArray<Vector3D>(kArrayNormal);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputNormal[a] = inputNormal[remapTable[a]];
		}
	}

	if (inputColor)
	{
		Color4C *outputColor = GetArray<Color4C>(kArrayColor);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputColor[a] = inputColor[remapTable[a]];
		}
	}

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		if (inputTexcoord[a])
		{
			Point2D *outputTexcoord = GetArray<Point2D>(kArrayTexcoord0 + a);
			for (machine b = 0; b < vertexCount; b++)
			{
				outputTexcoord[b] = inputTexcoord[a][remapTable[b]];
			}
		}
	}

	if (inputSurfaceIndex)
	{
		unsigned_int16 *outputSurfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputSurfaceIndex[a] = inputSurfaceIndex[remapTable[a]];
		}
	}

	Triangle *outputTriangle = GetArray<Triangle>(kArrayPrimitive);
	const TopoFace *topoFace = topoMesh.GetFirstFace();
	while (topoFace)
	{
		for (machine a = 0; a < 3; a++)
		{
			const TopoVertex *vertex = topoFace->GetVertex(a);
			outputTriangle->index[a] = (unsigned_int16) vertex->GetIndex();
		}

		outputTriangle++;
		topoFace = topoFace->Next();
	}
}

void Mesh::OptimizeMesh(const Mesh *inputMesh, float collapseThreshold, int32 baseTriangleCount)
{
	int32				vertexCount;
	int32				triangleCount;
	Point2D				*inputTexcoord[kMaxGeometryTexcoordCount];
	ArrayDescriptor		desc[kMaxGeometryTexcoordCount + 8];

	int32 totalTriangleCount = inputMesh->GetPrimitiveCount();

	if (baseTriangleCount < 0)
	{
		baseTriangleCount = totalTriangleCount;
	}

	TopoMesh topoMesh(inputMesh, baseTriangleCount);

	const Point3D *inputPosition = inputMesh->GetArray<Point3D>(kArrayPosition);
	topoMesh.OptimizeMesh(inputPosition, collapseThreshold);

	Buffer buffer(inputMesh->GetVertexCount() * 4);
	unsigned_int32 *remapTable = buffer.GetPtr<unsigned_int32>();
	topoMesh.CompactMesh(&vertexCount, &triangleCount, remapTable);

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	int32 arrayCount = 1;

	const Point3D *inputPosition1 = inputMesh->GetArray<Point3D>(kArrayPosition1);
	if (inputPosition1)
	{
		desc[1].identifier = kArrayPosition1;
		desc[1].elementCount = vertexCount;
		desc[1].elementSize = sizeof(Point3D);
		desc[1].componentCount = 3;

		arrayCount = 2;
	}

	const Vector3D *inputNormal = inputMesh->GetArray<Vector3D>(kArrayNormal);
	if (inputNormal)
	{
		desc[arrayCount].identifier = kArrayNormal;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = sizeof(Vector3D);
		desc[arrayCount].componentCount = 3;
		arrayCount++;
	}

	const Color4C *inputColor0 = inputMesh->GetArray<Color4C>(kArrayColor0);
	if (inputColor0)
	{
		desc[arrayCount].identifier = kArrayColor0;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	const Color4C *inputColor1 = inputMesh->GetArray<Color4C>(kArrayColor1);
	if (inputColor1)
	{
		desc[arrayCount].identifier = kArrayColor1;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	const Color4C *inputColor2 = inputMesh->GetArray<Color4C>(kArrayColor2);
	if (inputColor2)
	{
		desc[arrayCount].identifier = kArrayColor2;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 4;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		inputTexcoord[a] = inputMesh->GetArray<Point2D>(kArrayTexcoord0 + a);
		if (inputTexcoord[a])
		{
			desc[arrayCount].identifier = kArrayTexcoord0 + a;
			desc[arrayCount].elementCount = vertexCount;
			desc[arrayCount].elementSize = sizeof(Point2D);
			desc[arrayCount].componentCount = 2;
			arrayCount++;
		}
	}

	const unsigned_int16 *inputSurfaceIndex = inputMesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
	if (inputSurfaceIndex)
	{
		desc[arrayCount].identifier = kArraySurfaceIndex;
		desc[arrayCount].elementCount = vertexCount;
		desc[arrayCount].elementSize = 2;
		desc[arrayCount].componentCount = 1;
		arrayCount++;
	}

	desc[arrayCount].identifier = kArrayPrimitive;
	desc[arrayCount].elementCount = triangleCount + totalTriangleCount - baseTriangleCount;
	desc[arrayCount].elementSize = sizeof(Triangle);
	desc[arrayCount].componentCount = 1;
	arrayCount++;

	AllocateStorage(vertexCount, arrayCount, desc);

	Point3D *outputPosition = GetArray<Point3D>(kArrayPosition);
	for (machine a = 0; a < vertexCount; a++)
	{
		outputPosition[a] = inputPosition[remapTable[a]];
	}

	if (inputPosition1)
	{
		Point3D *outputPosition1 = GetArray<Point3D>(kArrayPosition1);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputPosition1[a] = inputPosition1[remapTable[a]];
		}
	}

	if (inputNormal)
	{
		Vector3D *outputNormal = GetArray<Vector3D>(kArrayNormal);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputNormal[a] = inputNormal[remapTable[a]];
		}
	}

	if (inputColor0)
	{
		Color4C *outputColor = GetArray<Color4C>(kArrayColor0);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputColor[a] = inputColor0[remapTable[a]];
		}
	}

	if (inputColor1)
	{
		Color4C *outputColor = GetArray<Color4C>(kArrayColor1);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputColor[a] = inputColor1[remapTable[a]];
		}
	}

	if (inputColor2)
	{
		Color4C *outputColor = GetArray<Color4C>(kArrayColor2);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputColor[a] = inputColor2[remapTable[a]];
		}
	}

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		if (inputTexcoord[a])
		{
			Point2D *outputTexcoord = GetArray<Point2D>(kArrayTexcoord0 + a);
			for (machine b = 0; b < vertexCount; b++)
			{
				outputTexcoord[b] = inputTexcoord[a][remapTable[b]];
			}
		}
	}

	if (inputSurfaceIndex)
	{
		unsigned_int16 *outputSurfaceIndex = GetArray<unsigned_int16>(kArraySurfaceIndex);
		for (machine a = 0; a < vertexCount; a++)
		{
			outputSurfaceIndex[a] = inputSurfaceIndex[remapTable[a]];
		}
	}

	Triangle *outputTriangle = GetArray<Triangle>(kArrayPrimitive);
	const TopoFace *topoFace = topoMesh.GetFirstFace();
	while (topoFace)
	{
		for (machine a = 0; a < 3; a++)
		{
			const TopoVertex *vertex = topoFace->GetVertex(a);
			outputTriangle->index[a] = (unsigned_int16) vertex->GetIndex();
		}

		outputTriangle++;
		topoFace = topoFace->Next();
	}

	const Triangle *inputTriangle = inputMesh->GetArray<Triangle>(kArrayPrimitive);
	int32 vertexDelta = inputMesh->GetVertexCount() - vertexCount;

	for (machine a = baseTriangleCount; a < totalTriangleCount; a++)
	{
		outputTriangle->Set(inputTriangle[a].index[0] - vertexDelta, inputTriangle[a].index[1] - vertexDelta, inputTriangle[a].index[2] - vertexDelta);
		outputTriangle++;
	}
}

float Mesh::CalculateVolume(void) const
{
	float volume = 0.0F;

	const Point3D *position = GetArray<Point3D>(kArrayPosition);
	const Triangle *triangle = GetArray<Triangle>(kArrayPrimitive);

	int32 primitiveCount = GetPrimitiveCount();
	for (machine a = 0; a < primitiveCount; a++)
	{
		const Point3D& p1 = position[triangle->index[0]];
		const Point3D& p2 = position[triangle->index[1]];
		const Point3D& p3 = position[triangle->index[2]];

		volume += p1 * (p2 % p3);

		triangle++;
	}

	return (volume * K::one_over_6);
}

// ZYUQURM
