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


#ifndef C4EditorGizmo_h
#define C4EditorGizmo_h


#include "C4EditorBase.h"


namespace C4
{
	class EditorManipulator;


	class EditorGizmo
	{
		private:

			struct ArrowVertex
			{
				ConstPoint3D		position;
				ConstVector4D		tangent;
				ConstPoint2D		texcoord;
			};

			Editor								*worldEditor;
			const EditorManipulator				*gizmoManipulator;

			VertexBuffer						axisVertexBuffer;
			List<Attribute>						axisAttributeList[3];
			Storage<DiffuseAttribute>			axisColorAttribute[3];
			Storage<DiffuseTextureAttribute>	axisTextureAttribute[3];
			Storage<Renderable>					axisRenderable[3];
			Vector4D							axisSizeVector;

			VertexBuffer						arrowVertexBuffer;
			List<Attribute>						arrowAttributeList[3];
			Storage<DiffuseAttribute>			arrowColorAttribute[3];
			Storage<DiffuseTextureAttribute>	arrowTextureAttribute[3];
			Storage<Renderable>					arrowRenderable[3];
			Vector4D							arrowSizeVector;

			VertexBuffer						squareVertexBuffer;
			List<Attribute>						squareAttributeList[12];
			Storage<DiffuseAttribute>			squareColorAttribute[12];
			Storage<DiffuseTextureAttribute>	squareTextureAttribute[12];
			Storage<Renderable>					squareRenderable[12];
			Vector4D							squareSizeVector[12];

			VertexBuffer						circleVertexBuffer;
			List<Attribute>						circleAttributeList[12];
			Storage<DiffuseAttribute>			circleColorAttribute[12];
			Storage<DiffuseTextureAttribute>	circleTextureAttribute[12];
			Storage<Renderable>					circleRenderable[12];
			Vector4D							circleSizeVector[12];

			Box3D								gizmoBox;
			Vector4D							boxExpandVector;
			Vector4D							edgeExpandVector;

			int32								hiliteArrowIndex;
			int32								hiliteSquareIndex;
			int32								hiliteCircleIndex;
			int32								hiliteFaceIndex;
			int32								hiliteEdgeIndex;
			float								squarePlaneSign;

			Transform4D							trackTransform;
			Transform4D							trackInverseTransform;
			Point3D								trackPosition;
			Vector3D							trackAxis[2];
			Antivector4D						trackPlane;
			int32								trackIndex;

			VertexBuffer						boxVertexBuffer;
			List<Attribute>						boxAttributeList;
			DiffuseAttribute					boxColorAttribute;
			DiffuseTextureAttribute				boxTextureAttribute;
			Renderable							boxRenderable;
			Point3D								boxVertex[24];

			VertexBuffer						faceVertexBuffer;
			List<Attribute>						faceAttributeList;
			DiffuseAttribute					faceColorAttribute;
			Renderable							faceRenderable;
			Point3D								faceVertex[4];

			VertexBuffer						edgeVertexBuffer;
			List<Attribute>						edgeAttributeList;
			DiffuseAttribute					edgeColorAttribute;
			DiffuseTextureAttribute				edgeTextureAttribute;
			Renderable							edgeRenderable;
			Point3D								edgeVertex[2];

			static const ConstColorRGBA			gizmoAxisColor[3];
			static const ConstPoint3D			gizmoAxisVertex[6];
			static const ArrowVertex			gizmoArrowVertex[12];
			static const ConstPoint3D			gizmoSquareVertex[36];
			static const ConstPoint3D			gizmoCircleVertex[396];

			bool PickArrow(const Point3D& origin, const Vector3D& direction, const ArrowVertex *vertex, const Vector3D& axis, float size, Point3D *pickPoint) const;
			bool PickSquare(const Point3D& origin, const Vector3D& direction, const Point3D *vertex, const Antivector4D& plane, float size, Point3D *pickPoint) const;
			bool PickCircle(const Point3D& origin, const Vector3D& direction, const Point3D *vertex, const Antivector4D& plane, float size, float scale, Point3D *pickPoint) const; 

			void RenderBox(const ManipulatorRenderData *renderData, List<Renderable> *renderList);
 
		public:
 
			EditorGizmo(Editor *editor, const EditorManipulator *manipulator);
			~EditorGizmo();
 
			const Box3D& GetGizmoBox(void) const
			{ 
				return (gizmoBox); 
			}

			const Transformable *GetTransformable(void) const
			{ 
				return (axisRenderable[0]->GetTransformable());
			}

			void HiliteFace(int32 face, float intensity = 1.0F);
			void HiliteEdge(int32 edge, float intensity = 1.0F);
			int32 PickFace(const Ray *ray, Point3D *point = nullptr) const;
			int32 PickEdge(const Ray *ray, Point3D *point = nullptr) const;

			int32 PickArrow(const Camera *camera, EditorViewportType viewportType, float lineScale, const Ray *ray);
			int32 PickSquare(const Camera *camera, EditorViewportType viewportType, float lineScale, const Ray *ray);
			int32 PickCircle(const Camera *camera, EditorViewportType viewportType, float lineScale, const Ray *ray);

			bool TrackArrow(const Ray *ray, Vector3D *delta) const;
			bool TrackSquare(const Ray *ray, Vector3D *delta) const;
			bool TrackCircle(const Ray *ray, float *delta) const;

			void Render(const ManipulatorRenderData *renderData, List<Renderable> *renderList);
			void Reset(void);
	};
}


#endif

// ZYUQURM
