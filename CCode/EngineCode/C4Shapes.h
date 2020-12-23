 

#ifndef C4Shapes_h
#define C4Shapes_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Inertia.h"
#include "C4Volumes.h"
#include "C4MaterialObjects.h"
#include "C4Node.h"


namespace C4
{
	typedef Type	ShapeType;


	enum : ObjectType
	{
		kObjectShape					= 'SHAP'
	};


	//# \enum	ShapeType

	enum : ShapeType
	{
		kShapeBox						= 'BOX ',		//## Box shape.
		kShapePyramid					= 'PYRA',		//## Pyramid shape.
		kShapeCylinder					= 'CYLD',		//## Cylinder shape.
		kShapeCone						= 'CONE',		//## Cone shape.
		kShapeSphere					= 'SPHR',		//## Sphere shape.
		kShapeDome						= 'DOME',		//## Dome shape.
		kShapeCapsule					= 'CPSL',		//## Capsule shape.
		kShapeTruncatedPyramid			= 'TPYR',		//## Truncated pyramid shape.
		kShapeTruncatedCone				= 'TCON',		//## Truncated cone shape.
		kShapeTruncatedDome				= 'TDOM'		//## Truncated dome shape.
	};


	//# \enum	ShapeFlags

	enum
	{
		kShapeCollisionInhibit			= 1 << 0		//## The shape affects mass distribution in the rigid body, but it does not participate in collision detection.
	};


	enum
	{
		kMaxIntersectionContactCount	= 8
	};


	class Mesh;
	class PrimitiveGeometry;
	class GeometryObject;
	class GeometryContact;
	class RigidBodyContact;
	struct CollisionOctree;


	struct ShapeMesh
	{
		unsigned_int8			vertexCount;
		unsigned_int8			edgeCount;
		unsigned_int8			polygonCount;

		const unsigned_int8		(*edgeArray)[2];
		const unsigned_int8		*polygonArray;
	};


	struct ShapeHitData
	{
		Point3D				position;
		Vector3D			normal;
		float				param;
	};


	struct IntersectionData
	{
		Point3D				alphaContact;
		Point3D				betaContact;
		Vector3D			contactNormal;
		float				contactParam;
		unsigned_int32		triangleIndex;
	};


	//# \class	ShapeObject		Encapsulates data pertaining to a physics shape.
	//
	//# The $ShapeObject$ class encapsulates data pertaining to a physics shape.
	//
	//# \def	class ShapeObject : public Object, public VolumeObject
	//
	//# \ctor	ShapeObject(ShapeType type, const ShapeMesh *mesh, const Point3D *vertex, Volume *volume);
	// 
	//# The constructor has protected access. The $ShapeObject$ class can only exist as the base class for another class.
	//
	//# \param	type		The type of the shape. See below for a list of possible types. 
	//# \param	volume		A pointer to the generic volume object representing the shape.
	// 
	//# \desc
	//# The $ShapeObject$ class encapsulates data describing a shape used to define the volume of a rigid body.
	//# 
	//# A shape object can be of one of the following types.
	// 
	//# \table	ShapeType 
	//
	//# \base		WorldMgr/Object		A $FieldObject$ is an object that can be shared by multiple field nodes.
	//# \privbase	VolumeObject		Used internally by the engine for generic volume objects.
	// 
	//# \also	$@Shape@$
	//# \also	$@RigidBodyController@$


	//# \function	ShapeObject::GetShapeType		Returns the specific type of a shape.
	//
	//# \proto	ShapeType GetShapeType(void) const;
	//
	//# \desc
	//# The $GetShapeType$ function returns the specific shape type, which can be one of the following constants.
	//
	//# \table	ShapeType


	//# \function	ShapeObject::GetShapeFlags		Returns the shape flags.
	//
	//# \proto	unsigned_int32 GetShapeFlags(void) const;
	//
	//# \desc
	//# The $GetShapeFlags$ function returns the shape flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	ShapeFlags
	//
	//# The initial value of the shape flags is 0.
	//
	//# \also	$@ShapeObject::SetShapeFlags@$


	//# \function	ShapeObject::SetShapeFlags		Sets the shape flags.
	//
	//# \proto	void SetShapeFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new shape flags.
	//
	//# \desc
	//# The $SetShapeFlags$ function sets the shape flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	ShapeFlags
	//
	//# The initial value of the shape flags is 0.
	//
	//# \also	$@ShapeObject::GetShapeFlags@$


	//# \function	ShapeObject::GetShapeDensity	Returns the density of a shape.
	//
	//# \proto	float GetShapeDensity(void) const;
	//
	//# \desc
	//# The $GetShapeDensity$ function returns the density of a shape, measured in kilograms per cubic decimeter (kg/dm<sup>3</sup>),
	//# or equivalently, metric tons per cubic meter (Mg/m<sup>3</sup>).
	//
	//# \also	$@ShapeObject::SetShapeDensity@$


	//# \function	ShapeObject::SetShapeDensity	Sets the density of a shape.
	//
	//# \proto	void SetShapeDensity(float density);
	//
	//# \param	density		The new density.
	//
	//# \desc
	//# The $SetShapeDensity$ function sets the density of a shape to the value specified by the $density$ parameter.
	//# Density is measured in kilograms per cubic decimeter (kg/dm<sup>3</sup>), or equivalently, metric tons per cubic meter (Mg/m<sup>3</sup>).
	//#
	//# By default, a shape has a density of 1.0 kg/dm<sup>3</sup>.
	//
	//# \also	$@ShapeObject::GetShapeDensity@$


	//# \function	ShapeObject::GetShapeSubstance		Returns the substance associated with a shape.
	//
	//# \proto	SubstanceType GetShapeSubstance(void) const;
	//
	//# \desc
	//# The $GetShapeSubstance$ function returns the substance associated with a shape.
	//#
	//# The default substance associated with a shape when it is initially created is $kSubstanceNone$.
	//
	//# \also	$@ShapeObject::SetShapeSubstance@$
	//# \also	$@GraphicsMgr/MaterialObject::GetFirstRegisteredSubstance@$
	//# \also	$@GraphicsMgr/MaterialObject::FindRegisteredSubstance@$
	//# \also	$@GraphicsMgr/MaterialObject::RegisterSubstance@$
	//# \also	$@GraphicsMgr/Substance@$


	//# \function	ShapeObject::SetShapeSubstance		Sets the substance associated with a shape.
	//
	//# \proto	void SetShapeSubstance(SubstanceType substance);
	//
	//# \param	substance	The new shape substance.
	//
	//# \desc
	//# The $SetShapeSubstance$ function sets the substance associated with a shape.
	//#
	//# The default substance associated with a shape when it is initially created is $kSubstanceNone$.
	//
	//# \also	$@ShapeObject::GetShapeSubstance@$
	//# \also	$@GraphicsMgr/MaterialObject::GetFirstRegisteredSubstance@$
	//# \also	$@GraphicsMgr/MaterialObject::FindRegisteredSubstance@$
	//# \also	$@GraphicsMgr/MaterialObject::RegisterSubstance@$
	//# \also	$@GraphicsMgr/Substance@$


	class ShapeObject : public Object, public VolumeObject
	{
		friend class WorldMgr;

		private:

			ShapeType			shapeType;

			unsigned_int32		shapeFlags;
			float				shapeDensity;
			SubstanceType		shapeSubstance;

			float				shapeVolume;
			float				shapeShrinkSize;

			const ShapeMesh		*shapeMesh;
			const Point3D		*shapeVertex;

			static ShapeObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			ShapeObject(ShapeType type, const ShapeMesh *mesh, const Point3D *vertex, Volume *volume);
			~ShapeObject();

			void SetShapeShrinkSize(float size)
			{
				shapeShrinkSize = size;
			}

		public:

			ShapeType GetShapeType(void) const
			{
				return (shapeType);
			}

			unsigned_int32 GetShapeFlags(void) const
			{
				return (shapeFlags);
			}

			void SetShapeFlags(unsigned_int32 flags)
			{
				shapeFlags = flags;
			}

			float GetShapeDensity(void) const
			{
				return (shapeDensity);
			}

			void SetShapeDensity(float density)
			{
				shapeDensity = density;
			}

			SubstanceType GetShapeSubstance(void) const
			{
				return (shapeSubstance);
			}

			void SetShapeSubstance(SubstanceType substance)
			{
				shapeSubstance = substance;
			}

			const float& GetShapeShrinkSize(void) const
			{
				return (shapeShrinkSize);
			}

			const ShapeMesh *GetShapeMesh(void) const
			{
				return (shapeMesh);
			}

			const Point3D *GetShapeVertexArray(void) const
			{
				return (shapeVertex);
			}

			float CalculateMass(void) const
			{
				return (CalculateVolume() * shapeDensity);
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			virtual void Preprocess(void);

			virtual float CalculateVolume(void) const = 0;
			virtual Point3D CalculateCenterOfMass(void) const = 0;
			virtual InertiaTensor CalculateInertiaTensor(void) const = 0;

			virtual Point3D GetInitialSupportPoint(void) const = 0;
			virtual Point3D CalculateSupportPoint(const Vector3D& direction) const = 0;
			virtual void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const = 0;
			virtual void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const = 0;

			bool InteriorPoint(const Point3D& p) const;
			virtual bool DetectSegmentIntersection(const Point3D& p1, const Point3D& p2, float radius, ShapeHitData *shapeHitData) const;

			virtual float CalculateSubmergedVolume(const Antivector4D& plane, Point3D *centroid) const;
	};


	//# \class	BoxShapeObject		Encapsulates data pertaining to a box shape.
	//
	//# The $BoxShapeObject$ class encapsulates data pertaining to a box shape.
	//
	//# \def	class BoxShapeObject : public ShapeObject, public BoxVolume
	//
	//# \ctor	BoxShapeObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxShapeObject$ class encapsulates a physics shape that is a box whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	ShapeObject				A $BoxShapeObject$ is an object that can be shared by multiple box shape nodes.
	//# \base	WorldMgr/BoxVolume		A $BoxShapeObject$ is represented by a generic box volume.
	//
	//# \also	$@BoxShape@$


	class BoxShapeObject : public ShapeObject, public BoxVolume
	{
		friend class ShapeObject;

		private:

			Point3D						boxVertex[8];

			static const unsigned_int8	boxEdge[12][2];
			static const unsigned_int8	boxPolygon[30];
			static const ShapeMesh		boxMesh;

			BoxShapeObject();
			~BoxShapeObject();

		public:

			BoxShapeObject(const Vector3D& size);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	PyramidShapeObject		Encapsulates data pertaining to a pyramid shape.
	//
	//# The $PyramidShapeObject$ class encapsulates data pertaining to a pyramid shape.
	//
	//# \def	class PyramidShapeObject : public ShapeObject, public PyramidVolume
	//
	//# \ctor	PyramidShapeObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the pyramid base.
	//# \param	height	The height of the pyramid.
	//
	//# \desc
	//# The $PyramidShapeObject$ class encapsulates a physics shape that is a pyramid whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	ShapeObject					A $PyramidShapeObject$ is an object that can be shared by multiple pyramid shape nodes.
	//# \base	WorldMgr/PyramidVolume		A $PyramidShapeObject$ is represented by a generic pyramid volume.
	//
	//# \also	$@PyramidShape@$


	class PyramidShapeObject : public ShapeObject, public PyramidVolume
	{
		friend class ShapeObject;

		private:

			Vector3D					pyramidShrinkSize;

			Point3D						pyramidVertex[5];

			static const unsigned_int8	pyramidEdge[8][2];
			static const unsigned_int8	pyramidPolygon[21];
			static const ShapeMesh		pyramidMesh;

			PyramidShapeObject();
			~PyramidShapeObject();

		public:

			PyramidShapeObject(const Vector2D& size, float height);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	CylinderShapeObject		Encapsulates data pertaining to a cylinder shape.
	//
	//# The $CylinderShapeObject$ class encapsulates data pertaining to a cylinder shape.
	//
	//# \def	class CylinderShapeObject : public ShapeObject, public CylinderVolume
	//
	//# \ctor	CylinderShapeObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderShapeObject$ class encapsulates a physics shape that is a cylinder whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	ShapeObject					A $CylinderShapeObject$ is an object that can be shared by multiple cylinder shape nodes.
	//# \base	WorldMgr/CylinderVolume		A $CylinderShapeObject$ is represented by a generic cylinder volume.
	//
	//# \also	$@CylinderShape@$


	class CylinderShapeObject : public ShapeObject, public CylinderVolume
	{
		friend class ShapeObject;

		private:

			Point3D						cylinderVertex[16];

			static const unsigned_int8	cylinderEdge[24][2];
			static const unsigned_int8	cylinderPolygon[58];
			static const ShapeMesh		cylinderMesh;

			CylinderShapeObject();
			~CylinderShapeObject();

		public:

			CylinderShapeObject(const Vector2D& size, float height);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	ConeShapeObject		Encapsulates data pertaining to a cone shape.
	//
	//# The $ConeShapeObject$ class encapsulates data pertaining to a cone shape.
	//
	//# \def	class ConeShapeObject : public ShapeObject, public ConeVolume
	//
	//# \ctor	ConeShapeObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cone base.
	//# \param	height	The height of the cone.
	//
	//# \desc
	//# The $ConeShapeObject$ class encapsulates a physics shape that is a cone whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	ShapeObject				A $ConeShapeObject$ is an object that can be shared by multiple cone shape nodes.
	//# \base	WorldMgr/ConeVolume		A $ConeShapeObject$ is represented by a generic cone volume.
	//
	//# \also	$@ConeShape@$


	class ConeShapeObject : public ShapeObject, public ConeVolume
	{
		friend class ShapeObject;

		private:

			Vector3D					coneShrinkSize;

			Point3D						coneVertex[9];

			static const unsigned_int8	coneEdge[16][2];
			static const unsigned_int8	conePolygon[41];
			static const ShapeMesh		coneMesh;

			ConeShapeObject();
			~ConeShapeObject();

		public:

			ConeShapeObject(const Vector2D& size, float height);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	SphereShapeObject		Encapsulates data pertaining to a sphere shape.
	//
	//# The $SphereShapeObject$ class encapsulates data pertaining to a sphere shape.
	//
	//# \def	class SphereShapeObject : public ShapeObject, public SphereVolume
	//
	//# \ctor	SphereShapeObject(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereShapeObject$ class encapsulates a physics shape that is a sphere whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	ShapeObject					A $SphereShapeObject$ is an object that can be shared by multiple sphere shape nodes.
	//# \base	WorldMgr/SphereVolume		A $SphereShapeObject$ is represented by a generic sphere volume.
	//
	//# \also	$@SphereShape@$


	class SphereShapeObject : public ShapeObject, public SphereVolume
	{
		friend class ShapeObject;

		private:

			bool						sphericalFlag;

			Point3D						sphereVertex[26];

			static const unsigned_int8	sphereEdge[56][2];
			static const unsigned_int8	spherePolygon[144];
			static const ShapeMesh		sphereMesh;

			SphereShapeObject();
			~SphereShapeObject();

		public:

			SphereShapeObject(const Vector3D& size);

			bool GetSphericalFlag(void) const
			{
				return (sphericalFlag);
			}

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;

			bool DetectSegmentIntersection(const Point3D& p1, const Point3D& p2, float radius, ShapeHitData *shapeHitData) const override;
			float CalculateSubmergedVolume(const Antivector4D& plane, Point3D *centroid) const override;
	};


	//# \class	DomeShapeObject		Encapsulates data pertaining to a dome shape.
	//
	//# The $DomeShapeObject$ class encapsulates data pertaining to a dome shape.
	//
	//# \def	class DomeShapeObject : public ShapeObject, public DomeVolume
	//
	//# \ctor	DomeShapeObject(const Vector3D& size);
	//
	//# \param	size	The size of the dome.
	//
	//# \desc
	//# The $DomeShapeObject$ class encapsulates a physics shape that is a dome whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	ShapeObject				A $DomeShapeObject$ is an object that can be shared by multiple dome shape nodes.
	//# \base	WorldMgr/DomeVolume		A $DomeShapeObject$ is represented by a generic dome volume.
	//
	//# \also	$@DomeShape@$


	class DomeShapeObject : public ShapeObject, public DomeVolume
	{
		friend class ShapeObject;

		private:

			Point3D						domeVertex[17];

			static const unsigned_int8	domeEdge[32][2];
			static const unsigned_int8	domePolygon[81];
			static const ShapeMesh		domeMesh;

			DomeShapeObject();
			~DomeShapeObject();

		public:

			DomeShapeObject(const Vector3D& size);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	CapsuleShapeObject		Encapsulates data pertaining to a capsule shape.
	//
	//# The $CapsuleShapeObject$ class encapsulates data pertaining to a capsule shape.
	//
	//# \def	class CapsuleShapeObject : public ShapeObject, public CapsuleVolume
	//
	//# \ctor	CapsuleShapeObject(const Vector3D& size, float height);
	//
	//# \param	size	The size of the hemispherical caps of the capsule.
	//# \param	height	The height of the cylindrical interior of the capsule.
	//
	//# \desc
	//# The $CapsuleShapeObject$ class encapsulates a physics shape that is a capsule whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	ShapeObject					A $CapsuleShapeObject$ is an object that can be shared by multiple capsule shape nodes.
	//# \base	WorldMgr/CapsuleVolume		A $CapsuleShapeObject$ is represented by a generic capsule volume.
	//
	//# \also	$@CapsuleShape@$


	class CapsuleShapeObject : public ShapeObject, public CapsuleVolume
	{
		friend class ShapeObject;

		private:

			Point3D						capsuleVertex[34];

			static const unsigned_int8	capsuleEdge[72][2];
			static const unsigned_int8	capsulePolygon[184];
			static const ShapeMesh		capsuleMesh;

			CapsuleShapeObject();
			~CapsuleShapeObject();

		public:

			CapsuleShapeObject(const Vector3D& size, float height);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	TruncatedPyramidShapeObject		Encapsulates data pertaining to a truncated pyramid shape.
	//
	//# The $TruncatedPyramidShapeObject$ class encapsulates data pertaining to a truncated pyramid shape.
	//
	//# \def	class TruncatedPyramidShapeObject : public ShapeObject, public TruncatedPyramidVolume
	//
	//# \ctor	TruncatedPyramidShapeObject(const Vector2D& size, float height, float ratio);
	//
	//# \param	size	The size of truncated pyramid base.
	//# \param	height	The height of the truncated pyramid.
	//# \param	ratio	The ratio of the size of the top face to the size of the bottom face (base) of the truncated pyramid.
	//
	//# \desc
	//# The $TruncatedPyramidShapeObject$ class encapsulates a physics shape that is a truncated pyramid whose dimensions
	//# are specified by the $size$, $height$, and $ratio$ parameters.
	//
	//# \base	ShapeObject							A $TruncatedPyramidShapeObject$ is an object that can be shared by multiple truncated pyramid shape nodes.
	//# \base	WorldMgr/TruncatedPyramidVolume		A $TruncatedPyramidShapeObject$ is represented by a generic truncated pyramid volume.
	//
	//# \also	$@TruncatedPyramidShape@$


	class TruncatedPyramidShapeObject : public ShapeObject, public TruncatedPyramidVolume
	{
		friend class ShapeObject;

		private:

			Vector2D					pyramidShrinkSize;

			Point3D						truncatedPyramidVertex[8];

			static const unsigned_int8	truncatedPyramidEdge[12][2];
			static const unsigned_int8	truncatedPyramidPolygon[30];
			static const ShapeMesh		truncatedPyramidMesh;

			TruncatedPyramidShapeObject();
			~TruncatedPyramidShapeObject();

		public:

			TruncatedPyramidShapeObject(const Vector2D& size, float height, float ratio);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	TruncatedConeShapeObject		Encapsulates data pertaining to a truncated cone shape.
	//
	//# The $TruncatedConeShapeObject$ class encapsulates data pertaining to a truncated cone shape.
	//
	//# \def	class TruncatedConeShapeObject : public ShapeObject, public TruncatedConeVolume
	//
	//# \ctor	TruncatedConeShapeObject(const Vector2D& size, float height, float ratio);
	//
	//# \param	size	The size of truncated cone base.
	//# \param	height	The height of the truncated cone.
	//# \param	ratio	The ratio of the size of the top face to the size of the bottom face (base) of the truncated cone.
	//
	//# \desc
	//# The $TruncatedConeShapeObject$ class encapsulates a physics shape that is a truncated cone whose dimensions
	//# are specified by the $size$, $height$, and $ratio$ parameters.
	//
	//# \base	ShapeObject						A $TruncatedConeShapeObject$ is an object that can be shared by multiple truncated cone shape nodes.
	//# \base	WorldMgr/TruncatedConeVolume	A $TruncatedConeShapeObject$ is represented by a generic truncated cone volume.
	//
	//# \also	$@TruncatedConeShape@$


	class TruncatedConeShapeObject : public ShapeObject, public TruncatedConeVolume
	{
		friend class ShapeObject;

		private:

			Vector2D					coneShrinkSize;

			Point3D						truncatedConeVertex[16];

			static const unsigned_int8	truncatedConeEdge[24][2];
			static const unsigned_int8	truncatedConePolygon[58];
			static const ShapeMesh		truncatedConeMesh;

			TruncatedConeShapeObject();
			~TruncatedConeShapeObject();

		public:

			TruncatedConeShapeObject(const Vector2D& size, float height, float ratio);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	TruncatedDomeShapeObject		Encapsulates data pertaining to a truncated dome shape.
	//
	//# The $TruncatedDomeShapeObject$ class encapsulates data pertaining to a truncated dome shape.
	//
	//# \def	class TruncatedDomeShapeObject : public ShapeObject, public TruncatedDomeVolume
	//
	//# \ctor	TruncatedDomeShapeObject(const Vector2D& size, float height, float ratio);
	//
	//# \param	size	The size of truncated dome base.
	//# \param	height	The height of the truncated dome.
	//# \param	ratio	The ratio of the size of the top face to the size of the bottom face (base) of the truncated dome.
	//
	//# \desc
	//# The $TruncatedDomeShapeObject$ class encapsulates a physics shape that is a truncated dome whose dimensions
	//# are specified by the $size$, $height$, and $ratio$ parameters.
	//
	//# \base	ShapeObject						A $TruncatedDomeShapeObject$ is an object that can be shared by multiple truncated dome shape nodes.
	//# \base	WorldMgr/TruncatedDomeVolume	A $TruncatedDomeShapeObject$ is represented by a generic truncated dome volume.
	//
	//# \also	$@TruncatedDomeShape@$


	class TruncatedDomeShapeObject : public ShapeObject, public TruncatedDomeVolume
	{
		friend class ShapeObject;

		private:

			Point3D						truncatedDomeVertex[24];

			static const unsigned_int8	truncatedDomeEdge[40][2];
			static const unsigned_int8	truncatedDomePolygon[98];
			static const ShapeMesh		truncatedDomeMesh;

			TruncatedDomeShapeObject();
			~TruncatedDomeShapeObject();

		public:

			TruncatedDomeShapeObject(const Vector2D& size, float height, float ratio);

			void Preprocess(void) override;

			float CalculateVolume(void) const override;
			Point3D CalculateCenterOfMass(void) const override;
			InertiaTensor CalculateInertiaTensor(void) const override;

			Point3D GetInitialSupportPoint(void) const override;
			Point3D CalculateSupportPoint(const Vector3D& direction) const override;
			void CalculateOpposingSupportPoints(const Vector3D& direction, Point3D *support) const override;
			void CalculateSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const override;
	};


	//# \class	Shape		Represents a shape node in a world.
	//
	//# The $Shape$ class represents a shape node in a world.
	//
	//# \def	class Shape : public Node, public ListElement<Shape>
	//
	//# \ctor	Shape(ShapeType type);
	//
	//# The constructor has protected access. A $Shape$ class can only exist as the base class for a more specific type of shape.
	//
	//# \param	type	The type of the shape. See below for a list of possible types.
	//
	//# \desc
	//# The $Shape$ class represents a shape node in a world.
	//#
	//# A shape node can be of one of the following types.
	//
	//# \table	ShapeType
	//
	//# \base	WorldMgr/Node					A $Shape$ node is a scene graph node.
	//# \base	Utilities/ListElement<Shape>	Each rigid body maintains a list of the shapes that compose it.
	//
	//# \also	$@ShapeObject@$
	//# \also	$@RigidBodyController@$


	//# \function	Shape::GetShapeType		Returns the specific type of a shape.
	//
	//# \proto	ShapeType GetShapeType(void) const;
	//
	//# \desc
	//# The $GetShapeType$ function returns the specific shape type, which can be one of the following constants.
	//
	//# \table	ShapeType


	class Shape : public Node, public ListElement<Shape>
	{
		friend class Node;

		private:

			struct TriangleIntersectionParams
			{
				Triangle				triangle;
				Point3D					triangleVertex[3];
				Antivector3D			triangleNormal;

				Transform4D				shapeTransform;
				Matrix3D				shapeInverseMatrix3D;

				const GeometryObject	*geometryObject;
				const Mesh				*geometryMesh;
				const Transform4D		*geometryTransform;
			};

			ShapeType					shapeType;

			Box3D						shapeCollisionBox;

			static unsigned_int32		(*const simplexMinFunc[4])(const Point3D *, Point3D *);

			static Shape *Create(Unpacker& data, unsigned_int32 unpackFlags);

			static unsigned_int32 CalculateSimplexMinimum(int32 count, const Point3D *simplex, Point3D *p)
			{
				return ((*simplexMinFunc[count])(simplex, p));
			}

			static unsigned_int32 CalculateZeroSimplexMinimum(const Point3D *simplex, Point3D *p);
			static unsigned_int32 CalculateOneSimplexMinimum(const Point3D *simplex, Point3D *p);
			static unsigned_int32 CalculateTwoSimplexMinimum(const Point3D *simplex, Point3D *p);
			static unsigned_int32 CalculateThreeSimplexMinimum(const Point3D *simplex, Point3D *p);

			bool StaticIntersectShape(const Shape *betaShape, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const;
			bool FindMinimumShapePenetration(const Shape *betaShape, const Transform4D& alphaTransform, const Transform4D& alphaInverseTransform, const Transform4D& betaTransform, const Transform4D& betaInverseTransform, IntersectionData *intersectionData) const;

			bool StaticIntersectPrimitive(const PrimitiveGeometry *geometry, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const;
			bool FindMinimumPrimitivePenetration(const PrimitiveGeometry *geometry, const Transform4D& shapeTransform, const Matrix3D& shapeInverseMatrix3D, IntersectionData *intersectionData) const;

			bool StaticIntersectConvexHull(const Geometry *geometry, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const;
			bool FindMinimumConvexHullPenetration(const Geometry *geometry, const Transform4D& shapeTransform, const Matrix3D& shapeInverseMatrix3D, IntersectionData *intersectionData) const;

			static unsigned_int32 CalculateTriangleSupportPoint(const Point3D *vertex, const Vector3D& direction);
			static void CalculateTriangleSupportPointArray(const Point3D *vertex, int32 count, const Vector3D *direction, unsigned_int8 *supportIndex);

			bool StaticIntersectTriangle(const TriangleIntersectionParams *params, IntersectionData *intersectionData) const;
			bool StaticIntersectTriangle(const TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 *dimension1, int32 *dimension2, const Vector3D& worldOffset) const;
			bool FindMinimumTrianglePenetration(const TriangleIntersectionParams *params, IntersectionData *intersectionData) const;
			bool DynamicIntersectTriangle(const Vector3D& displacement, const TriangleIntersectionParams *params, IntersectionData *intersectionData) const;

			int32 StaticIntersectGeometryOctree(const CollisionOctree *octree, const Box3D& collisionBox, TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 contactCount) const;
			int32 DynamicIntersectGeometryOctree(const Vector3D& displacement, const CollisionOctree *octree, const Box3D& collisionBox, TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 contactCount) const;
			int32 MixedIntersectGeometryOctree(const Vector3D& displacement, const CollisionOctree *octree, const Box3D& collisionBox, int32 contactTriangleCount, const unsigned_int32 *contactTriangleIndex, TriangleIntersectionParams *params, IntersectionData *intersectionData, int32 contactCount) const;

		protected:

			Shape(ShapeType type);
			Shape(const Shape& shape);

		public:

			enum
			{
				kPenetrationDirectionCount = 26
			};

			static const ConstVector3D penetrationDirectionTable[kPenetrationDirectionCount];

			using ListElement<Shape>::Previous;
			using ListElement<Shape>::Next;

			virtual ~Shape();

			ShapeType GetShapeType(void) const
			{
				return (shapeType);
			}

			ShapeObject *GetObject(void) const
			{
				return (static_cast<ShapeObject *>(Node::GetObject()));
			}

			const Box3D& GetCollisionBox(void) const
			{
				return (shapeCollisionBox);
			}

			void PackType(Packer& data) const override;

			void Preprocess(void) override;

			void CalculateCollisionBox(const Transform4D& transform1, const Transform4D& transform2);

			bool StaticIntersectShape(const Shape *betaShape, RigidBodyContact *rigidBodyContact, IntersectionData *intersectionData) const;
			bool DynamicIntersectShape(const Shape *betaShape, const Vector3D& alphaDisplacement, const Vector3D& betaDisplacement, IntersectionData *intersectionData) const;

			bool StaticIntersectPrimitive(const PrimitiveGeometry *geometry, GeometryContact *geometryContact, IntersectionData *intersectionData) const;
			bool DynamicIntersectPrimitive(const PrimitiveGeometry *geometry, const Vector3D& displacement, IntersectionData *intersectionData) const;

			bool StaticIntersectConvexHull(const Geometry *geometry, GeometryContact *geometryContact, IntersectionData *intersectionData) const;
			bool DynamicIntersectConvexHull(const Geometry *geometry, const Vector3D& displacement, IntersectionData *intersectionData) const;

			int32 StaticIntersectGeometry(const Geometry *geometry, IntersectionData *intersectionData) const;
			int32 DynamicIntersectGeometry(const Geometry *geometry, const Vector3D& displacement, IntersectionData *intersectionData) const;
			int32 MixedIntersectGeometry(const Geometry *geometry, const Vector3D& displacement, int32 contactTriangleCount, const unsigned_int32 *contactTriangleIndex, IntersectionData *intersectionData) const;
	};


	//# \class	BoxShape		Represents a box shape node in a world.
	//
	//# The $BoxShape$ class represents a box shape node in a world.
	//
	//# \def	class BoxShape : public Shape
	//
	//# \ctor	BoxShape(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxShape$ class represents a shape node that is shaped like a box
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Shape		A box shape is a specific type of shape.
	//
	//# \also	$@BoxShapeObject@$


	class BoxShape : public Shape
	{
		friend class Shape;

		private:

			BoxShape();
			BoxShape(const BoxShape& boxShape);

			Node *Replicate(void) const override;

		public:

			C4API BoxShape(const Vector3D& size);
			C4API ~BoxShape();

			BoxShapeObject *GetObject(void) const
			{
				return (static_cast<BoxShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	PyramidShape		Represents a pyramid shape node in a world.
	//
	//# The $PyramidShape$ class represents a pyramid shape node in a world.
	//
	//# \def	class PyramidShape : public Shape
	//
	//# \ctor	PyramidShape(const Vector2D& size, float height);
	//
	//# \param	size	The size of the pyramid base.
	//# \param	height	The height of the pyramid.
	//
	//# \desc
	//# The $PyramidShape$ class represents a shape node that is shaped like a pyramid
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Shape		A pyramid shape is a specific type of shape.
	//
	//# \also	$@PyramidShapeObject@$


	class PyramidShape : public Shape
	{
		friend class Shape;

		private:

			PyramidShape();
			PyramidShape(const PyramidShape& pyramidShape);

			Node *Replicate(void) const override;

		public:

			C4API PyramidShape(const Vector2D& size, float height);
			C4API ~PyramidShape();

			PyramidShapeObject *GetObject(void) const
			{
				return (static_cast<PyramidShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	CylinderShape		Represents a cylinder shape node in a world.
	//
	//# The $CylinderShape$ class represents a cylinder shape node in a world.
	//
	//# \def	class CylinderShape : public Shape
	//
	//# \ctor	CylinderShape(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderShape$ class represents a shape node that is shaped like a cylinder
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Shape		A cylinder shape is a specific type of shape.
	//
	//# \also	$@CylinderShapeObject@$


	class CylinderShape : public Shape
	{
		friend class Shape;

		private:

			CylinderShape();
			CylinderShape(const CylinderShape& cylinderShape);

			Node *Replicate(void) const override;

		public:

			C4API CylinderShape(const Vector2D& size, float height);
			C4API ~CylinderShape();

			CylinderShapeObject *GetObject(void) const
			{
				return (static_cast<CylinderShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	ConeShape		Represents a cone shape node in a world.
	//
	//# The $ConeShape$ class represents a cone shape node in a world.
	//
	//# \def	class ConeShape : public Shape
	//
	//# \ctor	ConeShape(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cone base.
	//# \param	height	The height of the cone.
	//
	//# \desc
	//# The $ConeShape$ class represents a shape node that is shaped like a cone
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Shape		A cone shape is a specific type of shape.
	//
	//# \also	$@ConeShapeObject@$


	class ConeShape : public Shape
	{
		friend class Shape;

		private:

			ConeShape();
			ConeShape(const ConeShape& coneShape);

			Node *Replicate(void) const override;

		public:

			C4API ConeShape(const Vector2D& size, float height);
			C4API ~ConeShape();

			ConeShapeObject *GetObject(void) const
			{
				return (static_cast<ConeShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	SphereShape		Represents a sphere shape node in a world.
	//
	//# The $SphereShape$ class represents a sphere shape node in a world.
	//
	//# \def	class SphereShape : public Shape
	//
	//# \ctor	SphereShape(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereShape$ class represents a shape node that is shaped like a sphere
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Shape		A sphere shape is a specific type of shape.
	//
	//# \also	$@SphereShapeObject@$


	class SphereShape : public Shape
	{
		friend class Shape;

		private:

			SphereShape();
			SphereShape(const SphereShape& sphereShape);

			Node *Replicate(void) const override;

		public:

			C4API SphereShape(const Vector3D& size);
			C4API ~SphereShape();

			SphereShapeObject *GetObject(void) const
			{
				return (static_cast<SphereShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	DomeShape		Represents a dome shape node in a world.
	//
	//# The $DomeShape$ class represents a dome shape node in a world.
	//
	//# \def	class DomeShape : public Shape
	//
	//# \ctor	DomeShape(const Vector3D& size);
	//
	//# \param	size	The size of the dome.
	//
	//# \desc
	//# The $DomeShape$ class represents a shape node that is shaped like a dome
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Shape		A dome shape is a specific type of shape.
	//
	//# \also	$@DomeShapeObject@$


	class DomeShape : public Shape
	{
		friend class Shape;

		private:

			DomeShape();
			DomeShape(const DomeShape& domeShape);

			Node *Replicate(void) const override;

		public:

			C4API DomeShape(const Vector3D& size);
			C4API ~DomeShape();

			DomeShapeObject *GetObject(void) const
			{
				return (static_cast<DomeShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	CapsuleShape		Represents a capsule shape node in a world.
	//
	//# The $CapsuleShape$ class represents a capsule shape node in a world.
	//
	//# \def	class CapsuleShape : public Shape
	//
	//# \ctor	CapsuleShape(const Vector3D& size, float height);
	//
	//# \param	size	The size of the hemispherical caps of the capsule.
	//# \param	height	The height of the cylindrical interior of the capsule.
	//
	//# \desc
	//# The $CapsuleShape$ class represents a shape node that is shaped like a capsule
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Shape		A capsule shape is a specific type of shape.
	//
	//# \also	$@CapsuleShapeObject@$


	class CapsuleShape : public Shape
	{
		friend class Shape;

		private:

			CapsuleShape();
			CapsuleShape(const CapsuleShape& capsuleShape);

			Node *Replicate(void) const override;

		public:

			C4API CapsuleShape(const Vector3D& size, float height);
			C4API ~CapsuleShape();

			CapsuleShapeObject *GetObject(void) const
			{
				return (static_cast<CapsuleShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	TruncatedPyramidShape		Represents a truncated pyramid shape node in a world.
	//
	//# The $TruncatedPyramidShape$ class represents a truncated pyramid shape node in a world.
	//
	//# \def	class TruncatedPyramidShape : public Shape
	//
	//# \ctor	TruncatedPyramidShape(const Vector2D& size, float height, float ratio);
	//
	//# \param	size	The size of truncated pyramid base.
	//# \param	height	The height of the truncated pyramid.
	//# \param	ratio	The ratio of the size of the top face to the size of the bottom face (base) of the truncated pyramid.
	//
	//# \desc
	//# The $TruncatedPyramidShape$ class represents a shape node that is shaped like a truncated pyramid
	//# whose dimensions are specified by the $size$, $height$, and $ratio$ parameters.
	//
	//# \base	Shape		A truncated pyramid shape is a specific type of shape.
	//
	//# \also	$@TruncatedPyramidShapeObject@$


	class TruncatedPyramidShape : public Shape
	{
		friend class Shape;

		private:

			TruncatedPyramidShape();
			TruncatedPyramidShape(const TruncatedPyramidShape& truncatedPyramidShape);

			Node *Replicate(void) const override;

		public:

			C4API TruncatedPyramidShape(const Vector2D& size, float height, float ratio);
			C4API ~TruncatedPyramidShape();

			TruncatedPyramidShapeObject *GetObject(void) const
			{
				return (static_cast<TruncatedPyramidShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	TruncatedConeShape		Represents a truncated cone shape node in a world.
	//
	//# The $TruncatedConeShape$ class represents a truncated cone shape node in a world.
	//
	//# \def	class TruncatedConeShape : public Shape
	//
	//# \ctor	TruncatedConeShape(const Vector2D& size, float height, float ratio);
	//
	//# \param	size	The size of truncated cone base.
	//# \param	height	The height of the truncated cone.
	//# \param	ratio	The ratio of the size of the top face to the size of the bottom face (base) of the truncated cone.
	//
	//# \desc
	//# The $TruncatedConeShape$ class represents a shape node that is shaped like a truncated cone
	//# whose dimensions are specified by the $size$, $height$, and $ratio$ parameters.
	//
	//# \base	Shape		A truncated cone shape is a specific type of shape.
	//
	//# \also	$@TruncatedConeShapeObject@$


	class TruncatedConeShape : public Shape
	{
		friend class Shape;

		private:

			TruncatedConeShape();
			TruncatedConeShape(const TruncatedConeShape& truncatedConeShape);

			Node *Replicate(void) const override;

		public:

			C4API TruncatedConeShape(const Vector2D& size, float height, float ratio);
			C4API ~TruncatedConeShape();

			TruncatedConeShapeObject *GetObject(void) const
			{
				return (static_cast<TruncatedConeShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};


	//# \class	TruncatedDomeShape		Represents a truncated dome shape node in a world.
	//
	//# The $TruncatedDomeShape$ class represents a truncated dome shape node in a world.
	//
	//# \def	class TruncatedDomeShape : public Shape
	//
	//# \ctor	TruncatedDomeShape(const Vector2D& size, float height, float ratio);
	//
	//# \param	size	The size of truncated dome base.
	//# \param	height	The height of the truncated dome.
	//# \param	ratio	The ratio of the size of the top face to the size of the bottom face (base) of the truncated dome.
	//
	//# \desc
	//# The $TruncatedDomeShape$ class represents a shape node that is shaped like a truncated dome
	//# whose dimensions are specified by the $size$, $height$, and $ratio$ parameters.
	//
	//# \base	Shape		A truncated dome shape is a specific type of shape.
	//
	//# \also	$@TruncatedDomeShapeObject@$


	class TruncatedDomeShape : public Shape
	{
		friend class Shape;

		private:

			TruncatedDomeShape();
			TruncatedDomeShape(const TruncatedDomeShape& truncatedDomeShape);

			Node *Replicate(void) const override;

		public:

			C4API TruncatedDomeShape(const Vector2D& size, float height, float ratio);
			C4API ~TruncatedDomeShape();

			TruncatedDomeShapeObject *GetObject(void) const
			{
				return (static_cast<TruncatedDomeShapeObject *>(Node::GetObject()));
			}

			bool CalculateBoundingBox(Box3D *box) const override;
	};
}


#endif

// ZYUQURM
