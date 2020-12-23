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


#ifndef C4ZoneManipulators_h
#define C4ZoneManipulators_h


#include "C4EditorManipulators.h"
#include "C4Zones.h"


namespace C4
{
	class ZoneManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			const Point3D					*zonePositionArray;
			const Vector4D					*zoneTangentArray;

			Vector4D						zoneSizeVector;
			AutoRelease<MaterialObject>		zoneMaterial;
			DiffuseAttribute				zoneColorAttribute;
			DiffuseTextureAttribute			zoneTextureAttribute;

			static ZoneManipulator *Create(Zone *zone);

		protected:

			struct ZoneVertex
			{
				Point3D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			VertexBuffer					zoneVertexBuffer;
			Renderable						zoneRenderable;

			ZoneManipulator(Zone *zone);

			void SetZonePositionArray(const Point3D *position)
			{
				zonePositionArray = position;
			}

			void SetZoneTangentArray(const Vector4D *tangent)
			{
				zoneTangentArray = tangent;
			}

			const Vector4D& GetZoneSize(void) const
			{
				return (zoneSizeVector);
			}

			void SetZoneSize(float x, float y, float z)
			{
				zoneSizeVector.GetVector3D().Set(x, y, z);
			}

		public:

			~ZoneManipulator();

			Zone *GetTargetNode(void) const
			{
				return (static_cast<Zone *>(EditorManipulator::GetTargetNode()));
			}

			ZoneObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			void SetTarget(bool target);

			void Invalidate(void) override;

			void Select(void) override;
			void Unselect(void) override;

			void HandleDelete(bool undoable) override;
			void HandleUndelete(void) override;
			void HandleSizeUpdate(int32 count, const float *size) override;
			void HandleSettingsUpdate(void) override;
			void HandleConnectorUpdate(void) override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void EndTransform(void) override;
			void EndResize(const ManipulatorResizeData *resizeData) override;

			void Update(void) override;
			void Render(const ManipulatorRenderData *renderData) override;
	};


	class InfiniteZoneManipulator : public ZoneManipulator 
	{
		private:
 
			Box3D		originalZoneBox;
 
			Point3D		boxPosition[48];

			bool CalculateNodeSphere(BoundingSphere *sphere) const; 

			int32 GetHandleTable(Point3D *handle) const; 
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override; 

		public:

			InfiniteZoneManipulator(InfiniteZone *infinite); 
			~InfiniteZoneManipulator();

			InfiniteZone *GetTargetNode(void) const
			{
				return (static_cast<InfiniteZone *>(EditorManipulator::GetTargetNode()));
			}

			InfiniteZoneObject *GetObject(void) const
			{
				return (static_cast<InfiniteZoneObject *>(GetTargetNode()->GetObject()));
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			void HandleSizeUpdate(int32 count, const float *size) override;

			void BeginResize(const ManipulatorResizeData *resizeData) override;
			bool Resize(const ManipulatorResizeData *resizeData) override;

			void Update(void) override;
	};


	class BoxZoneManipulator : public ZoneManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			static const ConstPoint3D		outlinePosition[48];
			static const ConstVector4D		outlineTangent[48];

			BoxZoneManipulator(BoxZone *box);
			~BoxZoneManipulator();

			BoxZone *GetTargetNode(void) const
			{
				return (static_cast<BoxZone *>(EditorManipulator::GetTargetNode()));
			}

			BoxZoneObject *GetObject(void) const
			{
				return (static_cast<BoxZoneObject *>(GetTargetNode()->GetObject()));
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class CylinderZoneManipulator : public ZoneManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			static const ConstPoint3D		outlinePosition[144];
			static const ConstVector4D		outlineTangent[144];

			CylinderZoneManipulator(CylinderZone *cylinder);
			~CylinderZoneManipulator();

			CylinderZone *GetTargetNode(void) const
			{
				return (static_cast<CylinderZone *>(EditorManipulator::GetTargetNode()));
			}

			CylinderZoneObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;
			void Update(void) override;
	};


	class PolygonZoneManipulator : public ZoneManipulator
	{
		private:

			Point3D		originalVertexPosition;

			Point3D		polygonPosition[kMaxZoneVertexCount * 12];
			Vector4D	polygonTangent[kMaxZoneVertexCount * 12];

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

			static Point3D ConstrainVertex(const Point3D& original, const Point3D& current, const Point3D& v1, const Point3D& v2);

		public:

			PolygonZoneManipulator(PolygonZone *polygon);
			~PolygonZoneManipulator();

			PolygonZone *GetTargetNode(void) const
			{
				return (static_cast<PolygonZone *>(EditorManipulator::GetTargetNode()));
			}

			PolygonZoneObject *GetObject(void) const
			{
				return (static_cast<PolygonZoneObject *>(GetTargetNode()->GetObject()));
			}

			Box3D CalculateNodeBoundingBox(void) const override;

			void BeginResize(const ManipulatorResizeData *resizeData) override;
			bool Resize(const ManipulatorResizeData *resizeData) override;

			bool Pick(const Ray *ray, PickData *data) const override;

			void Update(void) override;
	};
}


#endif

// ZYUQURM
