 

#include "C4Models.h"
#include "C4World.h"
#include "C4Configuration.h"


using namespace C4;


namespace
{
	const float kMorphEpsilon = 1.0e-6F;
}


namespace C4
{
	template class Registrable<Model, ModelRegistration>;
}


ResourceDescriptor ModelResource::descriptor("mdl", 0, 0, "C4/missing");


ModelResource::ModelResource(const char *name, ResourceCatalog *catalog) : Resource<ModelResource>(name, catalog)
{
}

ModelResource::~ModelResource()
{
}


Bone::Bone() : Node(kNodeBone)
{
	skeletonRoot = nullptr;

	boundingBoxFlag = false;
	boneBoundingBox.Set(Zero3D, Zero3D);
}

Bone::Bone(const Bone& bone) : Node(bone)
{
	skeletonRoot = nullptr;

	boundingBoxFlag = false;
	boneBoundingBox.Set(Zero3D, Zero3D);
}

Bone::~Bone()
{
}

Node *Bone::Replicate(void) const
{
	return (new Bone(*this));
}

void Bone::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	data << TerminatorChunk;
}

void Bone::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Bone>(data, unpackFlags);
}

bool Bone::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

void Bone::HandlePostprocessUpdate(void)
{
	modelTransform = skeletonRoot->GetInverseWorldTransform() * GetWorldTransform();
}

bool Bone::CalculateBoundingBox(Box3D *box) const
{
	*box = boneBoundingBox;
	return (true);
}

bool Bone::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(boneBoundingBox.GetCenter());
	sphere->SetRadius(Magnitude(boneBoundingBox.GetSize()) * 0.5F);
	return (true);
}

void Bone::Preprocess(void)
{
	Node::Preprocess();

	Node *node = GetSuperNode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel) 
		{
			skeletonRoot = node;
			SetActiveUpdateFlags(kUpdateTransform | kUpdatePostprocess); 
			break;
		} 

		node = node->GetSuperNode();
	} 

	if ((!skeletonRoot) && (GetManipulator())) 
	{ 
		node = GetSuperNode();
		while (node)
		{
			if (node->GetNodeType() == kNodeZone) 
			{
				skeletonRoot = node;
				SetActiveUpdateFlags(kUpdateTransform | kUpdatePostprocess);
				break;
			}

			node = node->GetSuperNode();
		}
	}
}

void Bone::Neutralize(void)
{
	boundingBoxFlag = false;
	Node::Neutralize();
}

void Bone::SetBoneBoundingBox(const Box3D& box)
{
	if (!boundingBoxFlag)
	{
		boundingBoxFlag = true;
		boneBoundingBox = box;
	}
	else
	{
		boneBoundingBox.Union(box);
	}
}


MorphController::MorphController() :
		Controller(kControllerMorph),
		morphUpdateJob(&JobUpdateMorph, &FinalizeMorphUpdate, this),
		morphIsolatedUpdateJob(&JobUpdateMorphIsolated, &FinalizeMorphIsolatedUpdate, this),
		morphVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetBaseControllerType(kControllerMorph);
	SetControllerFlags(kControllerLocal);

	morphUpdateFlag = false;
	baseMeshWeight = 1.0F;
	morphStorage = nullptr;
}

MorphController::MorphController(ControllerType type) :
		Controller(type),
		morphUpdateJob(&JobUpdateMorph, &FinalizeMorphUpdate, this),
		morphIsolatedUpdateJob(&JobUpdateMorphIsolated, &FinalizeMorphIsolatedUpdate, this),
		morphVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetBaseControllerType(kControllerMorph);
	SetControllerFlags(kControllerLocal);

	morphUpdateFlag = false;
	baseMeshWeight = 1.0F;
	morphStorage = nullptr;
}

MorphController::MorphController(const MorphController& morphController) :
		Controller(morphController),
		morphWeightArray(morphController.morphWeightArray),
		morphUpdateJob(&JobUpdateMorph, &FinalizeMorphUpdate, this),
		morphIsolatedUpdateJob(&JobUpdateMorphIsolated, &FinalizeMorphIsolatedUpdate, this),
		morphVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetControllerFlags(kControllerLocal);

	morphUpdateFlag = false;
	baseMeshWeight = morphController.baseMeshWeight;
	morphStorage = nullptr;
}

MorphController::~MorphController()
{
	delete[] morphStorage;
}

Controller *MorphController::Replicate(void) const
{
	return (new MorphController(*this));
}

MorphController::MorphJob::MorphJob(ExecuteProc *execProc, FinalizeProc *finalProc, void *cookie) : BatchJob(execProc, finalProc, cookie)
{
}

bool MorphController::ValidNode(const Node *node)
{
	if (node->GetNodeType() == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<const Geometry *>(node);
		if (geometry->GetGeometryType() == kGeometryGeneric)
		{
			return (geometry->GetObject()->GetGeometryLevel(0)->GetMorphTargetData() != nullptr);
		}
	}

	return (false);
}

void MorphController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('BASE', 4);
	data << baseMeshWeight;

	int32 count = morphWeightArray.GetElementCount();
	if (count != 0)
	{
		data << ChunkHeader('MCNT', 4);
		data << count;

		data << ChunkHeader('WGHT', count * 4);
		for (machine a = 0; a < count; a++)
		{
			data << morphWeightArray[a].weight;
		}

		data << ChunkHeader('HASH', count * 4);
		for (machine a = 0; a < count; a++)
		{
			data << morphWeightArray[a].hash;
		}
	}

	data << TerminatorChunk;
}

void MorphController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<MorphController>(data, unpackFlags);
}

bool MorphController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BASE':

			data >> baseMeshWeight;
			return (true);

		case 'MCNT':
		{
			int32	count;

			data >> count;
			morphWeightArray.SetElementCount(count);
			return (true);
		}

		case 'WGHT':
		{
			int32 count = morphWeightArray.GetElementCount();
			for (machine a = 0; a < count; a++)
			{
				data >> morphWeightArray[a].weight;
			}

			return (true);
		}

		case 'HASH':
		{
			int32 count = morphWeightArray.GetElementCount();
			for (machine a = 0; a < count; a++)
			{
				data >> morphWeightArray[a].hash;
			}

			return (true);
		}
	}

	return (false);
}

int32 MorphController::GetSettingCount(void) const
{
	int32 count = morphWeightArray.GetElementCount();
	return ((count != 0) ? count + 2 : 0);
}

Setting *MorphController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMorph, 'WGHT'));
		return (new HeadingSetting('WGHT', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMorph, 'BASE'));
		return (new IntegerSetting('BASE', (int32) (baseMeshWeight * 100.0F + 0.5F), title, 0, 100, 1));
	}

	index -= 2;
	if (index < morphWeightArray.GetElementCount())
	{
		String<127> title(table->GetString(StringID('CTRL', kControllerMorph, 'T000')));
		return (new IntegerSetting('T000' + index, (int32) (morphWeightArray[index].weight * 100.0F + 0.5F), title += index + 1, 0, 100, 1));
	}

	return (nullptr);
}

void MorphController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'BASE')
	{
		baseMeshWeight = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
	}
	else
	{
		int32 index = identifier - 'T000';
		if ((unsigned_int32) index < (unsigned_int32) morphWeightArray.GetElementCount())
		{
			morphWeightArray[index].weight = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
	}
}

void MorphController::Preprocess(void)
{
	Controller::Preprocess();

	delete[] morphStorage;
	morphStorage = nullptr;

	if (!morphWeightArray.Empty())
	{
		GenericGeometry *geometry = GetTargetNode();
		geometry->SetShaderFlags(geometry->GetShaderFlags() | kShaderNormalizeBasisVectors);
		geometry->SetVertexBufferArrayFlags((1 << kArrayPosition) | (1 << kArrayNormal) | (1 << kArrayTangent));
		geometry->SetVertexAttributeArray(kArrayPosition, 0, 3);
		geometry->SetVertexAttributeArray(kArrayNormal, sizeof(Point3D), 3);
		geometry->SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) + sizeof(Vector3D), 4);

		GeometryObject *object = geometry->GetObject();
		object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);

		const Mesh *mesh = object->GetGeometryLevel(0);
		int32 vertexCount = mesh->GetVertexCount();

		morphVertexBuffer.Establish(vertexCount * sizeof(MorphVertex));
		geometry->SetVertexBuffer(kVertexBufferAttributeArray1, &morphVertexBuffer, sizeof(MorphVertex));

		int32 morphCount = mesh->GetMorphTargetCount();
		unsigned_int32 size = sizeof(MorphTerm) * morphCount;

		// If this is not an isolated morph controller (e.g., it's also a skin controller), or we are
		// inside the World Editor, then the calculated positions, normals, and tangents are stored in
		// main memory. This gives the subclass or editor access to the morphed vertices (for further
		// processing or picking, respectively). In the case that this is just a base morph controller
		// and we're in the editor, then the vertex data is copied to the vertex buffer in the job's
		// finalize callback.

		bool isolatedFlag = ((GetControllerType() == kControllerMorph) && (!geometry->GetManipulator()));
		if (isolatedFlag)
		{
			activeMorphUpdateJob = &morphIsolatedUpdateJob;
		}
		else
		{
			size += (sizeof(Point3D) + sizeof(Vector3D) + sizeof(Vector4D)) * vertexCount;
			activeMorphUpdateJob = &morphUpdateJob;
		}

		morphStorage = new char[size];
		morphTermArray = reinterpret_cast<MorphTerm *>(morphStorage);

		if (!isolatedFlag)
		{
			morphPositionArray = reinterpret_cast<Point3D *>(morphTermArray + morphCount);
			morphNormalArray = morphPositionArray + vertexCount;
			morphTangentArray = reinterpret_cast<Vector4D *>(morphNormalArray + vertexCount);
		}

		const unsigned_int32 *morphHashArray = mesh->GetArray<unsigned_int32>(kArrayMorphHash);
		for (MorphWeight& morphWeight : morphWeightArray)
		{
			unsigned_int32 hash = morphWeight.hash;
			int32 index = -1;

			for (machine a = 0; a < morphCount; a++)
			{
				if (morphHashArray[a] == hash)
				{
					index = a;
					break;
				}
			}

			morphWeight.index = index;
		}

		morphUpdateFlag = true;
	}
}

void MorphController::Neutralize(void)
{
	morphVertexBuffer.Establish(0);

	delete[] morphStorage;
	morphStorage = nullptr;

	Controller::Neutralize();
}

void MorphController::Update(void)
{
	Controller::Update();

	if (morphUpdateFlag)
	{
		morphUpdateFlag = false;

		MorphJob *job = activeMorphUpdateJob;
		job->attributeBuffer = morphVertexBuffer.BeginUpdate();
		GetTargetNode()->GetWorld()->SubmitWorldJob(job);
	}
}

void MorphController::SetDetailLevel(int32 level)
{
	if (!morphWeightArray.Empty())
	{
		morphUpdateFlag = true;
	}

	Controller::SetDetailLevel(level);
}

void MorphController::JobUpdateMorph(Job *job, void *cookie)
{
	MorphController *morphController = static_cast<MorphController *>(cookie);

	GenericGeometry *geometry = morphController->GetTargetNode();
	const GeometryObject *object = geometry->GetObject();
	const Mesh *mesh = object->GetGeometryLevel(geometry->GetDetailLevel());
	int32 vertexCount = mesh->GetVertexCount();

	MorphTerm *morphTerm = morphController->morphTermArray;
	int32 morphWeightCount = morphController->morphWeightArray.GetElementCount();
	const MorphAttrib *morphTargetData = mesh->GetMorphTargetData();

	const MorphWeight *morphWeight = morphController->morphWeightArray;
	for (machine m = 0; m < morphWeightCount; m++)
	{
		float weight = morphWeight->weight;
		int32 index = morphWeight->index;

		if ((index >= 0) && (fabs(weight) > kMorphEpsilon))
		{
			morphTerm->morphWeight = weight;
			morphTerm->morphAttrib = morphTargetData + vertexCount * index;
			morphTerm++;
		}

		morphWeight++;
	}

	int32 morphTermCount = int32(morphTerm - morphController->morphTermArray);

	Point3D *restrict position = morphController->morphPositionArray;
	Vector3D *restrict normal = morphController->morphNormalArray;
	Vector4D *restrict tangent = morphController->morphTangentArray;

	const Point3D *basePosition = mesh->GetArray<Point3D>(kArrayPosition);
	const Vector3D *baseNormal = mesh->GetArray<Vector3D>(kArrayNormal);
	const Vector4D *baseTangent = mesh->GetArray<Vector4D>(kArrayTangent);
	float baseWeight = morphController->baseMeshWeight;

	for (machine a = 0; a < vertexCount; a++)
	{
		Point3D posi = *basePosition * baseWeight;
		Vector3D nrml = *baseNormal * baseWeight;
		Vector3D tang = baseTangent->GetVector3D() * baseWeight;

		morphTerm = morphController->morphTermArray;
		for (machine b = 0; b < morphTermCount; b++)
		{
			float weight = morphTerm->morphWeight;
			const MorphAttrib *morphAttrib = &morphTerm->morphAttrib[a];
			posi += morphAttrib->position * weight;
			nrml += morphAttrib->normal * weight;
			tang += morphAttrib->tangent * weight;

			morphTerm++;
		}

		*position = posi;
		*normal = nrml;
		tangent->Set(tang, baseTangent->w);

		position++;
		normal++;
		tangent++;

		basePosition++;
		baseNormal++;
		baseTangent++;
	}
}

void MorphController::FinalizeMorphUpdate(Job *job, void *cookie)
{
	MorphController *morphController = static_cast<MorphController *>(cookie);

	const MorphJob *morphJob = static_cast<MorphJob *>(job);
	volatile MorphVertex *restrict vertex = static_cast<volatile MorphVertex *>(morphJob->attributeBuffer);

	const Point3D *position = morphController->morphPositionArray;
	const Vector3D *normal = morphController->morphNormalArray;
	const Vector4D *tangent = morphController->morphTangentArray;

	const Geometry *geometry = morphController->GetTargetNode();
	int32 vertexCount = geometry->GetVertexCount();
	for (machine a = 0; a < vertexCount; a++)
	{
		vertex[a].position = position[a];
		vertex[a].normal = normal[a];
		vertex[a].tangent = tangent[a];
	}

	morphController->morphVertexBuffer.EndUpdate();
}

void MorphController::JobUpdateMorphIsolated(Job *job, void *cookie)
{
	MorphController *morphController = static_cast<MorphController *>(cookie);

	GenericGeometry *geometry = morphController->GetTargetNode();
	const GeometryObject *object = geometry->GetObject();
	const Mesh *mesh = object->GetGeometryLevel(geometry->GetDetailLevel());
	int32 vertexCount = mesh->GetVertexCount();

	MorphTerm *morphTerm = morphController->morphTermArray;
	int32 morphWeightCount = morphController->morphWeightArray.GetElementCount();
	const MorphAttrib *morphTargetData = mesh->GetMorphTargetData();

	const MorphWeight *morphWeight = morphController->morphWeightArray;
	for (machine m = 0; m < morphWeightCount; m++)
	{
		float weight = morphWeight->weight;
		int32 index = morphWeight->index;

		if ((index >= 0) && (fabs(weight) > kMorphEpsilon))
		{
			morphTerm->morphWeight = weight;
			morphTerm->morphAttrib = morphTargetData + vertexCount * index;
			morphTerm++;
		}

		morphWeight++;
	}

	int32 morphTermCount = int32(morphTerm - morphController->morphTermArray);

	const MorphJob *morphJob = static_cast<MorphJob *>(job);
	volatile MorphVertex *restrict vertex = static_cast<volatile MorphVertex *>(morphJob->attributeBuffer);

	const Point3D *basePosition = mesh->GetArray<Point3D>(kArrayPosition);
	const Vector3D *baseNormal = mesh->GetArray<Vector3D>(kArrayNormal);
	const Vector4D *baseTangent = mesh->GetArray<Vector4D>(kArrayTangent);
	float baseWeight = morphController->baseMeshWeight;

	for (machine a = 0; a < vertexCount; a++)
	{
		Point3D posi = *basePosition * baseWeight;
		Vector3D nrml = *baseNormal * baseWeight;
		Vector3D tang = baseTangent->GetVector3D() * baseWeight;

		morphTerm = morphController->morphTermArray;
		for (machine b = 0; b < morphTermCount; b++)
		{
			float weight = morphTerm->morphWeight;
			const MorphAttrib *morphAttrib = &morphTerm->morphAttrib[a];
			posi += morphAttrib->position * weight;
			nrml += morphAttrib->normal * weight;
			tang += morphAttrib->tangent * weight;

			morphTerm++;
		}

		vertex->position = posi;
		vertex->normal = nrml;
		vertex->tangent.Set(tang, baseTangent->w);

		vertex++;
		basePosition++;
		baseNormal++;
		baseTangent++;
	}
}

void MorphController::FinalizeMorphIsolatedUpdate(Job *job, void *cookie)
{
	MorphController *morphController = static_cast<MorphController *>(cookie);
	morphController->morphVertexBuffer.EndUpdate();
}


SkinController::SkinController() :
		MorphController(kControllerSkin),
		skinUpdateJob(&JobUpdateSkin, &FinalizeSkinUpdate, this),
		skinVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	SetControllerFlags(kControllerLocal);

	skinStorage = nullptr;
	motionBlurFlag = false;
}

SkinController::SkinController(const SkinController& skinController) :
		MorphController(skinController),
		skinUpdateJob(&JobUpdateSkin, &FinalizeSkinUpdate, this),
		skinVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	skinStorage = nullptr;
	motionBlurFlag = false;
}

SkinController::~SkinController()
{
	delete[] skinStorage;
}

Controller *SkinController::Replicate(void) const
{
	return (new SkinController(*this));
}

bool SkinController::ValidNode(const Node *node)
{
	if (node->GetNodeType() == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<const Geometry *>(node);
		if (geometry->GetGeometryType() == kGeometryGeneric)
		{
			return (geometry->GetObject()->GetGeometryLevel(0)->GetSkinWeightData() != nullptr);
		}
	}

	return (false);
}

void SkinController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MorphController::Pack(data, packFlags);

	data << TerminatorChunk;
}

void SkinController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	#if C4LEGACY

		if (data.GetVersion() >= 72)
		{
			MorphController::Unpack(data, unpackFlags);
		}
		else
		{
			Controller::Unpack(data, unpackFlags);
		}

	#else

		MorphController::Unpack(data, unpackFlags);

	#endif

	UnpackChunkList<SkinController>(data, unpackFlags);
}

bool SkinController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

void SkinController::Preprocess(void)
{
	MorphController::Preprocess();

	GenericGeometry *geometry = GetTargetNode();
	geometry->SetShaderFlags(geometry->GetShaderFlags() | kShaderNormalizeBasisVectors);
	geometry->SetVertexBufferArrayFlags((1 << kArrayPosition) | (1 << kArrayPrevious) | (1 << kArrayNormal) | (1 << kArrayTangent));
	geometry->SetVertexAttributeArray(kArrayPosition, 0, 3);
	geometry->SetVertexAttributeArray(kArrayPrevious, sizeof(Point3D), 3);
	geometry->SetVertexAttributeArray(kArrayNormal, sizeof(Point3D) * 2, 3);
	geometry->SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) * 2 + sizeof(Vector3D), 4);
	geometry->SetMotionBlurBox(&skinBoundingBox);

	GeometryObject *object = geometry->GetObject();
	object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);

	const Mesh *mesh = object->GetGeometryLevel(0);
	int32 vertexCount = mesh->GetVertexCount();

	skinVertexBuffer.Establish(vertexCount * sizeof(SkinVertex));
	geometry->SetVertexBuffer(kVertexBufferAttributeArray1, &skinVertexBuffer, sizeof(SkinVertex));

	int32 boneCount = mesh->GetArrayDescriptor(kArrayBoneHash)->elementCount;
	skinBoneCount = boneCount;

	unsigned_int32 size = (sizeof(Transform4D) + sizeof(Bone *)) * boneCount + (sizeof(Point3D) * 2) * vertexCount;

	delete[] skinStorage;
	char *pointer = new char[size];
	skinStorage = pointer;

	if (boneCount > 0)
	{
		transformTable = reinterpret_cast<Transform4D *>(pointer);
		pointer += sizeof(Transform4D) * boneCount;

		skinBoneTable = reinterpret_cast<Bone **>(pointer);
		pointer += sizeof(Bone *) * boneCount;

		Node *rootNode = geometry->GetSuperNode();
		while (rootNode)
		{
			NodeType type = rootNode->GetNodeType();
			if ((type == kNodeModel) || (type == kNodeZone))
			{
				break;
			}

			rootNode = rootNode->GetSuperNode();
		}

		for (machine a = 0; a < boneCount; a++)
		{
			skinBoneTable[a] = nullptr;
		}

		if (rootNode)
		{
			geometry->SetVisibilityProc(&Node::BoxVisible);
			geometry->SetOcclusionProc(&Node::BoxVisible);
			geometry->SetPostprocessProc(&HandlePostprocessUpdate);

			const unsigned_int32 *boneHash = mesh->GetArray<unsigned_int32>(kArrayBoneHash);

			if (rootNode->GetNodeType() == kNodeModel)
			{
				const Model *model = static_cast<const Model *>(rootNode);
				for (machine a = 0; a < boneCount; a++)
				{
					Node *node = model->FindNode(boneHash[a]);
					if ((node) && (node->GetNodeType() == kNodeBone))
					{
						skinBoneTable[a] = static_cast<Bone *>(node);
					}
				}
			}
			else
			{
				for (machine a = 0; a < boneCount; a++)
				{
					unsigned_int32 hash = boneHash[a];

					Node *node = rootNode->GetFirstSubnode();
					while (node)
					{
						NodeType type = node->GetNodeType();
						if (type == kNodeBone)
						{
							if (node->GetNodeHash() == hash)
							{
								skinBoneTable[a] = static_cast<Bone *>(node);
								break;
							}
						}
						else if (type == kNodeModel)
						{
							node = rootNode->GetNextLevelNode(node);
							continue;
						}

						node = rootNode->GetNextNode(node);
					}
				}
			}
		}
	}

	skinPositionArray[0] = reinterpret_cast<Point3D *>(pointer);
	skinPositionArray[1] = skinPositionArray[0] + vertexCount;

	CalculateBoneBoundingBoxes();
}

void SkinController::Neutralize(void)
{
	skinVertexBuffer.Establish(0);

	delete[] skinStorage;
	skinStorage = nullptr;

	MorphController::Neutralize();
}

void SkinController::CalculateBoneBoundingBoxes(void) const
{
	const Mesh *mesh = GetTargetNode()->GetObject()->GetGeometryLevel(0);
	const Transform4D *transform = mesh->GetArray<Transform4D>(kArrayInverseBindTransform);
	const Point3D *bindPosition = mesh->GetArray<Point3D>(kArrayPosition);
	const SkinWeight *skinWeight = mesh->GetSkinWeightData();

	int32 boneCount = skinBoneCount;
	Box3D *box = new Box3D[boneCount];
	for (machine a = 0; a < boneCount; a++)
	{
		box[a].Set(Zero3D, Zero3D);
	}

	int32 vertexCount = mesh->GetVertexCount();
	for (machine a = 0; a < vertexCount; a++)
	{
		int32 vertexBoneCount = skinWeight->boneCount;
		const BoneWeight *boneWeight = skinWeight->boneWeight;

		#if C4SIMD

			vec_float position = VecLoadUnaligned(&bindPosition->x);

		#endif

		do
		{
			int32 index = boneWeight->boneIndex;
			Box3D& b = box[index];

			#if C4SIMD

				const float *m = &transform[index](0,0);
				vec_float p = VecTransformPoint3D(VecLoad(m, 0), VecLoad(m, 4), VecLoad(m, 8), VecLoad(m, 12), position);
				VecStore3D(VecMin(VecLoadUnaligned(&b.min.x), p), &b.min.x);
				VecStore3D(VecMax(VecLoadUnaligned(&b.max.x), p), &b.max.x);

			#else

				Point3D p = transform[index] * *bindPosition;
				b.min.x = Fmin(b.min.x, p.x);
				b.min.y = Fmin(b.min.y, p.y);
				b.min.z = Fmin(b.min.z, p.z);
				b.max.x = Fmax(b.max.x, p.x);
				b.max.y = Fmax(b.max.y, p.y);
				b.max.z = Fmax(b.max.z, p.z);

			#endif

			boneWeight++;
		} while (--vertexBoneCount > 0);

		skinWeight = reinterpret_cast<const SkinWeight *>(boneWeight);
		bindPosition++;
	}

	for (machine a = 0; a < boneCount; a++)
	{
		Bone *bone = skinBoneTable[a];
		if (bone)
		{
			bone->SetBoneBoundingBox(box[a]);
		}
	}

	delete[] box;
}

void SkinController::HandlePostprocessUpdate(GenericGeometry *geometry)
{
	SkinController *controller = static_cast<SkinController *>(geometry->GetController());

	int32 boneCount = controller->skinBoneCount;
	const Bone *const *boneTable = controller->skinBoneTable;

	for (machine a = 0; a < boneCount; a++)
	{
		const Bone *bone = boneTable[a];
		if (bone)
		{
			Box3D box = bone->GetWorldBoundingBox();

			for (++a; a < boneCount; a++)
			{
				bone = boneTable[a];
				if (bone)
				{
					box.Union(bone->GetWorldBoundingBox());
				}
			}

			geometry->SetWorldBoundingBox(box);
			break;
		}
	}
}

void SkinController::StopMotion(void)
{
	motionBlurFlag = false;
	Invalidate();
}

void SkinController::Update(void)
{
	// Do not call MorphController::Update() because the skin update job will
	// call into the morph controller to calculate the morphed vertex positions.

	Controller::Update();

	skinUpdateJob.attributeBuffer = skinVertexBuffer.BeginUpdate();
	GetTargetNode()->GetWorld()->SubmitWorldJob(&skinUpdateJob);
}

void SkinController::JobUpdateSkin(Job *job, void *cookie)
{
	const Point3D		*bindPosition;
	const Vector3D		*bindNormal;
	const Vector4D		*bindTangent;

	SkinController *skinController = static_cast<SkinController *>(cookie);

	GenericGeometry *geometry = skinController->GetTargetNode();
	const GeometryObject *object = geometry->GetObject();
	const Mesh *mesh = object->GetGeometryLevel(geometry->GetDetailLevel());

	if (skinController->GetMorphWeightCount() == 0)
	{
		bindPosition = mesh->GetArray<Point3D>(kArrayPosition);
		bindNormal = mesh->GetArray<Vector3D>(kArrayNormal);
		bindTangent = mesh->GetArray<Vector4D>(kArrayTangent);
	}
	else
	{
		if (skinController->TakeMorphUpdateFlag())
		{
			JobUpdateMorph(nullptr, static_cast<MorphController *>(skinController));
		}

		bindPosition = skinController->GetMorphPositionArray();
		bindNormal = skinController->GetMorphNormalArray();
		bindTangent = skinController->GetMorphTangentArray();
	}

	const Transform4D *inverseBindTransform = mesh->GetArray<Transform4D>(kArrayInverseBindTransform);

	const Bone *const *boneTable = skinController->skinBoneTable;
	Transform4D *transformTable = skinController->transformTable;

	int32 boneCount = skinController->skinBoneCount;
	for (machine a = 0; a < boneCount; a++)
	{
		const Bone *bone = boneTable[a];
		transformTable[a] = (bone) ? bone->GetModelTransform() * inverseBindTransform[a] : inverseBindTransform[a];
	}

	unsigned_int32 parity = skinController->vertexParity;
	bool motionBlur = skinController->motionBlurFlag;

	Point3D *position = skinController->skinPositionArray[parity];
	const Point3D *previous = skinController->skinPositionArray[parity ^ motionBlur];

	const MorphJob *morphJob = static_cast<MorphJob *>(job);
	volatile SkinVertex *restrict vertex = static_cast<volatile SkinVertex *>(morphJob->attributeBuffer);

	#if C4SIMD

		vec_float minBounds = VecLoadSmearScalar(&K::infinity);
		vec_float maxBounds = VecLoadSmearScalar(&K::minus_infinity);

	#else

		Point3D minBounds(K::infinity, K::infinity, K::infinity);
		Point3D maxBounds(K::minus_infinity, K::minus_infinity, K::minus_infinity);

	#endif

	int32 vertexCount = mesh->GetVertexCount();
	const SkinWeight *skinWeight = mesh->GetSkinWeightData();

	for (machine a = 0; a < vertexCount; a++)
	{
		#if C4SIMD

			vec_float bpos = VecLoadUnaligned(&bindPosition->x);
			vec_float bnrm = VecLoadUnaligned(&bindNormal->x);
			vec_float btan = VecLoadUnaligned(&bindTangent->x);

			int32 vertexBoneCount = skinWeight->boneCount;
			const BoneWeight *boneWeight = skinWeight->boneWeight;

			const float *transform = &transformTable[boneWeight->boneIndex](0,0);
			vec_float c1 = VecLoad(transform, 0);
			vec_float c2 = VecLoad(transform, 4);
			vec_float c3 = VecLoad(transform, 8);
			vec_float c4 = VecLoad(transform, 12);

			vec_float weight = VecLoadSmearScalar(&boneWeight->weight);
			vec_float posi = VecMul(VecTransformPoint3D(c1, c2, c3, c4, bpos), weight);
			vec_float nrml = VecMul(VecTransformVector3D(c1, c2, c3, bnrm), weight);
			vec_float tang = VecMul(VecTransformVector3D(c1, c2, c3, btan), weight);

			while (boneWeight++, --vertexBoneCount)
			{
				transform = &transformTable[boneWeight->boneIndex](0,0);
				c1 = VecLoad(transform, 0);
				c2 = VecLoad(transform, 4);
				c3 = VecLoad(transform, 8);
				c4 = VecLoad(transform, 12);

				weight = VecLoadSmearScalar(&boneWeight->weight);
				posi = VecMadd(VecTransformPoint3D(c1, c2, c3, c4, bpos), weight, posi);
				nrml = VecMadd(VecTransformVector3D(c1, c2, c3, bnrm), weight, nrml);
				tang = VecMadd(VecTransformVector3D(c1, c2, c3, btan), weight, tang);
			}

			VecStore3D(posi, &position->x);
			VecStore3D(posi, const_cast<float *>(&vertex->position.x));
			vertex->previous = *previous;
			VecStore3D(nrml, const_cast<float *>(&vertex->normal.x));
			VecStore3D(tang, const_cast<float *>(&vertex->tangent.x));
			VecStoreW(btan, const_cast<float *>(&vertex->tangent.x), 3);

			minBounds = VecMin(minBounds, posi);
			maxBounds = VecMax(maxBounds, posi);

		#else

			int32 vertexBoneCount = skinWeight->boneCount;
			const BoneWeight *boneWeight = skinWeight->boneWeight;

			float weight = boneWeight->weight;
			const Transform4D *transform = &transformTable[boneWeight->boneIndex];
			Point3D posi = *transform * *bindPosition * weight;
			Vector3D nrml = *transform * *bindNormal * weight;
			Vector3D tang = *transform * bindTangent->GetVector3D() * weight;

			while (boneWeight++, --vertexBoneCount)
			{
				weight = boneWeight->weight;
				transform = &transformTable[boneWeight->boneIndex];
				posi += *transform * *bindPosition * weight;
				nrml += *transform * *bindNormal * weight;
				tang += *transform * bindTangent->GetVector3D() * weight;
			}

			*position = posi;
			vertex->position = posi;
			vertex->previous = *previous;
			vertex->normal = nrml;
			vertex->tangent.Set(tang, bindTangent->w);

			minBounds.x = Fmin(minBounds.x, posi.x);
			minBounds.y = Fmin(minBounds.y, posi.y);
			minBounds.z = Fmin(minBounds.z, posi.z);
			maxBounds.x = Fmax(maxBounds.x, posi.x);
			maxBounds.y = Fmax(maxBounds.y, posi.y);
			maxBounds.z = Fmax(maxBounds.z, posi.z);

		#endif

		skinWeight = reinterpret_cast<const SkinWeight *>(boneWeight);

		position++;
		previous++;
		vertex++;

		bindPosition++;
		bindNormal++;
		bindTangent++;
	}

	#if C4SIMD

		VecStore3D(minBounds, &skinController->skinBoundingBox.min.x);
		VecStore3D(maxBounds, &skinController->skinBoundingBox.max.x);

	#else

		skinController->skinBoundingBox.Set(minBounds, maxBounds);

	#endif
}

void SkinController::FinalizeSkinUpdate(Job *job, void *cookie)
{
	SkinController *skinController = static_cast<SkinController *>(cookie);
	skinController->skinVertexBuffer.EndUpdate();

	skinController->vertexParity ^= 1;
	skinController->motionBlurFlag = true;
}

void SkinController::SetDetailLevel(int32 level)
{
	vertexParity = 0;
	motionBlurFlag = false;

	MorphController::SetDetailLevel(level);
}


ModelRegistration::ModelRegistration(ModelType type, const char *name, const char *rsrcName, unsigned_int32 flags, ControllerType cntrlType, int32 propCount, const PropertyType *propTypeArray) : Registration<Model, ModelRegistration>(type)
{
	modelFlags = flags;
	modelName = name;
	resourceName = rsrcName;
	controllerType = cntrlType;

	propertyCount = propCount;
	propertyTypeArray = propTypeArray;

	prototypeModel = nullptr;
	if (modelFlags & kModelPrecache)
	{
		LoadPrototype();
	}
}

ModelRegistration::~ModelRegistration()
{
	cloneList.RemoveAll();
	delete prototypeModel;
}

Model *ModelRegistration::Create(void) const
{
	return (new Model(GetModelType()));
}

void ModelRegistration::LoadPrototype(void)
{
	Model *model = Model::New(resourceName, GetModelType());
	prototypeModel = model;

	if (model)
	{
		model->Preload();
	}
}

void ModelRegistration::Reload(void)
{
	delete prototypeModel;
	prototypeModel = nullptr;

	if (modelFlags & kModelPrecache)
	{
		LoadPrototype();
	}
}

Model *ModelRegistration::Clone(Model *model)
{
	if (!prototypeModel)
	{
		LoadPrototype();
		if (!prototypeModel)
		{
			return (nullptr);
		}
	}

	if (model)
	{
		prototypeModel->CloneSubtree(model);
	}
	else
	{
		model = static_cast<Model *>(prototypeModel->Clone());
	}

	cloneList.Append(model);
	return (model);
}

Node *ModelRegistration::CloneGroup(void)
{
	if (!prototypeModel)
	{
		LoadPrototype();
		if (!prototypeModel)
		{
			return (nullptr);
		}
	}

	Node *group = new Node;
	prototypeModel->CloneSubtree(group);
	return (group);
}

void ModelRegistration::Retire(Model *model)
{
	if (cloneList.Member(model))
	{
		cloneList.Remove(model);
		if ((cloneList.Empty()) && (!(modelFlags & kModelPrecache)))
		{
			delete prototypeModel;
			prototypeModel = nullptr;
		}
	}
}


Model::Model(ModelType type) : Node(kNodeModel)
{
	modelType = type;
	loadedFlag = false;

	transformNodeCount = 0;
	morphNodeCount = 0;
	morphWeightCount = 0;

	modelHashTable = nullptr;
	rootAnimator = nullptr;
}

Model::Model(const Model& model) : Node(model)
{
	modelType = model.modelType;
	loadedFlag = false;

	transformNodeCount = 0;
	morphNodeCount = 0;
	morphWeightCount = 0;

	modelHashTable = nullptr;
	rootAnimator = nullptr;
}

Model::~Model()
{
	RagdollController *ragdoll = ragdollController;
	if (ragdoll)
	{
		ragdoll->Delete();
	}

	delete[] modelHashTable;

	if (ListElement<Model>::GetOwningList())
	{
		ModelRegistration *registration = FindRegistration(modelType);
		if (registration)
		{
			registration->Retire(this);
		}
	}
}

Node *Model::Replicate(void) const
{
	return (new Model(*this));
}

Model *Model::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	ModelType type = data.GetType();
	if (type != kModelGeneric)
	{
		return (new Model(type));
	}

	return (new GenericModel);
}

void Model::PackType(Packer& data) const
{
	Node::PackType(data);
	data << modelType;
}

void Model::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	RagdollController *ragdoll = ragdollController;
	if (ragdoll)
	{
		data << ChunkHeader('RGDL', 4);
		data << ragdoll->GetTargetNode()->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void Model::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Model>(data, unpackFlags);
}

bool Model::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RGDL':
		{
			int32	index;

			data >> index;
			data.AddNodeLink(index, &RagdollLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void Model::RagdollLinkProc(Node *node, void *cookie)
{
	Model *model = static_cast<Model *>(cookie);
	model->ragdollController = static_cast<RagdollController *>(node->GetController());
}

void Model::Load(World *world)
{
	if (!loadedFlag)
	{
		ModelRegistration *registration = FindRegistration(modelType);
		if (registration)
		{
			registration->Clone(this);

			if (!Enabled())
			{
				Disable();
			}
		}
	}
}

void Model::Unload(void)
{
	loadedFlag = false;
	ListElement<Model>::Detach();
	PurgeSubtree();
}

void Model::Invalidate(void)
{
	Node::Invalidate();

	for (Controller *controller : modelControllerArray)
	{
		controller->Invalidate();
	}
}

void Model::Preprocess(void)
{
	unsigned_int16		bucketSize[kModelHashBucketCount];

	if (!GetManipulator())
	{
		Node *super = GetSuperNode();
		if (super)
		{
			World *world = super->GetWorld();
			if (world)
			{
				Load(world);

				RagdollController *ragdoll = ragdollController;
				if (ragdoll)
				{
					ragdoll->ReassociateNodes();
				}
			}
		}
	}

	modelControllerArray.Clear();

	delete[] modelHashTable;
	modelHashTable = nullptr;

	for (machine a = 0; a < kModelHashBucketCount; a++)
	{
		bucketSize[a] = 0;
	}

	int32 subnodeCount = 0;
	Node *node = GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			node = GetNextLevelNode(node);
		}
		else
		{
			unsigned_int32 hash = node->GetNodeHash();
			if (hash != 0)
			{
				subnodeCount++;
				bucketSize[hash & (kModelHashBucketCount - 1)]++;
			}

			Controller *controller = node->GetController();
			if ((controller) && (controller->GetBaseControllerType() == kControllerMorph))
			{
				modelControllerArray.AddElement(controller);
			}

			node = GetNextNode(node);
		}
	}

	int32 transformCount = 0;
	int32 morphCount = 0;
	int32 weightCount = 0;

	node = GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			node = GetNextLevelNode(node);
		}
		else
		{
			unsigned_int32 flags = node->GetNodeFlags();
			if (!(flags & kNodeAnimateInhibit))
			{
				if (!(flags & kNodeTransformAnimationInhibit))
				{
					transformCount++;
				}

				if (!(flags & kNodeMorphWeightAnimationInhibit))
				{
					const Controller *controller = node->GetController();
					if (ActiveMorphController(controller))
					{
						morphCount++;
						weightCount += static_cast<const MorphController *>(controller)->GetMorphWeightCount() + 1;
					}
				}
			}
			else if (!(flags & kNodeAnimateSubtree))
			{
				node = GetNextLevelNode(node);
				continue;
			}

			node = GetNextNode(node);
		}
	}

	transformNodeCount = transformCount;
	morphNodeCount = morphCount;
	morphWeightCount = weightCount;

	int32 totalCount = subnodeCount + transformCount + morphCount;
	if (totalCount != 0)
	{
		modelHashTable = new Node *[totalCount];
		transformNodeTable = modelHashTable + subnodeCount;
		morphNodeTable = reinterpret_cast<Geometry **>(transformNodeTable + transformCount);

		unsigned_int32 start = 0;
		for (machine a = 0; a < kModelHashBucketCount; a++)
		{
			unsigned_int32 count = bucketSize[a];
			bucketSize[a] = 0;

			hashBucket[a].count = (unsigned_int16) count;
			hashBucket[a].start = (unsigned_int16) start;
			start += count;
		}

		node = GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeModel)
			{
				node = GetNextLevelNode(node);
			}
			else
			{
				unsigned_int32 hash = node->GetNodeHash();
				if (hash != 0)
				{
					hash &= kModelHashBucketCount - 1;
					modelHashTable[hashBucket[hash].start + bucketSize[hash]++] = node;
				}

				node = GetNextNode(node);
			}
		}

		transformCount = 0;
		morphCount = 0;

		node = GetFirstSubnode();
		while (node)
		{
			if (node->GetNodeType() == kNodeModel)
			{
				node = GetNextLevelNode(node);
			}
			else
			{
				unsigned_int32 flags = node->GetNodeFlags();
				if (!(flags & kNodeAnimateInhibit))
				{
					if (!(flags & kNodeTransformAnimationInhibit))
					{
						transformNodeTable[transformCount++] = node;
					}

					if (!(flags & kNodeMorphWeightAnimationInhibit))
					{
						if (ActiveMorphController(node->GetController()))
						{
							morphNodeTable[morphCount++] = static_cast<Geometry *>(node);
						}
					}
				}
				else if (!(flags & kNodeAnimateSubtree))
				{
					node = GetNextLevelNode(node);
					continue;
				}

				node = GetNextNode(node);
			}
		}
	}

	SetNodeFlags(GetNodeFlags() | kNodeVisibilitySite);
	Node::Preprocess();
}

void Model::Neutralize(void)
{
	if (!GetManipulator())
	{
		Unload();
	}

	modelControllerArray.Purge();

	delete[] modelHashTable;
	modelHashTable = nullptr;

	transformNodeCount = 0;
	morphNodeCount = 0;
	morphWeightCount = 0;

	Node::Neutralize();
}

Node *Model::FindNode(unsigned_int32 hash) const
{
	if (modelHashTable)
	{
		const HashBucket *bucket = &hashBucket[hash & (kModelHashBucketCount - 1)];
		Node *const *nodeTable = modelHashTable + bucket->start;
		unsigned_int32 count = bucket->count;

		for (unsigned_machine a = 0; a < count; a++)
		{
			Node *node = nodeTable[a];
			if (node->GetNodeHash() == hash)
			{
				return (node);
			}
		}
	}

	return (nullptr);
}

int32 Model::GetTransformAnimationIndex(const Node *node) const
{
	int32 count = transformNodeCount;
	for (machine a = 0; a < count; a++)
	{
		if (transformNodeTable[a] == node)
		{
			return (a);
		}
	}

	return (-1);
}

void Model::SetRootAnimator(Animator *animator)
{
	rootAnimator = animator;
	if (animator)
	{
		animator->Preprocess();
		animator->Invalidate();
	}
}

void Model::Animate(void)
{
	if (rootAnimator)
	{
		rootAnimator->Premove();

		// It's possible that the root animator is deleted during Premove().

		if (rootAnimator)
		{
			rootAnimator->Update();
			rootAnimator->Move();

			const AnimatorTransform *const *outputTransformTable = rootAnimator->GetOutputTransformTable();
			if (outputTransformTable)
			{
				int32 start = rootAnimator->GetOutputTransformNodeStart();
				int32 count = rootAnimator->GetOutputTransformNodeCount();

				Node *const *targetTable = transformNodeTable + start;
				for (machine a = 0; a < count; a++)
				{
					const AnimatorTransform *transform = outputTransformTable[a];
					if (transform)
					{
						targetTable[a]->SetNodeTransform(transform->rotation.GetRotationScaleMatrix(), transform->position);
					}
				}
			}

			const float *const *outputMorphWeightTable = rootAnimator->GetOutputMorphWeightTable();
			if (outputMorphWeightTable)
			{
				int32 start = rootAnimator->GetOutputMorphNodeStart();
				int32 count = rootAnimator->GetOutputMorphNodeCount();

				Geometry *const *targetTable = morphNodeTable + start;
				for (machine a = 0; a < count; a++)
				{
					const float *morphWeight = outputMorphWeightTable[a];
					if (morphWeight)
					{
						Geometry *geometry = targetTable[a];
						MorphController *controller = static_cast<MorphController *>(geometry->GetController());

						controller->InvalidateMorph();
						controller->SetBaseMeshWeight(morphWeight[0]);
						morphWeight++;

						int32 weightCount = controller->GetMorphWeightCount();
						for (machine b = 0; b < weightCount; b++)
						{
							controller->SetMorphWeight(b, morphWeight[b]);
						}
					}
				}
			}

			Model::Invalidate();
		}
	}
}

Model *Model::New(const char *name, ModelType type, unsigned_int32 unpackFlags)
{
	ModelResource *resource = ModelResource::Get(name);
	if (resource)
	{
		#if C4LOG_RESOURCES

			TheResourceMgr->IncrementResourceLogLevel();

		#endif

		Model *model = static_cast<Model *>(Node::UnpackTree(resource->GetData(), unpackFlags | (kUnpackNonpersistent | kUnpackExternal)));
		resource->Release();

		#if C4LOG_RESOURCES

			TheResourceMgr->DecrementResourceLogLevel();

		#endif

		if (type != kModelUnknown)
		{
			model->modelType = type;
		}

		model->loadedFlag = true;
		model->SetNodeFlags(model->GetNodeFlags() & ~kNodeNonpersistent);
		return (model);
	}

	return (nullptr);
}

Model *Model::Get(ModelType type)
{
	ModelRegistration *registration = FindRegistration(type);
	if (registration)
	{
		Model *model = registration->Clone();
		model->loadedFlag = true;
		return (model);
	}

	return (nullptr);
}

RagdollController *Model::AttachRagdoll(ModelType type)
{
	ModelRegistration *registration = FindRegistration(type);
	if (registration)
	{
		Array<Node *, 16>	nodeArray;

		Node *group = registration->CloneGroup();
		RagdollController *ragdoll = nullptr;

		Node *subnode = group->GetFirstSubnode();
		while (subnode)
		{
			Controller *controller = subnode->GetController();
			if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
			{
				nodeArray.AddElement(subnode);

				if (controller->GetControllerType() == kControllerRagdoll)
				{
					ragdoll = static_cast<RagdollController *>(controller);
				}
			}

			subnode = subnode->Next();
		}

		if (ragdoll)
		{
			ragdollController = ragdoll;
			ragdoll->AttachModel(nodeArray, this);

			subnode = group->GetFirstSubnode();
			while (subnode)
			{
				subnode->SetNodeFlags(subnode->GetNodeFlags() & ~kNodeNonpersistent);
				subnode = group->GetNextNode(subnode);
			}

			Node *root = GetWorld()->GetRootNode();
			for (Node *node : nodeArray)
			{
				root->AppendSubnode(node);
				node->Update();
			}

			for (Node *node : nodeArray)
			{
				node->Preprocess();
			}

			Controller *modelController = GetController();
			if ((modelController) && (modelController->GetBaseControllerType() == kControllerRigidBody))
			{
				RigidBodyController *rigidBody = static_cast<RigidBodyController *>(modelController);
				rigidBody->SetCollisionExclusionMask(kCollisionExcludeAll);
				rigidBody->Sleep();
			}
		}

		delete group;
		return (ragdoll);
	}

	return (nullptr);
}


GenericModel::GenericModel() : Model(kModelGeneric)
{
}

GenericModel::GenericModel(const char *name) : Model(kModelGeneric)
{
	modelName = name;
}

GenericModel::GenericModel(const GenericModel& genericModel) : Model(genericModel)
{
	modelName = genericModel.modelName;

	List<GenericModel> *list = genericModel.ListElement<GenericModel>::GetOwningList();
	if (list)
	{
		list->Append(this);
	}
}

GenericModel::~GenericModel()
{
}

Node *GenericModel::Replicate(void) const
{
	return (new GenericModel(*this));
}

void GenericModel::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Model::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('MODL');
	data << modelName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void GenericModel::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Model::Unpack(data, unpackFlags);
	UnpackChunkList<GenericModel>(data, unpackFlags);
}

bool GenericModel::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MODL':

			data >> modelName;
			return (true);
	}

	return (false);
}

int32 GenericModel::GetCategoryCount(void) const
{
	return (Model::GetCategoryCount() + 1);
}

Type GenericModel::GetCategoryType(int32 index, const char **title) const
{
	int32 count = Model::GetCategoryCount();
	if (index == count)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kNodeModel));
		return (kNodeModel);
	}

	return (Model::GetCategoryType(index, title));
}

int32 GenericModel::GetCategorySettingCount(Type category) const
{
	if (category == kNodeModel)
	{
		return (2);
	}

	return (Model::GetCategorySettingCount(category));
}

Setting *GenericModel::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kNodeModel)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kNodeModel, 'GMOD'));
			return (new HeadingSetting('GMOD', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kNodeModel, 'GMOD', 'MODL'));
			const char *picker = table->GetString(StringID(kNodeModel, 'GMOD', 'PICK'));
			return (new ResourceSetting('MODL', modelName, title, picker, ModelResource::GetDescriptor()));
		}

		return (nullptr);
	}

	return (Model::GetCategorySetting(category, index, flags));
}

void GenericModel::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kNodeModel)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'MODL')
		{
			modelName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
	}
	else
	{
		Model::SetCategorySetting(category, setting);
	}
}

void GenericModel::Load(World *world)
{
	if ((modelName[0] != 0) && (!ListElement<GenericModel>::GetOwningList()))
	{
		Node *instanceRoot = world->NewGenericModel(modelName, this);
		if (instanceRoot)
		{
			for (;;)
			{
				Node *node = instanceRoot->GetFirstSubnode();
				if (!node)
				{
					break;
				}

				AppendSubnode(node);
			}

			delete instanceRoot;
		}
	}
}

void GenericModel::Unload(void)
{
	ListElement<GenericModel>::Detach();
	PurgeSubtree();
}


AnimationController::AnimationController() : Controller(kControllerAnimation)
{
	animationMode = kInterpolatorStop;
	animationName[0] = 0;
	frameAnimator = nullptr;
}

AnimationController::AnimationController(const AnimationController& animationController) : Controller(animationController)
{
	animationMode = animationController.animationMode;
	animationName = animationController.animationName;
	frameAnimator = nullptr;
}

AnimationController::~AnimationController()
{
	delete frameAnimator;
}

Controller *AnimationController::Replicate(void) const
{
	return (new AnimationController(*this));
}

bool AnimationController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeModel);
}

void AnimationController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<PlayAnimationFunction> playAnimationFunction(registration, kFunctionPlayAnimation, table->GetString(StringID('CTRL', kControllerAnimation, 'PLAY')), kFunctionRemote | kFunctionJournaled);
	static FunctionReg<StopAnimationFunction> stopAnimationFunction(registration, kFunctionStopAnimation, table->GetString(StringID('CTRL', kControllerAnimation, 'STOP')), kFunctionRemote | kFunctionJournaled);
}

void AnimationController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	if (animationName[0] != 0)
	{
		PackHandle handle = data.BeginChunk('NAME');
		data << animationName;
		data.EndChunk(handle);
	}

	if (animationMode != kInterpolatorStop)
	{
		data << ChunkHeader('MODE', 4);
		data << animationMode;
	}

	data << TerminatorChunk;
}

void AnimationController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<AnimationController>(data, unpackFlags);
}

bool AnimationController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> animationName;
			return (true);

		case 'MODE':

			data >> animationMode;
			return (true);
	}

	return (false);
}

void *AnimationController::BeginSettingsUnpack(void)
{
	animationMode = kInterpolatorStop;
	animationName[0] = 0;

	return (Controller::BeginSettingsUnpack());
}

int32 AnimationController::GetSettingCount(void) const
{
	return (5);
}

Setting *AnimationController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, 'NAME'));
		const char *picker = table->GetString(StringID('CTRL', kControllerAnimation, 'PICK'));
		return (new ResourceSetting('NAME', animationName, title, picker, AnimationResource::GetDescriptor()));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, 'IPLY'));
		return (new BooleanSetting('IPLY', (animationMode != kInterpolatorStop), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, 'RVRS'));
		return (new BooleanSetting('RVRS', ((animationMode & kInterpolatorBackward) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, 'LOOP'));
		return (new BooleanSetting('LOOP', ((animationMode & kInterpolatorLoop) != 0), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, 'OSCL'));
		return (new BooleanSetting('OSCL', ((animationMode & kInterpolatorOscillate) != 0), title));
	}

	return (nullptr);
}

void AnimationController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NAME')
	{
		animationName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else if (identifier == 'IPLY')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animationMode = kInterpolatorForward;
		}
		else
		{
			animationMode = kInterpolatorStop;
		}
	}
	else if (identifier == 'RVRS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animationMode = kInterpolatorBackward;
		}
	}
	else if (identifier == 'LOOP')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animationMode |= kInterpolatorLoop;
		}
	}
	else if (identifier == 'OSCL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animationMode |= kInterpolatorOscillate;
		}
	}
}

void AnimationController::Preprocess(void)
{
	Controller::Preprocess();

	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		if (!frameAnimator)
		{
			frameAnimator = new FrameAnimator(model);
		}

		frameAnimator->SetAnimation(animationName);
		frameAnimator->GetFrameInterpolator()->SetMode(animationMode);
		model->SetRootAnimator(frameAnimator);
	}
}

ControllerMessage *AnimationController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kAnimationMessageState:

			return (new AnimationStateMessage(GetControllerIndex()));
	}

	return (Controller::CreateMessage(type));
}

void AnimationController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kAnimationMessageState)
	{
		const AnimationStateMessage *m = static_cast<const AnimationStateMessage *>(message);

		animationName = m->GetAnimationName();
		animationMode = m->GetAnimationMode();

		frameAnimator->SetAnimation(animationName);
		Interpolator *interpolator = frameAnimator->GetFrameInterpolator();
		interpolator->SetMode(animationMode);
		interpolator->SetValue(m->GetAnimatorValue());
	}
	else
	{
		Controller::ReceiveMessage(message);
	}
}

void AnimationController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(AnimationStateMessage(GetControllerIndex(), animationName, frameAnimator->GetFrameInterpolator()->GetValue(), animationMode));
}

void AnimationController::Move(void)
{
	static_cast<Model *>(Controller::GetTargetNode())->Animate();
}

void AnimationController::PlayAnimation(const char *name, unsigned_int32 mode)
{
	animationName = name;
	animationMode = mode;

	frameAnimator->SetAnimation(name);
	frameAnimator->GetFrameInterpolator()->SetMode(mode);
}

void AnimationController::StopAnimation(void)
{
	unsigned_int32 mode = animationMode & ~(kInterpolatorForward | kInterpolatorBackward);
	animationMode = mode;

	frameAnimator->GetFrameInterpolator()->SetMode(mode);
}


AnimationStateMessage::AnimationStateMessage(int32 controllerIndex) : ControllerMessage(AnimationController::kAnimationMessageState, controllerIndex)
{
}

AnimationStateMessage::AnimationStateMessage(int32 controllerIndex, const char *name, float value, unsigned_int32 mode) : ControllerMessage(AnimationController::kAnimationMessageState, controllerIndex)
{
	animationName = name;
	animatorValue = value;
	animationMode = mode;
}

AnimationStateMessage::~AnimationStateMessage()
{
}

void AnimationStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << animationMode;
	data << animatorValue;
	data << animationName;
}

bool AnimationStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> animationMode;
		data >> animatorValue;
		data >> animationName;
		return (true);
	}

	return (false);
}


PlayAnimationFunction::PlayAnimationFunction() : Function(kFunctionPlayAnimation, kControllerAnimation)
{
	animationMode = kInterpolatorStop;
	animationName[0] = 0;
}

PlayAnimationFunction::PlayAnimationFunction(const PlayAnimationFunction& playAnimationFunction) : Function(playAnimationFunction)
{
	animationMode = playAnimationFunction.animationMode;
	animationName = playAnimationFunction.animationName;
}

PlayAnimationFunction::~PlayAnimationFunction()
{
}

Function *PlayAnimationFunction::Replicate(void) const
{
	return (new PlayAnimationFunction(*this));
}

void PlayAnimationFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	data << ChunkHeader('MODE', 4);
	data << animationMode;

	PackHandle handle = data.BeginChunk('NAME');
	data << animationName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void PlayAnimationFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);
	UnpackChunkList<PlayAnimationFunction>(data, unpackFlags);
}

bool PlayAnimationFunction::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MODE':

			data >> animationMode;
			return (true);

		case 'NAME':

			data >> animationName;
			return (true);
	}

	return (false);
}

void PlayAnimationFunction::Compress(Compressor& data) const
{
	Function::Compress(data);

	data << animationMode;
	data << animationName;
}

bool PlayAnimationFunction::Decompress(Decompressor& data)
{
	if (Function::Decompress(data))
	{
		data >> animationMode;
		data >> animationName;
		return (true);
	}

	return (false);
}

int32 PlayAnimationFunction::GetSettingCount(void) const
{
	return (4);
}

Setting *PlayAnimationFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, kFunctionPlayAnimation, 'ANAM'));
		const char *picker = table->GetString(StringID('CTRL', kControllerAnimation, 'PICK'));
		return (new ResourceSetting('ANAM', animationName, title, picker, AnimationResource::GetDescriptor()));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, kFunctionPlayAnimation, 'RVRS'));
		return (new BooleanSetting('RVRS', ((animationMode & kInterpolatorBackward) != 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, kFunctionPlayAnimation, 'LOOP'));
		return (new BooleanSetting('LOOP', ((animationMode & kInterpolatorLoop) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerAnimation, kFunctionPlayAnimation, 'OSCL'));
		return (new BooleanSetting('OSCL', ((animationMode & kInterpolatorOscillate) != 0), title));
	}

	return (nullptr);
}

void PlayAnimationFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'ANAM')
	{
		animationName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else if (identifier == 'RVRS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animationMode = kInterpolatorBackward;
		}
		else
		{
			animationMode = kInterpolatorForward;
		}
	}
	else if (identifier == 'LOOP')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animationMode |= kInterpolatorLoop;
		}
		else
		{
			animationMode &= ~kInterpolatorLoop;
		}
	}
	else if (identifier == 'OSCL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animationMode |= kInterpolatorOscillate;
		}
		else
		{
			animationMode &= ~kInterpolatorOscillate;
		}
	}
}

bool PlayAnimationFunction::OverridesFunction(const Function *function) const
{
	FunctionType type = function->GetFunctionType();
	return ((type == kFunctionPlayAnimation) || (type == kFunctionStopAnimation));
}

void PlayAnimationFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	AnimationController *animationController = static_cast<AnimationController *>(controller);
	animationController->PlayAnimation(animationName, animationMode);

	CallCompletionProc();
}


StopAnimationFunction::StopAnimationFunction() : Function(kFunctionStopAnimation, kControllerAnimation)
{
}

StopAnimationFunction::StopAnimationFunction(const StopAnimationFunction& stopAnimationFunction) : Function(stopAnimationFunction)
{
}

StopAnimationFunction::~StopAnimationFunction()
{
}

Function *StopAnimationFunction::Replicate(void) const
{
	return (new StopAnimationFunction(*this));
}

bool StopAnimationFunction::OverridesFunction(const Function *function) const
{
	return (function->GetFunctionType() == kFunctionStopAnimation);
}

void StopAnimationFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	AnimationController *animationController = static_cast<AnimationController *>(controller);
	animationController->StopAnimation();

	CallCompletionProc();
}

// ZYUQURM
