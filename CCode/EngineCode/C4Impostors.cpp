 

#include "C4Impostors.h"
#include "C4World.h"
#include "C4Shaders.h"
#include "C4Configuration.h"


using namespace C4;


ImpostorProperty::ImpostorProperty() : Property(kPropertyImpostor)
{
	textureWidth = 128;
	textureHeight = 128;
	textureUsage = kImpostorColorMap;
	impostorFlags = kImpostorAlphaChannel;
	textureName[0] = 0;
}

ImpostorProperty::ImpostorProperty(const ImpostorProperty& impostorProperty) : Property(impostorProperty)
{
	textureWidth = impostorProperty.textureWidth;
	textureHeight = impostorProperty.textureHeight;
	textureUsage = impostorProperty.textureUsage;
	impostorFlags = impostorProperty.impostorFlags;
	textureName = impostorProperty.textureName;
}

ImpostorProperty::ImpostorProperty(int32 width, int32 height, const char *name) : Property(kPropertyImpostor)
{
	textureWidth = width;
	textureHeight = height;
	textureUsage = kImpostorColorMap;
	impostorFlags = kImpostorAlphaChannel;
	textureName = name;
}

ImpostorProperty::~ImpostorProperty()
{
}

Property *ImpostorProperty::Replicate(void) const
{
	return (new ImpostorProperty(*this));
}

bool ImpostorProperty::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeZone) && (!node->GetSuperNode()));
}

void ImpostorProperty::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Property::Pack(data, packFlags);

	data << ChunkHeader('TSIZ', 8);
	data << textureWidth;
	data << textureHeight;

	data << ChunkHeader('USAG', 4);
	data << textureUsage;

	data << ChunkHeader('FLAG', 4);
	data << impostorFlags;

	PackHandle handle = data.BeginChunk('TNAM');
	data << textureName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void ImpostorProperty::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Property::Unpack(data, unpackFlags);
	UnpackChunkList<ImpostorProperty>(data, unpackFlags);
}

bool ImpostorProperty::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TSIZ':

			data >> textureWidth;
			data >> textureHeight;
			return (true);

		case 'USAG':

			data >> textureUsage;
			return (true);

		case 'FLAG':

			data >> impostorFlags;
			return (true);

		case 'TNAM':

			data >> textureName;
			return (true); 
	}

	return (false); 
}
 
int32 ImpostorProperty::GetSettingCount(void) const
{
	return (7); 
}
 
Setting *ImpostorProperty::GetSetting(int32 index) const 
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0) 
	{
		const char *title = table->GetString(StringID('PROP', kPropertyImpostor, 'TNAM'));
		const char *picker = table->GetString(StringID('PROP', kPropertyImpostor, 'TPCK'));
		return (new ResourceSetting('TNAM', textureName, title, picker, TextureResource::GetDescriptor()));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyImpostor, 'WIDE'));
		return (new PowerTwoSetting('WIDE', textureWidth, title, 32, 512));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyImpostor, 'HIGH'));
		return (new PowerTwoSetting('HIGH', textureHeight, title, 32, 2048));
	}

	if (index == 3)
	{
		int32 selection = 0;
		if (textureUsage == kImpostorNormalMap)
		{
			selection = 1;
		}
		else if (textureUsage == kImpostorShadowMap)
		{
			selection = 2;
		}

		const char *title = table->GetString(StringID('PROP', kPropertyImpostor, 'USAG'));
		MenuSetting *menu = new MenuSetting('USAG', selection, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('PROP', kPropertyImpostor, 'USAG', 'COLR')));
		menu->SetMenuItemString(1, table->GetString(StringID('PROP', kPropertyImpostor, 'USAG', 'NRML')));
		menu->SetMenuItemString(2, table->GetString(StringID('PROP', kPropertyImpostor, 'USAG', 'SHAD')));
		return (menu);
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyImpostor, 'ALFA'));
		return (new BooleanSetting('ALFA', ((impostorFlags & kImpostorAlphaChannel) != 0), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyImpostor, 'CLP1'));
		return (new BooleanSetting('CLP1', ((impostorFlags & kImpostorKeepBottom) != 0), title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('PROP', kPropertyImpostor, 'CLP2'));
		return (new BooleanSetting('CLP2', ((impostorFlags & kImpostorKeepTop) != 0), title));
	}

	return (nullptr);
}

void ImpostorProperty::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TNAM')
	{
		textureName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else if (identifier == 'WIDE')
	{
		textureWidth = static_cast<const PowerTwoSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'HIGH')
	{
		textureHeight = static_cast<const PowerTwoSetting *>(setting)->GetIntegerValue();
	}
	else if (identifier == 'USAG')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			textureUsage = kImpostorColorMap;
		}
		else if (selection == 1)
		{
			textureUsage = kImpostorNormalMap;
		}
		else
		{
			textureUsage = kImpostorShadowMap;
		}
	}
	else if (identifier == 'ALFA')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			impostorFlags |= kImpostorAlphaChannel;
		}
		else
		{
			impostorFlags &= ~kImpostorAlphaChannel;
		}
	}
	else if (identifier == 'CLP1')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			impostorFlags |= kImpostorKeepBottom;
		}
		else
		{
			impostorFlags &= ~kImpostorKeepBottom;
		}
	}
	else if (identifier == 'CLP2')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			impostorFlags |= kImpostorKeepTop;
		}
		else
		{
			impostorFlags &= ~kImpostorKeepTop;
		}
	}
}


Impostor::Impostor() : Node(kNodeImpostor)
{
	materialObject = nullptr;

	renderDistance = 400.0F;
	transitionLength = 100.0F;

	impostorSize.Set(0.0F, 0.0F);

	lightStamp = 0xFFFFFFFF;
	shadowStamp = 0xFFFFFFFF;

	impostorSystem = nullptr;
}

Impostor::Impostor(const Impostor& impostor) : Node(impostor)
{
	MaterialObject *object = impostor.materialObject;
	if (object)
	{
		object->Retain();
	}

	materialObject = object;

	renderDistance = impostor.renderDistance;
	transitionLength = impostor.transitionLength;

	impostorSize.Set(0.0F, 0.0F);

	lightStamp = 0xFFFFFFFF;
	shadowStamp = 0xFFFFFFFF;

	impostorSystem = nullptr;
}

Impostor::~Impostor()
{
	if (impostorSystem)
	{
		impostorSystem->RemoveImpostor(this);
	}

	if (materialObject)
	{
		materialObject->Release();
	}
}

Node *Impostor::Replicate(void) const
{
	return (new Impostor(*this));
}

void Impostor::Prepack(List<Object> *linkList) const
{
	Node::Prepack(linkList);

	if (materialObject)
	{
		linkList->Append(materialObject);
	}
}

void Impostor::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	if ((materialObject) && (!(packFlags & kPackSettings)))
	{
		data << ChunkHeader('MATL', 4);
		data << materialObject->GetObjectIndex();
	}

	data << ChunkHeader('DIST', 4);
	data << renderDistance;

	data << ChunkHeader('XLEN', 4);
	data << transitionLength;

	data << TerminatorChunk;
}

void Impostor::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Impostor>(data, unpackFlags);
}

bool Impostor::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MATL':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, &materialObject);
			return (true);
		}

		case 'DIST':

			data >> renderDistance;
			return (true);

		case 'XLEN':

			data >> transitionLength;
			return (true);
	}

	return (false);
}

void Impostor::MaterialObjectLinkProc(Object *object, void *cookie)
{
	*static_cast<MaterialObject **>(cookie) = static_cast<MaterialObject *>(object);
	object->Retain();
}

int32 Impostor::GetCategoryCount(void) const
{
	return (Node::GetCategoryCount() + 1);
}

Type Impostor::GetCategoryType(int32 index, const char **title) const
{
	int32 count = Node::GetCategoryCount();
	if (index == count)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kNodeImpostor));
		return (kNodeImpostor);
	}

	return (Node::GetCategoryType(index, title));
}

int32 Impostor::GetCategorySettingCount(Type category) const
{
	if (category == kNodeImpostor)
	{
		return (3);
	}

	return (Node::GetCategorySettingCount(category));
}

Setting *Impostor::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kNodeImpostor)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kNodeImpostor, 'IPST'));
			return (new HeadingSetting(kNodeImpostor, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kNodeImpostor, 'IPST', 'DIST'));
			return (new TextSetting('DIST', renderDistance, title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kNodeImpostor, 'IPST', 'XLEN'));
			return (new TextSetting('XLEN', transitionLength, title));
		}

		/*if (index == 3)
		{
			const char *title = table->GetString(StringID(kNodeImpostor, 'REND'));
			return (new HeadingSetting('REND', title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kNodeImpostor, 'REND', 'REFL'));
			return (new BooleanSetting('REFL', ((perspectiveInstantMask & kPerspectiveReflection) != 0), title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID(kNodeImpostor, 'REND', 'REFR'));
			return (new BooleanSetting('REFR', ((perspectiveInstantMask & kPerspectiveRefraction) != 0), title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID(kNodeImpostor, 'REND', 'CAMR'));
			return (new BooleanSetting('CAMR', ((perspectiveInstantMask & kPerspectiveCameraWidget) != 0), title));
		}*/

		return (nullptr);
	}

	return (Node::GetCategorySetting(category, index, flags));
}

void Impostor::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kNodeImpostor)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'DIST')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			renderDistance = Fmax(Text::StringToFloat(text), 1.0F);
		}
		else if (identifier == 'XLEN')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			transitionLength = Fmax(Text::StringToFloat(text), 1.0F);
		}
		/*else if (identifier == 'REFL')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				perspectiveInstantMask |= kPerspectiveReflection;
			}
			else
			{
				perspectiveInstantMask &= ~kPerspectiveReflection;
			}
		}
		else if (identifier == 'REFR')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				perspectiveInstantMask |= kPerspectiveRefraction;
			}
			else
			{
				perspectiveInstantMask &= ~kPerspectiveRefraction;
			}
		}
		else if (identifier == 'CAMR')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				perspectiveInstantMask |= kPerspectiveCameraWidget;
			}
			else
			{
				perspectiveInstantMask &= ~kPerspectiveCameraWidget;
			}
		}*/
	}
	else
	{
		Node::SetCategorySetting(category, setting);
	}
}

void Impostor::Invalidate(void)
{
	Node::Invalidate();

	if (impostorSystem)
	{
		impostorSystem->buildFlag = true;
	}
}

void Impostor::Preprocess(void)
{
	// Setting kNodeIsolatedVisibility prevents the bounding boxes of the subnodes from being included
	// in the bounding box of the impostor node. The rotated bounding box of a subnode could be much
	// larger than is necessary to enclose the impostor node.

	SetNodeFlags(GetNodeFlags() | (kNodeVisibilitySite | kNodeIsolatedVisibility));
	Node::Preprocess();

	if (materialObject)
	{
		const Attribute *attribute = materialObject->GetFirstAttribute();
		if ((attribute) && (attribute->GetAttributeType() == kAttributeShader))
		{
			const ShaderGraph *shaderGraph = static_cast<const ShaderAttribute *>(attribute)->GetShaderGraph();
			const Process *process = shaderGraph->GetFirstElement();
			while (process)
			{
				if (process->GetProcessType() == kProcessImpostorTexture)
				{
					const Texture *texture = static_cast<const ImpostorTextureProcess *>(process)->GetTexture();
					const Vector2D& size = texture->GetImpostorSize();
					impostorSize = size;

					const Point3D& position = GetWorldPosition();
					renderParameter[0].Set(position.x, position.y, position.z + size.y, position.z);

					float scale = 1.0F / transitionLength;
					renderParameter[1].Set(scale, -renderDistance * scale, 0.5F / size.x, 0.5F / size.y);

					squaredRenderDistance = renderDistance * renderDistance;
					float d = renderDistance + transitionLength;
					squaredGeometryDistance = d * d;

					const float *clipData = (texture->GetImpostorClipFlag()) ? texture->GetImpostorClipData() : nullptr;

					ImpostorSystem *system = GetWorld()->GetImpostorSystem(materialObject, clipData);
					system->AddImpostor(this);
					impostorSystem = system;

					SetWorldBoundingBox(Point3D(position.x - size.x, position.y - size.x, position.z), Point3D(position.x + size.x, position.y + size.x, position.z + size.y * 2.0F));

					Node *node = GetFirstSubnode();
					while (node)
					{
						if (node->GetNodeType() == kNodeGeometry)
						{
							static_cast<Geometry *>(node)->SetRenderParameterPointer(renderParameter);
						}

						node = GetNextNode(node);
					}

					return;
				}

				process = process->GetNextElement();
			}
		}
	}

	// Take the impostor out of the geometry cell graph if no appropriate
	// impostor material was found.

	PurgeIncomingEdges();
}

void Impostor::Neutralize(void)
{
	if (impostorSystem)
	{
		impostorSystem->RemoveImpostor(this);
		impostorSystem = nullptr;
	}

	Node::Neutralize();
}

void Impostor::HandleTransformUpdate(void)
{
	Node::HandleTransformUpdate();

	// Explicitly set the world bounding box here because we don't want to return a node bounding box
	// from the CalculateBoundingBox() function that would get transformed into a bigger world bounding box.

	if (impostorSystem)
	{
		const Point3D& position = GetWorldPosition();
		const Vector2D& size = impostorSize;
		SetWorldBoundingBox(Point3D(position.x - size.x, position.y - size.x, position.z), Point3D(position.x + size.x, position.y + size.x, position.z + size.y * 2.0F));
	}
}

void Impostor::SetMaterialObject(MaterialObject *object)
{
	MaterialObject *prevObject = materialObject;
	if (prevObject != object)
	{
		if (prevObject)
		{
			prevObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		materialObject = object;
	}
}


ImpostorSystem::ImpostorSystem(MaterialObject *material, const float *clipData) :
		attributeVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		indexVertexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		impostorRenderable(kRenderIndexedTriangles, kRenderDepthTest)
{
	materialObject = material;

	if (clipData)
	{
		geometryRadius[0] = clipData[0];
		geometryRadius[1] = clipData[1];

		float h1 = clipData[2];
		float h2 = clipData[3];
		geometryHeight[0] = h1;
		geometryHeight[1] = h2;

		if (h1 == h2)
		{
			geometryTriangleCount = 4;
			impostorRenderProc = &ImpostorSystem::RenderImpostor2;
		}
		else
		{
			geometryTriangleCount = 6;
			impostorRenderProc = &ImpostorSystem::RenderImpostor3;
		}
	}
	else
	{
		geometryTriangleCount = 2;
		impostorRenderProc = &ImpostorSystem::RenderImpostor1;
	}

	buildFlag = false;
	impostorTriangle[0] = nullptr;

	RenderSegment *segment = impostorRenderable.GetFirstRenderSegment();
	segment->SetMaterialObjectPointer(&materialObject);

	impostorRenderable.SetRenderableFlags(kRenderableStructureBufferInhibit);
	impostorRenderable.SetShaderFlags(kShaderVertexPostboard);
	impostorRenderable.SetRenderParameterPointer(&renderParameter);

	impostorRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &attributeVertexBuffer, sizeof(ImpostorVertex));
	impostorRenderable.SetVertexBuffer(kVertexBufferIndexArray, &indexVertexBuffer);

	impostorRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	impostorRenderable.SetVertexAttributeArray(kArrayRadius, sizeof(Point3D), 3);
	impostorRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector3D), 2);
}

ImpostorSystem::~ImpostorSystem()
{
	delete[] impostorTriangle[0];
}

void ImpostorSystem::RemoveImpostor(Impostor *impostor)
{
	buildFlag = true;
	impostor->ListElement<Impostor>::Detach();

	if (impostorList.Empty())
	{
		delete this;
	}
}

void ImpostorSystem::Build(void)
{
	if (buildFlag)
	{
		buildFlag = false;

		for (machine a = 0; a < kImpostorRenderCount; a++)
		{
			impostorRenderCount[a] = 0;
		}

		Impostor *impostor = impostorList.First();
		float distance = impostor->GetRenderDistance();
		float scale = 1.0F / impostor->GetTransitionLength();
		renderParameter.Set(scale, -distance * scale, 0.0F, 0.0F);

		int32 impostorCount = 0;
		do
		{
			impostor->impostorIndex = impostorCount++;
			impostor = impostor->Next();
		} while (impostor);

		int32 vertexCount = impostorCount * (geometryTriangleCount + 2);
		impostorRenderable.SetVertexCount(vertexCount);

		attributeVertexBuffer.Establish(vertexCount * sizeof(ImpostorVertex));
		FillAttributeVertexBuffer(&attributeVertexBuffer);

		int32 triangleCount = impostorCount * geometryTriangleCount;
		indexVertexBuffer.Establish(triangleCount * sizeof(Triangle));

		delete[] impostorTriangle[0];

		impostorTriangle[0] = new Triangle[triangleCount * kImpostorRenderCount];
		for (machine a = 1; a < kImpostorRenderCount; a++)
		{
			impostorTriangle[a] = impostorTriangle[a - 1] + triangleCount;
		}

		impostorRenderable.SetAmbientEnvironment(impostorList.First()->GetOwningZone()->GetAmbientEnvironment());
		impostorRenderable.InvalidateVertexData();
	}
}

void ImpostorSystem::FillAttributeVertexBuffer(VertexBuffer *vertexBuffer)
{
	volatile ImpostorVertex *restrict vertex = vertexBuffer->BeginUpdate<ImpostorVertex>();

	int32 triangleCount = geometryTriangleCount;
	if (triangleCount == 2)
	{
		const Impostor *impostor = impostorList.First();
		do
		{
			const Point3D& position = impostor->GetWorldPosition();
			const Vector3D& direction = impostor->GetWorldTransform()[0];
			float radius = impostor->impostorSize.x;
			float height = impostor->impostorSize.y * 2.0F;

			vertex[0].position = position;
			vertex[1].position = position;
			vertex[2].position.Set(position.x, position.y, position.z + height);
			vertex[3].position.Set(position.x, position.y, position.z + height);

			vertex[0].direction.Set(-radius, direction.x, direction.y);
			vertex[1].direction.Set(radius, direction.x, direction.y);
			vertex[2].direction.Set(-radius, direction.x, direction.y);
			vertex[3].direction.Set(radius, direction.x, direction.y);

			vertex[0].texcoord.Set(0.0F, 0.0F);
			vertex[1].texcoord.Set(1.0F, 0.0F);
			vertex[2].texcoord.Set(0.0F, 1.0F);
			vertex[3].texcoord.Set(1.0F, 1.0F);

			vertex += 4;
			impostor = impostor->Next();
		} while (impostor);
	}
	else if (triangleCount == 4)
	{
		float r1 = geometryRadius[0];
		float r2 = geometryRadius[1];
		float h = geometryHeight[0];

		const Impostor *impostor = impostorList.First();
		do
		{
			const Point3D& position = impostor->GetWorldPosition();
			const Vector3D& direction = impostor->GetWorldTransform()[0];
			float radius = impostor->impostorSize.x;
			float height = impostor->impostorSize.y * 2.0F;

			vertex[0].position = position;
			vertex[1].position = position;
			vertex[2].position.Set(position.x, position.y, position.z + height * h);
			vertex[3].position.Set(position.x, position.y, position.z + height * h);
			vertex[4].position.Set(position.x, position.y, position.z + height);
			vertex[5].position.Set(position.x, position.y, position.z + height);

			vertex[0].direction.Set(-radius * r1, direction.x, direction.y);
			vertex[1].direction.Set(radius * r1, direction.x, direction.y);
			vertex[2].direction.Set(-radius, direction.x, direction.y);
			vertex[3].direction.Set(radius, direction.x, direction.y);
			vertex[4].direction.Set(-radius * r2, direction.x, direction.y);
			vertex[5].direction.Set(radius * r2, direction.x, direction.y);

			vertex[0].texcoord.Set(0.5F - r1 * 0.5F, 0.0F);
			vertex[1].texcoord.Set(0.5F + r1 * 0.5F, 0.0F);
			vertex[2].texcoord.Set(0.0F, h);
			vertex[3].texcoord.Set(1.0F, h);
			vertex[4].texcoord.Set(0.5F - r2 * 0.5F, 1.0F);
			vertex[5].texcoord.Set(0.5F + r2 * 0.5F, 1.0F);

			vertex += 6;
			impostor = impostor->Next();
		} while (impostor);
	}
	else
	{
		float r1 = geometryRadius[0];
		float r2 = geometryRadius[1];
		float h1 = geometryHeight[0];
		float h2 = geometryHeight[1];

		const Impostor *impostor = impostorList.First();
		do
		{
			const Point3D& position = impostor->GetWorldPosition();
			const Vector3D& direction = impostor->GetWorldTransform()[0];
			float radius = impostor->impostorSize.x;
			float height = impostor->impostorSize.y * 2.0F;

			vertex[0].position = position;
			vertex[1].position = position;
			vertex[2].position.Set(position.x, position.y, position.z + height * h1);
			vertex[3].position.Set(position.x, position.y, position.z + height * h1);
			vertex[4].position.Set(position.x, position.y, position.z + height * h2);
			vertex[5].position.Set(position.x, position.y, position.z + height * h2);
			vertex[6].position.Set(position.x, position.y, position.z + height);
			vertex[7].position.Set(position.x, position.y, position.z + height);

			vertex[0].direction.Set(-radius * r1, direction.x, direction.y);
			vertex[1].direction.Set(radius * r1, direction.x, direction.y);
			vertex[2].direction.Set(-radius, direction.x, direction.y);
			vertex[3].direction.Set(radius, direction.x, direction.y);
			vertex[4].direction.Set(-radius, direction.x, direction.y);
			vertex[5].direction.Set(radius, direction.x, direction.y);
			vertex[6].direction.Set(-radius * r2, direction.x, direction.y);
			vertex[7].direction.Set(radius * r2, direction.x, direction.y);

			vertex[0].texcoord.Set(0.5F - r1 * 0.5F, 0.0F);
			vertex[1].texcoord.Set(0.5F + r1 * 0.5F, 0.0F);
			vertex[2].texcoord.Set(0.0F, h1);
			vertex[3].texcoord.Set(1.0F, h1);
			vertex[4].texcoord.Set(0.0F, h2);
			vertex[5].texcoord.Set(1.0F, h2);
			vertex[6].texcoord.Set(0.5F - r2 * 0.5F, 1.0F);
			vertex[7].texcoord.Set(0.5F + r2 * 0.5F, 1.0F);

			vertex += 8;
			impostor = impostor->Next();
		} while (impostor);
	}

	vertexBuffer->EndUpdate();
}

void ImpostorSystem::RenderImpostor1(const Impostor *impostor, int32 renderIndex)
{
	int32 count = impostorRenderCount[renderIndex];
	impostorRenderCount[renderIndex] = count + 1;

	unsigned_int32 i1 = impostor->impostorIndex * 4;
	unsigned_int32 i2 = i1 + 1;
	unsigned_int32 i3 = i1 + 2;
	unsigned_int32 i4 = i1 + 3;

	Triangle *triangle = &impostorTriangle[renderIndex][count * 2];
	triangle[0].Set(i1, i2, i3);
	triangle[1].Set(i2, i4, i3);
}

void ImpostorSystem::RenderImpostor2(const Impostor *impostor, int32 renderIndex)
{
	int32 count = impostorRenderCount[renderIndex];
	impostorRenderCount[renderIndex] = count + 1;

	unsigned_int32 i1 = impostor->impostorIndex * 6;
	unsigned_int32 i2 = i1 + 1;
	unsigned_int32 i3 = i1 + 2;
	unsigned_int32 i4 = i1 + 3;
	unsigned_int32 i5 = i1 + 4;
	unsigned_int32 i6 = i1 + 5;

	Triangle *triangle = &impostorTriangle[renderIndex][count * 4];
	triangle[0].Set(i1, i2, i3);
	triangle[1].Set(i2, i4, i3);
	triangle[2].Set(i3, i4, i5);
	triangle[3].Set(i4, i6, i5);
}

void ImpostorSystem::RenderImpostor3(const Impostor *impostor, int32 renderIndex)
{
	int32 count = impostorRenderCount[renderIndex];
	impostorRenderCount[renderIndex] = count + 1;

	unsigned_int32 i1 = impostor->impostorIndex * 8;
	unsigned_int32 i2 = i1 + 1;
	unsigned_int32 i3 = i1 + 2;
	unsigned_int32 i4 = i1 + 3;
	unsigned_int32 i5 = i1 + 4;
	unsigned_int32 i6 = i1 + 5;
	unsigned_int32 i7 = i1 + 6;
	unsigned_int32 i8 = i1 + 7;

	Triangle *triangle = &impostorTriangle[renderIndex][count * 6];
	triangle[0].Set(i1, i2, i3);
	triangle[1].Set(i2, i4, i3);
	triangle[2].Set(i3, i4, i5);
	triangle[3].Set(i4, i6, i5);
	triangle[4].Set(i5, i6, i7);
	triangle[5].Set(i6, i8, i7);
}

void ImpostorSystem::Render(Array<Renderable *> *renderArray, int32 renderIndex)
{
	int32 count = impostorRenderCount[renderIndex];
	if (count != 0)
	{
		impostorRenderCount[renderIndex] = 0;

		int32 triangleCount = count * geometryTriangleCount;
		impostorRenderable.SetPrimitiveCount(triangleCount);
		indexVertexBuffer.UpdateBuffer(0, triangleCount * sizeof(Triangle), impostorTriangle[renderIndex]);
		renderArray->AddElement(&impostorRenderable);
	}
}

// ZYUQURM
