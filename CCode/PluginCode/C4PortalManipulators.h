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


#ifndef C4PortalManipulators_h
#define C4PortalManipulators_h


#include "C4EditorManipulators.h"
#include "C4Portals.h"


namespace C4
{
	class PortalManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			struct InteriorVertex
			{
				Point3D		position;
				Point2D		texcoord;
			};

			struct OutlineVertex
			{
				Point3D		position;
			};

			struct DirectionVertex
			{
				Point3D		position;
				Point2D		texcoord;
			};

			ColorRGBA					portalColor;
			Vector4D					portalSizeVector;

			Point3D						originalVertexPosition;

			VertexBuffer				interiorVertexBuffer;
			VertexBuffer				interiorIndexBuffer;
			List<Attribute>				interiorAttributeList;
			DiffuseAttribute			interiorColorAttribute;
			DiffuseTextureAttribute		interiorTextureAttribute;
			Renderable					interiorRenderable;

			VertexBuffer				outlineVertexBuffer;
			List<Attribute>				outlineAttributeList;
			DiffuseAttribute			outlineColorAttribute;
			DiffuseTextureAttribute		outlineTextureAttribute;
			Renderable					outlineRenderable;

			VertexBuffer				directionVertexBuffer;
			List<Attribute>				directionAttributeList;
			DiffuseAttribute			directionColorAttribute;
			DiffuseTextureAttribute		directionTextureAttribute;
			Renderable					directionRenderable;

			static Manipulator *Create(Portal *portal);

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

			static Point3D ConstrainVertex(const Point3D& original, const Point3D& current, const Point3D& v1, const Point3D& v2);

		protected:

			PortalManipulator(Portal *portal, const ColorRGBA& color);

		public:

			~PortalManipulator();

			Portal *GetTargetNode(void) const
			{
				return (static_cast<Portal *>(EditorManipulator::GetTargetNode()));
			}

			PortalObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			const char *GetDefaultNodeName(void) const override;

			void Invalidate(void) override;

			void Select(void) override;
			void Unselect(void) override;

			void HandleConnectorUpdate(void) override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Pick(const Ray *ray, PickData *data) const override;
			bool RegionPick(const VisibilityRegion *region) const override;

			void BeginResize(const ManipulatorResizeData *resizeData) override;
			bool Resize(const ManipulatorResizeData *resizeData) override;

			void Update(void) override;
			void Render(const ManipulatorRenderData *renderData) override; 
	};

 
	class DirectPortalManipulator : public PortalManipulator
	{ 
		public:

			DirectPortalManipulator(DirectPortal *portal); 
			~DirectPortalManipulator();
 
			DirectPortal *GetTargetNode(void) const 
			{
				return (static_cast<DirectPortal *>(EditorManipulator::GetTargetNode()));
			}
	}; 


	class RemotePortalManipulator : public PortalManipulator
	{
		public:

			RemotePortalManipulator(RemotePortal *portal);
			~RemotePortalManipulator();

			RemotePortal *GetTargetNode(void) const
			{
				return (static_cast<RemotePortal *>(EditorManipulator::GetTargetNode()));
			}
	};


	class OcclusionPortalManipulator : public PortalManipulator
	{
		public:

			OcclusionPortalManipulator(OcclusionPortal *portal);
			~OcclusionPortalManipulator();

			OcclusionPortal *GetTargetNode(void) const
			{
				return (static_cast<OcclusionPortal *>(EditorManipulator::GetTargetNode()));
			}
	};
}


#endif

// ZYUQURM
