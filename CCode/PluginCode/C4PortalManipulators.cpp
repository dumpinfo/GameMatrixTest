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


#include "C4PortalManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kDirectPortalColor = {0.0F, 1.0F, 0.0F, 1.0F};
	const ConstColorRGBA kRemotePortalColor = {0.5F, 0.0F, 1.0F, 1.0F};
	const ConstColorRGBA kOcclusionPortalColor = {1.0F, 0.0F, 0.0F, 1.0F};
}


PortalManipulator::PortalManipulator(Portal *portal, const ColorRGBA& color) :
		EditorManipulator(portal, "WorldEditor/portal/Direct"),
		interiorVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		interiorIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		interiorColorAttribute(kAttributeMutable),
		interiorTextureAttribute("WorldEditor/volume/Portal"),
		interiorRenderable(kRenderIndexedTriangles, kRenderDepthTest | kRenderDepthInhibit),
		outlineVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		outlineColorAttribute(kAttributeMutable),
		outlineTextureAttribute(&outlineTextureHeader, outlineTextureImage),
		outlineRenderable(kRenderLines, kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset),
		directionVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		directionColorAttribute(kAttributeMutable),
		directionTextureAttribute("WorldEditor/direction"),
		directionRenderable(kRenderQuads, kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset)
{
	portalColor = color;
	portalSizeVector.Set(1.0F, 1.0F, 1.0F, 1.0F);
	Unselect();

	interiorRenderable.SetAmbientBlendState(kBlendInterpolate);
	interiorRenderable.SetShaderFlags(kShaderAmbientEffect);
	interiorRenderable.SetTransformable(portal);

	interiorRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &interiorVertexBuffer, sizeof(InteriorVertex));
	interiorRenderable.SetVertexBuffer(kVertexBufferIndexArray, &interiorIndexBuffer);
	interiorRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	interiorRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D), 2);

	interiorAttributeList.Append(&interiorColorAttribute);
	interiorAttributeList.Append(&interiorTextureAttribute);
	interiorRenderable.SetMaterialAttributeList(&interiorAttributeList);

	outlineRenderable.SetAmbientBlendState(kBlendInterpolate);
	outlineRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderProjectiveTexture);
	outlineRenderable.SetDepthOffset(0.0078125F, &portal->GetBoundingSphere()->GetCenter());
	outlineRenderable.SetGeometryShaderIndex(kGeometryShaderExpandLineSegment);
	outlineRenderable.SetRenderParameterPointer(&portalSizeVector);
	outlineRenderable.SetTransformable(portal);

	outlineRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &outlineVertexBuffer, sizeof(OutlineVertex));
	outlineRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);

	outlineAttributeList.Append(&outlineColorAttribute);
	outlineAttributeList.Append(&outlineTextureAttribute);
	outlineRenderable.SetMaterialAttributeList(&outlineAttributeList);

	directionRenderable.SetAmbientBlendState(kBlendInterpolate);
	directionRenderable.SetDepthOffset(0.0078125F, &portal->GetBoundingSphere()->GetCenter());
	directionRenderable.SetTransformable(portal);

	directionRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &directionVertexBuffer, sizeof(DirectionVertex));
	directionRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	directionRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D), 2);

	directionAttributeList.Append(&directionColorAttribute);
	directionAttributeList.Append(&directionTextureAttribute);
	directionRenderable.SetMaterialAttributeList(&directionAttributeList);
	directionRenderable.GetFirstRenderSegment()->SetMaterialState(kMaterialTwoSided);
}

PortalManipulator::~PortalManipulator()
{
}

Manipulator *PortalManipulator::Create(Portal *portal)
{
	switch (portal->GetPortalType())
	{
		case kPortalDirect:

			return (new DirectPortalManipulator(static_cast<DirectPortal *>(portal)));

		case kPortalRemote:

			return (new RemotePortalManipulator(static_cast<RemotePortal *>(portal)));

		case kPortalOcclusion:

			return (new OcclusionPortalManipulator(static_cast<OcclusionPortal *>(portal)));
	}

	return (nullptr);
}

const char *PortalManipulator::GetDefaultNodeName(void) const 
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodePortal, GetTargetNode()->GetPortalType())));
} 

void PortalManipulator::Invalidate(void) 
{
	EditorManipulator::Invalidate();
 
	Zone *zone = GetTargetNode()->GetOwningZone();
	if (zone) 
	{ 
		zone->InvalidateLightRegions();
	}
}
 
void PortalManipulator::Select(void)
{
	EditorManipulator::Select();

	interiorColorAttribute.SetDiffuseColor(portalColor);
	outlineColorAttribute.SetDiffuseColor(K::white);
	directionColorAttribute.SetDiffuseColor(portalColor);
}

void PortalManipulator::Unselect(void)
{
	EditorManipulator::Unselect();

	interiorColorAttribute.SetDiffuseColor(portalColor * 0.625F);
	outlineColorAttribute.SetDiffuseColor(portalColor);
	directionColorAttribute.SetDiffuseColor(portalColor * 0.5F);
}

void PortalManipulator::HandleConnectorUpdate(void)
{
	EditorManipulator::HandleConnectorUpdate();

	Zone *zone = GetTargetNode()->GetOwningZone();
	if (zone)
	{
		zone->InvalidateLightRegions();
	}
}

Box3D PortalManipulator::CalculateNodeBoundingBox(void) const
{
	const PortalObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	Box3D box(vertex[0], vertex[0]);
	for (machine a = 1; a < vertexCount; a++)
	{
		box.Union(vertex[a]);
	}

	return (box);
}

int32 PortalManipulator::GetHandleTable(Point3D *handle) const
{
	const PortalObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	for (machine a = 0; a < vertexCount; a++)
	{
		handle[a] = vertex[a];
	}

	return (vertexCount);
}

void PortalManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	const PortalObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	machine opposite = index;
	float distance = 0.0F;

	const Point3D& p = vertex[index];
	for (machine a = 0; a < vertexCount; a++) if (a != index)
	{
		float d = SquaredMag(vertex[a] - p);
		if (d > distance)
		{
			distance = d;
			opposite = a;
		}
	}

	handleData->handleFlags = 0;
	handleData->oppositeIndex = opposite;
}

bool PortalManipulator::Pick(const Ray *ray, PickData *data) const
{
	Point2D		planePoint;

	const PortalObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	bool interior = false;
	float sz = ray->origin.z;
	float dz = ray->direction.z;
	if ((sz > K::min_float) && (dz < -K::min_float))
	{
		float t = -sz / dz;
		if (t < ray->tmax)
		{
			planePoint = ray->origin.GetPoint2D() + ray->direction.GetVector2D() * t;

			data->rayParam = t;
			data->pickIndex[0] = -1;
			data->pickPoint = planePoint;
			interior = true;
		}
	}
	else
	{
		planePoint.Set(0.0F, 0.0F);
	}

	float r = (ray->radius != 0.0F) ? ray->radius : Editor::kFrustumRenderScale;
	float r2 = r * r * 16.0F;

	const Point3D *p1 = &vertex[vertexCount - 1];
	for (machine a = 0; a < vertexCount; a++)
	{
		float	s, t;

		const Point3D *p2 = &vertex[a];
		Vector3D dp = *p2 - *p1;

		if ((Math::CalculateNearestParameters(*p1, dp, ray->origin, ray->direction, &s, &t)) && (t < ray->tmax))
		{
			Point3D q = *p1 + dp * s;
			if (SquaredMag(q - ray->origin - ray->direction * t) < r2)
			{
				float f = dp * dp + r2;
				if ((Math::SquaredDistancePointToLine(*p1, ray->origin, ray->direction) < f) && (Math::SquaredDistancePointToLine(*p2, ray->origin, ray->direction) < f))
				{
					data->rayParam = t;
					data->pickIndex[0] = a;
					data->pickPoint = q;
					return (true);
				}
			}
		}

		Vector2D normal(-dp.y, dp.x);
		if (normal * (planePoint - p1->GetPoint2D()) < 0.0F)
		{
			interior = false;
		}

		p1 = p2;
	}

	return (interior);
}

bool PortalManipulator::RegionPick(const VisibilityRegion *region) const
{
	const Transform4D& worldTransform = GetTargetNode()->GetWorldTransform();

	const PortalObject *object = GetObject();
	int32 vertexCount = object->GetVertexCount();
	const Point3D *vertex = object->GetVertexArray();

	Point3D p1 = worldTransform * vertex[vertexCount - 1];
	for (machine a = 0; a < vertexCount; a++)
	{
		Point3D p2 = worldTransform * vertex[a];
		if (region->CylinderVisible(p1, p2, 0.0F))
		{
			return (true);
		}

		p1 = p2;
	}

	return (false);
}

void PortalManipulator::BeginResize(const ManipulatorResizeData *resizeData)
{
	EditorManipulator::BeginResize(resizeData);

	const PortalObject *object = GetObject();
	const Point3D *vertex = object->GetVertexArray();
	originalVertexPosition = vertex[resizeData->handleIndex];
}

Point3D PortalManipulator::ConstrainVertex(const Point3D& original, const Point3D& current, const Point3D& v1, const Point3D& v2)
{
	float x = current.x;
	float y = current.y;

	Vector2D dv(v1.x - v2.x, v1.y - v2.y);
	if ((x - v2.x) * dv.y - (y - v2.y) * dv.x < 0.0F)
	{
		float dx = x - original.x;
		float dy = y - original.y;

		Vector3D plane(dv.y, -dv.x, dv.x * v1.y - dv.y * v1.x);
		float t = -(plane.x * original.x + plane.y * original.y + plane.z) / (plane.x * dx + plane.y * dy);
		x = original.x + t * dx;
		y = original.y + t * dy;
	}

	return (Point3D(x, y, 0.0F));
}

bool PortalManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	if (resizeData->handleFlags == 0)
	{
		Portal *portal = GetTargetNode();
		Point3D p = portal->GetWorldTransform() * (originalVertexPosition + resizeData->resizeDelta);
		p = portal->GetInverseWorldTransform() * GetEditor()->SnapToGrid(p);

		PortalObject *object = GetObject();
		Point3D *vertex = object->GetVertexArray();

		int32 count = object->GetVertexCount();
		int32 index = resizeData->handleIndex;

		const Point3D *v1 = &vertex[(index != count - 1) ? index + 1 : 0];
		const Point3D *v2 = &vertex[(index != 0) ? index - 1 : count - 1];
		p = ConstrainVertex(originalVertexPosition, p, *v1, *v2);

		if (count > 3)
		{
			const Point3D *v3 = &vertex[(index < count - 2) ? index + 2 : index + 2 - count];
			p = ConstrainVertex(originalVertexPosition, p, *v1, *v3);

			v3 = &vertex[(index > 1) ? index - 2 : index - 2 + count];
			p = ConstrainVertex(originalVertexPosition, p, *v3, *v2);
		}

		vertex[index] = p;
	}

	return (false);
}

void PortalManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const PortalObject *object = GetObject();
		int32 vertexCount = object->GetVertexCount();

		interiorRenderable.InvalidateVertexData();
		interiorRenderable.SetVertexCount(vertexCount);
		interiorVertexBuffer.Establish(vertexCount * sizeof(InteriorVertex));

		outlineRenderable.InvalidateVertexData();
		outlineRenderable.SetVertexCount(vertexCount * 2);
		outlineVertexBuffer.Establish(vertexCount * (sizeof(OutlineVertex) * 2));

		directionRenderable.InvalidateVertexData();
		directionRenderable.SetVertexCount(vertexCount * 4);
		directionVertexBuffer.Establish(vertexCount * (sizeof(DirectionVertex) * 4));

		volatile InteriorVertex *restrict interiorVertex = interiorVertexBuffer.BeginUpdate<InteriorVertex>();
		volatile OutlineVertex *restrict outlineVertex = outlineVertexBuffer.BeginUpdate<OutlineVertex>();
		volatile DirectionVertex *restrict directionVertex = directionVertexBuffer.BeginUpdate<DirectionVertex>();

		const Point3D *portalVertex = object->GetVertexArray();
		const Point3D *p1 = &portalVertex[vertexCount - 1];

		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D *p2 = &portalVertex[a];

			interiorVertex[0].position = *p2;
			interiorVertex[0].texcoord.Set(p2->x, p2->y);

			Vector3D dp = *p2 - *p1;
			float m = Magnitude(dp);
			dp /= m;
			m *= 8.0F;

			outlineVertex[0].position = *p1;
			outlineVertex[1].position = *p2;

			directionVertex[0].position = *p1;
			directionVertex[0].texcoord.Set(0.0F, 1.0F);

			directionVertex[1].position.Set(p1->x, p1->y, 0.125F);
			directionVertex[1].texcoord.Set(0.0F, 0.0F);

			directionVertex[2].position.Set(p2->x, p2->y, 0.125F);
			directionVertex[2].texcoord.Set(m, 0.0F);

			directionVertex[3].position = *p2;
			directionVertex[3].texcoord.Set(m, 1.0F);

			interiorVertex++;
			outlineVertex += 2;
			directionVertex += 4;

			p1 = p2;
		}

		directionVertexBuffer.EndUpdate();
		outlineVertexBuffer.EndUpdate();
		interiorVertexBuffer.EndUpdate();

		int32 primitiveCount = vertexCount - 2;
		interiorRenderable.SetPrimitiveCount(primitiveCount);
		interiorIndexBuffer.Establish(primitiveCount * sizeof(Triangle));

		volatile Triangle *restrict interiorTriangle = interiorIndexBuffer.BeginUpdate<Triangle>();
		for (machine a = 0; a < primitiveCount; a++)
		{
			interiorTriangle[a].Set(0, a + 1, a + 2);
		}

		interiorIndexBuffer.EndUpdate();
	}

	EditorManipulator::Update();
}

void PortalManipulator::Render(const ManipulatorRenderData *renderData)
{
	List<Renderable> *renderList = renderData->manipulatorList;
	if (renderList)
	{
		portalSizeVector.w = renderData->lineScale;

		renderList->Append(&interiorRenderable);
		renderList->Append(&outlineRenderable);
		renderList->Append(&directionRenderable);
	}

	EditorManipulator::Render(renderData);
}


DirectPortalManipulator::DirectPortalManipulator(DirectPortal *portal) : PortalManipulator(portal, kDirectPortalColor)
{
}

DirectPortalManipulator::~DirectPortalManipulator()
{
}


RemotePortalManipulator::RemotePortalManipulator(RemotePortal *portal) : PortalManipulator(portal, kRemotePortalColor)
{
}

RemotePortalManipulator::~RemotePortalManipulator()
{
}


OcclusionPortalManipulator::OcclusionPortalManipulator(OcclusionPortal *portal) : PortalManipulator(portal, kOcclusionPortalColor)
{
}

OcclusionPortalManipulator::~OcclusionPortalManipulator()
{
}

// ZYUQURM
