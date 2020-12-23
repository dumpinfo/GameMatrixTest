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


#ifndef C4EditorViewports_h
#define C4EditorViewports_h


//# \component	World Editor
//# \prefix		WorldEditor/


#include "C4Viewports.h"
#include "C4EditorBase.h"


namespace C4
{
	//# \enum	EditorViewportType

	enum : EditorViewportType
	{
		kEditorViewportOrtho		= 'ORTH',		//## Orthographic viewport.
		kEditorViewportFrustum		= 'FRUS',		//## Perspective viewport.
		kEditorViewportGraph		= 'GRPH'		//## Scene graph viewport.
	};


	class EditorObject;
	class MoveOperation;


	class EditorViewport
	{
		private:

			EditorViewportType		editorViewportType;

			Editor					*worldEditor;
			ViewportWidget			*viewportWidget;

			Renderable				*toolRenderable;

			BorderWidget			borderWidget;
			TextWidget				titleWidget;

		protected:

			EditorViewport(EditorViewportType type, Editor *editor, int32 index, int32 mode, ViewportWidget *widget);

			Editor *GetEditor(void) const
			{
				return (worldEditor);
			}

			void Postconstruct(void);
			void Predestruct(void);

			static void HandleViewportTrack(const Point3D& position, ViewportWidget *viewport, void *cookie);

			void RenderNode(const Node *root, const ManipulatorRenderData *renderData);

		public:

			static const unsigned_int32 viewportIdentifier[8];

			virtual ~EditorViewport();

			EditorViewportType GetEditorViewportType(void) const
			{
				return (editorViewportType);
			}

			ViewportWidget *GetViewportWidget(void) const
			{
				return (viewportWidget);
			}

			Renderable *GetToolRenderable(void) const
			{
				return (toolRenderable);
			}

			void SetToolRenderable(Renderable *renderable)
			{
				toolRenderable = renderable;
			}

			void Show(void)
			{
				viewportWidget->Show();
			}

			void Hide(void)
			{
				viewportWidget->Hide();
			}

			void SetViewportCameraTransform(const Transform4D& transform)
			{
				viewportWidget->GetViewportCamera()->SetNodeTransform(transform);
			}

			virtual void SetViewportPosition(const Point3D& position, const Vector2D& size);
			virtual void Invalidate(void); 

			virtual void ShowViewportInfo(void);
			virtual void HideViewportInfo(void); 

			virtual void Hover(const Point3D& position); 
	};

 
	class OrthoEditorViewport : public EditorViewport
	{ 
		private: 

			OrthoViewportWidget		orthoViewport;
			ImageWidget				axesWidget;
 
			bool					gridValidFlag;
			Grid					viewportGrid;
			Transformable			gridTransformable;

			static void HandleViewportMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void RenderViewport(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);
			static void RenderOverlay(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);
			void RenderGrid(const EditorObject *editorObject);

		public:

			OrthoEditorViewport(Editor *editor, int32 index, const Vector2D& scale, int32 mode);
			~OrthoEditorViewport();

			OrthoViewportWidget *GetViewportWidget(void)
			{
				return (&orthoViewport);
			}

			void SetViewportPosition(const Point3D& position, const Vector2D& size);
			void Invalidate(void) override;

			void ShowViewportInfo(void);
			void HideViewportInfo(void);

			void Hover(const Point3D& position) override;
	};


	class FrustumEditorViewport : public EditorViewport
	{
		private:

			enum
			{
				kViewportTrackNone,
				kViewportTrackUseTool,
				kViewportTrackMoveGizmo,
				kViewportTrackRotateGizmo
			};

			int32					viewportTrackMode;
			MoveOperation			*trackOperation;

			Point3D					trackAnchor;
			Vector3D				trackDirection;
			Point3D					trackCenter;

			Vector3D				multiaxisTranslationRate;
			Vector3D				multiaxisRotationRate;

			FrustumViewportWidget	frustumViewport;

			static void HandleViewportMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void HandleViewportTrack(const Point3D& position, ViewportWidget *viewport, void *cookie);
			static void RenderViewport(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);
			static void RenderOverlay(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

		public:

			FrustumEditorViewport(Editor *editor, int32 index);
			~FrustumEditorViewport();

			FrustumViewportWidget *GetViewportWidget(void)
			{
				return (&frustumViewport);
			}

			void SetViewportCameraTransform(const Vector2D& angle, const Point3D& position)
			{
				frustumViewport.SetCameraTransform(angle.x, angle.y, position);
			}

			void Hover(const Point3D& position) override;
	};


	class GraphEditorViewport : public EditorViewport
	{
		private:

			OrthoViewportWidget		graphViewport;

			static void HandleViewportMouseEvent(const MouseEventData *eventData, ViewportWidget *viewport, void *cookie);
			static void RenderViewport(List<Renderable> *renderList, ViewportWidget *viewport, void *cookie);

		public:

			GraphEditorViewport(Editor *editor, int32 index, const Vector2D& scale);
			~GraphEditorViewport();

			OrthoViewportWidget *GetViewportWidget(void)
			{
				return (&graphViewport);
			}
	};
}


#endif

// ZYUQURM
