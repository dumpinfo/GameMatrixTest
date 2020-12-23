 

#ifndef C4Primitives_h
#define C4Primitives_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Geometries.h"
#include "C4Paths.h"


namespace C4
{
	enum
	{
		kGeometryPrimitive			= 'PRIM'
	};


	//# \enum	PrimitiveType

	enum : PrimitiveType
	{
		kPrimitivePlate				= 'PLAT',		//## Planar rectangle primitive.
		kPrimitiveDisk				= 'DISK',		//## Planar disk primitive.
		kPrimitiveHole				= 'HOLE',		//## Plate primitive with a hole cut out of the center.
		kPrimitiveAnnulus			= 'ANNU',		//## Planar annulus primitive.
		kPrimitiveBox				= 'BOX ',		//## Box primitive.
		kPrimitivePyramid			= 'PYRA',		//## Pyramid primitive.
		kPrimitiveCylinder			= 'CYLD',		//## Cylinder primitive.
		kPrimitiveCone				= 'CONE',		//## Cone primitive.
		kPrimitiveTruncatedCone		= 'TCON',		//## Truncated cone primitive.
		kPrimitiveSphere			= 'SPHR',		//## Sphere primitive.
		kPrimitiveDome				= 'DOME',		//## Dome primitive.
		kPrimitiveTorus				= 'TORS',		//## Torus primitive.
		kPrimitiveTube				= 'TUBE',		//## Path tube primitive.
		kPrimitiveExtrusion			= 'EXTR',		//## Path extrusion primitive.
		kPrimitiveRevolution		= 'REVO'		//## Path revolution primitive.
	};


	//# \enum	PrimitiveFlags

	enum
	{
		kPrimitiveEndcap			= 1 << 0,		//## The primitive should be built with endcaps, if applicable.
		kPrimitiveInvert			= 1 << 1		//## The primitive should be built inverted.
	};


	//# \class	PrimitiveGeometryObject		Encapsulates data pertaining to a primitive geometry.
	//
	//# The $PrimitiveGeometryObject$ class encapsulates data pertaining to a primitive geometry.
	//
	//# \def	class PrimitiveGeometryObject : public GeometryObject
	//
	//# \ctor	PrimitiveGeometryObject(PrimitiveType type);
	//
	//# The constructor has protected access. The $PrimitiveGeometryObject$ class can only exist as the base class for another class.
	//
	//# \param	type	The type of the primitive geometry. See below for a list of possible types.
	//
	//# \desc
	//# The $PrimitiveGeometryObject$ class is the base class for all primitive geometry object data. The $type$ parameter
	//# can be one of the following constants, and it is specified by the subclass's constructor.
	//
	//# \table	PrimitiveType
	//
	//# \base	GeometryObject		A $PrimitiveGeometryObject$ is an object that can be shared by multiple primitive geometry nodes.
	//
	//# \also	$@PrimitiveGeometry@$


	//# \function	PrimitiveGeometryObject::GetPrimitiveType		Returns the specific type of a primitive geometry.
	//
	//# \proto	PrimitiveType GetPrimitiveType(void) const;
	//
	//# \desc
	//# The $GetPrimitiveType$ function returns the specific primitive geometry type, which can be one of the following constants.
	//
	//# \table	PrimitiveType


	//# \function	PrimitiveGeometryObject::GetPrimitiveFlags		Returns the primitive geometry flags.
	//
	//# \proto	unsigned_int32 GetPrimitiveFlags(void) const;
	//
	//# \desc
	//# The $GetPrimitiveFlags$ function returns the primitive geometry flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	PrimitiveFlags
	//
	//# \also	$@PrimitiveGeometryObject::SetPrimitiveFlags@$


	//# \function	PrimitiveGeometryObject::SetPrimitiveFlags		Sets the primitive geometry flags.
	//
	//# \proto	void SetPrimitiveFlags(unsigned_int32 flags);
	// 
	//# \param	flags	The new primitive geometry flags.
	//
	//# \desc 
	//# The $SetPrimitiveFlags$ function sets the primitive geometry flags. The $flags$ parameter can be a combination (through logical OR)
	//# of the following constants. 
	//
	//# \table	PrimitiveFlags
	// 
	//# \also	$@PrimitiveGeometryObject::GetPrimitiveFlags@$
 
 
	class PrimitiveGeometryObject : public GeometryObject
	{
		friend class GeometryObject;
 
		private:

			PrimitiveType		primitiveType;
			unsigned_int32		primitiveFlags;

			int32				buildLevelCount;
			int32				maxSubdivX;
			int32				maxSubdivY;

			static PrimitiveGeometryObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API PrimitiveGeometryObject(PrimitiveType type);
			C4API ~PrimitiveGeometryObject();

			static void TriangulateDisk(int32 vertexCount, unsigned_int32 baseIndex, Triangle *triangle);

			C4API virtual void BuildPrimitiveLevel(int32 level, const Transformable *transformable);
			C4API void BuildStandardArrays(Mesh *inputMesh, Mesh *outputMesh, const Transformable *transformable) const;

		public:

			PrimitiveType GetPrimitiveType(void) const
			{
				return (primitiveType);
			}

			unsigned_int32 GetPrimitiveFlags(void) const
			{
				return (primitiveFlags);
			}

			void SetPrimitiveFlags(unsigned_int32 flags)
			{
				primitiveFlags = flags;
			}

			int32 GetBuildLevelCount(void) const
			{
				return (buildLevelCount);
			}

			void SetBuildLevelCount(int32 count)
			{
				buildLevelCount = count;
			}

			int32 GetMaxSubdivX(void) const
			{
				return (maxSubdivX);
			}

			void SetMaxSubdivX(int32 subdiv)
			{
				maxSubdivX = subdiv;
			}

			int32 GetMaxSubdivY(void) const
			{
				return (maxSubdivY);
			}

			void SetMaxSubdivY(int32 subdiv)
			{
				maxSubdivY = subdiv;
			}

			void SetMaxSubdiv(int32 x, int32 y)
			{
				maxSubdivX = x;
				maxSubdivY = y;
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void Preprocess(unsigned_int32 dynamicFlags) override;

			virtual Point3D GetInitialPrimitiveSupportPoint(void) const;
			virtual Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const;
			virtual void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const;

			C4API int32 GetMaxCollisionLevel(void) const override;

			C4API virtual void Build(Geometry *geometry);
	};


	//# \class	PlateGeometryObject		Encapsulates data pertaining to a plate primitive.
	//
	//# The $PlateGeometryObject$ class encapsulates data pertaining to a plate primitive.
	//
	//# \def	class PlateGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	PlateGeometryObject(const Vector2D& size);
	//
	//# \param	size	The size of the plate.
	//
	//# \desc
	//# The $PlateGeometryObject$ class encapsulates data pertaining to a plate primitive. In its local coordinate space,
	//# a plate primitive lies in the <i>x</i>-<i>y</i> plane with one of its corners at the origin. The components of
	//# the $size$ parameter define the extents of the plate in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \base	PrimitiveGeometryObject		A $PlateGeometryObject$ is an object that can be shared by multiple plate geometry nodes.
	//
	//# \also	$@PlateGeometry@$


	//# \function	PlateGeometryObject::GetPlateSize		Returns the plate size.
	//
	//# \proto	const Vector2D& GetPlateSize(void) const;
	//
	//# \desc
	//# The $GetPlateSize$ function returns the size of a plate primitive. The components of the return value
	//# correspond to the extents of the plate in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@PlateGeometryObject::SetPlateSize@$


	//# \function	PlateGeometryObject::SetPlateSize		Sets the plate size.
	//
	//# \proto	void SetPlateSize(const Vector2D& size);
	//
	//# \param	size	The new plate size.
	//
	//# \desc
	//# The $SetPlateSize$ function sets the size of a plate primitive. The components of the $size$ parameter
	//# define the extents of the plate in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@PlateGeometryObject::GetPlateSize@$


	class PlateGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		plateSize;

			SurfaceData		staticSurfaceData[1];

			PlateGeometryObject();
			~PlateGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			PlateGeometryObject(const Vector2D& size);

			const Vector2D& GetPlateSize(void) const
			{
				return (plateSize);
			}

			void SetPlateSize(const Vector2D& size)
			{
				plateSize = size;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	DiskGeometryObject		Encapsulates data pertaining to a disk primitive.
	//
	//# The $DiskGeometryObject$ class encapsulates data pertaining to a disk primitive.
	//
	//# \def	class DiskGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	DiskGeometryObject(const Vector2D& size);
	//
	//# \param	size	The size of the disk.
	//
	//# \desc
	//# The $DiskGeometryObject$ class encapsulates data pertaining to a disk primitive. In its local coordinate space,
	//# a disk primitive lies in the <i>x</i>-<i>y</i> plane with its center at the origin. The components of
	//# the $size$ parameter define the semiaxis lengths in the <i>x</i> and <i>y</i> directions.
	//
	//# \base	PrimitiveGeometryObject		A $DiskGeometryObject$ is an object that can be shared by multiple disk geometry nodes.
	//
	//# \also	$@DiskGeometry@$


	//# \function	DiskGeometryObject::GetDiskSize		Returns the disk size.
	//
	//# \proto	const Vector2D& GetDiskSize(void) const;
	//
	//# \desc
	//# The $GetDiskSize$ function returns the size of a disk primitive. The components of the return value
	//# correspond to the semiaxis lengths in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@DiskGeometryObject::SetDiskSize@$


	//# \function	DiskGeometryObject::SetDiskSize		Sets the disk size.
	//
	//# \proto	void SetDiskSize(const Vector2D& size);
	//
	//# \param	size	The new disk size.
	//
	//# \desc
	//# The $SetDiskSize$ function sets the size of a disk primitive. The components of the $size$ parameter
	//# define the semiaxis lengths in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@DiskGeometryObject::GetDiskSize@$


	class DiskGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		diskSize;
			float			ratioXY;

			SurfaceData		staticSurfaceData[1];

			DiskGeometryObject();
			~DiskGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			DiskGeometryObject(const Vector2D& size);

			const Vector2D& GetDiskSize(void) const
			{
				return (diskSize);
			}

			void SetDiskSize(const Vector2D& size)
			{
				diskSize = size;
				ratioXY = size.x / size.y;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D GetInitialPrimitiveSupportPoint(void) const override;
			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	HoleGeometryObject		Encapsulates data pertaining to a hole primitive.
	//
	//# The $HoleGeometryObject$ class encapsulates data pertaining to a hole primitive.
	//
	//# \def	class HoleGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	HoleGeometryObject(const Vector2D& inner, const Vector2D& outer);
	//
	//# \param	inner	The size of the inner hole.
	//# \param	outer	The size of the outer plate.
	//
	//# \desc
	//# The $HoleGeometryObject$ class encapsulates data pertaining to a hole primitive. In its local coordinate space,
	//# a hole primitive lies in the <i>x</i>-<i>y</i> plane with the center of the hole at the origin. The components of
	//# the $inner$ parameter define the semiaxis lengths of the hole in the <i>x</i> and <i>y</i> directions, and the components of
	//# the $outer$ parameter define the extents of the plate in the positive and negative <i>x</i> and <i>y</i> directions.
	//
	//# \base	PrimitiveGeometryObject		A $HoleGeometryObject$ is an object that can be shared by multiple hole geometry nodes.
	//
	//# \also	$@HoleGeometry@$


	//# \function	HoleGeometryObject::GetInnerSize		Returns the inner hole size.
	//
	//# \proto	const Vector2D& GetInnerSize(void) const;
	//
	//# \desc
	//# The $GetInnerSize$ function returns the size of the central hole for a hole primitive. The components of the return value
	//# correspond to the semiaxis lengths of the hole in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@HoleGeometryObject::SetInnerSize@$
	//# \also	$@HoleGeometryObject::GetOuterSize@$
	//# \also	$@HoleGeometryObject::SetOuterSize@$


	//# \function	HoleGeometryObject::SetInnerSize		Sets the inner hole size.
	//
	//# \proto	void SetInnerSize(const Vector2D& inner);
	//
	//# \param	inner	The new inner hole size.
	//
	//# \desc
	//# The $SetInnerSize$ function sets the size of the central hole for a hole primitive. The components of the $inner$ parameter
	//# define the semiaxis lengths of the hole in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@HoleGeometryObject::GetInnerSize@$
	//# \also	$@HoleGeometryObject::GetOuterSize@$
	//# \also	$@HoleGeometryObject::SetOuterSize@$


	//# \function	HoleGeometryObject::GetOuterSize		Returns the outer plate size.
	//
	//# \proto	const Vector2D& GetOuterSize(void) const;
	//
	//# \desc
	//# The $GetOuterSize$ function returns the size of the outer plate for a hole primitive. The components of the return value
	//# correspond to the extents in the positive and negative <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@HoleGeometryObject::SetOuterSize@$
	//# \also	$@HoleGeometryObject::GetInnerSize@$
	//# \also	$@HoleGeometryObject::SetInnerSize@$


	//# \function	HoleGeometryObject::SetOuterSize		Sets the outer plate size.
	//
	//# \proto	void SetOuterSize(const Vector2D& outer);
	//
	//# \param	outer	The new outer plate size.
	//
	//# \desc
	//# The $SetOuterSize$ function sets the size of the outer plate for a hole primitive. The components of the $outer$ parameter
	//# define the extents in the positive and negative <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@HoleGeometryObject::GetOuterSize@$
	//# \also	$@HoleGeometryObject::GetInnerSize@$
	//# \also	$@HoleGeometryObject::SetInnerSize@$


	class HoleGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		innerSize;
			Vector2D		outerSize;
			float			innerRatioXY;

			SurfaceData		staticSurfaceData[1];

			HoleGeometryObject();
			~HoleGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			HoleGeometryObject(const Vector2D& inner, const Vector2D& outer);

			const Vector2D& GetInnerSize(void) const
			{
				return (innerSize);
			}

			void SetInnerSize(const Vector2D& inner)
			{
				innerSize = inner;
				innerRatioXY = inner.x / inner.y;
			}

			const Vector2D& GetOuterSize(void) const
			{
				return (outerSize);
			}

			void SetOuterSize(const Vector2D& outer)
			{
				outerSize = outer;
			}

			float GetInnerRatioXY(void) const
			{
				return (innerRatioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;
	};


	//# \class	AnnulusGeometryObject		Encapsulates data pertaining to an annulus primitive.
	//
	//# The $AnnulusGeometryObject$ class encapsulates data pertaining to an annulus primitive.
	//
	//# \def	class AnnulusGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	AnnulusGeometryObject(const Vector2D& inner, const Vector2D& outer);
	//
	//# \param	inner	The size of the inner hole.
	//# \param	outer	The size of the outer disk.
	//
	//# \desc
	//# The $AnnulusGeometryObject$ class encapsulates data pertaining to an annulus primitive. In its local coordinate space,
	//# an annulus primitive lies in the <i>x</i>-<i>y</i> plane with the center at the origin. The components of
	//# the $inner$ parameter define the semiaxis lengths of the inner hole in the <i>x</i> and <i>y</i> directions, and the components of
	//# the $outer$ parameter define the semiaxis lengths of the outer disk in the <i>x</i> and <i>y</i> directions.
	//
	//# \base	PrimitiveGeometryObject		An $AnnulusGeometryObject$ is an object that can be shared by multiple annulus geometry nodes.
	//
	//# \also	$@AnnulusGeometry@$


	//# \function	AnnulusGeometryObject::GetInnerSize		Returns the inner hole size.
	//
	//# \proto	const Vector2D& GetInnerSize(void) const;
	//
	//# \desc
	//# The $GetInnerSize$ function returns the size of the inner hole for an annulus primitive. The components of the return value
	//# correspond to the semiaxis lengths of the inner hole in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@AnnulusGeometryObject::SetInnerSize@$
	//# \also	$@AnnulusGeometryObject::GetOuterSize@$
	//# \also	$@AnnulusGeometryObject::SetOuterSize@$


	//# \function	AnnulusGeometryObject::SetInnerSize		Sets the inner hole size.
	//
	//# \proto	void SetInnerSize(const Vector2D& inner);
	//
	//# \param	inner	The new inner hole size.
	//
	//# \desc
	//# The $SetInnerSize$ function sets the size of the inner hole for an annulus primitive. The components of the $inner$ parameter
	//# define the semiaxis lengths of the inner hole in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@AnnulusGeometryObject::GetInnerSize@$
	//# \also	$@AnnulusGeometryObject::GetOuterSize@$
	//# \also	$@AnnulusGeometryObject::SetOuterSize@$


	//# \function	AnnulusGeometryObject::GetOuterSize		Returns the outer disk size.
	//
	//# \proto	const Vector2D& GetOuterSize(void) const;
	//
	//# \desc
	//# The $GetOuterSize$ function returns the size of the outer disk for an annulus primitive. The components of the return value
	//# correspond to the semiaxis lengths of the outer disk in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@AnnulusGeometryObject::SetOuterSize@$
	//# \also	$@AnnulusGeometryObject::GetInnerSize@$
	//# \also	$@AnnulusGeometryObject::SetInnerSize@$


	//# \function	AnnulusGeometryObject::SetOuterSize		Sets the outer disk size.
	//
	//# \proto	void SetOuterSize(const Vector2D& outer);
	//
	//# \param	outer	The new outer disk size.
	//
	//# \desc
	//# The $SetOuterSize$ function sets the size of the outer disk for an annulus primitive. The components of the $outer$ parameter
	//# define the semiaxis lengths of the outer disk in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@AnnulusGeometryObject::GetOuterSize@$
	//# \also	$@AnnulusGeometryObject::GetInnerSize@$
	//# \also	$@AnnulusGeometryObject::SetInnerSize@$


	class AnnulusGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		innerSize;
			Vector2D		outerSize;
			float			innerRatioXY;
			float			outerRatioXY;

			SurfaceData		staticSurfaceData[1];

			AnnulusGeometryObject();
			~AnnulusGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			AnnulusGeometryObject(const Vector2D& inner, const Vector2D& outer);

			const Vector2D& GetInnerSize(void) const
			{
				return (innerSize);
			}

			void SetInnerSize(const Vector2D& inner)
			{
				innerSize = inner;
				innerRatioXY = inner.x / inner.y;
			}

			const Vector2D& GetOuterSize(void) const
			{
				return (outerSize);
			}

			void SetOuterSize(const Vector2D& outer)
			{
				outerSize = outer;
				outerRatioXY = outer.x / outer.y;
			}

			float GetInnerRatioXY(void) const
			{
				return (innerRatioXY);
			}

			float GetOuterRatioXY(void) const
			{
				return (outerRatioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;
	};


	//# \class	BoxGeometryObject		Encapsulates data pertaining to a box primitive.
	//
	//# The $BoxGeometryObject$ class encapsulates data pertaining to a box primitive.
	//
	//# \def	class BoxGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	BoxGeometryObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxGeometryObject$ class encapsulates data pertaining to a box primitive. In its local coordinate space,
	//# a box primitive has one of its corners at the origin, and the components of the $size$ parameter define the extents
	//# of the box in the positive <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \base	PrimitiveGeometryObject		A $BoxGeometryObject$ is an object that can be shared by multiple box geometry nodes.
	//
	//# \also	$@BoxGeometry@$


	//# \function	BoxGeometryObject::GetBoxSize		Returns the box size.
	//
	//# \proto	const Vector3D& GetBoxSize(void) const;
	//
	//# \desc
	//# The $GetBoxSize$ function returns the size of a box primitive. The components of the return value
	//# correspond to the extents of the box in the positive <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \also	$@BoxGeometryObject::SetBoxSize@$


	//# \function	BoxGeometryObject::SetBoxSize		Sets the box size.
	//
	//# \proto	void SetBoxSize(const Vector3D& size);
	//
	//# \param	size	The new box size.
	//
	//# \desc
	//# The $SetBoxSize$ function sets the size of a box primitive. The components of the $size$ parameter
	//# define the extents of the box in the positive <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \also	$@BoxGeometryObject::GetBoxSize@$


	class BoxGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector3D		boxSize;

			SurfaceData		staticSurfaceData[6];

			BoxGeometryObject();
			~BoxGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			BoxGeometryObject(const Vector3D& size);

			const Vector3D& GetBoxSize(void) const
			{
				return (boxSize);
			}

			void SetBoxSize(const Vector3D& size)
			{
				boxSize = size;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	PyramidGeometryObject		Encapsulates data pertaining to a pyramid primitive.
	//
	//# The $PyramidGeometryObject$ class encapsulates data pertaining to a pyramid primitive.
	//
	//# \def	class PyramidGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	PyramidGeometryObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the pyramid base.
	//# \param	height	The height of the pyramid.
	//
	//# \desc
	//# The $PyramidGeometryObject$ class encapsulates data pertaining to a pyramid primitive. In its local coordinate space,
	//# a pyramid primitive has one of the corners of its base at the origin, and the components of the $size$ parameter define
	//# the extents of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the height of
	//# the pyramid's apex above the base in the positive <i>z</i> direction.
	//
	//# \base	PrimitiveGeometryObject		A $PyramidGeometryObject$ is an object that can be shared by multiple pyramid geometry nodes.
	//
	//# \also	$@PyramidGeometry@$


	//# \function	PyramidGeometryObject::GetPyramidSize		Returns the pyramid base size.
	//
	//# \proto	const Vector2D& GetPyramidSize(void) const;
	//
	//# \desc
	//# The $GetPyramidSize$ function returns the size of the base of a pyramid primitive. The components of the return value
	//# correspond to the extents of the base of the pyramid in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@PyramidGeometryObject::SetPyramidSize@$
	//# \also	$@PyramidGeometryObject::GetPyramidHeight@$
	//# \also	$@PyramidGeometryObject::SetPyramidHeight@$


	//# \function	PyramidGeometryObject::SetPyramidSize		Sets the pyramid base size.
	//
	//# \proto	void SetPyramidSize(const Vector2D& size);
	//
	//# \param	size	The new pyramid base size.
	//
	//# \desc
	//# The $SetPyramidSize$ function sets the size of the base of a pyramid primitive. The components of the $size$ parameter
	//# define the extents of the base of the pyramid in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@PyramidGeometryObject::GetPyramidSize@$
	//# \also	$@PyramidGeometryObject::GetPyramidHeight@$
	//# \also	$@PyramidGeometryObject::SetPyramidHeight@$


	//# \function	PyramidGeometryObject::GetPyramidHeight		Returns the pyramid height.
	//
	//# \proto	float GetPyramidHeight(void) const;
	//
	//# \desc
	//# The $GetPyramidHeight$ function returns the height of a pyramid primitive. The return value corresponds to
	//# the height of the pyramid's apex above the base in the positive <i>z</i> direction.
	//
	//# \also	$@PyramidGeometryObject::SetPyramidHeight@$
	//# \also	$@PyramidGeometryObject::GetPyramidSize@$
	//# \also	$@PyramidGeometryObject::SetPyramidSize@$


	//# \function	PyramidGeometryObject::SetPyramidHeight		Sets the pyramid height.
	//
	//# \proto	void SetPyramidHeight(float height);
	//
	//# \param	height	The new pyramid height.
	//
	//# \desc
	//# The $SetPyramidHeight$ function sets the height of a pyramid primitive. The $height$ parameter defines
	//# the height of the pyramid's apex above the base in the positive <i>z</i> direction.
	//
	//# \also	$@PyramidGeometryObject::GetPyramidHeight@$
	//# \also	$@PyramidGeometryObject::GetPyramidSize@$
	//# \also	$@PyramidGeometryObject::SetPyramidSize@$


	class PyramidGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		pyramidSize;
			float			pyramidHeight;

			SurfaceData		staticSurfaceData[5];

			PyramidGeometryObject();
			~PyramidGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			PyramidGeometryObject(const Vector2D& size, float height);

			const Vector2D& GetPyramidSize(void) const
			{
				return (pyramidSize);
			}

			void SetPyramidSize(const Vector2D& size)
			{
				pyramidSize = size;
			}

			float GetPyramidHeight(void) const
			{
				return (pyramidHeight);
			}

			void SetPyramidHeight(float height)
			{
				pyramidHeight = height;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	CylinderGeometryObject		Encapsulates data pertaining to a cylinder primitive.
	//
	//# The $CylinderGeometryObject$ class encapsulates data pertaining to a cylinder primitive.
	//
	//# \def	class CylinderGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	CylinderGeometryObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderGeometryObject$ class encapsulates data pertaining to a cylinder primitive. In its local coordinate space,
	//# a cylinder primitive has the center of its base at the origin, and the components of the $size$ parameter define the
	//# semiaxis lengths of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the height
	//# of the cylinder above the base in the positive <i>z</i> direction.
	//
	//# \base	PrimitiveGeometryObject		A $CylinderGeometryObject$ is an object that can be shared by multiple cylinder geometry nodes.
	//
	//# \also	$@CylinderGeometry@$


	//# \function	CylinderGeometryObject::GetCylinderSize		Returns the cylinder base size.
	//
	//# \proto	const Vector2D& GetCylinderSize(void) const;
	//
	//# \desc
	//# The $GetCylinderSize$ function returns the size of the base of a cylinder primitive. The components of the return value
	//# correspond to the semiaxis lengths of the base of the cylinder in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@CylinderGeometryObject::SetCylinderSize@$
	//# \also	$@CylinderGeometryObject::GetCylinderHeight@$
	//# \also	$@CylinderGeometryObject::SetCylinderHeight@$


	//# \function	CylinderGeometryObject::SetCylinderSize		Sets the cylinder base size.
	//
	//# \proto	void SetCylinderSize(const Vector2D& size);
	//
	//# \param	size	The new cylinder base size.
	//
	//# \desc
	//# The $SetCylinderSize$ function sets the size of the base of a cylinder primitive. The components of the $size$ parameter
	//# define the semiaxis lengths of the base of the cylinder in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@CylinderGeometryObject::GetCylinderSize@$
	//# \also	$@CylinderGeometryObject::GetCylinderHeight@$
	//# \also	$@CylinderGeometryObject::SetCylinderHeight@$


	//# \function	CylinderGeometryObject::GetCylinderHeight		Returns the cylinder height.
	//
	//# \proto	float GetCylinderHeight(void) const;
	//
	//# \desc
	//# The $GetCylinderHeight$ function returns the height of a cylinder primitive. The return value corresponds to
	//# the height of the cylinder above the base in the positive <i>z</i> direction.
	//
	//# \also	$@CylinderGeometryObject::SetCylinderHeight@$
	//# \also	$@CylinderGeometryObject::GetCylinderSize@$
	//# \also	$@CylinderGeometryObject::SetCylinderSize@$


	//# \function	CylinderGeometryObject::SetCylinderHeight		Sets the cylinder height.
	//
	//# \proto	void SetCylinderHeight(float height);
	//
	//# \param	height	The new cylinder height.
	//
	//# \desc
	//# The $SetCylinderHeight$ function sets the height of a cylinder primitive. The $height$ parameter defines
	//# the height of the cylinder above the base in the positive <i>z</i> direction.
	//
	//# \also	$@CylinderGeometryObject::GetCylinderHeight@$
	//# \also	$@CylinderGeometryObject::GetCylinderSize@$
	//# \also	$@CylinderGeometryObject::SetCylinderSize@$


	class CylinderGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		cylinderSize;
			float			cylinderHeight;
			float			ratioXY;

			SurfaceData		staticSurfaceData[3];

			CylinderGeometryObject();
			~CylinderGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			CylinderGeometryObject(const Vector2D& size, float height);

			const Vector2D& GetCylinderSize(void) const
			{
				return (cylinderSize);
			}

			void SetCylinderSize(const Vector2D& size)
			{
				cylinderSize = size;
				ratioXY = size.x / size.y;
			}

			float GetCylinderHeight(void) const
			{
				return (cylinderHeight);
			}

			void SetCylinderHeight(float height)
			{
				cylinderHeight = height;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	ConeGeometryObject		Encapsulates data pertaining to a cone primitive.
	//
	//# The $ConeGeometryObject$ class encapsulates data pertaining to a cone primitive.
	//
	//# \def	class ConeGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	ConeGeometryObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cone base.
	//# \param	height	The height of the cone.
	//
	//# \desc
	//# The $ConeGeometryObject$ class encapsulates data pertaining to a cone primitive. In its local coordinate space,
	//# a cone primitive has the center of its base at the origin, and the components of the $size$ parameter define the
	//# semiaxis lengths of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the height
	//# of the cone's apex above the base in the positive <i>z</i> direction.
	//
	//# \base	PrimitiveGeometryObject		A $ConeGeometryObject$ is an object that can be shared by multiple cone geometry nodes.
	//
	//# \also	$@ConeGeometry@$


	//# \function	ConeGeometryObject::GetConeSize		Returns the cone base size.
	//
	//# \proto	const Vector2D& GetConeSize(void) const;
	//
	//# \desc
	//# The $GetConeSize$ function returns the size of the base of a cone primitive. The components of the return value
	//# correspond to the semiaxis lengths of the base of the cone in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@ConeGeometryObject::SetConeSize@$
	//# \also	$@ConeGeometryObject::GetConeHeight@$
	//# \also	$@ConeGeometryObject::SetConeHeight@$


	//# \function	ConeGeometryObject::SetConeSize		Sets the cone base size.
	//
	//# \proto	void SetConeSize(const Vector2D& size);
	//
	//# \param	size	The new cone base size.
	//
	//# \desc
	//# The $SetConeSize$ function sets the size of the base of a cone primitive. The components of the $size$ parameter
	//# define the semiaxis lengths of the base of the cone in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@ConeGeometryObject::GetConeSize@$
	//# \also	$@ConeGeometryObject::GetConeHeight@$
	//# \also	$@ConeGeometryObject::SetConeHeight@$


	//# \function	ConeGeometryObject::GetConeHeight		Returns the cone height.
	//
	//# \proto	float GetConeHeight(void) const;
	//
	//# \desc
	//# The $GetConeHeight$ function returns the height of a cone primitive. The return value corresponds to
	//# the height of the cone's apex above the base in the positive <i>z</i> direction.
	//
	//# \also	$@ConeGeometryObject::SetConeHeight@$
	//# \also	$@ConeGeometryObject::GetConeSize@$
	//# \also	$@ConeGeometryObject::SetConeSize@$


	//# \function	ConeGeometryObject::SetConeHeight		Sets the cone height.
	//
	//# \proto	void SetConeHeight(float height);
	//
	//# \param	height	The new cone height.
	//
	//# \desc
	//# The $SetConeHeight$ function sets the height of a cone primitive. The $height$ parameter defines
	//# the height of the cone's apex above the base in the positive <i>z</i> direction.
	//
	//# \also	$@ConeGeometryObject::GetConeHeight@$
	//# \also	$@ConeGeometryObject::GetConeSize@$
	//# \also	$@ConeGeometryObject::SetConeSize@$


	class ConeGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		coneSize;
			float			coneHeight;
			float			ratioXY;

			SurfaceData		staticSurfaceData[2];

			ConeGeometryObject();
			~ConeGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			ConeGeometryObject(const Vector2D& size, float height);

			const Vector2D& GetConeSize(void) const
			{
				return (coneSize);
			}

			void SetConeSize(const Vector2D& size)
			{
				coneSize = size;
				ratioXY = size.x / size.y;
			}

			float GetConeHeight(void) const
			{
				return (coneHeight);
			}

			void SetConeHeight(float height)
			{
				coneHeight = height;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	TruncatedConeGeometryObject		Encapsulates data pertaining to a truncated cone primitive.
	//
	//# The $TruncatedConeGeometryObject$ class encapsulates data pertaining to a truncated cone primitive.
	//
	//# \def	class TruncatedConeGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	TruncatedConeGeometryObject(const Vector2D& size, float height, float ratio);
	//
	//# \param	size	The size of the cone base.
	//# \param	height	The height of the cone.
	//# \param	ratio	The ratio of the upper size to the base size.
	//
	//# \desc
	//# The $TruncatedConeGeometryObject$ class encapsulates data pertaining to a truncated cone primitive. In its local coordinate space,
	//# a truncated cone primitive has the center of its base at the origin, and the components of the $size$ parameter define the
	//# semiaxis lengths of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the distance
	//# in the positive <i>z</i> direction between the base of the cone and the plane parallel to the base at which the cone is truncated.
	//# The $ratio$ parameter defines the ratio between the size of the top face and the bottom face (the base) of the truncated cone
	//# and should be in the range (0,1].
	//
	//# \base	PrimitiveGeometryObject		A $TruncatedConeGeometryObject$ is an object that can be shared by multiple truncated cone geometry nodes.
	//
	//# \also	$@TruncatedConeGeometry@$


	//# \function	TruncatedConeGeometryObject::GetConeSize		Returns the truncated cone base size.
	//
	//# \proto	const Vector2D& GetConeSize(void) const;
	//
	//# \desc
	//# The $GetConeSize$ function returns the size of the base of a truncated cone primitive. The components of the return value
	//# correspond to the semiaxis lengths of the base of the cone in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@TruncatedConeGeometryObject::SetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::GetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::SetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::GetConeRatio@$
	//# \also	$@TruncatedConeGeometryObject::SetConeRatio@$


	//# \function	TruncatedConeGeometryObject::SetConeSize		Sets the truncated cone base size.
	//
	//# \proto	void SetConeSize(const Vector2D& size);
	//
	//# \param	size	The new cone base size.
	//
	//# \desc
	//# The $SetConeSize$ function sets the size of the base of a truncated cone primitive. The components of the $size$ parameter
	//# define the semiaxis lengths of the base of the cone in the positive <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@TruncatedConeGeometryObject::GetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::GetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::SetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::GetConeRatio@$
	//# \also	$@TruncatedConeGeometryObject::SetConeRatio@$


	//# \function	TruncatedConeGeometryObject::GetConeHeight		Returns the truncated cone height.
	//
	//# \proto	float GetConeHeight(void) const;
	//
	//# \desc
	//# The $GetConeHeight$ function returns the height of a truncated cone primitive. The return value corresponds to
	//# the distance in the positive <i>z</i> direction between the base of the cone and the plane parallel to the base
	//# at which the cone is truncated.
	//
	//# \also	$@TruncatedConeGeometryObject::SetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::GetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::SetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::GetConeRatio@$
	//# \also	$@TruncatedConeGeometryObject::SetConeRatio@$


	//# \function	TruncatedConeGeometryObject::SetConeHeight		Sets the truncated cone height.
	//
	//# \proto	void SetConeHeight(float height);
	//
	//# \param	height	The new cone height.
	//
	//# \desc
	//# The $SetConeHeight$ function sets the height of a cone primitive. The $height$ parameter defines
	//# the distance in the positive <i>z</i> direction between the base of the cone and the plane parallel to the base
	//# at which the cone is truncated.
	//
	//# \also	$@TruncatedConeGeometryObject::GetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::GetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::SetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::GetConeRatio@$
	//# \also	$@TruncatedConeGeometryObject::SetConeRatio@$


	//# \function	TruncatedConeGeometryObject::GetConeRatio		Returns the ratio of the upper size to the base size of a truncated cone.
	//
	//# \proto	float GetConeRatio(void) const;
	//
	//# \desc
	//# The $GetConeRatio$ function returns the ratio between the dimensions of the top face and the dimensions of the bottom face (the base) of a truncated cone.
	//
	//# \also	$@TruncatedConeGeometryObject::SetConeRatio@$
	//# \also	$@TruncatedConeGeometryObject::GetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::SetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::GetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::SetConeHeight@$


	//# \function	TruncatedConeGeometryObject::SetConeRatio		Sets the ratio of the upper size to the base size of a truncated cone.
	//
	//# \proto	void SetConeRatio(float ratio);
	//
	//# \param	ratio	The ratio of the upper size to the base size.
	//
	//# \desc
	//# The $SetConeRatio$ function sets the ratio between the dimensions of the top face and the dimensions of the bottom face (the base) of a truncated cone.
	//
	//# \also	$@TruncatedConeGeometryObject::GetConeRatio@$
	//# \also	$@TruncatedConeGeometryObject::GetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::SetConeSize@$
	//# \also	$@TruncatedConeGeometryObject::GetConeHeight@$
	//# \also	$@TruncatedConeGeometryObject::SetConeHeight@$


	class TruncatedConeGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		coneSize;
			float			coneHeight;
			float			coneRatio;
			float			ratioXY;

			SurfaceData		staticSurfaceData[3];

			TruncatedConeGeometryObject();
			~TruncatedConeGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			TruncatedConeGeometryObject(const Vector2D& size, float height, float ratio);

			const Vector2D& GetConeSize(void) const
			{
				return (coneSize);
			}

			void SetConeSize(const Vector2D& size)
			{
				coneSize = size;
				ratioXY = size.x / size.y;
			}

			float GetConeHeight(void) const
			{
				return (coneHeight);
			}

			void SetConeHeight(float height)
			{
				coneHeight = height;
			}

			float GetConeRatio(void) const
			{
				return (coneRatio);
			}

			void SetConeRatio(float ratio)
			{
				coneRatio = ratio;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	SphereGeometryObject	Encapsulates data pertaining to a sphere primitive.
	//
	//# The $SphereGeometryObject$ class encapsulates data pertaining to a sphere primitive.
	//
	//# \def	class SphereGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	SphereGeometryObject(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereGeometryObject$ class encapsulates data pertaining to a sphere primitive. In its local coordinate space,
	//# a sphere primitive has its center at the origin. The components of the $size$ parameter define the semiaxis lengths
	//# in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \base	PrimitiveGeometryObject		A $SphereGeometryObject$ is an object that can be shared by multiple sphere geometry nodes.
	//
	//# \also	$@SphereGeometry@$


	//# \function	SphereGeometryObject::GetSphereSize		Returns the sphere size.
	//
	//# \proto	const Vector3D& GetSphereSize(void) const;
	//
	//# \desc
	//# The $GetSphereSize$ function returns the size of a sphere primitive. The components of the return value
	//# correspond to the semiaxis lengths in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \also	$@SphereGeometryObject::SetSphereSize@$


	//# \function	SphereGeometryObject::SetSphereSize		Sets the sphere size.
	//
	//# \proto	void SetSphereSize(const Vector3D& size);
	//
	//# \param	size	The new sphere size.
	//
	//# \desc
	//# The $SetSphereSize$ function sets the size of a sphere primitive. The components of the $size$ parameter
	//# define the semiaxis lengths in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \also	$@SphereGeometryObject::GetSphereSize@$


	class SphereGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector3D		sphereSize;
			float			ratioXY;
			float			ratioXZ;

			SurfaceData		staticSurfaceData[1];

			SphereGeometryObject();
			~SphereGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			SphereGeometryObject(const Vector3D& size);

			const Vector3D& GetSphereSize(void) const
			{
				return (sphereSize);
			}

			void SetSphereSize(const Vector3D& size)
			{
				sphereSize = size;
				ratioXY = size.x / size.y;
				ratioXZ = size.x / size.z;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			float GetRatioXZ(void) const
			{
				return (ratioXZ);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			Point3D GetInitialPrimitiveSupportPoint(void) const override;
			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	DomeGeometryObject	Encapsulates data pertaining to a dome primitive.
	//
	//# The $DomeGeometryObject$ class encapsulates data pertaining to a dome primitive.
	//
	//# \def	class DomeGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	DomeGeometryObject(const Vector3D& size);
	//
	//# \param	size	The size of the dome.
	//
	//# \desc
	//# The $DomeGeometryObject$ class encapsulates data pertaining to a dome primitive. In its local coordinate space,
	//# a dome primitive has the center of its base at the origin. The components of the $size$ parameter define the semiaxis lengths
	//# in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \base	PrimitiveGeometryObject		A $DomeGeometryObject$ is an object that can be shared by multiple dome geometry nodes.
	//
	//# \also	$@DomeGeometry@$


	//# \function	DomeGeometryObject::GetDomeSize		Returns the dome size.
	//
	//# \proto	const Vector3D& GetDomeSize(void) const;
	//
	//# \desc
	//# The $GetDomeSize$ function returns the size of a dome primitive. The components of the return value
	//# correspond to the semiaxis lengths in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \also	$@DomeGeometryObject::SetDomeSize@$


	//# \function	DomeGeometryObject::SetDomeSize		Sets the dome size.
	//
	//# \proto	void SetDomeSize(const Vector3D& size);
	//
	//# \param	size	The new dome size.
	//
	//# \desc
	//# The $SetDomeSize$ function sets the size of a dome primitive. The components of the $size$ parameter
	//# define the semiaxis lengths in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//
	//# \also	$@DomeGeometryObject::GetDomeSize@$


	class DomeGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector3D		domeSize;
			float			ratioXY;
			float			ratioXZ;

			SurfaceData		staticSurfaceData[2];

			DomeGeometryObject();
			~DomeGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			DomeGeometryObject(const Vector3D& size);

			const Vector3D& GetDomeSize(void) const
			{
				return (domeSize);
			}

			void SetDomeSize(const Vector3D& size)
			{
				domeSize = size;
				ratioXY = size.x / size.y;
				ratioXZ = size.x / size.z;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			float GetRatioXZ(void) const
			{
				return (ratioXZ);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			Point3D CalculatePrimitiveSupportPoint(const Vector3D& direction) const override;
			void CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	TorusGeometryObject		Encapsulates data pertaining to a torus primitive.
	//
	//# The $TorusGeometryObject$ class encapsulates data pertaining to a torus primitive.
	//
	//# \def	class TorusGeometryObject final : public PrimitiveGeometryObject
	//
	//# \ctor	TorusGeometryObject(const Vector2D& primary, float secondary);
	//
	//# \param	primary		The primary size of the torus.
	//# \param	secondary	The secondary radius of the torus.
	//
	//# \desc
	//# The $TorusGeometryObject$ class encapsulates data pertaining to a torus primitive. In its local coordinate space,
	//# a torus primitive has its center at the origin. The components of the $primary$ parameter define the primary semiaxis
	//# lengths of the torus in the <i>x</i>-<i>y</i> plane, and the $secondary$ parameter defines the secondary radius of the
	//# torus about the primary ring.
	//
	//# \base	PrimitiveGeometryObject		A $TorusGeometryObject$ is an object that can be shared by multiple torus geometry nodes.
	//
	//# \also	$@TorusGeometry@$


	//# \function	TorusGeometryObject::GetPrimarySize		Returns the primary size of a torus.
	//
	//# \proto	const Vector2D& GetPrimarySize(void) const;
	//
	//# \desc
	//# The $GetPrimarySize$ function returns the primary size of a torus primitive. The components of the return value
	//# correspond to the semiaxis lengths in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@TorusGeometryObject::SetPrimarySize@$
	//# \also	$@TorusGeometryObject::GetSecondaryRadius@$
	//# \also	$@TorusGeometryObject::SetSecondaryRadius@$


	//# \function	TorusGeometryObject::SetPrimarySize		Sets the primary size of a torus.
	//
	//# \proto	void SetPrimarySize(const Vector2D& primary);
	//
	//# \param	primary		The new primary size of the torus.
	//
	//# \desc
	//# The $SetPrimarySize$ function sets the primary size of a torus primitive. The components of the $size$ parameter
	//# define the semiaxis lengths in the <i>x</i> and <i>y</i> directions.
	//
	//# \also	$@TorusGeometryObject::GetPrimarySize@$
	//# \also	$@TorusGeometryObject::GetSecondaryRadius@$
	//# \also	$@TorusGeometryObject::SetSecondaryRadius@$


	//# \function	TorusGeometryObject::GetSecondaryRadius		Returns the secondary radius of a torus.
	//
	//# \proto	float GetSecondaryRadius(void) const;
	//
	//# \desc
	//# The $GetSecondaryRadius$ function returns the secondary radius of a torus primitive. The return value
	//# corresponds to the radius of the torus about the primary ring.
	//
	//# \also	$@TorusGeometryObject::SetSecondaryRadius@$
	//# \also	$@TorusGeometryObject::GetPrimarySize@$
	//# \also	$@TorusGeometryObject::SetPrimarySize@$


	//# \function	TorusGeometryObject::SetSecondaryRadius		Sets the secondary radius of a torus.
	//
	//# \proto	void SetSecondaryRadius(float secondary);
	//
	//# \param	secondary	The new secondary radius of the torus.
	//
	//# \desc
	//# The $SetSecondaryRadius$ function sets the secondary radius of a torus primitive. The $secondary$ parameter
	//# defines the radius of the torus about the primary ring.
	//
	//# \also	$@TorusGeometryObject::GetSecondaryRadius@$
	//# \also	$@TorusGeometryObject::GetPrimarySize@$
	//# \also	$@TorusGeometryObject::SetPrimarySize@$


	class TorusGeometryObject final : public PrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		primarySize;
			float			secondaryRadius;
			float			primaryRatioXY;

			SurfaceData		staticSurfaceData[1];

			TorusGeometryObject();
			~TorusGeometryObject();

			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			TorusGeometryObject(const Vector2D& primary, float secondary);

			const Vector2D& GetPrimarySize(void) const
			{
				return (primarySize);
			}

			void SetPrimarySize(const Vector2D& primary)
			{
				primarySize = primary;
				primaryRatioXY = primary.x / primary.y;
			}

			float GetSecondaryRadius(void) const
			{
				return (secondaryRadius);
			}

			void SetSecondaryRadius(float seconadry)
			{
				secondaryRadius = seconadry;
			}

			float GetPrimaryRatioXY(void) const
			{
				return (primaryRatioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;
	};


	//# \class	PathPrimitiveGeometryObject		Encapsulates data pertaining to primitive geometries that use paths.
	//
	//# The $PathPrimitiveGeometryObject$ class encapsulates data pertaining to primitive geometries that use paths.
	//
	//# \def	class PathPrimitiveGeometryObject : public PrimitiveGeometryObject
	//
	//# \ctor	PathPrimitiveGeometryObject(PrimitiveType type, const Path *path);
	//
	//# The constructor has protected access. The $PathPrimitiveGeometryObject$ class can only exist as a base class for other primitive geometry objects.
	//
	//# \param	type		The primitive geometry type.
	//# \param	path		The path that the primitive geometry uses.
	//
	//# \desc
	//
	//# \base	PrimitiveGeometryObject		A $PathPrimitiveGeometryObject$ is an object that can be shared by multiple geometry nodes.
	//
	//# \also	$@PathPrimitiveGeometry@$


	class PathPrimitiveGeometryObject : public PrimitiveGeometryObject
	{
		protected:

			Path		primitivePath;
			Box3D		pathBoundingBox;
			Box3D		primitiveBoundingBox;

			float		texcoordScale;

			PathPrimitiveGeometryObject(PrimitiveType type);
			PathPrimitiveGeometryObject(PrimitiveType type, const Path *path);
			~PathPrimitiveGeometryObject();

			virtual void InitPathSurfaces(void) = 0;

		public:

			Path *GetPrimitivePath(void)
			{
				return (&primitivePath);
			}

			const Path *GetPrimitivePath(void) const
			{
				return (&primitivePath);
			}

			const Box3D& GetPathBoundingBox(void) const
			{
				return (pathBoundingBox);
			}

			const Box3D& GetPrimitiveBoundingBox(void) const
			{
				return (primitiveBoundingBox);
			}

			float GetTexcoordScale(void) const
			{
				return (texcoordScale);
			}

			void SetTexcoordScale(float scale)
			{
				texcoordScale = scale;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;
			void EndSettingsUnpack(void *cookie) override;

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const override;

			C4API void SetPrimitivePath(const Path *path);
	};


	//# \class	TubeGeometryObject		Encapsulates data pertaining to a path tube primitive.
	//
	//# The $TubeGeometryObject$ class encapsulates data pertaining to a path tube primitive.
	//
	//# \def	class TubeGeometryObject final : public PathPrimitiveGeometryObject
	//
	//# \ctor	TubeGeometryObject(const Path *path, const Vector2D& size);
	//
	//# \param	path		The path along which the tube is created.
	//# \param	size		The size of the cross section of the tube.
	//
	//# \desc
	//
	//# \base	PathPrimitiveGeometryObject		A $TubeGeometryObject$ is an object that can be shared by multiple path tube geometry nodes.
	//
	//# \also	$@TubeGeometry@$


	class TubeGeometryObject final : public PathPrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		tubeSize;
			float			ratioXY;

			TubeGeometryObject();
			~TubeGeometryObject();

			void InitPathSurfaces(void);
			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			TubeGeometryObject(const Path *path, const Vector2D& size);

			const Vector2D& GetTubeSize(void) const
			{
				return (tubeSize);
			}

			void SetTubeSize(const Vector2D& size)
			{
				tubeSize = size;
				ratioXY = size.x / size.y;
			}

			float GetRatioXY(void) const
			{
				return (ratioXY);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	//# \class	ExtrusionGeometryObject		Encapsulates data pertaining to a path extrusion primitive.
	//
	//# The $ExtrusionGeometryObject$ class encapsulates data pertaining to a path extrusion primitive.
	//
	//# \def	class ExtrusionGeometryObject final : public PathPrimitiveGeometryObject
	//
	//# \ctor	ExtrusionGeometryObject(const Path *path, const Vector2D& size, float height);
	//
	//# \param	path		The path making up the base of the extrusion.
	//# \param	size		The base size of the extrusion.
	//# \param	height		The height of the extrusion.
	//
	//# \desc
	//
	//# \base	PathPrimitiveGeometryObject		An $ExtrusionGeometryObject$ is an object that can be shared by multiple path extrusion geometry nodes.
	//
	//# \also	$@ExtrusionGeometry@$


	class ExtrusionGeometryObject final : public PathPrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		extrusionSize;
			float			extrusionHeight;

			ExtrusionGeometryObject();
			~ExtrusionGeometryObject();

			void InitPathSurfaces(void);
			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			ExtrusionGeometryObject(const Path *path, const Vector2D& size, float height);

			const Vector2D& GetExtrusionSize(void) const
			{
				return (extrusionSize);
			}

			void SetExtrusionSize(const Vector2D& size)
			{
				extrusionSize = size;
			}

			float GetExtrusionHeight(void) const
			{
				return (extrusionHeight);
			}

			void SetExtrusionHeight(float height)
			{
				extrusionHeight = height;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	//# \class	RevolutionGeometryObject		Encapsulates data pertaining to a path revolution primitive.
	//
	//# The $RevolutionGeometryObject$ class encapsulates data pertaining to a path revolution primitive.
	//
	//# \def	class RevolutionGeometryObject final : public PathPrimitiveGeometryObject
	//
	//# \ctor	RevolutionGeometryObject(const Path *path, const Vector2D& size, float height);
	//
	//# \param	path		The path defining the lateral shape of the revolution.
	//# \param	size		The size of the base of the revolution.
	//# \param	height		The height of the revolution.
	//
	//# \desc
	//
	//# \base	PathPrimitiveGeometryObject		A $RevolutionGeometryObject$ is an object that can be shared by multiple path revolution geometry nodes.
	//
	//# \also	$@RevolutionGeometry@$


	class RevolutionGeometryObject final : public PathPrimitiveGeometryObject
	{
		friend class PrimitiveGeometryObject;

		private:

			Vector2D		revolutionSize;
			float			revolutionHeight;
			float			ratioXY;

			RevolutionGeometryObject();
			~RevolutionGeometryObject();

			void InitPathSurfaces(void);
			void BuildPrimitiveLevel(int32 level, const Transformable *transformable) override;

		public:

			RevolutionGeometryObject(const Path *path, const Vector2D& size, float height);

			const Vector2D& GetRevolutionSize(void) const
			{
				return (revolutionSize);
			}

			void SetRevolutionSize(const Vector2D& size)
			{
				revolutionSize = size;
				ratioXY = size.x / size.y;
			}

			float GetRevolutionHeight(void) const
			{
				return (revolutionHeight);
			}

			void SetRevolutionHeight(float height)
			{
				revolutionHeight = height;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	//# \class	PrimitiveGeometry		Represents a primitive geometry node in a world.
	//
	//# The $PrimitiveGeometry$ class represents a primitive geometry node in a world.
	//
	//# \def	class PrimitiveGeometry : public Geometry
	//
	//# \ctor	PrimitiveGeometry(PrimitiveType type);
	//
	//# The constructor has protected access. The $PrimitiveGeometry$ class can only exist as the base class for another class.
	//
	//# \param	type	The type of the primitive geometry. See below for a list of possible types.
	//
	//# \desc
	//# The $PrimitiveGeometry$ class is the base class for all primitive geometry nodes. The $type$ parameter
	//# can be one of the following constants, and it is specified by the subclass's constructor.
	//
	//# \table	PrimitiveType
	//
	//# \base	Geometry	A primitive geometry is a specific type of geometry.
	//
	//# \also	$@PrimitiveGeometryObject@$


	//# \function	PrimitiveGeometry::GetPrimitiveType		Returns the specific type of a primitive geometry.
	//
	//# \proto	PrimitiveType GetPrimitiveType(void) const;
	//
	//# \desc
	//# The $GetPrimitiveType$ function returns the specific primitive geometry type, which can be one of the following constants.
	//
	//# \table	PrimitiveType


	class PrimitiveGeometry : public Geometry
	{
		friend class Geometry;

		private:

			PrimitiveType		primitiveType;

			static PrimitiveGeometry *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API PrimitiveGeometry(PrimitiveType type);
			C4API PrimitiveGeometry(const PrimitiveGeometry& primitive);

		public:

			C4API ~PrimitiveGeometry();

			PrimitiveType GetPrimitiveType(void) const
			{
				return (primitiveType);
			}

			PrimitiveGeometryObject *GetObject(void) const
			{
				return (static_cast<PrimitiveGeometryObject *>(Node::GetObject()));
			}

			C4API void PackType(Packer& data) const override;

			C4API virtual bool PathPrimitive(void) const;
	};


	//# \class	PlateGeometry		Represents a plate primitive node in a world.
	//
	//# The $PlateGeometry$ class represents a plate primitive node in a world.
	//
	//# \def	class PlateGeometry final : public PrimitiveGeometry
	//
	//# \ctor	PlateGeometry(const Vector2D& size);
	//
	//# \param	size	The size of the plate.
	//
	//# \desc
	//# The $PlateGeometry$ class represents a plate primitive node in a world. In its local coordinate space,
	//# a plate primitive lies in the <i>x</i>-<i>y</i> plane with one of its corners at the origin. The components of
	//# the $size$ parameter define the extents of the plate in the positive <i>x</i> and <i>y</i> directions.
	//#
	//# When a new plate geometry is created, the corresponding $@PlateGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A plate is a specific type of primitive geometry.
	//
	//# \also	$@PlateGeometryObject@$


	class PlateGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			PlateGeometry();
			PlateGeometry(const PlateGeometry& plateGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API PlateGeometry(const Vector2D& size);
			C4API ~PlateGeometry();

			PlateGeometryObject *GetObject(void) const
			{
				return (static_cast<PlateGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	DiskGeometry		Represents a disk primitive node in a world.
	//
	//# The $DiskGeometry$ class represents a disk primitive node in a world.
	//
	//# \def	class DiskGeometry final : public PrimitiveGeometry
	//
	//# \ctor	DiskGeometry(const Vector2D& size);
	//
	//# \param	size	The size of the disk.
	//
	//# \desc
	//# The $DiskGeometry$ class represents a disk primitive node in a world. In its local coordinate space,
	//# a disk primitive lies in the <i>x</i>-<i>y</i> plane with its center at the origin. The components of
	//# the $size$ parameter define the semiaxis lengths in the <i>x</i> and <i>y</i> directions.
	//#
	//# When a new disk geometry is created, the corresponding $@DiskGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A disk is a specific type of primitive geometry.
	//
	//# \also	$@DiskGeometryObject@$


	class DiskGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			DiskGeometry();
			DiskGeometry(const DiskGeometry& diskGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API DiskGeometry(const Vector2D& size);
			C4API ~DiskGeometry();

			DiskGeometryObject *GetObject(void) const
			{
				return (static_cast<DiskGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	HoleGeometry		Represents a hole primitive node in a world.
	//
	//# The $HoleGeometry$ class represents a hole primitive node in a world.
	//
	//# \def	class HoleGeometry final : public PrimitiveGeometry
	//
	//# \ctor	HoleGeometry(const Vector2D& inner, const Vector2D& outer);
	//
	//# \param	inner		The size of the inner hole.
	//# \param	outer		The size of the outer plate.
	//
	//# \desc
	//# The $HoleGeometry$ class represents a hole primitive node in a world. In its local coordinate space,
	//# a hole primitive lies in the <i>x</i>-<i>y</i> plane with the center of the hole at the origin. The components of
	//# the $inner$ parameter define the semiaxis lengths of the hole in the <i>x</i> and <i>y</i> directions, and the components of
	//# the $outer$ parameter define the extents of the plate in the positive and negative <i>x</i> and <i>y</i> directions.
	//#
	//# When a new hole geometry is created, the corresponding $@HoleGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A hole is a specific type of primitive geometry.
	//
	//# \also	$@HoleGeometryObject@$


	class HoleGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			HoleGeometry();
			HoleGeometry(const HoleGeometry& holeGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API HoleGeometry(const Vector2D& inner, const Vector2D& outer);
			C4API ~HoleGeometry();

			HoleGeometryObject *GetObject(void) const
			{
				return (static_cast<HoleGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	AnnulusGeometry		Represents an annulus primitive node in a world.
	//
	//# The $AnnulusGeometry$ class represents an annulus primitive node in a world.
	//
	//# \def	class AnnulusGeometry final : public PrimitiveGeometry
	//
	//# \ctor	AnnulusGeometry(const Vector2D& inner, const Vector2D& outer);
	//
	//# \param	inner		The size of the inner hole.
	//# \param	outer		The size of the outer disk.
	//
	//# \desc
	//# The $AnnulusGeometry$ class represents an annulus primitive node in a world. In its local coordinate space,
	//# an annulus primitive lies in the <i>x</i>-<i>y</i> plane with the center at the origin. The components of
	//# the $inner$ parameter define the semiaxis lengths of the inner hole in the <i>x</i> and <i>y</i> directions, and the components of
	//# the $outer$ parameter define the semiaxis lengths of the outer disk in the <i>x</i> and <i>y</i> directions.
	//#
	//# When a new annulus geometry is created, the corresponding $@AnnulusGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		An annulus is a specific type of primitive geometry.
	//
	//# \also	$@AnnulusGeometryObject@$


	class AnnulusGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			AnnulusGeometry();
			AnnulusGeometry(const AnnulusGeometry& annulusGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API AnnulusGeometry(const Vector2D& inner, const Vector2D& outer);
			C4API ~AnnulusGeometry();

			AnnulusGeometryObject *GetObject(void) const
			{
				return (static_cast<AnnulusGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	BoxGeometry		Represents a box primitive node in a world.
	//
	//# The $BoxGeometry$ class represents a box primitive node in a world.
	//
	//# \def	class BoxGeometry final : public PrimitiveGeometry
	//
	//# \ctor	BoxGeometry(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxGeometry$ class represents a box primitive node in a world. In its local coordinate space,
	//# a box primitive has one of its corners at the origin, and the components of the $size$ parameter define the extents
	//# of the box in the positive <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//#
	//# When a new box geometry is created, the corresponding $@BoxGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A box is a specific type of primitive geometry.
	//
	//# \also	$@BoxGeometryObject@$


	class BoxGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			BoxGeometry();
			BoxGeometry(const BoxGeometry& boxGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API BoxGeometry(const Vector3D& size);
			C4API ~BoxGeometry();

			BoxGeometryObject *GetObject(void) const
			{
				return (static_cast<BoxGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	PyramidGeometry		Represents a pyramid primitive node in a world.
	//
	//# The $PyramidGeometry$ class represents a pyramid primitive node in a world.
	//
	//# \def	class PyramidGeometry final : public PrimitiveGeometry
	//
	//# \ctor	PyramidGeometry(const Vector2D& size, float height);
	//
	//# \param	size		The size of the pyramid base.
	//# \param	height		The height of the pyramid.
	//
	//# \desc
	//# The $PyramidGeometry$ class represents a pyramid primitive node in a world. In its local coordinate space,
	//# a pyramid primitive has one of the corners of its base at the origin, and the components of the $size$ parameter define
	//# the extents of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the height of
	//# the pyramid's apex above the base in the positive <i>z</i> direction.
	//#
	//# When a new pyramid geometry is created, the corresponding $@PyramidGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A pyramid is a specific type of primitive geometry.
	//
	//# \also	$@PyramidGeometryObject@$


	class PyramidGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			PyramidGeometry();
			PyramidGeometry(const PyramidGeometry& pyramidGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API PyramidGeometry(const Vector2D& size, float height);
			C4API ~PyramidGeometry();

			PyramidGeometryObject *GetObject(void) const
			{
				return (static_cast<PyramidGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	CylinderGeometry	Represents a cylinder primitive node in a world.
	//
	//# The $CylinderGeometry$ class represents a cylinder primitive node in a world.
	//
	//# \def	class CylinderGeometry final : public PrimitiveGeometry
	//
	//# \ctor	CylinderGeometry(const Vector2D& size, float height);
	//
	//# \param	size		The size of the cylinder base.
	//# \param	height		The height of the cylinder.
	//
	//# \desc
	//# The $CylinderGeometry$ class represents a cylinder primitive node in a world. In its local coordinate space,
	//# a cylinder primitive has the center of its base at the origin, and the components of the $size$ parameter define the
	//# semiaxis lengths of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the height
	//# of the cylinder above the base in the positive <i>z</i> direction.
	//#
	//# When a new cylinder geometry is created, the corresponding $@CylinderGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A cylinder is a specific type of primitive geometry.
	//
	//# \also	$@CylinderGeometryObject@$


	class CylinderGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			CylinderGeometry();
			CylinderGeometry(const CylinderGeometry& cylinderGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API CylinderGeometry(const Vector2D& size, float height);
			C4API ~CylinderGeometry();

			CylinderGeometryObject *GetObject(void) const
			{
				return (static_cast<CylinderGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	ConeGeometry	Represents a cone primitive node in a world.
	//
	//# The $ConeGeometry$ class represents a cone primitive node in a world.
	//
	//# \def	class ConeGeometry final : public PrimitiveGeometry
	//
	//# \ctor	ConeGeometry(const Vector2D& size, float height);
	//
	//# \param	size		The size of the cone base.
	//# \param	height		The height of the cone.
	//
	//# \desc
	//# The $ConeGeometry$ class represents a cone primitive node in a world. In its local coordinate space,
	//# a cone primitive has the center of its base at the origin, and the components of the $size$ parameter define the
	//# semiaxis lengths of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the height
	//# of the cone's apex above the base in the positive <i>z</i> direction.
	//#
	//# When a new cone geometry is created, the corresponding $@ConeGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A cone is a specific type of primitive geometry.
	//
	//# \also	$@ConeGeometryObject@$


	class ConeGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			ConeGeometry();
			ConeGeometry(const ConeGeometry& coneGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API ConeGeometry(const Vector2D& size, float height);
			C4API ~ConeGeometry();

			ConeGeometryObject *GetObject(void) const
			{
				return (static_cast<ConeGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	TruncatedConeGeometry	Represents a truncated cone primitive node in a world.
	//
	//# The $TruncatedConeGeometry$ class represents a truncated cone primitive node in a world.
	//
	//# \def	class TruncatedConeGeometry final : public PrimitiveGeometry
	//
	//# \ctor	TruncatedConeGeometry(const Vector2D& size, float height, float ratio);
	//
	//# \param	size		The size of the cone base.
	//# \param	height		The height of the cone.
	//# \param	ratio		The ratio of the upper size to the base size.
	//
	//# \desc
	//# The $TruncatedConeGeometry$ class represents a truncated cone primitive node in a world. In its local coordinate space,
	//# a truncated cone primitive has the center of its base at the origin, and the components of the $size$ parameter define the
	//# semiaxis lengths of the base in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the distance
	//# in the positive <i>z</i> direction between the base of the cone and the plane parallel to the base at which the cone is truncated.
	//# The $ratio$ parameter defines the ratio between the size of the top face and the bottom face (the base) of the truncated cone
	//# and should be in the range (0,1].
	//#
	//# When a new truncated cone geometry is created, the corresponding $@TruncatedConeGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A truncated cone is a specific type of primitive geometry.
	//
	//# \also	$@TruncatedConeGeometryObject@$


	class TruncatedConeGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			TruncatedConeGeometry();
			TruncatedConeGeometry(const TruncatedConeGeometry& truncatedConeGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API TruncatedConeGeometry(const Vector2D& size, float height, float ratio);
			C4API ~TruncatedConeGeometry();

			TruncatedConeGeometryObject *GetObject(void) const
			{
				return (static_cast<TruncatedConeGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	SphereGeometry		Represents a sphere primitive node in a world.
	//
	//# The $SphereGeometry$ class represents a sphere primitive node in a world.
	//
	//# \def	class SphereGeometry final : public PrimitiveGeometry
	//
	//# \ctor	SphereGeometry(const Vector3D& size);
	//
	//# \param	size		The size of the sphere.
	//
	//# \desc
	//# The $SphereGeometry$ class represents a sphere primitive node in a world. In its local coordinate space,
	//# a sphere primitive has its center at the origin. The components of the $size$ parameter define the semiaxis lengths
	//# in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//#
	//# When a new sphere geometry is created, the corresponding $@SphereGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A sphere is a specific type of primitive geometry.
	//
	//# \also	$@SphereGeometryObject@$


	class SphereGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			SphereGeometry();
			SphereGeometry(const SphereGeometry& sphereGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API SphereGeometry(const Vector3D& size);
			C4API ~SphereGeometry();

			SphereGeometryObject *GetObject(void) const
			{
				return (static_cast<SphereGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	DomeGeometry		Represents a dome primitive node in a world.
	//
	//# The $DomeGeometry$ class represents a dome primitive node in a world.
	//
	//# \def	class DomeGeometry final : public PrimitiveGeometry
	//
	//# \ctor	DomeGeometry(const Vector3D& size);
	//
	//# \param	size		The size of the dome.
	//
	//# \desc
	//# The $DomeGeometry$ class represents a dome primitive node in a world. In its local coordinate space,
	//# a dome primitive has the center of its base at the origin. The components of the $size$ parameter define the semiaxis lengths
	//# in the <i>x</i>, <i>y</i>, and <i>z</i> directions.
	//#
	//# When a new dome geometry is created, the corresponding $@DomeGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A dome is a specific type of primitive geometry.
	//
	//# \also	$@DomeGeometryObject@$


	class DomeGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			DomeGeometry();
			DomeGeometry(const DomeGeometry& domeGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API DomeGeometry(const Vector3D& size);
			C4API ~DomeGeometry();

			DomeGeometryObject *GetObject(void) const
			{
				return (static_cast<DomeGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	TorusGeometry		Represents a torus primitive node in a world.
	//
	//# The $TorusGeometry$ class represents a torus primitive node in a world.
	//
	//# \def	class TorusGeometry final : public PrimitiveGeometry
	//
	//# \ctor	TorusGeometry(const Vector2D& primary, float secondary);
	//
	//# \param	primary		The primary size of the torus.
	//# \param	secondary	The secondary radius of the torus.
	//
	//# \desc
	//# The $TorusGeometry$ class represents a torus primitive node in a world. In its local coordinate space,
	//# a torus primitive has its center at the origin. The components of the $primary$ parameter define the primary semiaxis
	//# lengths of the torus in the <i>x</i>-<i>y</i> plane, and the $secondary$ parameter defines the secondary radius of the
	//# torus about the primary ring.
	//#
	//# When a new torus geometry is created, the corresponding $@TorusGeometryObject@$ is automatically created and referenced
	//# by the geometry node.
	//
	//# \base	PrimitiveGeometry		A torus is a specific type of primitive geometry.
	//
	//# \also	$@TorusGeometryObject@$


	class TorusGeometry final : public PrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			TorusGeometry();
			TorusGeometry(const TorusGeometry& torusGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API TorusGeometry(const Vector2D& primary, float secondary);
			C4API ~TorusGeometry();

			TorusGeometryObject *GetObject(void) const
			{
				return (static_cast<TorusGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	PathPrimitiveGeometry		Represents a path primitive node in a world.
	//
	//# The $PathPrimitiveGeometry$ class represents a path primitive node in a world.
	//
	//# \def	class PathPrimitiveGeometry : public PrimitiveGeometry
	//
	//# \ctor	PathPrimitiveGeometry(PrimitiveType type);
	//
	//# The constructor has protected access. The $PathPrimitiveGeometry$ class can only exist as a base class for other primitive geometry nodes.
	//
	//# \param	type		The primitive geometry type.
	//
	//# \desc
	//
	//# \base	PrimitiveGeometry		A path primitive is a specific type of primitive geometry.
	//
	//# \also	$@PathPrimitiveGeometryObject@$


	class PathPrimitiveGeometry : public PrimitiveGeometry
	{
		private:

			bool CalculateBoundingBox(Box3D *box) const override;

		protected:

			PathPrimitiveGeometry(PrimitiveType type);
			PathPrimitiveGeometry(const PathPrimitiveGeometry& pathPrimitiveGeometry);

		public:

			~PathPrimitiveGeometry();

			PathPrimitiveGeometryObject *GetObject(void) const
			{
				return (static_cast<PathPrimitiveGeometryObject *>(Node::GetObject()));
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			bool PathPrimitive(void) const;

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API PathMarker *GetConnectedPathMarker(void) const;
			C4API void SetConnectedPathMarker(PathMarker *marker);
	};


	//# \class	TubeGeometry		Represents a path tube primitive node in a world.
	//
	//# The $TubeGeometry$ class represents a path tube primitive node in a world.
	//
	//# \def	class TubeGeometry final : public PathPrimitiveGeometry
	//
	//# \ctor	TubeGeometry(const Path *path, const Vector2D& size);
	//
	//# \param	path		The path along which the tube is created.
	//# \param	size		The size of the cross section of the tube.
	//
	//# \desc
	//
	//# \base	PathPrimitiveGeometry		A path tube is a specific type of path primitive geometry.
	//
	//# \also	$@TubeGeometryObject@$


	class TubeGeometry final : public PathPrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			TubeGeometry();
			TubeGeometry(const TubeGeometry& tubeGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API TubeGeometry(const Path *path, const Vector2D& size);
			C4API ~TubeGeometry();

			TubeGeometryObject *GetObject(void) const
			{
				return (static_cast<TubeGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	ExtrusionGeometry		Represents a path extrusion primitive node in a world.
	//
	//# The $ExtrusionGeometry$ class represents a path extrusion primitive node in a world.
	//
	//# \def	class ExtrusionGeometry final : public PathPrimitiveGeometry
	//
	//# \ctor	ExtrusionGeometry(const Path *path, const Vector2D& size, float height);
	//
	//# \param	path		The path making up the base of the extrusion.
	//# \param	size		The base size of the extrusion.
	//# \param	height		The height of the extrusion.
	//
	//# \desc
	//
	//# \base	PathPrimitiveGeometry		A path extrusion is a specific type of path primitive geometry.
	//
	//# \also	$@ExtrusionGeometryObject@$


	class ExtrusionGeometry final : public PathPrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			ExtrusionGeometry();
			ExtrusionGeometry(const ExtrusionGeometry& extrusionGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API ExtrusionGeometry(const Path *path, const Vector2D& size, float height);
			C4API ~ExtrusionGeometry();

			ExtrusionGeometryObject *GetObject(void) const
			{
				return (static_cast<ExtrusionGeometryObject *>(Node::GetObject()));
			}
	};


	//# \class	RevolutionGeometry		Represents a path revolution primitive node in a world.
	//
	//# The $RevolutionGeometry$ class represents a path revolution primitive node in a world.
	//
	//# \def	class RevolutionGeometry final : public PathPrimitiveGeometry
	//
	//# \ctor	RevolutionGeometry(const Path *path, const Vector2D& size, float height);
	//
	//# \param	path		The path defining the lateral shape of the revolution.
	//# \param	size		The size of the base of the revolution.
	//# \param	height		The height of the revolution.
	//
	//# \desc
	//
	//# \base	PathPrimitiveGeometry		A path revolution is a specific type of path primitive geometry.
	//
	//# \also	$@RevolutionGeometryObject@$


	class RevolutionGeometry final : public PathPrimitiveGeometry
	{
		friend class PrimitiveGeometry;

		private:

			RevolutionGeometry();
			RevolutionGeometry(const RevolutionGeometry& revolutionGeometry);

			Node *Replicate(void) const override;

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API RevolutionGeometry(const Path *path, const Vector2D& size, float height);
			C4API ~RevolutionGeometry();

			RevolutionGeometryObject *GetObject(void) const
			{
				return (static_cast<RevolutionGeometryObject *>(Node::GetObject()));
			}
	};
}


#endif

// ZYUQURM
