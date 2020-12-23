#include "C4Animation.h"
#include "C4Models.h"


using namespace C4;


namespace C4
{
	template class Creatable<Animator>;
}


ResourceDescriptor AnimationResource::descriptor("anm", 0, 8388608);
const unsigned_int32 AnimationResource::resourceSignature[2] = {'C4AN', kEngineInternalVersion};


AnimationResource::AnimationResource(const char *name, ResourceCatalog *catalog) : Resource<AnimationResource>(name, catalog)
{
}

AnimationResource::~AnimationResource()
{
}


Animator::Animator(AnimatorType type) : weightInterpolator(1.0F)
{
	animatorType = type;
	updateFlag = true;

	targetModel = nullptr;
	targetNode = nullptr;

	animatorTransformNodeStart = 0;
	animatorTransformNodeCount = 0;
	animatorMorphNodeStart = 0;
	animatorMorphNodeCount = 0;
	animatorMorphWeightStart = 0;
	animatorMorphWeightCount = 0;

	storageSize = 0;
	outputStorage = nullptr;
	outputTransformTable = nullptr;
	animatorTransformTable = nullptr;
	animatorData = nullptr;
}

Animator::Animator(AnimatorType type, Model *model, Node *node) : weightInterpolator(1.0F)
{
	animatorType = type;
	updateFlag = true;

	targetModel = model;
	targetNode = (node) ? node : model;

	animatorTransformNodeStart = 0;
	animatorTransformNodeCount = 0;
	animatorMorphNodeStart = 0;
	animatorMorphNodeCount = 0;
	animatorMorphWeightStart = 0;
	animatorMorphWeightCount = 0;

	storageSize = 0;
	outputStorage = nullptr;
	outputTransformTable = nullptr;
	animatorTransformTable = nullptr;
	animatorData = nullptr;
}

Animator::~Animator()
{
	Animator *animator = GetSuperNode();
	while (animator)
	{
		animator->updateFlag = true;
		animator = animator->GetSuperNode();
	}

	delete[] outputStorage;
}

Animator *Animator::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kAnimatorFrame:

			return (new FrameAnimator);

		case kAnimatorMerge:

			return (new MergeAnimator);

		case kAnimatorBlend:

			return (new BlendAnimator);
	}

	return (Creatable<Animator>::Create(data, unpackFlags));
}

Animator *Animator::New(AnimatorType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

void Animator::PackType(Packer& data) const
{
	data << animatorType;
}

void Animator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PackHandle handle = data.BeginChunk('WGHT');
	weightInterpolator.Pack(data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void Animator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Animator>(data, unpackFlags);
}

bool Animator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'WGHT':

			weightInterpolator.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

void Animator::Detach(void)
{
	Animator *animator = GetSuperNode();
	if (animator)
	{
		do
		{
			animator->updateFlag = true;
			animator = animator->GetSuperNode();
		} while (animator);

		Tree<Animator>::Detach();
	}
}

void Animator::AppendSubnode(Animator *node)
{
	node->Animator::Detach();
	Tree<Animator>::AppendSubnode(node);
	node->Invalidate();
}

void Animator::PrependSubnode(Animator *node)
{
	node->Animator::Detach();
	Tree<Animator>::PrependSubnode(node);
	node->Invalidate();
}

void Animator::InsertSubnodeBefore(Animator *node, Animator *before)
{
	node->Animator::Detach();
	Tree<Animator>::InsertSubnodeBefore(node, before);
	node->Invalidate();
}

void Animator::InsertSubnodeAfter(Animator *node, Animator *after)
{
	node->Animator::Detach();
	Tree<Animator>::InsertSubnodeAfter(node, after);
	node->Invalidate();
}

void Animator::RemoveSubnode(Animator *node)
{
	Animator *animator = this;
	do
	{
		animator->updateFlag = true;
		animator = animator->GetSuperNode();
	} while (animator);

	Tree<Animator>::RemoveSubnode(node);
}

void Animator::Invalidate(void)
{
	updateFlag = true;

	Animator *animator = GetSuperNode();
	while (animator)
	{
		if (animator->updateFlag)
		{
			break;
		}

		animator->updateFlag = true;
		animator = animator->GetSuperNode();
	}

	animator = GetFirstSubnode();
	while (animator)
	{
		animator->Invalidate();
		animator = animator->Next();
	}
}

void Animator::Update(void)
{
	bool flag = updateFlag;
	updateFlag = false;

	Animator *animator = GetFirstSubnode();
	while (animator)
	{
		animator->Update();
		animator = animator->Next();
	}

	if (flag)
	{
		Configure();
	}
}

void Animator::AllocateStorage(int32 nodeStart, int32 nodeCount, int32 transformDataCount, unsigned_int32 animatorDataSize)
{
	outputTransformNodeStart = nodeStart;
	outputTransformNodeCount = nodeCount;

	outputMorphNodeStart = 0;
	outputMorphNodeCount = 0;
	outputMorphWeightStart = 0;
	outputMorphWeightCount = 0;

	unsigned_int32 size = nodeCount * sizeof(AnimatorTransform *) + (transformDataCount + 1) * sizeof(AnimatorTransform) + animatorDataSize;
	if (storageSize != size)
	{
		storageSize = size;
		delete[] outputStorage;
		outputStorage = new char[size];
	}

	outputTransformTable = reinterpret_cast<AnimatorTransform **>(outputStorage);
	outputMorphWeightTable = nullptr;

	AnimatorTransform *transform = reinterpret_cast<AnimatorTransform *>(outputTransformTable + nodeCount);
	animatorTransformTable = transform + 1;
	animatorMorphWeightTable = nullptr;

	animatorData = animatorTransformTable + transformDataCount;

	// Always store an identity transform as the first entry (index -1) in the transform table.

	transform->rotation = 1.0F;
	transform->position.Set(0.0F, 0.0F, 0.0F);
}

void Animator::AllocateStorage(int32 transformNodeStart, int32 transformNodeCount, int32 transformDataCount, int32 morphNodeStart, int32 morphNodeCount, int32 morphWeightStart, int32 morphWeightCount, int32 morphDataCount, unsigned_int32 animatorDataSize)
{
	outputTransformNodeStart = transformNodeStart;
	outputTransformNodeCount = transformNodeCount;

	outputMorphNodeStart = morphNodeStart;
	outputMorphNodeCount = morphNodeCount;
	outputMorphWeightStart = morphWeightStart;
	outputMorphWeightCount = morphWeightCount;

	unsigned_int32 size = transformNodeCount * sizeof(AnimatorTransform *) + morphNodeCount * sizeof(float *) + (transformDataCount + 1) * sizeof(AnimatorTransform) + (morphDataCount + 2) * 4 + animatorDataSize;
	if (storageSize != size)
	{
		storageSize = size;
		delete[] outputStorage;
		outputStorage = new char[size];
	}

	outputTransformTable = reinterpret_cast<AnimatorTransform **>(outputStorage);
	outputMorphWeightTable = reinterpret_cast<float **>(outputTransformTable + transformNodeCount);

	AnimatorTransform *transform = reinterpret_cast<AnimatorTransform *>(outputMorphWeightTable + morphNodeCount);
	animatorTransformTable = transform + 1;

	float *morphWeight = reinterpret_cast<float *>(animatorTransformTable + transformDataCount);
	animatorMorphWeightTable = morphWeight + 2;

	animatorData = animatorMorphWeightTable + morphDataCount;

	// Always store an identity transform as the first entry (index -1) in the transform table.

	transform->rotation = 1.0F;
	transform->position.Set(0.0F, 0.0F, 0.0F);

	// Always store one and zero morph weights as the first two entries (indexes -2 and -1) in the morph weight table.

	morphWeight[0] = 1.0F;
	morphWeight[1] = 0.0F;
}

void Animator::SetTargetModel(Model *model, Node *node)
{
	targetModel = model;
	targetNode = (node) ? node : model;
}

int32 Animator::GetNodeTransformIndex(const Node *target) const
{
	const Model *model = GetTargetModel();
	const Node *node = model->GetFirstSubnode();

	int32 transformIndex = 0;
	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			node = model->GetNextLevelNode(node);
		}
		else
		{
			if (node == target)
			{
				break;
			}

			unsigned_int32 flags = node->GetNodeFlags();
			if (!(flags & kNodeAnimateInhibit))
			{
				if (!(flags & kNodeTransformAnimationInhibit))
				{
					transformIndex++;
				}
			}
			else if (!(flags & kNodeAnimateSubtree))
			{
				node = model->GetNextLevelNode(node);
				continue;
			}

			node = model->GetNextNode(node);
		}
	}

	transformIndex -= animatorTransformNodeStart;
	if ((unsigned_int32) transformIndex >= (unsigned_int32) animatorTransformNodeCount)
	{
		return (-1);
	}

	return (transformIndex);
}

void Animator::Premove(void)
{
	Animator *animator = GetFirstSubnode();
	while (animator)
	{
		Animator *next = animator->Next();
		animator->Premove();
		animator = next;
	}

	weightInterpolator.UpdateValue();
}

void Animator::Preprocess(void)
{
	const Model *model = GetTargetModel();
	const Node *target = GetTargetNode();
	const Node *node = model->GetFirstSubnode();

	int32 transformNodeCount = 0;
	int32 morphNodeCount = 0;
	int32 morphWeightCount = 0;

	if (target != model)
	{
		while (node)
		{
			if (node->GetNodeType() == kNodeModel)
			{
				node = model->GetNextLevelNode(node);
			}
			else
			{
				if (node == target)
				{
					break;
				}

				unsigned_int32 flags = node->GetNodeFlags();
				if (!(flags & kNodeAnimateInhibit))
				{
					if (!(flags & kNodeTransformAnimationInhibit))
					{
						transformNodeCount++;
					}

					if (!(flags & kNodeMorphWeightAnimationInhibit))
					{
						const Controller *controller = node->GetController();
						if (Model::ActiveMorphController(controller))
						{
							morphNodeCount++;
							morphWeightCount += static_cast<const MorphController *>(controller)->GetMorphWeightCount() + 1;
						}
					}
				}
				else if (!(flags & kNodeAnimateSubtree))
				{
					node = model->GetNextLevelNode(node);
					continue;
				}

				node = model->GetNextNode(node);
			}
		}
	}

	int32 transformNodeStart = transformNodeCount;
	int32 morphNodeStart = morphNodeCount;
	int32 morphWeightStart = morphWeightCount;

	while (node)
	{
		if (node->GetNodeType() == kNodeModel)
		{
			node = target->GetNextLevelNode(node);
		}
		else
		{
			unsigned_int32 flags = node->GetNodeFlags();
			if (!(flags & kNodeAnimateInhibit))
			{
				if (!(flags & kNodeTransformAnimationInhibit))
				{
					transformNodeCount++;
				}

				if (!(flags & kNodeMorphWeightAnimationInhibit))
				{
					const Controller *controller = node->GetController();
					if (Model::ActiveMorphController(controller))
					{
						morphNodeCount++;
						morphWeightCount += static_cast<const MorphController *>(controller)->GetMorphWeightCount() + 1;
					}
				}
			}
			else if (!(flags & kNodeAnimateSubtree))
			{
				node = target->GetNextLevelNode(node);
				continue;
			}

			node = target->GetNextNode(node);
		}
	}

	animatorTransformNodeStart = transformNodeStart;
	animatorTransformNodeCount = transformNodeCount - transformNodeStart;

	animatorMorphNodeStart = morphNodeStart;
	animatorMorphNodeCount = morphNodeCount - morphNodeStart;
	animatorMorphWeightStart = morphWeightStart;
	animatorMorphWeightCount = morphWeightCount - morphWeightStart;

	Animator *animator = GetFirstSubnode();
	while (animator)
	{
		animator->Preprocess();
		animator = animator->Next();
	}
}

void Animator::Move(void)
{
	Animator *animator = GetFirstSubnode();
	while (animator)
	{
		Animator *next = animator->Next();
		animator->Move();
		animator = next;
	}
}


MergeAnimator::MergeAnimator() : Animator(kAnimatorMerge)
{
}

MergeAnimator::MergeAnimator(Model *model, Node *node) : Animator(kAnimatorMerge, model, node)
{
}

MergeAnimator::~MergeAnimator()
{
}

void MergeAnimator::Configure(void)
{
	const Model *model = GetTargetModel();

	int32 transformStart = model->GetAnimatedTransformNodeCount();
	int32 transformFinish = 0;

	int32 morphNodeStart = model->GetAnimatedMorphNodeCount();
	int32 morphWeightStart = model->GetAnimatedMorphWeightCount();
	int32 morphNodeFinish = 0;
	int32 morphWeightFinish = 0;

	const Animator *animator = GetFirstSubnode();
	while (animator)
	{
		int32 subanimatorTransformStart = animator->GetOutputTransformNodeStart();
		transformStart = Min(transformStart, subanimatorTransformStart);
		transformFinish = Max(transformFinish, subanimatorTransformStart + animator->GetOutputTransformNodeCount());

		int32 subanimatorMorphNodeStart = animator->GetOutputMorphNodeStart();
		int32 subanimatorMorphWeightStart = animator->GetOutputMorphWeightStart();
		morphNodeStart = Min(morphNodeStart, subanimatorMorphNodeStart);
		morphWeightStart = Min(morphWeightStart, subanimatorMorphWeightStart);
		morphNodeFinish = Max(morphNodeFinish, subanimatorMorphNodeStart + animator->GetOutputMorphNodeCount());
		morphWeightFinish = Max(morphWeightFinish, subanimatorMorphWeightStart + animator->GetOutputMorphWeightCount());

		animator = animator->Next();
	}

	int32 animatorTransformStart = GetAnimatorTransformNodeStart();
	int32 animatorTransformFinish = animatorTransformStart + GetAnimatorTransformNodeCount();

	transformStart = Max(transformStart, animatorTransformStart);
	transformFinish = Min(transformFinish, animatorTransformFinish);
	int32 transformCount = transformFinish - transformStart;

	int32 animatorMorphNodeStart = GetAnimatorMorphNodeStart();
	int32 animatorMorphWeightStart = GetAnimatorMorphWeightStart();
	int32 animatorMorphNodeFinish = animatorMorphNodeStart + GetAnimatorMorphNodeCount();
	int32 animatorMorphWeightFinish = animatorMorphWeightStart + GetAnimatorMorphWeightCount();

	morphNodeStart = Max(morphNodeStart, animatorMorphNodeStart);
	morphWeightStart = Max(morphWeightStart, animatorMorphWeightStart);
	morphNodeFinish = Min(morphNodeFinish, animatorMorphNodeFinish);
	morphWeightFinish = Min(morphWeightFinish, animatorMorphWeightFinish);
	int32 morphNodeCount = morphNodeFinish - morphNodeStart;
	int32 morphWeightCount = morphWeightFinish - morphWeightStart;

	if (transformCount + morphNodeCount != 0)
	{
		AllocateStorage(transformStart, transformCount, 0, morphNodeStart, morphNodeCount, morphWeightStart, morphWeightCount, 0);

		if (transformCount != 0)
		{
			AnimatorTransform **outputTransformTable = GetOutputTransformTable();
			for (machine a = 0; a < transformCount; a++)
			{
				outputTransformTable[a] = nullptr;
			}

			animator = GetFirstSubnode();
			while (animator)
			{
				AnimatorTransform *const *table = animator->GetOutputTransformTable();

				int32 start = animator->GetOutputTransformNodeStart() - transformStart;
				int32 count = animator->GetOutputTransformNodeCount();
				for (machine a = 0; a < count; a++)
				{
					outputTransformTable[a + start] = table[a];
				}

				animator = animator->Next();
			}
		}

		if (morphNodeCount != 0)
		{
			float **outputMorphWeightTable = GetOutputMorphWeightTable();
			for (machine a = 0; a < morphNodeCount; a++)
			{
				outputMorphWeightTable[a] = nullptr;
			}

			animator = GetFirstSubnode();
			while (animator)
			{
				float *const *table = animator->GetOutputMorphWeightTable();

				int32 start = animator->GetOutputMorphNodeStart() - morphNodeStart;
				int32 count = animator->GetOutputMorphNodeCount();
				for (machine a = 0; a < count; a++)
				{
					outputMorphWeightTable[a + start] = table[a];
				}

				animator = animator->Next();
			}
		}
	}
}


BlendAnimator::BlendAnimator() : Animator(kAnimatorBlend)
{
}

BlendAnimator::BlendAnimator(Model *model, Node *node) : Animator(kAnimatorBlend, model, node)
{
}

BlendAnimator::~BlendAnimator()
{
}

void BlendAnimator::Configure(void)
{
	const Animator *animator1 = GetFirstSubnode();
	if (animator1)
	{
		int32 animatorTransformStart = GetAnimatorTransformNodeStart();
		int32 animatorTransformFinish = animatorTransformStart + GetAnimatorTransformNodeCount();

		int32 animatorMorphNodeStart = GetAnimatorMorphNodeStart();
		int32 animatorMorphWeightStart = GetAnimatorMorphWeightStart();
		int32 animatorMorphNodeFinish = animatorMorphNodeStart + GetAnimatorMorphNodeCount();
		int32 animatorMorphWeightFinish = animatorMorphWeightStart + GetAnimatorMorphWeightCount();

		const Animator *animator2 = animator1->Next();
		if (animator2)
		{
			int32 transformStart1 = Max(animator1->GetOutputTransformNodeStart(), animatorTransformStart);
			int32 transformFinish1 = Min(animator1->GetOutputTransformNodeCount() + transformStart1, animatorTransformFinish);
			int32 transformStart2 = Max(animator2->GetOutputTransformNodeStart(), animatorTransformStart);
			int32 transformFinish2 = Min(animator2->GetOutputTransformNodeCount() + transformStart2, animatorTransformFinish);

			int32 outerTransformStart = Min(transformStart1, transformStart2);
			int32 outerTransformCount = Max(transformFinish1, transformFinish2) - outerTransformStart;
			int32 innerTransformStart = Max(transformStart1, transformStart2);
			int32 innerTransformCount = Min(transformFinish1, transformFinish2) - innerTransformStart;

			blendTransformStart = innerTransformStart;
			blendTransformCount = MaxZero(innerTransformCount);

			int32 morphNodeStart1 = Max(animator1->GetOutputMorphNodeStart(), animatorMorphNodeStart);
			int32 morphWeightStart1 = Max(animator1->GetOutputMorphWeightStart(), animatorMorphWeightStart);
			int32 morphNodeFinish1 = Min(animator1->GetOutputMorphNodeCount() + morphNodeStart1, animatorMorphNodeFinish);
			int32 morphWeightFinish1 = Min(animator1->GetOutputMorphWeightCount() + morphWeightStart1, animatorMorphWeightFinish);
			int32 morphNodeStart2 = Max(animator2->GetOutputMorphNodeStart(), animatorMorphNodeStart);
			int32 morphWeightStart2 = Max(animator2->GetOutputMorphWeightStart(), animatorMorphWeightStart);
			int32 morphNodeFinish2 = Min(animator2->GetOutputMorphNodeCount() + morphNodeStart2, animatorMorphNodeFinish);
			int32 morphWeightFinish2 = Min(animator2->GetOutputMorphWeightCount() + morphWeightStart2, animatorMorphWeightFinish);

			int32 outerMorphNodeStart = Min(morphNodeStart1, morphNodeStart2);
			int32 outerMorphWeightStart = Min(morphWeightStart1, morphWeightStart2);
			int32 outerMorphNodeCount = Max(morphNodeFinish1, morphNodeFinish2) - outerMorphNodeStart;
			int32 outerMorphWeightCount = Max(morphWeightFinish1, morphWeightFinish2) - outerMorphWeightStart;
			int32 innerMorphNodeStart = Max(morphNodeStart1, morphNodeStart2);
			int32 innerMorphWeightStart = Max(morphWeightStart1, morphWeightStart2);
			int32 innerMorphNodeCount = Min(morphNodeFinish1, morphNodeFinish2) - innerMorphNodeStart;
			int32 innerMorphWeightCount = Min(morphWeightFinish1, morphWeightFinish2) - innerMorphWeightStart;

			blendMorphNodeStart = innerMorphNodeStart;
			blendMorphWeightStart = innerMorphWeightStart;
			blendMorphNodeCount = MaxZero(innerMorphNodeCount);
			blendMorphWeightCount = MaxZero(innerMorphWeightCount);

			AllocateStorage(outerTransformStart, outerTransformCount, blendTransformCount, outerMorphNodeStart, outerMorphNodeCount, outerMorphWeightStart, outerMorphWeightCount, blendMorphWeightCount);

			if (outerTransformCount != 0)
			{
				AnimatorTransform **outputTransformTable = GetOutputTransformTable();
				AnimatorTransform *const *transformTable1 = animator1->GetOutputTransformTable();
				AnimatorTransform *const *transformTable2 = animator2->GetOutputTransformTable();

				int32 transformFinish = Min(transformFinish1, innerTransformStart);
				for (machine a = transformStart1; a < transformFinish; a++)
				{
					outputTransformTable[a - outerTransformStart] = transformTable1[a - transformStart1];
				}

				for (machine a = Max(transformStart1, innerTransformStart + innerTransformCount); a < transformFinish1; a++)
				{
					outputTransformTable[a - outerTransformStart] = transformTable1[a - transformStart1];
				}

				transformFinish = Min(transformFinish2, innerTransformStart);
				for (machine a = transformStart2; a < transformFinish; a++)
				{
					outputTransformTable[a - outerTransformStart] = transformTable2[a - transformStart2];
				}

				for (machine a = Max(transformStart2, innerTransformStart + innerTransformCount); a < transformFinish2; a++)
				{
					outputTransformTable[a - outerTransformStart] = transformTable2[a - transformStart2];
				}

				outputTransformTable += innerTransformStart - outerTransformStart;
				AnimatorTransform *animatorTransformTable = GetAnimatorTransformTable();

				for (machine a = 0; a < innerTransformCount; a++)
				{
					outputTransformTable[a] = &animatorTransformTable[a];
				}
			}

			if (outerMorphWeightCount != 0)
			{
				float **outputMorphWeightTable = GetOutputMorphWeightTable();
				float *const *morphWeightTable1 = animator1->GetOutputMorphWeightTable();
				float *const *morphWeightTable2 = animator2->GetOutputMorphWeightTable();

				int32 morphWeightFinish = Min(morphWeightFinish1, innerMorphWeightStart);
				for (machine a = morphWeightStart1; a < morphWeightFinish; a++)
				{
					outputMorphWeightTable[a - outerMorphWeightStart] = morphWeightTable1[a - morphWeightStart1];
				}

				for (machine a = Max(morphWeightStart1, innerMorphWeightStart + innerMorphWeightCount); a < morphWeightFinish1; a++)
				{
					outputMorphWeightTable[a - outerMorphWeightStart] = morphWeightTable1[a - morphWeightStart1];
				}

				morphWeightFinish = Min(morphWeightFinish2, innerMorphWeightStart);
				for (machine a = morphWeightStart2; a < morphWeightFinish; a++)
				{
					outputMorphWeightTable[a - outerMorphWeightStart] = morphWeightTable2[a - morphWeightStart2];
				}

				for (machine a = Max(morphWeightStart2, innerMorphWeightStart + innerMorphWeightCount); a < morphWeightFinish2; a++)
				{
					outputMorphWeightTable[a - outerMorphWeightStart] = morphWeightTable2[a - morphWeightStart2];
				}

				outputMorphWeightTable += innerMorphWeightStart - outerMorphWeightStart;
				float *animatorMorphWeightTable = GetAnimatorMorphWeightTable();

				const Geometry *const *morphNodeTable = GetTargetModel()->GetAnimatedMorphNodeTable() + innerMorphNodeStart;
				for (machine a = 0; a < innerMorphNodeCount; a++)
				{
					outputMorphWeightTable[a] = animatorMorphWeightTable;
					animatorMorphWeightTable += static_cast<MorphController *>(morphNodeTable[a]->GetController())->GetMorphWeightCount() + 1;
				}
			}
		}
		else
		{
			blendTransformStart = 0;
			blendTransformCount = 0;

			blendMorphNodeStart = 0;
			blendMorphWeightStart = 0;
			blendMorphNodeCount = 0;
			blendMorphWeightCount = 0;

			int32 subanimatorTransformStart = animator1->GetOutputTransformNodeStart();
			int32 transformStart = Min(subanimatorTransformStart, animatorTransformStart);
			int32 transformFinish = Max(subanimatorTransformStart + animator1->GetOutputTransformNodeCount(), animatorTransformFinish);
			int32 transformCount = transformFinish - transformStart;

			int32 subanimatorMorphNodeStart = animator1->GetOutputMorphNodeStart();
			int32 subanimatorMorphWeightStart = animator1->GetOutputMorphWeightStart();
			int32 morphNodeStart = Min(subanimatorMorphNodeStart, animatorMorphNodeStart);
			int32 morphWeightStart = Min(subanimatorMorphWeightStart, animatorMorphWeightStart);
			int32 morphNodeFinish = Max(subanimatorMorphNodeStart + animator1->GetOutputMorphNodeCount(), animatorMorphNodeFinish);
			int32 morphWeightFinish = Max(subanimatorMorphWeightStart + animator1->GetOutputMorphWeightCount(), animatorMorphWeightFinish);
			int32 morphNodeCount = morphNodeFinish - morphNodeStart;

			AllocateStorage(transformStart, transformCount, 0, morphNodeStart, morphNodeFinish, morphWeightStart, morphWeightFinish, 0);

			AnimatorTransform **outputTransformTable = GetOutputTransformTable();
			AnimatorTransform *const *transformTable = animator1->GetOutputTransformTable();
			for (machine a = 0; a < transformCount; a++)
			{
				outputTransformTable[a] = transformTable[a];
			}

			float **outputMorphWeightTable = GetOutputMorphWeightTable();
			float *const *morphWeightTable = animator1->GetOutputMorphWeightTable();
			for (machine a = 0; a < morphNodeCount; a++)
			{
				outputMorphWeightTable[a] = morphWeightTable[a];
			}
		}
	}
}

void BlendAnimator::Move(void)
{
	Animator::Move();

	int32 transformCount = blendTransformCount;
	int32 morphNodeCount = blendMorphNodeCount;

	const Animator *animator1 = GetFirstSubnode();
	const Animator *animator2 = animator1->Next();

	float w1 = animator1->GetWeightInterpolator()->GetValue();
	float w2 = animator2->GetWeightInterpolator()->GetValue();
	float sum = w1 + w2;

	if (sum > K::min_float)
	{
		float f = 1.0F / sum;
		w1 *= f;
		w2 *= f;

		if (transformCount != 0)
		{
			int32 transformStart1 = animator1->GetOutputTransformNodeStart();
			int32 transformStart2 = animator2->GetOutputTransformNodeStart();

			AnimatorTransform *animatorTransformTable = GetAnimatorTransformTable();
			const AnimatorTransform *const *transformTable1 = animator1->GetOutputTransformTable() + (blendTransformStart - transformStart1);
			const AnimatorTransform *const *transformTable2 = animator2->GetOutputTransformTable() + (blendTransformStart - transformStart2);

			for (machine a = 0; a < transformCount; a++)
			{
				const AnimatorTransform *transform1 = transformTable1[a];
				const AnimatorTransform *transform2 = transformTable2[a];

				animatorTransformTable[a].position = transform1->position * w1 + transform2->position * w2;

				const Quaternion& q1 = transform1->rotation;
				const Quaternion& q2 = transform2->rotation;
				Quaternion q3 = (Dot(q1, q2) > 0.0F) ? q1 * w1 + q2 * w2 : q1 * w1 - q2 * w2;
				animatorTransformTable[a].rotation = q3.Normalize();
			}
		}

		if (morphNodeCount != 0)
		{
			int32 morphNodeStart1 = animator1->GetOutputMorphNodeStart();
			int32 morphNodeStart2 = animator2->GetOutputMorphNodeStart();

			float *animatorMorphWeightTable = GetAnimatorMorphWeightTable();
			const float *const *morphWeightTable1 = animator1->GetOutputMorphWeightTable() + (blendMorphNodeStart - morphNodeStart1);
			const float *const *morphWeightTable2 = animator2->GetOutputMorphWeightTable() + (blendMorphNodeStart - morphNodeStart2);

			const Geometry *const *morphNodeTable = GetTargetModel()->GetAnimatedMorphNodeTable() + blendMorphNodeStart;
			for (machine a = 0; a < morphNodeCount; a++)
			{
				const float *morphWeight1 = morphWeightTable1[a];
				const float *morphWeight2 = morphWeightTable2[a];

				int32 morphWeightCount = static_cast<MorphController *>(morphNodeTable[a]->GetController())->GetMorphWeightCount() + 1;
				for (machine b = 0; b < morphWeightCount; b++)
				{
					animatorMorphWeightTable[b] = morphWeight1[b] * w1 + morphWeight2[b] * w2;
				}

				animatorMorphWeightTable += morphWeightCount;
			}
		}
	}
	else if (w1 > K::min_float)
	{
		if (transformCount != 0)
		{
			AnimatorTransform *animatorTransformTable = GetAnimatorTransformTable();
			const AnimatorTransform *const *transformTable1 = animator1->GetOutputTransformTable() + (blendTransformStart - animator1->GetOutputTransformNodeStart());

			for (machine a = 0; a < transformCount; a++)
			{
				const AnimatorTransform *transform1 = transformTable1[a];
				animatorTransformTable[a].position = transform1->position;
				animatorTransformTable[a].rotation = transform1->rotation;
			}
		}

		if (morphNodeCount != 0)
		{
			float *animatorMorphWeightTable = GetAnimatorMorphWeightTable();
			const float *const *morphWeightTable1 = animator1->GetOutputMorphWeightTable() + (blendMorphNodeStart - animator1->GetOutputMorphNodeStart());

			const Geometry *const *morphNodeTable = GetTargetModel()->GetAnimatedMorphNodeTable() + blendMorphNodeStart;
			for (machine a = 0; a < morphNodeCount; a++)
			{
				const float *morphWeight1 = morphWeightTable1[a];
				int32 morphWeightCount = static_cast<MorphController *>(morphNodeTable[a]->GetController())->GetMorphWeightCount() + 1;
				for (machine b = 0; b < morphWeightCount; b++)
				{
					animatorMorphWeightTable[b] = morphWeight1[b];
				}

				animatorMorphWeightTable += morphWeightCount;
			}
		}
	}
	else
	{
		if (transformCount != 0)
		{
			AnimatorTransform *animatorTransformTable = GetAnimatorTransformTable();
			const AnimatorTransform *const *transformTable2 = animator2->GetOutputTransformTable() + (blendTransformStart - animator2->GetOutputTransformNodeStart());

			for (machine a = 0; a < transformCount; a++)
			{
				const AnimatorTransform *transform2 = transformTable2[a];
				animatorTransformTable[a].position = transform2->position;
				animatorTransformTable[a].rotation = transform2->rotation;
			}
		}

		if (morphNodeCount != 0)
		{
			float *animatorMorphWeightTable = GetAnimatorMorphWeightTable();
			const float *const *morphWeightTable2 = animator2->GetOutputMorphWeightTable() + (blendMorphNodeStart - animator2->GetOutputMorphNodeStart());

			const Geometry *const *morphNodeTable = GetTargetModel()->GetAnimatedMorphNodeTable() + blendMorphNodeStart;
			for (machine a = 0; a < morphNodeCount; a++)
			{
				const float *morphWeight2 = morphWeightTable2[a];
				int32 morphWeightCount = static_cast<MorphController *>(morphNodeTable[a]->GetController())->GetMorphWeightCount() + 1;
				for (machine b = 0; b < morphWeightCount; b++)
				{
					animatorMorphWeightTable[b] = morphWeight2[b];
				}

				animatorMorphWeightTable += morphWeightCount;
			}
		}
	}
}


FrameAnimator::FrameAnimator() : Animator(kAnimatorFrame)
{
	animationResource = nullptr;
	animationHeader = nullptr;

	transformTrackHeader = nullptr;
	morphWeightTrackHeader = nullptr;
	cueTrackHeader = nullptr;
}

FrameAnimator::FrameAnimator(Model *model, Node *node) : Animator(kAnimatorFrame, model, node)
{
	animationResource = nullptr;
	animationHeader = nullptr;

	transformTrackHeader = nullptr;
	morphWeightTrackHeader = nullptr;
	cueTrackHeader = nullptr;
}

FrameAnimator::~FrameAnimator()
{
	if (animationResource)
	{
		animationResource->Release();
	}
}

void FrameAnimator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Animator::Pack(data, packFlags);

	if (animationResource)
	{
		PackHandle handle = data.BeginChunk('ANIM');
		data << animationResource->GetName();
		data.EndChunk(handle);

		handle = data.BeginChunk('INTP');
		frameInterpolator.Pack(data, packFlags);
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void FrameAnimator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Animator::Unpack(data, unpackFlags);
	UnpackChunkList<FrameAnimator>(data, unpackFlags);
}

bool FrameAnimator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'ANIM':
		{
			ResourceName	name;

			data >> name;
			SetAnimation(name);
			return (true);
		}

		case 'INTP':

			frameInterpolator.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

void FrameAnimator::GenerateNodeRemapTables(void) const
{
	if (transformTrackHeader)
	{
		int16 *remapTable = transformRemapTable;

		const TransformTrackHeader::NodeData *nodeData = transformTrackHeader->GetNodeData();
		unsigned_int32 bucketMask = transformTrackHeader->bucketCount - 1;

		const Node *target = GetTargetNode();
		const Node *node = (target == GetTargetModel()) ? target->GetFirstSubnode() : target;

		int32 nodeIndex = 0;
		while (node)
		{
			if (node->GetNodeType() == kNodeModel)
			{
				node = target->GetNextLevelNode(node);
			}
			else
			{
				unsigned_int32 flags = node->GetNodeFlags();
				if (!(flags & kNodeAnimateInhibit))
				{
					if (!(flags & kNodeTransformAnimationInhibit))
					{
						unsigned_int32 nodeHash = node->GetNodeHash();
						const TransformTrackHeader::NodeBucket *nodeBucket = &transformTrackHeader->bucketData[nodeHash & bucketMask];

						int32 index = nodeIndex;
						int32 nodeCount = nodeBucket->bucketNodeCount;
						const TransformTrackHeader::NodeData *data = nodeData + nodeBucket->nodeDataOffset;
						for (machine a = 0; a < nodeCount; a++)
						{
							if (data->nodeHash == nodeHash)
							{
								index = data->transformIndex;
								break;
							}

							data++;
						}

						*remapTable++ = (int16) index;
						nodeIndex++;
					}
				}
				else if (!(flags & kNodeAnimateSubtree))
				{
					node = target->GetNextLevelNode(node);
					continue;
				}

				node = target->GetNextNode(node);
			}
		}
	}

	if (morphWeightTrackHeader)
	{
		int16 *remapTable = morphWeightRemapTable;

		const MorphWeightTrackHeader::NodeData *nodeData = morphWeightTrackHeader->GetNodeData();
		const unsigned_int32 *morphHashArray = morphWeightTrackHeader->GetMorphHashArray();
		unsigned_int32 bucketMask = morphWeightTrackHeader->bucketCount - 1;

		const Node *target = GetTargetNode();
		const Node *node = (target == GetTargetModel()) ? target->GetFirstSubnode() : target;

		while (node)
		{
			if (node->GetNodeType() == kNodeModel)
			{
				node = target->GetNextLevelNode(node);
			}
			else
			{
				unsigned_int32 flags = node->GetNodeFlags();
				if (!(flags & kNodeAnimateInhibit))
				{
					if (!(flags & kNodeMorphWeightAnimationInhibit))
					{
						const Controller *controller = node->GetController();
						if (Model::ActiveMorphController(controller))
						{
							const MorphController *morphController = static_cast<const MorphController *>(controller);
							int32 morphWeightCount = morphController->GetMorphWeightCount() + 1;

							unsigned_int32 nodeHash = node->GetNodeHash();
							const MorphWeightTrackHeader::NodeBucket *nodeBucket = &morphWeightTrackHeader->bucketData[nodeHash & bucketMask];

							int32 nodeCount = nodeBucket->bucketNodeCount;
							const MorphWeightTrackHeader::NodeData *data = nodeData + nodeBucket->nodeDataOffset;
							for (machine a = 0; a < nodeCount; a++)
							{
								if (data->nodeHash == nodeHash)
								{
									unsigned_int32 dataWeightCount = data->weightCount;
									unsigned_int32 dataOffset = data->dataOffset;
									const unsigned_int32 *hashArray = morphHashArray + dataOffset;

									int32 weightIndex = -1;
									for (unsigned_machine i = 0; i < dataWeightCount; i++)
									{
										if (hashArray[i] == 0)
										{
											weightIndex = dataOffset + i;
											break;
										}
									}

									remapTable[0] = (int16) weightIndex;

									for (machine b = 1; b < morphWeightCount; b++)
									{
										weightIndex = -1;
										unsigned_int32 morphHash = morphController->GetMorphHash(b - 1);
										for (unsigned_machine i = 0; i < dataWeightCount; i++)
										{
											if (hashArray[i] == morphHash)
											{
												weightIndex = dataOffset + i;
												break;
											}
										}

										remapTable[b] = (int16) weightIndex;
									}

									goto end;
								}

								data++;
							}

							// If the node was not found in the animation data, then the output weights
							// are 1.0 for the base mesh and 0.0 for all other morph targets.

							remapTable[0] = -2;
							for (machine a = 1; a < morphWeightCount; a++)
							{
								remapTable[a] = -1;
							}

							end:
							remapTable += morphWeightCount;
						}
					}
				}
				else if (!(flags & kNodeAnimateSubtree))
				{
					node = target->GetNextLevelNode(node);
					continue;
				}

				node = target->GetNextNode(node);
			}
		}
	}
}

void FrameAnimator::Configure(void)
{
	int32 transformNodeStart = GetAnimatorTransformNodeStart();
	int32 transformNodeCount = GetAnimatorTransformNodeCount();
	int32 morphNodeStart = GetAnimatorMorphNodeStart();
	int32 morphNodeCount = GetAnimatorMorphNodeCount();
	int32 morphWeightStart = GetAnimatorMorphWeightStart();
	int32 morphWeightCount = GetAnimatorMorphWeightCount();

	unsigned_int32 animatorDataSize = transformNodeCount * 2 + morphWeightCount * 2;
	AllocateStorage(transformNodeStart, transformNodeCount, transformNodeCount, morphNodeStart, morphNodeCount, morphWeightStart, morphWeightCount, morphWeightCount, animatorDataSize);

	int16 *remapTable = static_cast<int16 *>(GetAnimatorData());

	if (transformNodeCount != 0)
	{
		transformRemapTable = remapTable;
		remapTable += transformNodeCount;

		AnimatorTransform **outputTransformTable = GetOutputTransformTable();
		AnimatorTransform *animatorTransformTable = GetAnimatorTransformTable();

		for (machine a = 0; a < transformNodeCount; a++)
		{
			outputTransformTable[a] = &animatorTransformTable[a];
		}
	}

	if (morphNodeCount != 0)
	{
		morphWeightRemapTable = remapTable;
		remapTable += morphWeightCount;

		float **outputMorphWeightTable = GetOutputMorphWeightTable();
		float *animatorMorphWeightTable = GetAnimatorMorphWeightTable();

		const Geometry *const *morphNodeTable = GetTargetModel()->GetAnimatedMorphNodeTable() + morphNodeStart;
		for (machine a = 0; a < morphNodeCount; a++)
		{
			outputMorphWeightTable[a] = animatorMorphWeightTable;
			animatorMorphWeightTable[0] = 1.0F;

			int32 weightCount = static_cast<MorphController *>(morphNodeTable[a]->GetController())->GetMorphWeightCount() + 1;
			for (machine b = 1; b < weightCount; b++)
			{
				animatorMorphWeightTable[b] = 0.0F;
			}

			animatorMorphWeightTable += morphWeightCount;
		}
	}

	GenerateNodeRemapTables();
}

void FrameAnimator::ExecuteAnimationFrame(float frame)
{
	float	f1, f2;

	PositiveFloorCeil(frame, &f1, &f2);

	float t2 = frame - f1;
	float t1 = 1.0F - t2;

	int32 i1 = (int32) f1;
	int32 i2 = (int32) f2;

	if (transformTrackHeader)
	{
		AnimatorTransform *transformTable = GetAnimatorTransformTable();
		const int16 *remapTable = transformRemapTable;

		int32 trackNodeCount = transformTrackHeader->transformNodeCount;
		const TransformFrameData *data = transformTrackHeader->GetTransformFrameData();
		const TransformFrameData *frameData1 = data + i1 * trackNodeCount;
		const TransformFrameData *frameData2 = data + i2 * trackNodeCount;

		int32 count = GetOutputTransformNodeCount();
		for (machine a = 0; a < count; a++)
		{
			Quaternion		q1, q2;

			int32 index = remapTable[a];
			const TransformFrameData *fd1 = frameData1 + index;
			const TransformFrameData *fd2 = frameData2 + index;

			transformTable[a].position = fd1->position * t1 + fd2->position * t2;

			q1.SetRotationMatrix(fd1->transform);
			q2.SetRotationMatrix(fd2->transform);
			Quaternion q3 = (Dot(q1, q2) > 0.0F) ? q1 * t1 + q2 * t2 : q1 * t1 - q2 * t2;
			transformTable[a].rotation = q3.Normalize();
		}
	}

	if (morphWeightTrackHeader)
	{
		float *morphWeightTable = GetAnimatorMorphWeightTable();
		const int16 *remapTable = morphWeightRemapTable;

		int32 frameMorphWeightCount = morphWeightTrackHeader->frameMorphWeightCount;
		const float *data = morphWeightTrackHeader->GetMorphWeightFrameData();
		const float *frameData1 = data + i1 * frameMorphWeightCount;
		const float *frameData2 = data + i2 * frameMorphWeightCount;

		int32 count = GetOutputMorphWeightCount();
		for (machine a = 0; a < count; a++)
		{
			int32 index = remapTable[a];
			morphWeightTable[a] = frameData1[index] * t1 + frameData2[index] * t2;
		}
	}

	animationFrame = frame;
}

void FrameAnimator::Move(void)
{
	Animator::Move();

	const AnimationHeader *header = animationHeader;
	if (header)
	{
		SubrangeData	subrangeData;

		float value = frameInterpolator.UpdateValue(&subrangeData);

		if (animationHeader != header)
		{
			float frame = frameInterpolator.GetValue() * frameFrequency;
			if ((GetWeightInterpolator()->GetValue() > 0.0F) || (animationFrame < 0.0F))
			{
				ExecuteAnimationFrame(frame);
			}
		}
		else
		{
			float frame = value * frameFrequency;
			if (animationFrame != frame)
			{
				if ((GetWeightInterpolator()->GetValue() > 0.0F) || (animationFrame < 0.0F))
				{
					ExecuteAnimationFrame(frame);

					const CueTrackHeader *track = cueTrackHeader;
					if (track)
					{
						int32 cueCount = track->cueCount;
						int32 subrangeCount = subrangeData.subrangeCount;
						const Range<float> *subrange = subrangeData.subrange;

						for (machine a = 0; a < subrangeCount; a++)
						{
							float begin = subrange->min;
							float end = subrange->max;

							if (begin < end)
							{
								const CueData *cueData = track->GetCueData();
								for (machine b = 0; b < cueCount; b++)
								{
									float time = cueData->cueTime;
									if (time > end)
									{
										break;
									}

									if (time > begin)
									{
										PostEvent(cueData->cueType);
									}

									cueData++;
								}
							}
							else
							{
								const CueData *cueData = track->GetCueData() + (cueCount - 1);
								for (machine b = 0; b < cueCount; b++)
								{
									float time = cueData->cueTime;
									if (time < end)
									{
										break;
									}

									if (time < begin)
									{
										PostEvent(cueData->cueType);
									}

									cueData--;
								}
							}

							subrange++;
						}
					}
				}
			}
		}
	}
	else
	{
		AnimatorTransform *transformTable = GetAnimatorTransformTable();

		int32 count = GetOutputTransformNodeCount();
		for (machine a = 0; a < count; a++)
		{
			transformTable[a].rotation = 1.0F;
			transformTable[a].position.Set(0.0F, 0.0F, 0.0F);
		}
	}
}

void FrameAnimator::SetAnimationHeader(const AnimationHeader *header)
{
	animationHeader = header;
	transformTrackHeader = nullptr;
	morphWeightTrackHeader = nullptr;
	cueTrackHeader = nullptr;

	int32 trackCount = header->trackCount;
	for (machine a = 0; a < trackCount; a++)
	{
		TrackType type = header->trackData[a].trackType;
		if (type == kTrackTransform)
		{
			transformTrackHeader = static_cast<const TransformTrackHeader *>(header->GetTrackHeader(a));
		}
		else if (type == kTrackMorphWeight)
		{
			morphWeightTrackHeader = static_cast<const MorphWeightTrackHeader *>(header->GetTrackHeader(a));
		}
		else if (type == kTrackCue)
		{
			cueTrackHeader = static_cast<const CueTrackHeader *>(header->GetTrackHeader(a));
		}
	}

	float frameCount = (float) (header->frameCount - 1);
	float frameDuration = header->frameDuration;

	animationDuration = frameDuration * frameCount;
	frameFrequency = 1.0F / frameDuration;
	animationFrame = -1.0F;

	frameInterpolator.Set(0.0F, 1.0F, kInterpolatorStop);
	frameInterpolator.SetRange(0.0F, animationDuration);

	if (GetAnimatorData())
	{
		GenerateNodeRemapTables();
	}
}

void FrameAnimator::SetAnimation(const char *name)
{
	if (animationResource)
	{
		animationResource->Release();
		animationResource = nullptr;

		animationHeader = nullptr;
		transformTrackHeader = nullptr;
		morphWeightTrackHeader = nullptr;
		cueTrackHeader = nullptr;
	}

	if (name)
	{
		AnimationResource *resource = AnimationResource::Get(name);
		if (resource)
		{
			animationResource = resource;
			SetAnimationHeader(resource->GetAnimationHeader());
		}
	}
}

// ZYUQURM
