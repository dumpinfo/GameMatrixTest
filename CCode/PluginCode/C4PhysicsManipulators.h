//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#ifndef C4PhysicsManipulators_h
#define C4PhysicsManipulators_h


#include "C4EditorManipulators.h"
#include "C4VolumeManipulators.h"
#include "C4Fields.h"
#include "C4Joints.h"
#include "C4Blockers.h"
#include "C4Physics.h"


namespace C4
{
	class PhysicsNodeManipulator : public EditorManipulator
	{
		public:

			PhysicsNodeManipulator(PhysicsNode *physicsNode);
			~PhysicsNodeManipulator();

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;
	};


	class ShapeManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			VolumeManipulator	*volumeManipulator;

			static Manipulator *Create(Shape *shape);

		protected:

			ShapeManipulator(Shape *shape, VolumeManipulator *volume);

		public:

			~ShapeManipulator();

			Shape *GetTargetNode(void) const
			{
				return (static_cast<Shape *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Select(void) override;
			void Unselect(void) override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class BoxShapeManipulator : public ShapeManipulator, public BoxVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			BoxShapeManipulator(BoxShape *box);
			~BoxShapeManipulator();

			BoxShape *GetTargetNode(void) const
			{
				return (static_cast<BoxShape *>(EditorManipulator::GetTargetNode()));
			}

			BoxShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class PyramidShapeManipulator : public ShapeManipulator, public PyramidVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override; 

		public:
 
			PyramidShapeManipulator(PyramidShape *pyramid);
			~PyramidShapeManipulator(); 

			PyramidShape *GetTargetNode(void) const
			{ 
				return (static_cast<PyramidShape *>(EditorManipulator::GetTargetNode()));
			} 
 
			PyramidShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			} 

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class CylinderShapeManipulator : public ShapeManipulator, public CylinderVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			CylinderShapeManipulator(CylinderShape *cylinder);
			~CylinderShapeManipulator();

			CylinderShape *GetTargetNode(void) const
			{
				return (static_cast<CylinderShape *>(EditorManipulator::GetTargetNode()));
			}

			CylinderShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class ConeShapeManipulator : public ShapeManipulator, public ConeVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			ConeShapeManipulator(ConeShape *cone);
			~ConeShapeManipulator();

			ConeShape *GetTargetNode(void) const
			{
				return (static_cast<ConeShape *>(EditorManipulator::GetTargetNode()));
			}

			ConeShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class SphereShapeManipulator : public ShapeManipulator, public SphereVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			SphereShapeManipulator(SphereShape *sphere);
			~SphereShapeManipulator();

			SphereShape *GetTargetNode(void) const
			{
				return (static_cast<SphereShape *>(EditorManipulator::GetTargetNode()));
			}

			SphereShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class DomeShapeManipulator : public ShapeManipulator, public DomeVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			DomeShapeManipulator(DomeShape *dome);
			~DomeShapeManipulator();

			DomeShape *GetTargetNode(void) const
			{
				return (static_cast<DomeShape *>(EditorManipulator::GetTargetNode()));
			}

			DomeShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class CapsuleShapeManipulator : public ShapeManipulator, public CapsuleVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			CapsuleShapeManipulator(CapsuleShape *capsule);
			~CapsuleShapeManipulator();

			CapsuleShape *GetTargetNode(void) const
			{
				return (static_cast<CapsuleShape *>(EditorManipulator::GetTargetNode()));
			}

			CapsuleShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class TruncatedPyramidShapeManipulator : public ShapeManipulator, public TruncatedPyramidVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TruncatedPyramidShapeManipulator(TruncatedPyramidShape *truncatedPyramid);
			~TruncatedPyramidShapeManipulator();

			TruncatedPyramidShape *GetTargetNode(void) const
			{
				return (static_cast<TruncatedPyramidShape *>(EditorManipulator::GetTargetNode()));
			}

			TruncatedPyramidShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class TruncatedConeShapeManipulator : public ShapeManipulator, public TruncatedConeVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TruncatedConeShapeManipulator(TruncatedConeShape *truncatedCone);
			~TruncatedConeShapeManipulator();

			TruncatedConeShape *GetTargetNode(void) const
			{
				return (static_cast<TruncatedConeShape *>(EditorManipulator::GetTargetNode()));
			}

			TruncatedConeShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class TruncatedDomeShapeManipulator : public ShapeManipulator, public TruncatedDomeVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TruncatedDomeShapeManipulator(TruncatedDomeShape *truncatedDome);
			~TruncatedDomeShapeManipulator();

			TruncatedDomeShape *GetTargetNode(void) const
			{
				return (static_cast<TruncatedDomeShape *>(EditorManipulator::GetTargetNode()));
			}

			TruncatedDomeShapeObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class JointManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			static Manipulator *Create(Joint *joint);

		protected:

			JointManipulator(Joint *joint, const char *iconName);

		public:

			~JointManipulator();

			Joint *GetTargetNode(void) const
			{
				return (static_cast<Joint *>(EditorManipulator::GetTargetNode()));
			}

			void Preprocess(void) override;

			const char *GetDefaultNodeName(void) const override;

			void HandleSettingsUpdate(void) override;
	};


	class SphericalJointManipulator : public JointManipulator
	{
		protected:

			SphericalJointManipulator(SphericalJoint *spherical, const char *iconName);

		public:

			SphericalJointManipulator(SphericalJoint *spherical);
			~SphericalJointManipulator();

			SphericalJoint *GetTargetNode(void) const
			{
				return (static_cast<SphericalJoint *>(EditorManipulator::GetTargetNode()));
			}
	};


	class UniversalJointManipulator : public SphericalJointManipulator
	{
		public:

			UniversalJointManipulator(UniversalJoint *universal);
			~UniversalJointManipulator();

			UniversalJoint *GetTargetNode(void) const
			{
				return (static_cast<UniversalJoint *>(EditorManipulator::GetTargetNode()));
			}
	};


	class DiscalJointManipulator : public JointManipulator
	{
		protected:

			DiscalJointManipulator(DiscalJoint *discal, const char *iconName);

		public:

			DiscalJointManipulator(DiscalJoint *discal);
			~DiscalJointManipulator();

			DiscalJoint *GetTargetNode(void) const
			{
				return (static_cast<DiscalJoint *>(EditorManipulator::GetTargetNode()));
			}
	};


	class RevoluteJointManipulator : public DiscalJointManipulator
	{
		public:

			RevoluteJointManipulator(RevoluteJoint *revolute);
			~RevoluteJointManipulator();

			RevoluteJoint *GetTargetNode(void) const
			{
				return (static_cast<RevoluteJoint *>(EditorManipulator::GetTargetNode()));
			}
	};


	class CylindricalJointManipulator : public DiscalJointManipulator
	{
		protected:

			CylindricalJointManipulator(CylindricalJoint *cylindrical, const char *iconName);

		public:

			CylindricalJointManipulator(CylindricalJoint *cylindrical);
			~CylindricalJointManipulator();

			CylindricalJoint *GetTargetNode(void) const
			{
				return (static_cast<CylindricalJoint *>(EditorManipulator::GetTargetNode()));
			}
	};


	class PrismaticJointManipulator : public CylindricalJointManipulator
	{
		public:

			PrismaticJointManipulator(PrismaticJoint *prismatic);
			~PrismaticJointManipulator();

			PrismaticJoint *GetTargetNode(void) const
			{
				return (static_cast<PrismaticJoint *>(EditorManipulator::GetTargetNode()));
			}
	};


	class FieldManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			VolumeManipulator	*volumeManipulator;

			static Manipulator *Create(Field *field);

		protected:

			FieldManipulator(Field *field, VolumeManipulator *volume);

		public:

			~FieldManipulator();

			Field *GetTargetNode(void) const
			{
				return (static_cast<Field *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Select(void) override;
			void Unselect(void) override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class BoxFieldManipulator : public FieldManipulator, public BoxVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			BoxFieldManipulator(BoxField *box);
			~BoxFieldManipulator();

			BoxField *GetTargetNode(void) const
			{
				return (static_cast<BoxField *>(EditorManipulator::GetTargetNode()));
			}

			BoxFieldObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class CylinderFieldManipulator : public FieldManipulator, public CylinderVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			CylinderFieldManipulator(CylinderField *cylinder);
			~CylinderFieldManipulator();

			CylinderField *GetTargetNode(void) const
			{
				return (static_cast<CylinderField *>(EditorManipulator::GetTargetNode()));
			}

			CylinderFieldObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class SphereFieldManipulator : public FieldManipulator, public SphereVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			SphereFieldManipulator(SphereField *sphere);
			~SphereFieldManipulator();

			SphereField *GetTargetNode(void) const
			{
				return (static_cast<SphereField *>(EditorManipulator::GetTargetNode()));
			}

			SphereFieldObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class BlockerManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			VolumeManipulator	*volumeManipulator;

			static Manipulator *Create(Blocker * blocker);

		protected:

			BlockerManipulator(Blocker *blocker, VolumeManipulator *volume);

		public:

			~BlockerManipulator();

			Blocker *GetTargetNode(void) const
			{
				return (static_cast<Blocker *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Select(void) override;
			void Unselect(void) override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class PlateBlockerManipulator : public BlockerManipulator, public PlateVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			PlateBlockerManipulator(PlateBlocker *plate);
			~PlateBlockerManipulator();

			PlateBlocker *GetTargetNode(void) const
			{
				return (static_cast<PlateBlocker *>(EditorManipulator::GetTargetNode()));
			}

			PlateBlockerObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class BoxBlockerManipulator : public BlockerManipulator, public BoxVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			BoxBlockerManipulator(BoxBlocker *box);
			~BoxBlockerManipulator();

			BoxBlocker *GetTargetNode(void) const
			{
				return (static_cast<BoxBlocker *>(EditorManipulator::GetTargetNode()));
			}

			BoxBlockerObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class CylinderBlockerManipulator : public BlockerManipulator, public CylinderVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			CylinderBlockerManipulator(CylinderBlocker *cylinder);
			~CylinderBlockerManipulator();

			CylinderBlocker *GetTargetNode(void) const
			{
				return (static_cast<CylinderBlocker *>(EditorManipulator::GetTargetNode()));
			}

			CylinderBlockerObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class SphereBlockerManipulator : public BlockerManipulator, public SphereVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			SphereBlockerManipulator(SphereBlocker *sphere);
			~SphereBlockerManipulator();

			SphereBlocker *GetTargetNode(void) const
			{
				return (static_cast<SphereBlocker *>(EditorManipulator::GetTargetNode()));
			}

			SphereBlockerObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class CapsuleBlockerManipulator : public BlockerManipulator, public CapsuleVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			CapsuleBlockerManipulator(CapsuleBlocker *capsule);
			~CapsuleBlockerManipulator();

			CapsuleBlocker *GetTargetNode(void) const
			{
				return (static_cast<CapsuleBlocker *>(EditorManipulator::GetTargetNode()));
			}

			CapsuleBlockerObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};
}


#endif

// ZYUQURM
