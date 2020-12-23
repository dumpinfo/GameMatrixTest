 

#include "C4Skybox.h"
#include "C4Cameras.h"
#include "C4Configuration.h"


using namespace C4;


Skybox::Skybox(unsigned_int32 flags) :
		Node(kNodeSkybox),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferStatic)
{
	skyboxFlags = flags;
	texcoordAdjustment = 0.0F;

	materialObject = nullptr;

	for (machine a = 0; a < 6; a++)
	{
		new(faceRenderable[a]) Renderable(kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit);
	}
}

Skybox::Skybox(const Skybox& skybox) :
		Node(skybox),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferStatic)
{
	skyboxFlags = skybox.skyboxFlags;
	texcoordAdjustment = skybox.texcoordAdjustment;

	materialObject = skybox.materialObject;
	if (materialObject)
	{
		materialObject->Retain();
	}

	for (machine a = 0; a < 6; a++)
	{
		new(faceRenderable[a]) Renderable(kRenderTriangleStrip, kRenderDepthTest | kRenderDepthInhibit);
	}
}

Skybox::~Skybox()
{
	for (machine a = 5; a >= 0; a--)
	{
		faceRenderable[a]->~Renderable();
	}

	if (materialObject)
	{
		materialObject->Release();
	}
}

Node *Skybox::Replicate(void) const
{
	return (new Skybox(*this));
}

void Skybox::Prepack(List<Object> *linkList) const
{
	Node::Prepack(linkList);
	if (materialObject)
	{
		linkList->Append(materialObject);
	}
}

void Skybox::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << skyboxFlags;

	data << ChunkHeader('TEXC', 4);
	data << texcoordAdjustment;

	if ((materialObject) && (!(packFlags & kPackSettings)))
	{
		data << ChunkHeader('MATL', 4);
		data << materialObject->GetObjectIndex();
	}

	for (machine a = 0; a < 6; a++)
	{
		const ResourceName& name = textureAttribute[a].GetTextureName();
		if (name[0] != 0)
		{
			PackHandle handle = data.BeginChunk('FACE');
			data << int32(a);
			data << name;
			data.EndChunk(handle);
		}
	}

	data << TerminatorChunk;
}

void Skybox::Unpack(Unpacker& data, unsigned_int32 unpackFlags) 
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Skybox>(data, unpackFlags); 
}
 
bool Skybox::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType) 
	{
		case 'FLAG': 
 
			data >> skyboxFlags;
			return (true);

		case 'TEXC': 

			data >> texcoordAdjustment;
			return (true);

		case 'MATL':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &MaterialObjectLinkProc, this);
			return (true);
		}

		case 'FACE':
		{
			int32			index;
			ResourceName	name;

			data >> index;
			data >> name;
			textureAttribute[index].SetTexture(name);
			return (true);
		}
	}

	return (false);
}

void *Skybox::BeginSettingsUnpack(void)
{
	for (machine a = 0; a < 6; a++)
	{
		textureAttribute[a].SetTexture(static_cast<const char *>(nullptr));
	}

	return (Node::BeginSettingsUnpack());
}

void Skybox::MaterialObjectLinkProc(Object *object, void *cookie)
{
	Skybox *skybox = static_cast<Skybox *>(cookie);
	skybox->SetMaterialObject(static_cast<MaterialObject *>(object));
}

int32 Skybox::GetCategoryCount(void) const
{
	return (Node::GetCategoryCount() + 1);
}

Type Skybox::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kNodeSkybox));
		return (kNodeSkybox);
	}

	return (Node::GetCategoryType(index - 1, title));
}

int32 Skybox::GetCategorySettingCount(Type category) const
{
	if (category == kNodeSkybox)
	{
		return (14);
	}

	return (Node::GetCategorySettingCount(category));
}

Setting *Skybox::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kNodeSkybox)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kNodeSkybox, 'SKYB'));
			return (new HeadingSetting(kNodeSkybox, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kNodeSkybox, 'SKYB', 'HORZ'));
			return (new BooleanSetting('HORZ', ((skyboxFlags & kSkyboxHorizon) != 0), title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kNodeSkybox, 'SKYB', 'SMER'));
			return (new BooleanSetting('SMER', ((skyboxFlags & kSkyboxSmearTexture) != 0), title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kNodeSkybox, 'SKYB', 'TEXC'));
			return (new TextSetting('TEXC', texcoordAdjustment, title));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kNodeSkybox, 'SKYB', 'GLOW'));
			return (new BooleanSetting('GLOW', ((skyboxFlags & kSkyboxGlowEnable) != 0), title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID(kNodeSkybox, 'SKYB', 'RFOG'));
			return (new BooleanSetting('RFOG', ((skyboxFlags & kSkyboxFogInhibit) == 0), title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID(kNodeSkybox, 'TXTR'));
			return (new HeadingSetting('TXTR', title));
		}

		if ((index >= 7) && (index <= 12))
		{
			index -= 7;
			Type identifier = 'FAC0' + index;

			const char *title = table->GetString(StringID(kNodeSkybox, 'TXTR', identifier));
			const char *picker = table->GetString(StringID(kNodeSkybox, 'TXTR', 'PICK'));
			return (new ResourceSetting(identifier, GetFaceTextureName(index), title, picker, TextureResource::GetDescriptor()));
		}

		return (nullptr);
	}

	return (Node::GetCategorySetting(category, index, flags));
}

void Skybox::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kNodeSkybox)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'HORZ')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				skyboxFlags |= kSkyboxHorizon;
			}
			else
			{
				skyboxFlags &= ~kSkyboxHorizon;
			}
		}
		else if (identifier == 'SMER')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				skyboxFlags |= kSkyboxSmearTexture;
			}
			else
			{
				skyboxFlags &= ~kSkyboxSmearTexture;
			}
		}
		else if (identifier == 'TEXC')
		{
			texcoordAdjustment = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'GLOW')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				skyboxFlags |= kSkyboxGlowEnable;
			}
			else
			{
				skyboxFlags &= ~kSkyboxGlowEnable;
			}
		}
		else if (identifier == 'RFOG')
		{
			if (!static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				skyboxFlags |= kSkyboxFogInhibit;
			}
			else
			{
				skyboxFlags &= ~kSkyboxFogInhibit;
			}
		}
		else if ((identifier >= 'FAC0') && (identifier <= 'FAC5'))
		{
			const char *name = static_cast<const ResourceSetting *>(setting)->GetResourceName();
			SetFaceTextureName(identifier - 'FAC0', name);
		}
	}
	else
	{
		Node::SetCategorySetting(category, setting);
	}
}

void Skybox::SetMaterialObject(MaterialObject *object)
{
	if (materialObject != object)
	{
		if (materialObject)
		{
			materialObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		materialObject = object;
	}
}

void Skybox::InvalidateShaderData(void)
{
	for (machine a = 0; a < 6; a++)
	{
		faceRenderable[a]->InvalidateShaderData();
	}
}

void Skybox::Preprocess(void)
{
	Node::Preprocess();

	int32 vertexCount = 0;

	unsigned_int32 shaderFlags = kShaderVertexInfinite;
	unsigned_int32 blendState = kBlendReplace;

	unsigned_int32 renderableFlags = kRenderableStructureBufferInhibit;
	if (skyboxFlags & kSkyboxFogInhibit)
	{
		renderableFlags |= kRenderableFogInhibit;
	}

	unsigned_int32 materialState = (skyboxFlags & kSkyboxGlowEnable) ? kMaterialEmissionGlow : 0;

	for (machine a = 0; a < 6; a++)
	{
		faceRenderFlag[a] = false;

		if (textureAttribute[a].GetTextureName()[0] != 0)
		{
			faceRenderFlag[a] = true;
			vertexCount += 4;

			Renderable *renderable = faceRenderable[a];
			renderable->SetRenderableFlags(renderableFlags);
			renderable->SetShaderFlags(shaderFlags);
			renderable->SetAmbientBlendState(blendState);
			renderable->SetTransformable(this);
			renderable->SetVertexCount(4);
			renderable->SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(SkyboxVertex));

			attributeList[a].Append(&textureAttribute[a]);
			renderable->SetMaterialAttributeList(&attributeList[a]);
			renderable->SetMaterialObjectPointer(&materialObject);
			renderable->GetFirstRenderSegment()->SetMaterialState(materialState);
		}
	}

	vertexBuffer.Establish(sizeof(SkyboxVertex) * vertexCount);
	if (vertexCount != 0)
	{
		unsigned_int32 vertexOffset = 0;

		for (machine a = 0; a < 6; a++)
		{
			if (faceRenderFlag[a])
			{
				Renderable *renderable = faceRenderable[a];
				renderable->SetVertexAttributeArray(kArrayPosition, vertexOffset, 3);
				renderable->SetVertexAttributeArray(kArrayTexcoord, vertexOffset + sizeof(Point3D), 2);

				vertexOffset += sizeof(SkyboxVertex) * 4;
			}
		}

		volatile SkyboxVertex *restrict vertex = vertexBuffer.BeginUpdateSync<SkyboxVertex>();

		float bottomPosition = (skyboxFlags & kSkyboxHorizon) ? 0.0F : -1.0F;
		float bottomTexcoord = (skyboxFlags & kSkyboxSmearTexture) ? texcoordAdjustment - 1.0F : texcoordAdjustment;

		if (faceRenderFlag[0])
		{
			vertex[0].position.Set(1.0F, 1.0F, bottomPosition);
			vertex[0].texcoord.Set(0.0F, bottomTexcoord);

			vertex[1].position.Set(1.0F, -1.0F, bottomPosition);
			vertex[1].texcoord.Set(1.0F, bottomTexcoord);

			vertex[2].position.Set(1.0F, 1.0F, 1.0F);
			vertex[2].texcoord.Set(0.0F, 1.0F);

			vertex[3].position.Set(1.0F, -1.0F, 1.0F);
			vertex[3].texcoord.Set(1.0F, 1.0F);

			vertex += 4;
		}

		if (faceRenderFlag[1])
		{
			vertex[0].position.Set(-1.0F, -1.0F, bottomPosition);
			vertex[0].texcoord.Set(0.0F, bottomTexcoord);

			vertex[1].position.Set(-1.0F, 1.0F, bottomPosition);
			vertex[1].texcoord.Set(1.0F, bottomTexcoord);

			vertex[2].position.Set(-1.0F, -1.0F, 1.0F);
			vertex[2].texcoord.Set(0.0F, 1.0F);

			vertex[3].position.Set(-1.0F, 1.0F, 1.0F);
			vertex[3].texcoord.Set(1.0F, 1.0F);

			vertex += 4;
		}

		if (faceRenderFlag[2])
		{
			vertex[0].position.Set(-1.0F, 1.0F, bottomPosition);
			vertex[0].texcoord.Set(0.0F, bottomTexcoord);

			vertex[1].position.Set(1.0F, 1.0F, bottomPosition);
			vertex[1].texcoord.Set(1.0F, bottomTexcoord);

			vertex[2].position.Set(-1.0F, 1.0F, 1.0F);
			vertex[2].texcoord.Set(0.0F, 1.0F);

			vertex[3].position.Set(1.0F, 1.0F, 1.0F);
			vertex[3].texcoord.Set(1.0F, 1.0F);

			vertex += 4;
		}

		if (faceRenderFlag[3])
		{
			vertex[0].position.Set(1.0F, -1.0F, bottomPosition);
			vertex[0].texcoord.Set(0.0F, bottomTexcoord);

			vertex[1].position.Set(-1.0F, -1.0F, bottomPosition);
			vertex[1].texcoord.Set(1.0F, bottomTexcoord);

			vertex[2].position.Set(1.0F, -1.0F, 1.0F);
			vertex[2].texcoord.Set(0.0F, 1.0F);

			vertex[3].position.Set(-1.0F, -1.0F, 1.0F);
			vertex[3].texcoord.Set(1.0F, 1.0F);

			vertex += 4;
		}

		if (faceRenderFlag[4])
		{
			vertex[0].position.Set(1.0F, -1.0F, 1.0F);
			vertex[0].texcoord.Set(0.0F, 0.0F);

			vertex[1].position.Set(-1.0F, -1.0F, 1.0F);
			vertex[1].texcoord.Set(1.0F, 0.0F);

			vertex[2].position.Set(1.0F, 1.0F, 1.0F);
			vertex[2].texcoord.Set(0.0F, 1.0F);

			vertex[3].position.Set(-1.0F, 1.0F, 1.0F);
			vertex[3].texcoord.Set(1.0F, 1.0F);

			vertex += 4;
		}

		if (faceRenderFlag[5])
		{
			vertex[0].position.Set(1.0F, 1.0F, -1.0F);
			vertex[0].texcoord.Set(0.0F, 0.0F);

			vertex[1].position.Set(-1.0F, 1.0F, -1.0F);
			vertex[1].texcoord.Set(1.0F, 0.0F);

			vertex[2].position.Set(1.0F, -1.0F, -1.0F);
			vertex[2].texcoord.Set(0.0F, 1.0F);

			vertex[3].position.Set(-1.0F, -1.0F, -1.0F);
			vertex[3].texcoord.Set(1.0F, 1.0F);

			vertex += 4;
		}

		vertexBuffer.EndUpdateSync();
	}
}

void Skybox::Neutralize(void)
{
	for (machine a = 0; a < 6; a++)
	{
		faceRenderable[a]->InvalidateVertexData();
	}

	vertexBuffer.Establish(0);
	Node::Neutralize();
}

void Skybox::Render(const FrustumCamera *camera, Array<Renderable *, 6> *renderArray)
{
	Vector3D direction = GetInverseWorldTransform() * camera->GetWorldTransform()[2];
	float sineHalfField = camera->GetSineHalfField();

	if ((faceRenderFlag[0]) && (direction.x > -sineHalfField))
	{
		renderArray->AddElement(faceRenderable[0]);
	}

	if ((faceRenderFlag[1]) && (direction.x < sineHalfField))
	{
		renderArray->AddElement(faceRenderable[1]);
	}

	if ((faceRenderFlag[2]) && (direction.y > -sineHalfField))
	{
		renderArray->AddElement(faceRenderable[2]);
	}

	if ((faceRenderFlag[3]) && (direction.y < sineHalfField))
	{
		renderArray->AddElement(faceRenderable[3]);
	}

	if ((faceRenderFlag[4]) && (direction.z > -sineHalfField))
	{
		renderArray->AddElement(faceRenderable[4]);
	}

	if ((faceRenderFlag[5]) && (direction.z < sineHalfField))
	{
		renderArray->AddElement(faceRenderable[5]);
	}
}

// ZYUQURM
