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


#ifndef C4EditorOperations_h
#define C4EditorOperations_h


#include "C4EditorManipulators.h"
#include "C4Impostors.h"
#include "C4Particles.h"
#include "C4Terrain.h"
#include "C4Zones.h"
#include "C4Paint.h"
#include "C4Instances.h"


namespace C4
{
	typedef Type	OperationType;


	enum
	{
		kOperationNone						= 0,
		kOperationCreate					= 'CREA',
		kOperationMove						= 'MOVE',
		kOperationSize						= 'SIZE',
		kOperationResize					= 'RSIZ',
		kOperationPaste						= 'PAST',
		kOperationDelete					= 'DELT',
		kOperationGroup						= 'GRUP',
		kOperationConnect					= 'CONN',
		kOperationReparent					= 'RPAR',
		kOperationZoneVertex				= 'ZVTX',
		kOperationPortalVertex				= 'PVTX',
		kOperationMaterial					= 'MATL',
		kOperationGeometry					= 'GEOM',
		kOperationTexture					= 'TXTR',
		kOperationPaint						= 'PANT',
		kOperationPath						= 'PATH',
		kOperationTubeEffect				= 'TUBE',
		kOperationReplaceInstance			= 'RINS',
		kOperationReplaceModifiers			= 'RMOD',
		kOperationAssociatePaintSpace		= 'APNT',
		kOperationObjectData				= 'OBJD',
		kOperationNodeInfo					= 'INFO'
	};


	class Editor;


	class NodeReference : public ListElement<NodeReference>
	{
		private:

			Node	*reference;

		public:

			NodeReference(Node *node)
			{
				reference = node;
			}

			Node *GetNode(void) const
			{
				return (reference);
			}
	};


	class NodeTransformReference : public NodeReference
	{
		private:

			Transform4D		transform;

		public:

			NodeTransformReference(Node *node) : NodeReference(node)
			{
				transform = node->GetNodeTransform();
			}

			const Transform4D& GetTransform(void) const
			{
				return (transform);
			}
	};


	class Operation : public ListElement<Operation>
	{
		private:

			OperationType	operationType;
			bool			coupledFlag;

		protected:

			C4EDITORAPI Operation(OperationType type);

		public: 

			C4EDITORAPI virtual ~Operation();
 
			OperationType GetOperationType(void) const
			{ 
				return (operationType);
			}
 
			bool Coupled(void) const
			{ 
				return (coupledFlag); 
			}

			void SetCoupledFlag(bool flag)
			{ 
				coupledFlag = flag;
			}

			virtual void Restore(Editor *editor) = 0;
	};


	class CreateOperation : public Operation
	{
		private:

			List<NodeReference>		createdList;

		public:

			C4EDITORAPI CreateOperation();
			C4EDITORAPI CreateOperation(Node *node);
			C4EDITORAPI CreateOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~CreateOperation();

			const List<NodeReference> *GetCreatedList(void) const
			{
				return (&createdList);
			}

			void AddNode(Node *node)
			{
				createdList.Append(new NodeReference(node));
			}

			void Restore(Editor *editor) override;
	};


	class MoveOperation : public Operation
	{
		private:

			class PathReference : public NodeTransformReference
			{
				private:

					Path		path;

				public:

					PathReference(PathMarker *marker);

					const Path *GetPath(void) const
					{
						return (&path);
					}
			};

			List<NodeReference>		movedList;

		public:

			C4EDITORAPI MoveOperation(Node *node);
			C4EDITORAPI MoveOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~MoveOperation();

			const List<NodeReference> *GetNodeList(void) const
			{
				return (&movedList);
			}

			void Restore(Editor *editor) override;
	};


	class SizeOperation : public Operation
	{
		private:

			Node	*sizeNode;
			int32	sizeCount;
			float	objectSize[kMaxObjectSizeCount];

		public:

			C4EDITORAPI SizeOperation(Node *node);
			C4EDITORAPI ~SizeOperation();

			void Restore(Editor *editor) override;
	};


	class ResizeOperation : public Operation
	{
		private:

			class ResizedReference : public ListElement<ResizedReference>
			{
				private:

					Node			*reference;
					Transform4D		transform;
					float			objectSize[kMaxObjectSizeCount];

				public:

					ResizedReference(Node *node);

					Node *GetNode(void) const
					{
						return (reference);
					}

					const Transform4D& GetTransform(void) const
					{
						return (transform);
					}

					const float *GetObjectSize(void) const
					{
						return (objectSize);
					}
			};

			class ResizedGeometryReference : public ResizedReference
			{
				private:

					Mesh		*geometryMesh;

				public:

					ResizedGeometryReference(Geometry *geometry);
					~ResizedGeometryReference();

					const Mesh *GetGeometryLevel(int32 level) const
					{
						return (&geometryMesh[level]);
					}
			};

			class ResizedMeshReference : public ResizedGeometryReference
			{
				private:

					BoundingSphere		boundingSphere;
					Box3D				boundingBox;

				public:

					ResizedMeshReference(GenericGeometry *mesh);

					const BoundingSphere *GetBoundingSphere(void) const
					{
						return (&boundingSphere);
					}

					const Box3D& GetBoundingBox(void) const
					{
						return (boundingBox);
					}
			};

			class ResizedPolygonZoneReference : public ResizedReference
			{
				private:

					Point3D		zoneVertex[kMaxZoneVertexCount];

				public:

					ResizedPolygonZoneReference(PolygonZone *polygon);

					const Point3D *GetVertexArray(void) const
					{
						return (zoneVertex);
					}
			};

			class ResizedPortalReference : public ResizedReference
			{
				private:

					Point3D		portalVertex[kMaxPortalVertexCount];

				public:

					ResizedPortalReference(Portal *portal);

					const Point3D *GetVertexArray(void) const
					{
						return (portalVertex);
					}
			};

			class AffectedReference : public ListElement<AffectedReference>
			{
				private:

					Node		*reference;
					Point3D		position;

				public:

					AffectedReference(Node *node);

					Node *GetNode(void) const
					{
						return (reference);
					}

					const Point3D& GetPosition(void) const
					{
						return (position);
					}
			};

			List<ResizedReference>		resizedList;
			List<AffectedReference>		affectedList;

		public:

			C4EDITORAPI ResizeOperation(Node *node);
			C4EDITORAPI ResizeOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~ResizeOperation();

			void AddNode(Node *node);

			void Restore(Editor *editor) override;
	};


	class PasteOperation : public Operation
	{
		private:

			List<NodeReference>		pastedList;

		public:

			C4EDITORAPI PasteOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~PasteOperation();

			void Restore(Editor *editor) override;
	};


	class DeleteOperation : public Operation
	{
		private:

			struct OutgoingConnectorData
			{
				Connector		*outgoingConnector;
				Node			*targetNode;

				OutgoingConnectorData(Connector *connector)
				{
					outgoingConnector = connector;
					targetNode = connector->GetConnectorTarget();
				}
			};

			struct IncomingConnectorData
			{
				ConnectorKey	connectorKey;
				Node			*connectorNode;

				IncomingConnectorData(const Connector *connector)
				{
					connectorKey = connector->GetConnectorKey();
					connectorNode = connector->GetStartElement()->GetNode();
				}
			};

			class AffectedReference : public ListElement<AffectedReference>
			{
				private:

					Node								*reference;
					Node								*superNode;
					Transform4D							nodeTransform;

					bool								deletedFlag;

					Array<Link<Node> *, 4>				linkArray;
					Array<OutgoingConnectorData, 4>		outgoingConnectorArray;
					Array<IncomingConnectorData, 4>		incomingConnectorArray;

				public:

					AffectedReference(Node *node, bool deleted);
					~AffectedReference();

					Node *GetNode(void) const
					{
						return (reference);
					}

					Node *GetSuperNode(void) const
					{
						return (superNode);
					}

					const Transform4D& GetNodeTransform(void) const
					{
						return (nodeTransform);
					}

					bool GetDeletedFlag(void) const
					{
						return (deletedFlag);
					}

					const ImmutableArray<Link<Node> *>& GetLinkArray(void) const
					{
						return (linkArray);
					}

					const ImmutableArray<OutgoingConnectorData>& GetOutgoingConnectorArray(void) const
					{
						return (outgoingConnectorArray);
					}

					const ImmutableArray<IncomingConnectorData>& GetIncomingConnectorArray(void) const
					{
						return (incomingConnectorArray);
					}
			};

			List<AffectedReference>		affectedList;

		public:

			C4EDITORAPI DeleteOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~DeleteOperation();

			void Restore(Editor *editor) override;
	};


	class GroupOperation : public Operation
	{
		private:

			List<NodeReference>		groupList;

		public:

			C4EDITORAPI GroupOperation();
			C4EDITORAPI ~GroupOperation();

			void AddGroup(Node *group)
			{
				groupList.Append(new NodeReference(group));
			}

			void Restore(Editor *editor) override;
	};


	class ConnectOperation : public Operation
	{
		private:

			struct ConnectorData
			{
				int32		connectorIndex;
				Node		*targetNode;

				ConnectorData(int32 index, Node *node)
				{
					connectorIndex = index;
					targetNode = node;
				}
			};

			class ConnectedReference : public ListElement<ConnectedReference>
			{
				private:

					Node						*reference;
					Array<ConnectorData, 4>		connectorArray;

				public:

					ConnectedReference(Node *node);
					~ConnectedReference();

					Node *GetNode(void) const
					{
						return (reference);
					}

					const ImmutableArray<ConnectorData>& GetConnectorArray(void) const
					{
						return (connectorArray);
					}
			};

			List<ConnectedReference>	connectedList;
			List<NodeReference>			genericControllerList;

		public:

			C4EDITORAPI ConnectOperation(const List<EditorManipulator> *manipulatorList);
			C4EDITORAPI ConnectOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~ConnectOperation();

			void AddGenericControllerNode(Node *node)
			{
				genericControllerList.Append(new NodeReference(node));
			}

			void Restore(Editor *editor) override;
	};


	class ReparentOperation : public Operation
	{
		private:

			class MovedReference : public ListElement<MovedReference>
			{
				private:

					Node			*reference;
					Node			*superNode;
					Zone			*owningZone;
					Transform4D		transform;

				public:

					MovedReference(Node *node);

					Node *GetNode(void) const
					{
						return (reference);
					}

					Node *GetSuperNode(void) const
					{
						return (superNode);
					}

					Zone *GetOwningZone(void) const
					{
						return (owningZone);
					}

					const Transform4D& GetTransform(void) const
					{
						return (transform);
					}
			};

			List<MovedReference>	movedList;

		public:

			C4EDITORAPI ReparentOperation();
			C4EDITORAPI ReparentOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~ReparentOperation();

			void AddNode(Node *node);

			void Restore(Editor *editor) override;
	};


	class ZoneVertexOperation : public Operation
	{
		private:

			PolygonZone		*zoneNode;

			int32			zoneVertexCount;
			Point3D			zoneVertex[kMaxZoneVertexCount];

		public:

			C4EDITORAPI ZoneVertexOperation(PolygonZone *polygon);
			C4EDITORAPI ~ZoneVertexOperation();

			void Restore(Editor *editor) override;
	};


	class PortalVertexOperation : public Operation
	{
		private:

			Portal		*portalNode;

			int32		portalVertexCount;
			Point3D		portalVertex[kMaxPortalVertexCount];

		public:

			C4EDITORAPI PortalVertexOperation(Portal *portal);
			C4EDITORAPI ~PortalVertexOperation();

			void Restore(Editor *editor) override;
	};


	class MaterialOperation : public Operation
	{
		private:

			class GeometryReference : public ListElement<GeometryReference>
			{
				private:

					Geometry			*reference;

					int32				materialCount;
					char				*materialStorage;
					MaterialObject		**materialObject;
					unsigned_int32		*materialIndex;

				public:

					GeometryReference(Geometry *geometry);
					~GeometryReference();

					Geometry *GetGeometry(void) const
					{
						return (reference);
					}

					int32 GetMaterialCount(void) const
					{
						return (materialCount);
					}

					MaterialObject *GetMaterialObject(unsigned_int32 index) const
					{
						return (materialObject[index]);
					}

					unsigned_int32 GetMaterialIndex(unsigned_int32 surface) const
					{
						return (materialIndex[surface]);
					}
			};

			class SkyboxReference : public ListElement<SkyboxReference>
			{
				private:

					Skybox			*reference;
					MaterialObject	*materialObject;

				public:

					SkyboxReference(Skybox *skybox);
					~SkyboxReference();

					Skybox *GetSkybox(void) const
					{
						return (reference);
					}

					MaterialObject *GetMaterialObject(void) const
					{
						return (materialObject);
					}
			};

			class ImpostorReference : public ListElement<ImpostorReference>
			{
				private:

					Impostor		*reference;
					MaterialObject	*materialObject;

				public:

					ImpostorReference(Impostor *impostor);
					~ImpostorReference();

					Impostor *GetImpostor(void) const
					{
						return (reference);
					}

					MaterialObject *GetMaterialObject(void) const
					{
						return (materialObject);
					}
			};

			class ParticleSystemReference : public ListElement<ParticleSystemReference>
			{
				private:

					ParticleSystem	*reference;
					MaterialObject	*materialObject;

				public:

					ParticleSystemReference(ParticleSystem *particleSystem);
					~ParticleSystemReference();

					ParticleSystem *GetParticleSystem(void) const
					{
						return (reference);
					}

					MaterialObject *GetMaterialObject(void) const
					{
						return (materialObject);
					}
			};

			class ReplaceMaterialModifierReference : public ListElement<ReplaceMaterialModifierReference>
			{
				private:

					Instance					*instance;
					ReplaceMaterialModifier		*reference;
					MaterialObject				*materialObject;

				public:

					ReplaceMaterialModifierReference(Instance *node, ReplaceMaterialModifier *replaceMaterialModifier);
					~ReplaceMaterialModifierReference();

					Instance *GetInstance(void) const
					{
						return (instance);
					}

					ReplaceMaterialModifier *GetReplaceMaterialModifier(void) const
					{
						return (reference);
					}

					MaterialObject *GetMaterialObject(void) const
					{
						return (materialObject);
					}
			};

			List<GeometryReference>						geometryList;
			List<SkyboxReference>						skyboxList;
			List<ImpostorReference>						impostorList;
			List<ParticleSystemReference>				particleSystemList;
			List<ReplaceMaterialModifierReference>		replaceMaterialModifierList;

		public:

			C4EDITORAPI MaterialOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~MaterialOperation();

			void Restore(Editor *editor) override;
	};


	class GeometryOperation : public Operation
	{
		private:

			class GeometryReference : public ListElement<GeometryReference>
			{
				private:

					Geometry			*reference;
					Transform4D			transform;
					unsigned_int32		primitiveFlags;

					int32				geometryLevelCount;
					int32				collisionLevel;
					Mesh				*geometryMesh;

				public:

					GeometryReference(Geometry *geometry);
					~GeometryReference();

					Geometry *GetGeometry(void) const
					{
						return (reference);
					}

					const Transform4D& GetTransform(void) const
					{
						return (transform);
					}

					unsigned_int32 GetPrimitiveFlags(void) const
					{
						return (primitiveFlags);
					}

					int32 GetGeometryLevelCount(void) const
					{
						return (geometryLevelCount);
					}

					int32 GetCollisionLevel(void) const
					{
						return (collisionLevel);
					}

					const Mesh *GetGeometryLevel(int32 level) const
					{
						return (&geometryMesh[level]);
					}
			};

			class TerrainReference : public GeometryReference
			{
				private:

					TerrainBorderRenderData		borderRenderData;

				public:

					TerrainReference(TerrainGeometry *terrain);

					const TerrainBorderRenderData *GetBorderRenderData(void) const
					{
						return (&borderRenderData);
					}
			};

			class MovedReference : public ListElement<MovedReference>
			{
				private:

					Node			*reference;
					Transform4D		transform;

				public:

					MovedReference(Node *node);

					Node *GetNode(void) const
					{
						return (reference);
					}

					const Transform4D& GetTransform(void) const
					{
						return (transform);
					}
			};

			List<GeometryReference>		geometryList;
			List<MovedReference>		movedList;

			void AddGeometry(Geometry *geometry);

		public:

			C4EDITORAPI GeometryOperation(Geometry *geometry);
			C4EDITORAPI GeometryOperation(const List<NodeReference> *referenceList, bool (*filter)(const Geometry *) = nullptr);
			C4EDITORAPI ~GeometryOperation();

			void Restore(Editor *editor) override;
	};


	class TextureOperation : public Operation
	{
		private:

			class GeometryReference : public ListElement<GeometryReference>
			{
				private:

					Geometry			*reference;

					char				*textureStorage;
					Point2D				*texcoordArray;
					TextureAlignData	*textureAlignData;

				public:

					GeometryReference(Geometry *geometry);
					~GeometryReference();

					Geometry *GetGeometry(void) const
					{
						return (reference);
					}

					const Point2D *GetTexcoordArray(void) const
					{
						return (texcoordArray);
					}

					const TextureAlignData *GetTextureAlignData(void) const
					{
						return (textureAlignData);
					}
			};

			List<GeometryReference>		geometryList;

		public:

			C4EDITORAPI TextureOperation(Geometry *geometry);
			C4EDITORAPI TextureOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~TextureOperation();

			void Restore(Editor *editor) override;
	};


	class PaintOperation : public Operation
	{
		private:

			const PaintSpaceObject		*paintSpaceObject;

			Rect						paintBounds;
			const void					*undoImage;

		public:

			C4EDITORAPI PaintOperation(const PaintSpaceObject *object, const Painter *painter);
			C4EDITORAPI ~PaintOperation();

			void Restore(Editor *editor) override;
	};


	class PathOperation : public Operation
	{
		private:

			class PathReference : public ListElement<PathReference>
			{
				private:

					PathMarker		*reference;
					Path			path;

				public:

					PathReference(PathMarker *marker);
					~PathReference();

					PathMarker *GetPathMarker(void) const
					{
						return (reference);
					}

					const Path *GetPath(void) const
					{
						return (&path);
					}
			};

			List<PathReference>		pathList;

		public:

			C4EDITORAPI PathOperation(PathMarker *marker);
			C4EDITORAPI PathOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~PathOperation();

			void Restore(Editor *editor) override;
	};


	class TubeEffectOperation : public Operation
	{
		private:

			class TubeReference : public ListElement<TubeReference>
			{
				private:

					TubeEffect		*reference;
					Path			path;

				public:

					TubeReference(TubeEffect *tube);
					~TubeReference();

					TubeEffect *GetTubeEffect(void) const
					{
						return (reference);
					}

					const Path *GetPath(void) const
					{
						return (&path);
					}
			};

			List<TubeReference>		tubeList;

		public:

			C4EDITORAPI TubeEffectOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~TubeEffectOperation();

			void Restore(Editor *editor) override;
	};


	class ReplaceInstanceOperation : public Operation
	{
		private:

			class InstanceReference : public ListElement<InstanceReference>
			{
				private:

					Instance			*reference;
					ResourceName		worldName;

				public:

					InstanceReference(Instance *instance);
					~InstanceReference();

					Instance *GetInstance(void) const
					{
						return (reference);
					}

					const ResourceName& GetWorldName(void) const
					{
						return (worldName);
					}
			};

			List<InstanceReference>		instanceList;

		public:

			C4EDITORAPI ReplaceInstanceOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~ReplaceInstanceOperation();

			void Restore(Editor *editor) override;
	};


	class ReplaceModifiersOperation : public Operation
	{
		private:

			class InstanceReference : public ListElement<InstanceReference>
			{
				private:

					Instance			*reference;
					List<Modifier>		modifierList;

				public:

					InstanceReference(Instance *instance);
					~InstanceReference();

					Instance *GetInstance(void) const
					{
						return (reference);
					}

					Modifier *GetFirstModifier(void) const
					{
						return (modifierList.First());
					}
			};

			List<InstanceReference>		instanceList;

		public:

			C4EDITORAPI ReplaceModifiersOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~ReplaceModifiersOperation();

			void Restore(Editor *editor) override;
	};


	class AssociatePaintSpaceOperation : public Operation
	{
		private:

			class PaintReference : public ListElement<PaintReference>
			{
				private:

					Node			*reference;
					PaintSpace		*paintSpace;
					bool			connectorFlag;

				public:

					PaintReference(Node *node);
					~PaintReference();

					Node *GetNode(void) const
					{
						return (reference);
					}

					PaintSpace *GetPaintSpace(void) const
					{
						return (paintSpace);
					}

					bool GetConnectorFlag(void) const
					{
						return (connectorFlag);
					}
			};

			List<PaintReference>		paintList;

		public:

			C4EDITORAPI AssociatePaintSpaceOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~AssociatePaintSpaceOperation();

			void Restore(Editor *editor) override;
	};


	class ObjectDataOperation : public Operation
	{
		private:

			class ObjectDataReference : public ListElement<ObjectDataReference>
			{
				private:

					Node		*reference;
					Package		objectPackage;

				public:

					ObjectDataReference(Node *node);
					~ObjectDataReference();

					Node *GetNode(void) const
					{
						return (reference);
					}

					const Package *GetObjectPackage(void) const
					{
						return (&objectPackage);
					}
			};

			List<ObjectDataReference>		objectDataList;

		public:

			C4EDITORAPI ObjectDataOperation(const List<NodeReference> *referenceList, bool (*filter)(const Node *) = nullptr);
			C4EDITORAPI ~ObjectDataOperation();

			void Restore(Editor *editor) override;
	};


	class NodeInfoOperation : public Operation
	{
		private:

			struct ConnectorData
			{
				ConnectorKey	connectorKey;
				Node			*targetNode;

				ConnectorData(Connector *connector)
				{
					connectorKey = connector->GetConnectorKey();
					targetNode = connector->GetConnectorTarget();
				}
			};

			class NodeInfoReference : public ListElement<NodeInfoReference>
			{
				private:

					Node					*reference;
					List<NodeReference>		propertyObjectList;

					Package					nodePackage;
					Package					objectPackage;
					Package					propertyPackage;

					Array<ConnectorData>	connectorArray;

				public:

					NodeInfoReference(Node *node);
					~NodeInfoReference();

					Node *GetNode(void) const
					{
						return (reference);
					}

					const NodeReference *GetFirstPropertyObjectNode(void) const
					{
						return (propertyObjectList.First());
					}

					const Package *GetNodePackage(void) const
					{
						return (&nodePackage);
					}

					const Package *GetObjectPackage(void) const
					{
						return (&objectPackage);
					}

					const Package *GetPropertyPackage(void) const
					{
						return (&propertyPackage);
					}

					const Array<ConnectorData>& GetConnectorArray(void) const
					{
						return (connectorArray);
					}
			};

			List<NodeInfoReference>		nodeList;

		public:

			C4EDITORAPI NodeInfoOperation(const List<NodeReference> *referenceList);
			C4EDITORAPI ~NodeInfoOperation();

			void Restore(Editor *editor) override;
	};
}


#endif

// ZYUQURM
