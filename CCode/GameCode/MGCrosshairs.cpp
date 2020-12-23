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


#include "MGCrosshairs.h"
#include "MGGame.h"


using namespace C4;


Crosshairs::Crosshairs() :
		Renderable(kRenderTriangleStrip, kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(kAttributeMutable),
		textureAttribute("game/Crosshairs")
{
	updateFlag = true;
	crosshairType = 0;
	crosshairSize = 0.02F;

	renderList.Append(this);

	SetRenderableFlags(kRenderableCameraTransformInhibit);
	SetShaderFlags(kShaderAmbientEffect);
	SetAmbientBlendState(BlendState(kBlendSourceAlpha, kBlendOne));

	SetVertexCount(4);
	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(CrosshairVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 3);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D), 2);
	vertexBuffer.Establish(sizeof(CrosshairVertex) * 4);

	attributeList.Append(&colorAttribute);
	attributeList.Append(&textureAttribute);
	SetMaterialAttributeList(&attributeList);
}

Crosshairs::~Crosshairs()
{
	renderList.Remove(this);
}

void Crosshairs::HandleCrossTypeEvent(Variable *variable)
{
	crosshairType = variable->GetIntegerValue();
	updateFlag = true;
}

void Crosshairs::HandleCrossColorEvent(Variable *variable)
{
	colorAttribute.SetDiffuseColor(ColorRGBA().SetHexString(variable->GetValue()));
}

void Crosshairs::HandleCrossSizeEvent(Variable *variable)
{
	crosshairSize = (float) (variable->GetIntegerValue() + 2) * 0.01F;
	updateFlag = true;
}

void Crosshairs::Render(void)
{
	if (crosshairType != 0)
	{
		if (updateFlag)
		{
			updateFlag = false;

			volatile CrosshairVertex *restrict vertex = vertexBuffer.BeginUpdate<CrosshairVertex>();

			float f = crosshairSize;
			float z = -TheGame->GetCameraFocalLength();

			float s = (float) (crosshairType & 3) * 0.25F;
			float t = 1.0F - (float) (crosshairType >> 2) * 0.125F;

			float s1 = s + K::one_over_128;
			float s2 = s + 0.25F;
			float t1 = t - K::one_over_256;
			float t2 = t - 0.125F;

			vertex[0].position.Set(-f, f, z);
			vertex[0].texcoord.Set(s1, t1);

			vertex[1].position.Set(-f, -f, z);
			vertex[1].texcoord.Set(s1, t2);

			vertex[2].position.Set(f, f, z);
			vertex[2].texcoord.Set(s2, t1);

			vertex[3].position.Set(f, -f, z);
			vertex[3].texcoord.Set(s2, t2);

			vertexBuffer.EndUpdate();
		}

		TheGraphicsMgr->Draw(&renderList);
	}
}

// ZYUQURM
