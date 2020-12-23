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


#ifndef C4LightManipulators_h
#define C4LightManipulators_h


#include "C4EditorManipulators.h"
#include "C4VolumeManipulators.h"
#include "C4Lights.h"


namespace C4
{
	class LightManipulator : public EditorManipulator
	{
		friend class EditorManipulator;

		private:

			static Manipulator *Create(Light *light);

		public:

			LightManipulator(Light *light, const char *iconName);
			~LightManipulator();

			Light *GetTargetNode(void) const
			{
				return (static_cast<Light *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;
	};


	class InfiniteLightManipulator : public LightManipulator
	{
		private:

			struct LightVertex
			{
				Point3D		position;
				float		radius;
				Point2D		texcoord;
			};

			static SharedVertexBuffer	directionVertexBuffer;
			List<Attribute>				directionAttributeList;
			DiffuseAttribute			directionColorAttribute;
			DiffuseTextureAttribute		directionTextureAttribute;
			Renderable					directionRenderable;
			Vector4D					directionSizeVector;

		public:

			InfiniteLightManipulator(InfiniteLight *infiniteLight);
			~InfiniteLightManipulator();

			InfiniteLight *GetTargetNode(void) const
			{
				return (static_cast<InfiniteLight *>(EditorManipulator::GetTargetNode()));
			}

			InfiniteLightObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			void Render(const ManipulatorRenderData *renderData) override;
	};


	class PointLightManipulator : public LightManipulator, public SphereVolumeManipulator
	{
		private:

			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override;

		public:

			PointLightManipulator(PointLight *pointLight);
			~PointLightManipulator();

			PointLight *GetTargetNode(void) const
			{
				return (static_cast<PointLight *>(EditorManipulator::GetTargetNode()));
			}

			PointLightObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			void Select(void) override;
			void Unselect(void) override;

			Box3D CalculateNodeBoundingBox(void) const override;
 
			bool Resize(const ManipulatorResizeData *resizeData) override;

			void Update(void) override; 
			void Render(const ManipulatorRenderData *renderData) override;
	}; 


	class SpotLightManipulator : public LightManipulator, public ProjectionVolumeManipulator 
	{
		private: 
 
			bool CalculateNodeSphere(BoundingSphere *sphere) const;

			int32 GetHandleTable(Point3D *handle) const;
			void GetHandleData(int32 index, ManipulatorHandleData *handleData) const override; 

		public:

			SpotLightManipulator(SpotLight *spotLight);
			~SpotLightManipulator();

			SpotLight *GetTargetNode(void) const
			{
				return (static_cast<SpotLight *>(EditorManipulator::GetTargetNode()));
			}

			SpotLightObject *GetObject(void) const
			{
				return (GetTargetNode()->GetObject());
			}

			void Select(void) override;
			void Unselect(void) override;

			Box3D CalculateNodeBoundingBox(void) const override;

			bool Resize(const ManipulatorResizeData *resizeData) override;

			void Update(void) override;
			void Render(const ManipulatorRenderData *renderData) override;
	};
}


#endif

// ZYUQURM
