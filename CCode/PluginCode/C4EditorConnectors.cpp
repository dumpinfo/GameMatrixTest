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


#include "C4EditorConnectors.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kConnectorBackgroundColor = {0.125F, 0.125F, 0.125F, 1.0F};
	const ConstColorRGBA kConnectorLineColor = {1.0F, 0.75F, 0.25F, 1.0F};
}


EditorConnector::EditorConnector(const EditorManipulator *manipulator, Connector *connector, int32 index) :
		lineWidget1(Vector2D(32.0F, 1.0F), kLineDotted1, K::white),
		lineWidget2(Vector2D(24.0F, 1.0F), kLineDotted1, K::white),
		backgroundWidget(Vector2D(96.0F, 18.0F), kConnectorBackgroundColor),
		borderWidget(Vector2D(96.0F, 18.0F), kLineSolid, K::white),
		textWidget(Vector2D(96.0F, 18.0F), connector->GetConnectorKey(), "font/Page"),
		lineVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		lineColorAttribute(kConnectorLineColor, kAttributeMutable),
		lineTextureAttribute("WorldEditor/connection"),
		lineRenderable(kRenderTriangleStrip)
{
	connectorObject = connector;
	connectorNode = manipulator->GetTargetNode();
	connectorIndex = index;

	lineWidget1.SetWidgetPosition(Point3D(0.0F, 9.0F, 0.0F));
	lineWidget2.SetWidgetTransform(K::minus_y_unit, K::x_unit, K::z_unit, Point3D(0.0F, 10.0F, 0.0F));

	backgroundWidget.SetWidgetPosition(Point3D(32.0F, 0.0F, 0.0F));
	borderWidget.SetWidgetPosition(Point3D(32.0F, 0.0F, 0.0F));

	textWidget.SetWidgetColor(K::white);
	textWidget.SetTextAlignment(kTextAlignCenter);
	textWidget.SetWidgetPosition(Point3D(32.0F, 3.0F, 0.0F));

	groupWidget.AppendSubnode(&lineWidget1);
	groupWidget.AppendSubnode(&lineWidget2);
	groupWidget.AppendSubnode(&backgroundWidget);
	groupWidget.AppendSubnode(&borderWidget);
	groupWidget.AppendSubnode(&textWidget);
	groupWidget.Preprocess();

	lineRenderable.SetAmbientBlendState(kBlendInterpolate);
	lineRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard);
	lineRenderable.SetRenderParameterPointer(&manipulator->GetManipulatorScaleVector());

	lineRenderable.SetVertexCount(66);
	lineRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &lineVertexBuffer, sizeof(ConnectorVertex));
	lineRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	lineRenderable.SetVertexAttributeArray(kArrayTangent, sizeof(Point3D), 4);
	lineRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector4D), 2);
	lineVertexBuffer.Establish(sizeof(ConnectorVertex) * 66);

	lineAttributeList.Append(&lineColorAttribute);
	lineAttributeList.Append(&lineTextureAttribute);
	lineRenderable.SetMaterialAttributeList(&lineAttributeList);
}

EditorConnector::~EditorConnector()
{
}

void EditorConnector::Select(void)
{
	ColorRGBA color = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite);
	backgroundWidget.SetVertexColor(1, color);
	backgroundWidget.SetVertexColor(3, color);

	color.red *= 0.75F;
	color.green *= 0.75F;
	color.blue *= 0.75F;
	backgroundWidget.SetVertexColor(0, color);
	backgroundWidget.SetVertexColor(2, color);

	lineColorAttribute.SetDiffuseColor(K::white);
}

void EditorConnector::Unselect(void)
{
	backgroundWidget.SetWidgetColor(kConnectorBackgroundColor);
	lineColorAttribute.SetDiffuseColor(kConnectorLineColor);
}

bool EditorConnector::Pick(const ManipulatorViewportData *viewportData, const Ray *ray) const
{
	float scale = viewportData->viewportScale;
	const Transform4D& cameraTransform = viewportData->viewportCamera->GetNodeTransform();
	Vector3D p = ray->origin - GetConnectorPosition(cameraTransform, scale);

	scale = 1.0F / scale;
	float x = p * cameraTransform[0] * scale;
	float y = p * cameraTransform[1] * scale;

	return ((x > 32.0F) && (x < 128.0F) && (y > 0.0F) && (y < 18.0F));
}

Point3D EditorConnector::GetConnectorPosition(const Transform4D& cameraTransform, float scale) const 
{
	Vector3D position = connectorNode->GetWorldPosition() - cameraTransform.GetTranslation();
	position.x = Floor(position.x / scale); 
	position.y = Floor(position.y / scale);
	position.z = Floor(position.z / scale); 

	position += cameraTransform[1] * ((float) connectorIndex * 24.0F + 13.0F);
	return (cameraTransform.GetTranslation() + position * scale); 
}
 
void EditorConnector::RenderBox(const ManipulatorViewportData *viewportData, List<Renderable> *renderList) 
{
	float scale = viewportData->viewportScale;
	const Transform4D& cameraTransform = viewportData->viewportCamera->GetNodeTransform();
 
	groupWidget.SetWidgetTransform(cameraTransform[0] * scale, cameraTransform[1] * scale, cameraTransform[2] * scale, GetConnectorPosition(cameraTransform, scale));
	groupWidget.Invalidate();
	groupWidget.Update();

	groupWidget.RenderTree(renderList);
}

void EditorConnector::RenderLine(const ManipulatorViewportData *viewportData, List<Renderable> *renderList)
{
	const Node *target = GetConnectorTarget();
	if (target)
	{
		float	texcoord[33];

		volatile ConnectorVertex *restrict vertex = lineVertexBuffer.BeginUpdate<ConnectorVertex>();

		float scale = viewportData->viewportScale;
		const Transform4D& cameraTransform = viewportData->viewportCamera->GetNodeTransform();

		Point3D p1 = GetConnectorPosition(cameraTransform, scale) + cameraTransform[0] * (scale * 128.0F) + cameraTransform[1] * (scale * 9.0F);
		Point3D p2 = p1 + cameraTransform[0] * (scale * 256.0F);
		Point3D texpoint = p1 - ProjectOnto(p1, cameraTransform[2]);

		const EditorManipulator *targetManipulator = Editor::GetManipulator(target);
		const BoundingSphere *sphere = targetManipulator->GetNodeSphere();
		const Point3D& p3 = ((sphere) && (!(targetManipulator->GetManipulatorState() & kManipulatorShowIcon))) ? sphere->GetCenter() : target->GetWorldPosition();

		Vector3D tangent = Normalize(p2 - p1);
		float radius = scale * 6.0F;

		vertex[0].position = p1;
		vertex[0].tangent.Set(tangent, -radius);

		vertex[1].position = p1;
		vertex[1].tangent.Set(tangent, radius);

		float u = 0.03125F;
		float dtex = 0.03125F / scale;
		float length = 0.0F;
		texcoord[0] = 0.0F;

		for (machine a = 2; a < 66; a += 2)
		{
			float v = 1.0F - u;
			float u2 = u * u;
			float v2 = v * v;

			Point3D p = p1 * v2 + p2 * (u * v * 2.0F) + p3 * u2;
			Vector3D t = p1 * -v + p2 * (1.0F - u * 2.0F) + p3 * u;
			t.Normalize();

			vertex[a].position = p;
			vertex[a].tangent.Set(t, -radius);

			vertex[a + 1].position = p;
			vertex[a + 1].tangent.Set(t, radius);

			p -= ProjectOnto(p, cameraTransform[2]);
			length += Magnitude(p - texpoint) * dtex;
			texcoord[a >> 1] = length;
			texpoint = p;

			u += 0.03125F;
		}

		length = PositiveCeil(length) / length;

		for (machine a = 0; a < 33; a++)
		{
			float s = texcoord[a] * length;
			vertex[0].texcoord.Set(s, 0.0F);
			vertex[1].texcoord.Set(s, 1.0F);
			vertex += 2;
		}

		lineVertexBuffer.EndUpdate();
		renderList->Append(&lineRenderable);
	}
}

// ZYUQURM
