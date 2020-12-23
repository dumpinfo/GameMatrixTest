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


#ifndef C4EditorManipulators_h
#define C4EditorManipulators_h


#include "C4Manipulator.h"
#include "C4Skybox.h"
#include "C4Impostors.h"
#include "C4EditorBase.h"
#include "C4EditorConnectors.h"


namespace C4
{
	const float kSizeEpsilon = 1.0 / 1024.0F;


	const float kGraphBoxWidth = 112.0F;
	const float kGraphBoxHeight = 16.0F;


	enum
	{
		kWidgetManipulator					= 'MANP'
	};


	enum
	{
		kManipulatorHilited					= kManipulatorBaseState << 0,
		kManipulatorTempSelected			= kManipulatorBaseState << 1,
		kManipulatorTarget					= kManipulatorBaseState << 2,
		kManipulatorShowGizmo				= kManipulatorBaseState << 3,
		kManipulatorShowIcon				= kManipulatorBaseState << 4,
		kManipulatorConnectorSelected		= kManipulatorBaseState << 5,
		kManipulatorDeleted					= kManipulatorBaseState << 6,
		kManipulatorUpdated					= kManipulatorBaseState << 7,
		kManipulatorGraphValid				= kManipulatorBaseState << 8,
		kManipulatorForceRender				= kManipulatorBaseState << 9
	};


	enum
	{
		kManipulatorLockedTransform			= 1 << 0,
		kManipulatorLockedSubtree			= 1 << 1,
		kManipulatorLockedController		= 1 << 2,
		kManipulatorAdjustablePlacement		= 1 << 3
	};


	enum
	{
		kMaxManipulatorHandleCount			= 20,
		kHandleOrigin						= -1
	};


	enum
	{
		kManipulatorHandleNegativeX			= 1 << 0,
		kManipulatorHandlePositiveX			= 1 << 1,
		kManipulatorHandleNegativeY			= 1 << 2,
		kManipulatorHandlePositiveY			= 1 << 3,
		kManipulatorHandleNegativeZ			= 1 << 4,
		kManipulatorHandlePositiveZ			= 1 << 5,
		kManipulatorHandleSecondary			= 1 << 6,
		kManipulatorHandleNonzeroX			= kManipulatorHandleNegativeX | kManipulatorHandlePositiveX,
		kManipulatorHandleNonzeroY			= kManipulatorHandleNegativeY | kManipulatorHandlePositiveY,
		kManipulatorHandleNonzeroZ			= kManipulatorHandleNegativeZ | kManipulatorHandlePositiveZ
	};


	enum
	{
		kManipulatorResizeCenter			= 1 << 0,
		kManipulatorResizeConstrain			= 1 << 1
	};


	enum
	{
		kEditorSelectionObject,
		kEditorSelectionSurface,
		kEditorSelectionFace,
		kEditorSelectionVertex
	};


	class Camera;
	class Skybox;
	class PhysicsNode;
	class EditorGizmo;


	class ManipulatorWidget final : public RenderableWidget
	{
		private:

			const EditorManipulator		*editorManipulator;
			float						viewportScale;
 
			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseAttribute			diffuseAttribute; 

		public: 

			ManipulatorWidget(EditorManipulator *manipulator);
			~ManipulatorWidget(); 

			void SetViewportScale(float scale) 
			{ 
				viewportScale = scale;
				SetBuildFlag();
			}
 
			void Preprocess(void) override;
			void Build(void) override;
	};


	class EditorManipulator : public Manipulator, public ListElement<EditorManipulator>
	{
		protected:

			struct MarkerVertex
			{
				Point2D		position;
				Vector2D	billboard;
				Point2D		texcoord;
			};

			struct HandleVertex
			{
				Point3D		position;
				Vector2D	billboard;
			};

			struct BoxVertex
			{
				Point3D		position;
				Vector3D	offset;
			};

		private:

			unsigned_int32						manipulatorFlags;

			Editor								*worldEditor;
			EditorGizmo							*editorGizmo;

			int32								selectionType;
			Vector4D							manipulatorScaleVector;

			float								originalSize[kMaxObjectSizeCount];
			Point3D								originalPosition;

			BoundingSphere						nodeSphere;
			BoundingSphere						treeSphere;
			const BoundingSphere				*nodeSpherePointer;
			const BoundingSphere				*treeSpherePointer;

			static SharedVertexBuffer			markerVertexBuffer;
			List<Attribute>						markerAttributeList;
			DiffuseAttribute					markerColorAttribute;
			DiffuseTextureAttribute				markerTextureAttribute;
			Renderable							markerRenderable;

			List<Attribute>						iconAttributeList;
			DiffuseTextureAttribute				iconTextureAttribute;
			Renderable							iconRenderable;

			int32								handleCount;
			VertexBuffer						handleVertexBuffer;
			Renderable							handleRenderable;
			Point3D								handlePosition[kMaxManipulatorHandleCount];

			int32								connectorCount;
			int32								connectorSelection;
			char								*connectorStorage;
			EditorConnector						*editorConnector;

			bool								graphValid;
			float								graphWidth;
			float								graphHeight;
			ImageWidget							graphBackground;
			ImageWidget							graphImage;
			TextWidget							graphText;
			ManipulatorWidget					graphBorder;
			GuiButtonWidget						graphCollapseButton;
			WidgetObserver<EditorManipulator>	graphCollapseObserver;

			static SharedVertexBuffer			boxVertexBuffer;
			static SharedVertexBuffer			boxIndexBuffer;

			void InvalidateGraphTree(void);

			void UpdateGraphColor(void);
			void HandleGraphCollapseEvent(Widget *widget, const WidgetEventData *eventData);

			C4EDITORAPI virtual int32 GetHandleTable(Point3D *handle) const;

		protected:

			enum
			{
				kManipulatorBoxVertexCount		= 64,
				kManipulatorBoxTriangleCount	= 144
			};

			C4EDITORAPI static const ConstPoint3D manipulatorBoxPickEdge[12][2];
			C4EDITORAPI static const ConstPoint3D manipulatorBoxPosition[kManipulatorBoxVertexCount];
			C4EDITORAPI static const ConstVector3D manipulatorBoxOffset[kManipulatorBoxVertexCount];
			C4EDITORAPI static const Triangle manipulatorBoxTriangle[kManipulatorBoxTriangleCount];

			void SetManipulatorFlags(unsigned_int32 flags)
			{
				manipulatorFlags = flags;
			}

			void SetSelectionType(int32 type)
			{
				selectionType = type;
			}

			SharedVertexBuffer *RetainBoxVertexBuffer(void);
			SharedVertexBuffer *RetainBoxIndexBuffer(void);
			void ReleaseBoxVertexBuffer(void);
			void ReleaseBoxIndexBuffer(void);

			void AllocateConnectorStorage(void);
			void ReleaseConnectorStorage(void);

			void HiliteSubtree(void);
			void UnhiliteSubtree(void);

			C4EDITORAPI virtual bool CalculateNodeSphere(BoundingSphere *sphere) const;

			C4EDITORAPI static bool PickLineSegment(const Ray *ray, const Point3D& p1, const Point3D& p2, float r2, float *param);
			C4EDITORAPI bool RegionPickLineSegment(const VisibilityRegion *region, const Point3D& p1, const Point3D& p2) const;

		public:

			C4EDITORAPI static const TextureHeader		outlineTextureHeader;
			C4EDITORAPI static const unsigned_int8		outlineTextureImage[15];

			C4EDITORAPI EditorManipulator(Node *node, const char *iconName);
			C4EDITORAPI virtual ~EditorManipulator();

			static Manipulator *Create(Node *node, unsigned_int32 flags);

			unsigned_int32 GetManipulatorFlags(void) const
			{
				return (manipulatorFlags);
			}

			Editor *GetEditor(void) const
			{
				return (worldEditor);
			}

			EditorGizmo *GetGizmo(void) const
			{
				return (editorGizmo);
			}

			int32 GetSelectionType(void) const
			{
				return (selectionType);
			}

			const Vector4D& GetManipulatorScaleVector(void) const
			{
				return (manipulatorScaleVector);
			}

			const BoundingSphere *GetNodeSphere(void) const
			{
				return (nodeSpherePointer);
			}

			const BoundingSphere *GetTreeSphere(void) const
			{
				return (treeSpherePointer);
			}

			const float *GetOriginalSize(void) const
			{
				return (originalSize);
			}

			const Point3D& GetOriginalPosition(void) const
			{
				return (originalPosition);
			}

			const char *GetIconName(void) const
			{
				return (iconTextureAttribute.GetTextureName());
			}

			int32 GetHandleCount(void) const
			{
				return (handleCount);
			}

			const Point3D& GetHandlePosition(int32 index) const
			{
				return (handlePosition[index]);
			}

			int32 GetConnectorCount(void) const
			{
				return (connectorCount);
			}

			Node *GetConnectorTarget(int32 index) const
			{
				return (editorConnector[index].GetConnectorTarget());
			}

			int32 GetConnectorSelection(void) const
			{
				return (connectorSelection);
			}

			Node *GetConnectorSelectionTarget(void) const
			{
				return (GetConnectorTarget(connectorSelection));
			}

			bool SetConnectorSelectionTarget(Node *node, const ConnectorKey **key = nullptr)
			{
				return (SetConnectorTarget(connectorSelection, node, key));
			}

			float GetGraphWidth(void) const
			{
				return (graphWidth);
			}

			float GetGraphHeight(void) const
			{
				return (graphHeight);
			}

			const Point3D& GetGraphPosition(void) const
			{
				return (graphBackground.GetWidgetPosition());
			}

			C4EDITORAPI void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4EDITORAPI void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4EDITORAPI virtual const char *GetDefaultNodeName(void) const;

			C4EDITORAPI void Preprocess(void) override;
			C4EDITORAPI void Invalidate(void) override;
			C4EDITORAPI void InvalidateGraph(void);
			C4EDITORAPI virtual void InvalidateNode(void);

			C4EDITORAPI void EnableGizmo(void);
			C4EDITORAPI void DisableGizmo(void);

			C4EDITORAPI virtual void Update(void);
			C4EDITORAPI void UpdateGraph(void);

			C4EDITORAPI virtual void Show(void);
			C4EDITORAPI virtual void Hide(void);

			C4EDITORAPI bool PredecessorSelected(void) const;

			C4EDITORAPI virtual void Select(void);
			C4EDITORAPI virtual void Unselect(void);

			C4EDITORAPI virtual void Hilite(void);
			C4EDITORAPI virtual void Unhilite(void);

			C4EDITORAPI virtual void HandleDelete(bool undoable);
			C4EDITORAPI virtual void HandleUndelete(void);

			C4EDITORAPI virtual void HandleSizeUpdate(int32 count, const float *size);

			C4EDITORAPI virtual void HandleSettingsUpdate(void);
			C4EDITORAPI virtual void HandleConnectorUpdate(void);

			C4EDITORAPI virtual bool MaterialSettable(void) const;
			C4EDITORAPI virtual bool MaterialRemovable(void) const;
			C4EDITORAPI virtual int32 GetMaterialCount(void) const;
			C4EDITORAPI virtual MaterialObject *GetMaterial(int32 index = 0) const;
			C4EDITORAPI virtual void SetMaterial(MaterialObject *material);
			C4EDITORAPI virtual void ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial);
			C4EDITORAPI virtual void RemoveMaterial(void);
			C4EDITORAPI virtual void InvalidateShaderData(void);

			C4EDITORAPI virtual bool ReparentedSubnodesAllowed(void) const;

			C4EDITORAPI virtual Box3D CalculateNodeBoundingBox(void) const;
			C4EDITORAPI virtual Box3D CalculateWorldBoundingBox(void) const;

			C4EDITORAPI static void AdjustBoundingBox(Box3D *box);

			C4EDITORAPI virtual bool Pick(const Ray *ray, PickData *data) const;
			C4EDITORAPI virtual bool RegionPick(const VisibilityRegion *region) const;

			C4EDITORAPI virtual void BeginTransform(void);
			C4EDITORAPI virtual void EndTransform(void);

			C4EDITORAPI virtual void GetHandleData(int32 index, ManipulatorHandleData *handleData) const;
			C4EDITORAPI virtual void BeginResize(const ManipulatorResizeData *resizeData);
			C4EDITORAPI virtual void EndResize(const ManipulatorResizeData *resizeData);
			C4EDITORAPI virtual bool Resize(const ManipulatorResizeData *resizeData);

			C4EDITORAPI void UpdateConnectors(void);
			C4EDITORAPI void SelectConnector(int32 index, bool toggle = false);
			C4EDITORAPI void UnselectConnector(void);

			C4EDITORAPI bool SetConnectorTarget(int32 index, Node *target, const ConnectorKey **key = nullptr);
			bool PickConnector(const ManipulatorViewportData *viewportData, const Ray *ray, PickData *pickData) const;

			Box2D GetGraphBox(void) const;

			C4EDITORAPI void ExpandSubgraph(void);
			C4EDITORAPI void CollapseSubgraph(void);

			Node *PickGraphNode(const ManipulatorViewportData *viewportData, const Ray *ray, Widget **widget = nullptr);
			void SelectGraphNodes(float left, float right, float top, float bottom, bool temp = false);

			C4EDITORAPI virtual void Render(const ManipulatorRenderData *renderData);
			void RenderGraph(const ManipulatorViewportData *viewportData, List<Renderable> *renderList);

			C4EDITORAPI static void Install(Editor *editor, Node *root, bool recursive = true);
	};


	class GroupManipulator : public EditorManipulator
	{
		public:

			GroupManipulator(Node *node);
			~GroupManipulator();

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;
	};


	class SkyboxManipulator : public EditorManipulator
	{
		public:

			SkyboxManipulator(Skybox *skybox);
			~SkyboxManipulator();

			Skybox *GetTargetNode(void) const
			{
				return (static_cast<Skybox *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;

			void HandleDelete(bool undoable) override;
			void HandleUndelete(void) override;
			void HandleSettingsUpdate(void) override;

			bool MaterialSettable(void) const override;
			bool MaterialRemovable(void) const override;
			int32 GetMaterialCount(void) const override;
			MaterialObject *GetMaterial(int32 index) const override;
			void SetMaterial(MaterialObject *material) override;
			void ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial) override;
			void RemoveMaterial(void) override;
			void InvalidateShaderData(void) override;
	};


	class ImpostorManipulator : public EditorManipulator
	{
		public:

			ImpostorManipulator(Impostor *impostor);
			~ImpostorManipulator();

			Impostor *GetTargetNode(void) const
			{
				return (static_cast<Impostor *>(EditorManipulator::GetTargetNode()));
			}

			const char *GetDefaultNodeName(void) const override;

			void Preprocess(void) override;

			bool MaterialSettable(void) const override;
			int32 GetMaterialCount(void) const override;
			MaterialObject *GetMaterial(int32 index) const override;
			void SetMaterial(MaterialObject *material) override;
			void ReplaceMaterial(MaterialObject *oldMaterial, MaterialObject *newMaterial) override;
	};
}


#endif

// ZYUQURM
