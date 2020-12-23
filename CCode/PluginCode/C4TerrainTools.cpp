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


#include "C4TerrainTools.h"
#include "C4TerrainBuilders.h"
#include "C4WorldEditor.h"
#include "C4EditorSupport.h"
#include "C4Shaders.h"
#include "C4World.h"


using namespace C4;


namespace
{
	enum
	{
		kWidgetPartTexture			= 'TEX0'
	};


	enum
	{
		kTerrainPaintUndoModified	= 1 << 0,
		kTerrainPaintUndoExisted	= 1 << 1
	};


	enum
	{
		kTerrainUpdateForceDelete	= 1 << 0
	};


	const ConstColorRGBA kBlockSelectedColor = {1.0F, 0.5F, 0.25F, 1.0F};
	const ConstColorRGBA kBlockUnselectedColor = {0.25F, 0.125F, 0.0625F, 1.0F};
}


const float TerrainPage::slopeTerm[kEditorTerrainSlopeCount][4] =
{
	{0.0F, 0.0F, -1.0F, 1.0F},
	{2.0F, -3.0F, 0.0F, 1.0F},
	{-1.0F, 0.0F, 0.0F, 1.0F},
	{-1.0F, 3.0F, -3.0F, 1.0F}
};


TerrainBlockManipulator::TerrainBlockManipulator(TerrainBlock *block) :
		EditorManipulator(block, "WorldEditor/node/TerrainBlock"),
		blockDiffuseColor(kBlockUnselectedColor, kAttributeMutable),
		blockRenderable(kRenderIndexedTriangles, kRenderDepthTest),
		darkDiffuseColor(ColorRGBA(0.0F, 0.0F, 0.0F, 0.5F)),
		sphereRenderable(kRenderIndexedTriangles, kRenderDepthInhibit),
		cylinderRenderable(kRenderIndexedTriangles, kRenderDepthInhibit)
{
	SetManipulatorFlags(kManipulatorLockedController);

	blockRenderable.SetShaderFlags(kShaderAmbientEffect | kShaderScaleVertex | kShaderOffsetVertex);
	blockRenderable.SetRenderParameterPointer(&blockSizeVector);
	blockRenderable.SetTransformable(block);

	blockRenderable.SetVertexCount(kManipulatorBoxVertexCount);
	blockRenderable.SetPrimitiveCount(kManipulatorBoxTriangleCount);
	blockRenderable.SetVertexBuffer(kVertexBufferAttributeArray, RetainBoxVertexBuffer(), sizeof(BoxVertex));
	blockRenderable.SetVertexBuffer(kVertexBufferIndexArray, RetainBoxIndexBuffer());
	blockRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	blockRenderable.SetVertexAttributeArray(kArrayOffset, sizeof(Point3D), 3);

	blockAttributeList.Append(&blockDiffuseColor);
	blockRenderable.SetMaterialAttributeList(&blockAttributeList);

	renderTool = -1;
	renderBrush = -1;

	MaterialObject *materialObject = new MaterialObject;
	brushDiffuseColor = new DiffuseAttribute(kAttributeMutable);
	materialObject->AddAttribute(brushDiffuseColor);

	sphereBrush = new SphereGeometry(Vector3D(1.0F, 1.0F, 1.0F));
	sphereBrush->SetManipulator(new Manipulator(sphereBrush));
	sphereBrush->SetMaterialObject(0, materialObject);

	SphereGeometryObject *sphereObject = sphereBrush->GetObject();
	sphereObject->SetCollisionExclusionMask(kCollisionExcludeAll);
	sphereObject->SetGeometryFlags((sphereObject->GetGeometryFlags() | (kGeometryMotionBlurInhibit | kGeometryRenderEffectPass)) & ~kGeometryCastShadows);
	sphereObject->SetGeometryEffectFlags(kGeometryEffectAccumulate);
	sphereObject->SetBuildLevelCount(1);
	sphereObject->SetMaxSubdiv(16, 8);

	cylinderBrush = new CylinderGeometry(Vector2D(1.0F, 1.0F), 2.0F);
	cylinderBrush->SetManipulator(new Manipulator(cylinderBrush));
	cylinderBrush->SetMaterialObject(0, materialObject);

	CylinderGeometryObject *cylinderObject = cylinderBrush->GetObject();
	cylinderObject->SetCollisionExclusionMask(kCollisionExcludeAll);
	cylinderObject->SetGeometryFlags((cylinderObject->GetGeometryFlags() | (kGeometryMotionBlurInhibit | kGeometryRenderEffectPass)) & ~kGeometryCastShadows);
	cylinderObject->SetGeometryEffectFlags(kGeometryEffectAccumulate);
	cylinderObject->SetBuildLevelCount(1);
	cylinderObject->SetMaxSubdiv(16, 1);

	materialObject->Release();
 
	darkAttributeList.Append(&darkDiffuseColor);

	sphereRenderable.SetMaterialAttributeList(&darkAttributeList); 
	sphereRenderable.SetShaderFlags(kShaderAmbientEffect);
	sphereRenderable.SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve); 
	sphereRenderable.SetTransformable(sphereBrush);

	cylinderRenderable.SetMaterialAttributeList(&darkAttributeList); 
	cylinderRenderable.SetShaderFlags(kShaderAmbientEffect);
	cylinderRenderable.SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve); 
	cylinderRenderable.SetTransformable(cylinderBrush); 
}

TerrainBlockManipulator::~TerrainBlockManipulator()
{ 
	delete cylinderBrush;
	delete sphereBrush;

	ReleaseBoxVertexBuffer();
	ReleaseBoxIndexBuffer();
}

const char *TerrainBlockManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeTerrainBlock)));
}

int32 TerrainBlockManipulator::GetHandleTable(Point3D *handle) const
{
	handle[0] = Zero3D + GetTargetNode()->GetBlockBoxSize();
	return (1);
}

void TerrainBlockManipulator::GetHandleData(int32 index, ManipulatorHandleData *handleData) const
{
	handleData->handleFlags = kManipulatorHandlePositiveX | kManipulatorHandlePositiveY | kManipulatorHandlePositiveZ;
	handleData->oppositeIndex = kHandleOrigin;
}

void TerrainBlockManipulator::CloneBrushRenderable(const Renderable *brush, Renderable *renderable)
{
	renderable->SetVertexCount(brush->GetVertexCount());
	renderable->SetVertexBufferArrayFlags(brush->GetVertexBufferArrayFlags());
	renderable->SetVertexBuffer(kVertexBufferAttributeArray0, brush->GetVertexBuffer(kVertexBufferAttributeArray0), brush->GetVertexBufferStride(kVertexBufferAttributeArray0));
	renderable->SetVertexBuffer(kVertexBufferIndexArray, brush->GetVertexBuffer(kVertexBufferIndexArray));
	renderable->SetVertexAttributeArray(kArrayPosition, brush->GetVertexAttributeOffset(kArrayPosition), brush->GetVertexAttributeComponentCount(kArrayPosition));
	renderable->SetPrimitiveCount(brush->GetPrimitiveCount());
	renderable->SetPrimitiveIndexOffset(brush->GetPrimitiveIndexOffset());

	renderable->InvalidateShaderData();
}

void TerrainBlockManipulator::BuildBrush(float radius, float height)
{
	static const ConstColorRGBA sculptBrushColor[TerrainPage::kEditorTerrainSculptCount] =
	{
		{0.0F, 0.5F, 0.0F, 0.0F},
		{0.5F, 0.0F, 0.0F, 0.0F},
		{0.25F, 0.5F, 0.0F, 0.0F},
		{0.5F, 0.25F, 0.0F, 0.0F},
		{0.5F, 0.5F, 0.5F, 0.0F}
	};

	static const ConstColorRGBA paintBrushColor[TerrainPage::kEditorTerrainPaintCount] =
	{
		{0.5F, 0.5F, 0.0F, 0.0F},
		{0.5F, 0.5F, 0.0F, 0.0F},
		{0.0F, 0.25F, 0.5F, 0.0F},
		{0.5F, 0.25F, 0.0F, 0.0F}
	};

	if (renderMode == TerrainPage::kEditorTerrainModeSculpt)
	{
		brushDiffuseColor->SetDiffuseColor(sculptBrushColor[renderTool]);
	}
	else
	{
		brushDiffuseColor->SetDiffuseColor(paintBrushColor[renderTool]);
	}

	if (renderBrush == TerrainPage::kEditorTerrainBrushSphere)
	{
		SphereGeometryObject *object = sphereBrush->GetObject();
		object->SetSphereSize(Vector3D(radius, radius, radius));

		sphereBrush->SetNodeTransform(Identity4D);
		sphereBrush->Invalidate();
		sphereBrush->Update();

		object->Build(sphereBrush);
		sphereBrush->Neutralize();
		sphereBrush->Preprocess();

		if ((renderMode == TerrainPage::kEditorTerrainModeSculpt) && (renderTool == TerrainPage::kEditorTerrainSculptAdd))
		{
			sphereBrush->SetRenderState(kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset);
			sphereBrush->SetDepthOffset(0.5F, &sphereBrush->GetBoundingSphere()->GetCenter());
		}
		else
		{
			sphereBrush->SetRenderState(kRenderDepthTest | kRenderDepthInhibit);
		}

		CloneBrushRenderable(sphereBrush, &sphereRenderable);
	}
	else
	{
		CylinderGeometryObject *object = cylinderBrush->GetObject();
		object->SetCylinderSize(Vector2D(radius, radius));
		object->SetCylinderHeight(height * 2.0F);

		cylinderBrush->SetNodeTransform(Identity4D);
		cylinderBrush->Invalidate();
		cylinderBrush->Update();

		object->Build(cylinderBrush);
		cylinderBrush->Neutralize();
		cylinderBrush->Preprocess();

		if ((renderMode == TerrainPage::kEditorTerrainModeSculpt) && (renderTool == TerrainPage::kEditorTerrainSculptAdd))
		{
			cylinderBrush->SetRenderState(kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset);
			cylinderBrush->SetDepthOffset(0.5F, &cylinderBrush->GetBoundingSphere()->GetCenter());
		}
		else
		{
			cylinderBrush->SetRenderState(kRenderDepthTest | kRenderDepthInhibit);
		}

		CloneBrushRenderable(cylinderBrush, &cylinderRenderable);
	}
}

void TerrainBlockManipulator::SetBrushTransform(const Point3D& position, const Vector3D& normal)
{
	Vector3D n = normal * GetTargetNode()->GetInverseWorldTransform();
	Vector3D t = Math::CreateUnitPerpendicular(n);

	if (renderBrush == TerrainPage::kEditorTerrainBrushSphere)
	{
		sphereBrush->SetNodeTransform(t, n % t, n, position);
		sphereBrush->Invalidate();
		sphereBrush->Update();
	}
	else
	{
		Point3D p = position - n * (cylinderBrush->GetObject()->GetCylinderHeight() * 0.5F);
		cylinderBrush->SetNodeTransform(t, n % t, n, p);
		cylinderBrush->Invalidate();
		cylinderBrush->Update();
	}
}

void TerrainBlockManipulator::Select(void)
{
	EditorManipulator::Select();
	blockDiffuseColor.SetDiffuseColor(kBlockSelectedColor);
}

void TerrainBlockManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	blockDiffuseColor.SetDiffuseColor(kBlockUnselectedColor);
}

void TerrainBlockManipulator::HandleSizeUpdate(int32 count, const float *size)
{
	RescaleBlock(Fmax(size[0], kSizeEpsilon));
	GetEditor()->InvalidateNode(GetTargetNode());
}

bool TerrainBlockManipulator::ReparentedSubnodesAllowed(void) const
{
	return (false);
}

bool TerrainBlockManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const TerrainBlock *block = GetTargetNode();
	Vector3D boxSize = block->GetBlockBoxSize() * 0.5F;

	sphere->SetCenter(Zero3D + boxSize);
	sphere->SetRadius(Magnitude(boxSize));
	return (true);
}

Box3D TerrainBlockManipulator::CalculateNodeBoundingBox(void) const
{
	const TerrainBlock *block = GetTargetNode();
	const Vector3D& boxSize = block->GetBlockBoxSize();
	return (Box3D(Zero3D, Zero3D + boxSize));
}

bool TerrainBlockManipulator::Pick(const Ray *ray, PickData *data) const
{
	Vector3D boxSize = GetTargetNode()->GetBlockBoxSize();

	float r = (ray->radius != 0.0F) ? ray->radius : Editor::kFrustumRenderScale;
	float r2 = r * r * 16.0F;

	for (machine a = 0; a < 12; a++)
	{
		const ConstPoint3D *edge = manipulatorBoxPickEdge[a];
		if (PickLineSegment(ray, edge[0] & boxSize, edge[1] & boxSize, r2, &data->rayParam))
		{
			return (true);
		}
	}

	return (false);
}

bool TerrainBlockManipulator::RegionPick(const VisibilityRegion *region) const
{
	Vector3D boxSize = GetTargetNode()->GetBlockBoxSize();

	for (machine a = 0; a < 12; a++)
	{
		const ConstPoint3D *edge = manipulatorBoxPickEdge[a];
		if (RegionPickLineSegment(region, edge[0] & boxSize, edge[1] & boxSize))
		{
			return (true);
		}
	}

	return (false);
}

bool TerrainBlockManipulator::Resize(const ManipulatorResizeData *resizeData)
{
	float	newScale;

	TerrainBlock *block = GetTargetNode();

	float oldScale = GetOriginalSize()[0];
	const Integer3D& size = block->GetBlockSize();
	Vector3D oldSize((float) (size.x * kTerrainDimension) * oldScale, (float) (size.y * kTerrainDimension) * oldScale, (float) (size.z * kTerrainDimension) * oldScale);

	const Vector3D& delta = resizeData->resizeDelta;
	Vector3D newSize(Fmax(oldSize.x + delta.x, kSizeEpsilon), Fmax(oldSize.y + delta.y, kSizeEpsilon), Fmax(oldSize.z + delta.z, kSizeEpsilon));

	float ax = Fabs(newSize.x - oldSize.x);
	float ay = Fabs(newSize.y - oldSize.y);
	float az = Fabs(newSize.z - oldSize.z);

	if ((ax >= ay) && (ax >= az))
	{
		newScale = oldScale * (newSize.x / oldSize.x);
	}
	else if (ay >= az)
	{
		newScale = oldScale * (newSize.y / oldSize.y);
	}
	else
	{
		newScale = oldScale * (newSize.z / oldSize.z);
	}

	if (newScale != oldScale)
	{
		RescaleBlock(newScale);
	}

	return (false);
}

void TerrainBlockManipulator::RescaleBlock(float scale)
{
	TerrainBlockObject *blockObject = GetObject();

	float factor = scale / blockObject->GetVoxelScale();
	blockObject->SetVoxelScale(scale);

	TerrainBlock *block = GetTargetNode();
	Node *node = block->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				TerrainGeometryObject *geometryObject = static_cast<TerrainGeometry *>(geometry)->GetObject();
				Mesh *mesh = geometryObject->GetGeometryLevel(0);

				int32 vertexCount = mesh->GetVertexCount();
				Point3D *position0 = mesh->GetArray<Point3D>(kArrayPosition0);
				Point3D *position1 = mesh->GetArray<Point3D>(kArrayPosition1);

				if (position1)
				{
					for (machine a = 0; a < vertexCount; a++)
					{
						position0[a] *= factor;
						position1[a] *= factor;
					}
				}
				else
				{
					for (machine a = 0; a < vertexCount; a++)
					{
						position0[a] *= factor;
					}
				}

				geometryObject->GetBoundingBox().Scale(factor);
				geometryObject->ScaleCollisionData(factor);

				geometry->Invalidate();
				geometry->Neutralize();
				geometry->Preprocess();
			}
		}

		node = block->GetNextNode(node);
	}
}

void TerrainBlockManipulator::Render(const ManipulatorRenderData *renderData)
{
	List<Renderable> *renderList = renderData->manipulatorList;
	if (renderList)
	{
		if (!(GetManipulatorState() & kManipulatorHidden))
		{
			blockSizeVector.Set(GetTargetNode()->GetBlockBoxSize(), renderData->viewportScale * 2.0F);
			renderList->Append(&blockRenderable);
		}

		if (renderTool >= 0)
		{
			if (renderBrush == TerrainPage::kEditorTerrainBrushSphere)
			{
				renderList->Append(sphereBrush);
				renderList->Append(&sphereRenderable);
			}
			else
			{
				renderList->Append(cylinderBrush);
				renderList->Append(&cylinderRenderable);
			}
		}
	}

	EditorManipulator::Render(renderData);
}

void TerrainBlockManipulator::SetRenderTool(int32 mode, int32 tool, int32 brush)
{
	renderMode = mode;
	renderTool = tool;
	renderBrush = brush;

	unsigned_int32 state = GetManipulatorState();
	if (tool >= 0)
	{
		state |= kManipulatorForceRender;
	}
	else
	{
		state &= ~kManipulatorForceRender;
	}

	SetManipulatorState(state);
}


TerrainGeometryManipulator::TerrainGeometryManipulator(TerrainGeometry *terrain) : GeometryManipulator(terrain)
{
	SetManipulatorFlags(kManipulatorLockedTransform | kManipulatorLockedController);

	paintMeshStorage = nullptr;
}

TerrainGeometryManipulator::~TerrainGeometryManipulator()
{
	delete[] paintMeshStorage;
}

const char *TerrainGeometryManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeGeometry, kGeometryTerrain)));
}

bool TerrainGeometryManipulator::ReparentedSubnodesAllowed(void) const
{
	return (false);
}

bool TerrainGeometryManipulator::Pick(const Ray *ray, PickData *data) const
{
	if (GetObject()->GetDetailLevel() > 0)
	{
		return (false);
	}

	return (GeometryManipulator::Pick(ray, data));
}

bool TerrainGeometryManipulator::RegionPick(const VisibilityRegion *region) const
{
	if (GetObject()->GetDetailLevel() > 0)
	{
		return (false);
	}

	return (GeometryManipulator::RegionPick(region));
}

void TerrainGeometryManipulator::CopyTriangleMesh(List<TerrainGeometryManipulator> *terrainPaintList)
{
	if (!paintMeshStorage)
	{
		const GeometryObject *object = GetObject();
		const Mesh *mesh = object->GetGeometryLevel(0);

		int32 vertexCount = mesh->GetVertexCount();
		paintTriangleCount = mesh->GetPrimitiveCount();

		paintMeshStorage = new char[sizeof(Point3D) * vertexCount + sizeof(Triangle) * paintTriangleCount];
		paintMeshVertex = reinterpret_cast<Point3D *>(paintMeshStorage);
		paintMeshTriangle = reinterpret_cast<Triangle *>(paintMeshVertex + vertexCount);

		const Point3D *vertex = mesh->GetArray<Point3D>(kArrayPosition);
		const Triangle *triangle = mesh->GetArray<Triangle>(kArrayPrimitive);

		MemoryMgr::CopyMemory(vertex, paintMeshVertex, sizeof(Point3D) * vertexCount);
		MemoryMgr::CopyMemory(triangle, paintMeshTriangle, sizeof(Triangle) * paintTriangleCount);

		terrainPaintList->Append(this);
	}
}

bool TerrainGeometryManipulator::GetPaintPosition(const Ray *ray, int32 mode, bool modifiedFlag, GeometryHitData *geometryHitData)
{
	float	t1, t2;

	const Node *node = GetTargetNode();
	const BoundingSphere *sphere = node->GetBoundingSphere();

	if (Math::IntersectRayAndSphere(ray, node->GetInverseWorldTransform() * sphere->GetCenter(), sphere->GetRadius(), &t1, &t2))
	{
		Point3D p1 = ray->origin + ray->direction * t1;
		Point3D p2 = ray->origin + ray->direction * t2;

		if (paintMeshStorage)
		{
			bool result = false;
			float tmax = ray->tmax;

			const Point3D *vertex = paintMeshVertex;
			const Triangle *triangle = paintMeshTriangle;

			int32 triangleCount = paintTriangleCount;
			for (machine index = 0; index < triangleCount; index++)
			{
				Point3D		position;
				Vector3D	normal;
				float		param;

				const Point3D& v0 = vertex[triangle->index[0]];
				const Point3D& v1 = vertex[triangle->index[1]];
				const Point3D& v2 = vertex[triangle->index[2]];

				if (Math::IntersectSegmentAndTriangle(p1, p2, v0, v1, v2, &position, &normal, &param))
				{
					param = t1 * (1.0F - param) + t2 * param;
					if ((param >= ray->tmin) && (param <= tmax))
					{
						tmax = param;
						geometryHitData->param = param;
						geometryHitData->position = position;
						geometryHitData->normal = normal;
						result = true;
					}
				}

				triangle++;
			}

			return (result);
		}
		else if ((mode != TerrainPage::kEditorTerrainModeSculpt) || (!modifiedFlag))
		{
			if (GetObject()->DetectCollision(p1, p2, 0.0F, geometryHitData))
			{
				float t = geometryHitData->param;
				t = t1 * (1.0F - t) + t2 * t;
				if ((t >= ray->tmin) && (t <= ray->tmax))
				{
					geometryHitData->param = t;
					return (true);
				}
			}
		}
	}

	return (false);
}

void TerrainGeometryManipulator::CleanupPaintMesh(void)
{
	delete[] paintMeshStorage;
	paintMeshStorage = nullptr;

	ListElement<TerrainGeometryManipulator>::Detach();
}


TerrainPaintOperation::TerrainPaintOperation(TerrainBlock *block, int32 channel1, int32 channel2) : Operation(kOperationTerrainPaint)
{
	blockNode = block;
	terrainChannel[0] = channel1;
	terrainChannel[1] = channel2;

	Integer3D size = block->GetBlockSize();
	int32 count0 = size.x * size.y * size.z;
	int32 totalCount = count0;
	terrainCount[0] = count0;

	for (machine a = 1; a < kTerrainEditLevelCount; a++)
	{
		size.x = (size.x + 1) >> 1;
		size.y = (size.y + 1) >> 1;
		size.z = (size.z + 1) >> 1;

		int32 count = size.x * size.y * size.z;
		totalCount += count;
		terrainCount[a] = count;
	}

	if (channel2 >= 0)
	{
		undoStorage = new char[count0 * (sizeof(char *) * 2) + totalCount * (sizeof(MaterialObject *) + 1)];
		drawingChannelData[0] = reinterpret_cast<char **>(undoStorage);
		drawingChannelData[1] = drawingChannelData[0] + count0;

		materialObject[0] = reinterpret_cast<MaterialObject **>(drawingChannelData[1] + count0);
		for (machine a = 1; a < kTerrainEditLevelCount; a++)
		{
			materialObject[a] = materialObject[a - 1] + terrainCount[a - 1];
		}

		terrainFlags[0] = reinterpret_cast<unsigned_int8 *>(materialObject[kTerrainEditLevelCount - 1] + terrainCount[kTerrainEditLevelCount - 1]);
		for (machine a = 1; a < kTerrainEditLevelCount; a++)
		{
			terrainFlags[a] = terrainFlags[a - 1] + terrainCount[a - 1];
		}

		for (machine a = 0; a < count0; a++)
		{
			drawingChannelData[0][a] = nullptr;
			drawingChannelData[1][a] = nullptr;
			terrainFlags[0][a] = 0;
		}
	}
	else
	{
		undoStorage = new char[count0 * sizeof(char *) + totalCount * (sizeof(MaterialObject *) + 1)];
		drawingChannelData[0] = reinterpret_cast<char **>(undoStorage);

		materialObject[0] = reinterpret_cast<MaterialObject **>(drawingChannelData[0] + count0);
		for (machine a = 1; a < kTerrainEditLevelCount; a++)
		{
			materialObject[a] = materialObject[a - 1] + terrainCount[a - 1];
		}

		terrainFlags[0] = reinterpret_cast<unsigned_int8 *>(materialObject[kTerrainEditLevelCount - 1] + terrainCount[kTerrainEditLevelCount - 1]);
		for (machine a = 1; a < kTerrainEditLevelCount; a++)
		{
			terrainFlags[a] = terrainFlags[a - 1] + terrainCount[a - 1];
		}

		for (machine a = 0; a < count0; a++)
		{
			drawingChannelData[0][a] = nullptr;
			terrainFlags[0][a] = 0;
		}
	}

	for (machine a = 1; a < kTerrainEditLevelCount; a++)
	{
		int32 count = terrainCount[a];
		for (machine b = 0; b < count; b++)
		{
			terrainFlags[a][b] = 0;
		}
	}
}

TerrainPaintOperation::~TerrainPaintOperation()
{
	int32 count = terrainCount[0];
	for (machine a = 0; a < count; a++)
	{
		if (terrainFlags[0][a] & kTerrainPaintUndoModified)
		{
			TerrainStorage::DeleteVoxelData(drawingChannelData[0][a]);
			if (terrainChannel[1] >= 0)
			{
				TerrainStorage::DeleteVoxelData(drawingChannelData[1][a]);
			}

			MaterialObject *object = materialObject[0][a];
			if (object)
			{
				object->Release();
			}
		}
	}

	for (machine a = 1; a < kTerrainEditLevelCount; a++)
	{
		count = terrainCount[a];
		for (machine b = 0; b < count; b++)
		{
			if (terrainFlags[a][b] & kTerrainPaintUndoModified)
			{
				MaterialObject *object = materialObject[a][b];
				if (object)
				{
					object->Release();
				}
			}
		}
	}

	delete[] undoStorage;
}

bool TerrainPaintOperation::AddTerrain(const Integer3D& coord, int32 level)
{
	int32 ds = (1 << level) - 1;
	const Integer3D& size = blockNode->GetBlockSize();
	int32 index = ((coord.z >> level) * ((size.y + ds) >> level) + (coord.y >> level)) * ((size.x + ds) >> level) + (coord.x >> level);

	if (!(terrainFlags[level][index] & kTerrainPaintUndoModified))
	{
		if (level == 0)
		{
			drawingChannelData[0][index] = blockNode->SaveVoxelData(terrainChannel[0], coord);
			if (terrainChannel[1] >= 0)
			{
				drawingChannelData[1][index] = blockNode->SaveVoxelData(terrainChannel[1], coord);
			}
		}

		unsigned_int32 flags = kTerrainPaintUndoModified;

		const TerrainGeometry *geometry = blockNode->FindTerrainGeometry(coord, level);
		if (geometry)
		{
			MaterialObject *object = geometry->GetMaterialObject(0);
			if (object)
			{
				object->Retain();
			}

			materialObject[level][index] = object;
			flags |= kTerrainPaintUndoExisted;
		}
		else
		{
			materialObject[level][index] = nullptr;
		}

		terrainFlags[level][index] = (unsigned_int8) flags;
		return (true);
	}

	return (false);
}

bool TerrainPaintOperation::TerrainModified(const Integer3D& coord, int32 level) const
{
	int32 ds = (1 << level) - 1;
	const Integer3D& size = blockNode->GetBlockSize();
	int32 index = ((coord.z >> level) * ((size.y + ds) >> level) + (coord.y >> level)) * ((size.x + ds) >> level) + (coord.x >> level);
	return ((terrainFlags[level][index] & kTerrainPaintUndoModified) != 0);
}

VoxelMap *TerrainPaintOperation::OpenSavedVoxelMap(const Integer3D& vmin, const Integer3D& vmax) const
{
	VoxelBox	blockBounds, voxelBounds;

	const Integer3D& blockSize = blockNode->GetBlockSize();

	int32 xmin = MaxZero(Min((vmin.x >> kTerrainLogDimension) - 1, blockSize.x - 1));
	int32 xmax = MaxZero(Min((vmax.x >> kTerrainLogDimension) + 1, blockSize.x - 1));
	int32 ymin = MaxZero(Min((vmin.y >> kTerrainLogDimension) - 1, blockSize.y - 1));
	int32 ymax = MaxZero(Min((vmax.y >> kTerrainLogDimension) + 1, blockSize.y - 1));
	int32 zmin = MaxZero(Min((vmin.z >> kTerrainLogDimension) - 1, blockSize.z - 1));
	int32 zmax = MaxZero(Min((vmax.z >> kTerrainLogDimension) + 1, blockSize.z - 1));

	blockBounds.min.Set(xmin, ymin, zmin);
	blockBounds.max.Set(xmax, ymax, zmax);

	voxelBounds.min.Set(Max(vmin.x, xmin << kTerrainLogDimension), Max(vmin.y, ymin << kTerrainLogDimension), Max(vmin.z, zmin << kTerrainLogDimension));
	voxelBounds.max.Set(Min(vmax.x, (xmax << kTerrainLogDimension) + (kTerrainDimension - 1)), Min(vmax.y, (ymax << kTerrainLogDimension) + (kTerrainDimension - 1)), Min(vmax.z, (zmax << kTerrainLogDimension) + (kTerrainDimension - 1)));

	Integer3D size((xmax - xmin + 1) * kTerrainDimension, (ymax - ymin + 1) * kTerrainDimension, (zmax - zmin + 1) * kTerrainDimension);
	Integer3D origin(-(xmin << kTerrainLogDimension), -(ymin << kTerrainLogDimension), -(zmin << kTerrainLogDimension));

	VoxelMap *voxelMap = new VoxelMap(size, origin, blockBounds, voxelBounds);

	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	Voxel *base = voxelMap->GetVoxelDataBase();

	for (machine k = zmin; k <= zmax; k++)
	{
		for (machine j = ymin; j <= ymax; j++)
		{
			for (machine i = xmin; i <= xmax; i++)
			{
				Voxel *voxel = base + ((k - zmin) * deckSize + (j - ymin) * rowSize + (i - xmin)) * kTerrainDimension;
				blockNode->LoadVoxels(terrainChannel[0], drawingChannelData[0], Integer3D(i, j, k), voxel, rowSize, deckSize);
			}
		}
	}

	return (voxelMap);
}

void TerrainPaintOperation::CloseSavedVoxelMap(VoxelMap *voxelMap)
{
	delete voxelMap;
}

void TerrainPaintOperation::Restore(Editor *editor)
{
	const Integer3D& size = blockNode->GetBlockSize();

	for (machine k = 0; k < size.z; k++)
	{
		for (machine j = 0; j < size.y; j++)
		{
			for (machine i = 0; i < size.x; i++)
			{
				int32 index = (k * size.y + j) * size.x + i;
				if (terrainFlags[0][index] & kTerrainPaintUndoModified)
				{
					blockNode->RestoreVoxelData(terrainChannel[0], Integer3D(i, j, k), drawingChannelData[0][index]);
					drawingChannelData[0][index] = nullptr;

					if (terrainChannel[1] >= 0)
					{
						blockNode->RestoreVoxelData(terrainChannel[1], Integer3D(i, j, k), drawingChannelData[1][index]);
						drawingChannelData[1][index] = nullptr;
					}
				}
			}
		}
	}

	for (machine a = 0; a < kTerrainEditLevelCount; a++)
	{
		int32 ds = (1 << a) - 1;
		int32 x = (size.x + ds) >> a;
		int32 y = (size.y + ds) >> a;
		int32 z = (size.z + ds) >> a;

		for (machine k = 0; k < z; k++)
		{
			for (machine j = 0; j < y; j++)
			{
				for (machine i = 0; i < x; i++)
				{
					int32 index = (k * y + j) * x + i;
					unsigned_int32 flags = terrainFlags[a][index];
					if (flags & kTerrainPaintUndoModified)
					{
						unsigned_int32 updateFlags = (flags & kTerrainPaintUndoExisted) ? 0 : kTerrainUpdateForceDelete;
						TerrainPage::UpdateBlock(editor, blockNode, Integer3D(i << a, j << a, k << a), a, materialObject[a][index], updateFlags);
					}
				}
			}
		}
	}

	blockNode->ProcessStructure();
}


TerrainTextureWidget::TerrainTextureWidget(int32 groupIndex) :
		RenderableWidget(kWidgetTerrainTexture, kRenderQuads, Vector2D(158.0F, 48.0F)),
		imageVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		backgroundVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		backgroundColorAttribute(ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F)),
		backgroundRenderable(kRenderQuads, kRenderDepthInhibit)
{
	textureGroupIndex = groupIndex;
	hiliteFace = -1;
}

TerrainTextureWidget::~TerrainTextureWidget()
{
}

WidgetPart TerrainTextureWidget::TestPosition(const Point3D& position) const
{
	float x = 0.0F;
	for (machine a = 0; a < 3; a++)
	{
		if ((position.x >= x) && (position.x <= x + 47.0F) && (position.y >= 0.0F) && (position.y <= 47.0F))
		{
			return (kWidgetPartTexture + a);
		}

		x += 53.0F;
	}

	return (kWidgetPartNone);
}

void TerrainTextureWidget::Preprocess(void)
{
	static const ConstPoint2D backgroundVertex[12] =
	{
		{ -1.0F,  48.0F}, { 48.0F,  48.0F}, { 48.0F,  -1.0F}, { -1.0F,  -1.0F},
		{ 52.0F,  48.0F}, {101.0F,  48.0F}, {101.0F,  -1.0F}, { 52.0F,  -1.0F},
		{105.0F,  48.0F}, {154.0F,  48.0F}, {154.0F,  -1.0F}, {105.0F,  -1.0F}
	};

	RenderableWidget::Preprocess();

	SetVertexCount(12);
	SetVertexBuffer(kVertexBufferAttributeArray, &imageVertexBuffer, sizeof(ImageVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 3);
	SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D) + sizeof(ColorRGB), 3);
	imageVertexBuffer.Establish(sizeof(ImageVertex) * 12);

	imageAttributeList.Append(&imageTextureAttribute);
	SetMaterialAttributeList(&imageAttributeList);
	GetFirstRenderSegment()->SetMaterialState(kMaterialAlphaSemanticInhibit);
	SetAmbientBlendState(kBlendReplace);

	InitRenderable(&backgroundRenderable);
	backgroundRenderable.SetVertexCount(12);
	backgroundRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &backgroundVertexBuffer, sizeof(Point2D));
	backgroundRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	backgroundVertexBuffer.Establish(sizeof(Point2D) * 12, backgroundVertex);

	backgroundAttributeList.Append(&backgroundColorAttribute);
	backgroundRenderable.SetMaterialAttributeList(&backgroundAttributeList);
}

void TerrainTextureWidget::Build(void)
{
	RenderableWidget::Build();

	volatile ImageVertex *restrict imageVertex = imageVertexBuffer.BeginUpdate<ImageVertex>();

	int32 face = hiliteFace;
	float c1 = (face == 0) ? 0.5F : 1.0F;
	float c2 = (face == 1) ? 0.5F : 1.0F;
	float c3 = (face == 2) ? 0.5F : 1.0F;

	imageVertex[0].position.Set(0.0F, 47.0F);
	imageVertex[0].color.Set(c1, c1, c1);
	imageVertex[1].position.Set(47.0F, 47.0F);
	imageVertex[1].color.Set(c1, c1, c1);
	imageVertex[2].position.Set(47.0F, 0.0F);
	imageVertex[2].color.Set(c1, c1, c1);
	imageVertex[3].position.Set(0.0F, 0.0F);
	imageVertex[3].color.Set(c1, c1, c1);

	imageVertex[4].position.Set(53.0F, 47.0F);
	imageVertex[4].color.Set(c2, c2, c2);
	imageVertex[5].position.Set(100.0F, 47.0F);
	imageVertex[5].color.Set(c2, c2, c2);
	imageVertex[6].position.Set(100.0F, 0.0F);
	imageVertex[6].color.Set(c2, c2, c2);
	imageVertex[7].position.Set(53.0F, 0.0F);
	imageVertex[7].color.Set(c2, c2, c2);

	imageVertex[8].position.Set(106.0F, 47.0F);
	imageVertex[8].color.Set(c3, c3, c3);
	imageVertex[9].position.Set(153.0F, 47.0F);
	imageVertex[9].color.Set(c3, c3, c3);
	imageVertex[10].position.Set(153.0F, 0.0F);
	imageVertex[10].color.Set(c3, c3, c3);
	imageVertex[11].position.Set(106.0F, 0.0F);
	imageVertex[11].color.Set(c3, c3, c3);

	const Texture *texture = imageTextureAttribute.GetTexture();
	for (machine a = 0; a < 3; a++)
	{
		float index = (float) paletteIndex[a];
		volatile ImageVertex *restrict vertex = imageVertex + a * 4;

		if ((texture) && (texture->GetTextureType() == kTextureArray2D))
		{
			vertex[0].texcoord.Set(0.25F, 0.25F, index);
			vertex[1].texcoord.Set(0.75F, 0.25F, index);
			vertex[2].texcoord.Set(0.75F, 0.75F, index);
			vertex[3].texcoord.Set(0.25F, 0.75F, index);
		}
		else
		{
			float w = (float) entryCountX;
			float t = PositiveFloor(index * (1.0F / w + 1.0e-5F));
			float s = index - t * w;

			float ds = 1.0F / (float) (entryCountX + entryCountX / 3);
			float dt = 1.0F / (float) (entryCountY + entryCountY / 3);

			s = (s * 1.25F + 0.375F) * ds;
			t = (t * 1.25F + 0.375F) * dt;

			vertex[0].texcoord.Set(s, t, 0.0F);
			vertex[1].texcoord.Set(s + ds * 0.5F, t, 0.0F);
			vertex[2].texcoord.Set(s + ds * 0.5F, t + dt * 0.5F, 0.0F);
			vertex[3].texcoord.Set(s, t + dt * 0.5F, 0.0F);
		}
	}

	imageVertexBuffer.EndUpdate();
}

void TerrainTextureWidget::SetTexture(const char *name)
{
	imageTextureAttribute.SetTexture(name ? name : "C4/black");
	InvalidateShaderData();

	const Texture *texture = imageTextureAttribute.GetTexture();
	if ((texture) && (texture->GetTextureFlags() & kTextureImagePalette))
	{
		const unsigned_int32 *paletteSize = texture->GetPaletteSize();
		entryCountX = paletteSize[0];
		entryCountY = paletteSize[1];
	}
	else
	{
		entryCountX = 1;
		entryCountY = 1;
	}

	SetBuildFlag();
}

void TerrainTextureWidget::SetTerrainMaterial(TerrainMaterial *material)
{
	if (textureGroupIndex == 0)
	{
		textureSelector = &material->primaryMaterial;
	}
	else
	{
		textureSelector = &material->secondaryMaterial;
	}

	paletteIndex[0] = textureSelector->GetRed();
	paletteIndex[1] = textureSelector->GetGreen();
	paletteIndex[2] = textureSelector->GetBlue();

	SetBuildFlag();
}

void TerrainTextureWidget::SetHilitedFace(int32 face)
{
	hiliteFace = face;
	SetBuildFlag();
}

void TerrainTextureWidget::SelectPaletteEntry(TerrainTextureWindow *window, TerrainTextureWidget *widget)
{
	int32 face = widget->activeFace;
	unsigned_int32 entry = window->GetSelectedEntry();

	if (face == 0)
	{
		widget->textureSelector->SetRed(entry);
	}
	else if (face == 1)
	{
		widget->textureSelector->SetGreen(entry);
	}
	else
	{
		widget->textureSelector->SetBlue(entry);
	}

	widget->paletteIndex[face] = entry;
	widget->SetBuildFlag();
}

void TerrainTextureWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	EventType eventType = eventData->eventType;

	if (eventType == kEventMouseDown)
	{
		SetHilitedFace(eventData->widgetPart - kWidgetPartTexture);
	}
	else
	{
		bool inside = (TerrainTextureWidget::TestPosition(eventData->mousePosition) == eventData->widgetPart);

		if (eventType == kEventMouseMoved)
		{
			if (inside)
			{
				SetHilitedFace(eventData->widgetPart - kWidgetPartTexture);
			}
			else
			{
				SetHilitedFace(-1);
			}
		}
		else if (eventType == kEventMouseUp)
		{
			SetHilitedFace(-1);
			if (inside)
			{
				Texture *texture = imageTextureAttribute.GetTexture();
				if ((texture) && (texture->GetTextureFlags() & kTextureImagePalette))
				{
					activeFace = eventData->widgetPart - kWidgetPartTexture;

					TerrainTextureWindow *window = new TerrainTextureWindow(texture, paletteIndex[activeFace]);
					window->SetCompletionProc(&SelectPaletteEntry, this);

					float windowWidth = window->GetWidgetSize().x;
					float windowHeight = window->GetWidgetSize().y;

					float widgetWidth = GetWidgetSize().x;
					float widgetHeight = GetWidgetSize().y;

					Point3D position(widgetWidth + 4.0F, (widgetHeight - windowHeight) * 0.5F, 0.0F);
					position += GetWorldPosition();

					const Vector2D& desktopSize = TheInterfaceMgr->GetDesktopSize();

					if (position.x + windowWidth > desktopSize.x)
					{
						position.x -= windowWidth + widgetWidth + 8.0F;
					}

					if (position.y + windowHeight > desktopSize.y)
					{
						position.y = desktopSize.y - windowHeight;
					}

					Window *owner = GetOwningWindow();
					window->SetWidgetPosition(Zero3D + (position - owner->GetWorldPosition()));
					owner->AddSubwindow(window);
				}
			}
		}
	}
}

void TerrainTextureWidget::Render(List<Renderable> *renderList)
{
	renderList->Append(&backgroundRenderable);
	RenderableWidget::Render(renderList);
}


PaletteWidget::PaletteWidget(const Vector2D& size, Texture *texture) : ImageWidget(size, texture)
{
}

PaletteWidget::~PaletteWidget()
{
}

void PaletteWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseDown)
	{
		Activate();
	}
}


TerrainTextureWindow::TerrainTextureWindow(Texture *texture, unsigned_int32 selection) : Window(Vector2D((float) texture->GetPaletteSize()[0] * 72.0F + 8.0F, (float) texture->GetPaletteSize()[1] * 72.0F + 8.0F), nullptr, kWindowPlain)
{
	paletteTexture = texture;
	selectedEntry = selection;
}

TerrainTextureWindow::~TerrainTextureWindow()
{
}

void TerrainTextureWindow::Preprocess(void)
{
	Window::Preprocess();

	AppendNewSubnode(new QuadWidget(GetWidgetSize(), K::black));

	const unsigned_int32 *paletteSize = paletteTexture->GetPaletteSize();
	unsigned_int32 entryCountX = paletteSize[0];
	unsigned_int32 entryCountY = paletteSize[1];
	entryCount = entryCountX * entryCountY;

	if (paletteTexture->GetTextureType() == kTextureArray2D)
	{
		int32 index = 0;
		for (unsigned_machine j = 0; j < entryCountY; j++)
		{
			float y = (float) j * 72.0F + 8.0F;

			for (unsigned_machine i = 0; i < entryCountX; i++)
			{
				float x = (float) i * 72.0F + 8.0F;

				PaletteWidget *image = new PaletteWidget(Vector2D(64.0F, 64.0F), paletteTexture);
				image->SetWidgetPosition(Point3D(x, y, 0.0F));
				image->SetImagePCoordinate((float) index);
				AppendNewSubnode(image);

				image->SetAmbientBlendState(kBlendReplace);
				paletteImage[index] = image;
				index++;
			}
		}
	}
	else
	{
		float ds = 1.0F / (float) (entryCountX + entryCountX / 3);
		float dt = 1.0F / (float) (entryCountY + entryCountY / 3);

		int32 index = 0;
		for (unsigned_machine j = 0; j < entryCountY; j++)
		{
			float y = (float) j * 72.0F + 8.0F;
			float t = ((float) j * 1.25F + 0.125F) * dt;

			for (unsigned_machine i = 0; i < entryCountX; i++)
			{
				float x = (float) i * 72.0F + 8.0F;
				float s = ((float) i * 1.25F + 0.125F) * ds;

				PaletteWidget *image = new PaletteWidget(Vector2D(64.0F, 64.0F), paletteTexture);
				image->SetWidgetPosition(Point3D(x, y, 0.0F));
				image->SetImageScale(Vector2D(ds, dt));
				image->SetImageOffset(Vector2D(s, t));
				AppendNewSubnode(image);

				image->SetAmbientBlendState(kBlendReplace);
				paletteImage[index] = image;
				index++;
			}
		}
	}
}

bool TerrainTextureWindow::HandleKeyboardEvent(const KeyboardEventData *eventData)
{
	if (eventData->eventType == kEventKeyDown)
	{
		if (eventData->keyCode == kKeyCodeEscape)
		{
			Close();
			return (true);
		}
	}

	return (Window::HandleKeyboardEvent(eventData));
}

void TerrainTextureWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		for (machine a = 0; a < entryCount; a++)
		{
			if (widget == paletteImage[a])
			{
				selectedEntry = a;
				CallCompletionProc();
				Close();
				break;
			}
		}
	}
}


OptimizeTerrainWindow::OptimizeTerrainWindow(Editor *editor) : Window("WorldEditor/terrain/Optimize")
{
	worldEditor = editor;
}

OptimizeTerrainWindow::~OptimizeTerrainWindow()
{
	int32 count = jobCount;
	for (machine a = count - 1; a >= 0; a--)
	{
		delete jobTable[a];
	}

	delete[] jobTable;

	const NodeReference *reference = worldEditor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				worldEditor->InvalidateGeometry(geometry);
			}
		}

		reference = reference->Next();
	}
}

void OptimizeTerrainWindow::OptimizeTerrainJob(Job *job, void *cookie)
{
	Mesh	tempMesh;

	TerrainGeometry *terrain = static_cast<TerrainGeometry *>(cookie);
	TerrainGeometryObject *object = terrain->GetObject();

	Mesh *geometryMesh = object->GetGeometryLevel(0);
	tempMesh.CopyMesh(geometryMesh);

	float threshold = terrain->GetBlockNode()->GetObject()->GetVoxelScale() * 0.75F;

	int32 detailLevel = object->GetDetailLevel();
	if (detailLevel == 0)
	{
		geometryMesh->OptimizeMesh(&tempMesh, threshold);
		object->BuildCollisionData();
	}
	else
	{
		TerrainLevelGeometryObject *levelObject = static_cast<TerrainLevelGeometryObject *>(object);

		int32 triangleCount = tempMesh.GetPrimitiveCount();
		geometryMesh->OptimizeMesh(&tempMesh, threshold * (float) (1 << detailLevel), levelObject->GetBaseTriangleCount());
		levelObject->OffsetBorderRenderData(geometryMesh->GetPrimitiveCount() - triangleCount);
	}
}

void OptimizeTerrainWindow::Preprocess(void)
{
	Window::Preprocess();

	stopButton = static_cast<PushButtonWidget *>(FindWidget("Stop"));
	progressBar = static_cast<ProgressWidget *>(FindWidget("Progress"));

	auto filter = [](const Geometry *geometry) -> bool
	{
		return (geometry->GetGeometryType() == kGeometryTerrain);
	};

	worldEditor->AddOperation(new GeometryOperation(worldEditor->GetSelectionList(), filter));

	int32 count = 0;
	const NodeReference *reference = worldEditor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				count++;
			}
		}

		reference = reference->Next();
	}

	progressBar->SetMaxValue(count);

	jobCount = count;
	jobTable = new Job *[count];

	count = 0;
	reference = worldEditor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				TerrainGeometry *terrain = static_cast<TerrainGeometry *>(geometry);
				Job *job = new Job(&OptimizeTerrainJob, terrain);
				TheJobMgr->SubmitJob(job);
				jobTable[count] = job;
				count++;
			}
		}

		reference = reference->Next();
	}
}

void OptimizeTerrainWindow::Move(void)
{
	Window::Move();

	int32 count = jobCount;
	int32 progress = 0;

	for (machine a = 0; a < count; a++)
	{
		progress += jobTable[a]->Complete();
	}

	progressBar->SetValue(progress);

	if (progress == count)
	{
		Close();
	}
}

void OptimizeTerrainWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if ((eventData->eventType == kEventWidgetActivate) && (widget == stopButton))
	{
		TheJobMgr->CancelJobArray(jobCount, jobTable);
		Close();
	}
}


RebuildTerrainBlockWindow::RebuildTerrainBlockWindow(Editor *editor, TerrainBlock *block) : Window("WorldEditor/terrain/Rebuild")
{
	worldEditor = editor;
	blockNode = block;
}

RebuildTerrainBlockWindow::~RebuildTerrainBlockWindow()
{
}

void RebuildTerrainBlockWindow::Preprocess(void)
{
	Window::Preprocess();

	okayButton = static_cast<PushButtonWidget *>(FindWidget("OK"));
	cancelButton = static_cast<PushButtonWidget *>(FindWidget("Cancel"));

	countBox[0] = static_cast<EditTextWidget *>(FindWidget("Xcount"));
	countBox[1] = static_cast<EditTextWidget *>(FindWidget("Ycount"));
	countBox[2] = static_cast<EditTextWidget *>(FindWidget("Zcount"));

	offsetBox[0] = static_cast<EditTextWidget *>(FindWidget("Xoffset"));
	offsetBox[1] = static_cast<EditTextWidget *>(FindWidget("Yoffset"));
	offsetBox[2] = static_cast<EditTextWidget *>(FindWidget("Zoffset"));

	const Integer3D& size = blockNode->GetBlockSize();
	countBox[0]->SetText(Text::IntegerToString(size[0]));
	countBox[1]->SetText(Text::IntegerToString(size[1]));
	countBox[2]->SetText(Text::IntegerToString(size[2]));

	SetFocusWidget(countBox[0]);
}

void RebuildTerrainBlockWindow::Move(void)
{
	Window::Move();

	for (machine a = 0; a < 3; a++)
	{
		if (Text::StringToInteger(countBox[a]->GetText()) <= 0)
		{
			okayButton->Disable();
			return;
		}
	}

	okayButton->Enable();
}

void RebuildTerrainBlockWindow::CommitSettings(void) const
{
	Integer3D		size;
	Integer3D		offset;

	for (machine a = 0; a < 3; a++)
	{
		size[a] = Text::StringToInteger(countBox[a]->GetText());
		offset[a] = (Text::StringToInteger(offsetBox[a]->GetText()) + 3) & ~3;
	}

	blockNode->ResizeBlock(size, offset);

	float scale = blockNode->GetObject()->GetVoxelScale();
	Vector3D dv((float) (offset.x * kTerrainDimension) * scale, (float) (offset.y * kTerrainDimension) * scale, (float) (offset.z * kTerrainDimension) * scale);

	Node *node = blockNode->GetFirstSubnode();
	while (node)
	{
		Node *next = blockNode->GetNextNode(node);

		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				TerrainGeometryObject *terrainObject = static_cast<TerrainGeometry *>(geometry)->GetObject();
				Integer3D coord = terrainObject->GetGeometryCoord() + offset;
				if (((unsigned_int32) coord.x < (unsigned_int32) size.x) && ((unsigned_int32) coord.y < (unsigned_int32) size.y) && ((unsigned_int32) coord.z < (unsigned_int32) size.z))
				{
					terrainObject->SetGeometryCoord(coord);

					Mesh *mesh = terrainObject->GetGeometryLevel(0);
					int32 vertexCount = mesh->GetVertexCount();

					Point3D *position0 = mesh->GetArray<Point3D>(kArrayPosition0);
					for (machine a = 0; a < vertexCount; a++)
					{
						position0[a] += dv;
					}

					Point3D *position1 = mesh->GetArray<Point3D>(kArrayPosition1);
					if (position1)
					{
						for (machine a = 0; a < vertexCount; a++)
						{
							position1[a] += dv;
						}
					}

					Box3D& boundingBox = terrainObject->GetBoundingBox();
					boundingBox.min += dv;
					boundingBox.max += dv;

					terrainObject->OffsetCollisionData(dv);
				}
				else
				{
					worldEditor->DeleteNode(geometry);
				}
			}
		}

		node = next;
	}

	node = blockNode->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetGeometryType() == kGeometryTerrain)
			{
				const TerrainGeometryObject *terrainObject = static_cast<TerrainGeometry *>(geometry)->GetObject();
				if (terrainObject->GetDetailLevel() != 0)
				{
					static_cast<TerrainLevelGeometry *>(geometry)->ProcessStructure();
				}

				worldEditor->InvalidateGeometry(geometry);
			}
		}

		node = blockNode->GetNextNode(node);
	}
}

void RebuildTerrainBlockWindow::HandleWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		if (widget == okayButton)
		{
			CommitSettings();
			Close();
		}
		else if (widget == cancelButton)
		{
			Close();
		}
	}
}


TerrainRebuildOperation::TerrainRebuildOperation(TerrainBlock *block) : Operation(kOperationTerrainRebuild)
{
	blockNode = block;
	blockSize = block->GetBlockSize();
	voxelScale = block->GetObject()->GetVoxelScale();
}

TerrainRebuildOperation::~TerrainRebuildOperation()
{
}

void TerrainRebuildOperation::Restore(Editor *editor)
{
	for (;;)
	{
		Node *node = blockNode->GetFirstSubnode();
		if (!node)
		{
			break;
		}

		editor->DeleteNode(node);
	}

	blockNode->SetBlockSize(blockSize);
	blockNode->GetObject()->SetVoxelScale(voxelScale);

	editor->InvalidateNode(blockNode);
	blockNode->Preprocess();
}


TerrainPage::TerrainPage() :
		EditorPage(kEditorPageTerrain, "WorldEditor/terrain/Terrain", kEditorBookEarth),
		editorObserver(this, &TerrainPage::HandleEditorEvent),
		toolObserver(this, &TerrainPage::HandleToolButtonEvent),
		brushObserver(this, &TerrainPage::HandleBrushEvent),
		slopeObserver(this, &TerrainPage::HandleSlopeEvent),
		planeObserver(this, &TerrainPage::HandlePlaneEvent),
		sliderObserver(this, &TerrainPage::HandleSliderEvent),
		checkObserver(this, &TerrainPage::HandleCheckEvent),
		menuButtonObserver(this, &TerrainPage::HandleMenuButtonEvent)
{
	currentTool = -1;
	currentBrush = kEditorTerrainBrushSphere;
	currentSlope = 0;
	currentPlane = kEditorTerrainPlaneHorizontal;

	radiusValue = 5;
	heightValue = 90;
	offsetValue = 50;
	bulldozerValue = 32;
	blendValue = 0;
	fuzzyValue = 50;
	stylusValue = 0;
	materialValue = 0;

	terrainMaterial.primaryMaterial.Set(0, 0, 0);
	terrainMaterial.secondaryMaterial.Set(0, 0, 0);
}

TerrainPage::~TerrainPage()
{
	terrainPaintList.RemoveAll();
}

void TerrainPage::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EditorPage::Pack(data, packFlags);

	data << ChunkHeader('BRSH', 4);
	data << currentBrush;

	data << ChunkHeader('SLOP', 4);
	data << currentSlope;

	data << ChunkHeader('PLAN', 4);
	data << currentPlane;

	data << ChunkHeader('RADI', 4);
	data << radiusValue;

	data << ChunkHeader('HITE', 4);
	data << heightValue;

	data << ChunkHeader('OFST', 4);
	data << offsetValue;

	data << ChunkHeader('DOZR', 4);
	data << bulldozerValue;

	data << ChunkHeader('BLND', 4);
	data << blendValue;

	data << ChunkHeader('FUZZ', 4);
	data << fuzzyValue;

	data << ChunkHeader('STYL', 4);
	data << stylusValue;

	data << ChunkHeader('MBOX', 4);
	data << materialValue;

	data << ChunkHeader('MATL', sizeof(Color3C) * 2);
	data << terrainMaterial.primaryMaterial;
	data << terrainMaterial.secondaryMaterial;

	data << TerminatorChunk;
}

void TerrainPage::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EditorPage::Unpack(data, unpackFlags);
	UnpackChunkList<TerrainPage>(data, unpackFlags);
}

bool TerrainPage::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BRSH':

			data >> currentBrush;
			return (true);

		case 'SLOP':

			data >> currentSlope;
			return (true);

		case 'PLAN':

			data >> currentPlane;
			return (true);

		case 'RADI':

			data >> radiusValue;
			return (true);

		case 'HITE':

			data >> heightValue;
			return (true);

		case 'OFST':

			data >> offsetValue;
			return (true);

		case 'DOZR':

			data >> bulldozerValue;
			return (true);

		case 'BLND':

			data >> blendValue;
			return (true);

		case 'FUZZ':

			data >> fuzzyValue;
			return (true);

		case 'STYL':

			data >> stylusValue;
			return (true);

		case 'MBOX':

			data >> materialValue;
			return (true);

		case 'MATL':

			data >> terrainMaterial.primaryMaterial;
			data >> terrainMaterial.secondaryMaterial;
			return (true);
	}

	return (false);
}

void TerrainPage::Preprocess(void)
{
	static const char *const sculptIdentifier[kEditorTerrainSculptCount] =
	{
		"Add", "Subtract", "Min", "Max", "Smooth"
	};

	static const char *const paintIdentifier[kEditorTerrainPaintCount] =
	{
		"Blend1", "Blend2", "Material", "Hole"
	};

	static const char *const brushIdentifier[kEditorTerrainBrushCount] =
	{
		"Sphere", "Cylinder", "Slope"
	};

	static const char *const slopeIdentifier[kEditorTerrainSlopeCount] =
	{
		"Curve1", "Curve2", "Curve3", "Curve4"
	};

	Editor *editor = GetEditor();
	editor->AddObserver(&editorObserver);

	terrainPaintWidget[0] = new TerrainTextureWidget(0);
	terrainPaintWidget[0]->SetWidgetPosition(Point3D(44.0F, 494.0F, 0.0F));
	AppendSubnode(terrainPaintWidget[0]);

	terrainPaintWidget[1] = new TerrainTextureWidget(1);
	terrainPaintWidget[1]->SetWidgetPosition(Point3D(44.0F, 576.0F, 0.0F));
	AppendSubnode(terrainPaintWidget[1]);

	EditorPage::Preprocess();

	blockButton = static_cast<IconButtonWidget *>(FindWidget("Block"));
	pickupButton = static_cast<IconButtonWidget *>(FindWidget("Pickup"));

	blockButton->SetObserver(&toolObserver);
	pickupButton->SetObserver(&toolObserver);

	for (machine a = 0; a < kEditorTerrainSculptCount; a++)
	{
		sculptButton[a] = static_cast<IconButtonWidget *>(FindWidget(sculptIdentifier[a]));
		sculptButton[a]->SetObserver(&toolObserver);
	}

	for (machine a = 0; a < kEditorTerrainPaintCount; a++)
	{
		paintButton[a] = static_cast<IconButtonWidget *>(FindWidget(paintIdentifier[a]));
		paintButton[a]->SetObserver(&toolObserver);
	}

	for (machine a = 0; a < kEditorTerrainBrushCount; a++)
	{
		brushButton[a] = static_cast<IconButtonWidget *>(FindWidget(brushIdentifier[a]));
		brushButton[a]->SetObserver(&brushObserver);
	}

	for (machine a = 0; a < kEditorTerrainSlopeCount; a++)
	{
		slopeButton[a] = static_cast<IconButtonWidget *>(FindWidget(slopeIdentifier[a]));
		slopeButton[a]->SetObserver(&slopeObserver);
	}

	planeMenu = static_cast<PopupMenuWidget *>(FindWidget("Plane"));
	planeMenu->SetObserver(&planeObserver);

	stylusWidget = static_cast<CheckWidget *>(FindWidget("Stylus"));
	materialWidget = static_cast<CheckWidget *>(FindWidget("Update"));

	stylusWidget->SetObserver(&checkObserver);
	materialWidget->SetObserver(&checkObserver);

	radiusSlider = static_cast<SliderWidget *>(FindWidget("Radius"));
	radiusText = static_cast<TextWidget *>(FindWidget("RadiusText"));

	heightSlider = static_cast<SliderWidget *>(FindWidget("Height"));
	heightText = static_cast<TextWidget *>(FindWidget("HeightText"));

	offsetSlider = static_cast<SliderWidget *>(FindWidget("Offset"));
	offsetText = static_cast<TextWidget *>(FindWidget("OffsetText"));

	bulldozerSlider = static_cast<SliderWidget *>(FindWidget("Dozer"));
	bulldozerText = static_cast<TextWidget *>(FindWidget("DozerText"));

	blendSlider = static_cast<SliderWidget *>(FindWidget("BlendValue"));
	blendText = static_cast<TextWidget *>(FindWidget("BlendText"));

	fuzzySlider = static_cast<SliderWidget *>(FindWidget("FuzzyValue"));
	fuzzyText = static_cast<TextWidget *>(FindWidget("FuzzyText"));

	radiusSlider->SetObserver(&sliderObserver);
	heightSlider->SetObserver(&sliderObserver);
	offsetSlider->SetObserver(&sliderObserver);
	bulldozerSlider->SetObserver(&sliderObserver);
	blendSlider->SetObserver(&sliderObserver);
	fuzzySlider->SetObserver(&sliderObserver);

	planeMenu->SetSelection(currentPlane);
	stylusWidget->SetValue(stylusValue);
	materialWidget->SetValue(materialValue);

	brushButton[currentBrush]->SetValue(1);
	slopeButton[currentSlope]->SetValue(1);

	radiusSlider->SetValue(radiusValue);
	heightSlider->SetValue(heightValue);
	offsetSlider->SetValue(offsetValue);
	bulldozerSlider->SetValue(bulldozerValue);
	blendSlider->SetValue(blendValue);
	fuzzySlider->SetValue(fuzzyValue);

	UpdateSlider(radiusSlider);
	UpdateSlider(heightSlider);
	UpdateSlider(offsetSlider);
	UpdateSlider(bulldozerSlider);
	UpdateSlider(blendSlider);
	UpdateSlider(fuzzySlider);

	for (machine a = 0; a < 2; a++)
	{
		terrainPaintWidget[a]->SetTerrainMaterial(&terrainMaterial);
	}

	UpdateMaterial(editor->GetSelectedMaterial()->GetMaterialObject());

	menuButton = static_cast<IconButtonWidget *>(FindWidget("Menu"));
	menuButton->SetObserver(&menuButtonObserver);

	const StringTable *table = TheWorldEditor->GetStringTable();

	MenuItemWidget *widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageTerrain, 'OGEO')), WidgetObserver<TerrainPage>(this, &TerrainPage::HandleOptimizeTerrainMenuItemEvent));
	terrainMenuItem[kTerrainMenuOptimizeTerrain] = widget;
	terrainMenuItemList.Append(widget);

	terrainMenuItemList.Append(new MenuItemWidget(kLineSolid));

	widget = new MenuItemWidget(table->GetString(StringID('PAGE', kEditorPageTerrain, 'RBLD')), WidgetObserver<TerrainPage>(this, &TerrainPage::HandleRebuildTerrainBlockMenuItemEvent));
	terrainMenuItem[kTerrainMenuRebuildTerrainBlock] = widget;
	terrainMenuItemList.Append(widget);
}

void TerrainPage::UpdateSlider(SliderWidget *widget)
{
	if (widget == radiusSlider)
	{
		radiusText->SetText(Text::FloatToString(GetBrushRadius()));
	}
	else if (widget == heightSlider)
	{
		int32 height = (int32) (GetBrushHeight() * 100.0F + 0.5F);
		heightText->SetText(String<15>(height) += '%');
	}
	else if (widget == offsetSlider)
	{
		int32 offset = (int32) (GetBrushOffset() * 100.0F + 0.5F);
		if (offset == 0)
		{
			offsetText->SetText("0%");
		}
		else if (offset > 0)
		{
			offsetText->SetText((String<15>("+") += offset) += '%');
		}
		else
		{
			offsetText->SetText((String<15>("\xE2\x88\x92") += 1 - offset) += '%');	// U+2212
		}
	}
	else if (widget == bulldozerSlider)
	{
		int32 bias = GetBulldozerBias();
		if (bias == 0)
		{
			bulldozerText->SetText("0");
		}
		else if (bias > 0)
		{
			bulldozerText->SetText(String<15>("+") += bias);
		}
		else
		{
			bulldozerText->SetText(String<15>("\xE2\x88\x92") += -bias);	// U+2212
		}
	}
	else if (widget == blendSlider)
	{
		int32 blend = (int32) (GetTextureBlend() * 100.0F + 0.5F);
		blendText->SetText(String<15>(blend) += '%');
	}
	else if (widget == fuzzySlider)
	{
		int32 fuzzy = (int32) (GetBlendFuzziness() * 100.0F + 0.5F);
		fuzzyText->SetText(String<15>(fuzzy) += '%');
	}
}

void TerrainPage::UpdateMaterial(const MaterialObject *materialObject)
{
	const char *textureName = nullptr;

	const Attribute *attribute = materialObject->GetFirstAttribute();
	while (attribute)
	{
		AttributeType attributeType = attribute->GetAttributeType();
		if (attributeType == kAttributeDiffuseTexture)
		{
			textureName = static_cast<const DiffuseTextureAttribute *>(attribute)->GetTextureName();
			break;
		}
		else if (attributeType == kAttributeShader)
		{
			const ShaderGraph *graph = static_cast<const ShaderAttribute *>(attribute)->GetShaderGraph();
			const Process *process = graph->GetFirstElement();
			while (process)
			{
				if (process->GetProcessType() == kProcessTerrainTexture)
				{
					textureName = static_cast<const TerrainTextureProcess *>(process)->GetTextureName();
					break;
				}

				process = process->GetNextElement();
			}

			break;
		}

		attribute = attribute->Next();
	}

	for (machine a = 0; a < 2; a++)
	{
		terrainPaintWidget[a]->SetTexture(textureName);
	}
}

void TerrainPage::HandleEditorEvent(Editor *editor, const EditorEvent& event)
{
	EditorEventType type = event.GetEventType();
	if ((type == kEditorEventMaterialSelected) || (type == kEditorEventMaterialModified))
	{
		UpdateMaterial(static_cast<const MaterialEditorEvent *>(&event)->GetEventMaterialObject());
	}
}

void TerrainPage::HandleToolButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		GetEditor()->SetCurrentTool(this, widget);
	}
}

void TerrainPage::HandleBrushEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		for (machine a = 0; a < kEditorTerrainBrushCount; a++)
		{
			if (widget == brushButton[a])
			{
				currentBrush = a;
			}
			else
			{
				brushButton[a]->SetValue(0);
			}
		}
	}
}

void TerrainPage::HandleSlopeEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		for (machine a = 0; a < kEditorTerrainSlopeCount; a++)
		{
			if (widget == slopeButton[a])
			{
				currentSlope = a;
			}
			else
			{
				slopeButton[a]->SetValue(0);
			}
		}
	}
}

void TerrainPage::HandlePlaneEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		currentPlane = static_cast<PopupMenuWidget *>(widget)->GetSelection();
	}
}

void TerrainPage::HandleSliderEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SliderWidget *sliderWidget = static_cast<SliderWidget *>(widget);
		int32 value = sliderWidget->GetValue();

		if (sliderWidget == radiusSlider)
		{
			radiusValue = value;
		}
		else if (sliderWidget == heightSlider)
		{
			heightValue = value;
		}
		else if (sliderWidget == offsetSlider)
		{
			offsetValue = value;
		}
		else if (sliderWidget == bulldozerSlider)
		{
			bulldozerValue = value;
		}
		else if (sliderWidget == blendSlider)
		{
			blendValue = value;
		}
		else if (sliderWidget == fuzzySlider)
		{
			fuzzyValue = value;
		}

		UpdateSlider(static_cast<SliderWidget *>(widget));
	}
}

void TerrainPage::HandleCheckEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		CheckWidget *checkWidget = static_cast<CheckWidget *>(widget);
		int32 value = checkWidget->GetValue();

		if (checkWidget == stylusWidget)
		{
			stylusValue = value;
		}
		else if (checkWidget == materialWidget)
		{
			materialValue = value;
		}
	}
}

void TerrainPage::HandleMenuButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		int32 geometryCount = 0;
		int32 blockCount = 0;

		const NodeReference *reference = GetEditor()->GetFirstSelectedNode();
		while (reference)
		{
			const Node *node = reference->GetNode();
			NodeType type = node->GetNodeType();

			if (type == kNodeGeometry)
			{
				if (static_cast<const Geometry *>(node)->GetGeometryType() == kGeometryTerrain)
				{
					geometryCount++;
				}
			}
			else if (type == kNodeTerrainBlock)
			{
				blockCount++;
			}

			reference = reference->Next();
		}

		if (geometryCount != 0)
		{
			terrainMenuItem[kTerrainMenuOptimizeTerrain]->Enable();
		}
		else
		{
			terrainMenuItem[kTerrainMenuOptimizeTerrain]->Disable();
		}

		if (blockCount == 1)
		{
			terrainMenuItem[kTerrainMenuRebuildTerrainBlock]->Enable();
		}
		else
		{
			terrainMenuItem[kTerrainMenuRebuildTerrainBlock]->Disable();
		}

		Menu *menu = new Menu(kMenuContextual, &terrainMenuItemList);
		menu->SetWidgetPosition(menuButton->GetWorldPosition() + Vector3D(25.0F, 0.0F, 0.0F));
		TheInterfaceMgr->SetActiveMenu(menu);
	}
}

void TerrainPage::HandleOptimizeTerrainMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();
	editor->AddSubwindow(new OptimizeTerrainWindow(editor));
}

void TerrainPage::HandleRebuildTerrainBlockMenuItemEvent(Widget *menuItem, const WidgetEventData *eventData)
{
	Editor *editor = GetEditor();

	const NodeReference *reference = editor->GetFirstSelectedNode();
	while (reference)
	{
		Node *node = reference->GetNode();

		if (node->GetNodeType() == kNodeTerrainBlock)
		{
			editor->AddSubwindow(new RebuildTerrainBlockWindow(editor, static_cast<TerrainBlock *>(node)));
			break;
		}

		reference = reference->Next();
	}
}

void TerrainPage::Engage(Editor *editor, void *cookie)
{
	Widget *widget = static_cast<Widget *>(cookie);

	if (widget == blockButton)
	{
		currentMode = kEditorTerrainModeBlock;
		currentTool = 0;
		blockButton->SetValue(1);

		editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
		return;
	}

	for (machine a = 0; a < kEditorTerrainSculptCount; a++)
	{
		if (widget == sculptButton[a])
		{
			currentMode = kEditorTerrainModeSculpt;
			currentTool = a;
			sculptButton[a]->SetValue(1);

			editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
			return;
		}
	}

	for (machine a = 0; a < kEditorTerrainPaintCount; a++)
	{
		if (widget == paintButton[a])
		{
			currentMode = kEditorTerrainModePaint;
			currentTool = a;
			paintButton[a]->SetValue(1);

			editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorCross));
			return;
		}
	}

	if (widget == pickupButton)
	{
		currentMode = kEditorTerrainModePickup;
		currentTool = 0;
		pickupButton->SetValue(1);

		editor->SetCurrentCursor(TheWorldEditor->GetEditorCursor(kEditorCursorDropper));
		return;
	}
}

void TerrainPage::Disengage(Editor *editor, void *cookie)
{
	if (currentTool != -1)
	{
		if (currentMode == kEditorTerrainModeBlock)
		{
			blockButton->SetValue(0);
		}
		else if (currentMode == kEditorTerrainModeSculpt)
		{
			sculptButton[currentTool]->SetValue(0);
		}
		else if (currentMode == kEditorTerrainModePaint)
		{
			paintButton[currentTool]->SetValue(0);
		}
		else
		{
			pickupButton->SetValue(0);
		}

		currentTool = -1;
	}
}

bool TerrainPage::BeginTool(Editor *editor, EditorTrackData *trackData)
{
	if (trackData->viewportType != kEditorViewportGraph)
	{
		if (currentMode == kEditorTerrainModeBlock)
		{
			if (trackData->viewportType == kEditorViewportOrtho)
			{
				TerrainBlock *block = new TerrainBlock(Integer3D(1, 1, 1), 0.0F, &terrainMaterial);
				editor->InitNewNode(trackData, block);
				return (true);
			}
		}
		else if ((currentMode == kEditorTerrainModeSculpt) || (currentMode == kEditorTerrainModePaint))
		{
			CollisionData	collisionData;

			Point3D p1 = trackData->worldRay.origin + trackData->worldRay.direction * trackData->worldRay.tmin;
			Point3D p2 = trackData->worldRay.origin + trackData->worldRay.direction * trackData->worldRay.tmax;
			if (editor->GetEditorWorld()->DetectCollision(p1, p2, 0.0F, kCollisionCamera, &collisionData))
			{
				const Geometry *geometry = collisionData.geometry;
				if (geometry->GetGeometryType() == kGeometryTerrain)
				{
					const TerrainGeometry *terrain = static_cast<const TerrainGeometry *>(geometry);
					TerrainBlock *block = terrain->GetBlockNode();
					if (block)
					{
						drawingBlock = block;
						drawingChannel = kTerrainSubchannelDensity;
						if (currentMode != kEditorTerrainModeSculpt)
						{
							drawingChannel = (currentTool <= kEditorTerrainPaintBlend2) ? kTerrainSubchannelBlend : kTerrainSubchannelMaterial;
						}

						drawingRadius = GetBrushRadius();
						drawingHeight = (currentBrush == kEditorTerrainBrushSphere) ? drawingRadius : drawingRadius * GetBrushHeight();
						drawingOffset = drawingHeight * GetBrushOffset();

						switch (currentPlane)
						{
							case kEditorTerrainPlaneNone:
							case kEditorTerrainPlaneTangent:

								drawingPlane.Set(collisionData.normal.Normalize(), collisionData.position);
								break;

							case kEditorTerrainPlaneHorizontal:

								drawingPlane.Set((collisionData.normal.z >= 0.0F) ? K::z_unit : K::minus_z_unit, collisionData.position);
								break;

							case kEditorTerrainPlaneCamera:

								drawingPlane.Set(-trackData->viewportCamera->GetNodeTransform()[2], collisionData.position);
								break;
						}

						if (currentBrush != kEditorTerrainBrushSlope)
						{
							brushNormal = drawingPlane.GetAntivector3D();
						}
						else
						{
							brushNormal = (drawingPlane.z >= 0.0F) ? K::z_unit : K::minus_z_unit;
						}

						float scale = block->GetObject()->GetVoxelScale();
						previousPoint = collisionData.position + brushNormal * (drawingOffset * scale);

						TerrainBlockManipulator *manipulator = static_cast<TerrainBlockManipulator *>(Editor::GetManipulator(block));
						manipulator->SetRenderTool(currentMode, currentTool, currentBrush);
						manipulator->BuildBrush(drawingRadius * scale, drawingHeight * scale);

						if (currentBrush != kEditorTerrainBrushSlope)
						{
							manipulator->SetBrushTransform(previousPoint, drawingPlane.GetAntivector3D());
						}
						else
						{
							manipulator->SetBrushTransform(previousPoint, K::z_unit);
						}

						paintOperation = new TerrainPaintOperation(block, drawingChannel, (drawingChannel != kTerrainSubchannelMaterial) ? kTerrainSubchannelMaterial : 0);
						editor->AddOperation(paintOperation);

						DrawTerrain(editor, previousPoint, previousPoint);
						return (true);
					}
				}
			}
		}
		else if (currentMode == kEditorTerrainModePickup)
		{
			PickData	pickData;

			auto filter = [](const Node *node, const PickData *, const void *) -> bool
			{
				return (node->GetNodeType() == kNodeGeometry);
			};

			const Node *node = editor->PickNode(trackData, &pickData, filter);
			if (node)
			{
				const Geometry *geometry = static_cast<const Geometry *>(node);
				if (geometry->GetGeometryType() == kGeometryTerrain)
				{
					const Mesh *mesh = geometry->GetObject()->GetGeometryLevel(0);

					const Triangle *triangle = mesh->GetArray<Triangle>(kArrayPrimitive);
					unsigned_int32 vertexIndex = triangle[pickData.triangleIndex].index[0];

					const Color4C *color0 = mesh->GetArray<Color4C>(kArrayColor0);
					const Color4C *color1 = mesh->GetArray<Color4C>(kArrayColor1);

					if ((color0) && (color1))
					{
						const Color4C& c0 = color0[vertexIndex];
						const Color4C& c1 = color1[vertexIndex];

						terrainMaterial.primaryMaterial.Set(c1.GetRed(), c1.GetGreen(), c0.GetRed());
						terrainMaterial.secondaryMaterial.Set(c1.GetBlue(), c1.GetAlpha(), c0.GetGreen());
					}

					for (machine a = 0; a < 2; a++)
					{
						terrainPaintWidget[a]->SetTerrainMaterial(&terrainMaterial);
					}
				}
			}
		}
	}

	return (false);
}

bool TerrainPage::TrackTool(Editor *editor, EditorTrackData *trackData)
{
	editor->AutoScroll(trackData);

	if (currentMode == kEditorTerrainModeBlock)
	{
		Point2D anchor = trackData->snappedAnchorPosition;
		float dx = trackData->snappedCurrentPosition.x - anchor.x;
		float dy = anchor.y - trackData->snappedCurrentPosition.y;

		float ax = Fabs(dx);
		float ay = Fabs(dy);

		ax = ay = Fmax(ax, ay);
		dx = (dx < 0.0F) ? -ax : ax;
		dy = (dy < 0.0F) ? -ax : ax;

		if (dx != trackData->currentSize.x)
		{
			trackData->currentSize.Set(dx, dy);

			unsigned_int32 editorFlags = editor->GetEditorObject()->GetEditorFlags();
			if (editorFlags & kEditorDrawFromCenter)
			{
				anchor.x -= ax;
				anchor.y += ay;
				dx = ax * 2.0F;
			}
			else
			{
				if (dx < 0.0F)
				{
					anchor.x += dx;
					dx = -dx;
				}

				if (dy < 0.0F)
				{
					anchor.y -= dy;
				}
			}

			TerrainBlock *block = static_cast<TerrainBlock *>(trackData->trackNode);
			block->SetNodePosition(editor->GetTargetSpacePosition(trackData, anchor));
			block->GetObject()->SetVoxelScale(dx / (float) kTerrainDimension);

			editor->InvalidateNode(block);
		}

		return (dx != 0.0F);
	}
	else
	{
		Point3D		currentPoint;

		editor->InvalidateAllViewports();

		float offset = drawingOffset;
		if ((currentMode == kEditorTerrainModeSculpt) && (stylusValue != 0))
		{
			if (currentTool == kEditorTerrainSculptAdd)
			{
				offset += (drawingHeight - drawingOffset) * TheEngine->GetStylusPressure();
			}
			else if (currentTool == kEditorTerrainSculptSubtract)
			{
				offset -= (drawingOffset + drawingHeight) * TheEngine->GetStylusPressure();
			}
		}

		offset *= drawingBlock->GetObject()->GetVoxelScale();

		if (currentPlane != kEditorTerrainPlaneNone)
		{
			float m = drawingPlane ^ trackData->worldRay.direction;
			if (m == 0.0F)
			{
				return (true);
			}

			float t = -(drawingPlane ^ trackData->worldRay.origin) / m;
			if ((t < trackData->worldRay.tmin) || (t > trackData->worldRay.tmax))
			{
				return (true);
			}

			currentPoint = trackData->worldRay.origin + trackData->worldRay.direction * t + brushNormal * offset;
		}
		else
		{
			Ray				blockRay;
			CollisionPoint	collisionPoint;

			const Transform4D& transform = drawingBlock->GetInverseWorldTransform();
			blockRay.origin = transform * trackData->worldRay.origin;
			blockRay.direction = transform * trackData->worldRay.direction;
			blockRay.radius = 0.0F;
			blockRay.tmin = trackData->worldRay.tmin;
			blockRay.tmax = trackData->worldRay.tmax;

			if (!DetectTerrainIntersection(drawingBlock, currentMode, &blockRay, &collisionPoint))
			{
				return (true);
			}

			drawingPlane.Set(collisionPoint.normal.Normalize(), collisionPoint.position);

			if (currentBrush != kEditorTerrainBrushSlope)
			{
				brushNormal = collisionPoint.normal;
			}
			else
			{
				brushNormal = (collisionPoint.normal.z >= 0.0F) ? K::z_unit : K::minus_z_unit;
			}

			currentPoint = drawingBlock->GetWorldTransform() * (collisionPoint.position + brushNormal * offset);
		}

		TerrainBlockManipulator *manipulator = static_cast<TerrainBlockManipulator *>(Editor::GetManipulator(drawingBlock));
		if (currentBrush != kEditorTerrainBrushSlope)
		{
			manipulator->SetBrushTransform(currentPoint, drawingPlane.GetAntivector3D());
		}
		else
		{
			manipulator->SetBrushTransform(currentPoint, K::z_unit);
		}

		DrawTerrain(editor, previousPoint, currentPoint);
		previousPoint = currentPoint;
	}

	return (true);
}

bool TerrainPage::EndTool(Editor *editor, EditorTrackData *trackData)
{
	bool result = TrackTool(editor, trackData);
	if (currentMode == kEditorTerrainModeBlock)
	{
		editor->CommitNewNode(trackData, result);
		if (result)
		{
			editor->AddSubwindow(new BuildTerrainWindow(editor, static_cast<TerrainBlock *>(trackData->trackNode)));
		}
	}
	else
	{
		static_cast<TerrainBlockManipulator *>(Editor::GetManipulator(drawingBlock))->SetRenderTool(-1, -1, -1);

		if (currentPlane == kEditorTerrainPlaneNone)
		{
			for (;;)
			{
				TerrainGeometryManipulator *manipulator = terrainPaintList.First();
				if (!manipulator)
				{
					break;
				}

				manipulator->CleanupPaintMesh();
			}
		}
	}

	return (true);
}

bool TerrainPage::DetectTerrainIntersection(const Node *node, int32 mode, Ray *ray, CollisionPoint *collisionPoint) const
{
	const Node *subnode = node->GetFirstSubnode();
	if (subnode)
	{
		bool result = false;
		do
		{
			result |= DetectTerrainIntersection(subnode, mode, ray, collisionPoint);
			subnode = subnode->Next();
		} while (subnode);

		return (result);
	}
	else if (node->GetNodeType() == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<const Geometry *>(node);
		if (geometry->GetGeometryType() == kGeometryTerrain)
		{
			GeometryHitData		geometryHitData;

			const TerrainGeometryObject *object = static_cast<const TerrainGeometry *>(geometry)->GetObject();
			bool modifiedFlag = paintOperation->TerrainModified(object->GetGeometryCoord(), object->GetDetailLevel());
			if (static_cast<TerrainGeometryManipulator *>(node->GetManipulator())->GetPaintPosition(ray, mode, modifiedFlag, &geometryHitData))
			{
				ray->tmax = geometryHitData.param;
				collisionPoint->param = geometryHitData.param;
				collisionPoint->position = geometryHitData.position;
				collisionPoint->normal = geometryHitData.normal;
				return (true);
			}
		}
	}

	return (false);
}

void TerrainPage::DrawTerrain(Editor *editor, const Point3D& endpoint1, const Point3D& endpoint2)
{
	Antivector4D	topPlane;
	Antivector4D	bottomPlane;
	int32			dh;

	float scale = 1.0F / drawingBlock->GetObject()->GetVoxelScale();
	Point3D p1 = drawingBlock->GetInverseWorldTransform() * endpoint1 * scale;
	Point3D p2 = drawingBlock->GetInverseWorldTransform() * endpoint2 * scale;

	float xmin = Fmin(p1.x, p2.x);
	float xmax = Fmax(p1.x, p2.x);
	float ymin = Fmin(p1.y, p2.y);
	float ymax = Fmax(p1.y, p2.y);
	float zmin = Fmin(p1.z, p2.z);
	float zmax = Fmax(p1.z, p2.z);

	int32 dr = (int32) PositiveCeil(drawingRadius) + 1;

	if (currentBrush == kEditorTerrainBrushCylinder)
	{
		float pz = drawingPlane.z;
		float height = drawingHeight * Fabs(pz) + drawingRadius * Sqrt(1.0F - pz * pz);
		dh = (int32) PositiveCeil(height) + 1;

		bottomPlane.Set(drawingPlane.GetAntivector3D() * drawingBlock->GetWorldTransform(), p1);
		topPlane.Set(bottomPlane.GetAntivector3D(), bottomPlane.w - drawingHeight);
		bottomPlane.w += drawingHeight;
	}
	else if (currentBrush == kEditorTerrainBrushSlope)
	{
		dh = (int32) PositiveCeil(drawingHeight) + 1;
	}
	else
	{
		dh = dr;
	}

	Integer3D vmin((int32) xmin - dr, (int32) ymin - dr, (int32) zmin - dh);
	Integer3D vmax((int32) xmax + (dr + 1), (int32) ymax + (dr + 1), (int32) zmax + (dh + 1));

	const Integer3D& size = drawingBlock->GetBlockSize();

	int32 imin = MaxZero((vmin.x - 1) >> kTerrainLogDimension);
	int32 imax = Min((vmax.x + 1) >> kTerrainLogDimension, size.x - 1);
	int32 jmin = MaxZero((vmin.y - 1) >> kTerrainLogDimension);
	int32 jmax = Min((vmax.y + 1) >> kTerrainLogDimension, size.y - 1);
	int32 kmin = MaxZero((vmin.z - 1) >> kTerrainLogDimension);
	int32 kmax = Min((vmax.z + 1) >> kTerrainLogDimension, size.z - 1);

	if ((currentPlane == kEditorTerrainPlaneNone) && (currentMode == kEditorTerrainModeSculpt))
	{
		for (machine k = kmin; k <= kmax; k++)
		{
			for (machine j = jmin; j <= jmax; j++)
			{
				for (machine i = imin; i <= imax; i++)
				{
					Integer3D coord(i, j, k);

					if (paintOperation->AddTerrain(coord, 0))
					{
						const TerrainGeometry *geometry = drawingBlock->FindTerrainGeometry(coord);
						if (geometry)
						{
							static_cast<TerrainGeometryManipulator *>(geometry->GetManipulator())->CopyTriangleMesh(&terrainPaintList);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = kmin; k <= kmax; k++)
		{
			for (machine j = jmin; j <= jmax; j++)
			{
				for (machine i = imin; i <= imax; i++)
				{
					paintOperation->AddTerrain(Integer3D(i, j, k), 0);
				}
			}
		}
	}

	for (machine a = 1; a < kTerrainEditLevelCount; a++)
	{
		int32 ds = 1 << a;
		int32 mask = ~(ds - 1);

		for (machine k = kmin & mask; k <= kmax; k += ds)
		{
			for (machine j = jmin & mask; j <= jmax; j += ds)
			{
				for (machine i = imin & mask; i <= imax; i += ds)
				{
					paintOperation->AddTerrain(Integer3D(i, j, k), a);
				}
			}
		}
	}

	VoxelMap *drawingMap = drawingBlock->OpenVoxelMap(drawingChannel, vmin, vmax);
	VoxelMap *materialMap = nullptr;
	UnsignedVoxel material = 0;

	if ((GetMaterialUpdateState()) || (drawingChannel == kTerrainSubchannelMaterial))
	{
		if ((currentMode != kEditorTerrainModePaint) || (currentTool != kEditorTerrainPaintHole))
		{
			int32 materialIndex = drawingBlock->GetTerrainMaterialIndex(&terrainMaterial);
			if (materialIndex < 0)
			{
				drawingBlock->OptimizeTerrainMaterials();
				materialIndex = MaxZero(drawingBlock->GetTerrainMaterialIndex(&terrainMaterial));
			}

			material = (UnsignedVoxel) materialIndex;
		}
		else
		{
			material = kDeadTerrainMaterialIndex;
		}

		if (drawingChannel != kTerrainSubchannelMaterial)
		{
			materialMap = drawingBlock->OpenVoxelMap(kTerrainSubchannelMaterial, vmin, vmax);
		}
	}

	if (drawingChannel == kTerrainSubchannelBlend)
	{
		UnsignedVoxel blend = (UnsignedVoxel) (GetTextureBlend() * 255.0F + 0.5F);
		float fuzzy = GetBlendFuzziness();

		if (currentTool == kEditorTerrainPaintBlend1)
		{
			if (currentBrush == kEditorTerrainBrushSphere)
			{
				DrawSphericalMinPaintBrush(drawingMap, materialMap, p1, p2, fuzzy, blend, material);
			}
			else if (currentBrush == kEditorTerrainBrushCylinder)
			{
				DrawCylindricalMinPaintBrush(drawingMap, materialMap, p1, p2, topPlane, bottomPlane, fuzzy, blend, material);
			}
		}
		else
		{
			if (currentBrush == kEditorTerrainBrushSphere)
			{
				DrawSphericalMaxPaintBrush(drawingMap, materialMap, p1, p2, fuzzy, blend, material);
			}
			else if (currentBrush == kEditorTerrainBrushCylinder)
			{
				DrawCylindricalMaxPaintBrush(drawingMap, materialMap, p1, p2, topPlane, bottomPlane, fuzzy, blend, material);
			}
		}
	}
	else if (drawingChannel == kTerrainSubchannelMaterial)
	{
		if (currentBrush == kEditorTerrainBrushSphere)
		{
			DrawSphericalMaterialBrush(drawingMap, p1, p2, material);
		}
		else if (currentBrush == kEditorTerrainBrushCylinder)
		{
			DrawCylindricalMaterialBrush(drawingMap, p1, p2, topPlane, bottomPlane, material);
		}
	}
	else
	{
		if (currentTool == kEditorTerrainSculptAdd)
		{
			if (currentBrush == kEditorTerrainBrushSphere)
			{
				DrawSphericalAdditiveBrush(drawingMap, materialMap, p1, p2, material);
			}
			else if (currentBrush == kEditorTerrainBrushCylinder)
			{
				DrawCylindricalAdditiveBrush(drawingMap, materialMap, p1, p2, topPlane, bottomPlane, material);
			}
			else if (currentBrush == kEditorTerrainBrushSlope)
			{
				DrawSlopeAdditiveBrush(drawingMap, materialMap, p1, p2, material);
			}
		}
		else if (currentTool == kEditorTerrainSculptSubtract)
		{
			if (currentBrush == kEditorTerrainBrushSphere)
			{
				DrawSphericalSubtractiveBrush(drawingMap, materialMap, p1, p2, material);
			}
			else if (currentBrush == kEditorTerrainBrushCylinder)
			{
				DrawCylindricalSubtractiveBrush(drawingMap, materialMap, p1, p2, topPlane, bottomPlane, material);
			}
			else if (currentBrush == kEditorTerrainBrushSlope)
			{
				DrawSlopeSubtractiveBrush(drawingMap, materialMap, p1, p2, material);
			}
		}
		else if (currentTool == kEditorTerrainSculptMin)
		{
			VoxelMap *savedVoxelMap = paintOperation->OpenSavedVoxelMap(vmin, vmax);

			if (currentBrush == kEditorTerrainBrushSphere)
			{
				DrawSphericalMinBrush(drawingMap, materialMap, savedVoxelMap, p1, p2, material);
			}
			else if (currentBrush == kEditorTerrainBrushCylinder)
			{
				DrawCylindricalMinBrush(drawingMap, materialMap, savedVoxelMap, p1, p2, topPlane, bottomPlane, material);
			}

			TerrainPaintOperation::CloseSavedVoxelMap(savedVoxelMap);
		}
		else if (currentTool == kEditorTerrainSculptMax)
		{
			VoxelMap *savedVoxelMap = paintOperation->OpenSavedVoxelMap(vmin, vmax);

			if (currentBrush == kEditorTerrainBrushSphere)
			{
				DrawSphericalMaxBrush(drawingMap, materialMap, savedVoxelMap, p1, p2, material);
			}
			else if (currentBrush == kEditorTerrainBrushCylinder)
			{
				DrawCylindricalMaxBrush(drawingMap, materialMap, savedVoxelMap, p1, p2, topPlane, bottomPlane, material);
			}

			TerrainPaintOperation::CloseSavedVoxelMap(savedVoxelMap);
		}
		else if (currentTool == kEditorTerrainSculptSmooth)
		{
			VoxelMap *savedVoxelMap = paintOperation->OpenSavedVoxelMap(vmin, vmax);

			if (currentBrush == kEditorTerrainBrushSphere)
			{
				DrawSphericalSmoothingBrush(drawingMap, materialMap, savedVoxelMap, p1, p2, material);
			}
			else if (currentBrush == kEditorTerrainBrushCylinder)
			{
				DrawCylindricalSmoothingBrush(drawingMap, materialMap, savedVoxelMap, p1, p2, topPlane, bottomPlane, material);
			}

			TerrainPaintOperation::CloseSavedVoxelMap(savedVoxelMap);
		}
	}

	if (materialMap)
	{
		drawingBlock->CloseVoxelMap(kTerrainSubchannelMaterial, materialMap);
	}

	drawingBlock->CloseVoxelMap(drawingChannel, drawingMap);

	MaterialObject *materialObject = editor->GetSelectedMaterial()->GetMaterialObject();
	for (machine a = 0; a < kTerrainEditLevelCount; a++)
	{
		int32 ds = 1 << a;
		int32 mask = ~(ds - 1);

		for (machine k = kmin & mask; k <= kmax; k += ds)
		{
			for (machine j = jmin & mask; j <= jmax; j += ds)
			{
				for (machine i = imin & mask; i <= imax; i += ds)
				{
					UpdateBlock(editor, drawingBlock, Integer3D(i, j, k), a, materialObject);
				}
			}
		}
	}

	drawingBlock->ProcessStructure();
}

void TerrainPage::DrawSphericalAdditiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	Voxel *voxel = voxelMap->GetVoxelData();
	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					float d = Magnitude(q) - drawingRadius;
					if (d < 1.0F)
					{
						int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Min(v, voxel[m]);

						if ((materialMap) && (d < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);

					float d = Magnitude(position - p1) - drawingRadius;
					int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
					int32 m = k * deckSize + j * rowSize + i;
					voxel[m] = (Voxel) Min(v, voxel[m]);

					if ((materialMap) && (d < 0.0F))
					{
						materialMap->SetUnsignedVoxelOctet(i, j, k, material);
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalAdditiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material)
{
	Voxel *voxel = voxelMap->GetVoxelData();
	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float d3 = Magnitude(q) - drawingRadius;
					if (d3 < 1.0F)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						float d = Fmax(d1, d2, d3);

						int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Min(v, voxel[m]);

						if ((materialMap) && (d < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float d1 = -(bottomPlane ^ position);
					float d2 = topPlane ^ position;
					float d3 = Magnitude(q - ProjectOnto(q, bottomPlane.GetAntivector3D())) - drawingRadius;
					float d = Fmax(d1, d2, d3);

					int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
					int32 m = k * deckSize + j * rowSize + i;
					voxel[m] = (Voxel) Min(v, voxel[m]);

					if ((materialMap) && (d < 0.0F))
					{
						materialMap->SetUnsignedVoxelOctet(i, j, k, material);
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSlopeAdditiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	Voxel *voxel = voxelMap->GetVoxelData();
	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();

	const float *term = slopeTerm[currentSlope];
	float a = term[0];
	float b = term[1];
	float c = term[2];
	float d = term[3];

	float squaredRadius = drawingRadius * drawingRadius;
	float inverseRadius = 1.0F / drawingRadius;
	float inverseHeight = 1.0F / drawingHeight;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					float r = q.x * q.x + q.y * q.y;
					if (r < squaredRadius)
					{
						r = Sqrt(r) * inverseRadius;
						float h = q.z * inverseHeight * 0.5F + 0.5F;

						float r2 = r * r;
						float f = a * r2 * r + b * r2 + c * r + d;
						float fp = 3.0F * a * r2 + 2.0F * b * r + c;
						fp *= drawingHeight * inverseRadius;

						f = (h - f) * drawingHeight * InverseSqrt(fp * fp + 1.0F);
						int32 v = Min(Max((int32) (f * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Min(v, voxel[m]);

						if ((materialMap) && (f < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float r = q.x * q.x + q.y * q.y;
					if (r < squaredRadius)
					{
						r = Sqrt(r) * inverseRadius;
						float h = q.z * inverseHeight * 0.5F + 0.5F;

						float r2 = r * r;
						float f = a * r2 * r + b * r2 + c * r + d;
						float fp = 3.0F * a * r2 + 2.0F * b * r + c;
						fp *= drawingHeight * inverseRadius;

						f = (h - f) * drawingHeight * InverseSqrt(fp * fp + 1.0F);
						int32 v = Min(Max((int32) (f * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Min(v, voxel[m]);

						if ((materialMap) && (f < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSphericalSubtractiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	Voxel *voxel = voxelMap->GetVoxelData();
	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					float d = Magnitude(q) - drawingRadius;
					if (d < 1.0F)
					{
						int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Max(-v, voxel[m]);

						if ((materialMap) && (d < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);

					float d = Magnitude(position - p1) - drawingRadius;
					int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
					int32 m = k * deckSize + j * rowSize + i;
					voxel[m] = (Voxel) Max(-v, voxel[m]);

					if ((materialMap) && (d < 0.0F))
					{
						materialMap->SetUnsignedVoxelOctet(i, j, k, material);
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalSubtractiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material)
{
	Voxel *voxel = voxelMap->GetVoxelData();
	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float d3 = Magnitude(q) - drawingRadius;
					if (d3 < 1.0F)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						float d = Fmax(d1, d2, d3);

						int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Max(-v, voxel[m]);

						if ((materialMap) && (d < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float d1 = -(bottomPlane ^ position);
					float d2 = topPlane ^ position;
					float d3 = Magnitude(q - ProjectOnto(q, bottomPlane.GetAntivector3D())) - drawingRadius;
					float d = Fmax(d1, d2, d3);

					int32 v = Min(Max((int32) (d * 127.0F), -127), 127);
					int32 m = k * deckSize + j * rowSize + i;
					voxel[m] = (Voxel) Max(-v, voxel[m]);

					if ((materialMap) && (d < 0.0F))
					{
						materialMap->SetUnsignedVoxelOctet(i, j, k, material);
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSlopeSubtractiveBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	Voxel *voxel = voxelMap->GetVoxelData();
	int32 rowSize = voxelMap->GetRowSize();
	int32 deckSize = voxelMap->GetDeckSize();
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();

	const float *term = slopeTerm[currentSlope];
	float a = term[0];
	float b = term[1];
	float c = term[2];
	float d = term[3];

	float squaredRadius = drawingRadius * drawingRadius;
	float inverseRadius = 1.0F / drawingRadius;
	float inverseHeight = 1.0F / drawingHeight;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					float r = q.x * q.x + q.y * q.y;
					if (r < squaredRadius)
					{
						r = Sqrt(r) * inverseRadius;
						float h = q.z * inverseHeight * 0.5F + 0.5F;

						float r2 = r * r;
						float f = a * r2 * r + b * r2 + c * r + d;
						float fp = 3.0F * a * r2 + 2.0F * b * r + c;
						fp *= drawingHeight * inverseRadius;

						f = (h + f - 1.0F) * drawingHeight * InverseSqrt(fp * fp + 1.0F);
						int32 v = Min(Max((int32) (f * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Max(v, voxel[m]);

						if ((materialMap) && (f < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float r = q.x * q.x + q.y * q.y;
					if (r < squaredRadius)
					{
						r = Sqrt(r) * inverseRadius;
						float h = q.z * inverseHeight * 0.5F + 0.5F;

						float r2 = r * r;
						float f = a * r2 * r + b * r2 + c * r + d;
						float fp = 3.0F * a * r2 + 2.0F * b * r + c;
						fp *= drawingHeight * inverseRadius;

						f = (h + f - 1.0F) * drawingHeight * InverseSqrt(fp * fp + 1.0F);
						int32 v = Min(Max((int32) (f * 127.0F), -127), 127);
						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (Voxel) Max(v, voxel[m]);

						if ((materialMap) && (f < 0.0F))
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSphericalMinBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;
	int32 bias = -GetBulldozerBias();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					if (SquaredMag(q) < squaredRadius)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMinHorizontalVoxel(i, j, k, bias * 8));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					if (SquaredMag(position - p1) < squaredRadius)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMinHorizontalVoxel(i, j, k, bias * 8));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalMinBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();
	int32 bias = -GetBulldozerBias();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float d3 = Magnitude(q) - drawingRadius;
					if (d3 < 0.0F)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						float d = Fmax(d1, d2, d3);
						if (d < 0.0F)
						{
							voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMinHorizontalVoxel(i, j, k, bias * 8));
							if (materialMap)
							{
								materialMap->SetUnsignedVoxelOctet(i, j, k, material);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float d1 = -(bottomPlane ^ position);
					float d2 = topPlane ^ position;
					float d3 = Magnitude(q - ProjectOnto(q, bottomPlane.GetAntivector3D())) - drawingRadius;
					float d = Fmax(d1, d2, d3);
					if (d < 0.0F)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMinHorizontalVoxel(i, j, k, bias * 8));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSphericalMaxBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;
	int32 bias = -GetBulldozerBias();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					if (SquaredMag(q) < squaredRadius)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMaxHorizontalVoxel(i, j, k, bias * 8));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					if (SquaredMag(position - p1) < squaredRadius)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMaxHorizontalVoxel(i, j, k, bias * 8));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalMaxBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();
	int32 bias = -GetBulldozerBias();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float d3 = Magnitude(q) - drawingRadius;
					if (d3 < 0.0F)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						float d = Fmax(d1, d2, d3);
						if (d < 0.0F)
						{
							voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMaxHorizontalVoxel(i, j, k, bias * 8));
							if (materialMap)
							{
								materialMap->SetUnsignedVoxelOctet(i, j, k, material);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float d1 = -(bottomPlane ^ position);
					float d2 = topPlane ^ position;
					float d3 = Magnitude(q - ProjectOnto(q, bottomPlane.GetAntivector3D())) - drawingRadius;
					float d = Fmax(d1, d2, d3);
					if (d < 0.0F)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetMaxHorizontalVoxel(i, j, k, bias * 8));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSphericalSmoothingBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					if (SquaredMag(q) < squaredRadius)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetFilteredVoxel(i, j, k));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					if (SquaredMag(position - p1) < squaredRadius)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetFilteredVoxel(i, j, k));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalSmoothingBrush(VoxelMap *voxelMap, VoxelMap *materialMap, const VoxelMap *savedVoxelMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = voxelMap->GetVoxelBounds();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float d3 = Magnitude(q) - drawingRadius;
					if (d3 < 0.0F)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						float d = Fmax(d1, d2, d3);
						if (d < 0.0F)
						{
							voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetFilteredVoxel(i, j, k));
							if (materialMap)
							{
								materialMap->SetUnsignedVoxelOctet(i, j, k, material);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float d1 = -(bottomPlane ^ position);
					float d2 = topPlane ^ position;
					float d3 = Magnitude(q - ProjectOnto(q, bottomPlane.GetAntivector3D())) - drawingRadius;
					float d = Fmax(d1, d2, d3);
					if (d < 0.0F)
					{
						voxelMap->SetVoxel(i, j, k, savedVoxelMap->GetFilteredVoxel(i, j, k));
						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSphericalMinPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material)
{
	UnsignedVoxel *voxel = drawingMap->GetUnsignedVoxelData();
	int32 rowSize = drawingMap->GetRowSize();
	int32 deckSize = drawingMap->GetDeckSize();
	const VoxelBox& voxelBounds = drawingMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;

	float f = 1.0F / (fuzzy * -0.99609375F - 0.00390625F);
	float inverseFuzzyRadius = 1.0F / drawingRadius;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					float r2 = SquaredMag(q);
					if (r2 < squaredRadius)
					{
						float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
						UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (UnsignedVoxel) Min(0xFF - ((blend * a) >> 8), voxel[m]);

						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					float r2 = SquaredMag(position - p1);
					if (r2 < squaredRadius)
					{
						float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
						UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (UnsignedVoxel) Min(0xFF - ((blend * a) >> 8), voxel[m]);

						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalMinPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material)
{
	UnsignedVoxel *voxel = drawingMap->GetUnsignedVoxelData();
	int32 rowSize = drawingMap->GetRowSize();
	int32 deckSize = drawingMap->GetDeckSize();
	const VoxelBox& voxelBounds = drawingMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;

	float f = 1.0F / (fuzzy * -0.99609375F - 0.00390625F);
	float inverseFuzzyRadius = 1.0F / drawingRadius;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float r2 = SquaredMag(q);
					if (r2 < squaredRadius)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						if (Fmax(d1, d2) < 0.0F)
						{
							float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
							UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

							int32 m = k * deckSize + j * rowSize + i;
							voxel[m] = (UnsignedVoxel) Min(0xFF - ((blend * a) >> 8), voxel[m]);

							if (materialMap)
							{
								materialMap->SetUnsignedVoxelOctet(i, j, k, material);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;
					float r2 = SquaredMag(q - ProjectOnto(q, bottomPlane.GetAntivector3D()));
					if (r2 < squaredRadius)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						if (Fmax(d1, d2) < 0.0F)
						{
							float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
							UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

							int32 m = k * deckSize + j * rowSize + i;
							voxel[m] = (UnsignedVoxel) Min(0xFF - ((blend * a) >> 8), voxel[m]);

							if (materialMap)
							{
								materialMap->SetUnsignedVoxelOctet(i, j, k, material);
							}
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSphericalMaxPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material)
{
	UnsignedVoxel *voxel = drawingMap->GetUnsignedVoxelData();
	int32 rowSize = drawingMap->GetRowSize();
	int32 deckSize = drawingMap->GetDeckSize();
	const VoxelBox& voxelBounds = drawingMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;

	float f = 1.0F / (fuzzy * -0.99609375F - 0.00390625F);
	float inverseFuzzyRadius = 1.0F / drawingRadius;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					float r2 = SquaredMag(q);
					if (r2 < squaredRadius)
					{
						float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
						UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (UnsignedVoxel) Max((blend * a) >> 8, voxel[m]);

						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					float r2 = SquaredMag(position - p1);
					if (r2 < squaredRadius)
					{
						float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
						UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

						int32 m = k * deckSize + j * rowSize + i;
						voxel[m] = (UnsignedVoxel) Max((blend * a) >> 8, voxel[m]);

						if (materialMap)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalMaxPaintBrush(VoxelMap *drawingMap, VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, float fuzzy, UnsignedVoxel blend, UnsignedVoxel material)
{
	UnsignedVoxel *voxel = drawingMap->GetUnsignedVoxelData();
	int32 rowSize = drawingMap->GetRowSize();
	int32 deckSize = drawingMap->GetDeckSize();
	const VoxelBox& voxelBounds = drawingMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;

	float f = 1.0F / (fuzzy * -0.99609375F - 0.00390625F);
	float inverseFuzzyRadius = 1.0F / drawingRadius;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float r2 = SquaredMag(q);
					if (r2 < squaredRadius)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						if (Fmax(d1, d2) < 0.0F)
						{
							float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
							UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

							int32 m = k * deckSize + j * rowSize + i;
							voxel[m] = (UnsignedVoxel) Max((blend * a) >> 8, voxel[m]);

							if (materialMap)
							{
								materialMap->SetUnsignedVoxelOctet(i, j, k, material);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;
					float r2 = SquaredMag(q - ProjectOnto(q, bottomPlane.GetAntivector3D()));
					if (r2 < squaredRadius)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						if (Fmax(d1, d2) < 0.0F)
						{
							float alpha = Sqrt(r2) * inverseFuzzyRadius * f - f;
							UnsignedFixed a = Min(MaxZero((UnsignedFixed) (alpha * 256.0F)), 0x0100);

							int32 m = k * deckSize + j * rowSize + i;
							voxel[m] = (UnsignedVoxel) Max((blend * a) >> 8, voxel[m]);

							if (materialMap)
							{
								materialMap->SetUnsignedVoxelOctet(i, j, k, material);
							}
						}
					}
				}
			}
		}
	}
}

void TerrainPage::DrawSphericalMaterialBrush(VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = materialMap->GetVoxelBounds();
	float squaredRadius = drawingRadius * drawingRadius;

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					if (SquaredMag(q) < squaredRadius)
					{
						materialMap->SetUnsignedVoxelOctet(i, j, k, material);
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					if (SquaredMag(position - p1) < squaredRadius)
					{
						materialMap->SetUnsignedVoxelOctet(i, j, k, material);
					}
				}
			}
		}
	}
}

void TerrainPage::DrawCylindricalMaterialBrush(VoxelMap *materialMap, const Point3D& p1, const Point3D& p2, const Antivector4D& topPlane, const Antivector4D& bottomPlane, UnsignedVoxel material)
{
	const VoxelBox& voxelBounds = materialMap->GetVoxelBounds();

	if (p1 != p2)
	{
		Vector3D dp = Normalize(p2 - p1);
		Antivector4D cap1(dp, p1);
		Antivector4D cap2(-dp, p2);

		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Vector3D	q;

					Point3D position((float) i, y, z);

					if ((cap1 ^ position) < 0.0F)
					{
						q = position - p1;
					}
					else if ((cap2 ^ position) < 0.0F)
					{
						q = position - p2;
					}
					else
					{
						q = position - p1;
						q -= ProjectOnto(q, dp);
					}

					q -= ProjectOnto(q, bottomPlane.GetAntivector3D());
					float d3 = Magnitude(q) - drawingRadius;
					if (d3 < 0.0F)
					{
						float d1 = -(bottomPlane ^ position);
						float d2 = topPlane ^ position;
						float d = Fmax(d1, d2, d3);
						if (d < 0.0F)
						{
							materialMap->SetUnsignedVoxelOctet(i, j, k, material);
						}
					}
				}
			}
		}
	}
	else
	{
		for (machine k = voxelBounds.min.z; k <= voxelBounds.max.z; k++)
		{
			float z = (float) k;
			for (machine j = voxelBounds.min.y; j <= voxelBounds.max.y; j++)
			{
				float y = (float) j;
				for (machine i = voxelBounds.min.x; i <= voxelBounds.max.x; i++)
				{
					Point3D position((float) i, y, z);
					Vector3D q = position - p1;

					float d1 = -(bottomPlane ^ position);
					float d2 = topPlane ^ position;
					float d3 = Magnitude(q - ProjectOnto(q, bottomPlane.GetAntivector3D())) - drawingRadius;
					float d = Fmax(d1, d2, d3);
					if (d < 0.0F)
					{
						materialMap->SetUnsignedVoxelOctet(i, j, k, material);
					}
				}
			}
		}
	}
}

void TerrainPage::UpdateBlock(Editor *editor, TerrainBlock *block, const Integer3D& coord, int32 level, MaterialObject *materialObject, unsigned_int32 flags)
{
	TerrainGeometry *geometry = block->FindTerrainGeometry(coord, level);

	if (!(flags & kTerrainUpdateForceDelete))
	{
		if (!geometry)
		{
			if (level == 0)
			{
				geometry = new TerrainGeometry(block, coord);
			}
			else
			{
				geometry = new TerrainLevelGeometry(block, coord, level);
			}

			block->AddTerrainGeometry(geometry);
		}
		else
		{
			geometry->Invalidate();
			geometry->InvalidateShaderData();
		}

		geometry->SetMaterialObject(0, materialObject);
		geometry->Update();

		TerrainGeometryObject *object = geometry->GetObject();
		object->Build(geometry);

		if (object->GetGeometryLevel(0)->GetPrimitiveCount() != 0)
		{
			if (!geometry->GetManipulator())
			{
				EditorManipulator::Install(editor, geometry, false);
			}

			Editor::GetManipulator(geometry)->InvalidateGraph();

			geometry->Invalidate();
			geometry->Neutralize();
			geometry->Preprocess();
		}
		else
		{
			if (geometry->GetManipulator())
			{
				editor->DeleteNode(geometry);
			}
			else
			{
				delete geometry;
			}
		}
	}
	else if (geometry)
	{
		editor->DeleteNode(geometry);
	}
}

// ZYUQURM
