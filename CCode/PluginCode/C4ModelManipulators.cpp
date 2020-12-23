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


#include "C4ModelManipulators.h"
#include "C4WorldEditor.h"
#include "C4EditorSupport.h"


using namespace C4;


namespace
{
	const ConstColorRGBA kBoneRenderColor = {1.0F, 0.75F, 0.0F, 1.0F};
}


BoneManipulator::BoneManipulator(Bone *bone) :
		EditorManipulator(bone, "WorldEditor/node/Bone"),
		boneVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		boneDiffuseColor(kBoneRenderColor),
		boneRenderable(kRenderLines)
{
	boneRenderable.SetShaderFlags(kShaderAmbientEffect);
	boneRenderable.SetTransformable(bone->GetSkeletonRoot());

	boneRenderable.SetVertexCount(2);
	boneRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &boneVertexBuffer, sizeof(Point3D));
	boneRenderable.SetVertexAttributeArray(kArrayPosition, 0, 3);
	boneVertexBuffer.Establish(sizeof(Point3D) * 2);

	boneAttributeList.Append(&boneDiffuseColor);
	boneRenderable.SetMaterialAttributeList(&boneAttributeList);
}

BoneManipulator::~BoneManipulator()
{
}

const char *BoneManipulator::GetDefaultNodeName(void) const
{
	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeBone)));
}

void BoneManipulator::InvalidateNode(void)
{
	EditorManipulator::InvalidateNode();

	Bone *bone = GetTargetNode();
	Node *node = bone->GetSuperNode();
	while (node)
	{
		NodeType type = node->GetNodeType();
		if ((type != kNodeBone) && (type != kNodeGeneric))
		{
			Node *subnode = node->GetFirstSubnode();
			while (subnode)
			{
				if (subnode->GetNodeType() == kNodeGeometry)
				{
					Controller *controller = subnode->GetController();
					if ((controller) && (controller->GetControllerType() == kControllerSkin))
					{
						controller->Invalidate();
						subnode->Invalidate();
					}
				}

				subnode = node->GetNextNode(subnode);
			}

			break;
		}

		node = node->GetSuperNode();
	}
}

bool BoneManipulator::CalculateNodeSphere(BoundingSphere *sphere) const
{
	const Node *node = GetTargetNode();
	const Node *super = node->GetSuperNode();

	if ((super) && (super->GetNodeType() == kNodeBone))
	{
		Point3D center = node->GetInverseWorldTransform() * super->GetWorldPosition() * 0.5F;
		sphere->SetCenter(center);
		sphere->SetRadius(Magnitude(center));
		return (true);
	}

	return (false);
}

void BoneManipulator::Render(const ManipulatorRenderData *renderData)
{
	List<Renderable> *renderList = renderData->manipulatorList;
	if (renderList)
	{
		Node *node = GetTargetNode();
		Node *super = node->GetSuperNode();

		if ((super) && (super->GetNodeType() == kNodeBone))
		{ 
			Point3D		vertex[2];

			vertex[0] = node->GetWorldPosition(); 
			vertex[1] = super->GetWorldPosition();
 
			boneVertexBuffer.UpdateBuffer(0, sizeof(Point3D) * 2, vertex);
			renderList->Append(&boneRenderable);
		} 
	}
 
	EditorManipulator::Render(renderData); 
}


ModelManipulator::ModelManipulator(Model *model) : EditorManipulator(model, "WorldEditor/model/Model") 
{
	SetManipulatorFlags(kManipulatorLockedSubtree | kManipulatorAdjustablePlacement);
}

ModelManipulator::~ModelManipulator()
{
}

const char *ModelManipulator::GetDefaultNodeName(void) const
{
	const ModelRegistration *registration = Model::FindRegistration(GetTargetNode()->GetModelType());
	if (registration)
	{
		const char *name = registration->GetModelName();
		if (name)
		{
			return (name);
		}
	}

	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeModel)));
}

void ModelManipulator::Preprocess(void)
{
	if (!GetTargetNode()->GetFirstSubnode())
	{
		SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	}

	EditorManipulator::Preprocess();
}

void ModelManipulator::Show(void)
{
	EditorManipulator::Show();

	Node *node = GetTargetNode();
	const Node *subnode = node->GetFirstSubnode();
	while (subnode)
	{
		Manipulator *manipulator = subnode->GetManipulator();
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kManipulatorHidden);

		subnode = node->GetNextNode(subnode);
	}
}

void ModelManipulator::Hide(void)
{
	EditorManipulator::Hide();

	Node *node = GetTargetNode();
	const Node *subnode = node->GetFirstSubnode();
	while (subnode)
	{
		Manipulator *manipulator = subnode->GetManipulator();
		manipulator->SetManipulatorState(manipulator->GetManipulatorState() | kManipulatorHidden);

		subnode = node->GetNextNode(subnode);
	}
}

void ModelManipulator::Select(void)
{
	EditorManipulator::Select();
	HiliteSubtree();
}

void ModelManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	UnhiliteSubtree();
}

void ModelManipulator::HandleDelete(bool undoable)
{
	EditorManipulator::HandleDelete(undoable);
	GetTargetNode()->Unload();
}

void ModelManipulator::HandleUndelete(void)
{
	EditorManipulator::HandleUndelete();
	GetEditor()->ExpandModel(GetTargetNode());
}

void ModelManipulator::HandleSettingsUpdate(void)
{
	EditorManipulator::HandleSettingsUpdate();

	Editor *editor = GetEditor();
	if (editor->GetEditorObject()->GetEditorFlags() & kEditorExpandModels)
	{
		Model *model = GetTargetNode();
		model->Unload();
		editor->ExpandModel(model);
	}
}

bool ModelManipulator::ReparentedSubnodesAllowed(void) const
{
	return (false);
}

Box3D ModelManipulator::CalculateNodeBoundingBox(void) const
{
	Box3D	modelBox;

	const Model *model = GetTargetNode();
	const Transform4D& inverseTransform = model->GetInverseWorldTransform();

	const Node *node = model->GetFirstSubnode();
	while (node)
	{
		if (node->CalculateBoundingBox(&modelBox))
		{
			modelBox.Transform(inverseTransform * node->GetWorldTransform());
			break;
		}

		node = model->GetNextNode(node);
	}

	if (node)
	{
		for (;;)
		{
			Box3D	box;

			node = model->GetNextNode(node);
			if (!node)
			{
				break;
			}

			if (node->CalculateBoundingBox(&box))
			{
				modelBox.Union(Transform(box, inverseTransform * node->GetWorldTransform()));
			}
		}

		return (modelBox);
	}

	return (EditorManipulator::CalculateNodeBoundingBox());
}

Box3D ModelManipulator::CalculateWorldBoundingBox(void) const
{
	const Model *model = GetTargetNode();
	if (model->GetFirstOutgoingEdge())
	{
		return (model->GetWorldBoundingBox());
	}

	return (EditorManipulator::CalculateWorldBoundingBox());
}

void ModelManipulator::ExpandModel(void)
{
	Model *model = GetTargetNode();
	model->Load(model->GetWorld());

	Node *node = model->GetFirstSubnode();
	while (node)
	{
		EditorManipulator::Install(GetEditor(), node);
		node = node->Next();
	}

	node = model->GetFirstSubnode();
	while (node)
	{
		node->SetNodeFlags(node->GetNodeFlags() | kNodeNonpersistent);
		node = model->GetNextNode(node);
	}

	model->Preprocess();
	model->Invalidate();
	InvalidateGraph();

	if (model->GetFirstSubnode())
	{
		SetManipulatorState(GetManipulatorState() & ~kManipulatorShowIcon);
	}
	else
	{
		SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	}

	if (Selected())
	{
		HiliteSubtree();
	}
}

void ModelManipulator::CollapseModel(void)
{
	Model *model = GetTargetNode();
	model->Unload();

	model->Preprocess();
	model->Invalidate();
	InvalidateGraph();

	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
}

// ZYUQURM
