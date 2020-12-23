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


#include "C4GeometryManipulators.h"
#include "C4TerrainTools.h"
#include "C4WaterTools.h"
#include "C4WorldEditor.h"


using namespace C4;


namespace
{
	const float kRayExpand = 4.0F;
}


GeometryManipulator::GeometryManipulator(Geometry *geometry) :
		EditorManipulator(geometry, "WorldEditor/geometry/Box"),
		selectionIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		selectionDiffuseColor(TheWorldEditor->GetSurfaceColor()),
		selectionRenderable(kRenderIndexedTriangles)
{
	geometryColor.Set(0.5F, 0.5F, 0.5F, 1.0F);
	geometry->SetWireframeColorPointer(&geometryColor);

	selectedSurfaceArray = nullptr;
	selectionDetailLevel = -1;

	selectionRenderable.SetAmbientBlendState(kBlendAccumulate);
	selectionRenderable.SetVertexBuffer(kVertexBufferIndexArray, &selectionIndexBuffer);

	selectionAttributeList.Append(&selectionDiffuseColor);
	selectionRenderable.SetMaterialAttributeList(&selectionAttributeList);
}

GeometryManipulator::~GeometryManipulator()
{
	delete[] selectedSurfaceArray;
}

Manipulator *GeometryManipulator::Create(Geometry *geometry)
{
	GeometryType geometryType = geometry->GetGeometryType();
	if (geometryType == kGeometryPrimitive)
	{
		PrimitiveGeometry *primitive = static_cast<PrimitiveGeometry *>(geometry);
		switch (primitive->GetPrimitiveType())
		{
			case kPrimitivePlate:

				return (new PlateGeometryManipulator(static_cast<PlateGeometry *>(primitive)));

			case kPrimitiveDisk:

				return (new DiskGeometryManipulator(static_cast<DiskGeometry *>(primitive)));

			case kPrimitiveHole:

				return (new HoleGeometryManipulator(static_cast<HoleGeometry *>(primitive)));

			case kPrimitiveAnnulus:

				return (new AnnulusGeometryManipulator(static_cast<AnnulusGeometry *>(primitive)));

			case kPrimitiveBox:

				return (new BoxGeometryManipulator(static_cast<BoxGeometry *>(primitive)));

			case kPrimitivePyramid:

				return (new PyramidGeometryManipulator(static_cast<PyramidGeometry *>(primitive)));

			case kPrimitiveCylinder:

				return (new CylinderGeometryManipulator(static_cast<CylinderGeometry *>(primitive)));

			case kPrimitiveCone:

				return (new ConeGeometryManipulator(static_cast<ConeGeometry *>(primitive)));

			case kPrimitiveTruncatedCone:

				return (new TruncatedConeGeometryManipulator(static_cast<TruncatedConeGeometry *>(primitive)));

			case kPrimitiveSphere:

				return (new SphereGeometryManipulator(static_cast<SphereGeometry *>(primitive)));

			case kPrimitiveDome:

				return (new DomeGeometryManipulator(static_cast<DomeGeometry *>(primitive)));

			case kPrimitiveTorus:

				return (new TorusGeometryManipulator(static_cast<TorusGeometry *>(primitive)));

			case kPrimitiveTube:

				return (new TubeGeometryManipulator(static_cast<TubeGeometry *>(primitive)));

			case kPrimitiveExtrusion:
 
				return (new ExtrusionGeometryManipulator(static_cast<ExtrusionGeometry *>(primitive)));

			case kPrimitiveRevolution: 

				return (new RevolutionGeometryManipulator(static_cast<RevolutionGeometry *>(primitive))); 

			case kPrimitiveRope:
 
				return (new RopeGeometryManipulator(static_cast<RopeGeometry *>(primitive)));
 
			case kPrimitiveCloth: 

				return (new ClothGeometryManipulator(static_cast<ClothGeometry *>(primitive)));
		}
	} 
	else if (geometryType == kGeometryGeneric)
	{
		return (new GenericGeometryManipulator(static_cast<GenericGeometry *>(geometry)));
	}
	else if (geometryType == kGeometryTerrain)
	{
		return (new TerrainGeometryManipulator(static_cast<TerrainGeometry *>(geometry)));
	}
	else if (geometryType == kGeometryWater)
	{
		return (new WaterGeometryManipulator(static_cast<WaterGeometry *>(geometry)));
	}
	else if (geometryType == kGeometryHorizonWater)
	{
		return (new HorizonWaterGeometryManipulator(static_cast<HorizonWaterGeometry *>(geometry)));
	}

	return (new GeometryManipulator(geometry));
}

const char *GeometryManipulator::GetDefaultNodeName(void) const
{
	const Geometry *geometry = GetTargetNode();
	GeometryType type = geometry->GetGeometryType();
	if (type == kGeometryPrimitive)
	{
		const PrimitiveGeometry *primitive = static_cast<const PrimitiveGeometry *>(GetTargetNode());
		return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeGeometry, kGeometryPrimitive, primitive->GetPrimitiveType())));
	}

	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeGeometry, kGeometryGeneric)));
}

void GeometryManipulator::Select(void)
{
	EditorManipulator::Select();
	GetTargetNode()->SetWireframeColorPointer(&K::white);
}

void GeometryManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	GetTargetNode()->SetWireframeColorPointer(&geometryColor);

	delete[] selectedSurfaceArray;
	selectedSurfaceArray = nullptr;

	selectionIndexBuffer.Establish(0);
}

void GeometryManipulator::Hilite(void)
{
	EditorManipulator::Hilite();
	geometryColor.Set(0.25F, 0.75F, 0.375F, 1.0F);
}

void GeometryManipulator::Unhilite(void)
{
	EditorManipulator::Unhilite();
	geometryColor.Set(0.5F, 0.5F, 0.5F, 1.0F);
}

void GeometryManipulator::SelectSurface(unsigned_int32 index)
{
	if (!selectedSurfaceArray)
	{
		int32 surfaceCount = Max(GetTargetNode()->GetObject()->GetSurfaceCount(), 1);
		selectedSurfaceArray = new bool[surfaceCount];
		for (machine a = 0; a < surfaceCount; a++)
		{
			selectedSurfaceArray[a] = false;
		}
	}

	selectedSurfaceArray[index] = true;
	UpdateSurfaceSelection();
}

void GeometryManipulator::UnselectSurface(unsigned_int32 index)
{
	selectedSurfaceArray[index] = false;
	UpdateSurfaceSelection();
}

int32 GeometryManipulator::GetSelectedSurfaceCount(void) const
{
	int32 count = 0;

	if (GetSelectionType() == kEditorSelectionSurface)
	{
		const GeometryObject *object = GetTargetNode()->GetObject();
		int32 surfaceCount = Max(object->GetSurfaceCount(), 1);

		for (machine a = 0; a < surfaceCount; a++)
		{
			count += selectedSurfaceArray[a];
		}
	}

	return (count);
}

void GeometryManipulator::UpdateSurfaceSelection(void)
{
	if (selectedSurfaceArray)
	{
		selectionRenderable.InvalidateShaderData();

		const Geometry *geometry = GetTargetNode();
		int32 level = geometry->GetDetailLevel();
		selectionDetailLevel = level;

		const GeometryObject *object = geometry->GetObject();
		const Mesh *mesh = object->GetGeometryLevel(level);
		int32 primitiveCount = mesh->GetPrimitiveCount();

		selectionIndexBuffer.Establish(sizeof(Triangle) * primitiveCount);
		volatile Triangle *restrict selectedTriangle = selectionIndexBuffer.BeginUpdate<Triangle>();

		int32 selectedPrimitiveCount = 0;
		const Triangle *geometryTriangle = mesh->GetArray<Triangle>(kArrayPrimitive);

		const unsigned_int16 *surfaceIndex = mesh->GetArray<unsigned_int16>(kArraySurfaceIndex);
		if (surfaceIndex)
		{
			for (machine a = 0; a < primitiveCount; a++)
			{
				const Triangle& triangle = geometryTriangle[a];
				if (selectedSurfaceArray[surfaceIndex[triangle.index[0]]])
				{
					selectedTriangle[selectedPrimitiveCount++] = triangle;
				}
			}

			selectionRenderable.SetPrimitiveCount(selectedPrimitiveCount);
		}
		else if (selectedSurfaceArray[0])
		{
			selectedPrimitiveCount = primitiveCount;
			selectionRenderable.SetPrimitiveCount(primitiveCount);

			for (machine a = 0; a < primitiveCount; a++)
			{
				selectedTriangle[a] = geometryTriangle[a];
			}
		}

		selectionIndexBuffer.EndUpdate();

		SetSelectionType((selectedPrimitiveCount != 0) ? kEditorSelectionSurface : kEditorSelectionObject);
	}
}

void GeometryManipulator::HandleSizeUpdate(int32 count, const float *size)
{
	Geometry *geometry = GetTargetNode();
	Object *object = geometry->GetObject();
	if (object)
	{
		float	objectSize[kMaxObjectSizeCount];

		for (machine a = 0; a < count; a++)
		{
			objectSize[a] = Fmax(size[a], kSizeEpsilon);
		}

		object->SetObjectSize(objectSize);
		GetEditor()->RebuildGeometry(geometry);
	}
}

bool GeometryManipulator::MaterialSettable(void) const
{
	return (true);
}

int32 GeometryManipulator::GetMaterialCount(void) const
{
	return (GetTargetNode()->GetMaterialCount());
}

MaterialObject *GeometryManipulator::GetMaterial(int32 index) const
{
	const Geometry *geometry = GetTargetNode();
	return (geometry->GetMaterialObject(index % geometry->GetMaterialCount()));
}

void GeometryManipulator::SetMaterial(MaterialObject *material)
{
	Geometry *geometry = GetTargetNode();

	int32 count = geometry->GetMaterialCount();
	for (machine a = 0; a < count; a++)
	{
		geometry->SetMaterialObject(a, material);
	}
}

void GeometryManipulator::ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial)
{
	bool replaced = false;

	Geometry *geometry = GetTargetNode();
	int32 count = geometry->GetMaterialCount();
	for (machine a = 0; a < count; a++)
	{
		if (geometry->GetMaterialObject(a) == oldMaterial)
		{
			geometry->SetMaterialObject(a, newMaterial);
			replaced = true;
		}
	}

	if (replaced)
	{
		geometry->InvalidateShaderData();
	}
}

void GeometryManipulator::InvalidateShaderData(void)
{
	GetTargetNode()->InvalidateShaderData();
}

bool GeometryManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const Geometry *geometry = GetTargetNode();

	const Controller *controller = geometry->GetController();
	if ((controller) && (controller->GetBaseControllerType() == kControllerMorph))
	{
		Box3D box = Transform(geometry->GetWorldBoundingBox(), geometry->GetInverseWorldTransform());
		sphere->SetCenter(box.GetCenter());
		sphere->SetRadius(Magnitude(box.GetSize()) * 0.5F);
		return (true);
	}

	return (EditorManipulator::CalculateNodeSphere(sphere));
}

Box3D GeometryManipulator::CalculateNodeBoundingBox(void) const
{
	Box3D	box;

	const Geometry *geometry = GetTargetNode();

	const Controller *controller = geometry->GetController();
	if ((controller) && (controller->GetBaseControllerType() == kControllerMorph))
	{
		return (Transform(geometry->GetWorldBoundingBox(), geometry->GetInverseWorldTransform()));
	}

	if (GetTargetNode()->CalculateBoundingBox(&box))
	{
		return (box);
	}

	return (EditorManipulator::CalculateNodeBoundingBox());
}

int32 GeometryManipulator::GetGeometryPositionArray(const Geometry *geometry, const float **array)
{
	const Controller *controller = geometry->GetController();
	if (controller)
	{
		ControllerType type = controller->GetControllerType();
		if (type == kControllerMorph)
		{
			const MorphController *morphController = static_cast<const MorphController *>(controller);
			if (morphController->GetMorphWeightCount() != 0)
			{
				*array = &morphController->GetMorphPositionArray()->x;
				return (3);
			}
		}
		else if (type == kControllerSkin)
		{
			*array = &static_cast<const SkinController *>(controller)->GetSkinPositionArray()->x;
			return (3);
		}
	}

	const ArrayBundle *bundle = geometry->GetPositionArrayBundle();
	*array = static_cast<float *>(bundle->pointer);
	return (bundle->descriptor.componentCount);
}

bool GeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	Point3D p1 = ray->origin + ray->direction * ray->tmin;
	Point3D p2 = ray->origin + ray->direction * ray->tmax;

	const Geometry *geometry = GetTargetNode();
	const GeometryObject *object = geometry->GetObject();
	if (object->GetCollisionOctree())
	{
		GeometryHitData		geometryHitData;

		if (object->DetectCollision(p1, p2, 0.0F, &geometryHitData))
		{
			float param = geometryHitData.param;
			data->rayParam = ray->tmin * (1.0F - param) + ray->tmax * param;
			data->pickPoint = geometryHitData.position;
			data->pickNormal = geometryHitData.normal;
			data->triangleIndex = geometryHitData.triangleIndex;
			return (true);
		}
	}
	else
	{
		Point3D q1 = p1;
		Point3D q2 = p2;

		const ArrayBundle *triangleBundle = geometry->GetPrimitiveArrayBundle();
		if (triangleBundle)
		{
			const float		*positionArray;

			const Triangle *triangle = static_cast<Triangle *>(triangleBundle->pointer);
			int32 triangleCount = triangleBundle->descriptor.elementCount;

			int32 componentCount = GetGeometryPositionArray(geometry, &positionArray);
			if (componentCount == 3)
			{
				const Point3D *vertex = reinterpret_cast<const Point3D *>(positionArray);

				for (machine index = 0; index < triangleCount; index++)
				{
					Point3D		position;
					Vector3D	normal;
					float		param;

					const Point3D& v1 = vertex[triangle->index[0]];
					const Point3D& v2 = vertex[triangle->index[1]];
					const Point3D& v3 = vertex[triangle->index[2]];

					if (Math::IntersectSegmentAndTriangle(q1, q2, v1, v2, v3, &position, &normal, &param))
					{
						data->rayParam = ray->tmin * (1.0F - param) + ray->tmax * param;
						data->pickPoint = position;
						data->pickNormal = normal;
						data->triangleIndex = index;
						return (true);
					}

					triangle++;
				}
			}
			else
			{
				const Point2D *vertex = reinterpret_cast<const Point2D *>(positionArray);

				for (machine index = 0; index < triangleCount; index++)
				{
					Point3D		position;
					Vector3D	normal;
					float		param;

					Point3D v1 = vertex[triangle->index[0]];
					Point3D v2 = vertex[triangle->index[1]];
					Point3D v3 = vertex[triangle->index[2]];

					if (Math::IntersectSegmentAndTriangle(q1, q2, v1, v2, v3, &position, &normal, &param))
					{
						data->rayParam = ray->tmin * (1.0F - param) + ray->tmax * param;
						data->pickPoint = position;
						data->pickNormal = normal;
						data->triangleIndex = index;
						return (true);
					}

					triangle++;
				}
			}
		}
	}

	return (false);
}

bool GeometryManipulator::RegionPick(const VisibilityRegion *region) const
{
	Antivector4D	objectPlane[kMaxPolyhedronFaceCount];

	const Geometry *geometry = GetTargetNode();
	if (!geometry->Visible(region))
	{
		return (false);
	}

	int32 planeCount = region->GetPlaneCount();
	const Antivector4D *planeArray = region->GetPlaneArray();
	const Transform4D& worldTransform = geometry->GetWorldTransform();
	for (machine a = 0; a < planeCount; a++)
	{
		objectPlane[a] = planeArray[a] * worldTransform;
	}

	const ArrayBundle *triangleBundle = geometry->GetPrimitiveArrayBundle();
	if (triangleBundle)
	{
		const float		*positionArray;
		Point3D			vertexArray[2][kMaxPolyhedronFaceCount + 3];
		int8			location[kMaxPolyhedronFaceCount + 3];

		const Triangle *triangle = static_cast<Triangle *>(triangleBundle->pointer);
		int32 triangleCount = triangleBundle->descriptor.elementCount;

		int32 componentCount = GetGeometryPositionArray(geometry, &positionArray);
		if (componentCount == 3)
		{
			const Point3D *vertex = reinterpret_cast<const Point3D *>(positionArray);

			for (machine index = 0; index < triangleCount; index++)
			{
				vertexArray[0][0] = vertex[triangle->index[0]];
				vertexArray[0][1] = vertex[triangle->index[1]];
				vertexArray[0][2] = vertex[triangle->index[2]];

				int32 vertexCount = 3;
				int32 parity = 0;

				for (machine a = 0; a < planeCount; a++)
				{
					vertexCount = Math::ClipPolygon(vertexCount, vertexArray[parity], objectPlane[a], location, vertexArray[parity ^ 1]);
					if (vertexCount == 0)
					{
						goto next1;
					}

					parity ^= 1;
				}

				if (vertexCount != 0)
				{
					return (true);
				}

				next1:;
				triangle++;
			}
		}
		else
		{
			const Point2D *vertex = reinterpret_cast<const Point2D *>(positionArray);

			for (machine index = 0; index < triangleCount; index++)
			{
				vertexArray[0][0] = vertex[triangle->index[0]];
				vertexArray[0][1] = vertex[triangle->index[1]];
				vertexArray[0][2] = vertex[triangle->index[2]];

				int32 vertexCount = 3;
				int32 parity = 0;

				for (machine a = 0; a < planeCount; a++)
				{
					vertexCount = Math::ClipPolygon(vertexCount, vertexArray[parity], objectPlane[a], location, vertexArray[parity ^ 1]);
					if (vertexCount == 0)
					{
						goto next2;
					}

					parity ^= 1;
				}

				if (vertexCount != 0)
				{
					return (true);
				}

				next2:;
				triangle++;
			}
		}
	}

	return (false);
}

void GeometryManipulator::Render(const ManipulatorRenderData *renderData)
{
	Geometry *geometry = GetTargetNode();
	renderData->geometryList->Append(geometry);

	if (GetSelectionType() == kEditorSelectionSurface)
	{
		if (selectionDetailLevel != geometry->GetDetailLevel())
		{
			UpdateSurfaceSelection();
		}

		selectionRenderable.SetTransformable(geometry->GetTransformable());
		selectionRenderable.SetVertexCount(geometry->GetVertexCount());
		selectionRenderable.SetVertexBuffer(kVertexBufferAttributeArray0, geometry->GetVertexBuffer(kVertexBufferAttributeArray0), geometry->GetVertexBufferStride(kVertexBufferAttributeArray0));
		selectionRenderable.SetVertexBuffer(kVertexBufferAttributeArray1, geometry->GetVertexBuffer(kVertexBufferAttributeArray1), geometry->GetVertexBufferStride(kVertexBufferAttributeArray1));
		selectionRenderable.SetVertexBufferArrayFlags(geometry->GetVertexBufferArrayFlags());
		selectionRenderable.SetVertexAttributeArray(kArrayPosition, geometry->GetVertexAttributeOffset(kArrayPosition), geometry->GetVertexAttributeComponentCount(kArrayPosition));
		renderData->manipulatorList->Append(&selectionRenderable);
	}

	EditorManipulator::Render(renderData);
}


GenericGeometryManipulator::GenericGeometryManipulator(GenericGeometry *geometry) : GeometryManipulator(geometry)
{
}

GenericGeometryManipulator::~GenericGeometryManipulator()
{
}

int32 GenericGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Box3D& bounds = GetObject()->GetBoundingBox();

	float xmin = bounds.min.x;
	float ymin = bounds.min.y;
	float zmin = bounds.min.z;

	float xmax = bounds.max.x;
	float ymax = bounds.max.y;
	float zmax = bounds.max.z;

	float xhalf = (xmin + xmax) * 0.5F;
	float yhalf = (ymin + ymax) * 0.5F;
	float zhalf = (zmin + zmax) * 0.5F;

	handle[0].Set(xmin, ymin, zmin);
	handle[1].Set(xhalf, ymin, zmin);
	handle[2].Set(xmax, ymin, zmin);
	handle[3].Set(xmax, yhalf, zmin);
	handle[4].Set(xmax, ymax, zmin);
	handle[5].Set(xhalf, ymax, zmin);
	handle[6].Set(xmin, ymax, zmin);
	handle[7].Set(xmin, yhalf, zmin);

	handle[8].Set(xmin, ymin, zmax);
	handle[9].Set(xhalf, ymin, zmax);
	handle[10].Set(xmax, ymin, zmax);
	handle[11].Set(xmax, yhalf, zmax);
	handle[12].Set(xmax, ymax, zmax);
	handle[13].Set(xhalf, ymax, zmax);
	handle[14].Set(xmin, ymax, zmax);
	handle[15].Set(xmin, yhalf, zmax);

	handle[16].Set(xmin, ymin, zhalf);
	handle[17].Set(xmax, ymin, zhalf);
	handle[18].Set(xmax, ymax, zhalf);
	handle[19].Set(xmin, ymax, zhalf);

	return (20);
}

void GenericGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[20] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 16) ? index ^ 12 : index ^ 2;
}

void GenericGeometryManipulator::BeginResize(const ManipulatorResizeData *resizeData)
{
	GeometryManipulator::BeginResize(resizeData);
	originalBounds = GetObject()->GetBoundingBox();
}

bool GenericGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	GenericGeometryObject *object = GetObject();

	const Box3D& bounds = object->GetBoundingBox();
	Vector3D newMeshSize = bounds.max - bounds.min;

	const Vector3D *oldMeshSize = reinterpret_cast<const Vector3D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	unsigned_int32 handleFlags = resizeData->handleFlags;
	Vector3D offset(0.0F, 0.0F, 0.0F);

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			newMeshSize.x = Fmax(oldMeshSize->x + dx * 2.0F, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeX)
		{
			newMeshSize.x = Fmax(oldMeshSize->x - dx * 2.0F, kSizeEpsilon);
		}
		else
		{
			newMeshSize.x = oldMeshSize->x;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			newMeshSize.y = Fmax(oldMeshSize->y + dy * 2.0F, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeY)
		{
			newMeshSize.y = Fmax(oldMeshSize->y - dy * 2.0F, kSizeEpsilon);
		}
		else
		{
			newMeshSize.y = oldMeshSize->y;
		}

		if (handleFlags & kManipulatorHandlePositiveZ)
		{
			newMeshSize.z = Fmax(oldMeshSize->z + dz * 2.0F, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeZ)
		{
			newMeshSize.z = Fmax(oldMeshSize->z - dz * 2.0F, kSizeEpsilon);
		}
		else
		{
			newMeshSize.z = oldMeshSize->z;
		}

		float fx = (oldMeshSize->x > 0.0F) ? newMeshSize.x / oldMeshSize->x : 1.0F;
		float fy = (oldMeshSize->y > 0.0F) ? newMeshSize.y / oldMeshSize->y : 1.0F;
		float fz = (oldMeshSize->z > 0.0F) ? newMeshSize.z / oldMeshSize->z : 1.0F;

		if (resizeData->resizeFlags & kManipulatorResizeConstrain)
		{
			if ((fx >= 1.0F) && (fy >= 1.0F) && (fz >= 1.0F))
			{
				if ((fx >= fy) && (fx >= fz))
				{
					newMeshSize.y = oldMeshSize->y * fx;
					newMeshSize.z = oldMeshSize->z * fx;
				}
				else if (fy >= fz)
				{
					newMeshSize.x = oldMeshSize->x * fy;
					newMeshSize.z = oldMeshSize->z * fy;
				}
				else
				{
					newMeshSize.x = oldMeshSize->x * fz;
					newMeshSize.y = oldMeshSize->y * fz;
				}
			}
			else
			{
				if ((fx <= fy) && (fx <= fz))
				{
					newMeshSize.y = oldMeshSize->y * fx;
					newMeshSize.z = oldMeshSize->z * fx;
				}
				else if (fy <= fz)
				{
					newMeshSize.x = oldMeshSize->x * fy;
					newMeshSize.z = oldMeshSize->z * fy;
				}
				else
				{
					newMeshSize.x = oldMeshSize->x * fz;
					newMeshSize.y = oldMeshSize->y * fz;
				}
			}

			fx = (oldMeshSize->x > 0.0F) ? newMeshSize.x / oldMeshSize->x : 1.0F;
			fy = (oldMeshSize->y > 0.0F) ? newMeshSize.y / oldMeshSize->y : 1.0F;
			fz = (oldMeshSize->z > 0.0F) ? newMeshSize.z / oldMeshSize->z : 1.0F;
		}

		if (handleFlags & kManipulatorHandlePositiveX)
		{
			float xmin = originalBounds.min.x;
			offset.x = fx * xmin - xmin + (newMeshSize.x - oldMeshSize->x) * 0.5F;
		}
		else
		{
			float xmax = originalBounds.max.x;
			offset.x = fx * xmax - xmax + (newMeshSize.x - oldMeshSize->x) * -0.5F;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			float ymin = originalBounds.min.y;
			offset.y = fy * ymin - ymin + (newMeshSize.y - oldMeshSize->y) * 0.5F;
		}
		else
		{
			float ymax = originalBounds.max.y;
			offset.y = fy * ymax - ymax + (newMeshSize.y - oldMeshSize->y) * -0.5F;
		}

		if (handleFlags & kManipulatorHandlePositiveZ)
		{
			float zmin = originalBounds.min.z;
			offset.z = fz * zmin - zmin + (newMeshSize.z - oldMeshSize->z) * 0.5F;
		}
		else
		{
			float zmax = originalBounds.max.z;
			offset.z = fz * zmax - zmax + (newMeshSize.z - oldMeshSize->z) * -0.5F;
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			newMeshSize.x = Fmax(oldMeshSize->x + dx, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeX)
		{
			newMeshSize.x = Fmax(oldMeshSize->x - dx, kSizeEpsilon);
		}
		else
		{
			newMeshSize.x = oldMeshSize->x;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			newMeshSize.y = Fmax(oldMeshSize->y + dy, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeY)
		{
			newMeshSize.y = Fmax(oldMeshSize->y - dy, kSizeEpsilon);
		}
		else
		{
			newMeshSize.y = oldMeshSize->y;
		}

		if (handleFlags & kManipulatorHandlePositiveZ)
		{
			newMeshSize.z = Fmax(oldMeshSize->z + dz, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeZ)
		{
			newMeshSize.z = Fmax(oldMeshSize->z - dz, kSizeEpsilon);
		}
		else
		{
			newMeshSize.z = oldMeshSize->z;
		}

		float fx = (oldMeshSize->x > 0.0F) ? newMeshSize.x / oldMeshSize->x : 1.0F;
		float fy = (oldMeshSize->y > 0.0F) ? newMeshSize.y / oldMeshSize->y : 1.0F;
		float fz = (oldMeshSize->z > 0.0F) ? newMeshSize.z / oldMeshSize->z : 1.0F;

		if (resizeData->resizeFlags & kManipulatorResizeConstrain)
		{
			if ((fx >= 1.0F) && (fy >= 1.0F) && (fz >= 1.0F))
			{
				if ((fx >= fy) && (fx >= fz))
				{
					newMeshSize.y = oldMeshSize->y * fx;
					newMeshSize.z = oldMeshSize->z * fx;
				}
				else if (fy >= fz)
				{
					newMeshSize.x = oldMeshSize->x * fy;
					newMeshSize.z = oldMeshSize->z * fy;
				}
				else
				{
					newMeshSize.x = oldMeshSize->x * fz;
					newMeshSize.y = oldMeshSize->y * fz;
				}
			}
			else
			{
				if ((fx <= fy) && (fx <= fz))
				{
					newMeshSize.y = oldMeshSize->y * fx;
					newMeshSize.z = oldMeshSize->z * fx;
				}
				else if (fy <= fz)
				{
					newMeshSize.x = oldMeshSize->x * fy;
					newMeshSize.z = oldMeshSize->z * fy;
				}
				else
				{
					newMeshSize.x = oldMeshSize->x * fz;
					newMeshSize.y = oldMeshSize->y * fz;
				}
			}

			fx = (oldMeshSize->x > 0.0F) ? newMeshSize.x / oldMeshSize->x : 1.0F;
			fy = (oldMeshSize->y > 0.0F) ? newMeshSize.y / oldMeshSize->y : 1.0F;
			fz = (oldMeshSize->z > 0.0F) ? newMeshSize.z / oldMeshSize->z : 1.0F;
		}

		if (handleFlags & kManipulatorHandlePositiveX)
		{
			float xmin = originalBounds.min.x;
			offset.x = fx * xmin - xmin;
		}
		else
		{
			float xmax = originalBounds.max.x;
			offset.x = fx * xmax - xmax;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			float ymin = originalBounds.min.y;
			offset.y = fy * ymin - ymin;
		}
		else
		{
			float ymax = originalBounds.max.y;
			offset.y = fy * ymax - ymax;
		}

		if (handleFlags & kManipulatorHandlePositiveZ)
		{
			float zmin = originalBounds.min.z;
			offset.z = fz * zmin - zmin;
		}
		else
		{
			float zmax = originalBounds.max.z;
			offset.z = fz * zmax - zmax;
		}
	}

	object->SetObjectSize(&newMeshSize.x);

	resizeData->positionOffset = -offset;
	return (true);
}


PlateGeometryManipulator::PlateGeometryManipulator(PlateGeometry *plate) : GeometryManipulator(plate)
{
}

PlateGeometryManipulator::~PlateGeometryManipulator()
{
}

int32 PlateGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Vector2D& plateSize = GetObject()->GetPlateSize();
	float x = plateSize.x;
	float y = plateSize.y;

	handle[0].Set(0.0F, 0.0F, 0.0F);
	handle[1].Set(x * 0.5F, 0.0F, 0.0F);
	handle[2].Set(x, 0.0F, 0.0F);
	handle[3].Set(x, y * 0.5F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(x * 0.5F, y, 0.0F);
	handle[6].Set(0.0F, y, 0.0F);
	handle[7].Set(0.0F, y * 0.5F, 0.0F);

	return (8);
}

void PlateGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[8] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 4;
}

bool PlateGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	float qx = ray->origin.x;
	float qy = ray->origin.y;
	float qz = ray->origin.z;
	float vx = ray->direction.x;
	float vy = ray->direction.y;
	float vz = ray->direction.z;
	float expand = ray->radius * kRayExpand;

	const Vector2D& plateSize = GetObject()->GetPlateSize();

	if (vx > 0.0F)
	{
		float t = -qx / vx;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float y = qy + vy * t;
			float z = qz + vz * t;
			if ((y > -expand) && (y < plateSize.y + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}
	else if (vx < 0.0F)
	{
		float t = (plateSize.x - qx) / vx;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float y = qy + vy * t;
			float z = qz + vz * t;
			if ((y > -expand) && (y < plateSize.y + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	if (vy > 0.0F)
	{
		float t = -qy / vy;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float z = qz + vz * t;
			if ((x > -expand) && (x < plateSize.x + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}
	else if (vy < 0.0F)
	{
		float t = (plateSize.y - qy) / vy;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float z = qz + vz * t;
			if ((x > -expand) && (x < plateSize.x + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	return (GeometryManipulator::Pick(ray, data));
}

bool PlateGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	PlateGeometryObject *object = GetObject();
	Vector2D newPlateSize = object->GetPlateSize();
	const Vector2D *oldPlateSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= (handleFlags & kManipulatorHandleNegativeX) ? -2.0F : 2.0F;
			newPlateSize.x = Fmax(oldPlateSize->x + dx, kSizeEpsilon);
			resizeData->positionOffset.x = (oldPlateSize->x - newPlateSize.x) * 0.5F;
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= (handleFlags & kManipulatorHandleNegativeY) ? -2.0F : 2.0F;
			newPlateSize.y = Fmax(oldPlateSize->y + dy, kSizeEpsilon);
			resizeData->positionOffset.y = (oldPlateSize->y - newPlateSize.y) * 0.5F;
		}

		move = true;
	}
	else
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			newPlateSize.x = Fmax(oldPlateSize->x + dx, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeX)
		{
			newPlateSize.x = Fmax(oldPlateSize->x - dx, kSizeEpsilon);
			resizeData->positionOffset.x = oldPlateSize->x - newPlateSize.x;
			move = true;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			newPlateSize.y = Fmax(oldPlateSize->y + dy, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeY)
		{
			newPlateSize.y = Fmax(oldPlateSize->y - dy, kSizeEpsilon);
			resizeData->positionOffset.y = oldPlateSize->y - newPlateSize.y;
			move = true;
		}
	}

	object->SetPlateSize(newPlateSize);
	return (move);
}


DiskGeometryManipulator::DiskGeometryManipulator(DiskGeometry *disk) : GeometryManipulator(disk)
{
}

DiskGeometryManipulator::~DiskGeometryManipulator()
{
}

int32 DiskGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Vector2D& diskSize = GetObject()->GetDiskSize();
	float x = diskSize.x;
	float y = diskSize.y;

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	return (8);
}

void DiskGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[8] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 4;
}

bool DiskGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	float qx = ray->origin.x;
	float qy = ray->origin.y;
	float qz = ray->origin.z;
	float vx = ray->direction.x;
	float vy = ray->direction.y;
	float vz = ray->direction.z;
	float expand = ray->radius * kRayExpand;

	const DiskGeometryObject *object = GetObject();
	const Vector2D& diskSize = GetObject()->GetDiskSize();

	float r = diskSize.x + expand;
	float m = object->GetRatioXY();
	float m2 = m * m;

	float a = vx * vx + vy * vy * m2;
	float b = qx * vx + qy * vy * m2;
	float c = qx * qx + qy * qy * m2 - r * r;
	float D = b * b - a * c;

	if (D > 0.0F)
	{
		float t = -(b + Sqrt(D)) / a;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float z = qz + vz * t;
			if (Fabs(z) < expand)
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	return (GeometryManipulator::Pick(ray, data));
}

bool DiskGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	DiskGeometryObject *object = GetObject();
	Vector2D newDiskSize = object->GetDiskSize();
	const Vector2D *oldDiskSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if ((resizeData->resizeFlags & kManipulatorResizeCenter) != 0)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			if (handleFlags & kManipulatorHandleNegativeX)
			{
				dx = -dx;
			}

			newDiskSize.x = Fmax(oldDiskSize->x + dx, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			if (handleFlags & kManipulatorHandleNegativeY)
			{
				dy = -dy;
			}

			newDiskSize.y = Fmax(oldDiskSize->y + dy, kSizeEpsilon);
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeX)
			{
				newDiskSize.x = Fmax(oldDiskSize->x - dx, kSizeEpsilon);
				resizeData->positionOffset.x = oldDiskSize->x - newDiskSize.x;
			}
			else
			{
				newDiskSize.x = Fmax(oldDiskSize->x + dx, kSizeEpsilon);
				resizeData->positionOffset.x = newDiskSize.x - oldDiskSize->x;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeY)
			{
				newDiskSize.y = Fmax(oldDiskSize->y - dy, kSizeEpsilon);
				resizeData->positionOffset.y = oldDiskSize->y - newDiskSize.y;
			}
			else
			{
				newDiskSize.y = Fmax(oldDiskSize->y + dy, kSizeEpsilon);
				resizeData->positionOffset.y = newDiskSize.y - oldDiskSize->y;
			}
		}

		move = true;
	}

	object->SetDiskSize(newDiskSize);
	return (move);
}


HoleGeometryManipulator::HoleGeometryManipulator(HoleGeometry *hole) : GeometryManipulator(hole)
{
}

HoleGeometryManipulator::~HoleGeometryManipulator()
{
}

int32 HoleGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const HoleGeometryObject *object = GetObject();

	const Vector2D& outerSize = object->GetOuterSize();
	float x = outerSize.x;
	float y = outerSize.y;

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	const Vector2D& innerSize = object->GetInnerSize();
	x = innerSize.x;
	y = innerSize.y;

	handle[8].Set(-x, 0.0F, 0.0F);
	handle[9].Set(0.0F, -y, 0.0F);
	handle[10].Set(x, 0.0F, 0.0F);
	handle[11].Set(0.0F, y, 0.0F);

	return (12);
}

void HoleGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[12] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX,
		kManipulatorHandleNegativeX | kManipulatorHandleSecondary,
		kManipulatorHandleNegativeY | kManipulatorHandleSecondary,
		kManipulatorHandlePositiveX | kManipulatorHandleSecondary,
		kManipulatorHandlePositiveY | kManipulatorHandleSecondary
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 8) ? index ^ 4 : index ^ 2;
}

bool HoleGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	float qx = ray->origin.x;
	float qy = ray->origin.y;
	float qz = ray->origin.z;
	float vx = ray->direction.x;
	float vy = ray->direction.y;
	float vz = ray->direction.z;
	float expand = ray->radius * kRayExpand;

	const Vector2D& outerSize = GetObject()->GetOuterSize();
	float rxo = outerSize.x + expand;
	float ryo = outerSize.y + expand;

	if (vx > 0.0F)
	{
		float t = (-outerSize.x - qx) / vx;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float y = qy + vy * t;
			float z = qz + vz * t;
			if ((Fabs(y) < ryo) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}
	else if (vx < 0.0F)
	{
		float t = (outerSize.x - qx) / vx;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float y = qy + vy * t;
			float z = qz + vz * t;
			if ((Fabs(y) < ryo) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	if (vy > 0.0F)
	{
		float t = (-outerSize.y - qy) / vy;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float z = qz + vz * t;
			if ((Fabs(x) < rxo) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}
	else if (vy < 0.0F)
	{
		float t = (outerSize.y - qy) / vy;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float z = qz + vz * t;
			if ((Fabs(x) < rxo) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	return (GeometryManipulator::Pick(ray, data));
}

bool HoleGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	HoleGeometryObject *object = GetObject();
	Vector2D newInnerSize = object->GetInnerSize();
	Vector2D newOuterSize = object->GetOuterSize();
	const Vector2D *oldInnerSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	const Vector2D *oldOuterSize = reinterpret_cast<const Vector2D *>(GetOriginalSize() + 2);

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (!(handleFlags & kManipulatorHandleSecondary))
	{
		float tx = oldInnerSize->x / oldOuterSize->x;
		float ty = oldInnerSize->y / oldOuterSize->y;

		if (resizeData->resizeFlags & kManipulatorResizeCenter)
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				if (handleFlags & kManipulatorHandleNegativeX)
				{
					dx = -dx;
				}

				newOuterSize.x = Fmax(oldOuterSize->x + dx, kSizeEpsilon);
				newInnerSize.x = newOuterSize.x * tx;
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				if (handleFlags & kManipulatorHandleNegativeY)
				{
					dy = -dy;
				}

				newOuterSize.y = Fmax(oldOuterSize->y + dy, kSizeEpsilon);
				newInnerSize.y = newOuterSize.y * ty;
			}
		}
		else
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				dx *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeX)
				{
					newOuterSize.x = Fmax(oldOuterSize->x - dx, kSizeEpsilon);
					resizeData->positionOffset.x = oldOuterSize->x - newOuterSize.x;
				}
				else
				{
					newOuterSize.x = Fmax(oldOuterSize->x + dx, kSizeEpsilon);
					resizeData->positionOffset.x = newOuterSize.x - oldOuterSize->x;
				}

				newInnerSize.x = newOuterSize.x * tx;
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				dy *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeY)
				{
					newOuterSize.y = Fmax(oldOuterSize->y - dy, kSizeEpsilon);
					resizeData->positionOffset.y = oldOuterSize->y - newOuterSize.y;
				}
				else
				{
					newOuterSize.y = Fmax(oldOuterSize->y + dy, kSizeEpsilon);
					resizeData->positionOffset.y = newOuterSize.y - oldOuterSize->y;
				}

				newInnerSize.y = newOuterSize.y * ty;
			}

			move = true;
		}

		object->SetInnerSize(newInnerSize);
		object->SetOuterSize(newOuterSize);
		return (move);
	}

	if (handleFlags & kManipulatorHandleNonzeroX)
	{
		if (handleFlags & kManipulatorHandleNegativeX)
		{
			dx = -dx;
		}

		newInnerSize.x = Clamp(oldInnerSize->x + dx, kSizeEpsilon, oldOuterSize->x - kSizeEpsilon);
		if (resizeData->resizeFlags & kManipulatorResizeConstrain)
		{
			newInnerSize.y = Fmin(newInnerSize.x, oldOuterSize->y - kSizeEpsilon);
		}
		else
		{
			newInnerSize.y = oldInnerSize->y;
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNegativeY)
		{
			dy = -dy;
		}

		newInnerSize.y = Clamp(oldInnerSize->y + dy, kSizeEpsilon, oldOuterSize->y - kSizeEpsilon);
		if (resizeData->resizeFlags & kManipulatorResizeConstrain)
		{
			newInnerSize.x = Fmin(newInnerSize.y, oldOuterSize->x - kSizeEpsilon);
		}
		else
		{
			newInnerSize.x = oldInnerSize->x;
		}
	}

	object->SetInnerSize(newInnerSize);

	return (false);
}


AnnulusGeometryManipulator::AnnulusGeometryManipulator(AnnulusGeometry *annulus) : GeometryManipulator(annulus)
{
}

AnnulusGeometryManipulator::~AnnulusGeometryManipulator()
{
}

int32 AnnulusGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const AnnulusGeometryObject *object = GetObject();

	const Vector2D& outerSize = object->GetOuterSize();
	float x = outerSize.x;
	float y = outerSize.y;

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	const Vector2D& innerSize = object->GetInnerSize();
	x = innerSize.x;
	y = innerSize.y;

	handle[8].Set(-x, 0.0F, 0.0F);
	handle[9].Set(0.0F, -y, 0.0F);
	handle[10].Set(x, 0.0F, 0.0F);
	handle[11].Set(0.0F, y, 0.0F);

	return (12);
}

void AnnulusGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[12] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX,
		kManipulatorHandleNegativeX | kManipulatorHandleSecondary,
		kManipulatorHandleNegativeY | kManipulatorHandleSecondary,
		kManipulatorHandlePositiveX | kManipulatorHandleSecondary,
		kManipulatorHandlePositiveY | kManipulatorHandleSecondary
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 8) ? index ^ 4 : index ^ 2;
}

bool AnnulusGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	float qx = ray->origin.x;
	float qy = ray->origin.y;
	float qz = ray->origin.z;
	float vx = ray->direction.x;
	float vy = ray->direction.y;
	float vz = ray->direction.z;
	float expand = ray->radius * kRayExpand;

	const AnnulusGeometryObject *object = GetObject();
	const Vector2D& outerSize = object->GetOuterSize();

	float ro = outerSize.x + expand;
	float mo = object->GetOuterRatioXY();

	float m2 = mo * mo;
	float a = vx * vx + vy * vy * m2;
	float b = qx * vx + qy * vy * m2;
	float c = qx * qx + qy * qy * m2 - ro * ro;
	float D = b * b - a * c;

	if (D > 0.0F)
	{
		float t = -(b + Sqrt(D)) / a;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float z = qz + vz * t;
			if (Fabs(z) < expand)
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	return (GeometryManipulator::Pick(ray, data));
}

bool AnnulusGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	AnnulusGeometryObject *object = GetObject();
	Vector2D newInnerSize = object->GetInnerSize();
	Vector2D newOuterSize = object->GetOuterSize();
	const Vector2D *oldInnerSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	const Vector2D *oldOuterSize = reinterpret_cast<const Vector2D *>(GetOriginalSize() + 2);

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (!(handleFlags & kManipulatorHandleSecondary))
	{
		float tx = oldInnerSize->x / oldOuterSize->x;
		float ty = oldInnerSize->y / oldOuterSize->y;

		if (resizeData->resizeFlags & kManipulatorResizeCenter)
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				if (handleFlags & kManipulatorHandleNegativeX)
				{
					dx = -dx;
				}

				newOuterSize.x = Fmax(oldOuterSize->x + dx, kSizeEpsilon);
				newInnerSize.x = newOuterSize.x * tx;
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				if (handleFlags & kManipulatorHandleNegativeY)
				{
					dy = -dy;
				}

				newOuterSize.y = Fmax(oldOuterSize->y + dy, kSizeEpsilon);
				newInnerSize.y = newOuterSize.y * ty;
			}
		}
		else
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				dx *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeX)
				{
					newOuterSize.x = Fmax(oldOuterSize->x - dx, kSizeEpsilon);
					resizeData->positionOffset.x = oldOuterSize->x - newOuterSize.x;
				}
				else
				{
					newOuterSize.x = Fmax(oldOuterSize->x + dx, kSizeEpsilon);
					resizeData->positionOffset.x = newOuterSize.x - oldOuterSize->x;
				}

				newInnerSize.x = newOuterSize.x * tx;
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				dy *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeY)
				{
					newOuterSize.y = Fmax(oldOuterSize->y - dy, kSizeEpsilon);
					resizeData->positionOffset.y = oldOuterSize->y - newOuterSize.y;
				}
				else
				{
					newOuterSize.y = Fmax(oldOuterSize->y + dy, kSizeEpsilon);
					resizeData->positionOffset.y = newOuterSize.y - oldOuterSize->y;
				}

				newInnerSize.y = newOuterSize.y * ty;
			}

			move = true;
		}

		object->SetInnerSize(newInnerSize);
		object->SetOuterSize(newOuterSize);
		return (move);
	}

	if (handleFlags & kManipulatorHandleNonzeroX)
	{
		if (handleFlags & kManipulatorHandleNegativeX)
		{
			dx = -dx;
		}

		newInnerSize.x = Clamp(oldInnerSize->x + dx, kSizeEpsilon, oldOuterSize->x - kSizeEpsilon);
		if (resizeData->resizeFlags & kManipulatorResizeConstrain)
		{
			newInnerSize.y = Fmin(newInnerSize.x, oldOuterSize->y - kSizeEpsilon);
		}
		else
		{
			newInnerSize.y = oldInnerSize->y;
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNegativeY)
		{
			dy = -dy;
		}

		newInnerSize.y = Clamp(oldInnerSize->y + dy, kSizeEpsilon, oldOuterSize->y - kSizeEpsilon);
		if (resizeData->resizeFlags & kManipulatorResizeConstrain)
		{
			newInnerSize.x = Fmin(newInnerSize.y, oldOuterSize->x - kSizeEpsilon);
		}
		else
		{
			newInnerSize.x = oldInnerSize->x;
		}
	}

	object->SetInnerSize(newInnerSize);
	return (false);
}


BoxGeometryManipulator::BoxGeometryManipulator(BoxGeometry *box) : GeometryManipulator(box)
{
}

BoxGeometryManipulator::~BoxGeometryManipulator()
{
}

int32 BoxGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Vector3D& boxSize = GetObject()->GetBoxSize();
	float x = boxSize.x;
	float y = boxSize.y;
	float z = boxSize.z;

	handle[0].Set(0.0F, 0.0F, 0.0F);
	handle[1].Set(x * 0.5F, 0.0F, 0.0F);
	handle[2].Set(x, 0.0F, 0.0F);
	handle[3].Set(x, y * 0.5F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(x * 0.5F, y, 0.0F);
	handle[6].Set(0.0F, y, 0.0F);
	handle[7].Set(0.0F, y * 0.5F, 0.0F);

	handle[8].Set(0.0F, 0.0F, z);
	handle[9].Set(x * 0.5F, 0.0F, z);
	handle[10].Set(x, 0.0F, z);
	handle[11].Set(x, y * 0.5F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(x * 0.5F, y, z);
	handle[14].Set(0.0F, y, z);
	handle[15].Set(0.0F, y * 0.5F, z);

	z *= 0.5F;
	handle[16].Set(0.0F, 0.0F, z);
	handle[17].Set(x, 0.0F, z);
	handle[18].Set(x, y, z);
	handle[19].Set(0.0F, y, z);

	return (20);
}

void BoxGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[20] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 16) ? index ^ 12 : index ^ 2;
}

bool BoxGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	float vz = ray->direction.z;
	if (Fabs(vz) > K::min_float)
	{
		float qx = ray->origin.x;
		float qy = ray->origin.y;
		float qz = ray->origin.z;
		float vx = ray->direction.x;
		float vy = ray->direction.y;
		float expand = ray->radius * kRayExpand;

		const Vector3D& boxSize = GetObject()->GetBoxSize();

		float t = ((vz > 0.0F) ? -qz : boxSize.z - qz) / vz;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float y = qy + vy * t;
			if ((x > -expand) && (x < boxSize.x + expand) && (y > -expand) && (y < boxSize.y + expand))
			{
				if ((x < expand) || (x > boxSize.x - expand) || (y < expand) || (y > boxSize.y - expand))
				{
					data->rayParam = t;
					data->pickNormal.Set(0.0F, 0.0F, (vz > 0.0F) ? -1.0F : 1.0F);
					data->triangleIndex = kInvalidTriangleIndex;
					return (true);
				}
			}
		}
	}

	return (GeometryManipulator::Pick(ray, data));
}

bool BoxGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	BoxGeometryObject *object = GetObject();
	Vector3D newBoxSize = object->GetBoxSize();
	const Vector3D *oldBoxSize = reinterpret_cast<const Vector3D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= (handleFlags & kManipulatorHandleNegativeX) ? -2.0F : 2.0F;
			newBoxSize.x = Fmax(oldBoxSize->x + dx, kSizeEpsilon);
			resizeData->positionOffset.x = (oldBoxSize->x - newBoxSize.x) * 0.5F;
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= (handleFlags & kManipulatorHandleNegativeY) ? -2.0F : 2.0F;
			newBoxSize.y = Fmax(oldBoxSize->y + dy, kSizeEpsilon);
			resizeData->positionOffset.y = (oldBoxSize->y - newBoxSize.y) * 0.5F;
		}

		move = true;
	}
	else
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			newBoxSize.x = Fmax(oldBoxSize->x + dx, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeX)
		{
			newBoxSize.x = Fmax(oldBoxSize->x - dx, kSizeEpsilon);
			resizeData->positionOffset.x = oldBoxSize->x - newBoxSize.x;
			move = true;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			newBoxSize.y = Fmax(oldBoxSize->y + dy, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeY)
		{
			newBoxSize.y = Fmax(oldBoxSize->y - dy, kSizeEpsilon);
			resizeData->positionOffset.y = oldBoxSize->y - newBoxSize.y;
			move = true;
		}
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		newBoxSize.z = Fmax(oldBoxSize->z + dz, kSizeEpsilon);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		newBoxSize.z = Fmax(oldBoxSize->z - dz, kSizeEpsilon);
		resizeData->positionOffset.z = oldBoxSize->z - newBoxSize.z;
		move = true;
	}

	object->SetBoxSize(newBoxSize);
	return (move);
}


PyramidGeometryManipulator::PyramidGeometryManipulator(PyramidGeometry *pyramid) : GeometryManipulator(pyramid)
{
}

PyramidGeometryManipulator::~PyramidGeometryManipulator()
{
}

int32 PyramidGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const PyramidGeometryObject *object = GetObject();
	const Vector2D& pyramidSize = object->GetPyramidSize();
	float x = pyramidSize.x;
	float y = pyramidSize.y;
	float z = object->GetPyramidHeight();

	handle[0].Set(0.0F, 0.0F, 0.0F);
	handle[1].Set(x * 0.5F, 0.0F, 0.0F);
	handle[2].Set(x, 0.0F, 0.0F);
	handle[3].Set(x, y * 0.5F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(x * 0.5F, y, 0.0F);
	handle[6].Set(0.0F, y, 0.0F);
	handle[7].Set(0.0F, y * 0.5F, 0.0F);

	handle[8].Set(0.0F, 0.0F, z);
	handle[9].Set(x * 0.5F, 0.0F, z);
	handle[10].Set(x, 0.0F, z);
	handle[11].Set(x, y * 0.5F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(x * 0.5F, y, z);
	handle[14].Set(0.0F, y, z);
	handle[15].Set(0.0F, y * 0.5F, z);

	return (16);
}

void PyramidGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[16] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 12;
}

bool PyramidGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	PyramidGeometryObject *object = GetObject();
	Vector2D newPyramidSize = object->GetPyramidSize();
	float newPyramidHeight = object->GetPyramidHeight();
	const Vector2D *oldPyramidSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldPyramidHeight = GetOriginalSize()[2];

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= (handleFlags & kManipulatorHandleNegativeX) ? -2.0F : 2.0F;
			newPyramidSize.x = Fmax(oldPyramidSize->x + dx, kSizeEpsilon);
			resizeData->positionOffset.x = (oldPyramidSize->x - newPyramidSize.x) * 0.5F;
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= (handleFlags & kManipulatorHandleNegativeY) ? -2.0F : 2.0F;
			newPyramidSize.y = Fmax(oldPyramidSize->y + dy, kSizeEpsilon);
			resizeData->positionOffset.y = (oldPyramidSize->y - newPyramidSize.y) * 0.5F;
		}

		move = true;
	}
	else
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			newPyramidSize.x = Fmax(oldPyramidSize->x + dx, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeX)
		{
			newPyramidSize.x = Fmax(oldPyramidSize->x - dx, kSizeEpsilon);
			resizeData->positionOffset.x = oldPyramidSize->x - newPyramidSize.x;
			move = true;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			newPyramidSize.y = Fmax(oldPyramidSize->y + dy, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeY)
		{
			newPyramidSize.y = Fmax(oldPyramidSize->y - dy, kSizeEpsilon);
			resizeData->positionOffset.y = oldPyramidSize->y - newPyramidSize.y;
			move = true;
		}
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		newPyramidHeight = Fmax(oldPyramidHeight + dz, kSizeEpsilon);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		newPyramidHeight = Fmax(oldPyramidHeight - dz, kSizeEpsilon);
		resizeData->positionOffset.z = oldPyramidHeight - newPyramidHeight;
		move = true;
	}

	object->SetPyramidSize(newPyramidSize);
	object->SetPyramidHeight(newPyramidHeight);
	return (move);
}


CylinderGeometryManipulator::CylinderGeometryManipulator(CylinderGeometry *cylinder) : GeometryManipulator(cylinder)
{
}

CylinderGeometryManipulator::~CylinderGeometryManipulator()
{
}

int32 CylinderGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const CylinderGeometryObject *object = GetObject();
	const Vector2D& cylinderSize = object->GetCylinderSize();
	float x = cylinderSize.x;
	float y = cylinderSize.y;
	float z = object->GetCylinderHeight();

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	handle[8].Set(-x, -y, z);
	handle[9].Set(0.0F, -y, z);
	handle[10].Set(x, -y, z);
	handle[11].Set(x, 0.0F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(0.0F, y, z);
	handle[14].Set(-x, y, z);
	handle[15].Set(-x, 0.0F, z);

	return (16);
}

void CylinderGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[16] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 12;
}

bool CylinderGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	float vz = ray->direction.z;
	if (Fabs(vz) > K::min_float)
	{
		float qx = ray->origin.x;
		float qy = ray->origin.y;
		float qz = ray->origin.z;
		float vx = ray->direction.x;
		float vy = ray->direction.y;
		float expand = ray->radius * kRayExpand;

		const CylinderGeometryObject *object = GetObject();
		const Vector2D& cylinderSize = object->GetCylinderSize();

		float r = cylinderSize.x + expand;
		float m = object->GetRatioXY();
		float m2 = m * m;

		float t = ((vz > 0.0F) ? -qz : object->GetCylinderHeight() - qz) / vz;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float ri = cylinderSize.x - expand;
			float x = qx + vx * t;
			float y = qy + vy * t;
			if ((x * x + y * y * m2 < r * r) && (x * x + y * y * m2 > ri * ri))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, (vz > 0.0F) ? -1.0F : 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	return (GeometryManipulator::Pick(ray, data));
}

bool CylinderGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	CylinderGeometryObject *object = GetObject();
	Vector2D newCylinderSize = object->GetCylinderSize();
	float newCylinderHeight = object->GetCylinderHeight();
	const Vector2D *oldCylinderSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldCylinderHeight = GetOriginalSize()[2];

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			if (handleFlags & kManipulatorHandleNegativeX)
			{
				dx = -dx;
			}

			newCylinderSize.x = Fmax(oldCylinderSize->x + dx, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			if (handleFlags & kManipulatorHandleNegativeY)
			{
				dy = -dy;
			}

			newCylinderSize.y = Fmax(oldCylinderSize->y + dy, kSizeEpsilon);
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeX)
			{
				newCylinderSize.x = Fmax(oldCylinderSize->x - dx, kSizeEpsilon);
				resizeData->positionOffset.x = oldCylinderSize->x - newCylinderSize.x;
			}
			else
			{
				newCylinderSize.x = Fmax(oldCylinderSize->x + dx, kSizeEpsilon);
				resizeData->positionOffset.x = newCylinderSize.x - oldCylinderSize->x;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeY)
			{
				newCylinderSize.y = Fmax(oldCylinderSize->y - dy, kSizeEpsilon);
				resizeData->positionOffset.y = oldCylinderSize->y - newCylinderSize.y;
			}
			else
			{
				newCylinderSize.y = Fmax(oldCylinderSize->y + dy, kSizeEpsilon);
				resizeData->positionOffset.y = newCylinderSize.y - oldCylinderSize->y;
			}
		}
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		newCylinderHeight = Fmax(oldCylinderHeight + dz, kSizeEpsilon);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		newCylinderHeight = Fmax(oldCylinderHeight - dz, kSizeEpsilon);
		resizeData->positionOffset.z = oldCylinderHeight - newCylinderHeight;
	}

	object->SetCylinderSize(newCylinderSize);
	object->SetCylinderHeight(newCylinderHeight);
	return (true);
}


ConeGeometryManipulator::ConeGeometryManipulator(ConeGeometry *cone) : GeometryManipulator(cone)
{
}

ConeGeometryManipulator::~ConeGeometryManipulator()
{
}

int32 ConeGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const ConeGeometryObject *object = GetObject();
	const Vector2D& coneSize = object->GetConeSize();
	float x = coneSize.x;
	float y = coneSize.y;
	float z = object->GetConeHeight();

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	handle[8].Set(-x, -y, z);
	handle[9].Set(0.0F, -y, z);
	handle[10].Set(x, -y, z);
	handle[11].Set(x, 0.0F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(0.0F, y, z);
	handle[14].Set(-x, y, z);
	handle[15].Set(-x, 0.0F, z);

	return (16);
}

void ConeGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[16] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 12;
}

bool ConeGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	ConeGeometryObject *object = GetObject();
	Vector2D newConeSize = object->GetConeSize();
	float newConeHeight = object->GetConeHeight();
	const Vector2D *oldConeSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldConeHeight = GetOriginalSize()[2];

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			if (handleFlags & kManipulatorHandleNegativeX)
			{
				dx = -dx;
			}

			newConeSize.x = Fmax(oldConeSize->x + dx, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			if (handleFlags & kManipulatorHandleNegativeY)
			{
				dy = -dy;
			}

			newConeSize.y = Fmax(oldConeSize->y + dy, kSizeEpsilon);
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeX)
			{
				newConeSize.x = Fmax(oldConeSize->x - dx, kSizeEpsilon);
				resizeData->positionOffset.x = oldConeSize->x - newConeSize.x;
			}
			else
			{
				newConeSize.x = Fmax(oldConeSize->x + dx, kSizeEpsilon);
				resizeData->positionOffset.x = newConeSize.x - oldConeSize->x;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeY)
			{
				newConeSize.y = Fmax(oldConeSize->y - dy, kSizeEpsilon);
				resizeData->positionOffset.y = oldConeSize->y - newConeSize.y;
			}
			else
			{
				newConeSize.y = Fmax(oldConeSize->y + dy, kSizeEpsilon);
				resizeData->positionOffset.y = newConeSize.y - oldConeSize->y;
			}
		}
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		newConeHeight = Fmax(oldConeHeight + dz, kSizeEpsilon);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		newConeHeight = Fmax(oldConeHeight - dz, kSizeEpsilon);
		resizeData->positionOffset.z = oldConeHeight - newConeHeight;
	}

	object->SetConeSize(newConeSize);
	object->SetConeHeight(newConeHeight);
	return (true);
}


TruncatedConeGeometryManipulator::TruncatedConeGeometryManipulator(TruncatedConeGeometry *truncatedCone) : GeometryManipulator(truncatedCone)
{
}

TruncatedConeGeometryManipulator::~TruncatedConeGeometryManipulator()
{
}

int32 TruncatedConeGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const TruncatedConeGeometryObject *object = GetObject();
	const Vector2D& coneSize = object->GetConeSize();
	float x = coneSize.x;
	float y = coneSize.y;
	float z = object->GetConeHeight();

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	handle[8].Set(-x, -y, z);
	handle[9].Set(0.0F, -y, z);
	handle[10].Set(x, -y, z);
	handle[11].Set(x, 0.0F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(0.0F, y, z);
	handle[14].Set(-x, y, z);
	handle[15].Set(-x, 0.0F, z);

	float ratio = object->GetConeRatio();
	x *= ratio * K::sqrt_2_over_2;
	y *= ratio * K::sqrt_2_over_2;
	handle[16].Set(x, y, z);

	return (17);
}

void TruncatedConeGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[17] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ,
		0
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 16) ? index ^ 12 : kHandleOrigin;
}

bool TruncatedConeGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	TruncatedConeGeometryObject *object = GetObject();

	const Vector2D *oldConeSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;

	unsigned_int32 handleFlags = resizeData->handleFlags;
	if (handleFlags != 0)
	{
		Vector2D newConeSize = object->GetConeSize();
		float newConeHeight = object->GetConeHeight();
		float oldConeHeight = GetOriginalSize()[2];

		float dz = resizeData->resizeDelta.z;

		if (resizeData->resizeFlags & kManipulatorResizeCenter)
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				if (handleFlags & kManipulatorHandleNegativeX)
				{
					dx = -dx;
				}

				newConeSize.x = Fmax(oldConeSize->x + dx, kSizeEpsilon);
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				if (handleFlags & kManipulatorHandleNegativeY)
				{
					dy = -dy;
				}

				newConeSize.y = Fmax(oldConeSize->y + dy, kSizeEpsilon);
			}
		}
		else
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				dx *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeX)
				{
					newConeSize.x = Fmax(oldConeSize->x - dx, kSizeEpsilon);
					resizeData->positionOffset.x = oldConeSize->x - newConeSize.x;
				}
				else
				{
					newConeSize.x = Fmax(oldConeSize->x + dx, kSizeEpsilon);
					resizeData->positionOffset.x = newConeSize.x - oldConeSize->x;
				}
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				dy *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeY)
				{
					newConeSize.y = Fmax(oldConeSize->y - dy, kSizeEpsilon);
					resizeData->positionOffset.y = oldConeSize->y - newConeSize.y;
				}
				else
				{
					newConeSize.y = Fmax(oldConeSize->y + dy, kSizeEpsilon);
					resizeData->positionOffset.y = newConeSize.y - oldConeSize->y;
				}
			}
		}

		if (handleFlags & kManipulatorHandlePositiveZ)
		{
			newConeHeight = Fmax(oldConeHeight + dz, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeZ)
		{
			newConeHeight = Fmax(oldConeHeight - dz, kSizeEpsilon);
			resizeData->positionOffset.z = oldConeHeight - newConeHeight;
		}

		object->SetConeSize(newConeSize);
		object->SetConeHeight(newConeHeight);
		return (true);
	}

	float dr = (dx + dy) * InverseMag(*oldConeSize);
	object->SetConeRatio(Clamp(GetOriginalSize()[3] + dr, kSizeEpsilon, 1.0F - kSizeEpsilon));
	return (false);
}


SphereGeometryManipulator::SphereGeometryManipulator(SphereGeometry *sphere) : GeometryManipulator(sphere)
{
}

SphereGeometryManipulator::~SphereGeometryManipulator()
{
}

int32 SphereGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Vector3D& sphereSize = GetObject()->GetSphereSize();
	float x = sphereSize.x;
	float y = sphereSize.y;
	float z = sphereSize.z;

	handle[0].Set(-x, -y, -z);
	handle[1].Set(0.0F, -y, -z);
	handle[2].Set(x, -y, -z);
	handle[3].Set(x, 0.0F, -z);
	handle[4].Set(x, y, -z);
	handle[5].Set(0.0F, y, -z);
	handle[6].Set(-x, y, -z);
	handle[7].Set(-x, 0.0F, -z);

	handle[8].Set(-x, -y, z);
	handle[9].Set(0.0F, -y, z);
	handle[10].Set(x, -y, z);
	handle[11].Set(x, 0.0F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(0.0F, y, z);
	handle[14].Set(-x, y, z);
	handle[15].Set(-x, 0.0F, z);

	return (16);
}

void SphereGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[16] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 12;
}

bool SphereGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	SphereGeometryObject *object = GetObject();
	Vector3D newSphereSize = object->GetSphereSize();
	const Vector3D *oldSphereSize = reinterpret_cast<const Vector3D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			if (handleFlags & kManipulatorHandleNegativeX)
			{
				dx = -dx;
			}

			newSphereSize.x = Fmax(oldSphereSize->x + dx, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			if (handleFlags & kManipulatorHandleNegativeY)
			{
				dy = -dy;
			}

			newSphereSize.y = Fmax(oldSphereSize->y + dy, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroZ)
		{
			if (handleFlags & kManipulatorHandleNegativeZ)
			{
				dz = -dz;
			}

			newSphereSize.z = Fmax(oldSphereSize->z + dz, kSizeEpsilon);
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeX)
			{
				newSphereSize.x = Fmax(oldSphereSize->x - dx, kSizeEpsilon);
				resizeData->positionOffset.x = oldSphereSize->x - newSphereSize.x;
			}
			else
			{
				newSphereSize.x = Fmax(oldSphereSize->x + dx, kSizeEpsilon);
				resizeData->positionOffset.x = newSphereSize.x - oldSphereSize->x;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeY)
			{
				newSphereSize.y = Fmax(oldSphereSize->y - dy, kSizeEpsilon);
				resizeData->positionOffset.y = oldSphereSize->y - newSphereSize.y;
			}
			else
			{
				newSphereSize.y = Fmax(oldSphereSize->y + dy, kSizeEpsilon);
				resizeData->positionOffset.y = newSphereSize.y - oldSphereSize->y;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroZ)
		{
			dz *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeZ)
			{
				newSphereSize.z = Fmax(oldSphereSize->z - dz, kSizeEpsilon);
				resizeData->positionOffset.z = oldSphereSize->z - newSphereSize.z;
			}
			else
			{
				newSphereSize.z = Fmax(oldSphereSize->z + dz, kSizeEpsilon);
				resizeData->positionOffset.z = newSphereSize.z - oldSphereSize->z;
			}
		}
	}

	object->SetSphereSize(newSphereSize);
	return (true);
}


DomeGeometryManipulator::DomeGeometryManipulator(DomeGeometry *dome) : GeometryManipulator(dome)
{
}

DomeGeometryManipulator::~DomeGeometryManipulator()
{
}

int32 DomeGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Vector3D& domeSize = GetObject()->GetDomeSize();
	float x = domeSize.x;
	float y = domeSize.y;
	float z = domeSize.z;

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	handle[8].Set(-x, -y, z);
	handle[9].Set(0.0F, -y, z);
	handle[10].Set(x, -y, z);
	handle[11].Set(x, 0.0F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(0.0F, y, z);
	handle[14].Set(-x, y, z);
	handle[15].Set(-x, 0.0F, z);

	return (16);
}

void DomeGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[16] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 12;
}

bool DomeGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	DomeGeometryObject *object = GetObject();
	Vector3D newDomeSize = object->GetDomeSize();
	const Vector3D *oldDomeSize = reinterpret_cast<const Vector3D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			if (handleFlags & kManipulatorHandleNegativeX)
			{
				dx = -dx;
			}

			newDomeSize.x = Fmax(oldDomeSize->x + dx, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			if (handleFlags & kManipulatorHandleNegativeY)
			{
				dy = -dy;
			}

			newDomeSize.y = Fmax(oldDomeSize->y + dy, kSizeEpsilon);
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeX)
			{
				newDomeSize.x = Fmax(oldDomeSize->x - dx, kSizeEpsilon);
				resizeData->positionOffset.x = oldDomeSize->x - newDomeSize.x;
			}
			else
			{
				newDomeSize.x = Fmax(oldDomeSize->x + dx, kSizeEpsilon);
				resizeData->positionOffset.x = newDomeSize.x - oldDomeSize->x;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeY)
			{
				newDomeSize.y = Fmax(oldDomeSize->y - dy, kSizeEpsilon);
				resizeData->positionOffset.y = oldDomeSize->y - newDomeSize.y;
			}
			else
			{
				newDomeSize.y = Fmax(oldDomeSize->y + dy, kSizeEpsilon);
				resizeData->positionOffset.y = newDomeSize.y - oldDomeSize->y;
			}
		}
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		newDomeSize.z = Fmax(oldDomeSize->z + dz, kSizeEpsilon);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		newDomeSize.z = Fmax(oldDomeSize->z - dz, kSizeEpsilon);
		resizeData->positionOffset.z = oldDomeSize->z - newDomeSize.z;
	}

	object->SetDomeSize(newDomeSize);
	return (true);
}


TorusGeometryManipulator::TorusGeometryManipulator(TorusGeometry *torus) : GeometryManipulator(torus)
{
}

TorusGeometryManipulator::~TorusGeometryManipulator()
{
}

int32 TorusGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const TorusGeometryObject *object = GetObject();

	const Vector2D& primarySize = object->GetPrimarySize();
	float secondaryRadius = object->GetSecondaryRadius();

	float x = primarySize.x + secondaryRadius;
	float y = primarySize.y + secondaryRadius;

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	x = primarySize.x - secondaryRadius;
	y = primarySize.y - secondaryRadius;

	handle[8].Set(-x, 0.0F, 0.0F);
	handle[9].Set(0.0F, -y, 0.0F);
	handle[10].Set(x, 0.0F, 0.0F);
	handle[11].Set(0.0F, y, 0.0F);

	return (12);
}

void TorusGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[12] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX,
		kManipulatorHandleNegativeX | kManipulatorHandleSecondary,
		kManipulatorHandleNegativeY | kManipulatorHandleSecondary,
		kManipulatorHandlePositiveX | kManipulatorHandleSecondary,
		kManipulatorHandlePositiveY | kManipulatorHandleSecondary
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 8) ? index ^ 4 : index ^ 2;
}

bool TorusGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	TorusGeometryObject *object = GetObject();
	Vector2D newPrimarySize = object->GetPrimarySize();
	const Vector2D *oldPrimarySize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldSecondaryRadius = GetOriginalSize()[2];

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (!(handleFlags & kManipulatorHandleSecondary))
	{
		float minPrimaryRadius = oldSecondaryRadius + kSizeEpsilon;

		if (resizeData->resizeFlags & kManipulatorResizeCenter)
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				if (handleFlags & kManipulatorHandleNegativeX)
				{
					dx = -dx;
				}

				newPrimarySize.x = Fmax(oldPrimarySize->x + dx, minPrimaryRadius);
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				if (handleFlags & kManipulatorHandleNegativeY)
				{
					dy = -dy;
				}

				newPrimarySize.y = Fmax(oldPrimarySize->y + dy, minPrimaryRadius);
			}
		}
		else
		{
			if (handleFlags & kManipulatorHandleNonzeroX)
			{
				dx *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeX)
				{
					newPrimarySize.x = Fmax(oldPrimarySize->x - dx, minPrimaryRadius);
					resizeData->positionOffset.x = oldPrimarySize->x - newPrimarySize.x;
				}
				else
				{
					newPrimarySize.x = Fmax(oldPrimarySize->x + dx, minPrimaryRadius);
					resizeData->positionOffset.x = newPrimarySize.x - oldPrimarySize->x;
				}
			}

			if (handleFlags & kManipulatorHandleNonzeroY)
			{
				dy *= 0.5F;

				if (handleFlags & kManipulatorHandleNegativeY)
				{
					newPrimarySize.y = Fmax(oldPrimarySize->y - dy, minPrimaryRadius);
					resizeData->positionOffset.y = oldPrimarySize->y - newPrimarySize.y;
				}
				else
				{
					newPrimarySize.y = Fmax(oldPrimarySize->y + dy, minPrimaryRadius);
					resizeData->positionOffset.y = newPrimarySize.y - oldPrimarySize->y;
				}
			}

			move = true;
		}

		object->SetPrimarySize(newPrimarySize);
		return (move);
	}

	float maxSecondaryRadius = Fmin(oldPrimarySize->x, oldPrimarySize->y) - kSizeEpsilon;
	if (handleFlags & kManipulatorHandleNonzeroX)
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			dx = -dx;
		}

		object->SetSecondaryRadius(Clamp(oldSecondaryRadius + dx, kSizeEpsilon, maxSecondaryRadius));
	}
	else
	{
		if (handleFlags & kManipulatorHandlePositiveY)
		{
			dy = -dy;
		}

		object->SetSecondaryRadius(Clamp(oldSecondaryRadius + dy, kSizeEpsilon, maxSecondaryRadius));
	}

	return (false);
}


TubeGeometryManipulator::TubeGeometryManipulator(TubeGeometry *tube) : GeometryManipulator(tube)
{
}

TubeGeometryManipulator::~TubeGeometryManipulator()
{
}

int32 TubeGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Vector2D& tubeSize = GetObject()->GetTubeSize();
	float x = tubeSize.x;
	float y = tubeSize.y;

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	return (8);
}

void TubeGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[8] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 4;
}

bool TubeGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	TubeGeometryObject *object = GetObject();
	Vector2D newTubeSize = object->GetTubeSize();
	const Vector2D *oldTubeSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if ((resizeData->resizeFlags & kManipulatorResizeCenter) != 0)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			if (handleFlags & kManipulatorHandleNegativeX)
			{
				dx = -dx;
			}

			newTubeSize.x = Fmax(oldTubeSize->x + dx, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			if (handleFlags & kManipulatorHandleNegativeY)
			{
				dy = -dy;
			}

			newTubeSize.y = Fmax(oldTubeSize->y + dy, kSizeEpsilon);
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeX)
			{
				newTubeSize.x = Fmax(oldTubeSize->x - dx, kSizeEpsilon);
				resizeData->positionOffset.x = oldTubeSize->x - newTubeSize.x;
			}
			else
			{
				newTubeSize.x = Fmax(oldTubeSize->x + dx, kSizeEpsilon);
				resizeData->positionOffset.x = newTubeSize.x - oldTubeSize->x;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeY)
			{
				newTubeSize.y = Fmax(oldTubeSize->y - dy, kSizeEpsilon);
				resizeData->positionOffset.y = oldTubeSize->y - newTubeSize.y;
			}
			else
			{
				newTubeSize.y = Fmax(oldTubeSize->y + dy, kSizeEpsilon);
				resizeData->positionOffset.y = newTubeSize.y - oldTubeSize->y;
			}
		}

		move = true;
	}

	object->SetTubeSize(newTubeSize);
	return (move);
}


ExtrusionGeometryManipulator::ExtrusionGeometryManipulator(ExtrusionGeometry *extrusion) : GeometryManipulator(extrusion)
{
}

ExtrusionGeometryManipulator::~ExtrusionGeometryManipulator()
{
}

int32 ExtrusionGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const ExtrusionGeometryObject *object = GetObject();
	const Vector2D& extrusionSize = object->GetExtrusionSize();
	float x = extrusionSize.x;
	float y = extrusionSize.y;
	float z = object->GetExtrusionHeight();

	handle[0].Set(0.0F, 0.0F, 0.0F);
	handle[1].Set(x * 0.5F, 0.0F, 0.0F);
	handle[2].Set(x, 0.0F, 0.0F);
	handle[3].Set(x, y * 0.5F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(x * 0.5F, y, 0.0F);
	handle[6].Set(0.0F, y, 0.0F);
	handle[7].Set(0.0F, y * 0.5F, 0.0F);

	handle[8].Set(0.0F, 0.0F, z);
	handle[9].Set(x * 0.5F, 0.0F, z);
	handle[10].Set(x, 0.0F, z);
	handle[11].Set(x, y * 0.5F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(x * 0.5F, y, z);
	handle[14].Set(0.0F, y, z);
	handle[15].Set(0.0F, y * 0.5F, z);

	z *= 0.5F;
	handle[16].Set(0.0F, 0.0F, z);
	handle[17].Set(x, 0.0F, z);
	handle[18].Set(x, y, z);
	handle[19].Set(0.0F, y, z);

	return (20);
}

void ExtrusionGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[20] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 16) ? index ^ 12 : index ^ 2;
}

bool ExtrusionGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	ExtrusionGeometryObject *object = GetObject();
	Vector2D newExtrusionSize = object->GetExtrusionSize();
	float newExtrusionHeight = object->GetExtrusionHeight();
	const Vector2D *oldExtrusionSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldExtrusionHeight = GetOriginalSize()[2];

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= (handleFlags & kManipulatorHandleNegativeX) ? -2.0F : 2.0F;
			newExtrusionSize.x = Fmax(oldExtrusionSize->x + dx, kSizeEpsilon);
			resizeData->positionOffset.x = (oldExtrusionSize->x - newExtrusionSize.x) * 0.5F;
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= (handleFlags & kManipulatorHandleNegativeY) ? -2.0F : 2.0F;
			newExtrusionSize.y = Fmax(oldExtrusionSize->y + dy, kSizeEpsilon);
			resizeData->positionOffset.y = (oldExtrusionSize->y - newExtrusionSize.y) * 0.5F;
		}

		move = true;
	}
	else
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			newExtrusionSize.x = Fmax(oldExtrusionSize->x + dx, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeX)
		{
			newExtrusionSize.x = Fmax(oldExtrusionSize->x - dx, kSizeEpsilon);
			resizeData->positionOffset.x = oldExtrusionSize->x - newExtrusionSize.x;
			move = true;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			newExtrusionSize.y = Fmax(oldExtrusionSize->y + dy, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeY)
		{
			newExtrusionSize.y = Fmax(oldExtrusionSize->y - dy, kSizeEpsilon);
			resizeData->positionOffset.y = oldExtrusionSize->y - newExtrusionSize.y;
			move = true;
		}
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		newExtrusionHeight = Fmax(oldExtrusionHeight + dz, kSizeEpsilon);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		newExtrusionHeight = Fmax(oldExtrusionHeight - dz, kSizeEpsilon);
		resizeData->positionOffset.z = oldExtrusionHeight - newExtrusionHeight;
		move = true;
	}

	object->SetExtrusionSize(newExtrusionSize);
	object->SetExtrusionHeight(newExtrusionHeight);
	return (move);
}


RevolutionGeometryManipulator::RevolutionGeometryManipulator(RevolutionGeometry *revolution) : GeometryManipulator(revolution)
{
}

RevolutionGeometryManipulator::~RevolutionGeometryManipulator()
{
}

int32 RevolutionGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const RevolutionGeometryObject *object = GetObject();
	const Vector2D& revolutionSize = object->GetRevolutionSize();
	float x = revolutionSize.x;
	float y = revolutionSize.y;
	float z = object->GetRevolutionHeight();

	handle[0].Set(-x, -y, 0.0F);
	handle[1].Set(0.0F, -y, 0.0F);
	handle[2].Set(x, -y, 0.0F);
	handle[3].Set(x, 0.0F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(0.0F, y, 0.0F);
	handle[6].Set(-x, y, 0.0F);
	handle[7].Set(-x, 0.0F, 0.0F);

	handle[8].Set(-x, -y, z);
	handle[9].Set(0.0F, -y, z);
	handle[10].Set(x, -y, z);
	handle[11].Set(x, 0.0F, z);
	handle[12].Set(x, y, z);
	handle[13].Set(0.0F, y, z);
	handle[14].Set(-x, y, z);
	handle[15].Set(-x, 0.0F, z);

	return (16);
}

void RevolutionGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[16] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeZ,
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveZ
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 12;
}

bool RevolutionGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	RevolutionGeometryObject *object = GetObject();
	Vector2D newRevolutionSize = object->GetRevolutionSize();
	float newRevolutionHeight = object->GetRevolutionHeight();
	const Vector2D *oldRevolutionSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());
	float oldRevolutionHeight = GetOriginalSize()[2];

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	float dz = resizeData->resizeDelta.z;

	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			if (handleFlags & kManipulatorHandleNegativeX)
			{
				dx = -dx;
			}

			newRevolutionSize.x = Fmax(oldRevolutionSize->x + dx, kSizeEpsilon);
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			if (handleFlags & kManipulatorHandleNegativeY)
			{
				dy = -dy;
			}

			newRevolutionSize.y = Fmax(oldRevolutionSize->y + dy, kSizeEpsilon);
		}
	}
	else
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeX)
			{
				newRevolutionSize.x = Fmax(oldRevolutionSize->x - dx, kSizeEpsilon);
				resizeData->positionOffset.x = oldRevolutionSize->x - newRevolutionSize.x;
			}
			else
			{
				newRevolutionSize.x = Fmax(oldRevolutionSize->x + dx, kSizeEpsilon);
				resizeData->positionOffset.x = newRevolutionSize.x - oldRevolutionSize->x;
			}
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= 0.5F;

			if (handleFlags & kManipulatorHandleNegativeY)
			{
				newRevolutionSize.y = Fmax(oldRevolutionSize->y - dy, kSizeEpsilon);
				resizeData->positionOffset.y = oldRevolutionSize->y - newRevolutionSize.y;
			}
			else
			{
				newRevolutionSize.y = Fmax(oldRevolutionSize->y + dy, kSizeEpsilon);
				resizeData->positionOffset.y = newRevolutionSize.y - oldRevolutionSize->y;
			}
		}
	}

	if (handleFlags & kManipulatorHandlePositiveZ)
	{
		newRevolutionHeight = Fmax(oldRevolutionHeight + dz, kSizeEpsilon);
	}
	else if (handleFlags & kManipulatorHandleNegativeZ)
	{
		newRevolutionHeight = Fmax(oldRevolutionHeight - dz, kSizeEpsilon);
		resizeData->positionOffset.z = oldRevolutionHeight - newRevolutionHeight;
	}

	object->SetRevolutionSize(newRevolutionSize);
	object->SetRevolutionHeight(newRevolutionHeight);
	return (true);
}


RopeGeometryManipulator::RopeGeometryManipulator(RopeGeometry *rope) : GeometryManipulator(rope)
{
	SetManipulatorFlags(kManipulatorLockedController);
}

RopeGeometryManipulator::~RopeGeometryManipulator()
{
}

int32 RopeGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const RopeGeometryObject *object = GetObject();
	float ropeLength = object->GetRopeLength();
	float ropeRadius = object->GetRopeRadius();

	handle[0].Set(0.0F, 0.0F, 0.0F);
	handle[1].Set(ropeLength, 0.0F, 0.0F);
	handle[2].Set(0.0F, ropeRadius, 0.0F);

	return (3);
}

void RopeGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[3] =
	{
		kManipulatorHandleNegativeX,
		kManipulatorHandlePositiveX,
		0
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = (index < 2) ? index ^ 1 : index;
}

bool RopeGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	RopeGeometryObject *object = GetObject();

	float newRopeLength = object->GetRopeLength();
	float newRopeRadius = object->GetRopeRadius();
	float oldRopeLength = GetOriginalSize()[0];
	float oldRopeRadius = GetOriginalSize()[1];

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (handleFlags & kManipulatorHandleNonzeroX)
	{
		float dx = resizeData->resizeDelta.x;

		if (resizeData->resizeFlags & kManipulatorResizeCenter)
		{
			dx *= (handleFlags & kManipulatorHandleNegativeX) ? -2.0F : 2.0F;
			newRopeLength = Fmax(oldRopeLength + dx, kSizeEpsilon);
			resizeData->positionOffset.x = (oldRopeLength - newRopeLength) * 0.5F;
			move = true;
		}
		else if (handleFlags & kManipulatorHandlePositiveX)
		{
			newRopeLength = Fmax(oldRopeLength + dx, kSizeEpsilon);
		}
		else
		{
			newRopeLength = Fmax(oldRopeLength - dx, kSizeEpsilon);
			resizeData->positionOffset.x = oldRopeLength - newRopeLength;
			move = true;
		}
	}
	else
	{
		float dy = resizeData->resizeDelta.y;
		newRopeRadius = Fmax(oldRopeRadius + dy, kSizeEpsilon);
	}

	object->SetRopeLength(newRopeLength);
	object->SetRopeRadius(newRopeRadius);
	return (move);
}


ClothGeometryManipulator::ClothGeometryManipulator(ClothGeometry *cloth) : GeometryManipulator(cloth)
{
	SetManipulatorFlags(kManipulatorLockedController);
}

ClothGeometryManipulator::~ClothGeometryManipulator()
{
}

int32 ClothGeometryManipulator::GetHandleTable(Point3D *handle) const
{
	const Vector2D& clothSize = GetObject()->GetClothSize();
	float x = clothSize.x;
	float y = clothSize.y;

	handle[0].Set(0.0F, 0.0F, 0.0F);
	handle[1].Set(x * 0.5F, 0.0F, 0.0F);
	handle[2].Set(x, 0.0F, 0.0F);
	handle[3].Set(x, y * 0.5F, 0.0F);
	handle[4].Set(x, y, 0.0F);
	handle[5].Set(x * 0.5F, y, 0.0F);
	handle[6].Set(0.0F, y, 0.0F);
	handle[7].Set(0.0F, y * 0.5F, 0.0F);

	return (8);
}

void ClothGeometryManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	static const unsigned_int32 handleFlags[8] =
	{
		kManipulatorHandleNegativeX | kManipulatorHandleNegativeY,
		kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX | kManipulatorHandleNegativeY,
		kManipulatorHandlePositiveX,
		kManipulatorHandlePositiveX | kManipulatorHandlePositiveY,
		kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX | kManipulatorHandlePositiveY,
		kManipulatorHandleNegativeX
	};

	handleData->handleFlags = handleFlags[index];
	handleData->oppositeIndex = index ^ 4;
}

bool ClothGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	float qx = ray->origin.x;
	float qy = ray->origin.y;
	float qz = ray->origin.z;
	float vx = ray->direction.x;
	float vy = ray->direction.y;
	float vz = ray->direction.z;
	float expand = ray->radius * kRayExpand;

	const ClothGeometryObject *object = GetObject();
	const Vector2D& clothSize = object->GetClothSize();

	if (Fabs(vz) > K::min_float)
	{
		float t = -qz / vz;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float y = qy + vy * t;
			if ((x > -expand) && (x < clothSize.x + expand) && (y > -expand) && (y < clothSize.y + expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);

				const Mesh *mesh = object->GetGeometryLevel(0);
				const Triangle *triangle = mesh->GetArray<Triangle>(kArrayPrimitive);

				bool b1 = (vz < 0.0F);
				bool b2 = (triangle->index[0] < mesh->GetVertexCount() / 2);

				data->triangleIndex = (b1 ^ b2) ? mesh->GetPrimitiveCount() / 2 : 0;
				return (true);
			}
		}
	}

	if (vx > 0.0F)
	{
		float t = -qx / vx;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float y = qy + vy * t;
			float z = qz + vz * t;
			if ((y > -expand) && (y < clothSize.y + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}
	else if (vx < 0.0F)
	{
		float t = (clothSize.x - qx) / vx;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float y = qy + vy * t;
			float z = qz + vz * t;
			if ((y > -expand) && (y < clothSize.y + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	if (vy > 0.0F)
	{
		float t = -qy / vy;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float z = qz + vz * t;
			if ((x > -expand) && (x < clothSize.x + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}
	else if (vy < 0.0F)
	{
		float t = (clothSize.y - qy) / vy;
		if ((t > ray->tmin) && (t < ray->tmax))
		{
			float x = qx + vx * t;
			float z = qz + vz * t;
			if ((x > -expand) && (x < clothSize.x + expand) && (Fabs(z) < expand))
			{
				data->rayParam = t;
				data->pickNormal.Set(0.0F, 0.0F, 1.0F);
				data->triangleIndex = kInvalidTriangleIndex;
				return (true);
			}
		}
	}

	return (false);
}

bool ClothGeometryManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	ClothGeometryObject *object = GetObject();
	Vector2D newClothSize = object->GetClothSize();
	const Vector2D *oldClothSize = reinterpret_cast<const Vector2D *>(GetOriginalSize());

	float dx = resizeData->resizeDelta.x;
	float dy = resizeData->resizeDelta.y;
	if (resizeData->resizeFlags & kManipulatorResizeConstrain)
	{
		if (Fabs(dx) > Fabs(dy))
		{
			dy = dx * oldClothSize->y / oldClothSize->x;
		}
		else
		{
			dx = dy * oldClothSize->x / oldClothSize->y;
		}
	}

	bool move = false;
	unsigned_int32 handleFlags = resizeData->handleFlags;

	if (resizeData->resizeFlags & kManipulatorResizeCenter)
	{
		if (handleFlags & kManipulatorHandleNonzeroX)
		{
			dx *= (handleFlags & kManipulatorHandleNegativeX) ? -2.0F : 2.0F;
			newClothSize.x = Fmax(oldClothSize->x + dx, kSizeEpsilon);
			resizeData->positionOffset.x = (oldClothSize->x - newClothSize.x) * 0.5F;
		}

		if (handleFlags & kManipulatorHandleNonzeroY)
		{
			dy *= (handleFlags & kManipulatorHandleNegativeY) ? -2.0F : 2.0F;
			newClothSize.y = Fmax(oldClothSize->y + dy, kSizeEpsilon);
			resizeData->positionOffset.y = (oldClothSize->y - newClothSize.y) * 0.5F;
		}

		move = true;
	}
	else
	{
		if (handleFlags & kManipulatorHandlePositiveX)
		{
			newClothSize.x = Fmax(oldClothSize->x + dx, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeX)
		{
			newClothSize.x = Fmax(oldClothSize->x - dx, kSizeEpsilon);
			resizeData->positionOffset.x = oldClothSize->x - newClothSize.x;
			move = true;
		}

		if (handleFlags & kManipulatorHandlePositiveY)
		{
			newClothSize.y = Fmax(oldClothSize->y + dy, kSizeEpsilon);
		}
		else if (handleFlags & kManipulatorHandleNegativeY)
		{
			newClothSize.y = Fmax(oldClothSize->y - dy, kSizeEpsilon);
			resizeData->positionOffset.y = oldClothSize->y - newClothSize.y;
			move = true;
		}
	}

	object->SetClothSize(newClothSize);
	return (move);
}

// ZYUQURM
