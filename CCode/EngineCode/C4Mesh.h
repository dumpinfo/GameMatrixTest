 

#ifndef C4Mesh_h
#define C4Mesh_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Renderable.h"
#include "C4Packing.h"


namespace C4
{
	typedef Type	TextureAlignMode;


	enum
	{
		kArrayPrimitive					= 16,
		kArraySurfaceIndex				= 17,
		kArrayMorphHash					= 18,
		kArrayBoneHash					= 19,
		kArrayInverseBindTransform		= 20,
		kArraySegment					= 21,
		kMaxGeometryArrayCount			= 22
	};


	enum GeometryArrayType
	{
		kGeometryFloat,
		kGeometryUnsignedInt8,
		kGeometryUnsignedInt16,
		kGeometryUnsignedInt32,
		kGeometryTriangle,
		kGeometryQuad,
		kGeometrySegment,
		kGeometryEdge
	};


	enum
	{
		kMaxGeometryTexcoordCount		= 2
	};


	enum : TextureAlignMode
	{
		kTextureAlignNatural			= 'NATL',
		kTextureAlignObjectPlane		= 'OPLN',
		kTextureAlignWorldPlane			= 'WPLN',
		kTextureAlignGlobalObjectPlane	= 'GOPL',
		kTextureAlignModeCount			= 4
	};


	enum
	{
		kSurfaceValidNormals			= 1 << 0,
		kSurfaceValidTangents			= 1 << 1,
		kSurfaceValidColors				= 1 << 2
	};


	class GeometryObject;


	struct TextureAlignData
	{
		TextureAlignMode	alignMode;
		Antivector4D		alignPlane;
	};


	struct SurfaceData
	{
		unsigned_int16		surfaceFlags;
		unsigned_int16		materialIndex;
		TextureAlignData	textureAlignData[2];
	};


	//# \struct	ArrayDescriptor		Contains information about a geometrical array.
	//
	//# The $ArrayDescriptor$ structure contains information about a geometrical array.
	//
	//# \def	struct ArrayDescriptor
	//
	//# \data	ArrayDescriptor


	//# \member		ArrayDescriptor

	struct ArrayDescriptor
	{
		int32		identifier;			//## The array identifier.
		int32		elementCount;		//## The number of elements in the array.
		int16		elementSize;		//## The size of each element in the array, in bytes.
		int16		componentCount;		//## The number of vector components used by each element in the array. 
	};

 
	struct ArrayBundle
	{ 
		ArrayDescriptor		descriptor;
		void				*pointer;
 
		unsigned_int32 GetArraySize(void) const
		{ 
			return (descriptor.elementCount * descriptor.elementSize); 
		}
	};

 
	struct SegmentData
	{
		unsigned_int32		materialIndex;
		int32				primitiveStart;
		int32				primitiveCount;
	};


	struct BoneWeight
	{
		int32				boneIndex;
		float				weight;
	};


	struct SkinWeight
	{
		int32				boneCount;
		BoneWeight			boneWeight[1];

		unsigned_int32 GetSize(void) const
		{
			return (sizeof(SkinWeight) - sizeof(BoneWeight) + boneCount * sizeof(BoneWeight));
		}
	};


	struct MorphAttrib
	{
		Point3D				position;
		Vector3D			normal;
		Vector3D			tangent;
	};


	class GeometryVertex : public ListElement<GeometryVertex>, public EngineMemory<GeometryVertex>
	{
		public:

			Point3D				position;
			Vector3D			normal;
			Vector4D			tangent;
			ColorRGBA			color;
			Point2D				texcoord[kMaxGeometryTexcoordCount];
			unsigned_int32		index;

			C4API GeometryVertex();
			C4API ~GeometryVertex();
	};


	class GeometryPolygon : public ListElement<GeometryPolygon>, public EngineMemory<GeometryVertex>
	{
		public:

			List<GeometryVertex>	vertexList;

			C4API GeometryPolygon();
			C4API ~GeometryPolygon();
	};


	class GeometrySurface : public ListElement<GeometrySurface>, public EngineMemory<GeometryVertex>
	{
		public:

			unsigned_int32			surfaceFlags;
			unsigned_int32			materialIndex;
			int32					texcoordCount;

			List<GeometryPolygon>	polygonList;

			C4API GeometrySurface(unsigned_int32 flags = 0, unsigned_int32 material = 0, int32 texcoord = 1);
			C4API ~GeometrySurface();
	};


	//# \class	Mesh	Encapsulates data for a single geometrical mesh.
	//
	//# The $Mesh$ class encapsulates data for a single geometrical mesh.
	//
	//# \def	class Mesh : public Packable
	//
	//# \ctor	Mesh();
	//
	//# \desc
	//# The $Mesh$ class serves as a container that holds all of the geometric data for an individual triangle mesh.
	//# The geometric data comprises various vertex attribute arrays, a triangle array, optional skinning information,
	//# and optional surface data (for multiple material assignment). One or more $Mesh$ objects are stored in a
	//# $@GeometryObject@$ structure as the raw data for one or more levels of detail making up a single instanceable
	//# geometry in the world.
	//
	//# \base	ResourceMgr/Packable	Meshes can be packed for storage in resources.
	//
	//# \also	$@GeometryObject@$


	//# \function	Mesh::GetVertexCount		Returns the number of vertices in a mesh.
	//
	//# \proto	int32 GetVertexCount(void) const;
	//
	//# \desc
	//# The $GetVertexCount$ function returns the number of vertices in a mesh.
	//
	//# \also	$@Mesh::GetPrimitiveCount@$


	//# \function	Mesh::GetPrimitiveCount		Returns the number of primitives in a mesh.
	//
	//# \proto	int32 GetPrimitiveCount(void) const;
	//
	//# \desc
	//# The $GetPrimitiveCount$ function returns the number of primitives (triangles) in a mesh. This is
	//# equivalent to the number of elements in the array with identifier $kArrayPrimitive$.
	//
	//# \also	$@Mesh::GetVertexCount@$


	//# \function	Mesh::GetArray		Returns a pointer to one of the geometrical arrays stored in a mesh.
	//
	//# \proto	void *GetArray(int32 index) const;
	//# \proto	template <typename type> type *GetArray<float>(int32 index) const;
	//
	//# \desc
	//# The $GetArray$ function and associated template function return a pointer to one of the geometrical arrays stored in a mesh.
	//# Each of the functions returns a pointer representing the same address, but with a different type. Most arrays can
	//# only contain data of one type, but some can have different types for different geometries.
	//
	//# \also	$@Mesh::GetArrayDescriptor@$


	//# \function	Mesh::GetArrayDescriptor	Returns the descriptor for one of the geometrical arrays stored in a mesh.
	//
	//# \proto	const ArrayDescriptor *GetArrayDescriptor(int32 index) const;
	//
	//# \desc
	//# The $GetArrayDescriptor$ function returns the descriptor for one of the geometrical arrays stored in a mesh.
	//# See the $@ArrayDescriptor@$ structure.
	//
	//# \also	$@ArrayDescriptor@$
	//# \also	$@Mesh::GetArray@$


	//# \function	Mesh::AllocateStorage		Allocates memory for the contents of a mesh.
	//
	//# \proto	void AllocateStorage(int32 vertexCount, int32 arrayCount, const ArrayDescriptor *arrayDesc, unsigned_int32 skinSize = 0, int32 morphCount = 0);
	//# \proto	void AllocateStorage(const Mesh *inputMesh, int32 arrayCount, const ArrayDescriptor *arrayDesc, unsigned_int32 skinSize = 0, int32 morphCount = 0);
	//
	//# \param	vertexCount		The new vertex count.
	//# \param	arrayCount		The number of arrays to allocate space for.
	//# \param	arrayDesc		A pointer to an array (of size $arrayCount$) of array descriptors.
	//# \param	skinSize		The size of the skin weight data, in bytes.
	//# \param	morphCount		The number of morph targets, excluding the base mesh.
	//# \param	inputMesh		The input mesh from which data is copied for arrays not being replaced. This may not be the same object for which $AllocateStorage$ is called.
	//
	//# \desc
	//# The $AllocateStorage$ function allocates space for all of the geometrical data stored in a mesh. There are
	//# two variants of this function, and both take an array of $@ArrayDescriptor@$ records describing what types
	//# of arrays memory needs to be allocated for. The $arrayDesc$ parameter should point to an array having the
	//# size specified by the $arrayCount$ parameter.
	//#
	//# If the function taking the $vertexCount$ parameter is called, then memory is only allocated for the arrays
	//# specified by the $arrayDesc$ array. Any data previously existing in the mesh is deleted, and the newly
	//# allocated space is uninitialized.
	//#
	//# If the function taking the $inputMesh$ parameter is called, then memory is allocated for the arrays specified
	//# by the $arrayDesc$ array in addition to any other arrays existing in the mesh specified by $inputMesh$. In this
	//# case, the new (or replaced) arrays specified by the $arrayDesc$ array are uninitialized, but data for all other
	//# arrays is copied from the input mesh.
	//
	//# \also	$@ArrayDescriptor@$


	//# \function	Mesh::CopyMesh		Copies the contents of a mesh.
	//
	//# \proto	void CopyMesh(const Mesh *inputMesh, unsigned_int32 exclusionMask = 0);
	//
	//# \param	inputMesh		The input mesh.
	//# \param	exclusionMask	A bit mask indicating which arrays should be excluded from the copy.
	//
	//# \desc
	//# The $CopyMesh$ function copies the contents of the mesh specified by the $inputMesh$ parameter to the mesh
	//# for which this function is called (the output mesh). The previous contents of the output mesh are deleted.
	//#
	//# If the $exclusionMask$ parameter is not zero, then the position of the set bits correspond to the indexes
	//# of arrays that are excluded from the copy. For example, to prevent the tangent array from being copied, the
	//# $exclusionMask$ parameter should be $1 << kArrayTangent$.
	//
	//# \also	$@Mesh::CopyRigidMesh@$


	//# \function	Mesh::CopyRigidMesh		Copies the contents of a mesh and removes skinning data.
	//
	//# \proto	void CopyRigidMesh(const Mesh *inputMesh, unsigned_int32 exclusionMask = 0);
	//
	//# \param	inputMesh		The input mesh.
	//# \param	exclusionMask	A bit mask indicating which arrays should be excluded from the copy.
	//
	//# \desc
	//# The $CopyRigidMesh$ function copies the contents of the mesh specified by the $inputMesh$ parameter to the
	//# mesh for which this function is called (the output mesh). The previous contents  of the output mesh are deleted.
	//#
	//# If the input mesh contains skinning data, then it is removed during the copy operation. The removed skinning
	//# data consists of the vertex weighting data, the bone hash array, and the inverse bind transform array.
	//#
	//# If the $exclusionMask$ parameter is not zero, then the position of the set bits correspond to the indexes
	//# of arrays that are excluded from the copy. For example, to prevent the tangent array from being copied, the
	//# $exclusionMask$ parameter should be $1 << kArrayTangent$. The bone hash array and inverse bind transform array
	//# are always excluded regardless of the value of the $exclusionMask$ parameter.
	//
	//# \also	$@Mesh::CopyMesh@$


	//# \function	Mesh::TransformMesh		Transforms the geometrical data in a mesh.
	//
	//# \proto	void TransformMesh(const Transform4D& transform);
	//
	//# \param	transform	The transform to apply to the mesh.
	//
	//# \desc
	//# The $TransformMesh$ function transforms the contents of a mesh by the matrix given by the $transform$ parameter.
	//# This operation affects the vertex array, normal array, and plane array. Any existing tangent array must be
	//# recalculated after this operation is applied.
	//
	//# \also	$@Mesh::TranslateMesh@$
	//# \also	$@Mesh::ScaleMesh@$


	//# \function	Mesh::TranslateMesh		Translates the geometrical data in a mesh.
	//
	//# \proto	void TranslateMesh(const Vector3D& translation);
	//
	//# \param	translation		The translation to apply to the mesh.
	//
	//# \desc
	//# The $TranslateMesh$ function translates the contents of a mesh by the offset vector given by the $translation$
	//# parameter. This operation affects the vertex array and plane array.
	//
	//# \also	$@Mesh::TransformMesh@$
	//# \also	$@Mesh::ScaleMesh@$


	//# \function	Mesh::ScaleMesh		Scales the geometrical data in a mesh.
	//
	//# \proto	void ScaleMesh(const Vector3D& scale);
	//
	//# \param	scale		The scale to apply to the mesh.
	//
	//# \desc
	//# The $ScaleMesh$ function scales the contents of a mesh by the scaling vector given by the $scale$ parameter.
	//# This operation affects the vertex array, normal array, and plane array. Any existing tangent array must be
	//# recalculated after this operation is applied.
	//
	//# \also	$@Mesh::TransformMesh@$
	//# \also	$@Mesh::TranslateMesh@$


	//# \function	Mesh::InvertMesh		Inverts the geometrical data in a mesh.
	//
	//# \proto	void InvertMesh(void);
	//
	//# \desc
	//# The $InvertMesh$ function inverts the contents of a mesh. This operation affects the normal array, tangent array,
	//# primitive array, edge array, and plane array.


	//# \function	Mesh::WeldMesh		Welds the surfaces a mesh together.
	//
	//# \proto	void WeldMesh(float epsilon);
	//
	//# \param	epsilon		The distance below which vertices are forced to coincide.
	//
	//# \desc
	//# The $WeldMesh$ function searches for pairs of vertices that are within the distance $epsilon$ of each other,
	//# but belong to different surfaces. When such a pair is found, one of the vertices is moved so that it coincides
	//# exactly with the other vertex in the pair. This operation welds the boundaries between surfaces together.
	//
	//# \also	$@Mesh::MendMesh@$
	//# \also	$@Mesh::UnifyMesh@$


	//# \function	Mesh::MendMesh		Mends the vertices in each surface of a mesh.
	//
	//# \proto	void MendMesh(float vertexEpsilon, float normalEpsilon, float texcoordEpsilon);
	//
	//# \param	vertexEpsilon		The vertex position difference threshold.
	//# \param	normalEpsilon		The normal vector difference threshold.
	//# \param	texcoordEpsilon		The texture coordinate difference threshold.
	//
	//# \desc
	//# The $MendMesh$ function searches for pairs of vertices in the same surface that are within the distance
	//# $vertexEpsilon$ of each other, have normal vectors whose dot product is less than $1.0 - normalEpsilon$,
	//# and whose texture coordinates are each within $texcoordEpsilon$ of each other. When such a pair is found,
	//# one of the vertices is changed so that it has exactly the same position, normal, and texture coordinates
	//# as the other vertex. If there is no normal array, then only position and texture coordinates are considered.
	//#
	//# If the input mesh has a 4D tangent array, then two vertices are considered to be distinct if the <i>w</i>
	//# coordinates of the tangents are not the same or the 3D dot product between the tangents is not positive.
	//#
	//# The mending operation is ordinarily followed by a call to the $@Mesh::UnifyMesh@$ function to remove
	//# duplicate vertices.
	//
	//# \also	$@Mesh::WeldMesh@$
	//# \also	$@Mesh::UnifyMesh@$


	//# \function	Mesh::UnifyMesh		Unifies duplicate vertices in a mesh.
	//
	//# \proto	void UnifyMesh(const Mesh *inputMesh);
	//
	//# \param	inputMesh		The input mesh.
	//
	//# \desc
	//# The $UnifyMesh$ function searches for pairs of vertices in the same surface that have identical positions,
	//# normal vectors, colors, and texture coordinates. Duplicates are removed, and index data in the primitive
	//# array is remapped. If the normal array or color array does not exist, then the unification proceeds without
	//# considering normals and/or colors.
	//#
	//# If the input mesh has a 4D tangent array, then two vertices are considered to be distinct if the <i>w</i>
	//# coordinates of the tangents are not the same or the 3D dot product between the tangents is not positive.
	//# When the $UnifyMesh$ function returns, the mesh no longer has a tangent array, so the $@Mesh::BuildTangentArray@$
	//# function must be called to generate a new tangent array for the unified mesh.
	//
	//# \also	$@Mesh::WeldMesh@$
	//# \also	$@Mesh::MendMesh@$
	//# \also	$@Mesh::BuildTangentArray@$


	//# \function	Mesh::BuildNormalArray		Builds the array of normal vectors for a mesh.
	//
	//# \proto	void BuildNormalArray(const Mesh *inputMesh);
	//
	//# \param	inputMesh	The input mesh.
	//
	//# \desc
	//# The $BuildNormalArray$ function adds a normal array to a mesh. The vertex and primitive information in the
	//# input mesh is used to a calculate normal vector for each vertex. All other array data from the input mesh
	//# is copied to the output mesh.
	//
	//# \also	$@Mesh::BuildTangentArray@$


	//# \function	Mesh::BuildTangentArray		Builds the array of tangent vectors for a mesh.
	//
	//# \proto	void BuildTangentArray(const Mesh *inputMesh);
	//
	//# \param	inputMesh	The input mesh.
	//
	//# \desc
	//# The $BuildTangentArray$ function adds a tangent array to a mesh. The vertex, normal, texture coordinate,
	//# and primitive information in the input mesh is used to a calculate tangent vector and handedness for each
	//# vertex. All other array data from the input mesh is copied to the output mesh.
	//
	//# \also	$@Mesh::BuildNormalArray@$


	class Mesh : public Packable
	{
		friend class GeometryObject;

		private:

			char					*meshStorage;
			ArrayBundle				arrayBundle[kMaxGeometryArrayCount];

			int32					meshVertexCount;
			int32					attributeArrayCount;
			unsigned_int8			attributeArrayIndex[kMaxAttributeArrayCount];

			unsigned_int32			primitiveOffset;
			unsigned_int32			attributeOffset[kMaxAttributeArrayCount];

			unsigned_int32			skinWeightSize;
			SkinWeight				*skinWeightData;

			int32					morphTargetCount;
			MorphAttrib				*morphTargetData;

			static GeometryArrayType GetArrayType(const ArrayDescriptor *desc, int32 *count);

			static bool TangentsSimilar(const Vector4D& t1, const Vector4D& t2);
			static bool GetTexcoordTransform(const TextureAlignData *alignData, const Transformable *transformable, Antivector4D *plane);

		public:

			C4API Mesh();
			C4API ~Mesh();

			const ArrayBundle *GetArrayBundle(int32 index) const
			{
				return (&arrayBundle[index]);
			}

			const ArrayDescriptor *GetArrayDescriptor(int32 index) const
			{
				return (&arrayBundle[index].descriptor);
			}

			void *GetArray(int32 index) const
			{
				return (arrayBundle[index].pointer);
			}

			template <typename type> type *GetArray(int32 index) const
			{
				return (static_cast<type *>(arrayBundle[index].pointer));
			}

			int32 GetVertexCount(void) const
			{
				return (meshVertexCount);
			}

			int32 GetPrimitiveCount(void) const
			{
				return (arrayBundle[kArrayPrimitive].descriptor.elementCount);
			}

			int32 GetAttributeArrayCount(void) const
			{
				return (attributeArrayCount);
			}

			const unsigned_int8 *GetAttributeArrayIndex(void) const
			{
				return (attributeArrayIndex);
			}

			unsigned_int32 GetPrimitiveIndexOffset(void) const
			{
				return (primitiveOffset);
			}

			unsigned_int32 GetVertexAttributeOffset(int32 index) const
			{
				return (attributeOffset[index]);
			}

			unsigned_int32 GetSkinWeightSize(void) const
			{
				return (skinWeightSize);
			}

			SkinWeight *GetSkinWeightData(void) const
			{
				return (skinWeightData);
			}

			int32 GetMorphTargetCount(void) const
			{
				return (morphTargetCount);
			}

			MorphAttrib *GetMorphTargetData(void) const
			{
				return (morphTargetData);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			C4API void AllocateStorage(int32 vertexCount, int32 arrayCount, const ArrayDescriptor *arrayDesc, unsigned_int32 skinSize = 0, int32 morphCount = 0);
			C4API void AllocateStorage(const Mesh *inputMesh, int32 arrayCount, const ArrayDescriptor *arrayDesc, unsigned_int32 skinSize = 0, int32 morphCount = 0);

			C4API void CopyMesh(const Mesh *inputMesh, unsigned_int32 exclusionMask = 0);
			C4API void CopyRigidMesh(const Mesh *inputMesh, unsigned_int32 exclusionMask = 0);

			C4API void TransformMesh(const Transform4D& transform);
			C4API void TranslateMesh(const Vector3D& translation);
			C4API void ScaleMesh(const Vector3D& scale);
			C4API void InvertMesh(void);

			C4API void WeldMesh(float epsilon);
			C4API void MendMesh(float vertexEpsilon, float normalEpsilon, float texcoordEpsilon);
			C4API void UnifyMesh(const Mesh *inputMesh);

			C4API bool GenerateTexcoords(const Transformable *transformable, const GeometryObject *object);
			C4API bool TransformTexcoords(const Transformable *transformable, const GeometryObject *object);

			C4API void BuildNormalArray(const Mesh *inputMesh);
			C4API void CalculateNormalArray(void);

			C4API void BuildTangentArray(const Mesh *inputMesh);
			C4API void CalculateTangentArray(void);

			C4API void CalculateMorphNormalArray(int32 morph);
			C4API void CalculateMorphTangentArray(int32 morph);
			C4API void SubtractMorphBase(int32 morph, int32 base);

			C4API void BuildSegmentArray(const Mesh *inputMesh, int32 surfaceCount, const SurfaceData *surfaceData);
			C4API void BuildTexcoordArray(const Mesh *inputMesh, const Transformable *transformable, const GeometryObject *object);

			C4API void SimplifyBoundaryEdges(const Mesh *inputMesh);
			C4API void OptimizeMesh(const Mesh *inputMesh, float collapseThreshold, int32 baseTriangleCount = -1);

			C4API float CalculateVolume(void) const;
	};
}


#endif

// ZYUQURM
