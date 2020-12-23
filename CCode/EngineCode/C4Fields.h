 

#ifndef C4Fields_h
#define C4Fields_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Volumes.h"
#include "C4Node.h"


namespace C4
{
	typedef Type	FieldType;


	enum : ObjectType
	{
		kObjectField		= 'FELD'
	};


	//# \enum	FieldType

	enum : FieldType
	{
		kFieldBox			= 'BOX ',		//## Box field.
		kFieldCylinder		= 'CYLD',		//## Cylinder field.
		kFieldSphere		= 'SPHR'		//## Sphere field.
	};


	//# \enum	FieldFlags

	enum
	{
		kFieldExclusive		= 1 << 0		//## The field is for exclusive use by a controller and does not globally affect rigid bodies or deformable bodies.
	};


	class Force;


	//# \class	FieldObject		Encapsulates data pertaining to a field volume.
	//
	//# The $FieldObject$ class encapsulates data pertaining to a field volume.
	//
	//# \def	class FieldObject : public Object, public VolumeObject
	//
	//# \ctor	FieldObject(FieldType type, Volume *volume);
	//
	//# The constructor has protected access. The $FieldObject$ class can only exist as the base class for another class.
	//
	//# \param	type		The type of the field volume. See below for a list of possible types.
	//# \param	volume		A pointer to the generic volume object representing the field.
	//
	//# \desc
	//# The $FieldObject$ class encapsulates data describing a volume used as a force field in a physics simulation.
	//#
	//# A field object can be of one of the following types.
	//
	//# \table	FieldType
	//
	//# \base		WorldMgr/Object		A $FieldObject$ is an object that can be shared by multiple field nodes.
	//# \privbase	VolumeObject		Used internally by the engine for generic volume objects.
	//
	//# \also	$@Field@$


	//# \function	FieldObject::GetFieldType		Returns the specific type of a field.
	//
	//# \proto	FieldType GetFieldType(void) const;
	//
	//# \desc
	//# The $GetFieldType$ function returns the specific field type, which can be one of the following constants.
	//
	//# \table	FieldType


	//# \function	FieldObject::GetFieldFlags		Returns the field flags.
	//
	//# \proto	unsigned_int32 GetFieldFlags(void) const;
	//
	//# \desc
	//# The $GetFieldFlags$ function returns the field flags, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	FieldFlags
	//
	//# The initial value for the field flags is 0.
	//
	//# \also	$@FieldObject::SetFieldFlags@$


	//# \function	FieldObject::SetFieldFlags		Sets the field flags.
	//
	//# \proto	void SetFieldFlags(unsigned_int32 flags);
	//
	//# \param	flags		The new field flags.
	// 
	//# \desc
	//# The $SetFieldFlags$ function sets the field flags to the value specified by the $flags$ parameter,
	//# which can be a combination (through logical OR) of the following constants. 
	//
	//# \table	FieldFlags 
	//
	//# The initial value for the field flags is 0.
	// 
	//# \also	$@FieldObject::GetFieldFlags@$
 
 
	class FieldObject : public Object, public VolumeObject
	{
		friend class WorldMgr;
 
		private:

			FieldType		fieldType;
			unsigned_int32	fieldFlags;

			static FieldObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			FieldObject(FieldType type, Volume *volume);
			~FieldObject();

		public:

			FieldType GetFieldType(void) const
			{
				return (fieldType);
			}

			unsigned_int32 GetFieldFlags(void) const
			{
				return (fieldFlags);
			}

			void SetFieldFlags(unsigned_int32 flags)
			{
				fieldFlags = flags;
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

			virtual Antivector4D CalculateSurfacePlane(void) const;
	};


	//# \class	BoxFieldObject		Encapsulates data pertaining to a box field.
	//
	//# The $BoxFieldObject$ class encapsulates data pertaining to a box field.
	//
	//# \def	class BoxFieldObject final : public FieldObject, public BoxVolume
	//
	//# \ctor	BoxFieldObject(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxFieldObject$ class encapsulates a field volume shaped like a box whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	FieldObject				A $BoxFieldObject$ is an object that can be shared by multiple box field nodes.
	//# \base	WorldMgr/BoxVolume		A $BoxFieldObject$ is represented by a generic box volume.
	//
	//# \also	$@BoxField@$


	class BoxFieldObject final : public FieldObject, public BoxVolume
	{
		friend class FieldObject;

		private:

			BoxFieldObject();
			~BoxFieldObject();

		public:

			BoxFieldObject(const Vector3D& size);

			Antivector4D CalculateSurfacePlane(void) const override;
	};


	//# \class	CylinderFieldObject		Encapsulates data pertaining to a cylinder field.
	//
	//# The $CylinderFieldObject$ class encapsulates data pertaining to a cylinder field.
	//
	//# \def	class CylinderFieldObject final : public FieldObject, public CylinderVolume
	//
	//# \ctor	CylinderFieldObject(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderFieldObject$ class encapsulates a field volume shaped like a cylinder whose dimensions
	//# are specified by the $size$ and $height$ parameters.
	//
	//# \base	FieldObject					A $CylinderFieldObject$ is an object that can be shared by multiple cylinder field nodes.
	//# \base	WorldMgr/CylinderVolume		A $CylinderFieldObject$ is represented by a generic cylinder volume.
	//
	//# \also	$@CylinderField@$


	class CylinderFieldObject final : public FieldObject, public CylinderVolume
	{
		friend class FieldObject;

		private:

			CylinderFieldObject();
			~CylinderFieldObject();

		public:

			CylinderFieldObject(const Vector2D& size, float height);

			Antivector4D CalculateSurfacePlane(void) const override;
	};


	//# \class	SphereFieldObject		Encapsulates data pertaining to a sphere field.
	//
	//# The $SphereFieldObject$ class encapsulates data pertaining to a sphere field.
	//
	//# \def	class SphereFieldObject final : public FieldObject, public SphereVolume
	//
	//# \ctor	SphereFieldObject(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereFieldObject$ class encapsulates a field volume shaped like a sphere whose dimensions
	//# are specified by the $size$ parameter.
	//
	//# \base	FieldObject					A $SphereFieldObject$ is an object that can be shared by multiple sphere field nodes.
	//# \base	WorldMgr/SphereVolume		A $SphereFieldObject$ is represented by a generic sphere volume.
	//
	//# \also	$@SphereField@$


	class SphereFieldObject final : public FieldObject, public SphereVolume
	{
		friend class FieldObject;

		private:

			SphereFieldObject();
			~SphereFieldObject();

		public:

			SphereFieldObject(const Vector3D& size);
	};


	//# \class	Field		Represents a field node in a world.
	//
	//# The $Field$ class represents a field node in a world.
	//
	//# \def	class Field : public Node
	//
	//# \ctor	Field(FieldType type);
	//
	//# The constructor has protected access. A $Field$ class can only exist as the base class for a more specific type of field.
	//
	//# \param	type	The type of the field. See below for a list of possible types.
	//
	//# \desc
	//# The $Field$ class represents a field node in a world.
	//#
	//# A field node can be of one of the following types.
	//
	//# \table	FieldType
	//
	//# \base	WorldMgr/Node		A $Field$ node is a scene graph node.
	//
	//# \also	$@FieldObject@$
	//# \also	$@Force@$


	//# \function	Field::GetFieldType		Returns the specific type of a field.
	//
	//# \proto	FieldType GetFieldType(void) const;
	//
	//# \desc
	//# The $GetFieldType$ function returns the specific field type, which can be one of the following constants.
	//
	//# \table	FieldType


	//# \function	Field::GetForce		Returns the force assigned to a field.
	//
	//# \proto	Force *GetForce(void) const;
	//
	//# \desc
	//# The $GetForce$ function returns the force assigned to a field. If there is no force assigned, then
	//# this function returns $nullptr$.
	//
	//# \also	$@Field::SetForce@$
	//# \also	$@Force@$


	//# \function	Field::SetForce		Assigns a force to a field.
	//
	//# \proto	void SetForce(Force *force);
	//
	//# \param	force	The force object to assign to the field.
	//
	//# \desc
	//# The $SetForce$ function assigns the force specified by the $force$ parameter to a field. The $force$
	//# parameter may be $nullptr$ to remove any existing force from the field.
	//
	//# \also	$@Field::SetForce@$
	//# \also	$@Force@$


	class Field : public Node
	{
		friend class Node;

		private:

			FieldType			fieldType;

			Force				*fieldForce;
			Antivector4D		surfacePlane;

			volatile int32		queryThreadFlags;

			static Field *Create(Unpacker& data, unsigned_int32 unpackFlags);

			void HandleTransformUpdate(void) override;
			void HandleVisibilityUpdate(void) override;

			bool CalculateBoundingBox(Box3D *box) const override;

		protected:

			Field(FieldType type);
			Field(const Field& field);

		public:

			virtual ~Field();

			FieldType GetFieldType(void) const
			{
				return (fieldType);
			}

			FieldObject *GetObject(void) const
			{
				return (static_cast<FieldObject *>(Node::GetObject()));
			}

			Force *GetForce(void) const
			{
				return (fieldForce);
			}

			const Antivector4D& GetSurfacePlane(void) const
			{
				return (surfacePlane);
			}

			volatile int32 *GetQueryThreadFlags(void)
			{
				return (&queryThreadFlags);
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			void Preprocess(void) override;
			void Neutralize(void) override;

			C4API void SetForce(Force *force);
	};


	//# \class	BoxField		Represents a box field node in a world.
	//
	//# The $BoxField$ class represents a box field node in a world.
	//
	//# \def	class BoxField final : public Field
	//
	//# \ctor	BoxField(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxField$ class represents a field node that is shaped like a box
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Field		A box field is a specific type of field.
	//
	//# \also	$@BoxFieldObject@$


	class BoxField final : public Field
	{
		friend class Field;

		private:

			BoxField();
			BoxField(const BoxField& boxField);

			Node *Replicate(void) const override;

		public:

			C4API BoxField(const Vector3D& size);
			C4API ~BoxField();

			BoxFieldObject *GetObject(void) const
			{
				return (static_cast<BoxFieldObject *>(Node::GetObject()));
			}
	};


	//# \class	CylinderField		Represents a cylinder field node in a world.
	//
	//# The $CylinderField$ class represents a cylinder field node in a world.
	//
	//# \def	class CylinderField final : public Field
	//
	//# \ctor	CylinderField(const Vector2D& size, float height);
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderField$ class represents a field node that is shaped like a cylinder
	//# whose dimensions are specified by the $size$ and $height$ parameters.
	//
	//# \base	Field		A cylinder field is a specific type of field.
	//
	//# \also	$@CylinderFieldObject@$


	class CylinderField final : public Field
	{
		friend class Field;

		private:

			CylinderField();
			CylinderField(const CylinderField& cylinderField);

			Node *Replicate(void) const override;

		public:

			C4API CylinderField(const Vector2D& size, float height);
			C4API ~CylinderField();

			CylinderFieldObject *GetObject(void) const
			{
				return (static_cast<CylinderFieldObject *>(Node::GetObject()));
			}
	};


	//# \class	SphereField		Represents a sphere field node in a world.
	//
	//# The $SphereField$ class represents a sphere field node in a world.
	//
	//# \def	class SphereField final : public Field
	//
	//# \ctor	SphereField(const Vector3D& size);
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereField$ class represents a field node that is shaped like a sphere
	//# whose dimensions are specified by the $size$ parameter.
	//
	//# \base	Field		A sphere field is a specific type of field.
	//
	//# \also	$@SphereFieldObject@$


	class SphereField final : public Field
	{
		friend class Field;

		private:

			SphereField();
			SphereField(const SphereField& sphereField);

			Node *Replicate(void) const override;

		public:

			C4API SphereField(const Vector3D& size);
			C4API ~SphereField();

			SphereFieldObject *GetObject(void) const
			{
				return (static_cast<SphereFieldObject *>(Node::GetObject()));
			}
	};
}

#endif

// ZYUQURM
