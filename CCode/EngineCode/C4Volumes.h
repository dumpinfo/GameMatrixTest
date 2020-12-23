 

#ifndef C4Volumes_h
#define C4Volumes_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Packing.h"


namespace C4
{
	class Box3D;
	class BoundingSphere;


	class Volume
	{
		protected:

			Volume() {}

		public:

			virtual ~Volume() = default;

			virtual void PackVolume(Packer& data, unsigned_int32 packFlags) const;
			virtual void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags);

			virtual int32 GetVolumeObjectSize(float *size) const;
			virtual void SetVolumeObjectSize(const float *size);

			virtual float GetVolumeLength(void) const = 0;

			virtual void CalculateBoundingBox(Box3D *box) const = 0;
			virtual void CalculateBoundingSphere(BoundingSphere *sphere) const = 0;

			virtual bool ExteriorSphere(const Point3D& center, float radius) const = 0;
			virtual bool InteriorPoint(const Point3D& p) const = 0;
	};


	class VolumeObject
	{
		private:

			Volume		*volumePointer;

		protected:

			VolumeObject(Volume *volume)
			{
				volumePointer = volume;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const
			{
				volumePointer->PackVolume(data, packFlags);
			}

			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) const
			{
				volumePointer->UnpackVolume(data, unpackFlags);
			}

			int32 GetVolumeObjectSize(float *size) const
			{
				return (volumePointer->GetVolumeObjectSize(size));
			}

			void SetVolumeObjectSize(const float *size) const
			{
				volumePointer->SetVolumeObjectSize(size);
			}

		public:

			Volume *GetVolume(void) const
			{
				return (volumePointer);
			}

			float GetVolumeLength(void) const
			{
				return (volumePointer->GetVolumeLength());
			}

			void CalculateBoundingBox(Box3D *box) const
			{
				volumePointer->CalculateBoundingBox(box);
			}

			void CalculateBoundingSphere(BoundingSphere *sphere) const
			{
				volumePointer->CalculateBoundingSphere(sphere);
			}

			bool ExteriorSphere(const Point3D& center, float radius) const
			{
				return (volumePointer->ExteriorSphere(center, radius)); 
			}

			bool InteriorPoint(const Point3D& p) const 
			{
				return (volumePointer->InteriorPoint(p)); 
			}
	};
 

	//# \class	PlateVolume		Encapsulates data pertaining to a generic plate volume. 
	// 
	//# The $PlateVolume$ class encapsulates data pertaining to a generic plate volume.
	//
	//# \def	class PlateVolume : public Volume
	// 
	//# \ctor	PlateVolume(const Vector2D& size);
	//
	//# The constructor has protected access. The $PlateVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the plate.
	//
	//# \desc
	//# The $PlateVolume$ class encapsulates the physical dimensions of a generic semi-infinite rectangular volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The volume corresponds to the space below the <i>x</i>-<i>y</i> plane inside a rectangular plate.
	//# One corner of the plate coincides with the origin, and the two components of the $size$ parameter
	//# correspond to the dimensions of the plate along the positive <i>x</i> and <i>y</i> axes.
	//
	//# \privbase	Volume		A $PlateVolume$ is a generic volume.


	//# \function	PlateVolume::GetPlateSize		Returns the plate size.
	//
	//# \proto	const Vector2D& GetPlateSize(void) const;
	//
	//# \desc
	//# The $GetPlateSize$ function returns a 2D vector containing the physical dimensions of the rectangular
	//# cross-section of the semi-infinite volume. One corner of the plate coincides with the origin, and the two
	//# components of the size correspond to the dimensions of the plate along the positive <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@PlateVolume::SetPlateSize@$


	//# \function	PlateVolume::SetPlateSize		Sets the plate size.
	//
	//# \proto	void SetPlateSize(const Vector2D& size);
	//
	//# \param	size	The new plate size.
	//
	//# \desc
	//# The $SetPlateSize$ function sets the physical dimensions of the rectangular cross-section of the semi-infinite
	//# volume to those specified by the $size$ parameter. One corner of the plate coincides with the origin, and the
	//# two components of the size correspond to the dimensions of the plate along the positive <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@PlateVolume::GetPlateSize@$


	class PlateVolume : public Volume
	{
		private:

			Vector2D	plateSize;

		protected:

			PlateVolume() {}

			PlateVolume(const Vector2D& size)
			{
				plateSize = size;
			}

		public:

			~PlateVolume() = default;

			const Vector2D& GetPlateSize(void) const
			{
				return (plateSize);
			}

			void SetPlateSize(const Vector2D& size)
			{
				plateSize = size;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	BoxVolume		Encapsulates data pertaining to a generic box volume.
	//
	//# The $BoxVolume$ class encapsulates data pertaining to a generic box volume.
	//
	//# \def	class BoxVolume : public Volume
	//
	//# \ctor	BoxVolume(const Vector3D& size);
	//
	//# The constructor has protected access. The $BoxVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//# The $BoxVolume$ class encapsulates the physical dimensions of a generic box-shaped volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# One corner of the box coincides with the origin, and the three components of the $size$ parameter
	//# correspond to the dimensions of the box along the positive <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \privbase	Volume		A $BoxVolume$ is a generic volume.


	//# \function	BoxVolume::GetBoxSize		Returns the box size.
	//
	//# \proto	const Vector3D& GetBoxSize(void) const;
	//
	//# \desc
	//# The $GetBoxSize$ function returns a 3D vector containing the physical dimensions of the box volume.
	//# One corner of the box coincides with the origin, and the three components of the size correspond to
	//# the dimensions of the box along the positive <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@BoxVolume::SetBoxSize@$


	//# \function	BoxVolume::SetBoxSize		Sets the box size.
	//
	//# \proto	void SetBoxSize(const Vector3D& size);
	//
	//# \param	size	The new box size.
	//
	//# \desc
	//# The $SetBoxSize$ function sets the physical dimensions of the box volume to those specified
	//# by the $size$ parameter. One corner of the box coincides with the origin, and the three components
	//# of the size correspond to the dimensions of the box along the positive <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@BoxVolume::GetBoxSize@$


	class BoxVolume : public Volume
	{
		private:

			Vector3D	boxSize;

		protected:

			BoxVolume() {}

			BoxVolume(const Vector3D& size)
			{
				boxSize = size;
			}

		public:

			~BoxVolume() = default;

			const Vector3D& GetBoxSize(void) const
			{
				return (boxSize);
			}

			void SetBoxSize(const Vector3D& size)
			{
				boxSize = size;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	PyramidVolume		Encapsulates data pertaining to a generic pyramid volume.
	//
	//# The $PyramidVolume$ class encapsulates data pertaining to a generic pyramid volume.
	//
	//# \def	class PyramidVolume : public Volume
	//
	//# \ctor	PyramidVolume(const Vector2D& size, float height);
	//
	//# The constructor has protected access. The $PyramidVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the pyramid base.
	//# \param	height	The height of the pyramid.
	//
	//# \desc
	//# The $PyramidVolume$ class encapsulates the physical dimensions of a generic pyramidal volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# One corner of the pyramid coincides with the origin, and the two components of the $size$ parameter
	//# correspond to the dimensions of the pyramid's base along the positive <i>x</i> and <i>y</i> axes.
	//# The $height$ parameter specifies the height of the pyramid's apex along the positive <i>z</i> axis.
	//
	//# \privbase	Volume		A $PyramidVolume$ is a generic volume.


	//# \function	PyramidVolume::GetPyramidSize		Returns the pyramid base size.
	//
	//# \proto	const Vector2D& GetPyramidSize(void) const;
	//
	//# \desc
	//# The $GetPyramidSize$ function returns a 2D vector containing the physical dimensions of the base
	//# of the pyramid volume. One corner of the pyramid coincides with the origin, and the two components
	//# of the $size$ parameter correspond to the dimensions of the pyramid's base along the positive
	//# <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@PyramidVolume::SetPyramidSize@$
	//# \also	$@PyramidVolume::GetPyramidHeight@$
	//# \also	$@PyramidVolume::SetPyramidHeight@$


	//# \function	PyramidVolume::SetPyramidSize		Sets the pyramid base size.
	//
	//# \proto	void SetPyramidSize(const Vector2D& size);
	//
	//# \param	size	The new pyramid base size.
	//
	//# \desc
	//# The $SetPyramidSize$ function sets the physical dimensions of the base of the pyramid volume to
	//# those specified by the $size$ parameter. One corner of the pyramid coincides with the origin, and
	//# the two components of the $size$ parameter correspond to the dimensions of the pyramid's base along
	//# the positive <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@PyramidVolume::GetPyramidSize@$
	//# \also	$@PyramidVolume::GetPyramidHeight@$
	//# \also	$@PyramidVolume::SetPyramidHeight@$


	//# \function	PyramidVolume::GetPyramidHeight		Returns the pyramid height.
	//
	//# \proto	const float& GetPyramidHeight(void) const;
	//
	//# \desc
	//# The $GetPyramidHeight$ function returns the physical height of the pyramid volume along the
	//# positive <i>z</i> axis.
	//
	//# \also	$@PyramidVolume::SetPyramidHeight@$
	//# \also	$@PyramidVolume::GetPyramidSize@$
	//# \also	$@PyramidVolume::SetPyramidSize@$


	//# \function	PyramidVolume::SetPyramidHeight		Sets the pyramid height.
	//
	//# \proto	void SetPyramidHeight(float height);
	//
	//# \param	height	The new pyramid height.
	//
	//# \desc
	//# The $SetPyramidHeight$ function sets the physical height of the pyramid volume along the positive
	//# <i>z</i> axis to that specified by the $height$ parameter.
	//
	//# \also	$@PyramidVolume::GetPyramidHeight@$
	//# \also	$@PyramidVolume::GetPyramidSize@$
	//# \also	$@PyramidVolume::SetPyramidSize@$


	class PyramidVolume : public Volume
	{
		private:

			Vector2D	pyramidSize;
			float		pyramidHeight;

		protected:

			PyramidVolume() {}

			PyramidVolume(const Vector2D& size, float height)
			{
				pyramidSize = size;
				pyramidHeight = height;
			}

		public:

			~PyramidVolume() = default;

			const Vector2D& GetPyramidSize(void) const
			{
				return (pyramidSize);
			}

			void SetPyramidSize(const Vector2D& size)
			{
				pyramidSize = size;
			}

			const float& GetPyramidHeight(void) const
			{
				return (pyramidHeight);
			}

			void SetPyramidHeight(float height)
			{
				pyramidHeight = height;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	CylinderVolume		Encapsulates data pertaining to a generic cylinder volume.
	//
	//# The $CylinderVolume$ class encapsulates data pertaining to a generic cylinder volume.
	//
	//# \def	class CylinderVolume : public Volume
	//
	//# \ctor	CylinderVolume(const Vector2D& size, float height);
	//
	//# The constructor has protected access. The $CylinderVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the cylinder base.
	//# \param	height	The height of the cylinder.
	//
	//# \desc
	//# The $CylinderVolume$ class encapsulates the physical dimensions of a generic cylindrical volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The center of the base coincides with the origin, and the two components of the $size$ parameter
	//# correspond to the radii along the <i>x</i> and <i>y</i> axes. The $height$ parameter specifies the
	//# physical height of the cylinder along the positive <i>z</i> axis.
	//
	//# \privbase	Volume		A $CylinderVolume$ is a generic volume.


	//# \function	CylinderVolume::GetCylinderSize		Returns the cylinder base size.
	//
	//# \proto	const Vector2D& GetCylinderSize(void) const;
	//
	//# \desc
	//# The $GetCylinderSize$ function returns a 2D vector containing the physical dimensions of the base
	//# of the cylinder volume. The center of the base coincides with the origin, and the two components of
	//# the size correspond to the radii along the <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@CylinderVolume::SetCylinderSize@$
	//# \also	$@CylinderVolume::GetCylinderHeight@$
	//# \also	$@CylinderVolume::SetCylinderHeight@$


	//# \function	CylinderVolume::SetCylinderSize		Sets the cylinder base size.
	//
	//# \proto	void SetCylinderSize(const Vector2D& size);
	//
	//# \param	size	The new cylinder base size.
	//
	//# \desc
	//# The $SetCylinderSize$ function sets the physical dimensions of the base of the cylinder volume to
	//# those specified by the $size$ parameter. The center of the base coincides with the origin, and the
	//# two components of the size correspond to the radii along the <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@CylinderVolume::GetCylinderSize@$
	//# \also	$@CylinderVolume::GetCylinderHeight@$
	//# \also	$@CylinderVolume::SetCylinderHeight@$


	//# \function	CylinderVolume::GetCylinderHeight	Returns the cylinder height.
	//
	//# \proto	const float& GetCylinderHeight(void) const;
	//
	//# \desc
	//# The $GetCylinderHeight$ function returns the physical height of the cylinder volume along the
	//# positive <i>z</i> axis.
	//
	//# \also	$@CylinderVolume::SetCylinderHeight@$
	//# \also	$@CylinderVolume::GetCylinderSize@$
	//# \also	$@CylinderVolume::SetCylinderSize@$


	//# \function	CylinderVolume::SetCylinderHeight	Sets the cylinder height.
	//
	//# \proto	void SetCylinderHeight(float height);
	//
	//# \param	height	The new cylinder height.
	//
	//# \desc
	//# The $SetCylinderHeight$ function sets the physical height of the cylinder volume along the positive
	//# <i>z</i> axis to that specified by the $height$ parameter.
	//
	//# \also	$@CylinderVolume::GetCylinderHeight@$
	//# \also	$@CylinderVolume::GetCylinderSize@$
	//# \also	$@CylinderVolume::SetCylinderSize@$


	class CylinderVolume : public Volume
	{
		private:

			Vector2D	cylinderSize;
			float		cylinderHeight;

		protected:

			CylinderVolume() {}

			CylinderVolume(const Vector2D& size, float height)
			{
				cylinderSize = size;
				cylinderHeight = height;
			}

		public:

			~CylinderVolume() = default;

			const Vector2D& GetCylinderSize(void) const
			{
				return (cylinderSize);
			}

			void SetCylinderSize(const Vector2D& size)
			{
				cylinderSize = size;
			}

			const float& GetCylinderHeight(void) const
			{
				return (cylinderHeight);
			}

			void SetCylinderHeight(float height)
			{
				cylinderHeight = height;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	ConeVolume		Encapsulates data pertaining to a generic cone volume.
	//
	//# The $ConeVolume$ class encapsulates data pertaining to a generic cone volume.
	//
	//# \def	class ConeVolume : public Volume
	//
	//# \ctor	ConeVolume(const Vector2D& size, float height);
	//
	//# The constructor has protected access. The $ConeVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the cone base.
	//# \param	height	The height of the cone.
	//
	//# \desc
	//# The $ConeVolume$ class encapsulates the physical dimensions of a generic conical volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The center of the base coincides with the origin, and the two components of the $size$ parameter
	//# correspond to the radii along the <i>x</i> and <i>y</i> axes. The $height$ parameter specifies the
	//# physical height of the cone along the positive <i>z</i> axis.
	//
	//# \privbase	Volume		A $ConeVolume$ is a generic volume.


	//# \function	ConeVolume::GetConeSize		Returns the cone base size.
	//
	//# \proto	const Vector2D& GetConeSize(void) const;
	//
	//# \desc
	//# The $GetConeSize$ function returns a 2D vector containing the physical dimensions of the base
	//# of the cone volume. The center of the base coincides with the origin, and the two components of
	//# the size correspond to the radii along the <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@ConeVolume::SetConeSize@$
	//# \also	$@ConeVolume::GetConeHeight@$
	//# \also	$@ConeVolume::SetConeHeight@$


	//# \function	ConeVolume::SetConeSize		Sets the cone base size.
	//
	//# \proto	void SetConeSize(const Vector2D& size);
	//
	//# \param	size	The new cone base size.
	//
	//# \desc
	//# The $SetConeSize$ function sets the physical dimensions of the base of the cone volume to
	//# those specified by the $size$ parameter. The center of the base coincides with the origin, and the
	//# two components of the size correspond to the radii along the <i>x</i> and <i>y</i> axes.
	//
	//# \also	$@ConeVolume::GetConeSize@$
	//# \also	$@ConeVolume::GetConeHeight@$
	//# \also	$@ConeVolume::SetConeHeight@$


	//# \function	ConeVolume::GetConeHeight	Returns the cone height.
	//
	//# \proto	const float& GetConeHeight(void) const;
	//
	//# \desc
	//# The $GetConeHeight$ function returns the physical height of the cone volume along the
	//# positive <i>z</i> axis.
	//
	//# \also	$@ConeVolume::SetConeHeight@$
	//# \also	$@ConeVolume::GetConeSize@$
	//# \also	$@ConeVolume::SetConeSize@$


	//# \function	ConeVolume::SetConeHeight	Sets the cone height.
	//
	//# \proto	void SetConeHeight(float height);
	//
	//# \param	height	The new cone height.
	//
	//# \desc
	//# The $SetConeHeight$ function sets the physical height of the cone volume along the positive
	//# <i>z</i> axis to that specified by the $height$ parameter.
	//
	//# \also	$@ConeVolume::GetConeHeight@$
	//# \also	$@ConeVolume::GetConeSize@$
	//# \also	$@ConeVolume::SetConeSize@$


	class ConeVolume : public Volume
	{
		private:

			Vector2D	coneSize;
			float		coneHeight;

		protected:

			ConeVolume() {}

			ConeVolume(const Vector2D& size, float height)
			{
				coneSize = size;
				coneHeight = height;
			}

		public:

			~ConeVolume() = default;

			const Vector2D& GetConeSize(void) const
			{
				return (coneSize);
			}

			void SetConeSize(const Vector2D& size)
			{
				coneSize = size;
			}

			const float& GetConeHeight(void) const
			{
				return (coneHeight);
			}

			void SetConeHeight(float height)
			{
				coneHeight = height;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	SphereVolume	Encapsulates data pertaining to a generic sphere volume.
	//
	//# The $SphereVolume$ class encapsulates data pertaining to a generic sphere volume.
	//
	//# \def	class SphereVolume : public Volume
	//
	//# \ctor	SphereVolume(const Vector3D& size);
	//
	//# The constructor has protected access. The $SphereVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the sphere.
	//
	//# \desc
	//# The $SphereVolume$ class encapsulates the physical dimensions of a generic spherical volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The center of the sphere coincides with the origin, and the three components of the $size$ parameter
	//# correspond to the radii along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \privbase	Volume		A $SphereVolume$ is a generic volume.


	//# \function	SphereVolume::GetSphereSize		Returns the sphere size.
	//
	//# \proto	const Vector3D& GetSphereSize(void) const;
	//
	//# \desc
	//# The $GetSphereSize$ function returns a 3D vector containing the physical dimensions of the sphere
	//# volume. The center of the sphere coincides with the origin, and the three components of the size
	//# correspond to the radii along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@SphereVolume::SetSphereSize@$


	//# \function	SphereVolume::SetSphereSize		Sets the sphere size.
	//
	//# \proto	void SetSphereSize(const Vector3D& size);
	//
	//# \param	size	The new sphere size.
	//
	//# \desc
	//# The $SetSphereSize$ function sets the physical dimensions of the sphere volume to those specified
	//# by the $size$ parameter. The center of the sphere coincides with the origin, and the three components
	//# of the size correspond to the radii along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@SphereVolume::GetSphereSize@$


	class SphereVolume : public Volume
	{
		private:

			Vector3D	sphereSize;

		protected:

			SphereVolume() {}

			SphereVolume(const Vector3D& size)
			{
				sphereSize = size;
			}

		public:

			~SphereVolume() = default;

			const Vector3D& GetSphereSize(void) const
			{
				return (sphereSize);
			}

			void SetSphereSize(const Vector3D& size)
			{
				sphereSize = size;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	DomeVolume		Encapsulates data pertaining to a generic dome volume.
	//
	//# The $DomeVolume$ class encapsulates data pertaining to a generic dome volume.
	//
	//# \def	class DomeVolume : public Volume
	//
	//# \ctor	DomeVolume(const Vector3D& size);
	//
	//# The constructor has protected access. The $DomeVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the dome.
	//
	//# \desc
	//# The $DomeVolume$ class encapsulates the physical dimensions of a generic dome volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The center of the base coincides with the origin, and the three components of the $size$ parameter
	//# correspond to the radii along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \privbase	Volume		A $DomeVolume$ is a generic volume.


	//# \function	DomeVolume::GetDomeSize		Returns the dome size.
	//
	//# \proto	const Vector3D& GetDomeSize(void) const;
	//
	//# \desc
	//# The $GetDomeSize$ function returns a 3D vector containing the physical dimensions of the dome
	//# volume. The center of the base coincides with the origin, and the three components of the size
	//# correspond to the radii along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@DomeVolume::SetDomeSize@$


	//# \function	DomeVolume::SetDomeSize		Sets the dome size.
	//
	//# \proto	void SetDomeSize(const Vector3D& size);
	//
	//# \param	size	The new dome size.
	//
	//# \desc
	//# The $SetDomeSize$ function sets the physical dimensions of the dome volume to those specified
	//# by the $size$ parameter. The center of the base coincides with the origin, and the three components
	//# of the size correspond to the radii along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@DomeVolume::GetDomeSize@$


	class DomeVolume : public Volume
	{
		private:

			Vector3D	domeSize;

		protected:

			DomeVolume() {}

			DomeVolume(const Vector3D& size)
			{
				domeSize = size;
			}

			void SetDomeSizeXY(const Vector2D& size)
			{
				domeSize.x = size.x;
				domeSize.y = size.y;
			}

			void SetDomeSizeZ(float z)
			{
				domeSize.z = z;
			}

		public:

			~DomeVolume() = default;

			const Vector3D& GetDomeSize(void) const
			{
				return (domeSize);
			}

			void SetDomeSize(const Vector3D& size)
			{
				domeSize = size;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	CapsuleVolume		Encapsulates data pertaining to a generic capsule volume.
	//
	//# The $CapsuleVolume$ class encapsulates data pertaining to a generic capsule volume.
	//
	//# \def	class CapsuleVolume : public Volume
	//
	//# \ctor	CapsuleVolume(const Vector3D& size, float height);
	//
	//# The constructor has protected access. The $CapsuleVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the hemispherical caps.
	//# \param	height	The height of the cylindrical interior.
	//
	//# \desc
	//# The $CapsuleVolume$ class encapsulates the physical dimensions of a generic capsule volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The center of the base of the interior cylinder coincides with the origin, and the three components of the
	//# $size$ parameter correspond to the radii of the hemispherical caps along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//# The $height$ parameter specifies the physical height of the cylindrical interior along the positive <i>z</i> axis.
	//
	//# \privbase	Volume		A $CapsuleVolume$ is a generic volume.


	//# \function	CapsuleVolume::GetCapsuleSize		Returns the hemispherical cap size.
	//
	//# \proto	const Vector3D& GetCapsuleSize(void) const;
	//
	//# \desc
	//# The $GetCapsuleSize$ function returns a 3D vector containing the physical dimensions of the hemispherical
	//# caps of the capsule volume. The center of the base of the interior cylinder coincides with the origin, and the
	//# three components of the size correspond to the radii of the hemispherical caps along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@CapsuleVolume::SetCapsuleSize@$
	//# \also	$@CapsuleVolume::GetCapsuleHeight@$
	//# \also	$@CapsuleVolume::SetCapsuleHeight@$


	//# \function	CapsuleVolume::SetCapsuleSize		Sets the hemispherical cap size.
	//
	//# \proto	void SetCapsuleSize(const Vector3D& size);
	//
	//# \param	size	The new hemispherical cap size.
	//
	//# \desc
	//# The $SetCapsuleSize$ function sets the physical dimensions of the hemispherical caps of the capsule volume to
	//# those specified by the $size$ parameter. The center of the base of the interior cylinder coincides with the origin, and the
	//# three components of the size correspond to the radii of the hemispherical caps along the <i>x</i>, <i>y</i>, and <i>z</i> axes.
	//
	//# \also	$@CapsuleVolume::GetCapsuleSize@$
	//# \also	$@CapsuleVolume::GetCapsuleHeight@$
	//# \also	$@CapsuleVolume::SetCapsuleHeight@$


	//# \function	CapsuleVolume::GetCapsuleHeight		Returns the height of the cylindrical interior.
	//
	//# \proto	const float& GetCapsuleHeight(void) const;
	//
	//# \desc
	//# The $GetCapsuleHeight$ function returns the physical height of the cylindrical interior of the capsule volume along the
	//# positive <i>z</i> axis.
	//
	//# \also	$@CapsuleVolume::SetCapsuleHeight@$
	//# \also	$@CapsuleVolume::GetCapsuleSize@$
	//# \also	$@CapsuleVolume::SetCapsuleSize@$


	//# \function	CapsuleVolume::SetCapsuleHeight		Sets the height of the cylindrical interior.
	//
	//# \proto	void SetCapsuleHeight(float height);
	//
	//# \param	height	The new cylinder height.
	//
	//# \desc
	//# The $SetCapsuleHeight$ function sets the physical height of the cylindrical interior of the capsule volume along the
	//# positive <i>z</i> axis to that specified by the $height$ parameter.
	//
	//# \also	$@CapsuleVolume::GetCapsuleHeight@$
	//# \also	$@CapsuleVolume::GetCapsuleSize@$
	//# \also	$@CapsuleVolume::SetCapsuleSize@$


	class CapsuleVolume : public Volume
	{
		private:

			Vector3D	capsuleSize;
			float		capsuleHeight;

		protected:

			CapsuleVolume() {}

			CapsuleVolume(const Vector3D& size, float height)
			{
				capsuleSize = size;
				capsuleHeight = height;
			}

		public:

			~CapsuleVolume() = default;

			const Vector3D& GetCapsuleSize(void) const
			{
				return (capsuleSize);
			}

			void SetCapsuleSize(const Vector3D& size)
			{
				capsuleSize = size;
			}

			const float& GetCapsuleHeight(void) const
			{
				return (capsuleHeight);
			}

			void SetCapsuleHeight(float height)
			{
				capsuleHeight = height;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			float GetVolumeLength(void) const override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	TruncatedPyramidVolume		Encapsulates data pertaining to a generic pyramid volume.
	//
	//# The $TruncatedPyramidVolume$ class encapsulates data pertaining to a generic pyramid volume.
	//
	//# \def	class TruncatedPyramidVolume : public PyramidVolume
	//
	//# \ctor	TruncatedPyramidVolume(const Vector2D& size, float height, float ratio);
	//
	//# The constructor has protected access. The $TruncatedPyramidVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the pyramid base.
	//# \param	height	The height of the truncated pyramid, measuring the distance between the top and bottom faces.
	//# \param	ratio	The ratio of the dimensions of the top face to the dimensions of the bottom face.
	//
	//# \desc
	//# The $TruncatedPyramidVolume$ class encapsulates the physical dimensions of a generic truncated pyramidal volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# One corner of the truncated pyramid coincides with the origin, and the two components of the $size$ parameter
	//# correspond to the dimensions of the pyramid's base along the positive <i>x</i> and <i>y</i> axes.
	//# The $height$ parameter specifies the distance from the bottom face (the base) to the top face along the positive <i>z</i> axis.
	//# The $ratio$ parameter specifies the ratio of the dimensions of the top face to the dimensions of the bottom face
	//# and should be in the range (0,1).
	//
	//# \privbase	PyramidVolume		A $TruncatedPyramidVolume$ is an extension of a pyramid volume.


	//# \function	TruncatedPyramidVolume::GetPyramidRatio		Returns the truncated pyramid ratio.
	//
	//# \proto	const float& GetPyramidRatio(void) const;
	//
	//# \desc
	//# The $GetPyramidRatio$ function returns the ratio between the dimensions of the top face to the dimensions of the bottom face
	//# (the base) of the truncated pyramid.
	//
	//# \also	$@TruncatedPyramidVolume::SetPyramidRatio@$


	//# \function	TruncatedPyramidVolume::SetPyramidRatio		Sets the truncated pyramid ratio.
	//
	//# \proto	void SetPyramidRatio(float ratio);
	//
	//# \param	ratio	The new truncated pyramid ratio.
	//
	//# \desc
	//# The $SetPyramidRatio$ function sets the ratio between the dimensions of the top face to the dimensions of the bottom face
	//# (the base) of the truncated pyramid to that specified by the $ratio$ parameter.
	//
	//# \also	$@TruncatedPyramidVolume::GetPyramidRatio@$


	class TruncatedPyramidVolume : public PyramidVolume
	{
		private:

			float		pyramidRatio;

		protected:

			TruncatedPyramidVolume() {}

			TruncatedPyramidVolume(const Vector2D& size, float height, float ratio) : PyramidVolume(size, height)
			{
				pyramidRatio = ratio;
			}

		public:

			~TruncatedPyramidVolume() = default;

			const float& GetPyramidRatio(void) const
			{
				return (pyramidRatio);
			}

			void SetPyramidRatio(float ratio)
			{
				pyramidRatio = ratio;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	TruncatedConeVolume		Encapsulates data pertaining to a generic cone volume.
	//
	//# The $TruncatedConeVolume$ class encapsulates data pertaining to a generic cone volume.
	//
	//# \def	class TruncatedConeVolume : public ConeVolume
	//
	//# \ctor	TruncatedConeVolume(const Vector2D& size, float height, float ratio);
	//
	//# The constructor has protected access. The $TruncatedConeVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the cone base.
	//# \param	height	The height of the truncated cone, measuring the distance between the top and bottom faces.
	//# \param	ratio	The ratio of the dimensions of the top face to the dimensions of the bottom face.
	//
	//# \desc
	//# The $TruncatedConeVolume$ class encapsulates the physical dimensions of a generic truncated conical volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The center of the base coincides with the origin, and the two components of the $size$ parameter
	//# correspond to the radii along the <i>x</i> and <i>y</i> axes. The $height$ parameter specifies the distance
	//# from the bottom face (the base) to the top face along the positive <i>z</i> axis. The $ratio$ parameter specifies
	//# the ratio of the dimensions of the top face to the dimensions of the bottom face and should be in the range (0,1).
	//
	//# \privbase	ConeVolume		A $TruncatedConeVolume$ is an extension of a cone volume.


	//# \function	TruncatedConeVolume::GetConeRatio		Returns the truncated cone ratio.
	//
	//# \proto	const float& GetConeRatio(void) const;
	//
	//# \desc
	//# The $GetConeRatio$ function returns the ratio between the dimensions of the top face to the dimensions of the bottom face
	//# (the base) of the truncated cone.
	//
	//# \also	$@TruncatedConeVolume::SetConeRatio@$


	//# \function	TruncatedConeVolume::SetConeRatio		Sets the truncated cone ratio.
	//
	//# \proto	void SetConeRatio(float ratio);
	//
	//# \param	ratio	The new truncated cone ratio.
	//
	//# \desc
	//# The $SetConeRatio$ function sets the ratio between the dimensions of the top face to the dimensions of the bottom face
	//# (the base) of the truncated cone to that specified by the $ratio$ parameter.
	//
	//# \also	$@TruncatedConeVolume::GetConeRatio@$


	class TruncatedConeVolume : public ConeVolume
	{
		private:

			float		coneRatio;

		protected:

			TruncatedConeVolume() {}

			TruncatedConeVolume(const Vector2D& size, float height, float ratio) : ConeVolume(size, height)
			{
				coneRatio = ratio;
			}

		public:

			~TruncatedConeVolume() = default;

			const float& GetConeRatio(void) const
			{
				return (coneRatio);
			}

			void SetConeRatio(float ratio)
			{
				coneRatio = ratio;
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};


	//# \class	TruncatedDomeVolume		Encapsulates data pertaining to a generic dome volume.
	//
	//# The $TruncatedDomeVolume$ class encapsulates data pertaining to a generic dome volume.
	//
	//# \def	class TruncatedDomeVolume : public DomeVolume
	//
	//# \ctor	TruncatedDomeVolume(const Vector2D& size, float height, float ratio);
	//
	//# The constructor has protected access. The $TruncatedDomeVolume$ class can only exist as a base class for another class.
	//
	//# \param	size	The size of the dome base.
	//# \param	height	The height of the truncated dome, measuring the distance between the top and bottom faces.
	//# \param	ratio	The ratio of the dimensions of the top face to the dimensions of the bottom face.
	//
	//# \desc
	//# The $TruncatedDomeVolume$ class encapsulates the physical dimensions of a generic truncated dome volume
	//# and functionality that is used by various subclasses throughout the engine.
	//#
	//# The center of the base coincides with the origin, and the two components of the $size$ parameter
	//# correspond to the radii along the <i>x</i> and <i>y</i> axes. The $height$ parameter specifies the distance
	//# from the bottom face (the base) to the top face along the positive <i>z</i> axis. The $ratio$ parameter specifies
	//# the ratio of the dimensions of the top face to the dimensions of the bottom face and should be in the range (0,1).
	//
	//# \privbase	DomeVolume		A $TruncatedDomeVolume$ is an extension of a dome volume.


	//# \function	TruncatedDomeVolume::GetDomeHeight		Returns the truncated dome height.
	//
	//# \proto	const float& GetDomeHeight(void) const;
	//
	//# \desc
	//# The $GetDomeHeight$ function returns the height of the truncated dome, measuring the distance between the top and bottom faces.
	//#
	//# The <i>z</i> coordinate of the size returned by the $@DomeVolume::GetDomeSize@$ function is equal to the semiaxis length of the
	//# dome as if it were not truncated. This size is equal to <i>h</i>&nbsp;/&nbsp;sqrt(1&nbsp;&minus;&nbsp;<i>r</i><sup>2</sup>), where
	//# <i>h</i> is the truncated dome height, and <i>r</i> is the ratio between the dimensions of the top face to the dimensions of the
	//# bottom face (the base) of the truncated dome.
	//
	//# \also	$@TruncatedDomeVolume::SetDomeHeight@$


	//# \function	TruncatedDomeVolume::SetDomeHeight		Sets the truncated dome height.
	//
	//# \proto	void SetDomeHeight(float height);
	//
	//# \param	height		The new truncated dome height.
	//
	//# \desc
	//# The $SetDomeHeight$ function sets the height of the truncated dome, measuring the distance between the top and bottom faces,
	//# to that specified by the $height$ parameter.
	//
	//# \also	$@TruncatedDomeVolume::GetDomeHeight@$


	//# \function	TruncatedDomeVolume::GetDomeRatio		Returns the truncated dome ratio.
	//
	//# \proto	const float& GetDomeRatio(void) const;
	//
	//# \desc
	//# The $GetDomeRatio$ function returns the ratio between the dimensions of the top face to the dimensions of the bottom face
	//# (the base) of the truncated dome.
	//
	//# \also	$@TruncatedDomeVolume::SetDomeRatio@$


	//# \function	TruncatedDomeVolume::SetDomeRatio		Sets the truncated dome ratio.
	//
	//# \proto	void SetDomeRatio(float ratio);
	//
	//# \param	ratio	The new truncated dome ratio.
	//
	//# \desc
	//# The $SetDomeRatio$ function sets the ratio between the dimensions of the top face to the dimensions of the bottom face
	//# (the base) of the truncated dome to that specified by the $ratio$ parameter.
	//
	//# \also	$@TruncatedDomeVolume::GetDomeRatio@$


	class TruncatedDomeVolume : public DomeVolume
	{
		private:

			float		domeHeight;
			float		domeRatio;

		protected:

			TruncatedDomeVolume() {}

			TruncatedDomeVolume(const Vector2D& size, float height, float ratio) : DomeVolume(Vector3D(size.x, size.y, height * InverseSqrt(1.0F - ratio * ratio)))
			{
				domeHeight = height;
				domeRatio = ratio;
			}

		public:

			~TruncatedDomeVolume() = default;

			void SetDomeSize(const Vector2D& size)
			{
				SetDomeSizeXY(size);
			}

			const float& GetDomeHeight(void) const
			{
				return (domeHeight);
			}

			void SetDomeHeight(float height)
			{
				domeHeight = height;
				SetDomeSizeZ(height * InverseSqrt(1.0F - domeRatio * domeRatio));
			}

			const float& GetDomeRatio(void) const
			{
				return (domeRatio);
			}

			void SetDomeRatio(float ratio)
			{
				domeRatio = ratio;
				SetDomeSizeZ(domeHeight * InverseSqrt(1.0F - ratio * ratio));
			}

			void PackVolume(Packer& data, unsigned_int32 packFlags) const override;
			void UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetVolumeObjectSize(float *size) const override;
			void SetVolumeObjectSize(const float *size) override;

			void CalculateBoundingBox(Box3D *box) const override;
			void CalculateBoundingSphere(BoundingSphere *sphere) const override;

			bool ExteriorSphere(const Point3D& center, float radius) const override;
			bool InteriorPoint(const Point3D& p) const override;
	};
}


#endif

// ZYUQURM
