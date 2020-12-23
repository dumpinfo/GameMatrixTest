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


#include "C4InstanceManipulators.h"
#include "C4WorldEditor.h"
#include "C4EditorSupport.h"


using namespace C4;


InstanceManipulator::InstanceManipulator(Instance *instance) : EditorManipulator(instance, "WorldEditor/world/World")
{
	SetManipulatorFlags(kManipulatorLockedSubtree | kManipulatorAdjustablePlacement);
}

InstanceManipulator::~InstanceManipulator()
{
}

const char *InstanceManipulator::GetDefaultNodeName(void) const
{
	const char *name = GetTargetNode()->GetWorldName();
	if (name[0] != 0)
	{
		return (&name[Text::GetDirectoryPathLength(name)]);
	}

	return (TheWorldEditor->GetStringTable()->GetString(StringID('NAME', kNodeInstance)));
}

void InstanceManipulator::Preprocess(void)
{
	if (!GetTargetNode()->GetFirstSubnode())
	{
		SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
	}

	EditorManipulator::Preprocess();
}

void InstanceManipulator::Show(void)
{
	EditorManipulator::Show();

	Node *node = GetTargetNode();
	const Node *subnode = node->GetFirstSubnode();
	while (subnode)
	{
		if (!(subnode->GetNodeFlags() & kNodeInstanceHidden))
		{
			Manipulator *manipulator = subnode->GetManipulator();
			manipulator->SetManipulatorState(manipulator->GetManipulatorState() & ~kManipulatorHidden);
		}

		subnode = node->GetNextNode(subnode);
	}
}

void InstanceManipulator::Hide(void)
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

void InstanceManipulator::Select(void)
{
	EditorManipulator::Select();
	HiliteSubtree();
}

void InstanceManipulator::Unselect(void)
{
	EditorManipulator::Unselect();
	UnhiliteSubtree();
}

void InstanceManipulator::HandleDelete(bool undoable)
{
	EditorManipulator::HandleDelete(undoable);
	GetTargetNode()->Collapse();
}

void InstanceManipulator::HandleUndelete(void)
{
	EditorManipulator::HandleUndelete();
	GetEditor()->ExpandWorld(GetTargetNode());
}

void InstanceManipulator::HandleSettingsUpdate(void)
{
	EditorManipulator::HandleSettingsUpdate();

	Editor *editor = GetEditor();
	if (editor->GetEditorObject()->GetEditorFlags() & kEditorExpandWorlds) 
	{
		Instance *instance = GetTargetNode();
		instance->Collapse(); 
		editor->ExpandWorld(instance);
	} 
}

bool InstanceManipulator::MaterialSettable(void) const 
{
	const Modifier *modifier = GetTargetNode()->GetFirstModifier(); 
	while (modifier) 
	{
		if (modifier->GetModifierType() == kModifierReplaceMaterial)
		{
			return (true); 
		}

		modifier = modifier->Next();
	}

	return (false);
}

int32 InstanceManipulator::GetMaterialCount(void) const
{
	int32 count = 0;

	Modifier *modifier = GetTargetNode()->GetFirstModifier();
	while (modifier)
	{
		if (modifier->GetModifierType() == kModifierReplaceMaterial)
		{
			ReplaceMaterialModifier *replaceMaterialModifier = static_cast<ReplaceMaterialModifier *>(modifier);
			if (replaceMaterialModifier->GetMaterialObject())
			{
				count++;
			}
		}

		modifier = modifier->Next();
	}

	return (count);
}

MaterialObject *InstanceManipulator::GetMaterial(int32 index) const
{
	int32 count = 0;

	Modifier *modifier = GetTargetNode()->GetFirstModifier();
	while (modifier)
	{
		if (modifier->GetModifierType() == kModifierReplaceMaterial)
		{
			ReplaceMaterialModifier *replaceMaterialModifier = static_cast<ReplaceMaterialModifier *>(modifier);
			MaterialObject *materialObject = replaceMaterialModifier->GetMaterialObject();
			if (materialObject)
			{
				if (count == index)
				{
					return (materialObject);
				}

				count++;
			}
		}

		modifier = modifier->Next();
	}

	return (nullptr);
}

void InstanceManipulator::SetMaterial(MaterialObject *material)
{
	Instance *instance = GetTargetNode();
	bool reload = false;

	Modifier *modifier = instance->GetFirstModifier();
	while (modifier)
	{
		if (modifier->GetModifierType() == kModifierReplaceMaterial)
		{
			ReplaceMaterialModifier *replaceMaterialModifier = static_cast<ReplaceMaterialModifier *>(modifier);
			replaceMaterialModifier->SetMaterialObject(material);
			reload = true;
		}

		modifier = modifier->Next();
	}

	if (reload)
	{
		Editor *editor = GetEditor();
		if (editor->GetEditorObject()->GetEditorFlags() & kEditorExpandWorlds)
		{
			editor->InvalidateAllViewports();

			CollapseWorld();
			ExpandWorld();
		}
	}
}

void InstanceManipulator::ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial)
{
	Instance *instance = GetTargetNode();
	bool reload = false;

	Modifier *modifier = instance->GetFirstModifier();
	while (modifier)
	{
		if (modifier->GetModifierType() == kModifierReplaceMaterial)
		{
			ReplaceMaterialModifier *replaceMaterialModifier = static_cast<ReplaceMaterialModifier *>(modifier);
			if (replaceMaterialModifier->GetMaterialObject() == oldMaterial)
			{
				replaceMaterialModifier->SetMaterialObject(newMaterial);
				reload = true;
			}
		}

		modifier = modifier->Next();
	}

	if (reload)
	{
		Editor *editor = GetEditor();
		if (editor->GetEditorObject()->GetEditorFlags() & kEditorExpandWorlds)
		{
			editor->InvalidateAllViewports();

			CollapseWorld();
			ExpandWorld();
		}
	}
}

bool InstanceManipulator::ReparentedSubnodesAllowed(void) const
{
	return (false);
}

Box3D InstanceManipulator::CalculateNodeBoundingBox(void) const
{
	Box3D	instanceBox;

	const Instance *instance = GetTargetNode();
	const Transform4D& inverseTransform = instance->GetInverseWorldTransform();

	const Node *node = instance->GetFirstSubnode();
	while (node)
	{
		if (node->CalculateBoundingBox(&instanceBox))
		{
			instanceBox.Transform(inverseTransform * node->GetWorldTransform());
			break;
		}

		node = instance->GetNextNode(node);
	}

	if (node)
	{
		for (;;)
		{
			Box3D	box;

			node = instance->GetNextNode(node);
			if (!node)
			{
				break;
			}

			if (node->CalculateBoundingBox(&box))
			{
				instanceBox.Union(Transform(box, inverseTransform * node->GetWorldTransform()));
			}
		}

		return (instanceBox);
	}

	return (EditorManipulator::CalculateNodeBoundingBox());
}

Box3D InstanceManipulator::CalculateWorldBoundingBox(void) const
{
	const Instance *instance = GetTargetNode();
	if (instance->GetFirstOutgoingEdge())
	{
		return (instance->GetWorldBoundingBox());
	}

	return (EditorManipulator::CalculateWorldBoundingBox());
}

void InstanceManipulator::ExpandWorld(void)
{
	Instance *instance = GetTargetNode();
	instance->Expand(instance->GetWorld());
	instance->EstablishVisibility();

	Node *node = instance->GetFirstSubnode();
	while (node)
	{
		EditorManipulator::Install(GetEditor(), node);
		node->Preprocess();
		node = node->Next();
	}

	instance->Invalidate();
	InvalidateGraph();

	if (instance->GetFirstSubnode())
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
	else
	{
		node = instance->GetSuperNode();
		do
		{
			if ((node->GetNodeType() == kNodeInstance) && (Editor::GetManipulator(node)->Selected()))
			{
				HiliteSubtree();
				break;
			}

			node = node->GetSuperNode();
		} while (node);
	}
}

void InstanceManipulator::CollapseWorld(void)
{
	GetTargetNode()->Collapse();
	InvalidateGraph();

	SetManipulatorState(GetManipulatorState() | kManipulatorShowIcon);
}

void InstanceManipulator::InvalidatePaintSpace(void)
{
	Instance *instance = GetTargetNode();

	Node *node = instance->GetFirstSubnode();
	while (node)
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			node->ProcessInternalConnectors();
			static_cast<Geometry *>(node)->InvalidateShaderData();
		}

		node = instance->GetNextNode(node);
	}
}

// ZYUQURM
