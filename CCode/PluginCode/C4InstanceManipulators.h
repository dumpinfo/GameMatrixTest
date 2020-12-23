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


#ifndef C4InstanceManipulators_h
#define C4InstanceManipulators_h


#include "C4EditorManipulators.h"
#include "C4Instances.h"


namespace C4
{
	class InstanceManipulator : public EditorManipulator
	{
		public:

			InstanceManipulator(Instance *instance);
			~InstanceManipulator();

			Instance *GetTargetNode(void) const
			{
				return (static_cast<Instance *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;

			void Show(void) override;
			void Hide(void) override;

			void Select(void) override;
			void Unselect(void) override;

			void HandleDelete(bool undoable) override;
			void HandleUndelete(void) override;
			void HandleSettingsUpdate(void) override;

			bool MaterialSettable(void) const override;
			int32 GetMaterialCount(void) const override;
			MaterialObject *GetMaterial(int32 index) const override;
			void SetMaterial(MaterialObject *material) override;
			void ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial) override;

			bool ReparentedSubnodesAllowed(void) const override;

			Box3D CalculateNodeBoundingBox(void) const override;
			Box3D CalculateWorldBoundingBox(void) const override;

			void ExpandWorld(void);
			void CollapseWorld(void);

			void InvalidatePaintSpace(void);
	};
}


#endif

// ZYUQURM
