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


#include "C4EditorManipulators.h"
#include "C4GeometryManipulators.h"
#include "C4CameraManipulators.h"
#include "C4LightManipulators.h"
#include "C4SourceManipulators.h"
#include "C4ZoneManipulators.h"
#include "C4PortalManipulators.h"
#include "C4SpaceManipulators.h"
#include "C4MarkerManipulators.h"
#include "C4TriggerManipulators.h"
#include "C4EffectManipulators.h"
#include "C4EmitterManipulators.h"
#include "C4InstanceManipulators.h"
#include "C4ModelManipulators.h"
#include "C4PhysicsManipulators.h"
#include "C4TerrainTools.h"
#include "C4WaterTools.h"
#include "C4WorldEditor.h"
#include "C4EditorSupport.h"
#include "C4EditorGizmo.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kUnselectedMarkerColor = {0.5F, 0.5F, 0.5F, 1.0F};
}


SharedVertexBuffer EditorManipulator::markerVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);

SharedVertexBuffer EditorManipulator::boxVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic);
SharedVertexBuffer EditorManipulator::boxIndexBuffer(kVertexBufferIndex | kVertexBufferStatic);


const ConstPoint3D EditorManipulator::manipulatorBoxPickEdge[12][2] =
{
	{{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}},
	{{1.0F, 0.0F, 0.0F}, {1.0F, 1.0F, 0.0F}},
	{{1.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}},
	{{0.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 0.0F}},
	{{0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}},
	{{1.0F, 0.0F, 1.0F}, {1.0F, 1.0F, 1.0F}},
	{{1.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}},
	{{0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F}},
	{{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F}},
	{{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 1.0F}},
	{{1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 1.0F}},
	{{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 1.0F}}
};

const ConstPoint3D EditorManipulator::manipulatorBoxPosition[kManipulatorBoxVertexCount] =
{
	{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F},
	{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F},
	{1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F},
	{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
	{0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F},
	{1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F},
	{1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F},
	{0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 1.0F, 1.0F}
};

const ConstVector3D EditorManipulator::manipulatorBoxOffset[kManipulatorBoxVertexCount] =
{
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F},
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F},
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F},
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F},
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F},
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F},
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F},
	{-1.0F, -1.0F, -1.0F}, {1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, -1.0F}, {-1.0F, 1.0F, -1.0F}, {-1.0F, -1.0F, 1.0F}, {1.0F, -1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}, {-1.0F, 1.0F, 1.0F}
};

const Triangle EditorManipulator::manipulatorBoxTriangle[kManipulatorBoxTriangleCount] =
{
	{{ 1,  8,  5}}, {{ 5,  8, 12}}, {{ 5, 12,  6}}, {{ 6, 12, 15}}, {{ 6, 15,  2}}, {{ 2, 15, 11}}, {{ 2, 11,  1}}, {{ 1, 11,  8}},
	{{10, 17, 14}}, {{14, 17, 21}}, {{14, 21, 15}}, {{15, 21, 20}}, {{15, 20, 11}}, {{11, 20, 16}}, {{11, 16, 10}}, {{10, 16, 17}},
	{{19, 26, 23}}, {{23, 26, 30}}, {{23, 30, 20}}, {{20, 30, 29}}, {{20, 29, 16}}, {{16, 29, 25}}, {{16, 25, 19}}, {{19, 25, 26}},
	{{24,  3, 28}}, {{28,  3,  7}}, {{28,  7, 29}}, {{29,  7,  6}}, {{29,  6, 25}}, {{25,  6,  2}}, {{25,  2, 24}}, {{24,  2,  3}},
	{{33, 40, 37}}, {{37, 40, 44}}, {{37, 44, 38}}, {{38, 44, 47}}, {{38, 47, 34}}, {{34, 47, 43}}, {{34, 43, 33}}, {{33, 43, 40}},
	{{42, 49, 46}}, {{46, 49, 53}}, {{46, 53, 47}}, {{47, 53, 52}}, {{47, 52, 43}}, {{43, 52, 48}}, {{43, 48, 42}}, {{42, 48, 49}},
	{{51, 58, 55}}, {{55, 58, 62}}, {{55, 62, 52}}, {{52, 62, 61}}, {{52, 61, 48}}, {{48, 61, 57}}, {{48, 57, 51}}, {{51, 57, 58}},
	{{56, 35, 60}}, {{60, 35, 39}}, {{60, 39, 61}}, {{61, 39, 38}}, {{61, 38, 57}}, {{57, 38, 34}}, {{57, 34, 56}}, {{56, 34, 35}},
	{{ 6, 34,  5}}, {{ 5, 34, 33}}, {{ 5, 33,  4}}, {{ 4, 33, 32}}, {{ 4, 32,  7}}, {{ 7, 32, 35}}, {{ 7, 35,  6}}, {{ 6, 35, 34}},
	{{14, 42, 13}}, {{13, 42, 41}}, {{13, 41, 12}}, {{12, 41, 40}}, {{12, 40, 15}}, {{15, 40, 43}}, {{15, 43, 14}}, {{14, 43, 42}},
	{{22, 50, 21}}, {{21, 50, 49}}, {{21, 49, 20}}, {{20, 49, 48}}, {{20, 48, 23}}, {{23, 48, 51}}, {{23, 51, 22}}, {{22, 51, 50}},
	{{30, 58, 29}}, {{29, 58, 57}}, {{29, 57, 28}}, {{28, 57, 56}}, {{28, 56, 31}}, {{31, 56, 59}}, {{31, 59, 30}}, {{30, 59, 58}},
	{{ 0,  1,  4}}, {{ 1,  5,  4}}, {{ 3,  2,  0}}, {{ 2,  1,  0}}, {{ 3,  0,  7}}, {{ 0,  4,  7}},
	{{ 9, 10, 13}}, {{10, 14, 13}}, {{ 8, 11,  9}}, {{11, 10,  9}}, {{ 8,  9, 12}}, {{ 9, 13, 12}},
	{{18, 19, 22}}, {{19, 23, 22}}, {{17, 16, 18}}, {{16, 19, 18}}, {{17, 18, 21}}, {{18, 22, 21}},
	{{27, 24, 31}}, {{24, 28, 31}}, {{26, 25, 27}}, {{25, 24, 27}}, {{26, 27, 30}}, {{27, 31, 30}},
	{{32, 33, 36}}, {{33, 37, 36}}, {{39, 36, 38}}, {{38, 36, 37}}, {{35, 32, 39}}, {{32, 36, 39}},
	{{41, 42, 45}}, {{42, 46, 45}}, {{44, 45, 47}}, {{47, 45, 46}}, {{40, 41, 44}}, {{41, 45, 44}},
	{{50, 51, 54}}, {{51, 55, 54}}, {{53, 54, 52}}, {{52, 54, 55}}, {{49, 50, 53}}, {{50, 54, 53}},
	{{59, 56, 63}}, {{56, 60, 63}}, {{62, 63, 61}}, {{61, 63, 60}}, {{58, 59, 62}}, {{59, 63, 62}}
};
 

const TextureHeader EditorManipulator::outlineTextureHeader =
{ 
	kTexture2D,
	kTextureForceHighQuality, 
	kTextureSemanticEmission,
	kTextureSemanticTransparency,
	kTextureI8, 
	8, 1, 1,
	{kTextureClampBorder, kTextureRepeat, kTextureRepeat}, 
	4 
};


const unsigned_int8 EditorManipulator::outlineTextureImage[15] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF,
	0xFF
};


ManipulatorWidget::ManipulatorWidget(EditorManipulator *manipulator) :
		RenderableWidget(kWidgetManipulator, kRenderQuads, Vector2D(kGraphBoxWidth, kGraphBoxHeight)),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		diffuseAttribute(K::black)
{
	editorManipulator = manipulator;
	viewportScale = 1.0F;
}

ManipulatorWidget::~ManipulatorWidget()
{
}

void ManipulatorWidget::Preprocess(void)
{
	RenderableWidget::Preprocess();

	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point2D));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	vertexBuffer.Establish(sizeof(Point2D) * 28);

	attributeList.Append(&diffuseAttribute);
	SetMaterialAttributeList(&attributeList);
}

void ManipulatorWidget::Build(void)
{
	volatile Point2D *restrict vertex = vertexBuffer.BeginUpdate<Point2D>();

	float scale = viewportScale;

	vertex[0].Set(-scale, -scale);
	vertex[1].Set(-scale, 0.0F);
	vertex[2].Set(kGraphBoxWidth + scale, 0.0F);
	vertex[3].Set(kGraphBoxWidth + scale, -scale);

	vertex[4].Set(-scale, kGraphBoxHeight);
	vertex[5].Set(-scale, kGraphBoxHeight + scale);
	vertex[6].Set(kGraphBoxWidth + scale, kGraphBoxHeight + scale);
	vertex[7].Set(kGraphBoxWidth + scale, kGraphBoxHeight);

	vertex[8].Set(-scale, 0.0F);
	vertex[9].Set(-scale, kGraphBoxHeight);
	vertex[10].Set(0.0F, kGraphBoxHeight);
	vertex[11].Set(0.0F, 0.0F);

	vertex[12].Set(kGraphBoxWidth, 0.0F);
	vertex[13].Set(kGraphBoxWidth, kGraphBoxHeight);
	vertex[14].Set(kGraphBoxWidth + scale, kGraphBoxHeight);
	vertex[15].Set(kGraphBoxWidth + scale, 0.0F);

	int32 count = 16;
	const Node *node = editorManipulator->GetTargetNode();

	if (node->GetFirstSubnode())
	{
		count = 20;
		vertex[16].Set(kGraphBoxWidth + 1.0F, 7.0F);
		vertex[17].Set(kGraphBoxWidth + 1.0F, 7.0F + scale);
		vertex[18].Set(kGraphBoxWidth + 10.0F, 7.0F + scale);
		vertex[19].Set(kGraphBoxWidth + 10.0F, 7.0F);
	}

	if (node->GetSuperNode())
	{
		const Node *previous = node->Previous();
		if (previous)
		{
			vertex[count].Set(-14.0F, 7.0F);
			vertex[count + 1].Set(-14.0F, 7.0F + scale);
			vertex[count + 2].Set(-1.0F, 7.0F + scale);
			vertex[count + 3].Set(-1.0F, 7.0F);

			float h = static_cast<EditorManipulator *>(previous->GetManipulator())->GetGraphHeight();
			float y = (previous->Previous()) ? 8.0F - h : 13.0F - h;

			vertex[count + 4].Set(-15.0F, y);
			vertex[count + 5].Set(-15.0F, 8.0F);
			vertex[count + 6].Set(scale - 15.0F, 8.0F);
			vertex[count + 7].Set(scale - 15.0F, y);
			count += 8;
		}
		else
		{
			vertex[count].Set(-9.0F, 7.0F);
			vertex[count + 1].Set(-9.0F, 7.0F + scale);
			vertex[count + 2].Set(-1.0F, 7.0F + scale);
			vertex[count + 3].Set(-1.0F, 7.0F);
			count += 4;
		}
	}

	SetVertexCount(count);
	vertexBuffer.EndUpdate();
}


EditorManipulator::EditorManipulator(Node *node, const char *iconName) :
		Manipulator(node),
		markerColorAttribute(kUnselectedMarkerColor, kAttributeMutable),
		markerTextureAttribute("WorldEditor/marker"),
		markerRenderable(kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		iconTextureAttribute(iconName),
		iconRenderable(kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		handleVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		handleRenderable(kRenderQuads),
		graphBackground(Vector2D(kGraphBoxWidth + 8.0F, kGraphBoxHeight + 8.0F), "WorldEditor/graph"),
		graphImage(Vector2D(16.0F, 16.0F)),
		graphText(Vector2D(kGraphBoxWidth - 21.0F, kGraphBoxHeight), nullptr, "font/Normal"),
		graphBorder(this),
		graphCollapseButton(Vector2D(13.0F, 13.0F), Point2D(0.5625F, 0.6875F), Point2D(0.6875F, 0.8125F)),
		graphCollapseObserver(this, &EditorManipulator::HandleGraphCollapseEvent)
{
	manipulatorFlags = 0;

	worldEditor = nullptr;
	editorGizmo = nullptr;

	selectionType = kEditorSelectionObject;

	nodeSpherePointer = nullptr;
	treeSpherePointer = nullptr;

	handleCount = 0;
	connectorCount = 0;
	connectorStorage = nullptr;

	if (markerVertexBuffer.Retain() == 1)
	{
		static const MarkerVertex markerVertex[8] =
		{
			{Point2D(0.0F, 0.0F), Vector2D(-36.0F, -36.0F), Point2D(0.0F, 1.0F)},
			{Point2D(0.0F, 0.0F), Vector2D(-36.0F, 12.0F), Point2D(0.0F, 0.0F)},
			{Point2D(0.0F, 0.0F), Vector2D(12.0F, -36.0F), Point2D(1.0F, 1.0F)},
			{Point2D(0.0F, 0.0F), Vector2D(12.0F, 12.0F), Point2D(1.0F, 0.0F)},
			{Point2D(0.0F, 0.0F), Vector2D(-34.5F, -34.5F), Point2D(0.0F, 1.0F)},
			{Point2D(0.0F, 0.0F), Vector2D(-34.5F, -13.5F), Point2D(0.0F, 0.0F)},
			{Point2D(0.0F, 0.0F), Vector2D(-13.5F, -34.5F), Point2D(1.0F, 1.0F)},
			{Point2D(0.0F, 0.0F), Vector2D(-13.5F, -13.5F), Point2D(1.0F, 0.0F)}
		};

		markerVertexBuffer.Establish(sizeof(MarkerVertex) * 8, markerVertex);
	}

	markerRenderable.SetAmbientBlendState(kBlendInterpolate);
	markerRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexBillboard | kShaderScaleVertex);
	markerRenderable.SetRenderParameterPointer(&manipulatorScaleVector);
	markerRenderable.SetTransformable(node);

	markerRenderable.SetVertexCount(4);
	markerRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &markerVertexBuffer, sizeof(MarkerVertex));
	markerRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	markerRenderable.SetVertexAttributeArray(kArrayBillboard, sizeof(Point2D), 2);
	markerRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) * 2, 2);

	markerAttributeList.Append(&markerColorAttribute);
	markerAttributeList.Append(&markerTextureAttribute);
	markerRenderable.SetMaterialAttributeList(&markerAttributeList);

	iconRenderable.SetAmbientBlendState(kBlendInterpolate);
	iconRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexBillboard | kShaderScaleVertex);
	iconRenderable.SetRenderParameterPointer(&manipulatorScaleVector);
	iconRenderable.SetTransformable(node);

	iconRenderable.SetVertexCount(4);
	iconRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &markerVertexBuffer, sizeof(MarkerVertex));
	iconRenderable.SetVertexAttributeArray(kArrayPosition, sizeof(MarkerVertex) * 4, 2);
	iconRenderable.SetVertexAttributeArray(kArrayBillboard, sizeof(MarkerVertex) * 4 + sizeof(Point2D), 2);
	iconRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(MarkerVertex) * 4 + sizeof(Point2D) * 2, 2);

	iconAttributeList.Append(&iconTextureAttribute);
	iconRenderable.SetMaterialAttributeList(&iconAttributeList);

	handleRenderable.SetAmbientBlendState(kBlendInterpolate);
	handleRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexBillboard | kShaderScaleVertex);
	handleRenderable.SetRenderParameterPointer(&manipulatorScaleVector);
	handleRenderable.SetTransformable(node);

	handleRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &handleVertexBuffer, sizeof(HandleVertex));
	handleRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	handleRenderable.SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D), 2);

	graphBackground.SetQuadOffset(Vector2D(-4.0F, -1.0F));

	graphText.SetWidgetPosition(Point3D(21.0F, 2.0F, 0.0F));
	graphText.SetTextFlags(kTextUnformatted | kTextClipped);

	graphCollapseButton.SetWidgetPosition(Point3D(120.0F, 2.0F, 0.0F));
	graphCollapseButton.SetWidgetColor(ColorRGBA(1.0F, 1.0F, 0.5F, 1.0F));
	graphCollapseButton.SetObserver(&graphCollapseObserver);

	graphBackground.AppendSubnode(&graphImage);
	graphBackground.AppendSubnode(&graphText);
	graphBackground.AppendSubnode(&graphBorder);
	graphBackground.AppendSubnode(&graphCollapseButton);
	graphBackground.Preprocess();
}

EditorManipulator::~EditorManipulator()
{
	ReleaseConnectorStorage();
	delete editorGizmo;

	markerVertexBuffer.Release();
}

Manipulator *EditorManipulator::Create(Node *node, unsigned_int32 flags)
{
	switch (node->GetNodeType())
	{
		case kNodeGeneric:

			return (new GroupManipulator(node));

		case kNodeCamera:

			return (CameraManipulator::Create(static_cast<Camera *>(node)));

		case kNodeLight:

			return (LightManipulator::Create(static_cast<Light *>(node)));

		case kNodeSource:

			return (SourceManipulator::Create(static_cast<Source *>(node)));

		case kNodeGeometry:

			return (GeometryManipulator::Create(static_cast<Geometry *>(node)));

		case kNodeInstance:

			return (new InstanceManipulator(static_cast<Instance *>(node)));

		case kNodeModel:

			return (new ModelManipulator(static_cast<Model *>(node)));

		case kNodeBone:

			return (new BoneManipulator(static_cast<Bone *>(node)));

		case kNodeMarker:

			return (MarkerManipulator::Create(static_cast<Marker *>(node)));

		case kNodeTrigger:

			return (TriggerManipulator::Create(static_cast<Trigger *>(node)));

		case kNodeEffect:

			return (EffectManipulator::Create(static_cast<Effect *>(node)));

		case kNodeEmitter:

			return (EmitterManipulator::Create(static_cast<Emitter *>(node)));

		case kNodeSpace:

			return (SpaceManipulator::Create(static_cast<Space *>(node)));

		case kNodePortal:

			return (PortalManipulator::Create(static_cast<Portal *>(node)));

		case kNodeZone:

			return (ZoneManipulator::Create(static_cast<Zone *>(node)));

		case kNodeShape:

			return (ShapeManipulator::Create(static_cast<Shape *>(node)));

		case kNodeJoint:

			return (JointManipulator::Create(static_cast<Joint *>(node)));

		case kNodeField:

			return (FieldManipulator::Create(static_cast<Field *>(node)));

		case kNodeBlocker:

			return (BlockerManipulator::Create(static_cast<Blocker *>(node)));

		case kNodePhysics:

			return (new PhysicsNodeManipulator(static_cast<PhysicsNode *>(node)));

		case kNodeSkybox:

			return (new SkyboxManipulator(static_cast<Skybox *>(node)));

		case kNodeImpostor:

			return (new ImpostorManipulator(static_cast<Impostor *>(node)));

		case kNodeTerrainBlock:

			return (new TerrainBlockManipulator(static_cast<TerrainBlock *>(node)));

		case kNodeWaterBlock:

			return (new WaterBlockManipulator(static_cast<WaterBlock *>(node)));
	}

	return (nullptr);
}

void EditorManipulator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Manipulator::Pack(data, packFlags);

	if (graphCollapseButton.GetWidgetState() & kWidgetCollapsed)
	{
		data << ChunkHeader('CLPS', 0);
	}

	data << TerminatorChunk;
}

void EditorManipulator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Manipulator::Unpack(data, unpackFlags);
	UnpackChunkList<EditorManipulator>(data, unpackFlags);
}

bool EditorManipulator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CLPS':

			graphCollapseButton.SetWidgetState(graphCollapseButton.GetWidgetState() | kWidgetCollapsed);
			return (true);
	}

	return (false);
}

const char *EditorManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', 'NODE')));
}

void EditorManipulator::Preprocess(void)
{
	Manipulator::Preprocess();

	AllocateConnectorStorage();

	UpdateGraphColor();
	graphImage.SetTexture(0, GetIconName());

	if (GetTargetNode()->GetNodeFlags() & kNodeNonpersistent)
	{
		SetManipulatorState(GetManipulatorState() & ~kManipulatorShowIcon);
	}
}

void EditorManipulator::Invalidate(void)
{
	EditorManipulator *manipulator = this;
	for (;;)
	{
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kManipulatorUpdated);

		Node *super = manipulator->GetTargetNode()->GetSuperNode();
		if (!super)
		{
			break;
		}

		manipulator = static_cast<EditorManipulator *>(super->GetManipulator());
		if ((!manipulator) || (!(manipulator->GetManipulatorState() & kManipulatorUpdated)))
		{
			break;
		}
	}

	if (GetManipulatorState() & kManipulatorShowGizmo)
	{
		worldEditor->PostEvent(GizmoEditorEvent(kEditorEventGizmoTargetInvalidated, GetTargetNode()));
	}
}

void EditorManipulator::InvalidateGraph(void)
{
	unsigned_int32 state = GetManipulatorState();
	if (state & kManipulatorGraphValid)
	{
		SetManipulatorState(state & ~kManipulatorGraphValid);
		InvalidateGraphTree();
	}

	Node *node = GetTargetNode();
	for (;;)
	{
		Node *previous = node->Previous();
		if (previous)
		{
			EditorManipulator *manipulator = static_cast<EditorManipulator *>(previous->GetManipulator());
			state = manipulator->GetManipulatorState();
			if (state & kManipulatorGraphValid)
			{
				manipulator->SetManipulatorState(state & ~kManipulatorGraphValid);
				manipulator->InvalidateGraphTree();
			}
		}

		Node *next = node->Next();
		while (next)
		{
			EditorManipulator *manipulator = static_cast<EditorManipulator *>(next->GetManipulator());
			state = manipulator->GetManipulatorState();
			if (!(state & kManipulatorGraphValid))
			{
				break;
			}

			manipulator->SetManipulatorState(state & ~kManipulatorGraphValid);
			manipulator->InvalidateGraphTree();

			next = next->Next();
		}

		node = node->GetSuperNode();
		if (!node)
		{
			break;
		}

		EditorManipulator *manipulator = static_cast<EditorManipulator *>(node->GetManipulator());
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kManipulatorGraphValid);
		manipulator->graphBackground.Invalidate();
	}
}

void EditorManipulator::InvalidateGraphTree(void)
{
	graphBackground.Invalidate();

	Node *subnode = GetTargetNode()->GetFirstSubnode();
	while (subnode)
	{
		EditorManipulator *manipulator = static_cast<EditorManipulator *>(subnode->GetManipulator());
		unsigned_int32 state = manipulator->GetManipulatorState();
		if (state & kManipulatorGraphValid)
		{
			manipulator->SetManipulatorState(state & ~kManipulatorGraphValid);
			manipulator->InvalidateGraphTree();
		}

		subnode = subnode->Next();
	}
}

void EditorManipulator::InvalidateNode(void)
{
	GetTargetNode()->Invalidate();
}

void EditorManipulator::EnableGizmo(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowGizmo);

	if (!editorGizmo)
	{
		editorGizmo = new EditorGizmo(worldEditor, this);
	}
}

void EditorManipulator::DisableGizmo(void)
{
	SetManipulatorState(GetManipulatorState() & ~kManipulatorShowGizmo);

	delete editorGizmo;
	editorGizmo = nullptr;
}

void EditorManipulator::Update(void)
{
	unsigned_int32 state = GetManipulatorState();
	if (!(state & kManipulatorUpdated))
	{
		SetManipulatorState(state | kManipulatorUpdated);
		const Node *node = GetTargetNode();

		const Node *subnode = node->GetFirstSubnode();
		while (subnode)
		{
			EditorManipulator *manipulator = static_cast<EditorManipulator *>(subnode->GetManipulator());
			manipulator->Update();

			subnode = subnode->Next();
		}

		nodeSpherePointer = nullptr;
		treeSpherePointer = nullptr;

		bool icon = ((state & kManipulatorShowIcon) != 0);

		if (CalculateNodeSphere(&nodeSphere))
		{
			nodeSphere.SetCenter(node->GetWorldTransform() * nodeSphere.GetCenter());
			nodeSpherePointer = &nodeSphere;

			if (icon)
			{
				BoundingSphere sphere(node->GetWorldPosition(), Editor::kFrustumRenderScale * 12.0F);
				nodeSphere.Union(&sphere);
			}

			treeSphere = nodeSphere;
			treeSpherePointer = &treeSphere;
		}
		else if (icon)
		{
			nodeSphere.SetCenter(node->GetWorldPosition());
			nodeSphere.SetRadius(Editor::kFrustumRenderScale * 12.0F);
			nodeSpherePointer = &nodeSphere;

			treeSphere = nodeSphere;
			treeSpherePointer = &treeSphere;
		}
		else
		{
			nodeSphere.SetCenter(node->GetWorldPosition());
			nodeSphere.SetRadius(Editor::kFrustumRenderScale);
			nodeSpherePointer = &nodeSphere;

			treeSphere = nodeSphere;
			treeSpherePointer = &treeSphere;
		}

		int32 count = GetHandleTable(handlePosition);
		handleCount = count;

		handleRenderable.InvalidateVertexData();
		handleRenderable.SetVertexCount(count * 4);
		handleVertexBuffer.Establish(count * (sizeof(HandleVertex) * 4));

		if (count != 0)
		{
			volatile HandleVertex *restrict vertex = handleVertexBuffer.BeginUpdate<HandleVertex>();

			for (machine a = 0; a < count; a++)
			{
				const Point3D& p = handlePosition[a];

				vertex[0].position = p;
				vertex[0].billboard.Set(-3.0F, -3.0F);

				vertex[1].position = p;
				vertex[1].billboard.Set(-3.0F, 3.0F);

				vertex[2].position = p;
				vertex[2].billboard.Set(3.0F, 3.0F);

				vertex[3].position = p;
				vertex[3].billboard.Set(3.0F, -3.0F);

				vertex += 4;
			}

			handleVertexBuffer.EndUpdate();
		}

		subnode = GetTargetNode()->GetFirstSubnode();
		while (subnode)
		{
			EditorManipulator *manipulator = static_cast<EditorManipulator *>(subnode->GetManipulator());

			const BoundingSphere *sphere = manipulator->GetTreeSphere();
			if (sphere)
			{
				if (treeSpherePointer)
				{
					treeSphere.Union(sphere);
				}
				else
				{
					treeSphere = *sphere;
					nodeSpherePointer = &treeSphere;
					treeSpherePointer = &treeSphere;
				}
			}

			subnode = subnode->Next();
		}
	}
}

void EditorManipulator::UpdateGraph(void)
{
	unsigned_int32 state = GetManipulatorState();
	if (!(state & kManipulatorGraphValid))
	{
		SetManipulatorState(state | kManipulatorGraphValid);
		const Node *node = GetTargetNode();

		const Node *previous = node->Previous();
		if (previous)
		{
			const EditorManipulator *manipulator = static_cast<EditorManipulator *>(previous->GetManipulator());
			const Point3D& position = manipulator->GetGraphPosition();
			graphBackground.SetWidgetPosition(Point3D(position.x, position.y + manipulator->GetGraphHeight(), 0.0F));
		}
		else
		{
			const Node *super = node->GetSuperNode();
			if (super)
			{
				const EditorManipulator *manipulator = static_cast<EditorManipulator *>(super->GetManipulator());
				const Point3D& position = manipulator->GetGraphPosition();
				graphBackground.SetWidgetPosition(Point3D(position.x + kGraphBoxWidth + 29.0F, position.y, 0.0F));
			}
			else
			{
				graphBackground.SetWidgetPosition(Point3D(0.0F, 0.0F, 0.0F));
			}
		}

		const char *name = node->GetNodeName();
		graphText.SetText((name) ? name : GetDefaultNodeName());

		const Node *subnode = node->GetFirstSubnode();
		if (subnode)
		{
			float width = 0.0F;
			float height = 0.0F;
			do
			{
				EditorManipulator *manipulator = static_cast<EditorManipulator *>(subnode->GetManipulator());
				manipulator->UpdateGraph();

				width = Fmax(width, manipulator->GetGraphWidth());
				height += manipulator->GetGraphHeight();

				subnode = subnode->Next();
			} while (subnode);

			if (!(graphCollapseButton.GetWidgetState() & kWidgetCollapsed))
			{
				graphWidth = width + kGraphBoxWidth + 29.0F;
				graphHeight = height;
			}
			else
			{
				graphWidth = kGraphBoxWidth + 29.0F;
				graphHeight = kGraphBoxHeight + 12.0F;
			}

			graphCollapseButton.Show();
		}
		else
		{
			graphWidth = kGraphBoxWidth;

			const Node *next = node->Next();
			if ((next) && (!next->GetFirstSubnode()))
			{
				graphHeight = kGraphBoxHeight + 8.0F;
			}
			else
			{
				graphHeight = kGraphBoxHeight + 12.0F;
			}

			graphCollapseButton.Hide();
		}

		graphBackground.Update();
	}
}

void EditorManipulator::UpdateGraphColor(void)
{
	if (!(GetTargetNode()->GetNodeFlags() & kNodeNonpersistent))
	{
		if (!Selected())
		{
			if (!Hidden())
			{
				graphBackground.SetWidgetColor(ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F));
			}
			else
			{
				graphBackground.SetWidgetColor(ColorRGBA(0.5F, 0.5F, 0.5F, 1.0F));
			}
		}
		else
		{
			graphBackground.SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorHilite));
		}
	}
	else
	{
		graphBackground.SetWidgetColor(ColorRGBA(1.0F, 0.5F, 0.5F, 1.0F));
	}
}

void EditorManipulator::HandleGraphCollapseEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		InvalidateGraph();
		widget->SetWidgetState(widget->GetWidgetState() ^ kWidgetCollapsed);
	}
}

bool EditorManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	if (!GetTargetNode()->CalculateBoundingSphere(sphere))
	{
		sphere->SetCenter(Zero3D);
		sphere->SetRadius(0.0F);
	}

	return (true);
}

bool EditorManipulator::PickLineSegment(const Ray *ray, const Point3D& p1, const Point3D& p2, float r2, float *param)
{
	float	u1, u2;

	Vector3D dp = p2 - p1;
	if (Math::CalculateNearestParameters(ray->origin, ray->direction, p1, dp, &u1, &u2))
	{
		if ((u1 > ray->tmin) && (u1 < ray->tmax) && (u2 > 0.0F) && (u2 < 1.0F))
		{
			if (SquaredMag(ray->origin + ray->direction * u1 - p1 - dp * u2) < r2)
			{
				*param = u1;
				return (true);
			}
		}
	}

	return (false);
}

bool EditorManipulator::RegionPickLineSegment(const VisibilityRegion *region, const Point3D& p1, const Point3D& p2) const
{
	const Transform4D& worldTransform = GetTargetNode()->GetWorldTransform();
	return (region->CylinderVisible(worldTransform * p1, worldTransform * p2, 0.0F));
}

void EditorManipulator::Show(void)
{
	SetManipulatorState(GetManipulatorState() & ~kManipulatorHidden);
	UpdateGraphColor();
}

void EditorManipulator::Hide(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorHidden);
	UpdateGraphColor();
}

bool EditorManipulator::PredecessorSelected(void) const
{
	const Node *node = GetTargetNode();
	for (;;)
	{
		node = node->GetSuperNode();
		if (!node)
		{
			break;
		}

		if (node->GetManipulator()->Selected())
		{
			return (true);
		}
	}

	return (false);
}

void EditorManipulator::Select(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorSelected);
	selectionType = kEditorSelectionObject;

	markerColorAttribute.SetDiffuseColor(K::white);
	Show();
}

void EditorManipulator::Unselect(void)
{
	SetManipulatorState(GetManipulatorState() & ~(kManipulatorSelected | kManipulatorTempSelected));
	selectionType = kEditorSelectionObject;

	markerColorAttribute.SetDiffuseColor(kUnselectedMarkerColor);
	UpdateGraphColor();
}

void EditorManipulator::Hilite(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorHilited);
}

void EditorManipulator::Unhilite(void)
{
	SetManipulatorState(GetManipulatorState() & ~kManipulatorHilited);
}

void EditorManipulator::HandleDelete(bool undoable)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorDeleted);
	UnselectConnector();
}

void EditorManipulator::HandleUndelete(void)
{
	SetManipulatorState(GetManipulatorState() & ~kManipulatorDeleted);
}

void EditorManipulator::HandleSizeUpdate(int32 count, const float *size)
{
	Node *node = GetTargetNode();
	Object *object = node->GetObject();
	if (object)
	{
		float	objectSize[kMaxObjectSizeCount];

		for (machine a = 0; a < count; a++)
		{
			objectSize[a] = Fmax(size[a], kSizeEpsilon);
		}

		object->SetObjectSize(objectSize);
		worldEditor->InvalidateNode(node);
	}
}

void EditorManipulator::HandleSettingsUpdate(void)
{
	GetTargetNode()->Invalidate();
	InvalidateGraph();
}

void EditorManipulator::HandleConnectorUpdate(void)
{
	GetTargetNode()->ProcessInternalConnectors();
}

bool EditorManipulator::MaterialSettable(void) const
{
	return (false);
}

bool EditorManipulator::MaterialRemovable(void) const
{
	return (false);
}

int32 EditorManipulator::GetMaterialCount(void) const
{
	return (0);
}

MaterialObject *EditorManipulator::GetMaterial(int32 index) const
{
	return (nullptr);
}

void EditorManipulator::SetMaterial(MaterialObject *material)
{
}

void EditorManipulator::ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial)
{
}

void EditorManipulator::RemoveMaterial(void)
{
}

void EditorManipulator::InvalidateShaderData(void)
{
}

bool EditorManipulator::ReparentedSubnodesAllowed(void) const
{
	return ((GetTargetNode()->GetNodeFlags() & kNodeNonpersistent) == 0);
}

Box3D EditorManipulator::CalculateNodeBoundingBox(void) const
{
	return (Box3D(Zero3D, Zero3D));
}

Box3D EditorManipulator::CalculateWorldBoundingBox(void) const
{
	return (Transform(CalculateNodeBoundingBox(), GetTargetNode()->GetWorldTransform()));
}

void EditorManipulator::AdjustBoundingBox(Box3D *box)
{
	Vector3D size = box->GetSize();

	if (size.x < 0.25F)
	{
		float x = (box->min.x + box->max.x) * 0.5F;
		box->min.x = x - 0.125F;
		box->max.x = x + 0.125F;
	}

	if (size.y < 0.25F)
	{
		float y = (box->min.y + box->max.y) * 0.5F;
		box->min.y = y - 0.125F;
		box->max.y = y + 0.125F;
	}

	if (size.z < 0.25F)
	{
		float z = (box->min.z + box->max.z) * 0.5F;
		box->min.z = z - 0.125F;
		box->max.z = z + 0.125F;
	}

	float expand = Fmax(size.x, size.y, size.z) * 0.03125F;
	box->min -= Vector3D(expand, expand, expand);
	box->max += Vector3D(expand, expand, expand);
}

bool EditorManipulator::Pick(const Ray *ray, PickData *data) const
{
	if (GetManipulatorState() & kManipulatorShowIcon)
	{
		float	t2;

		float r = ray->radius * 11.0F;
		if (r == 0.0F)
		{
			r = Editor::kFrustumRenderScale * 12.0F;
		}

		return (Math::IntersectRayAndSphere(ray, Zero3D, r, &data->rayParam, &t2));
	}

	return (false);
}

bool EditorManipulator::RegionPick(const VisibilityRegion *region) const
{
	return (region->SphereVisible(GetTargetNode()->GetWorldPosition(), 0.0F));
}

void EditorManipulator::BeginTransform(void)
{
}

void EditorManipulator::EndTransform(void)
{
}

int32 EditorManipulator::GetHandleTable(Point3D *handle) const
{
	return (0);
}

void EditorManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	handleData->handleFlags = 0;
	handleData->oppositeIndex = kHandleOrigin;
}

void EditorManipulator::BeginResize(const ManipulatorResizeData *resizeData)
{
	const Node *node = GetTargetNode();
	const Object *object = node->GetObject();
	if (object)
	{
		object->GetObjectSize(originalSize);
	}

	originalPosition = node->GetNodePosition();
}

void EditorManipulator::EndResize(const ManipulatorResizeData *resizeData)
{
}

bool EditorManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	return (false);
}

SharedVertexBuffer *EditorManipulator::RetainBoxVertexBuffer(void)
{
	if (boxVertexBuffer.Retain() == 1)
	{
		boxVertexBuffer.Establish(sizeof(BoxVertex) * kManipulatorBoxVertexCount);

		volatile BoxVertex *restrict vertex = boxVertexBuffer.BeginUpdate<BoxVertex>();

		const Point3D *position = &manipulatorBoxPosition[0];
		const Vector3D *offset = &manipulatorBoxOffset[0];

		for (machine a = 0; a < kManipulatorBoxVertexCount; a++)
		{
			vertex[a].position = position[a];
			vertex[a].offset = offset[a];
		}

		boxVertexBuffer.EndUpdate();
	}

	return (&boxVertexBuffer);
}

SharedVertexBuffer *EditorManipulator::RetainBoxIndexBuffer(void)
{
	if (boxIndexBuffer.Retain() == 1)
	{
		boxIndexBuffer.Establish(sizeof(Triangle) * kManipulatorBoxTriangleCount, manipulatorBoxTriangle);
	}

	return (&boxIndexBuffer);
}

void EditorManipulator::ReleaseBoxVertexBuffer(void)
{
	boxVertexBuffer.Release();
}

void EditorManipulator::ReleaseBoxIndexBuffer(void)
{
	boxIndexBuffer.Release();
}

void EditorManipulator::AllocateConnectorStorage(void)
{
	ReleaseConnectorStorage();

	connectorCount = 0;

	const Hub *hub = GetTargetNode()->GetHub();
	if (hub)
	{
		int32 count = hub->GetOutgoingEdgeCount();
		connectorCount = count;
		if (count != 0)
		{
			connectorStorage = new char[sizeof(EditorConnector) * count];
			editorConnector = reinterpret_cast<EditorConnector *>(connectorStorage);

			Connector *connector = hub->GetFirstOutgoingEdge();
			for (machine a = 0; a < count; a++)
			{
				new(&editorConnector[a]) EditorConnector(this, connector, a);
				connector = connector->GetNextOutgoingEdge();
			}
		}
	}
}

void EditorManipulator::ReleaseConnectorStorage(void)
{
	if (connectorStorage)
	{
		for (machine index = connectorCount - 1; index >= 0; index--)
		{
			editorConnector[index].~EditorConnector();
		}

		delete[] connectorStorage;
		connectorStorage = nullptr;
	}
}

void EditorManipulator::UpdateConnectors(void)
{
	AllocateConnectorStorage();
	SetManipulatorState(GetManipulatorState() & ~kManipulatorConnectorSelected);
	Detach();
}

void EditorManipulator::SelectConnector(int32 index, bool toggle)
{
	unsigned_int32 state = GetManipulatorState();
	if (state & kManipulatorConnectorSelected)
	{
		if (connectorSelection == index)
		{
			if (toggle)
			{
				editorConnector[index].Unselect();
				SetManipulatorState(state & ~kManipulatorConnectorSelected);
			}

			return;
		}

		editorConnector[connectorSelection].Unselect();
	}
	else
	{
		SetManipulatorState(state | kManipulatorConnectorSelected);
	}

	connectorSelection = index;
	editorConnector[index].Select();
}

void EditorManipulator::UnselectConnector(void)
{
	unsigned_int32 state = GetManipulatorState();
	if (state & kManipulatorConnectorSelected)
	{
		SetManipulatorState(state & ~kManipulatorConnectorSelected);
		editorConnector[connectorSelection].Unselect();
		Detach();
	}
}

bool EditorManipulator::SetConnectorTarget(int32 index, Node *target, const ConnectorKey **key)
{
	Node *node = GetTargetNode();
	const Hub *hub = node->GetHub();
	if (hub)
	{
		Connector *connector = hub->GetOutgoingEdge(index);
		if (connector)
		{
			const ConnectorKey& connectorKey = connector->GetConnectorKey();

			if (target)
			{
				if (node->ValidConnectedNode(connectorKey, target))
				{
					connector->SetConnectorTarget(target);

					if (key)
					{
						*key = &connectorKey;
					}

					HandleConnectorUpdate();
					return (true);
				}
			}
			else
			{
				connector->SetConnectorTarget(nullptr);

				if (key)
				{
					*key = &connectorKey;
				}

				HandleConnectorUpdate();
				return (true);
			}
		}
	}

	return (false);
}

bool EditorManipulator::PickConnector(const ManipulatorViewportData *viewportData, const Ray *ray, PickData *pickData) const
{
	int32 count = connectorCount;
	for (machine a = count - 1; a >= 0; a--)
	{
		if (editorConnector[a].Pick(viewportData, ray))
		{
			pickData->pickIndex[0] = a;
			return (true);
		}
	}

	return (false);
}

Box2D EditorManipulator::GetGraphBox(void) const
{
	const Point2D& p = GetGraphPosition().GetPoint2D();
	return (Box2D(p, Point2D(p.x + kGraphBoxWidth, p.y + 16.0F)));
}

void EditorManipulator::ExpandSubgraph(void)
{
	unsigned_int32 state = graphCollapseButton.GetWidgetState();
	if (state & kWidgetCollapsed)
	{
		InvalidateGraph();
		graphCollapseButton.SetWidgetState(state & ~kWidgetCollapsed);
	}
}

void EditorManipulator::CollapseSubgraph(void)
{
	unsigned_int32 state = graphCollapseButton.GetWidgetState();
	if (!(state & kWidgetCollapsed))
	{
		InvalidateGraph();
		graphCollapseButton.SetWidgetState(state | kWidgetCollapsed);
	}
}

Node *EditorManipulator::PickGraphNode(const ManipulatorViewportData *viewportData, const Ray *ray, Widget **widget)
{
	const Point3D& position = GetGraphPosition();
	float x = ray->origin.x - position.x;
	float y = ray->origin.y - position.y;

	if ((y > -1.0F) && (y < graphHeight) && (x > -1.0F) && (x < graphWidth + 29.0F))
	{
		Node *node = GetTargetNode();

		if ((x > -1.0F) && (x < kGraphBoxWidth + 1.0F) && (y < 17.0F))
		{
			while (node->GetNodeFlags() & kNodeNonpersistent)
			{
				node = node->GetSuperNode();
			}

			return (node);
		}

		if (widget)
		{
			if ((graphCollapseButton.Visible()) && (graphCollapseButton.GetBoundingBox()->Contains(ray->origin.GetPoint2D())))
			{
				*widget = &graphCollapseButton;
				return (nullptr);
			}
		}

		const Node *subnode = node->GetFirstSubnode();
		while (subnode)
		{
			Node *pick = static_cast<EditorManipulator *>(subnode->GetManipulator())->PickGraphNode(viewportData, ray, widget);
			if (pick)
			{
				return (pick);
			}

			subnode = subnode->Next();
		}
	}

	return (nullptr);
}

void EditorManipulator::SelectGraphNodes(float left, float right, float top, float bottom, bool temp)
{
	const Point3D& position = GetGraphPosition();
	if ((position.y < bottom) && (position.y + graphHeight > top) && (position.x < right) && (position.x + graphWidth > left))
	{
		Node *node = GetTargetNode();
		if ((position.x + kGraphBoxWidth > left) && (position.y + 16.0F > top))
		{
			if (!Selected())
			{
				worldEditor->SelectNode(node);

				if (temp)
				{
					SetManipulatorState(GetManipulatorState() | kManipulatorTempSelected);
				}
			}
		}

		unsigned_int32 state = graphCollapseButton.GetWidgetState();
		if (!(state & kWidgetCollapsed))
		{
			const Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				static_cast<EditorManipulator *>(subnode->GetManipulator())->SelectGraphNodes(left, right, top, bottom, temp);
				subnode = subnode->Next();
			}
		}
	}
}

void EditorManipulator::HiliteSubtree(void)
{
	Node *root = GetTargetNode();
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		Editor::GetManipulator(node)->Hilite();
		node = root->GetNextNode(node);
	}
}

void EditorManipulator::UnhiliteSubtree(void)
{
	Node *root = GetTargetNode();
	Node *node = root->GetFirstSubnode();
	while (node)
	{
		Editor::GetManipulator(node)->Unhilite();
		node = root->GetNextNode(node);
	}
}

void EditorManipulator::Render(const ManipulatorRenderData *renderData)
{
	float scale = renderData->viewportScale;
	manipulatorScaleVector.Set(scale, scale, scale, scale);

	unsigned_int32 state = GetManipulatorState();
	bool showConnectors = ((renderData->connectorList) && (state & (kManipulatorSelected | kManipulatorConnectorSelected)) && (connectorCount != 0));

	if ((state & kManipulatorShowIcon) || (showConnectors))
	{
		List<Renderable> *renderList = renderData->manipulatorList;
		if (renderList)
		{
			renderList->Append(&markerRenderable);
			renderList->Append(&iconRenderable);
		}
	}

	if (state & kManipulatorSelected)
	{
		List<Renderable> *renderList = renderData->handleList;
		if ((renderList) && (handleCount != 0))
		{
			renderList->Append(&handleRenderable);
		}
	}

	if (showConnectors)
	{
		List<Renderable> *renderList = renderData->connectorList;

		for (machine a = 0; a < connectorCount; a++)
		{
			editorConnector[a].RenderLine(renderData, renderList);
		}

		for (machine a = 0; a < connectorCount; a++)
		{
			editorConnector[a].RenderBox(renderData, renderList);
		}
	}
}

void EditorManipulator::RenderGraph(const ManipulatorViewportData *viewportData, List<Renderable> *renderList)
{
	const Node *node = GetTargetNode();
	const Node *previous = node->Previous();

	const Point3D& cameraPosition = viewportData->viewportCamera->GetNodePosition();
	const Vector3D& position = GetGraphPosition() - cameraPosition;

	float left = position.x - 29.0F;
	float right = position.x + graphWidth;
	float top = (previous) ? Editor::GetManipulator(previous)->GetGraphPosition().y - cameraPosition.y : position.y - 1.0F;
	float bottom = position.y + graphHeight;

	const OrthoCameraObject *cameraObject = static_cast<OrthoCamera *>(viewportData->viewportCamera)->GetObject();
	if ((top < cameraObject->GetOrthoRectBottom()) && (bottom > cameraObject->GetOrthoRectTop()) && (left < cameraObject->GetOrthoRectRight()) && (right > cameraObject->GetOrthoRectLeft()))
	{
		graphBorder.SetViewportScale(viewportData->viewportScale);
		graphBackground.RenderTree(renderList);

		if (!(graphCollapseButton.GetWidgetState() & kWidgetCollapsed))
		{
			const Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				static_cast<EditorManipulator *>(subnode->GetManipulator())->RenderGraph(viewportData, renderList);
				subnode = subnode->Next();
			}
		}
	}
}

void EditorManipulator::Install(Editor *editor, Node *root, bool recursive)
{
	EditorManipulator *manipulator = Editor::GetManipulator(root);
	if (!manipulator)
	{
		manipulator = static_cast<EditorManipulator *>(Manipulator::Create(root));
		if (!manipulator)
		{
			manipulator = new EditorManipulator(root, "WorldEditor/node/Node");
		}

		root->SetManipulator(manipulator);
		manipulator->Invalidate();
	}
	else
	{
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kManipulatorSelected);
	}

	manipulator->worldEditor = editor;

	if (recursive)
	{
		Node *node = root->GetFirstSubnode();
		while (node)
		{
			Install(editor, node);
			node = node->Next();
		}
	}
}


GroupManipulator::GroupManipulator(Node *node) : EditorManipulator(node, "WorldEditor/node/Group")
{
}

GroupManipulator::~GroupManipulator()
{
}

const char *GroupManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeGeneric)));
}

void GroupManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();
}


SkyboxManipulator::SkyboxManipulator(Skybox *skybox) : EditorManipulator(skybox, "WorldEditor/atmosphere/Skybox")
{
}

SkyboxManipulator::~SkyboxManipulator()
{
}

const char *SkyboxManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeSkybox)));
}

void SkyboxManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();

	GetEditor()->SetProcessPropertiesFlag();
}

void SkyboxManipulator::HandleDelete(bool undoable)
{
	EditorManipulator::HandleDelete(undoable);
	GetEditor()->SetProcessPropertiesFlag();
}

void SkyboxManipulator::HandleUndelete(void)
{
	EditorManipulator::HandleUndelete();
	GetEditor()->SetProcessPropertiesFlag();
}

void SkyboxManipulator::HandleSettingsUpdate(void)
{
	EditorManipulator::HandleSettingsUpdate();
	GetTargetNode()->InvalidateShaderData();
}

bool SkyboxManipulator::MaterialSettable(void) const
{
	return (true);
}

bool SkyboxManipulator::MaterialRemovable(void) const
{
	return (true);
}

int32 SkyboxManipulator::GetMaterialCount(void) const
{
	return (1);
}

MaterialObject *SkyboxManipulator::GetMaterial(int32 index) const
{
	return (GetTargetNode()->GetMaterialObject());
}

void SkyboxManipulator::SetMaterial(MaterialObject *material)
{
	Skybox *skybox = GetTargetNode();
	skybox->SetMaterialObject(material);
	skybox->InvalidateShaderData();
}

void SkyboxManipulator::ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial)
{
	Skybox *skybox = GetTargetNode();
	if (skybox->GetMaterialObject() == oldMaterial)
	{
		skybox->SetMaterialObject(newMaterial);
		skybox->InvalidateShaderData();
	}
}

void SkyboxManipulator::RemoveMaterial(void)
{
	Skybox *skybox = GetTargetNode();
	skybox->SetMaterialObject(nullptr);
	skybox->InvalidateShaderData();
}

void SkyboxManipulator::InvalidateShaderData(void)
{
	GetTargetNode()->InvalidateShaderData();
}


ImpostorManipulator::ImpostorManipulator(Impostor *impostor) : EditorManipulator(impostor, "WorldEditor/impostor/Impostor")
{
}

ImpostorManipulator::~ImpostorManipulator()
{
}

const char *ImpostorManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeImpostor)));
}

void ImpostorManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();
}

bool ImpostorManipulator::MaterialSettable(void) const
{
	return (true);
}

int32 ImpostorManipulator::GetMaterialCount(void) const
{
	return (1);
}

MaterialObject *ImpostorManipulator::GetMaterial(int32 index) const
{
	return (GetTargetNode()->GetMaterialObject());
}

void ImpostorManipulator::SetMaterial(MaterialObject *material)
{
	Impostor *impostor = GetTargetNode();
	impostor->SetMaterialObject(material);
}

void ImpostorManipulator::ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial)
{
	Impostor *impostor = GetTargetNode();
	if (impostor->GetMaterialObject() == oldMaterial)
	{
		impostor->SetMaterialObject(newMaterial);
	}
}

// ZYUQURM
