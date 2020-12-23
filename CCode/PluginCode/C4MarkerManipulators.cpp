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


#include "C4MarkerManipulators.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const float kPathRenderRadius = 0.015625F;


	const ConstColorRGBA kPathRenderColor = {0.25F, 1.0F, 0.25F, 1.0F};
	const ConstColor4C kTangentRenderColor = {192, 192, 192, 255};
	const ConstColor4C kPointRenderColor = {128, 128, 128, 255};


	const TextureHeader pathTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticEmission,
		kTextureSemanticTransparency,
		kTextureI8,
		8, 4, 1,
		{kTextureClamp, kTextureRepeat, kTextureRepeat},
		4
	};


	const unsigned_int8 pathTextureImage[43] =
	{
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
		0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x80, 0xFF, 0xFF, 0x80,
		0x00, 0x00, 0x00, 0x00,
		0x80, 0x80,
		0x80
	};
}


MarkerManipulator::MarkerManipulator(Marker *marker, const char *iconName) : EditorManipulator(marker, iconName)
{
}

MarkerManipulator::~MarkerManipulator()
{
}

Manipulator *MarkerManipulator::Create(Marker *marker)
{
	switch (marker->GetMarkerType())
	{
		case kMarkerLocator:

			return (new LocatorMarkerManipulator(static_cast<LocatorMarker *>(marker)));

		case kMarkerConnection:

			return (new MarkerManipulator(marker, "WorldEditor/marker/Connection"));

		case kMarkerCube:

			return (new MarkerManipulator(marker, "WorldEditor/marker/Cube"));

		case kMarkerShader:

			return (new MarkerManipulator(marker, "WorldEditor/marker/Shader"));

		case kMarkerPath:

			return (new PathManipulator(static_cast<PathMarker *>(marker)));
	}

	return (nullptr);
}

const char *MarkerManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeMarker, GetTargetNode()->GetMarkerType())));
}

void MarkerManipulator::Preprocess(void)
{
	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	EditorManipulator::Preprocess();
}


LocatorMarkerManipulator::LocatorMarkerManipulator(LocatorMarker *marker) : MarkerManipulator(marker, "WorldEditor/marker/Locator")
{
	SetManipulatorFlags(kManipulatorAdjustablePlacement);
}

LocatorMarkerManipulator::~LocatorMarkerManipulator()
{
}
 
const char *LocatorMarkerManipulator::GetDefaultNodeName(void) const
{
	const LocatorRegistration *registration = LocatorMarker::FindRegistration(GetTargetNode()->GetLocatorType()); 
	if (registration)
	{ 
		const char *name = registration->GetLocatorName();
		if (name)
		{ 
			return (name);
		} 
	} 

	return (MarkerManipulator::GetDefaultNodeName());
}
 

PathManipulator::PathManipulator(PathMarker *path) :
		EditorManipulator(path, "WorldEditor/path/Path"),
		pathVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		pathColorAttribute(kPathRenderColor, kAttributeMutable),
		pathTextureAttribute(&pathTextureHeader, pathTextureImage),
		pathRenderable(kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit),
		tangentVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		tangentTextureAttribute(&pathTextureHeader, pathTextureImage),
		tangentRenderable(kRenderQuads),
		pointVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		pointRenderable(kRenderQuads)
{
	pointSelectionArray = nullptr;
	maxSelectedPointCount = 0;

	pathRenderable.SetAmbientBlendState(kBlendPremultInterp);
	pathRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderOrthoPolyboard | kShaderScaleVertex);
	pathRenderable.SetRenderParameterPointer(&pathSizeVector);
	pathRenderable.SetTransformable(path);

	pathRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &pathVertexBuffer, sizeof(PathVertex));
	pathRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	pathRenderable.SetVertexAttributeArray(kArrayTangent, sizeof(Point3D), 4);
	pathRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector4D), 2);

	pathAttributeList.Append(&pathColorAttribute);
	pathAttributeList.Append(&pathTextureAttribute);
	pathRenderable.SetMaterialAttributeList(&pathAttributeList);

	tangentRenderable.SetAmbientBlendState(kBlendPremultInterp);
	tangentRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexPolyboard | kShaderLinearPolyboard | kShaderOrthoPolyboard | kShaderScaleVertex);
	tangentRenderable.SetRenderParameterPointer(&pathSizeVector);
	tangentRenderable.SetTransformable(path);

	tangentRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &tangentVertexBuffer, sizeof(TangentVertex));
	tangentRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	tangentRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 1);
	tangentRenderable.SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) + sizeof(Color4C), 4);
	tangentRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Color4C) + sizeof(Vector4D), 2);

	tangentAttributeList.Append(&tangentTextureAttribute);
	tangentRenderable.SetMaterialAttributeList(&tangentAttributeList);

	pointRenderable.SetAmbientBlendState(kBlendPremultInterp);
	pointRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderVertexBillboard | kShaderScaleVertex);
	pointRenderable.SetRenderParameterPointer(&pointSizeVector);
	pointRenderable.SetTransformable(path);

	pointRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &pointVertexBuffer, sizeof(PointVertex));
	pointRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	pointRenderable.SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 1);
	pointRenderable.SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D) + sizeof(Color4C), 2);
}

PathManipulator::~PathManipulator()
{
	delete[] pointSelectionArray;
}

const char *PathManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeMarker, kMarkerPath)));
}

bool PathManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	Box3D	bounds;

	GetTargetNode()->GetPath()->GetBoundingBox(&bounds);

	sphere->SetCenter((bounds.min + bounds.max) * 0.5F);
	sphere->SetRadius(Magnitude(bounds.max - bounds.min) * 0.5F);
	return (true);
}

void PathManipulator::Select(void)
{
	EditorManipulator::Select();
	Invalidate();

	pathColorAttribute.SetDiffuseColor(K::white);
}

void PathManipulator::Unselect(void)
{
	EditorManipulator::Unselect();

	delete[] pointSelectionArray;
	pointSelectionArray = nullptr;
	maxSelectedPointCount = 0;

	pathColorAttribute.SetDiffuseColor(kPathRenderColor);
}

const PathComponent *PathManipulator::GetControlPointComponent(const Path *path, int32 *index)
{
	int32 i = *index;

	const PathComponent *component = path->GetFirstPathComponent();
	while (component)
	{
		int32 count = component->GetControlPointCount();
		if (i < count)
		{
			*index = i;
			return (component);
		}

		i -= count;
		component = component->Next();
	}

	return (nullptr);
}

void PathManipulator::SelectControlPoint(int32 index, bool selectTangent)
{
	if (index >= 0)
	{
		const Path *path = GetTargetNode()->GetPath();

		if (!pointSelectionArray)
		{
			int32 pointCount = 0;
			const PathComponent *component = path->GetFirstPathComponent();
			while (component)
			{
				pointCount += component->GetControlPointCount();
				component = component->Next();
			}

			maxSelectedPointCount = pointCount;
			pointSelectionArray = new float[pointCount];
			for (machine a = 0; a < pointCount; a++)
			{
				pointSelectionArray[a] = 0.0F;
			}
		}

		pointSelectionArray[index] = 1.0F;

		if (selectTangent)
		{
			int32 pointIndex = index;
			const PathComponent *component = GetControlPointComponent(path, &pointIndex);
			if (component)
			{
				PathType type = component->GetPathType();
				if (type == kPathElliptical)
				{
					if (pointIndex == 0)
					{
						pointSelectionArray[index + 2] = 1.0F;
					}
				}
				else if (type == kPathBezier)
				{
					if (pointIndex == 0)
					{
						pointSelectionArray[index + 1] = 1.0F;
					}
					else if (pointIndex == 3)
					{
						pointSelectionArray[index - 1] = 1.0F;
					}
				}
			}
		}

		UpdateControlPointSelection();
	}
}

void PathManipulator::UnselectControlPoint(int32 index, bool unselectTangent)
{
	if (index >= 0)
	{
		pointSelectionArray[index] = 0.0F;

		if (unselectTangent)
		{
			int32 pointIndex = index;
			const PathComponent *component = GetControlPointComponent(GetTargetNode()->GetPath(), &pointIndex);
			if (component)
			{
				PathType type = component->GetPathType();
				if (type == kPathElliptical)
				{
					if (pointIndex == 0)
					{
						pointSelectionArray[index + 2] = 0.0F;
					}
				}
				else if (type == kPathBezier)
				{
					if (pointIndex == 0)
					{
						pointSelectionArray[index + 1] = 0.0F;
					}
					else if (pointIndex == 3)
					{
						pointSelectionArray[index - 1] = 0.0F;
					}
				}
			}
		}

		UpdateControlPointSelection();
	}
}

void PathManipulator::UpdateControlPointSelection(void)
{
	GetEditor()->InvalidateNode(GetTargetNode());

	int32 selectedPointCount = 0;
	int32 pointCount = maxSelectedPointCount;
	for (machine a = 0; a < pointCount; a++)
	{
		selectedPointCount += (pointSelectionArray[a] != 0.0F);
	}

	SetSelectionType((selectedPointCount != 0) ? kEditorSelectionVertex : kEditorSelectionObject);
}

void PathManipulator::MoveSelectedControlPoints(const Vector3D& delta, bool maintainTangents)
{
	const float *selection = pointSelectionArray;

	PathComponent *component = GetTargetNode()->GetPath()->GetFirstPathComponent();
	while (component)
	{
		PathType type = component->GetPathType();
		if (type == kPathLinear)
		{
			LinearPathComponent *linearComponent = static_cast<LinearPathComponent *>(component);
			for (machine a = 0; a < 2; a++)
			{
				float strength = selection[a];
				if ((strength > 0.0F) || (maintainTangents))
				{
					linearComponent->SetControlPoint(a, linearComponent->GetControlPoint(a) + delta * strength);
				}
			}

			selection += 2;
		}
		else if (type == kPathElliptical)
		{
			EllipticalPathComponent *ellipticalComponent = static_cast<EllipticalPathComponent *>(component);
			for (machine a = 0; a < 3; a++)
			{
				float strength = selection[a];
				if ((strength > 0.0F) || (maintainTangents))
				{
					ellipticalComponent->SetControlPoint(a, ellipticalComponent->GetControlPoint(a) + delta * strength);
				}
			}

			selection += 3;
		}
		else if (type == kPathBezier)
		{
			BezierPathComponent *bezierComponent = static_cast<BezierPathComponent *>(component);
			for (machine a = 0; a < 4; a++)
			{
				float strength = selection[a];
				if (strength > 0.0F)
				{
					bezierComponent->SetControlPoint(a, bezierComponent->GetControlPoint(a) + delta * strength);
				}

				if (maintainTangents)
				{
					if (a == 1)
					{
						PathComponent *prev = component->Previous();
						if ((prev) && (prev->GetPathType() == kPathBezier) && (selection[a - 2] == 0.0F) && (selection[a - 3] == 0.0F))
						{
							BezierPathComponent *prevBezier = static_cast<BezierPathComponent *>(prev);
							prevBezier->SetControlPoint(2, prevBezier->GetControlPoint(2) - delta * strength);
						}
					}
					else if (a == 2)
					{
						PathComponent *next = component->Next();
						if ((next) && (next->GetPathType() == kPathBezier) && (selection[a + 2] == 0.0F) && (selection[a + 3] == 0.0F))
						{
							BezierPathComponent *nextBezier = static_cast<BezierPathComponent *>(next);
							nextBezier->SetControlPoint(1, nextBezier->GetControlPoint(1) - delta * strength);
						}
					}
				}
			}

			selection += 4;
		}

		component = component->Next();
	}

	GetEditor()->InvalidateNode(GetTargetNode());
}

bool PathManipulator::PickControlPoint(const Ray *ray, PickData *data) const
{
	float r = ray->radius;
	if (r == 0.0F)
	{
		r = kPathRenderRadius * 4.0F;
	}
	else
	{
		r *= 6.0F;
	}

	const Path *path = GetTargetNode()->GetPath();

	int32 pointIndex = 0;
	const PathComponent *component = path->GetFirstPathComponent();
	while (component)
	{
		const PathComponent *nextComponent = component->Next();

		PathType type = component->GetPathType();
		if (type == kPathLinear)
		{
			const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
			for (machine a = 0; a < 2; a++)
			{
				const Point3D& p = linearComponent->GetControlPoint(a);
				Antivector4D plane(ray->direction, p);

				float t = -(plane ^ ray->origin) / (plane ^ ray->direction);
				if ((t > ray->tmin) && (t < ray->tmax))
				{
					Point3D q = ray->origin + ray->direction * t;
					if (SquaredMag(q - p) < r * r)
					{
						data->rayParam = t;
						data->pickIndex[0] = pointIndex;
						data->pickIndex[1] = ((a == 1) && (nextComponent)) ? pointIndex + 1 : -1;
						return (true);
					}
				}

				pointIndex++;
			}
		}
		else if (type == kPathElliptical)
		{
			const EllipticalPathComponent *ellipticalComponent = static_cast<const EllipticalPathComponent *>(component);
			for (machine a = 0; a < 3; a++)
			{
				const Point3D& p = ellipticalComponent->GetControlPoint(a);
				Antivector4D plane(ray->direction, p);

				float t = -(plane ^ ray->origin) / (plane ^ ray->direction);
				if ((t > ray->tmin) && (t < ray->tmax))
				{
					Point3D q = ray->origin + ray->direction * t;
					if (SquaredMag(q - p) < r * r)
					{
						data->rayParam = t;
						data->pickIndex[0] = pointIndex;
						data->pickIndex[1] = ((a == 1) && (nextComponent)) ? pointIndex + 2 : -1;
						return (true);
					}
				}

				pointIndex++;
			}
		}
		else if (type == kPathBezier)
		{
			const BezierPathComponent *bezierComponent = static_cast<const BezierPathComponent *>(component);
			for (machine a = 0; a < 4; a++)
			{
				const Point3D& p = bezierComponent->GetControlPoint(a);
				Antivector4D plane(ray->direction, p);

				float t = -(plane ^ ray->origin) / (plane ^ ray->direction);
				if ((t > ray->tmin) && (t < ray->tmax))
				{
					Point3D q = ray->origin + ray->direction * t;
					if (SquaredMag(q - p) < r * r)
					{
						data->rayParam = t;
						data->pickIndex[0] = pointIndex;
						data->pickIndex[1] = ((a == 3) && (nextComponent)) ? pointIndex + 1 : -1;
						return (true);
					}
				}

				pointIndex++;
			}
		}

		component = nextComponent;
	}

	return (false);
}

bool PathManipulator::Pick(const Ray *ray, PickData *data) const
{
	if ((Selected()) && (PickControlPoint(ray, data)))
	{
		return (true);
	}

	float r = ray->radius;
	if (r == 0.0F)
	{
		r = kPathRenderRadius * 4.0F;
	}

	float r2 = r * r;

	const Path *path = GetTargetNode()->GetPath();
	const PathComponent *component = path->GetFirstPathComponent();
	while (component)
	{
		Box3D	bounds;
		float	u1, u2;

		component->GetBoundingBox(&bounds);

		if (Math::IntersectRayAndSphere(ray, (bounds.min + bounds.max) * 0.5F, Magnitude(bounds.max - bounds.min) * 0.5F, &u1, &u2))
		{
			PathType type = component->GetPathType();
			if (type == kPathLinear)
			{
				const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
				const Point3D& p1 = linearComponent->GetControlPoint(0);
				const Point3D& p2 = linearComponent->GetControlPoint(1);

				if (PickLineSegment(ray, p1, p2, r2, &data->rayParam))
				{
					data->pickIndex[0] = -1;
					return (true);
				}
			}
			else
			{
				Point3D p1 = component->GetPosition(0.0F);
				Vector3D t1 = component->GetTangent(0.0F);

				for (machine a = 1; a <= 32; a++)
				{
					float t = (float) a * 0.03125F;
					Point3D p2 = component->GetPosition(t);
					Vector3D t2 = component->GetTangent(t);
					Vector3D dp = p2 - p1;

					if (Math::CalculateNearestParameters(ray->origin, ray->direction, p1, dp, &u1, &u2))
					{
						if ((u1 > ray->tmin) && (u1 < ray->tmax))
						{
							Point3D q = ray->origin + ray->direction * u1;
							if (SquaredMag(q - p1 - dp * u2) < r2)
							{
								if (((Antivector4D(t1, p1) ^ q) > 0.0F) && ((Antivector4D(t2, p2) ^ q) < 0.0F))
								{
									data->rayParam = u1;
									data->pickIndex[0] = -1;
									return (true);
								}
							}
						}
					}

					p1 = p2;
					t1 = t2;
				}
			}
		}

		component = component->Next();
	}

	return (false);
}

bool PathManipulator::RegionPick(const VisibilityRegion *region) const
{
	const Transform4D& worldTransform = GetTargetNode()->GetWorldTransform();

	const Path *path = GetTargetNode()->GetPath();
	const PathComponent *component = path->GetFirstPathComponent();
	while (component)
	{
		Box3D		bounds;
		Vector3D	axis[3];

		component->GetBoundingBox(&bounds);

		Point3D center = worldTransform * ((bounds.min + bounds.max) * 0.5F);
		axis[0] = worldTransform[0] * ((bounds.max.x - bounds.min.x) * 0.5F);
		axis[1] = worldTransform[1] * ((bounds.max.y - bounds.min.y) * 0.5F);
		axis[2] = worldTransform[2] * ((bounds.max.z - bounds.min.z) * 0.5F);

		if (region->BoxVisible(center, axis))
		{
			PathType type = component->GetPathType();
			if (type == kPathLinear)
			{
				const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
				const Point3D& p1 = linearComponent->GetControlPoint(0);
				const Point3D& p2 = linearComponent->GetControlPoint(1);

				if (region->CylinderVisible(worldTransform * p1, worldTransform * p2, 0.0F))
				{
					return (true);
				}
			}
			else
			{
				Point3D p1 = worldTransform * component->GetPosition(0.0F);

				for (machine a = 1; a <= 32; a++)
				{
					Point3D p2 = worldTransform * component->GetPosition((float) a * 0.03125F);
					if (region->CylinderVisible(p1, p2, 0.0F))
					{
						return (true);
					}

					p1 = p2;
				}
			}
		}

		component = component->Next();
	}

	return (false);
}

void PathManipulator::Update(void)
{
	if (!(GetManipulatorState() & kManipulatorUpdated))
	{
		const PathMarker *pathMarker = GetTargetNode();

		int32 pathCount = 0;
		int32 tangentCount = 0;
		int32 pointCount = 0;

		const Path *path = pathMarker->GetPath();
		const PathComponent *component = path->GetFirstPathComponent();
		while (component)
		{
			pathCount += 2;

			PathType type = component->GetPathType();
			if (type == kPathLinear)
			{
				pathCount += 2;
				pointCount += 8;
			}
			else if (type == kPathElliptical)
			{
				pathCount += 64;
				tangentCount += 4;
				pointCount += 12;
			}
			else if (type == kPathBezier)
			{
				pathCount += 64;
				tangentCount += 8;
				pointCount += 16;
			}

			component = component->Next();
		}

		pathRenderable.InvalidateVertexData();
		pathRenderable.SetVertexCount(pathCount);
		pathVertexBuffer.Establish(sizeof(PathVertex) * pathCount);

		tangentRenderable.InvalidateVertexData();
		tangentRenderable.SetVertexCount(tangentCount);
		tangentVertexBuffer.Establish(sizeof(TangentVertex) * tangentCount);

		pointRenderable.InvalidateVertexData();
		pointRenderable.SetVertexCount(pointCount);
		pointVertexBuffer.Establish(sizeof(PointVertex) * pointCount);

		volatile PathVertex *restrict pathVertex = pathVertexBuffer.BeginUpdate<PathVertex>();

		component = path->GetFirstPathComponent();
		while (component)
		{
			PathType type = component->GetPathType();
			if (type == kPathLinear)
			{
				const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
				const Point3D& p1 = linearComponent->GetControlPoint(0);
				const Point3D& p2 = linearComponent->GetControlPoint(1);
				Vector3D tangent = Normalize(p2 - p1);

				pathVertex[0].position = p1;
				pathVertex[0].tangent.Set(tangent, -1.0F);
				pathVertex[0].texcoord.Set(0.0F, 0.125F);

				pathVertex[1].position = p1;
				pathVertex[1].tangent.Set(tangent, 1.0F);
				pathVertex[1].texcoord.Set(1.0F, 0.125F);

				pathVertex += 2;
			}
			else
			{
				for (machine a = 0; a < 32; a++)
				{
					float t = (float) a * 0.03125F;
					Point3D position = component->GetPosition(t);
					Vector3D tangent = Normalize(component->GetTangent(t));

					pathVertex[0].position = position;
					pathVertex[0].tangent.Set(tangent, -1.0F);
					pathVertex[0].texcoord.Set(0.0F, 0.125F);

					pathVertex[1].position = position;
					pathVertex[1].tangent.Set(tangent, 1.0F);
					pathVertex[1].texcoord.Set(1.0F, 0.125F);

					pathVertex += 2;
				}
			}

			const Point3D& position = component->GetEndPosition();
			Vector3D tangent = Normalize(component->GetEndTangent());

			pathVertex[0].position = position;
			pathVertex[0].tangent.Set(tangent, -1.0F);
			pathVertex[0].texcoord.Set(0.0F, 0.125F);

			pathVertex[1].position = position;
			pathVertex[1].tangent.Set(tangent, 1.0F);
			pathVertex[1].texcoord.Set(1.0F, 0.125F);

			pathVertex += 2;
			component = component->Next();
		}

		pathVertexBuffer.EndUpdate();

		volatile TangentVertex *restrict tangentVertex = (tangentCount != 0) ? tangentVertexBuffer.BeginUpdate<TangentVertex>() : nullptr;
		volatile PointVertex *restrict pointVertex = pointVertexBuffer.BeginUpdate<PointVertex>();

		int32 pointIndex = 0;
		component = path->GetFirstPathComponent();
		while (component)
		{
			PathType type = component->GetPathType();
			if (type == kPathLinear)
			{
				const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);

				for (machine a = 0; a < 2; a++)
				{
					const Point3D& p = linearComponent->GetControlPoint(a);

					if (ControlPointSelected(pointIndex))
					{
						pointVertex[0].position = p;
						pointVertex[0].color.Set(255, 255, 255, 255);
						pointVertex[0].billboard.Set(4.0F, 4.0F);

						pointVertex[1].position = p;
						pointVertex[1].color.Set(255, 255, 255, 255);
						pointVertex[1].billboard.Set(4.0F, -4.0F);

						pointVertex[2].position = p;
						pointVertex[2].color.Set(255, 255, 255, 255);
						pointVertex[2].billboard.Set(-4.0F, -4.0F);

						pointVertex[3].position = p;
						pointVertex[3].color.Set(255, 255, 255, 255);
						pointVertex[3].billboard.Set(-4.0F, 4.0F);
					}
					else
					{
						pointVertex[0].position = p;
						pointVertex[0].color = kPointRenderColor;
						pointVertex[0].billboard.Set(3.0F, 3.0F);

						pointVertex[1].position = p;
						pointVertex[1].color = kPointRenderColor;
						pointVertex[1].billboard.Set(3.0F, -3.0F);

						pointVertex[2].position = p;
						pointVertex[2].color = kPointRenderColor;
						pointVertex[2].billboard.Set(-3.0F, -3.0F);

						pointVertex[3].position = p;
						pointVertex[3].color = kPointRenderColor;
						pointVertex[3].billboard.Set(-3.0F, 3.0F);
					}

					pointIndex++;
					pointVertex += 4;
				}
			}
			else if (type == kPathElliptical)
			{
				const EllipticalPathComponent *ellipticalComponent = static_cast<const EllipticalPathComponent *>(component);
				const Point3D& p1 = ellipticalComponent->GetControlPoint(0);
				const Point3D& p3 = ellipticalComponent->GetControlPoint(2);

				Vector3D dp = p3 - p1;
				float m = Magnitude(dp) * 16.0F;

				tangentVertex[0].position = p1;
				tangentVertex[0].color = kTangentRenderColor;
				tangentVertex[0].tangent.Set(dp, -0.5F);
				tangentVertex[0].texcoord.Set(0.0F, 0.0F);

				tangentVertex[1].position = p1;
				tangentVertex[1].color = kTangentRenderColor;
				tangentVertex[1].tangent.Set(dp, 0.5F);
				tangentVertex[1].texcoord.Set(1.0F, 0.0F);

				tangentVertex[2].position = p3;
				tangentVertex[2].color = kTangentRenderColor;
				tangentVertex[2].tangent.Set(dp, 0.5F);
				tangentVertex[2].texcoord.Set(1.0F, m);

				tangentVertex[3].position = p3;
				tangentVertex[3].color = kTangentRenderColor;
				tangentVertex[3].tangent.Set(dp, -0.5F);
				tangentVertex[3].texcoord.Set(0.0F, m);

				tangentVertex += 4;

				for (machine a = 0; a < 3; a++)
				{
					const Point3D& p = ellipticalComponent->GetControlPoint(a);

					if (ControlPointSelected(pointIndex))
					{
						pointVertex[0].position = p;
						pointVertex[0].color.Set(255, 255, 255, 255);
						pointVertex[0].billboard.Set(4.0F, 4.0F);

						pointVertex[1].position = p;
						pointVertex[1].color.Set(255, 255, 255, 255);
						pointVertex[1].billboard.Set(4.0F, -4.0F);

						pointVertex[2].position = p;
						pointVertex[2].color.Set(255, 255, 255, 255);
						pointVertex[2].billboard.Set(-4.0F, -4.0F);

						pointVertex[3].position = p;
						pointVertex[3].color.Set(255, 255, 255, 255);
						pointVertex[3].billboard.Set(-4.0F, 4.0F);
					}
					else
					{
						pointVertex[0].position = p;
						pointVertex[0].color = kPointRenderColor;
						pointVertex[0].billboard.Set(3.0F, 3.0F);

						pointVertex[1].position = p;
						pointVertex[1].color = kPointRenderColor;
						pointVertex[1].billboard.Set(3.0F, -3.0F);

						pointVertex[2].position = p;
						pointVertex[2].color = kPointRenderColor;
						pointVertex[2].billboard.Set(-3.0F, -3.0F);

						pointVertex[3].position = p;
						pointVertex[3].color = kPointRenderColor;
						pointVertex[3].billboard.Set(-3.0F, 3.0F);
					}

					pointIndex++;
					pointVertex += 4;
				}
			}
			else if (type == kPathBezier)
			{
				const BezierPathComponent *bezierComponent = static_cast<const BezierPathComponent *>(component);
				const Point3D& p1 = bezierComponent->GetControlPoint(0);
				const Point3D& p2 = bezierComponent->GetControlPoint(1);
				const Point3D& p3 = bezierComponent->GetControlPoint(2);
				const Point3D& p4 = bezierComponent->GetControlPoint(3);

				Vector3D t1 = Normalize(p2 - p1);
				Vector3D t2 = Normalize(p3 - p4);
				float m1 = Magnitude(p2 - p1) * 16.0F;
				float m2 = Magnitude(p3 - p4) * 16.0F;

				tangentVertex[0].position = p1;
				tangentVertex[0].color = kTangentRenderColor;
				tangentVertex[0].tangent.Set(t1, -0.5F);
				tangentVertex[0].texcoord.Set(0.0F, 0.0F);

				tangentVertex[1].position = p1;
				tangentVertex[1].color = kTangentRenderColor;
				tangentVertex[1].tangent.Set(t1, 0.5F);
				tangentVertex[1].texcoord.Set(1.0F, 0.0F);

				tangentVertex[2].position = p2;
				tangentVertex[2].color = kTangentRenderColor;
				tangentVertex[2].tangent.Set(t1, 0.5F);
				tangentVertex[2].texcoord.Set(1.0F, m1);

				tangentVertex[3].position = p2;
				tangentVertex[3].color = kTangentRenderColor;
				tangentVertex[3].tangent.Set(t1, -0.5F);
				tangentVertex[3].texcoord.Set(0.0F, m1);

				tangentVertex[4].position = p4;
				tangentVertex[4].color = kTangentRenderColor;
				tangentVertex[4].tangent.Set(t2, -0.5F);
				tangentVertex[4].texcoord.Set(0.0F, 0.0F);

				tangentVertex[5].position = p4;
				tangentVertex[5].color = kTangentRenderColor;
				tangentVertex[5].tangent.Set(t2, 0.5F);
				tangentVertex[5].texcoord.Set(1.0F, 0.0F);

				tangentVertex[6].position = p3;
				tangentVertex[6].color = kTangentRenderColor;
				tangentVertex[6].tangent.Set(t2, 0.5F);
				tangentVertex[6].texcoord.Set(1.0F, m2);

				tangentVertex[7].position = p3;
				tangentVertex[7].color = kTangentRenderColor;
				tangentVertex[7].tangent.Set(t2, -0.5F);
				tangentVertex[7].texcoord.Set(0.0F, m2);

				tangentVertex += 8;

				for (machine a = 0; a < 4; a++)
				{
					const Point3D& p = bezierComponent->GetControlPoint(a);

					if (ControlPointSelected(pointIndex))
					{
						pointVertex[0].position = p;
						pointVertex[0].color.Set(255, 255, 255, 255);
						pointVertex[0].billboard.Set(4.0F, 4.0F);

						pointVertex[1].position = p;
						pointVertex[1].color.Set(255, 255, 255, 255);
						pointVertex[1].billboard.Set(4.0F, -4.0F);

						pointVertex[2].position = p;
						pointVertex[2].color.Set(255, 255, 255, 255);
						pointVertex[2].billboard.Set(-4.0F, -4.0F);

						pointVertex[3].position = p;
						pointVertex[3].color.Set(255, 255, 255, 255);
						pointVertex[3].billboard.Set(-4.0F, 4.0F);
					}
					else
					{
						pointVertex[0].position = p;
						pointVertex[0].color = kPointRenderColor;
						pointVertex[0].billboard.Set(3.0F, 3.0F);

						pointVertex[1].position = p;
						pointVertex[1].color = kPointRenderColor;
						pointVertex[1].billboard.Set(3.0F, -3.0F);

						pointVertex[2].position = p;
						pointVertex[2].color = kPointRenderColor;
						pointVertex[2].billboard.Set(-3.0F, -3.0F);

						pointVertex[3].position = p;
						pointVertex[3].color = kPointRenderColor;
						pointVertex[3].billboard.Set(-3.0F, 3.0F);
					}

					pointIndex++;
					pointVertex += 4;
				}
			}

			component = component->Next();
		}

		pointVertexBuffer.EndUpdate();
		if (tangentCount != 0)
		{
			tangentVertexBuffer.EndUpdate();
		}
	}

	EditorManipulator::Update();
}

void PathManipulator::Render(const ManipulatorRenderData *renderData)
{
	List<Renderable> *renderList = renderData->manipulatorList;
	if (renderList)
	{
		float scale = renderData->viewportScale;
		pathSizeVector.Set(1.0F, 1.0F, 1.0F, scale);
		pointSizeVector.Set(scale, scale, scale, scale);

		renderList->Append(&pathRenderable);
		if (Selected())
		{
			renderList->Append(&tangentRenderable);
			renderList->Append(&pointRenderable);
		}
	}

	EditorManipulator::Render(renderData);
}

// ZYUQURM
