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


#include "C4EditorGizmo.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const float kGizmoAxisThreshold = 0.965926F;		// cos 15°
	const float kGizmoPlaneThreshold = 0.258819F;		// cos 75°

	const float kGizmoLineScaleMultiplier = 1.5F;
	const float kGizmoRenderSizeMultiplier = 120.0F;


	const TextureHeader edgeTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticDiffuse,
		kTextureSemanticTransparency,
		kTextureL8,
		8, 1, 1,
		{kTextureClamp, kTextureRepeat, kTextureClamp},
		1
	};


	const unsigned_int8 edgeTextureImage[8] =
	{
		0x00, 0x55, 0xAA, 0xFF, 0xFF, 0xAA, 0x55, 0x00
	};
}


const ConstColorRGBA EditorGizmo::gizmoAxisColor[3] =
{
	{1.0F, 0.125F, 0.25F, 1.0F}, {0.0F, 1.0F, 0.0F, 1.0F}, {0.125F, 0.625F, 1.0F, 1.0F}
};

const ConstPoint3D EditorGizmo::gizmoAxisVertex[6] =
{
	{-0.5F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F},
	{0.0F, -0.5F, 0.0F}, {0.0F, 1.0F, 0.0F},
	{0.0F, 0.0F, -0.5F}, {0.0F, 0.0F, 1.0F}
};

const EditorGizmo::ArrowVertex EditorGizmo::gizmoArrowVertex[12] =
{
	{{0.984375F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, -0.0625F}, {0.0F, 0.015625F}}, {{0.984375F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 0.0625F}, {0.0F, 1.0F}},
	{{1.1875F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 0.0625F}, {1.0F, 1.0F}}, {{1.1875F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, -0.0625F}, {1.0F, 0.015625F}},
	{{0.0F, 0.984375F, 0.0F}, {0.0F, 1.0F, 0.0F, -0.0625F}, {0.0F, 0.015625F}}, {{0.0F, 0.984375F, 0.0F}, {0.0F, 1.0F, 0.0F, 0.0625F}, {0.0F, 1.0F}},
	{{0.0F, 1.1875F, 0.0F}, {0.0F, 1.0F, 0.0F, 0.0625F}, {1.0F, 1.0F}}, {{0.0F, 1.1875F, 0.0F}, {0.0F, 1.0F, 0.0F, -0.0625F}, {1.0F, 0.015625F}},
	{{0.0F, 0.0F, 0.984375F}, {0.0F, 0.0F, 1.0F, -0.0625F}, {0.0F, 0.015625F}}, {{0.0F, 0.0F, 0.984375F}, {0.0F, 0.0F, 1.0F, 0.0625F}, {0.0F, 1.0F}},
	{{0.0F, 0.0F, 1.1875F}, {0.0F, 0.0F, 1.0F, 0.0625F}, {1.0F, 1.0F}}, {{0.0F, 0.0F, 1.1875F}, {0.0F, 0.0F, 1.0F, -0.0625F}, {1.0F, 0.015625F}}
};

const ConstPoint3D EditorGizmo::gizmoSquareVertex[36] =
{
	{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 1.0F}, {0.0F, 0.0F, 1.0F},
	{0.0F, -1.0F, 0.0F}, {0.0F, -1.0F, 1.0F}, {0.0F, 0.0F, 1.0F},
	{0.0F, 1.0F, 0.0F}, {0.0F, 1.0F, -1.0F}, {0.0F, 0.0F, -1.0F},
	{0.0F, -1.0F, 0.0F}, {0.0F, -1.0F, -1.0F}, {0.0F, 0.0F, -1.0F},

	{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F},
	{1.0F, 0.0F, 0.0F}, {1.0F, 0.0F, -1.0F}, {0.0F, 0.0F, -1.0F},
	{-1.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, 1.0F}, {0.0F, 0.0F, 1.0F},
	{-1.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, -1.0F}, {0.0F, 0.0F, -1.0F},

	{1.0F, 0.0F, 0.0F}, {1.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
	{-1.0F, 0.0F, 0.0F}, {-1.0F, 1.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
	{1.0F, 0.0F, 0.0F}, {1.0F, -1.0F, 0.0F}, {0.0F, -1.0F, 0.0F},
	{-1.0F, 0.0F, 0.0F}, {-1.0F, -1.0F, 0.0F}, {0.0F, -1.0F, 0.0F}
};

const ConstPoint3D EditorGizmo::gizmoCircleVertex[396] =
{
	{0.0F, 1.0F, 0.0F}, {0.0F, 0.9987953F, 0.0490676F}, {0.0F, 0.9951846F, 0.098017F}, {0.0F, 0.9891765F, 0.1467304F},
	{0.0F, 0.9807852F, 0.1950902F}, {0.0F, 0.9700312F, 0.2429801F}, {0.0F, 0.9569402F, 0.2902846F}, {0.0F, 0.941544F, 0.3368898F},
	{0.0F, 0.9238795F, 0.3826833F}, {0.0F, 0.9039893F, 0.427555F}, {0.0F, 0.8819211F, 0.4713966F}, {0.0F, 0.8577286F, 0.5141026F},
	{0.0F, 0.8314695F, 0.5555702F}, {0.0F, 0.8032075F, 0.5956993F}, {0.0F, 0.7730103F, 0.6343933F}, {0.0F, 0.740951F, 0.6715589F},
	{0.0F, 0.7071067F, 0.7071067F}, {0.0F, 0.6715588F, 0.7409511F}, {0.0F, 0.6343932F, 0.7730103F}, {0.0F, 0.5956993F, 0.8032075F},
	{0.0F, 0.5555701F, 0.8314696F}, {0.0F, 0.5141026F, 0.8577286F}, {0.0F, 0.4713965F, 0.8819212F}, {0.0F, 0.427555F, 0.9039893F},
	{0.0F, 0.3826833F, 0.9238795F}, {0.0F, 0.3368897F, 0.941544F}, {0.0F, 0.2902846F, 0.9569402F}, {0.0F, 0.2429801F, 0.9700312F},
	{0.0F, 0.1950901F, 0.9807852F}, {0.0F, 0.1467304F, 0.9891765F}, {0.0F, 0.098017F, 0.9951846F}, {0.0F, 0.0490676F, 0.9987953F},
	{0.0F, 0.0F, 1.0F},

	{0.0F, -1.0F, 0.0F}, {0.0F, -0.9987953F, 0.0490676F}, {0.0F, -0.9951846F, 0.098017F}, {0.0F, -0.9891765F, 0.1467304F},
	{0.0F, -0.9807852F, 0.1950902F}, {0.0F, -0.9700312F, 0.2429801F}, {0.0F, -0.9569402F, 0.2902846F}, {0.0F, -0.941544F, 0.3368898F},
	{0.0F, -0.9238795F, 0.3826833F}, {0.0F, -0.9039893F, 0.427555F}, {0.0F, -0.8819211F, 0.4713966F}, {0.0F, -0.8577286F, 0.5141026F},
	{0.0F, -0.8314695F, 0.5555702F}, {0.0F, -0.8032075F, 0.5956993F}, {0.0F, -0.7730103F, 0.6343933F}, {0.0F, -0.740951F, 0.6715589F},
	{0.0F, -0.7071067F, 0.7071067F}, {0.0F, -0.6715588F, 0.7409511F}, {0.0F, -0.6343932F, 0.7730103F}, {0.0F, -0.5956993F, 0.8032075F},
	{0.0F, -0.5555701F, 0.8314696F}, {0.0F, -0.5141026F, 0.8577286F}, {0.0F, -0.4713965F, 0.8819212F}, {0.0F, -0.427555F, 0.9039893F},
	{0.0F, -0.3826833F, 0.9238795F}, {0.0F, -0.3368897F, 0.941544F}, {0.0F, -0.2902846F, 0.9569402F}, {0.0F, -0.2429801F, 0.9700312F},
	{0.0F, -0.1950901F, 0.9807852F}, {0.0F, -0.1467304F, 0.9891765F}, {0.0F, -0.098017F, 0.9951846F}, {0.0F, -0.0490676F, 0.9987953F},
	{0.0F, 0.0F, 1.0F},

	{0.0F, 1.0F, 0.0F}, {0.0F, 0.9987953F, -0.0490676F}, {0.0F, 0.9951846F, -0.098017F}, {0.0F, 0.9891765F, -0.1467304F},
	{0.0F, 0.9807852F, -0.1950902F}, {0.0F, 0.9700312F, -0.2429801F}, {0.0F, 0.9569402F, -0.2902846F}, {0.0F, 0.941544F, -0.3368898F},
	{0.0F, 0.9238795F, -0.3826833F}, {0.0F, 0.9039893F, -0.427555F}, {0.0F, 0.8819211F, -0.4713966F}, {0.0F, 0.8577286F, -0.5141026F}, 
	{0.0F, 0.8314695F, -0.5555702F}, {0.0F, 0.8032075F, -0.5956993F}, {0.0F, 0.7730103F, -0.6343933F}, {0.0F, 0.740951F, -0.6715589F},
	{0.0F, 0.7071067F, -0.7071067F}, {0.0F, 0.6715588F, -0.7409511F}, {0.0F, 0.6343932F, -0.7730103F}, {0.0F, 0.5956993F, -0.8032075F},
	{0.0F, 0.5555701F, -0.8314696F}, {0.0F, 0.5141026F, -0.8577286F}, {0.0F, 0.4713965F, -0.8819212F}, {0.0F, 0.427555F, -0.9039893F}, 
	{0.0F, 0.3826833F, -0.9238795F}, {0.0F, 0.3368897F, -0.941544F}, {0.0F, 0.2902846F, -0.9569402F}, {0.0F, 0.2429801F, -0.9700312F},
	{0.0F, 0.1950901F, -0.9807852F}, {0.0F, 0.1467304F, -0.9891765F}, {0.0F, 0.098017F, -0.9951846F}, {0.0F, 0.0490676F, -0.9987953F}, 
	{0.0F, 0.0F, -1.0F},

	{0.0F, -1.0F, 0.0F}, {0.0F, -0.9987953F, -0.0490676F}, {0.0F, -0.9951846F, -0.098017F}, {0.0F, -0.9891765F, -0.1467304F}, 
	{0.0F, -0.9807852F, -0.1950902F}, {0.0F, -0.9700312F, -0.2429801F}, {0.0F, -0.9569402F, -0.2902846F}, {0.0F, -0.941544F, -0.3368898F},
	{0.0F, -0.9238795F, -0.3826833F}, {0.0F, -0.9039893F, -0.427555F}, {0.0F, -0.8819211F, -0.4713966F}, {0.0F, -0.8577286F, -0.5141026F}, 
	{0.0F, -0.8314695F, -0.5555702F}, {0.0F, -0.8032075F, -0.5956993F}, {0.0F, -0.7730103F, -0.6343933F}, {0.0F, -0.740951F, -0.6715589F}, 
	{0.0F, -0.7071067F, -0.7071067F}, {0.0F, -0.6715588F, -0.7409511F}, {0.0F, -0.6343932F, -0.7730103F}, {0.0F, -0.5956993F, -0.8032075F},
	{0.0F, -0.5555701F, -0.8314696F}, {0.0F, -0.5141026F, -0.8577286F}, {0.0F, -0.4713965F, -0.8819212F}, {0.0F, -0.427555F, -0.9039893F},
	{0.0F, -0.3826833F, -0.9238795F}, {0.0F, -0.3368897F, -0.941544F}, {0.0F, -0.2902846F, -0.9569402F}, {0.0F, -0.2429801F, -0.9700312F},
	{0.0F, -0.1950901F, -0.9807852F}, {0.0F, -0.1467304F, -0.9891765F}, {0.0F, -0.098017F, -0.9951846F}, {0.0F, -0.0490676F, -0.9987953F}, 
	{0.0F, 0.0F, -1.0F},

	{1.0F, 0.0F, 0.0F}, {0.9987953F, 0.0F, 0.0490676F}, {0.9951846F, 0.0F, 0.098017F}, {0.9891765F, 0.0F, 0.1467304F},
	{0.9807852F, 0.0F, 0.1950902F}, {0.9700312F, 0.0F, 0.2429801F}, {0.9569402F, 0.0F, 0.2902846F}, {0.941544F, 0.0F, 0.3368898F},
	{0.9238795F, 0.0F, 0.3826833F}, {0.9039893F, 0.0F, 0.427555F}, {0.8819211F, 0.0F, 0.4713966F}, {0.8577286F, 0.0F, 0.5141026F},
	{0.8314695F, 0.0F, 0.5555702F}, {0.8032075F, 0.0F, 0.5956993F}, {0.7730103F, 0.0F, 0.6343933F}, {0.740951F, 0.0F, 0.6715589F},
	{0.7071067F, 0.0F, 0.7071067F}, {0.6715588F, 0.0F, 0.7409511F}, {0.6343932F, 0.0F, 0.7730103F}, {0.5956993F, 0.0F, 0.8032075F},
	{0.5555701F, 0.0F, 0.8314696F}, {0.5141026F, 0.0F, 0.8577286F}, {0.4713965F, 0.0F, 0.8819212F}, {0.427555F, 0.0F, 0.9039893F},
	{0.3826833F, 0.0F, 0.9238795F}, {0.3368897F, 0.0F, 0.941544F}, {0.2902846F, 0.0F, 0.9569402F}, {0.2429801F, 0.0F, 0.9700312F},
	{0.1950901F, 0.0F, 0.9807852F}, {0.1467304F, 0.0F, 0.9891765F}, {0.098017F, 0.0F, 0.9951846F}, {0.0490676F, 0.0F, 0.9987953F},
	{0.0F, 0.0F, 1.0F},

	{1.0F, 0.0F, 0.0F}, {0.9987953F, 0.0F, -0.0490676F}, {0.9951846F, 0.0F, -0.098017F}, {0.9891765F, 0.0F, -0.1467304F},
	{0.9807852F, 0.0F, -0.1950902F}, {0.9700312F, 0.0F, -0.2429801F}, {0.9569402F, 0.0F, -0.2902846F}, {0.941544F, 0.0F, -0.3368898F},
	{0.9238795F, 0.0F, -0.3826833F}, {0.9039893F, 0.0F, -0.427555F}, {0.8819211F, 0.0F, -0.4713966F}, {0.8577286F, 0.0F, -0.5141026F},
	{0.8314695F, 0.0F, -0.5555702F}, {0.8032075F, 0.0F, -0.5956993F}, {0.7730103F, 0.0F, -0.6343933F}, {0.740951F, 0.0F, -0.6715589F},
	{0.7071067F, 0.0F, -0.7071067F}, {0.6715588F, 0.0F, -0.7409511F}, {0.6343932F, 0.0F, -0.7730103F}, {0.5956993F, 0.0F, -0.8032075F},
	{0.5555701F, 0.0F, -0.8314696F}, {0.5141026F, 0.0F, -0.8577286F}, {0.4713965F, 0.0F, -0.8819212F}, {0.427555F, 0.0F, -0.9039893F},
	{0.3826833F, 0.0F, -0.9238795F}, {0.3368897F, 0.0F, -0.941544F}, {0.2902846F, 0.0F, -0.9569402F}, {0.2429801F, 0.0F, -0.9700312F},
	{0.1950901F, 0.0F, -0.9807852F}, {0.1467304F, 0.0F, -0.9891765F}, {0.098017F, 0.0F, -0.9951846F}, {0.0490676F, 0.0F, -0.9987953F},
	{0.0F, 0.0F, -1.0F},

	{-1.0F, 0.0F, 0.0F}, {-0.9987953F, 0.0F, 0.0490676F}, {-0.9951846F, 0.0F, 0.098017F}, {-0.9891765F, 0.0F, 0.1467304F},
	{-0.9807852F, 0.0F, 0.1950902F}, {-0.9700312F, 0.0F, 0.2429801F}, {-0.9569402F, 0.0F, 0.2902846F}, {-0.941544F, 0.0F, 0.3368898F},
	{-0.9238795F, 0.0F, 0.3826833F}, {-0.9039893F, 0.0F, 0.427555F}, {-0.8819211F, 0.0F, 0.4713966F}, {-0.8577286F, 0.0F, 0.5141026F},
	{-0.8314695F, 0.0F, 0.5555702F}, {-0.8032075F, 0.0F, 0.5956993F}, {-0.7730103F, 0.0F, 0.6343933F}, {-0.740951F, 0.0F, 0.6715589F},
	{-0.7071067F, 0.0F, 0.7071067F}, {-0.6715588F, 0.0F, 0.7409511F}, {-0.6343932F, 0.0F, 0.7730103F}, {-0.5956993F, 0.0F, 0.8032075F},
	{-0.5555701F, 0.0F, 0.8314696F}, {-0.5141026F, 0.0F, 0.8577286F}, {-0.4713965F, 0.0F, 0.8819212F}, {-0.427555F, 0.0F, 0.9039893F},
	{-0.3826833F, 0.0F, 0.9238795F}, {-0.3368897F, 0.0F, 0.941544F}, {-0.2902846F, 0.0F, 0.9569402F}, {-0.2429801F, 0.0F, 0.9700312F},
	{-0.1950901F, 0.0F, 0.9807852F}, {-0.1467304F, 0.0F, 0.9891765F}, {-0.098017F, 0.0F, 0.9951846F}, {-0.0490676F, 0.0F, 0.9987953F},
	{0.0F, 0.0F, 1.0F},

	{-1.0F, 0.0F, 0.0F}, {-0.9987953F, 0.0F, -0.0490676F}, {-0.9951846F, 0.0F, -0.098017F}, {-0.9891765F, 0.0F, -0.1467304F},
	{-0.9807852F, 0.0F, -0.1950902F}, {-0.9700312F, 0.0F, -0.2429801F}, {-0.9569402F, 0.0F, -0.2902846F}, {-0.941544F, 0.0F, -0.3368898F},
	{-0.9238795F, 0.0F, -0.3826833F}, {-0.9039893F, 0.0F, -0.427555F}, {-0.8819211F, 0.0F, -0.4713966F}, {-0.8577286F, 0.0F, -0.5141026F},
	{-0.8314695F, 0.0F, -0.5555702F}, {-0.8032075F, 0.0F, -0.5956993F}, {-0.7730103F, 0.0F, -0.6343933F}, {-0.740951F, 0.0F, -0.6715589F},
	{-0.7071067F, 0.0F, -0.7071067F}, {-0.6715588F, 0.0F, -0.7409511F}, {-0.6343932F, 0.0F, -0.7730103F}, {-0.5956993F, 0.0F, -0.8032075F},
	{-0.5555701F, 0.0F, -0.8314696F}, {-0.5141026F, 0.0F, -0.8577286F}, {-0.4713965F, 0.0F, -0.8819212F}, {-0.427555F, 0.0F, -0.9039893F},
	{-0.3826833F, 0.0F, -0.9238795F}, {-0.3368897F, 0.0F, -0.941544F}, {-0.2902846F, 0.0F, -0.9569402F}, {-0.2429801F, 0.0F, -0.9700312F},
	{-0.1950901F, 0.0F, -0.9807852F}, {-0.1467304F, 0.0F, -0.9891765F}, {-0.098017F, 0.0F, -0.9951846F}, {-0.0490676F, 0.0F, -0.9987953F},
	{0.0F, 0.0F, -1.0F},

	{1.0F, 0.0F, 0.0F}, {0.9987953F, 0.0490676F, 0.0F}, {0.9951846F, 0.098017F, 0.0F}, {0.9891765F, 0.1467304F, 0.0F},
	{0.9807852F, 0.1950902F, 0.0F}, {0.9700312F, 0.2429801F, 0.0F}, {0.9569402F, 0.2902846F, 0.0F}, {0.941544F, 0.3368898F, 0.0F},
	{0.9238795F, 0.3826833F, 0.0F}, {0.9039893F, 0.427555F, 0.0F}, {0.8819211F, 0.4713966F, 0.0F}, {0.8577286F, 0.5141026F, 0.0F},
	{0.8314695F, 0.5555702F, 0.0F}, {0.8032075F, 0.5956993F, 0.0F}, {0.7730103F, 0.6343933F, 0.0F}, {0.740951F, 0.6715589F, 0.0F},
	{0.7071067F, 0.7071067F, 0.0F}, {0.6715588F, 0.7409511F, 0.0F}, {0.6343932F, 0.7730103F, 0.0F}, {0.5956993F, 0.8032075F, 0.0F},
	{0.5555701F, 0.8314696F, 0.0F}, {0.5141026F, 0.8577286F, 0.0F}, {0.4713965F, 0.8819212F, 0.0F}, {0.427555F, 0.9039893F, 0.0F},
	{0.3826833F, 0.9238795F, 0.0F}, {0.3368897F, 0.941544F, 0.0F}, {0.2902846F, 0.9569402F, 0.0F}, {0.2429801F, 0.9700312F, 0.0F},
	{0.1950901F, 0.9807852F, 0.0F}, {0.1467304F, 0.9891765F, 0.0F}, {0.098017F, 0.9951846F, 0.0F}, {0.0490676F, 0.9987953F, 0.0F},
	{0.0F, 1.0F, 0.0F},

	{-1.0F, 0.0F, 0.0F}, {-0.9987953F, 0.0490676F, 0.0F}, {-0.9951846F, 0.098017F, 0.0F}, {-0.9891765F, 0.1467304F, 0.0F},
	{-0.9807852F, 0.1950902F, 0.0F}, {-0.9700312F, 0.2429801F, 0.0F}, {-0.9569402F, 0.2902846F, 0.0F}, {-0.941544F, 0.3368898F, 0.0F},
	{-0.9238795F, 0.3826833F, 0.0F}, {-0.9039893F, 0.427555F, 0.0F}, {-0.8819211F, 0.4713966F, 0.0F}, {-0.8577286F, 0.5141026F, 0.0F},
	{-0.8314695F, 0.5555702F, 0.0F}, {-0.8032075F, 0.5956993F, 0.0F}, {-0.7730103F, 0.6343933F, 0.0F}, {-0.740951F, 0.6715589F, 0.0F},
	{-0.7071067F, 0.7071067F, 0.0F}, {-0.6715588F, 0.7409511F, 0.0F}, {-0.6343932F, 0.7730103F, 0.0F}, {-0.5956993F, 0.8032075F, 0.0F},
	{-0.5555701F, 0.8314696F, 0.0F}, {-0.5141026F, 0.8577286F, 0.0F}, {-0.4713965F, 0.8819212F, 0.0F}, {-0.427555F, 0.9039893F, 0.0F},
	{-0.3826833F, 0.9238795F, 0.0F}, {-0.3368897F, 0.941544F, 0.0F}, {-0.2902846F, 0.9569402F, 0.0F}, {-0.2429801F, 0.9700312F, 0.0F},
	{-0.1950901F, 0.9807852F, 0.0F}, {-0.1467304F, 0.9891765F, 0.0F}, {-0.098017F, 0.9951846F, 0.0F}, {-0.0490676F, 0.9987953F, 0.0F},
	{0.0F, 1.0F, 0.0F},

	{1.0F, 0.0F, 0.0F}, {0.9987953F, -0.0490676F, 0.0F}, {0.9951846F, -0.098017F, 0.0F}, {0.9891765F, -0.1467304F, 0.0F},
	{0.9807852F, -0.1950902F, 0.0F}, {0.9700312F, -0.2429801F, 0.0F}, {0.9569402F, -0.2902846F, 0.0F}, {0.941544F, -0.3368898F, 0.0F},
	{0.9238795F, -0.3826833F, 0.0F}, {0.9039893F, -0.427555F, 0.0F}, {0.8819211F, -0.4713966F, 0.0F}, {0.8577286F, -0.5141026F, 0.0F},
	{0.8314695F, -0.5555702F, 0.0F}, {0.8032075F, -0.5956993F, 0.0F}, {0.7730103F, -0.6343933F, 0.0F}, {0.740951F, -0.6715589F, 0.0F},
	{0.7071067F, -0.7071067F, 0.0F}, {0.6715588F, -0.7409511F, 0.0F}, {0.6343932F, -0.7730103F, 0.0F}, {0.5956993F, -0.8032075F, 0.0F},
	{0.5555701F, -0.8314696F, 0.0F}, {0.5141026F, -0.8577286F, 0.0F}, {0.4713965F, -0.8819212F, 0.0F}, {0.427555F, -0.9039893F, 0.0F},
	{0.3826833F, -0.9238795F, 0.0F}, {0.3368897F, -0.941544F, 0.0F}, {0.2902846F, -0.9569402F, 0.0F}, {0.2429801F, -0.9700312F, 0.0F},
	{0.1950901F, -0.9807852F, 0.0F}, {0.1467304F, -0.9891765F, 0.0F}, {0.098017F, -0.9951846F, 0.0F}, {0.0490676F, -0.9987953F, 0.0F},
	{0.0F, -1.0F, 0.0F},

	{-1.0F, 0.0F, 0.0F}, {-0.9987953F, -0.0490676F, 0.0F}, {-0.9951846F, -0.098017F, 0.0F}, {-0.9891765F, -0.1467304F, 0.0F},
	{-0.9807852F, -0.1950902F, 0.0F}, {-0.9700312F, -0.2429801F, 0.0F}, {-0.9569402F, -0.2902846F, 0.0F}, {-0.941544F, -0.3368898F, 0.0F},
	{-0.9238795F, -0.3826833F, 0.0F}, {-0.9039893F, -0.427555F, 0.0F}, {-0.8819211F, -0.4713966F, 0.0F}, {-0.8577286F, -0.5141026F, 0.0F},
	{-0.8314695F, -0.5555702F, 0.0F}, {-0.8032075F, -0.5956993F, 0.0F}, {-0.7730103F, -0.6343933F, 0.0F}, {-0.740951F, -0.6715589F, 0.0F},
	{-0.7071067F, -0.7071067F, 0.0F}, {-0.6715588F, -0.7409511F, 0.0F}, {-0.6343932F, -0.7730103F, 0.0F}, {-0.5956993F, -0.8032075F, 0.0F},
	{-0.5555701F, -0.8314696F, 0.0F}, {-0.5141026F, -0.8577286F, 0.0F}, {-0.4713965F, -0.8819212F, 0.0F}, {-0.427555F, -0.9039893F, 0.0F},
	{-0.3826833F, -0.9238795F, 0.0F}, {-0.3368897F, -0.941544F, 0.0F}, {-0.2902846F, -0.9569402F, 0.0F}, {-0.2429801F, -0.9700312F, 0.0F},
	{-0.1950901F, -0.9807852F, 0.0F}, {-0.1467304F, -0.9891765F, 0.0F}, {-0.098017F, -0.9951846F, 0.0F}, {-0.0490676F, -0.9987953F, 0.0F},
	{0.0F, -1.0F, 0.0F}
};


EditorGizmo::EditorGizmo(Editor *editor, const EditorManipulator *manipulator) :
		axisVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		arrowVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		squareVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		circleVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		boxVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		boxColorAttribute(ColorRGBA(0.5F, 1.0F, 1.0F, 1.0F)),
		boxTextureAttribute(&EditorManipulator::outlineTextureHeader, &EditorManipulator::outlineTextureImage),
		boxRenderable(kRenderLines),
		faceVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		faceColorAttribute(kAttributeMutable),
		faceRenderable(kRenderQuads),
		edgeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		edgeColorAttribute(kAttributeMutable),
		edgeTextureAttribute(&edgeTextureHeader, edgeTextureImage),
		edgeRenderable(kRenderLines)
{
	worldEditor = editor;
	gizmoManipulator = manipulator;

	axisVertexBuffer.Establish(sizeof(Point3D) * 6, gizmoAxisVertex);
	arrowVertexBuffer.Establish(sizeof(ArrowVertex) * 12, gizmoArrowVertex);
	squareVertexBuffer.Establish(sizeof(Point3D) * 36, gizmoSquareVertex);
	circleVertexBuffer.Establish(sizeof(Point3D) * 396, gizmoCircleVertex);

	for (machine a = 0; a < 3; a++)
	{
		new(axisRenderable[a]) Renderable(kRenderLines);

		axisRenderable[a]->SetAmbientBlendState(kBlendPremultInterp);
		axisRenderable[a]->SetShaderFlags(kShaderAmbientEffect | kShaderProjectiveTexture);
		axisRenderable[a]->SetTransformable(manipulator->GetTargetNode());
		axisRenderable[a]->SetGeometryShaderIndex(kGeometryShaderExpandLineSegment);
		axisRenderable[a]->SetRenderParameterPointer(&axisSizeVector);

		axisRenderable[a]->SetVertexCount(2);
		axisRenderable[a]->SetVertexAttributeArray(kArrayPosition, a * (sizeof(Point3D) * 2), 3);

		new(axisColorAttribute[a]) DiffuseAttribute(gizmoAxisColor[a]);
		new(axisTextureAttribute[a]) DiffuseTextureAttribute(&EditorManipulator::outlineTextureHeader, &EditorManipulator::outlineTextureImage);

		axisAttributeList[a].Append(axisColorAttribute[a]);
		axisAttributeList[a].Append(axisTextureAttribute[a]);
		axisRenderable[a]->SetMaterialAttributeList(&axisAttributeList[a]);

		axisRenderable[a]->SetVertexBuffer(kVertexBufferAttributeArray, &axisVertexBuffer, sizeof(Point3D));
	}

	for (machine a = 0; a < 3; a++)
	{
		new(arrowRenderable[a]) Renderable(kRenderQuads);

		arrowRenderable[a]->SetAmbientBlendState(kBlendInterpolate);
		arrowRenderable[a]->SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard | kShaderScaleVertex);
		arrowRenderable[a]->SetTransformable(manipulator->GetTargetNode());
		arrowRenderable[a]->SetRenderParameterPointer(&arrowSizeVector);

		arrowRenderable[a]->SetVertexCount(4);
		arrowRenderable[a]->SetVertexAttributeArray(kArrayPosition, a * (sizeof(ArrowVertex) * 4), 3);
		arrowRenderable[a]->SetVertexAttributeArray(kArrayTangent, a * (sizeof(ArrowVertex) * 4) + sizeof(Point3D), 4);
		arrowRenderable[a]->SetVertexAttributeArray(kArrayTexcoord, a * (sizeof(ArrowVertex) * 4) + sizeof(Point3D) + sizeof(Vector4D), 2);

		new(arrowColorAttribute[a]) DiffuseAttribute(gizmoAxisColor[a], kAttributeMutable);
		new(arrowTextureAttribute[a]) DiffuseTextureAttribute("WorldEditor/arrow");

		arrowAttributeList[a].Append(arrowColorAttribute[a]);
		arrowAttributeList[a].Append(arrowTextureAttribute[a]);
		arrowRenderable[a]->SetMaterialAttributeList(&arrowAttributeList[a]);

		arrowRenderable[a]->SetVertexBuffer(kVertexBufferAttributeArray, &arrowVertexBuffer, sizeof(ArrowVertex));
	}

	for (machine a = 0; a < 12; a++)
	{
		new(squareRenderable[a]) Renderable(kRenderLineStrip);

		squareRenderable[a]->SetAmbientBlendState(kBlendPremultInterp);
		squareRenderable[a]->SetShaderFlags(kShaderAmbientEffect | kShaderProjectiveTexture);
		squareRenderable[a]->SetTransformable(manipulator->GetTargetNode());
		squareRenderable[a]->SetGeometryShaderIndex(kGeometryShaderExpandLineSegment);
		squareRenderable[a]->SetRenderParameterPointer(&squareSizeVector[a]);

		squareRenderable[a]->SetVertexCount(3);
		squareRenderable[a]->SetVertexAttributeArray(kArrayPosition, a * (sizeof(Point3D) * 3), 3);

		new(squareColorAttribute[a]) DiffuseAttribute(ColorRGBA(1.0F, 1.0F, 0.5F, 1.0F));
		new(squareTextureAttribute[a]) DiffuseTextureAttribute(&EditorManipulator::outlineTextureHeader, &EditorManipulator::outlineTextureImage);

		squareAttributeList[a].Append(squareColorAttribute[a]);
		squareAttributeList[a].Append(squareTextureAttribute[a]);
		squareRenderable[a]->SetMaterialAttributeList(&squareAttributeList[a]);

		squareRenderable[a]->SetVertexBuffer(kVertexBufferAttributeArray, &squareVertexBuffer, sizeof(Point3D));
	}

	for (machine a = 0; a < 12; a++)
	{
		new(circleRenderable[a]) Renderable(kRenderLineStrip);

		circleRenderable[a]->SetAmbientBlendState(kBlendPremultInterp);
		circleRenderable[a]->SetShaderFlags(kShaderAmbientEffect | kShaderProjectiveTexture);
		circleRenderable[a]->SetTransformable(manipulator->GetTargetNode());
		circleRenderable[a]->SetGeometryShaderIndex(kGeometryShaderExpandLineSegment);
		circleRenderable[a]->SetRenderParameterPointer(&circleSizeVector[a]);

		circleRenderable[a]->SetVertexCount(33);
		circleRenderable[a]->SetVertexAttributeArray(kArrayPosition, a * (sizeof(Point3D) * 33), 3);

		new(circleColorAttribute[a]) DiffuseAttribute(ColorRGBA(1.0F, 1.0F, 0.5F, 1.0F));
		new(circleTextureAttribute[a]) DiffuseTextureAttribute(&EditorManipulator::outlineTextureHeader, &EditorManipulator::outlineTextureImage);

		circleAttributeList[a].Append(circleColorAttribute[a]);
		circleAttributeList[a].Append(circleTextureAttribute[a]);
		circleRenderable[a]->SetMaterialAttributeList(&circleAttributeList[a]);

		circleRenderable[a]->SetVertexBuffer(kVertexBufferAttributeArray, &circleVertexBuffer, sizeof(Point3D));
	}

	hiliteArrowIndex = -1;
	hiliteSquareIndex = -1;
	hiliteCircleIndex = -1;
	hiliteFaceIndex = -1;
	hiliteEdgeIndex = -1;

	boxRenderable.SetAmbientBlendState(kBlendPremultInterp);
	boxRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderProjectiveTexture);
	boxRenderable.SetTransformable(manipulator->GetTargetNode());
	boxRenderable.SetGeometryShaderIndex(kGeometryShaderExpandLineSegment);
	boxRenderable.SetRenderParameterPointer(&boxExpandVector);
	boxExpandVector.Set(1.0F, 1.0F, 1.0F, 1.0F);

	boxRenderable.SetVertexCount(24);
	boxRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &boxVertexBuffer, sizeof(Point3D));
	boxRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	boxVertexBuffer.Establish(sizeof(Point3D) * 24);

	boxAttributeList.Append(&boxColorAttribute);
	boxAttributeList.Append(&boxTextureAttribute);
	boxRenderable.SetMaterialAttributeList(&boxAttributeList);

	faceRenderable.SetAmbientBlendState(kBlendAccumulate);
	faceRenderable.SetShaderFlags(kShaderAmbientEffect);
	faceRenderable.SetTransformable(manipulator->GetTargetNode());

	faceRenderable.SetVertexCount(4);
	faceRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &faceVertexBuffer, sizeof(Point3D));
	faceRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	faceVertexBuffer.Establish(sizeof(Point3D) * 4);

	faceAttributeList.Append(&faceColorAttribute);
	faceRenderable.SetMaterialAttributeList(&faceAttributeList);

	edgeRenderable.SetAmbientBlendState(kBlendAccumulate);
	edgeRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderProjectiveTexture);
	edgeRenderable.SetTransformable(manipulator->GetTargetNode());
	edgeRenderable.SetGeometryShaderIndex(kGeometryShaderExpandLineSegment);
	edgeRenderable.SetRenderParameterPointer(&edgeExpandVector);
	edgeExpandVector.Set(1.0F, 1.0F, 1.0F, 1.0F);

	edgeRenderable.SetVertexCount(2);
	edgeRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &edgeVertexBuffer, sizeof(Point3D));
	edgeRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	edgeVertexBuffer.Establish(sizeof(Point3D) * 2);

	edgeAttributeList.Append(&edgeColorAttribute);
	edgeAttributeList.Append(&edgeTextureAttribute);
	edgeRenderable.SetMaterialAttributeList(&edgeAttributeList);
}

EditorGizmo::~EditorGizmo()
{
	for (machine a = 11; a >= 0; a--)
	{
		circleTextureAttribute[a]->~DiffuseTextureAttribute();
		circleColorAttribute[a]->~DiffuseAttribute();
		circleRenderable[a]->~Renderable();
	}

	for (machine a = 11; a >= 0; a--)
	{
		squareTextureAttribute[a]->~DiffuseTextureAttribute();
		squareColorAttribute[a]->~DiffuseAttribute();
		squareRenderable[a]->~Renderable();
	}

	for (machine a = 2; a >= 0; a--)
	{
		arrowTextureAttribute[a]->~DiffuseTextureAttribute();
		arrowColorAttribute[a]->~DiffuseAttribute();
		arrowRenderable[a]->~Renderable();
	}

	for (machine a = 2; a >= 0; a--)
	{
		axisTextureAttribute[a]->~DiffuseTextureAttribute();
		axisColorAttribute[a]->~DiffuseAttribute();
		axisRenderable[a]->~Renderable();
	}
}

bool EditorGizmo::PickArrow(const Point3D& origin, const Vector3D& direction, const ArrowVertex *vertex, const Vector3D& axis, float size, Point3D *pickPoint) const
{
	float	s, t;

	if (Math::CalculateNearestParameters(Zero3D, axis, origin, direction, &s, &t))
	{
		Point3D p1 = Zero3D + axis * s;
		Point3D p2 = origin + direction * t;

		float d2 = SquaredMag(p2 - p1);
		float width = vertex[2].tangent.w * size;
		if (d2 < width * width)
		{
			if ((s > Fmax(vertex[0].position.x, vertex[0].position.y, vertex[0].position.z) * size) && (s < Fmax(vertex[2].position.x, vertex[2].position.y, vertex[2].position.z) * size))
			{
				*pickPoint = p1;
				return (true);
			}
		}
	}

	return (false);
}

bool EditorGizmo::PickSquare(const Point3D& origin, const Vector3D& direction, const Point3D *vertex, const Antivector4D& plane, float size, Point3D *pickPoint) const
{
	Vector4D h = plane ^ Bivector4D(origin, direction);
	if (Fabs(h.w) > K::min_float)
	{
		Point3D p = h.GetPoint3D() / h.w;

		Antivector3D n = Normalize(vertex[0]);
		Antivector4D k1(n, 0.0F);
		Antivector4D k2(-n, vertex[0] * size);

		n = Normalize(vertex[2]);
		Antivector4D k3(n, 0.0F);
		Antivector4D k4(-n, vertex[2] * size);

		if (Fmin(k1 ^ p, k2 ^ p, k3 ^ p, k4 ^ p) > 0.0F)
		{
			*pickPoint = p;
			return (true);
		}
	}

	return (false);
}

bool EditorGizmo::PickCircle(const Point3D& origin, const Vector3D& direction, const Point3D *vertex, const Antivector4D& plane, float size, float scale, Point3D *pickPoint) const
{
	Vector4D h = plane ^ Bivector4D(origin, direction);
	if (Fabs(h.w) > K::min_float)
	{
		Point3D p = h.GetPoint3D() / h.w;

		float width = scale * 8.0F;
		float squaredWidth = width * width;

		for (machine a = 1; a < 33; a++)
		{
			Point3D q1 = vertex[a - 1] * size;
			Point3D q2 = vertex[a] * size;
			Vector3D dq = Normalize(q2 - q1);

			Vector3D v = p - q1;
			float d = v * dq;

			if ((SquaredMag(v) - d * d < squaredWidth) && (d > -width) && (d < Magnitude(q2 - q1) + width))
			{
				*pickPoint = p;
				return (true);
			}
		}
	}

	return (false);
}

void EditorGizmo::HiliteFace(int32 face, float intensity)
{
	hiliteFaceIndex = face;
	if (face >= 0)
	{
		intensity *= 0.125F;
		faceColorAttribute.SetDiffuseColor(ColorRGBA(intensity, intensity, intensity, 1.0F));
	}
}

void EditorGizmo::HiliteEdge(int32 edge, float intensity)
{
	hiliteEdgeIndex = edge;
	if (edge >= 0)
	{
		intensity *= 0.25F;
		edgeColorAttribute.SetDiffuseColor(ColorRGBA(intensity, intensity, intensity, 1.0F));
	}
}

int32 EditorGizmo::PickFace(const Ray *ray, Point3D *point) const
{
	if ((hiliteArrowIndex >= 0) || (hiliteSquareIndex >= 0))
	{
		return (-1);
	}

	const Transform4D& transform = GetTransformable()->GetInverseWorldTransform();
	const Point3D& position = transform * ray->origin;
	const Vector3D& direction = transform * ray->direction;
	const Box3D& box = gizmoBox;

	if ((position.x > box.max.x) && (direction.x < 0.0F))
	{
		float t = (box.max.x - position.x) / direction.x;
		float y = position.y + t * direction.y;
		float z = position.z + t * direction.z;

		if ((y > box.min.y) && (y < box.max.y) && (z > box.min.z) && (z < box.max.z))
		{
			if (point)
			{
				*point = ray->origin + ray->direction * t;
			}

			return (0);
		}
	}

	if ((position.x < box.min.x) && (direction.x > 0.0F))
	{
		float t = (box.min.x - position.x) / direction.x;
		float y = position.y + t * direction.y;
		float z = position.z + t * direction.z;

		if ((y > box.min.y) && (y < box.max.y) && (z > box.min.z) && (z < box.max.z))
		{
			if (point)
			{
				*point = ray->origin + ray->direction * t;
			}

			return (1);
		}
	}

	if ((position.y > box.max.y) && (direction.y < 0.0F))
	{
		float t = (box.max.y - position.y) / direction.y;
		float x = position.x + t * direction.x;
		float z = position.z + t * direction.z;

		if ((x > box.min.x) && (x < box.max.x) && (z > box.min.z) && (z < box.max.z))
		{
			if (point)
			{
				*point = ray->origin + ray->direction * t;
			}

			return (2);
		}
	}

	if ((position.y < box.min.y) && (direction.y > 0.0F))
	{
		float t = (box.min.y - position.y) / direction.y;
		float x = position.x + t * direction.x;
		float z = position.z + t * direction.z;

		if ((x > box.min.x) && (x < box.max.x) && (z > box.min.z) && (z < box.max.z))
		{
			if (point)
			{
				*point = ray->origin + ray->direction * t;
			}

			return (3);
		}
	}

	if ((position.z > box.max.z) && (direction.z < 0.0F))
	{
		float t = (box.max.z - position.z) / direction.z;
		float x = position.x + t * direction.x;
		float y = position.y + t * direction.y;

		if ((x > box.min.x) && (x < box.max.x) && (y > box.min.y) && (y < box.max.y))
		{
			if (point)
			{
				*point = ray->origin + ray->direction * t;
			}

			return (4);
		}
	}

	if ((position.z < box.min.z) && (direction.z > 0.0F))
	{
		float t = (box.min.z - position.z) / direction.z;
		float x = position.x + t * direction.x;
		float y = position.y + t * direction.y;

		if ((x > box.min.x) && (x < box.max.x) && (y > box.min.y) && (y < box.max.y))
		{
			if (point)
			{
				*point = ray->origin + ray->direction * t;
			}

			return (5);
		}
	}

	return (-1);
}

int32 EditorGizmo::PickEdge(const Ray *ray, Point3D *point) const
{
	if (hiliteCircleIndex >= 0)
	{
		return (-1);
	}

	const Transform4D& transform = GetTransformable()->GetInverseWorldTransform();
	Bivector4D rayLine(transform * ray->origin, transform * ray->direction);

	const Box3D& box = gizmoBox;
	Vector3D size = box.GetSize();
	float width = Fmin(size.x, size.y, size.z) * 0.0625F;

	for (machine a = 0; a < 4; a++)
	{
		float y = box[a & 1].y;
		float z = box[(a >> 1) & 1].z;

		Bivector4D edgeLine(Point3D(box.min.x, y, z), Point3D(box.max.x, y, z));
		Antivector3D normal = rayLine.GetTangent() % edgeLine.GetTangent();

		float t = SquaredMag(normal);
		if (t > K::min_float)
		{
			float d = Fabs((rayLine ^ edgeLine) * InverseSqrt(t));
			if (d < width)
			{
				Antivector4D plane = edgeLine ^ normal;
				Vector4D p = rayLine ^ plane;
				if (Fabs(p.w) > K::min_float)
				{
					float w = 1.0F / p.w;
					float x = p.x * w;
					if ((x > box.min.x) && (x < box.max.x))
					{
						if (point)
						{
							*point = GetTransformable()->GetWorldTransform() * Point3D(x, p.y * w, p.z * w);
						}

						return (a);
					}
				}
			}
		}
	}

	for (machine a = 0; a < 4; a++)
	{
		float x = box[a & 1].x;
		float z = box[(a >> 1) & 1].z;

		Bivector4D edgeLine(Point3D(x, box.min.y, z), Point3D(x, box.max.y, z));
		Antivector3D normal = rayLine.GetTangent() % edgeLine.GetTangent();

		float t = SquaredMag(normal);
		if (t > K::min_float)
		{
			float d = Fabs((rayLine ^ edgeLine) * InverseSqrt(t));
			if (d < width)
			{
				Antivector4D plane = edgeLine ^ normal;
				Vector4D p = rayLine ^ plane;
				if (Fabs(p.w) > K::min_float)
				{
					float w = 1.0F / p.w;
					float y = p.y * w;
					if ((y > box.min.y) && (y < box.max.y))
					{
						if (point)
						{
							*point = GetTransformable()->GetWorldTransform() * Point3D(p.x * w, y, p.z * w);
						}

						return (a + 4);
					}
				}
			}
		}
	}

	for (machine a = 0; a < 4; a++)
	{
		float x = box[a & 1].x;
		float y = box[(a >> 1) & 1].y;

		Bivector4D edgeLine(Point3D(x, y, box.min.z), Point3D(x, y, box.max.z));
		Antivector3D normal = rayLine.GetTangent() % edgeLine.GetTangent();

		float t = SquaredMag(normal);
		if (t > K::min_float)
		{
			float d = Fabs((rayLine ^ edgeLine) * InverseSqrt(t));
			if (d < width)
			{
				Antivector4D plane = edgeLine ^ normal;
				Vector4D p = rayLine ^ plane;
				if (Fabs(p.w) > K::min_float)
				{
					float w = 1.0F / p.w;
					float z = p.z * w;
					if ((z > box.min.z) && (z < box.max.z))
					{
						if (point)
						{
							*point = GetTransformable()->GetWorldTransform() * Point3D(p.x * w, p.y * w, z);
						}

						return (a + 8);
					}
				}
			}
		}
	}

	return (-1);
}

int32 EditorGizmo::PickArrow(const Camera *camera, EditorViewportType viewportType, float lineScale, const Ray *ray)
{
	Vector3D	viewDirection;

	const Transformable *transformable = GetTransformable();
	const Transform4D& inverseTransform = transformable->GetInverseWorldTransform();

	float size = lineScale * kGizmoRenderSizeMultiplier;

	if (viewportType == kEditorViewportFrustum)
	{
		viewDirection = inverseTransform * camera->GetNodePosition();

		float depth = SquaredMag(viewDirection);
		if (depth < K::min_float)
		{
			return (-1);
		}

		float f = InverseSqrt(depth);
		viewDirection *= -f;
		size *= depth * f;
	}
	else
	{
		viewDirection = inverseTransform * camera->GetNodeTransform()[2];
	}

	Point3D rayOrigin = inverseTransform * ray->origin;
	Vector3D rayDirection = inverseTransform * ray->direction;

	unsigned_int32 gizmoState = worldEditor->GetGizmoState();

	if (gizmoState & kGizmoMoveEnable)
	{
		if (Fabs(viewDirection.x) < kGizmoAxisThreshold)
		{
			if (PickArrow(rayOrigin, rayDirection, &gizmoArrowVertex[0], Vector3D(1.0F, 0.0F, 0.0F), size, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackAxis[0].Set(1.0F, 0.0F, 0.0F);

				hiliteArrowIndex = 0;
				trackIndex = 0;
				return (0);
			}
		}

		if (Fabs(viewDirection.y) < kGizmoAxisThreshold)
		{
			if (PickArrow(rayOrigin, rayDirection, &gizmoArrowVertex[4], Vector3D(0.0F, 1.0F, 0.0F), size, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackAxis[0].Set(0.0F, 1.0F, 0.0F);

				hiliteArrowIndex = 1;
				trackIndex = 1;
				return (1);
			}
		}

		if (Fabs(viewDirection.z) < kGizmoAxisThreshold)
		{
			if (PickArrow(rayOrigin, rayDirection, &gizmoArrowVertex[8], Vector3D(0.0F, 0.0F, 1.0F), size, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackAxis[0].Set(0.0F, 0.0F, 1.0F);

				hiliteArrowIndex = 2;
				trackIndex = 2;
				return (2);
			}
		}
	}

	hiliteArrowIndex = -1;
	return (-1);
}

int32 EditorGizmo::PickSquare(const Camera *camera, EditorViewportType viewportType, float lineScale, const Ray *ray)
{
	Vector3D	viewDirection;

	const Transformable *transformable = GetTransformable();
	const Transform4D& inverseTransform = transformable->GetInverseWorldTransform();

	float size = lineScale * kGizmoRenderSizeMultiplier;

	if (viewportType == kEditorViewportFrustum)
	{
		viewDirection = inverseTransform * camera->GetNodePosition();

		float depth = SquaredMag(viewDirection);
		if (depth < K::min_float)
		{
			return (-1);
		}

		float f = InverseSqrt(depth);
		viewDirection *= -f;

		depth *= f;
		size *= depth;
	}
	else
	{
		viewDirection = inverseTransform * camera->GetNodeTransform()[2];
	}

	Point3D rayOrigin = inverseTransform * ray->origin;
	Vector3D rayDirection = inverseTransform * ray->direction;

	unsigned_int32 gizmoState = worldEditor->GetGizmoState();

	if (gizmoState & kGizmoMoveEnable)
	{
		size *= 0.25F;

		if (Fabs(viewDirection.x) > kGizmoPlaneThreshold)
		{
			int32 index = (viewDirection.y > 0.0F) | ((viewDirection.z > 0.0F) << 1);
			if (PickSquare(rayOrigin, rayDirection, &gizmoSquareVertex[index * 3], Antivector4D(1.0F, 0.0F, 0.0F, 0.0F), size, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);

				hiliteSquareIndex = index;
				squarePlaneSign = Fsgn(-rayDirection.x);
				trackIndex = 0;
				return (0);
			}
		}

		if (Fabs(viewDirection.y) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.z > 0.0F) | ((viewDirection.x > 0.0F) << 1)) + 4;
			if (PickSquare(rayOrigin, rayDirection, &gizmoSquareVertex[index * 3], Antivector4D(0.0F, 1.0F, 0.0F, 0.0F), size, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

				hiliteSquareIndex = index;
				squarePlaneSign = Fsgn(-rayDirection.y);
				trackIndex = 1;
				return (1);
			}
		}

		if (Fabs(viewDirection.z) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.x > 0.0F) | ((viewDirection.y > 0.0F) << 1)) + 8;
			if (PickSquare(rayOrigin, rayDirection, &gizmoSquareVertex[index * 3], Antivector4D(0.0F, 0.0F, 1.0F, 0.0F), size, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

				hiliteSquareIndex = index;
				squarePlaneSign = Fsgn(-rayDirection.z);
				trackIndex = 2;
				return (2);
			}
		}
	}

	hiliteSquareIndex = -1;
	return (-1);
}

int32 EditorGizmo::PickCircle(const Camera *camera, EditorViewportType viewportType, float lineScale, const Ray *ray)
{
	Vector3D	viewDirection;

	const Transformable *transformable = GetTransformable();
	const Transform4D& inverseTransform = transformable->GetInverseWorldTransform();

	float scale = lineScale * kGizmoLineScaleMultiplier;
	float size = lineScale * kGizmoRenderSizeMultiplier;

	if (viewportType == kEditorViewportFrustum)
	{
		viewDirection = inverseTransform * camera->GetNodePosition();

		float depth = SquaredMag(viewDirection);
		if (depth < K::min_float)
		{
			return (-1);
		}

		float f = InverseSqrt(depth);
		viewDirection *= -f;

		depth *= f;
		size *= depth;
		scale *= depth;
	}
	else
	{
		viewDirection = inverseTransform * camera->GetNodeTransform()[2];
	}

	Point3D rayOrigin = inverseTransform * ray->origin;
	Vector3D rayDirection = inverseTransform * ray->direction;

	unsigned_int32 gizmoState = worldEditor->GetGizmoState();

	if (gizmoState & kGizmoRotateEnable)
	{
		size *= 0.5F;
		scale *= 0.5F;

		if (Fabs(viewDirection.x) > kGizmoPlaneThreshold)
		{
			int32 index = (viewDirection.y > 0.0F) | ((viewDirection.z > 0.0F) << 1);
			if (PickCircle(rayOrigin, rayDirection, &gizmoCircleVertex[index * 33], Antivector4D(1.0F, 0.0F, 0.0F, 0.0F), size, scale, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackAxis[0].Set(0.0F, 1.0F, 0.0F);
				trackAxis[1].Set(0.0F, 0.0F, 1.0F);
				trackPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);

				hiliteCircleIndex = index;
				trackIndex = 0;
				return (0);
			}
		}

		if (Fabs(viewDirection.y) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.z > 0.0F) | ((viewDirection.x > 0.0F) << 1)) + 4;
			if (PickCircle(rayOrigin, rayDirection, &gizmoCircleVertex[index * 33], Antivector4D(0.0F, 1.0F, 0.0F, 0.0F), size, scale, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackAxis[0].Set(0.0F, 0.0F, 1.0F);
				trackAxis[1].Set(1.0F, 0.0F, 0.0F);
				trackPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

				hiliteCircleIndex = index;
				trackIndex = 1;
				return (1);
			}
		}

		if (Fabs(viewDirection.z) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.x > 0.0F) | ((viewDirection.y > 0.0F) << 1)) + 8;
			if (PickCircle(rayOrigin, rayDirection, &gizmoCircleVertex[index * 33], Antivector4D(0.0F, 0.0F, 1.0F, 0.0F), size, scale, &trackPosition))
			{
				trackTransform = transformable->GetWorldTransform();
				trackInverseTransform = inverseTransform;
				trackAxis[0].Set(1.0F, 0.0F, 0.0F);
				trackAxis[1].Set(0.0F, 1.0F, 0.0F);
				trackPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

				hiliteCircleIndex = index;
				trackIndex = 2;
				return (2);
			}
		}
	}

	hiliteCircleIndex = -1;
	return (-1);
}

bool EditorGizmo::TrackArrow(const Ray *ray, Vector3D *delta) const
{
	float	s, t;

	Point3D origin = trackInverseTransform * ray->origin;
	Vector3D direction = trackInverseTransform * ray->direction;

	if (Math::CalculateNearestParameters(Zero3D, trackAxis[0], origin, direction, &s, &t))
	{
		*delta = trackTransform * (Zero3D + trackAxis[0] * s - trackPosition);
		return (true);
	}

	return (false);
}

bool EditorGizmo::TrackSquare(const Ray *ray, Vector3D *delta) const
{
	Point3D origin = trackInverseTransform * ray->origin;
	Vector3D direction = trackInverseTransform * ray->direction;

	Vector4D h = trackPlane ^ Bivector4D(origin, direction);
	if (h.w * squarePlaneSign > K::min_float)
	{
		*delta = trackTransform * (h.GetPoint3D() / h.w - trackPosition);
		return (true);
	}

	return (false);
}

bool EditorGizmo::TrackCircle(const Ray *ray, float *delta) const
{
	Point3D origin = trackInverseTransform * ray->origin;
	Vector3D direction = trackInverseTransform * ray->direction;

	Vector4D h = trackPlane ^ Bivector4D(origin, direction);
	if (Fabs(h.w) > K::min_float)
	{
		Point3D p = h.GetPoint3D() / h.w;

		Vector3D v1(p * trackAxis[0], p * trackAxis[1], 0.0F);
		Vector3D v2(trackPosition * trackAxis[0], trackPosition * trackAxis[1], 0.0F);

		float f = Acos(v1 * v2 * InverseSqrt(SquaredMag(v1) * SquaredMag(v2)));
		if (v1.x * v2.y - v1.y * v2.x > 0.0F)
		{
			f = -f;
		}

		*delta = f;
		return (true);
	}

	return (false);
}

void EditorGizmo::Render(const ManipulatorRenderData *renderData, List<Renderable> *renderList)
{
	Vector3D	viewDirection;
	bool		visibleFlag[3];

	if (renderData->viewportType == kEditorViewportFrustum)
	{
		RenderBox(renderData, renderList);
	}

	const Transform4D& inverseTransform = GetTransformable()->GetInverseWorldTransform();
	const Camera *camera = renderData->viewportCamera;

	float scale = renderData->lineScale * kGizmoLineScaleMultiplier;
	float size = renderData->lineScale * kGizmoRenderSizeMultiplier;

	if (renderData->viewportType == kEditorViewportFrustum)
	{
		viewDirection = inverseTransform * camera->GetNodePosition();

		float depth = SquaredMag(viewDirection);
		if (depth < K::min_float)
		{
			return;
		}

		float f = InverseSqrt(depth);
		viewDirection *= -f;
		size *= depth * f;
	}
	else
	{
		viewDirection = inverseTransform * camera->GetNodeTransform()[2];
	}

	axisSizeVector.Set(size, size, size, scale);
	arrowSizeVector.Set(size, size, size, size);

	for (machine a = 0; a < 3; a++)
	{
		visibleFlag[a] = (Fabs(viewDirection[a]) < kGizmoAxisThreshold);
	}

	unsigned_int32 gizmoState = worldEditor->GetGizmoState();
	int32 arrowIndex = (gizmoState & kGizmoAxisMoveActive) ? trackIndex : hiliteArrowIndex;

	for (machine a = 0; a < 3; a++)
	{
		if (visibleFlag[a])
		{
			if (a != arrowIndex)
			{
				arrowColorAttribute[a]->SetDiffuseColor(gizmoAxisColor[a]);
			}
			else
			{
				arrowColorAttribute[a]->SetDiffuseColor(K::white);
			}

			renderList->Append(axisRenderable[a]);
			renderList->Append(arrowRenderable[a]);
		}
	}

	float squareSize = size * 0.25F;

	if (gizmoState & kGizmoPlaneMoveActive)
	{
		if (trackIndex == 0)
		{
			for (machine a = 0; a < 4; a++)
			{
				squareSizeVector[a].Set(squareSize, squareSize, squareSize, scale * 0.5F);
				renderList->Append(squareRenderable[a]);
			}
		}
		else if (trackIndex == 1)
		{
			for (machine a = 4; a < 8; a++)
			{
				squareSizeVector[a].Set(squareSize, squareSize, squareSize, scale * 0.5F);
				renderList->Append(squareRenderable[a]);
			}
		}
		else if (trackIndex == 2)
		{
			for (machine a = 8; a < 12; a++)
			{
				squareSizeVector[a].Set(squareSize, squareSize, squareSize, scale * 0.5F);
				renderList->Append(squareRenderable[a]);
			}
		}
	}
	else if (gizmoState & kGizmoMoveEnable)
	{
		if (Fabs(viewDirection.x) > kGizmoPlaneThreshold)
		{
			int32 index = (viewDirection.y > 0.0F) | ((viewDirection.z > 0.0F) << 1);
			squareSizeVector[index].Set(squareSize, squareSize, squareSize, (hiliteSquareIndex == index) ? scale : scale * 0.5F);
			renderList->Append(squareRenderable[index]);
		}

		if (Fabs(viewDirection.y) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.z > 0.0F) | ((viewDirection.x > 0.0F) << 1)) + 4;
			squareSizeVector[index].Set(squareSize, squareSize, squareSize, (hiliteSquareIndex == index) ? scale : scale * 0.5F);
			renderList->Append(squareRenderable[index]);
		}

		if (Fabs(viewDirection.z) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.x > 0.0F) | ((viewDirection.y > 0.0F) << 1)) + 8;
			squareSizeVector[index].Set(squareSize, squareSize, squareSize, (hiliteSquareIndex == index) ? scale : scale * 0.5F);
			renderList->Append(squareRenderable[index]);
		}
	}

	float circleSize = size * 0.5F;

	if (gizmoState & kGizmoRotateActive)
	{
		if (trackIndex == 0)
		{
			for (machine a = 0; a < 4; a++)
			{
				circleSizeVector[a].Set(circleSize, circleSize, circleSize, scale * 0.5F);
				renderList->Append(circleRenderable[a]);
			}
		}
		else if (trackIndex == 1)
		{
			for (machine a = 4; a < 8; a++)
			{
				circleSizeVector[a].Set(circleSize, circleSize, circleSize, scale * 0.5F);
				renderList->Append(circleRenderable[a]);
			}
		}
		else if (trackIndex == 2)
		{
			for (machine a = 8; a < 12; a++)
			{
				circleSizeVector[a].Set(circleSize, circleSize, circleSize, scale * 0.5F);
				renderList->Append(circleRenderable[a]);
			}
		}
	}
	else if (gizmoState & kGizmoRotateEnable)
	{
		if (Fabs(viewDirection.x) > kGizmoPlaneThreshold)
		{
			int32 index = (viewDirection.y > 0.0F) | ((viewDirection.z > 0.0F) << 1);
			circleSizeVector[index].Set(circleSize, circleSize, circleSize, (hiliteCircleIndex == index) ? scale : scale * 0.5F);
			renderList->Append(circleRenderable[index]);
		}

		if (Fabs(viewDirection.y) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.z > 0.0F) | ((viewDirection.x > 0.0F) << 1)) + 4;
			circleSizeVector[index].Set(circleSize, circleSize, circleSize, (hiliteCircleIndex == index) ? scale : scale * 0.5F);
			renderList->Append(circleRenderable[index]);
		}

		if (Fabs(viewDirection.z) > kGizmoPlaneThreshold)
		{
			int32 index = ((viewDirection.x > 0.0F) | ((viewDirection.y > 0.0F) << 1)) + 8;
			circleSizeVector[index].Set(circleSize, circleSize, circleSize, (hiliteCircleIndex == index) ? scale : scale * 0.5F);
			renderList->Append(circleRenderable[index]);
		}
	}
}

void EditorGizmo::RenderBox(const ManipulatorRenderData *renderData, List<Renderable> *renderList)
{
	Box3D& box = gizmoBox;
	box = gizmoManipulator->CalculateNodeBoundingBox();
	EditorManipulator::AdjustBoundingBox(&box);

	boxExpandVector.w = renderData->lineScale * 0.75F;
	edgeExpandVector.w = renderData->lineScale * 6.25F;

	boxVertex[0].Set(box.min.x, box.min.y, box.min.z);
	boxVertex[1].Set(box.max.x, box.min.y, box.min.z);

	boxVertex[2].Set(box.min.x, box.max.y, box.min.z);
	boxVertex[3].Set(box.max.x, box.max.y, box.min.z);

	boxVertex[4].Set(box.min.x, box.min.y, box.max.z);
	boxVertex[5].Set(box.max.x, box.min.y, box.max.z);

	boxVertex[6].Set(box.min.x, box.max.y, box.max.z);
	boxVertex[7].Set(box.max.x, box.max.y, box.max.z);

	boxVertex[8].Set(box.min.x, box.min.y, box.min.z);
	boxVertex[9].Set(box.min.x, box.max.y, box.min.z);

	boxVertex[10].Set(box.max.x, box.min.y, box.min.z);
	boxVertex[11].Set(box.max.x, box.max.y, box.min.z);

	boxVertex[12].Set(box.min.x, box.min.y, box.max.z);
	boxVertex[13].Set(box.min.x, box.max.y, box.max.z);

	boxVertex[14].Set(box.max.x, box.min.y, box.max.z);
	boxVertex[15].Set(box.max.x, box.max.y, box.max.z);

	boxVertex[16].Set(box.min.x, box.min.y, box.min.z);
	boxVertex[17].Set(box.min.x, box.min.y, box.max.z);

	boxVertex[18].Set(box.max.x, box.min.y, box.min.z);
	boxVertex[19].Set(box.max.x, box.min.y, box.max.z);

	boxVertex[20].Set(box.min.x, box.max.y, box.min.z);
	boxVertex[21].Set(box.min.x, box.max.y, box.max.z);

	boxVertex[22].Set(box.max.x, box.max.y, box.min.z);
	boxVertex[23].Set(box.max.x, box.max.y, box.max.z);

	boxVertexBuffer.UpdateBuffer(0, sizeof(Point3D) * 24, boxVertex);
	renderList->Append(&boxRenderable);

	int32 index = hiliteFaceIndex;
	if (index >= 0)
	{
		if (index == 0)
		{
			faceVertex[0].Set(box.max.x, box.min.y, box.min.z);
			faceVertex[1].Set(box.max.x, box.max.y, box.min.z);
			faceVertex[2].Set(box.max.x, box.max.y, box.max.z);
			faceVertex[3].Set(box.max.x, box.min.y, box.max.z);
		}
		else if (index == 1)
		{
			faceVertex[0].Set(box.min.x, box.max.y, box.min.z);
			faceVertex[1].Set(box.min.x, box.min.y, box.min.z);
			faceVertex[2].Set(box.min.x, box.min.y, box.max.z);
			faceVertex[3].Set(box.min.x, box.max.y, box.max.z);
		}
		else if (index == 2)
		{
			faceVertex[0].Set(box.max.x, box.max.y, box.min.z);
			faceVertex[1].Set(box.min.x, box.max.y, box.min.z);
			faceVertex[2].Set(box.min.x, box.max.y, box.max.z);
			faceVertex[3].Set(box.max.x, box.max.y, box.max.z);
		}
		else if (index == 3)
		{
			faceVertex[0].Set(box.min.x, box.min.y, box.min.z);
			faceVertex[1].Set(box.max.x, box.min.y, box.min.z);
			faceVertex[2].Set(box.max.x, box.min.y, box.max.z);
			faceVertex[3].Set(box.min.x, box.min.y, box.max.z);
		}
		else if (index == 4)
		{
			faceVertex[0].Set(box.min.x, box.min.y, box.max.z);
			faceVertex[1].Set(box.max.x, box.min.y, box.max.z);
			faceVertex[2].Set(box.max.x, box.max.y, box.max.z);
			faceVertex[3].Set(box.min.x, box.max.y, box.max.z);
		}
		else if (index == 5)
		{
			faceVertex[0].Set(box.min.x, box.max.y, box.min.z);
			faceVertex[1].Set(box.max.x, box.max.y, box.min.z);
			faceVertex[2].Set(box.max.x, box.min.y, box.min.z);
			faceVertex[3].Set(box.min.x, box.min.y, box.min.z);
		}

		faceVertexBuffer.UpdateBuffer(0, sizeof(Point3D) * 4, faceVertex);
		renderList->Append(&faceRenderable);
	}

	index = hiliteEdgeIndex;
	if (index >= 0)
	{
		if (index < 4)
		{
			float y = box[index & 1].y;
			float z = box[(index >> 1) & 1].z;
			edgeVertex[0].Set(box.min.x, y, z);
			edgeVertex[1].Set(box.max.x, y, z);
		}
		else if (index < 8)
		{
			index -= 4;
			float x = box[index & 1].x;
			float z = box[(index >> 1) & 1].z;
			edgeVertex[0].Set(x, box.min.y, z);
			edgeVertex[1].Set(x, box.max.y, z);
		}
		else
		{
			index -= 8;
			float x = box[index & 1].x;
			float y = box[(index >> 1) & 1].y;
			edgeVertex[0].Set(x, y, box.min.z);
			edgeVertex[1].Set(x, y, box.max.z);
		}

		edgeVertexBuffer.UpdateBuffer(0, sizeof(Point3D) * 2, edgeVertex);
		renderList->Append(&edgeRenderable);
	}
}

void EditorGizmo::Reset(void)
{
	hiliteArrowIndex = -1;
	hiliteSquareIndex = -1;
	hiliteCircleIndex = -1;
}

// ZYUQURM
