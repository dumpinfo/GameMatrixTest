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


#ifndef C4ModelManipulators_h
#define C4ModelManipulators_h


#include "C4EditorManipulators.h"
#include "C4Models.h"


namespace C4
{
	class BoneManipulator : public EditorManipulator
	{
		private:

			VertexBuffer			boneVertexBuffer;
			List<Attribute>			boneAttributeList;
			DiffuseAttribute		boneDiffuseColor;
			Renderable				boneRenderable;

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

		public:

			BoneManipulator(Bone *bone);
			~BoneManipulator();

			Bone *GetTargetNode(void) const
			{
				return (static_cast<Bone *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void InvalidateNode(void) override;
			void Render(const ManipulatorRenderData *renderData) override;
	};


	class ModelManipulator : public EditorManipulator
	{
		public:

			ModelManipulator(Model *model);
			~ModelManipulator();

			Model *GetTargetNode(void) const
			{
				return (static_cast<Model *>(EditorManipulator::GetTargetNode()));
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

			bool ReparentedSubnodesAllowed(void) const override;

			Box3D CalculateNodeBoundingBox(void) const override;
			Box3D CalculateWorldBoundingBox(void) const override;

			void ExpandModel(void);
			void CollapseModel(void);
	};
}


#endif

// ZYUQURM
