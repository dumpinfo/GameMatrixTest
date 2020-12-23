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


#ifndef C4GeometryManipulators_h
#define C4GeometryManipulators_h


#include "C4Deformable.h"
#include "C4EditorManipulators.h"


namespace C4
{
	class GeometryManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			ColorRGBA			geometryColor;

			bool				*selectedSurfaceArray;
			int32				selectionDetailLevel;

			VertexBuffer		selectionIndexBuffer;
			List<Attribute>		selectionAttributeList;
			DiffuseAttribute	selectionDiffuseColor;
			Renderable			selectionRenderable;

			static Manipulator *Create(Geometry *geometry);

			bool CalculateNodeSphere(BoundingSphere *sphere) const override;

			static int32 GetGeometryPositionArray(const Geometry *geometry, const float **array);

		public:

			GeometryManipulator(Geometry *geometry);
			~GeometryManipulator();

			Geometry *GetTargetNode(void) const
			{
				return (static_cast<Geometry *>(EditorManipulator::GetTargetNode()));
			}

			bool SurfaceSelected(unsigned_int32 index) const
			{
				return ((selectedSurfaceArray) && (selectedSurfaceArray[index]));
			}

			const char *GetDefaultNodeName(void) const override;

			void Select(void) override;
			void Unselect(void) override;

			void Hilite(void) override;
			void Unhilite(void) override;

			void SelectSurface(unsigned_int32 index);
			void UnselectSurface(unsigned_int32 index);

			int32 GetSelectedSurfaceCount(void) const;
			void UpdateSurfaceSelection(void);

			void HandleSizeUpdate(int32 count, const float *size) override;

			bool MaterialSettable(void) const override;
			int32 GetMaterialCount(void) const override;
			MaterialObject *GetMaterial(int32 index) const override;
			void SetMaterial(MaterialObject *material) override;
			void ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial) override;
			void InvalidateShaderData(void) override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class GenericGeometryManipulator : public GeometryManipulator
	{
		private:

			Box3D	originalBounds;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			GenericGeometryManipulator(GenericGeometry *geometry);
			~GenericGeometryManipulator();

			GenericGeometry *GetTargetNode(void) const
			{
				return (static_cast<GenericGeometry *>(EditorManipulator::GetTargetNode()));
			}

			GenericGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject()); 
			}

			void BeginResize(const ManipulatorResizeData *resizeData) override; 
			bool Resize(const ManipulatorResizeData *resizeData) override;
	}; 


	class PlateGeometryManipulator : public GeometryManipulator 
	{
		private: 
 
			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public: 

			PlateGeometryManipulator(PlateGeometry *plate);
			~PlateGeometryManipulator();

			PlateGeometry *GetTargetNode(void) const
			{
				return (static_cast<PlateGeometry *>(EditorManipulator::GetTargetNode()));
			}

			PlateGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Pick(const Ray *ray, PickData *data) const override;
			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class DiskGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			DiskGeometryManipulator(DiskGeometry *disk);
			~DiskGeometryManipulator();

			DiskGeometry *GetTargetNode(void) const
			{
				return (static_cast<DiskGeometry *>(EditorManipulator::GetTargetNode()));
			}

			DiskGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Pick(const Ray *ray, PickData *data) const override;
			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class HoleGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			HoleGeometryManipulator(HoleGeometry *disk);
			~HoleGeometryManipulator();

			HoleGeometry *GetTargetNode(void) const
			{
				return (static_cast<HoleGeometry *>(EditorManipulator::GetTargetNode()));
			}

			HoleGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Pick(const Ray *ray, PickData *data) const override;
			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class AnnulusGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			AnnulusGeometryManipulator(AnnulusGeometry *annulus);
			~AnnulusGeometryManipulator();

			AnnulusGeometry *GetTargetNode(void) const
			{
				return (static_cast<AnnulusGeometry *>(EditorManipulator::GetTargetNode()));
			}

			AnnulusGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Pick(const Ray *ray, PickData *data) const override;
			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class BoxGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			BoxGeometryManipulator(BoxGeometry *box);
			~BoxGeometryManipulator();

			BoxGeometry *GetTargetNode(void) const
			{
				return (static_cast<BoxGeometry *>(EditorManipulator::GetTargetNode()));
			}

			BoxGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Pick(const Ray *ray, PickData *data) const override;
			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class PyramidGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			PyramidGeometryManipulator(PyramidGeometry *pyramid);
			~PyramidGeometryManipulator();

			PyramidGeometry *GetTargetNode(void) const
			{
				return (static_cast<PyramidGeometry *>(EditorManipulator::GetTargetNode()));
			}

			PyramidGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class CylinderGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			CylinderGeometryManipulator(CylinderGeometry *cylinder);
			~CylinderGeometryManipulator();

			CylinderGeometry *GetTargetNode(void) const
			{
				return (static_cast<CylinderGeometry *>(EditorManipulator::GetTargetNode()));
			}

			CylinderGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Pick(const Ray *ray, PickData *data) const override;
			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class ConeGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			ConeGeometryManipulator(ConeGeometry *cone);
			~ConeGeometryManipulator();

			ConeGeometry *GetTargetNode(void) const
			{
				return (static_cast<ConeGeometry *>(EditorManipulator::GetTargetNode()));
			}

			ConeGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class TruncatedConeGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TruncatedConeGeometryManipulator(TruncatedConeGeometry *truncatedCone);
			~TruncatedConeGeometryManipulator();

			TruncatedConeGeometry *GetTargetNode(void) const
			{
				return (static_cast<TruncatedConeGeometry *>(EditorManipulator::GetTargetNode()));
			}

			TruncatedConeGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class SphereGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			SphereGeometryManipulator(SphereGeometry *sphere);
			~SphereGeometryManipulator();

			SphereGeometry *GetTargetNode(void) const
			{
				return (static_cast<SphereGeometry *>(EditorManipulator::GetTargetNode()));
			}

			SphereGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class DomeGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			DomeGeometryManipulator(DomeGeometry *dome);
			~DomeGeometryManipulator();

			DomeGeometry *GetTargetNode(void) const
			{
				return (static_cast<DomeGeometry *>(EditorManipulator::GetTargetNode()));
			}

			DomeGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class TorusGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TorusGeometryManipulator(TorusGeometry *torus);
			~TorusGeometryManipulator();

			TorusGeometry *GetTargetNode(void) const
			{
				return (static_cast<TorusGeometry *>(EditorManipulator::GetTargetNode()));
			}

			TorusGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class TubeGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			TubeGeometryManipulator(TubeGeometry *tube);
			~TubeGeometryManipulator();

			TubeGeometry *GetTargetNode(void) const
			{
				return (static_cast<TubeGeometry *>(EditorManipulator::GetTargetNode()));
			}

			TubeGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class ExtrusionGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			ExtrusionGeometryManipulator(ExtrusionGeometry *extrusion);
			~ExtrusionGeometryManipulator();

			ExtrusionGeometry *GetTargetNode(void) const
			{
				return (static_cast<ExtrusionGeometry *>(EditorManipulator::GetTargetNode()));
			}

			ExtrusionGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class RevolutionGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			RevolutionGeometryManipulator(RevolutionGeometry *revolution);
			~RevolutionGeometryManipulator();

			RevolutionGeometry *GetTargetNode(void) const
			{
				return (static_cast<RevolutionGeometry *>(EditorManipulator::GetTargetNode()));
			}

			RevolutionGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class RopeGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			RopeGeometryManipulator(RopeGeometry *rope);
			~RopeGeometryManipulator();

			RopeGeometry *GetTargetNode(void) const
			{
				return (static_cast<RopeGeometry *>(EditorManipulator::GetTargetNode()));
			}

			RopeGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Resize(const ManipulatorResizeData *resizeData) override;
	};


	class ClothGeometryManipulator : public GeometryManipulator
	{
		private:

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			ClothGeometryManipulator(ClothGeometry *cloth);
			~ClothGeometryManipulator();

			ClothGeometry *GetTargetNode(void) const
			{
				return (static_cast<ClothGeometry *>(EditorManipulator::GetTargetNode()));
			}

			ClothGeometryObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			bool Pick(const Ray *ray, PickData *data) const override;
			bool Resize(const ManipulatorResizeData *resizeData) override;
	};
}


#endif

// ZYUQURM
