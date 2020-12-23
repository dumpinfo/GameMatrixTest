 

#ifndef C4Spaces_h
#define C4Spaces_h


//# \component	World Manager
//# \prefix		WorldMgr/

//# \import		C4SpaceObjects.h


#include "C4SpaceObjects.h"
#include "C4Node.h"


namespace C4
{
	struct Line;
	class SoundRoom;
	class CameraRegion;
	class FrustumCamera;
	class OrthoCamera;


	//# \class	Space	Represents a space node in a world.
	//
	//# The $Space$ class represents a space node in a world.
	//
	//# \def	class Space : public Node
	//
	//# \ctor	Space(SpaceType type);
	//
	//# The constructor has protected access. A $Space$ class can only exist as the base class for a more specific type of space.
	//
	//# \param	type	The type of the space. See below for a list of possible types.
	//
	//# \desc
	//# The $Space$ class is the base class for all space nodes. The $type$ parameter can be one of the following
	//# constants, and it is specified by the subclass's constructor.
	//
	//# \table	SpaceType
	//
	//# \base	Node	A $Space$ node is a scene graph node.
	//
	//# \also	$@SpaceObject@$
	//
	//# \wiki	Spaces


	//# \function	Space::GetSpaceType		Returns the specific type of a space.
	//
	//# \proto	SpaceType GetSpaceType(void) const;
	//
	//# \desc
	//# The $GetSpaceType$ function returns the specific space type, which can be one of the following constants.
	//
	//# \table	SpaceType


	class Space : public Node
	{
		friend class Node;

		private:

			SpaceType		spaceType;

			static Space *Create(Unpacker& data, unsigned_int32 unpackFlags);

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		protected:

			Space(SpaceType type);
			Space(const Space& space);

		public:

			virtual ~Space();

			SpaceType GetSpaceType(void) const
			{
				return (spaceType);
			}

			SpaceObject *GetObject(void) const
			{
				return (static_cast<SpaceObject *>(Node::GetObject()));
			}

			void PackType(Packer& data) const override;
	};


	//# \class	FogSpace		Represents a fog space node in a world.
	//
	//# The $FogSpace$ class represents a fog space node in a world.
	//
	//# \def	class FogSpace final : public Space, public ListElement<FogSpace>
	//
	//# \ctor	FogSpace(const Vector2D& size); 
	//
	//# \param	size	The size of the plane.
	// 
	//# \desc
	//# 
	//
	//# \base	Space								A fog space is a specific type of space.
	//# \base	Utilities/ListElement<FogSpace>		Used internally by the World Manager. 
	//
	//# \also	$@FogSpaceObject@$ 
 

	class FogSpace final : public Space, public ListElement<FogSpace>
	{
		friend class Space; 

		private:

			Point3D			worldVertex[4];

			FogSpace();
			FogSpace(const FogSpace& fogSpace);

			Node *Replicate(void) const override;

			static bool FogVisible(const Node *node, const VisibilityRegion *region);
			static bool FogVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

			void HandleTransformUpdate(void) override;

		public:

			C4API FogSpace(const Vector2D& size);
			C4API ~FogSpace();

			using ListElement<FogSpace>::Previous;
			using ListElement<FogSpace>::Next;

			FogSpaceObject *GetObject(void) const
			{
				return (static_cast<FogSpaceObject *>(Node::GetObject()));
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void Preprocess(void) override;
			void Neutralize(void) override;
	};


	//# \class	ShadowSpace		Represents a shadow space node in a world.
	//
	//# The $ShadowSpace$ class represents a shadow space node in a world.
	//
	//# \def	class ShadowSpace final : public Space
	//
	//# \ctor	ShadowSpace(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//#
	//
	//# \base	Space		A shadow space is a specific type of space.
	//
	//# \also	$@ShadowSpaceObject@$


	class ShadowSpace final : public Space
	{
		friend class Space;

		private:

			ShadowSpace();
			ShadowSpace(const ShadowSpace& shadowSpace);

			Node *Replicate(void) const override;

			static bool ClipSegmentToPlanes(Point3D& p1, Point3D& p2, int32 planeCount, const Antivector4D *plane);

		public:

			C4API ShadowSpace(const Vector3D& size);
			C4API ~ShadowSpace();

			ShadowSpaceObject *GetObject(void) const
			{
				return (static_cast<ShadowSpaceObject *>(Node::GetObject()));
			}

			int32 ClipToShadowBounds(int32 planeCount, const Antivector4D *plane, Point3D *vertex, Line *line, int32 baseIndex) const;
	};


	//# \class	RadiositySpace		Represents a radiosity space node in a world.
	//
	//# The $RadiositySpace$ class represents a radiosity space node in a world.
	//
	//# \def	class RadiositySpace final : public Space
	//
	//# \ctor	RadiositySpace(const Vector3D& size, const Integer3D& resolution);
	//
	//# \param	size			The size of the box bounding the radiosity space.
	//# \param	resolution		The resolution of the volumetric lighting texture.
	//
	//# \desc
	//#
	//
	//# \base	Space		A radiosity space is a specific type of space.
	//
	//# \also	$@RadiositySpaceObject@$


	class RadiositySpace final : public Space
	{
		friend class Space;

		private:

			RadiositySpace();
			RadiositySpace(const RadiositySpace& radiositySpace);

			Node *Replicate(void) const override;

		public:

			C4API RadiositySpace(const Vector3D& size, const Integer3D& resolution);
			C4API ~RadiositySpace();

			RadiositySpaceObject *GetObject(void) const
			{
				return (static_cast<RadiositySpaceObject *>(Node::GetObject()));
			}
	};


	//# \class	AcousticsSpace		Represents an acoustic space node in a world.
	//
	//# The $AcousticsSpace$ class represents an acoustic space node in a world.
	//
	//# \def	class AcousticsSpace final : public Space
	//
	//# \ctor	AcousticsSpace(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//#
	//
	//# \base	Space		An acoustic space is a specific type of space.
	//
	//# \also	$@AcousticsSpaceObject@$


	class AcousticsSpace final : public Space
	{
		friend class Space;

		private:

			SoundRoom	*soundRoom;

			AcousticsSpace();
			AcousticsSpace(const AcousticsSpace& acousticsSpace);

			Node *Replicate(void) const override;

		public:

			C4API AcousticsSpace(const Vector3D& size);
			C4API ~AcousticsSpace();

			AcousticsSpaceObject *GetObject(void) const
			{
				return (static_cast<AcousticsSpaceObject *>(Node::GetObject()));
			}

			SoundRoom *GetSoundRoom(void) const
			{
				return (soundRoom);
			}

			void Preprocess(void) override;
			void Neutralize(void) override;
	};


	//# \class	OcclusionSpace		Represents an occlusion space node in a world.
	//
	//# The $OcclusionSpace$ class represents an occlusion space node in a world.
	//
	//# \def	class OcclusionSpace final : public Space, public ListElement<OcclusionSpace>
	//
	//# \ctor	OcclusionSpace(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//#
	//
	//# \base	Space									An occlusion space is a specific type of space.
	//# \base	Utilities/ListElement<OcclusionSpace>	Used internally by the World Manager.
	//
	//# \also	$@OcclusionSpaceObject@$


	class OcclusionSpace final : public Space, public ListElement<OcclusionSpace>
	{
		friend class Space;

		private:

			Point3D		worldCenter;
			Vector3D	worldAxis[3];

			OcclusionSpace();
			OcclusionSpace(const OcclusionSpace& occlusionSpace);

			Node *Replicate(void) const override;

			void HandleTransformUpdate(void) override;

			static bool BoxVisible(const Node *node, const VisibilityRegion *region);
			static bool BoxVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

		public:

			C4API OcclusionSpace(const Vector3D& size);
			C4API ~OcclusionSpace();

			using ListElement<OcclusionSpace>::Previous;
			using ListElement<OcclusionSpace>::Next;

			OcclusionSpaceObject *GetObject(void) const
			{
				return (static_cast<OcclusionSpaceObject *>(Node::GetObject()));
			}

			void Preprocess(void) override;
			void Neutralize(void) override;

			OcclusionRegion *NewFrustumOcclusionRegion(const FrustumCamera *camera) const;
			OcclusionRegion *NewOrthoOcclusionRegion(const OrthoCamera *camera) const;
	};


	//# \class	PaintSpace		Represents a paint space node in a world.
	//
	//# The $PaintSpace$ class represents a paint space node in a world.
	//
	//# \def	class PaintSpace final : public Space
	//
	//# \ctor	PaintSpace(const Vector3D& size, const Integer2D& resolution);
	//
	//# \param	size		The size of the box.
	//# \param	resolution	The resolution of the paint texture. This must be a power of two between the values of $kPaintMinResolution$ and $kPaintMaxResolution$, inclusive.
	//
	//# \desc
	//#
	//
	//# \base	Space		A paint space is a specific type of space.
	//
	//# \also	$@PaintSpaceObject@$


	class PaintSpace final : public Space
	{
		friend class Space;

		private:

			PaintEnvironment		paintEnvironment;

			PaintSpace();
			PaintSpace(const PaintSpace& paintSpace);

			Node *Replicate(void) const override;

			void HandleTransformUpdate(void) override;

		public:

			C4API PaintSpace(const Vector3D& size, const Integer2D& resolution, int32 count);
			C4API ~PaintSpace();

			PaintSpaceObject *GetObject(void) const
			{
				return (static_cast<PaintSpaceObject *>(Node::GetObject()));
			}

			const PaintEnvironment *GetPaintEnvironment(void) const
			{
				return (&paintEnvironment);
			}

			void Preprocess(void) override;
			void Neutralize(void) override;
	};


	//# \class	PhysicsSpace		Represents a physics space node in a world.
	//
	//# The $PhysicsSpace$ class represents a physics space node in a world.
	//
	//# \def	class PhysicsSpace final : public Space
	//
	//# \ctor	PhysicsSpace(const Vector3D& size);
	//
	//# \param	size	The size of the box.
	//
	//# \desc
	//#
	//
	//# \base	Space		A physics space is a specific type of space.
	//
	//# \also	$@PhysicsSpaceObject@$


	class PhysicsSpace final : public Space
	{
		friend class Space;

		private:

			PhysicsSpace();
			PhysicsSpace(const PhysicsSpace& physicsSpace);

			Node *Replicate(void) const override;

		public:

			C4API PhysicsSpace(const Vector3D& size);
			C4API ~PhysicsSpace();

			PhysicsSpaceObject *GetObject(void) const
			{
				return (static_cast<PhysicsSpaceObject *>(Node::GetObject()));
			}
	};
}


#endif

// ZYUQURM
