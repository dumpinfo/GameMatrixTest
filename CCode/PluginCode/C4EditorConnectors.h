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


#ifndef C4EditorConnectors_h
#define C4EditorConnectors_h


#include "C4Node.h"
#include "C4Widgets.h"


namespace C4
{
	class EditorManipulator;
	struct ManipulatorViewportData;


	class EditorConnector
	{
		private:

			struct ConnectorVertex
			{
				Point3D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			Connector						*connectorObject;
			const Node						*connectorNode;
			int32							connectorIndex;

			Widget							groupWidget;
			LineWidget						lineWidget1;
			LineWidget						lineWidget2;
			QuadWidget						backgroundWidget;
			BorderWidget					borderWidget;
			TextWidget						textWidget;

			VertexBuffer					lineVertexBuffer;
			List<Attribute>					lineAttributeList;
			DiffuseAttribute				lineColorAttribute;
			DiffuseTextureAttribute			lineTextureAttribute;
			Renderable						lineRenderable;

			Point3D GetConnectorPosition(const Transform4D& cameraTransform, float scale) const;

		public:

			EditorConnector(const EditorManipulator *manipulator, Connector *connector, int32 index);
			~EditorConnector();

			Node *GetConnectorTarget(void) const
			{
				return (connectorObject->GetConnectorTarget());
			}

			void Select(void);
			void Unselect(void);

			bool Pick(const ManipulatorViewportData *viewportData, const Ray *ray) const;

			void RenderBox(const ManipulatorViewportData *viewportData, List<Renderable> *renderList);
			void RenderLine(const ManipulatorViewportData *viewportData, List<Renderable> *renderList);
	};
}


#endif

// ZYUQURM
