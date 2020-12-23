 

#include "C4Markings.h"
#include "C4World.h"


using namespace C4;


namespace
{
	const float kMarkingEpsilon = 0.25F;
}


namespace C4
{
	template <> Heap EngineMemory<MarkingEffect>::heap("MarkingEffect");
	template class EngineMemory<MarkingEffect>;
}


MarkingEffect::MarkingEffect() :
		Effect(kEffectMarking, kRenderIndexedTriangles, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);

	largeArrayStorage = nullptr;
	materialObject = nullptr;
}

MarkingEffect::MarkingEffect(const Geometry *geometry, const MarkingData *data) :
		Effect(kEffectMarking, kRenderIndexedTriangles, kRenderDepthTest | kRenderDepthInhibit),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		indexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		colorAttribute(data->color, kAttributeMutable)
{
	Point3D			markingCenter;
	Vector3D		markingNormal;
	Vector3D		markingTangent;
	ClippingData	clippingData;

	SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);

	largeArrayStorage = nullptr;
	materialObject = nullptr;

	markingFlags = data->markingFlags;
	markingAlpha = data->color.alpha;

	int32 life = data->lifeTime;
	if (life >= 0)
	{
		markingLifeTime = life;
		markingFadeTime = Max(markingLifeTime * 3 / 4, 1);
		markingKillTime = markingFadeTime / 8;
	}
	else
	{
		markingLifeTime = TheTimeMgr->GetDeltaTime() + 1;
		markingFadeTime = 0;
		markingKillTime = 0;
	}

	markingDepthOffset = data->markingOffset;

	MarkingList *markingList = data->markingList;
	if (markingList)
	{
		markingList->Append(this);
	}

	float radius = data->radius;
	float zmin = -radius;
	float zmax = radius;

	if (markingFlags & kMarkingClipRange)
	{
		zmin = data->clip.min;
		zmax = data->clip.max;
	}

	float z = Fmax(Fabs(zmin), Fabs(zmax));
	effectRadius = Sqrt(z * z + radius * radius * 2.0F);

	effectPosition = geometry->GetInverseWorldTransform() * data->center;

	const Transformable *transformable = geometry->GetTransformable();
	if (transformable)
	{
		const Transform4D& transform = transformable->GetWorldTransform();
		const Transform4D& inverse = transformable->GetInverseWorldTransform();

		markingCenter = inverse * data->center;
		markingNormal = data->normal * transform;
		markingTangent = inverse * data->tangent;
	}
	else
	{
		markingCenter = data->center;
		markingNormal = data->normal; 
		markingTangent = data->tangent;
	}
 
	positionArray = smallPositionArray;
	normalArray = smallNormalArray; 
	colorArray = smallColorArray;
	tangentArray = smallTangentArray;
	texcoordArray = smallTexcoordArray; 
	triangleArray = smallTriangleArray;
 
	Antivector3D up = Normalize(markingNormal % markingTangent); 
	Antivector3D right = Normalize(up % markingNormal);

	float d = markingCenter * right;
	clippingData.leftPlane.Set(right, radius - d); 
	clippingData.rightPlane.Set(-right, radius + d);

	d = markingCenter * up;
	clippingData.bottomPlane.Set(up, radius - d);
	clippingData.topPlane.Set(-up, radius + d);

	d = markingCenter * markingNormal;
	clippingData.backPlane.Set(markingNormal, -zmin - d);
	clippingData.frontPlane.Set(-markingNormal, zmax + d);

	markingVertexCount = 0;
	markingTriangleCount = 0;
	clippingData.maxMarkingVertexCount = kMaxSmallMarkingVertexCount;

	const GeometryObject *object = geometry->GetObject();
	if ((object->GetGeometryFlags() & kGeometryMarkingFullPolygon) && (!(markingFlags & kMarkingForceClip)))
	{
		markingFlags |= kMarkingFullPolygon;
	}

	const Mesh *mesh = object->GetGeometryLevel(0);
	int32 vertexCount = mesh->GetVertexCount();
	clippingData.geometryVertexCount = vertexCount;

	const Point3D *vertex = mesh->GetArray<Point3D>(kArrayPosition);
	const Vector3D *normal = mesh->GetArray<Vector3D>(kArrayNormal);

	int32 triangleCount = mesh->GetPrimitiveCount();
	const Triangle *triangle = mesh->GetArray<Triangle>(kArrayPrimitive);

	for (machine a = 0; a < triangleCount; a++)
	{
		int32 i1 = triangle->index[0];
		int32 i2 = triangle->index[1];
		int32 i3 = triangle->index[2];

		const Point3D& v1 = vertex[i1];
		const Point3D& v2 = vertex[i2];
		const Point3D& v3 = vertex[i3];

		Antivector3D triangleNormal = Normalize((v2 - v1) % (v3 - v1));
		if (markingNormal * triangleNormal > kMarkingEpsilon)
		{
			Point3D		polygonVertex[3];
			Vector3D	polygonNormal[3];
			Point3D		tempVertex[2][9];
			Vector3D	tempNormal[2][9];

			polygonVertex[0] = v1;
			polygonVertex[1] = v2;
			polygonVertex[2] = v3;

			polygonNormal[0] = normal[i1];
			polygonNormal[1] = normal[i2];
			polygonNormal[2] = normal[i3];

			int32 count = ClipPolygon(clippingData.leftPlane, 3, polygonVertex, polygonNormal, tempVertex[0], tempNormal[0]);
			if (count != 0)
			{
				count = ClipPolygon(clippingData.rightPlane, count, tempVertex[0], tempNormal[0], tempVertex[1], tempNormal[1]);
				if (count != 0)
				{
					count = ClipPolygon(clippingData.bottomPlane, count, tempVertex[1], tempNormal[1], tempVertex[0], tempNormal[0]);
					if (count != 0)
					{
						count = ClipPolygon(clippingData.topPlane, count, tempVertex[0], tempNormal[0], tempVertex[1], tempNormal[1]);
						if (count != 0)
						{
							count = ClipPolygon(clippingData.backPlane, count, tempVertex[1], tempNormal[1], tempVertex[0], tempNormal[0]);
							if (count != 0)
							{
								count = ClipPolygon(clippingData.frontPlane, count, tempVertex[0], tempNormal[0], tempVertex[1], tempNormal[1]);
								if (count != 0)
								{
									if (!(markingFlags & kMarkingFullPolygon))
									{
										if (!AddPolygon(count, tempVertex[1], tempNormal[1], &clippingData))
										{
											break;
										}
									}
									else
									{
										if (!AddPolygon(3, polygonVertex, polygonNormal, &clippingData))
										{
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		triangle++;
	}

	if (markingVertexCount > 0)
	{
		float f = 0.5F / radius;
		Vector3D sdir = clippingData.leftPlane.GetAntivector3D() * (data->texcoordScale.x * f);
		Vector3D tdir = clippingData.bottomPlane.GetAntivector3D() * (data->texcoordScale.y * f);
		float ds = data->texcoordScale.x * 0.5F + data->texcoordOffset.x;
		float dt = data->texcoordScale.y * 0.5F + data->texcoordOffset.y;

		for (machine a = 0; a < markingVertexCount; a++)
		{
			Vector3D v = positionArray[a] - markingCenter;
			float s = v * sdir + ds;
			float t = v * tdir + dt;
			texcoordArray[a].Set(s, t);
		}

		if (markingFlags & kMarkingLight)
		{
			MaterialObject *material = data->materialObject;
			if (material)
			{
				material->Retain();
			}

			materialObject = material;
		}
		else
		{
			textureAttribute.SetTexture(data->textureName);
		}
	}
	else
	{
		markingLifeTime = 0;
	}
}

MarkingEffect::~MarkingEffect()
{
	if (materialObject)
	{
		materialObject->Release();
	}

	delete[] largeArrayStorage;

	List<MarkingEffect> *list = ListElement<MarkingEffect>::GetOwningList();
	if (list)
	{
		list->Remove(this);
		if (list->Empty())
		{
			static_cast<MarkingList *>(list)->HandleDestruction();
		}
	}
}

void MarkingEffect::Prepack(List<Object> *linkList) const
{
	Effect::Prepack(linkList);

	if (materialObject)
	{
		linkList->Append(materialObject);
	}
}

void MarkingEffect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << markingFlags;

	data << ChunkHeader('ALFA', 4);
	data << markingAlpha;

	data << ChunkHeader('DOFF', 4);
	data << markingDepthOffset;

	data << ChunkHeader('TIME', 12);
	data << markingLifeTime;
	data << markingFadeTime;
	data << markingKillTime;

	data << ChunkHeader('SPHR', sizeof(Point3D) + 4);
	data << effectPosition;
	data << effectRadius;

	PackHandle handle = data.BeginChunk('DIFF');
	colorAttribute.Pack(data, packFlags);
	data.EndChunk(handle);

	if (markingFlags & kMarkingLight)
	{
		if (materialObject)
		{
			data << ChunkHeader('MATL', 4);
			data << materialObject->GetObjectIndex();
		}
	}
	else
	{
		handle = data.BeginChunk('TXTR');
		textureAttribute.Pack(data, packFlags);
		data.EndChunk(handle);
	}

	int32 vertexCount = markingVertexCount;
	int32 triangleCount = markingTriangleCount;

	data << ChunkHeader('GEOM', 8);
	data << vertexCount;
	data << triangleCount;

	if (triangleCount & 1)
	{
		triangleArray[triangleCount].Set(0, 0, 0);
		triangleCount++;
	}

	data << ChunkHeader('POSI', vertexCount * sizeof(Point3D));
	data.WriteArray(vertexCount, positionArray);

	data << ChunkHeader('TEX0', vertexCount * sizeof(Point2D));
	data.WriteArray(vertexCount, texcoordArray);

	if (markingFlags & kMarkingLight)
	{
		data << ChunkHeader('FRAM', vertexCount * sizeof(Vector3D) * 2);
		data.WriteArray(vertexCount, normalArray);
		data.WriteArray(vertexCount, tangentArray);
	}
	else
	{
		data << ChunkHeader('COLR', vertexCount * sizeof(Color4C));
		data.WriteArray(vertexCount, colorArray);
	}

	data << ChunkHeader('FACE', triangleCount * sizeof(Triangle));
	data.WriteArray(triangleCount, triangleArray);

	data << TerminatorChunk;
}

void MarkingEffect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<MarkingEffect>(data, unpackFlags);
}

bool MarkingEffect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> markingFlags;
			return (true);

		case 'ALFA':

			data >> markingAlpha;
			return (true);

		case 'DOFF':

			data >> markingDepthOffset;
			return (true);

		case 'TIME':

			data >> markingLifeTime;
			data >> markingFadeTime;
			data >> markingKillTime;
			return (true);

		case 'SPHR':

			data >> effectPosition;
			data >> effectRadius;
			return (true);

		case 'DIFF':

			colorAttribute.Unpack(data, unpackFlags);
			return (true);

		case 'MATL':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, this);
			return (true);
		}

		case 'TXTR':

			textureAttribute.Unpack(data, unpackFlags);
			return (true);

		case 'GEOM':

			data >> markingVertexCount;
			data >> markingTriangleCount;

			if (markingVertexCount > kMaxSmallMarkingVertexCount)
			{
				AllocateLargeArrays(markingVertexCount, (markingTriangleCount + 1) & ~1);
			}
			else
			{
				positionArray = smallPositionArray;
				normalArray = smallNormalArray;
				colorArray = smallColorArray;
				tangentArray = smallTangentArray;
				texcoordArray = smallTexcoordArray;
				triangleArray = smallTriangleArray;
			}

			return (true);

		case 'POSI':

			data.ReadArray(markingVertexCount, positionArray);
			return (true);

		case 'TEX0':

			data.ReadArray(markingVertexCount, texcoordArray);
			return (true);

		case 'FRAM':

			data.ReadArray(markingVertexCount, normalArray);
			data.ReadArray(markingVertexCount, tangentArray);
			return (true);

		case 'COLR':

			data.ReadArray(markingVertexCount, colorArray);
			return (true);

		case 'FACE':

			data.ReadArray((markingTriangleCount + 1) & ~1, triangleArray);
			return (true);
	}

	return (false);
}

void *MarkingEffect::BeginSettingsUnpack(void)
{
	colorAttribute.BeginSettingsUnpack();
	textureAttribute.BeginSettingsUnpack();

	delete[] largeArrayStorage;
	largeArrayStorage = nullptr;

	return (Effect::BeginSettingsUnpack());
}

void MarkingEffect::MaterialObjectLinkProc(Object *object, void *cookie)
{
	static_cast<MarkingEffect *>(cookie)->materialObject = static_cast<MaterialObject *>(object);
	object->Retain();
}

bool MarkingEffect::CalculateBoundingBox(Box3D *box) const
{
	const Point3D& p = effectPosition;
	float r = effectRadius;

	box->min.Set(p.x - r, p.y - r, p.z - r);
	box->max.Set(p.x + r, p.y + r, p.z + r);

	return (true);
}

bool MarkingEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(effectPosition);
	sphere->SetRadius(effectRadius);
	return (true);
}

void MarkingEffect::Preprocess(void)
{
	Effect::Preprocess();
	Wake();

	SetEffectListIndex((markingFlags & kMarkingLight) ? kEffectListLight : kEffectListOpaque);

	const Node *node = GetSuperNode();
	if (node->GetNodeType() == kNodeGeometry)
	{
		SetTransformable(static_cast<const Geometry *>(node)->GetTransformable());
	}

	if (!(markingFlags & kMarkingFullPolygon))
	{
		SetRenderState(GetRenderState() | kRenderDepthOffset);
		SetDepthOffset(markingDepthOffset, &GetBoundingSphere()->GetCenter());
	}
	else
	{
		SetRenderState(GetRenderState() | kRenderPolygonOffset);
	}

	int32 vertexCount = markingVertexCount;
	SetVertexCount(vertexCount);
	SetVertexAttributeArray(kArrayPosition, 0, 3);

	RenderSegment *segment = GetFirstRenderSegment();

	attributeList.Append(&colorAttribute);
	segment->SetMaterialAttributeList(&attributeList);

	if (markingFlags & kMarkingLight)
	{
		if (markingFlags & kMarkingDepthWrite)
		{
			SetRenderState(GetRenderState() & ~kRenderDepthInhibit);
		}

		if (markingFlags & kMarkingBlendLight)
		{
			SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);
			SetLightBlendState(BlendState(kBlendSourceAlpha, kBlendOne));
		}

		segment->SetMaterialObjectPointer(&materialObject);

		SetVertexAttributeArray(kArrayNormal, sizeof(Point3D), 3);
		SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) + sizeof(Vector3D), 3);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector3D) * 2, 2);

		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(LitMarkingVertex));
		vertexBuffer.Establish(vertexCount * sizeof(LitMarkingVertex));

		volatile LitMarkingVertex *restrict vertex = vertexBuffer.BeginUpdateSync<LitMarkingVertex>();

		for (machine a = 0; a < vertexCount; a++)
		{
			vertex[a].position = positionArray[a];
			vertex[a].normal = normalArray[a];
			vertex[a].tangent = tangentArray[a];
			vertex[a].texcoord = texcoordArray[a];
		}

		vertexBuffer.EndUpdateSync();
	}
	else
	{
		SetAmbientBlendState(kBlendInterpolate | kBlendAlphaPreserve);

		if (markingFlags & kMarkingTwoSided)
		{
			segment->SetMaterialState(kMaterialTwoSided);
		}

		attributeList.Append(&textureAttribute);

		SetVertexAttributeArray(kArrayColor, sizeof(Point3D), 1);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Color4C), 2);

		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(MarkingVertex));
		vertexBuffer.Establish(vertexCount * sizeof(MarkingVertex));

		volatile MarkingVertex *restrict vertex = vertexBuffer.BeginUpdateSync<MarkingVertex>();

		for (machine a = 0; a < vertexCount; a++)
		{
			vertex[a].position = positionArray[a];
			vertex[a].color = colorArray[a];
			vertex[a].texcoord = texcoordArray[a];
		}

		vertexBuffer.EndUpdateSync();
	}

	SetPrimitiveCount(markingTriangleCount);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	indexBuffer.Establish(markingTriangleCount * sizeof(Triangle), triangleArray);

	markingInvisibleTime = -1;
}

void MarkingEffect::New(const World *world, const MarkingData *data)
{
	world->QueryProximity(data->center, data->radius, &MarkGeometry, const_cast<MarkingData *>(data));
}

ProximityResult MarkingEffect::MarkGeometry(Node *node, const Point3D& center, float radius, void *cookie)
{
	if (node->GetNodeType() == kNodeGeometry)
	{
		Geometry *geometry = static_cast<Geometry *>(node);
		unsigned_int32 geometryFlags = geometry->GetObject()->GetGeometryFlags();

		if (!(geometryFlags & (kGeometryInvisible | kGeometryMarkingInhibit)))
		{
			MarkingEffect *marking = new MarkingEffect(geometry, static_cast<MarkingData *>(cookie));
			if (!marking->Empty())
			{
				if (geometryFlags & kGeometryCubeLightInhibit)
				{
					marking->SetEffectFlags(kEffectCubeLightInhibit);
				}

				geometry->AppendNewSubnode(marking);
			}
			else
			{
				delete marking;
			}
		}
	}

	return (kProximityContinue);
}

void MarkingEffect::AllocateLargeArrays(int32 vertexCount, int32 triangleCount)
{
	if (markingFlags & kMarkingLight)
	{
		char *array = new char[vertexCount * (sizeof(Point3D) + sizeof(Vector3D) * 2 + sizeof(Point2D)) + triangleCount * sizeof(Triangle)];
		largeArrayStorage = array;

		positionArray = reinterpret_cast<Point3D *>(array);
		normalArray = positionArray + vertexCount;
		tangentArray = normalArray + vertexCount;
		texcoordArray = reinterpret_cast<Point2D *>(tangentArray + vertexCount);
		triangleArray = reinterpret_cast<Triangle *>(texcoordArray + vertexCount);
	}
	else
	{
		char *array = new char[vertexCount * (sizeof(Point3D) + sizeof(Color4C) + sizeof(Point2D)) + triangleCount * sizeof(Triangle)];
		largeArrayStorage = array;

		positionArray = reinterpret_cast<Point3D *>(array);
		colorArray = reinterpret_cast<Color4C *>(positionArray + vertexCount);
		texcoordArray = reinterpret_cast<Point2D *>(colorArray + vertexCount);
		triangleArray = reinterpret_cast<Triangle *>(texcoordArray + vertexCount);
	}
}

bool MarkingEffect::AddPolygon(int32 vertexCount, const Point3D *position, const Vector3D *normal, ClippingData *clippingData)
{
	int32 count = markingVertexCount;
	if (count + vertexCount >= clippingData->maxMarkingVertexCount)
	{
		if (clippingData->maxMarkingVertexCount != kMaxSmallMarkingVertexCount)
		{
			return (false);
		}

		int32 newMaxCount = clippingData->geometryVertexCount * 4;
		clippingData->maxMarkingVertexCount = newMaxCount;
		AllocateLargeArrays(newMaxCount, newMaxCount * 3);

		if (markingFlags & kMarkingLight)
		{
			MemoryMgr::CopyMemory(smallPositionArray, positionArray, count * sizeof(Point3D));
			MemoryMgr::CopyMemory(smallNormalArray, normalArray, count * sizeof(Vector3D));
			MemoryMgr::CopyMemory(smallTangentArray, tangentArray, count * sizeof(Vector3D));
			MemoryMgr::CopyMemory(smallTexcoordArray, texcoordArray, count * sizeof(Point2D));
			MemoryMgr::CopyMemory(smallTriangleArray, triangleArray, markingTriangleCount * sizeof(Triangle));
		}
		else
		{
			MemoryMgr::CopyMemory(smallPositionArray, positionArray, count * sizeof(Point3D));
			MemoryMgr::CopyMemory(smallColorArray, colorArray, count * sizeof(Color4C));
			MemoryMgr::CopyMemory(smallTexcoordArray, texcoordArray, count * sizeof(Point2D));
			MemoryMgr::CopyMemory(smallTriangleArray, triangleArray, markingTriangleCount * sizeof(Triangle));
		}
	}

	Triangle *triangle = triangleArray + markingTriangleCount;
	markingTriangleCount += vertexCount - 2;
	for (machine a = 2; a < vertexCount; a++)
	{
		triangle->Set(count, count + a - 1, count + a);
		triangle++;
	}

	if (markingFlags & kMarkingLight)
	{
		const Vector3D& tang = clippingData->leftPlane.GetAntivector3D();
		for (machine a = 0; a < vertexCount; a++)
		{
			positionArray[count] = position[a];
			Vector3D nrml = normal[a] * InverseMag(normal[a]);
			normalArray[count] = nrml;
			tangentArray[count] = Normalize(tang - nrml * (nrml * tang));
			count++;
		}
	}
	else
	{
		float f = 255.0F / (1.0F - kMarkingEpsilon);
		const Vector3D& markingNormal = clippingData->backPlane.GetAntivector3D();
		for (machine a = 0; a < vertexCount; a++)
		{
			positionArray[count] = position[a];
			const Vector3D& nrml = normal[a];
			colorArray[count].Set(255, 255, 255, (int32) FmaxZero((markingNormal * nrml * InverseMag(nrml) - kMarkingEpsilon) * f + 0.5F));
			count++;
		}
	}

	markingVertexCount = count;
	return (true);
}

int32 MarkingEffect::ClipPolygon(const Antivector4D& plane, int32 vertexCount, const Point3D *position, const Vector3D *normal, Point3D *newPosition, Vector3D *newNormal)
{
	bool	negative[9];

	int32 negativeCount = 0;
	for (machine a = 0; a < vertexCount; a++)
	{
		bool neg = ((plane ^ position[a]) < 0.0F);
		negative[a] = neg;
		negativeCount += neg;
	}

	if (negativeCount == vertexCount)
	{
		return (0);
	}

	int32 count = 0;
	int32 previous = vertexCount - 1;
	for (machine index = 0; index < vertexCount; index++)
	{
		if (negative[index])
		{
			if (!negative[previous])
			{
				const Point3D& v1 = position[previous];
				const Point3D& v2 = position[index];
				float t = (plane ^ v1) / (plane ^ (v1 - v2));
				newPosition[count] = v1 * (1.0F - t) + v2 * t;

				const Vector3D& n1 = normal[previous];
				const Vector3D& n2 = normal[index];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}
		}
		else
		{
			if (negative[previous])
			{
				const Point3D& v1 = position[index];
				const Point3D& v2 = position[previous];
				float t = (plane ^ v1) / (plane ^ (v1 - v2));
				newPosition[count] = v1 * (1.0F - t) + v2 * t;

				const Vector3D& n1 = normal[index];
				const Vector3D& n2 = normal[previous];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}

			newPosition[count] = position[index];
			newNormal[count] = normal[index];
			count++;
		}

		previous = index;
	}

	return (count);
}

void MarkingEffect::Move(void)
{
	int32 life = markingLifeTime;
	int32 fade = markingFadeTime;
	int32 invisible = markingInvisibleTime;

	if ((life <= 0) || ((life < fade) && (invisible > markingKillTime)))
	{
		delete this;
	}
	else
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		markingLifeTime = life - dt;

		if (invisible >= 0)
		{
			markingInvisibleTime = invisible + dt;
		}
		else
		{
			markingInvisibleTime = 0;
		}
	}
}

void MarkingEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	int32 life = markingLifeTime;
	if (life != 0)
	{
		markingInvisibleTime = -1;

		int32 fade = markingFadeTime;
		if (life < fade)
		{
			float alpha = markingAlpha;
			float ratio = (float) life / (float) fade;

			if (!(markingFlags & kMarkingLight))
			{
				alpha *= ratio;
			}
			else
			{
				alpha *= ratio * 0.5F + 0.5F;
			}

			colorAttribute.SetDiffuseAlpha(alpha);
		}

		effectList[GetEffectListIndex()].Append(this);
	}
}


void MarkingList::HandleDestruction(void)
{
}

// ZYUQURM
