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


#ifndef C4MarkerManipulators_h
#define C4MarkerManipulators_h


#include "C4EditorManipulators.h"
#include "C4Instances.h"
#include "C4Paths.h"


namespace C4
{
	class MarkerManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			static Manipulator *Create(Marker *marker);

		public:

			MarkerManipulator(Marker *marker, const char *iconName);
			~MarkerManipulator();

			Marker *GetTargetNode(void) const
			{
				return (static_cast<Marker *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;
	};


	class LocatorMarkerManipulator : public MarkerManipulator
	{
		public:

			LocatorMarkerManipulator(LocatorMarker *marker);
			~LocatorMarkerManipulator();

			LocatorMarker *GetTargetNode(void) const
			{
				return (static_cast<LocatorMarker *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;
	};


	class PathManipulator : public EditorManipulator
	{
		private:

			struct PathVertex
			{
				Point3D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			struct TangentVertex
			{
				Point3D		position;
				Color4C		color;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			struct PointVertex
			{
				Point3D		position;
				Color4C		color;
				Vector2D	billboard;
			};

			float						*pointSelectionArray;
			int32						maxSelectedPointCount;

			Vector4D					pathSizeVector;
			Vector4D					pointSizeVector;

			VertexBuffer				pathVertexBuffer;
			List<Attribute>				pathAttributeList;
			DiffuseAttribute			pathColorAttribute;
			DiffuseTextureAttribute		pathTextureAttribute;
			Renderable					pathRenderable;

			VertexBuffer				tangentVertexBuffer;
			List<Attribute>				tangentAttributeList;
			DiffuseTextureAttribute		tangentTextureAttribute;
			Renderable					tangentRenderable;

			VertexBuffer				pointVertexBuffer;
			Renderable					pointRenderable;

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			static const PathComponent *GetControlPointComponent(const Path *path, int32 *index);
			void UpdateControlPointSelection(void); 

		public:
 
			PathManipulator(PathMarker *path);
			~PathManipulator(); 

			PathMarker *GetTargetNode(void) const
			{ 
				return (static_cast<PathMarker *>(EditorManipulator::GetTargetNode()));
			} 
 
			bool ControlPointSelected(int32 index) const
			{
				return ((pointSelectionArray) && (pointSelectionArray[index] > 0.0F));
			} 

			const char *GetDefaultNodeName(void) const override;

			void Select(void) override;
			void Unselect(void) override;

			void SelectControlPoint(int32 index, bool selectTangent = false);
			void UnselectControlPoint(int32 index, bool unselectTangent = false);
			void MoveSelectedControlPoints(const Vector3D& delta, bool maintainTangents = false);

			bool PickControlPoint(const Ray *ray, PickData *data) const;
			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void Update(void) override;
			void Render(const ManipulatorRenderData *renderData) override;
	};
}


#endif

// ZYUQURM
