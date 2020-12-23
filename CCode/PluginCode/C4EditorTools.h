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


#ifndef C4EditorTools_h
#define C4EditorTools_h


#include "C4Viewports.h"
#include "C4EditorBase.h"


namespace C4
{
	enum
	{
		kEditorToolNodeBoxSelect,
		kEditorToolNodeSelect,
		kEditorToolNodeMove,
		kEditorToolNodeRotate,
		kEditorToolNodeResize,
		kEditorToolConnect,
		kEditorToolSurfaceSelect,
		kEditorToolViewportScroll,
		kEditorToolViewportZoom,
		kEditorToolViewportBoxZoom,
		kEditorToolOrbitCamera,
		kEditorToolFreeCamera,
		kEditorToolCount
	};


	enum
	{
		kFreeCameraForward		= 1 << 0,
		kFreeCameraBackward		= 1 << 1,
		kFreeCameraLeft			= 1 << 2,
		kFreeCameraRight		= 1 << 3,
		kFreeCameraUp			= 1 << 4,
		kFreeCameraDown			= 1 << 5
	};


	class Editor;
	class EditorManipulator;
	struct EditorTrackData;


	class EditorTool
	{
		protected:

			C4EDITORAPI EditorTool();

			static bool SelectNode(Editor *editor, EditorTrackData *trackData);

		public:

			C4EDITORAPI virtual ~EditorTool();

			C4EDITORAPI virtual void Engage(Editor *editor, void *cookie = nullptr);
			C4EDITORAPI virtual void Disengage(Editor *editor, void *cookie = nullptr);

			C4EDITORAPI virtual bool BeginTool(Editor *editor, EditorTrackData *trackData);
			C4EDITORAPI virtual bool TrackTool(Editor *editor, EditorTrackData *trackData);
			C4EDITORAPI virtual bool EndTool(Editor *editor, EditorTrackData *trackData);
	};


	class StandardEditorTool : public EditorTool
	{
		private:

			IconButtonWidget						*toolButton;
			WidgetObserver<StandardEditorTool>		toolObserver;

			void HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData);

		protected:

			StandardEditorTool(IconButtonWidget *widget);

		public:

			~StandardEditorTool();

			void Engage(Editor *editor, void *cookie = nullptr) override;
			void Disengage(Editor *editor, void *cookie = nullptr) override;
	};


	class NodeSelectTool : public StandardEditorTool
	{
		public:

			NodeSelectTool(IconButtonWidget *widget);
			~NodeSelectTool();

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class NodeMoveTool : public StandardEditorTool
	{
		private: 

			bool		operationFlag;
			bool		planarMove; 

		public: 

			NodeMoveTool(IconButtonWidget *widget);
			~NodeMoveTool(); 

			void Engage(Editor *editor, void *cookie) override; 
			void Disengage(Editor *editor, void *cookie) override; 

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override; 
	};


	class NodeRotateTool : public StandardEditorTool
	{
		private:

			bool		operationFlag;

			Point3D		rotationCenter;
			float		accumAngle;

		public:

			NodeRotateTool(IconButtonWidget *widget);
			~NodeRotateTool();

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class NodeScaleTool : public StandardEditorTool
	{
		private:

			bool		operationFlag;

		public:

			NodeScaleTool(IconButtonWidget *widget);
			~NodeScaleTool();

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ConnectTool : public StandardEditorTool
	{
		public:

			ConnectTool(IconButtonWidget *widget);
			~ConnectTool();

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class SurfaceSelectTool : public StandardEditorTool
	{
		public:

			SurfaceSelectTool(IconButtonWidget *widget);
			~SurfaceSelectTool();

			void Engage(Editor *editor, void *cookie) override;
			void Disengage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ViewportScrollTool : public StandardEditorTool
	{
		private:

			Point3D		initalCameraPosition;
			Cursor		*previousCursor;

		public:

			ViewportScrollTool(IconButtonWidget *widget);
			~ViewportScrollTool();

			void Engage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ViewportZoomTool : public StandardEditorTool
	{
		public:

			ViewportZoomTool(IconButtonWidget *widget);
			~ViewportZoomTool();

			void Engage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class DragRectTool : public StandardEditorTool
	{
		protected:

			DragRect		dragRect;

			DragRectTool(IconButtonWidget *widget, const ColorRGBA& color);

		public:

			~DragRectTool();

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class BoxSelectTool : public DragRectTool
	{
		public:

			BoxSelectTool(IconButtonWidget *widget);
			~BoxSelectTool();

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class ViewportBoxZoomTool : public DragRectTool
	{
		public:

			ViewportBoxZoomTool(IconButtonWidget *widget);
			~ViewportBoxZoomTool();

			void Engage(Editor *editor, void *cookie) override;

			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class OrbitCameraTool : public StandardEditorTool
	{
		private:

			Point3D		orbitCenter;

		public:

			OrbitCameraTool(IconButtonWidget *widget);
			~OrbitCameraTool();

			void Engage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class FreeCameraTool : public StandardEditorTool
	{
		private:

			float				cameraSpeed;
			unsigned_int32		cameraFlags;

			Cursor				*previousCursor;

		public:

			FreeCameraTool(IconButtonWidget *widget);
			~FreeCameraTool();

			unsigned_int32 GetCameraFlags(void) const
			{
				return (cameraFlags);
			}

			void SetCameraFlags(unsigned_int32 flags)
			{
				cameraFlags = flags;
			}

			void Engage(Editor *editor, void *cookie) override;

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};


	class NodeReparentTool : public EditorTool
	{
		private:

			struct ReparentVertex
			{
				Point2D		position;
				Color4C		color;
			};

			bool						reparentVisible;

			VertexBuffer				reparentVertexBuffer;
			static SharedVertexBuffer	reparentIndexBuffer;
			Renderable					reparentRenderable;

			void CalculateReparentVertices(const Point2D& position, const Box2D *box = nullptr);
			EditorManipulator *GetReparentNode(Editor *editor, EditorTrackData *trackData);

		public:

			NodeReparentTool();
			~NodeReparentTool();

			bool BeginTool(Editor *editor, EditorTrackData *trackData) override;
			bool TrackTool(Editor *editor, EditorTrackData *trackData) override;
			bool EndTool(Editor *editor, EditorTrackData *trackData) override;
	};
}


#endif

// ZYUQURM
