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


#ifndef C4OpenGexImporter_h
#define C4OpenGexImporter_h


#include "C4EditorPlugins.h"
#include "C4Geometries.h"
#include "C4Lights.h"


extern "C"
{
	C4MODULEEXPORT C4::Plugin *CreatePlugin(void);
}


namespace C4
{
	enum
	{
		kStructureMetric						= 'mtrc',
		kStructureName							= 'name',
		kStructureObjectRef						= 'obrf',
		kStructureMaterialRef					= 'mtrf',
		kStructureMatrix						= 'mtrx',
		kStructureTransform						= 'xfrm',
		kStructureTranslation					= 'xslt',
		kStructureRotation						= 'rota',
		kStructureScale							= 'scal',
		kStructureMorphWeight					= 'mwgt',
		kStructureNode							= 'node',
		kStructureBoneNode						= 'bnnd',
		kStructureGeometryNode					= 'gmnd',
		kStructureLightNode						= 'ltnd',
		kStructureCameraNode					= 'cmnd',
		kStructureVertexArray					= 'vert',
		kStructureIndexArray					= 'indx',
		kStructureBoneRefArray					= 'bref',
		kStructureBoneCountArray				= 'bcnt',
		kStructureBoneIndexArray				= 'bidx',
		kStructureBoneWeightArray				= 'bwgt',
		kStructureSkeleton						= 'skel',
		kStructureSkin							= 'skin',
		kStructureMorph							= 'mrph',
		kStructureMesh							= 'mesh',
		kStructureObject						= 'objc',
		kStructureGeometryObject				= 'gmob',
		kStructureLightObject					= 'ltob',
		kStructureCameraObject					= 'cmob',
		kStructureAttrib						= 'attr',
		kStructureParam							= 'parm',
		kStructureColor							= 'colr',
		kStructureTexture						= 'txtr',
		kStructureAtten							= 'attn',
		kStructureMaterial						= 'matl',
		kStructureKey							= 'key ',
		kStructureCurve							= 'curv',
		kStructureTime							= 'time',
		kStructureValue							= 'valu',
		kStructureTrack							= 'trac',
		kStructureAnimation						= 'anim',
		kStructureClip							= 'clip',
		kStructureExtension						= 'extn'
	};


	enum
	{
		kDataOpenGexInvalidUpDirection			= 'ivud',
		kDataOpenGexInvalidTranslationKind		= 'ivtk',
		kDataOpenGexInvalidRotationKind			= 'ivrk',
		kDataOpenGexInvalidScaleKind			= 'ivsk',
		kDataOpenGexDuplicateLod				= 'dlod',
		kDataOpenGexMissingLodSkin				= 'mlsk',
		kDataOpenGexMissingLodMorph				= 'mlmp',
		kDataOpenGexDuplicateMorph				= 'dmph',
		kDataOpenGexUndefinedLightType			= 'ivlt',
		kDataOpenGexUndefinedCurve				= 'udcv',
		kDataOpenGexUndefinedAtten				= 'udan',
		kDataOpenGexDuplicateVertexArray		= 'dpva',
		kDataOpenGexPositionArrayRequired		= 'parq',
		kDataOpenGexVertexCountUnsupported		= 'vcus',
		kDataOpenGexIndexValueUnsupported		= 'ivus',
		kDataOpenGexIndexArrayRequired			= 'iarq',
		kDataOpenGexVertexCountMismatch			= 'vcmm',
		kDataOpenGexBoneCountMismatch			= 'bcmm',
		kDataOpenGexBoneWeightCountMismatch		= 'bwcm',
		kDataOpenGexInvalidBoneRef				= 'ivbr',
		kDataOpenGexInvalidObjectRef			= 'ivor',
		kDataOpenGexInvalidMaterialRef			= 'ivmr',
		kDataOpenGexMaterialIndexUnsupported	= 'mius',
		kDataOpenGexDuplicateMaterialRef		= 'dprf',
		kDataOpenGexMissingMaterialRef			= 'msrf',
		kDataOpenGexTargetRefNotLocal			= 'trnl',
		kDataOpenGexInvalidTargetStruct			= 'ivts',
		kDataOpenGexInvalidKeyKind				= 'ivkk',
		kDataOpenGexInvalidCurveType			= 'ivct',
		kDataOpenGexKeyCountMismatch			= 'kycm',
		kDataOpenGexEmptyKeyStructure			= 'emky'
	};

 
	class MaterialStructure;
	class ObjectStructure;
	class GeometryObjectStructure; 
	class LightObjectStructure;
	class CameraObjectStructure; 
	class OpenGexDataDescription;

 
	class OpenGexResource : public Resource<OpenGexResource>
	{ 
		friend class Resource<OpenGexResource>; 

		private:

			static ResourceDescriptor	descriptor; 

			~OpenGexResource();

		public:

			OpenGexResource(const char *name, ResourceCatalog *catalog);

			const char *GetText(void) const
			{
				return (static_cast<const char *>(GetData()));
			}
	};


	class OpenGexStructure : public Structure
	{
		protected:

			OpenGexStructure(StructureType type);

		public:

			~OpenGexStructure();

			Structure *GetFirstCoreSubnode(void) const;
			Structure *GetLastCoreSubnode(void) const;

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
	};


	class MetricStructure : public OpenGexStructure
	{
		private:

			String<>	metricKey;

		public:

			MetricStructure();
			~MetricStructure();

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class NameStructure : public OpenGexStructure
	{
		private:

			const char		*name;

		public:

			NameStructure();
			~NameStructure();

			const char *GetName(void) const
			{
				return (name);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class ObjectRefStructure : public OpenGexStructure
	{
		private:

			Structure		*targetStructure;

		public:

			ObjectRefStructure();
			~ObjectRefStructure();

			Structure *GetTargetStructure(void) const
			{
				return (targetStructure);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class MaterialRefStructure : public OpenGexStructure
	{
		private:

			unsigned_int32				materialIndex;
			const MaterialStructure		*targetStructure;

		public:

			MaterialRefStructure();
			~MaterialRefStructure();

			unsigned_int32 GetMaterialIndex(void) const
			{
				return (materialIndex);
			}

			const MaterialStructure *GetTargetStructure(void) const
			{
				return (targetStructure);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class AnimatableStructure : public OpenGexStructure
	{
		protected:

			AnimatableStructure(StructureType type);
			~AnimatableStructure();

		public:

			virtual void UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data) = 0;
	};


	class MatrixStructure : public AnimatableStructure
	{
		private:

			bool			objectFlag;

		protected:

			Transform4D		matrixValue;

			MatrixStructure(StructureType type);

		public:

			~MatrixStructure();

			bool GetObjectFlag(void) const
			{
				return (objectFlag);
			}

			const Transform4D& GetMatrix(void) const
			{
				return (matrixValue);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
	};


	class TransformStructure final : public MatrixStructure
	{
		private:

			int32					transformCount;
			const Transform4D		*transformArray;

		public:

			TransformStructure();
			~TransformStructure();

			int32 GetTransformCount(void) const
			{
				return (transformCount);
			}

			const Transform4D& GetTransform(int32 index = 0) const
			{
				return (transformArray[index]);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data) override;
	};


	class TranslationStructure final : public MatrixStructure
	{
		private:

			String<>		translationKind;

		public:

			TranslationStructure();
			~TranslationStructure();

			const String<>& GetTranslationKind(void) const
			{
				return (translationKind);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data) override;
	};


	class RotationStructure final : public MatrixStructure
	{
		private:

			String<>		rotationKind;

		public:

			RotationStructure();
			~RotationStructure();

			const String<>& GetRotationKind(void) const
			{
				return (rotationKind);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data) override;
	};


	class ScaleStructure final : public MatrixStructure
	{
		private:

			String<>		scaleKind;

		public:

			ScaleStructure();
			~ScaleStructure();

			const String<>& GetScaleKind(void) const
			{
				return (scaleKind);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data) override;
	};


	class MorphWeightStructure : public AnimatableStructure, public ListElement<MorphWeightStructure>
	{
		private:

			unsigned_int32		morphIndex;
			float				morphWeight;

		public:

			MorphWeightStructure();
			~MorphWeightStructure();

			using ListElement<MorphWeightStructure>::Previous;
			using ListElement<MorphWeightStructure>::Next;

			unsigned_int32 GetMorphIndex(void) const
			{
				return (morphIndex);
			}

			float GetMorphWeight(void) const
			{
				return (morphWeight);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data) override;
	};


	class NodeStructure : public OpenGexStructure
	{
		private:

			const char			*nodeName;
			unsigned_int32		nodeHash;

			Transform4D			nodeTransform;
			Transform4D			objectTransform;
			Transform4D			inverseObjectTransform;

			virtual const ObjectStructure *GetObjectStructure(void) const;

			void CalculateTransforms(const OpenGexDataDescription *dataDescription);

		protected:

			NodeStructure(StructureType type);

		public:

			NodeStructure();
			~NodeStructure();

			unsigned_int32 GetNodeHash(void) const
			{
				return (nodeHash);
			}

			const Transform4D& GetNodeTransform(void) const
			{
				return (nodeTransform);
			}

			const Transform4D& GetObjectTransform(void) const
			{
				return (objectTransform);
			}

			const Transform4D& GetInverseObjectTransform(void) const
			{
				return (inverseObjectTransform);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			virtual Node *CreateNode(const OpenGexDataDescription *dataDescription);
			virtual Transform4D GetTweakTransform(void) const;

			Transform4D CalculateFinalNodeTransform(void) const;
			void UpdateNodeTransforms(const OpenGexDataDescription *dataDescription);

			Node *BuildNodeTree(const OpenGexDataDescription *dataDescription);
	};


	class BoneNodeStructure : public NodeStructure
	{
		public:

			BoneNodeStructure();
			~BoneNodeStructure();

			Node *CreateNode(const OpenGexDataDescription *dataDescription) override;
	};


	class GeometryNodeStructure : public NodeStructure
	{
		private:

			bool		visibleFlag[2];
			bool		shadowFlag[2];
			bool		motionBlurFlag[2];

			GeometryObjectStructure					*geometryObjectStructure;
			Array<const MaterialStructure *, 4>		materialStructureArray;
			List<MorphWeightStructure>				morphWeightList;

			const ObjectStructure *GetObjectStructure(void) const override;

		public:

			GeometryNodeStructure();
			~GeometryNodeStructure();

			const GeometryObjectStructure *GetGeometryObjectStructure(void) const
			{
				return (geometryObjectStructure);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			Node *CreateNode(const OpenGexDataDescription *dataDescription) override;
			const MorphWeightStructure *FindMorphWeightStructure(unsigned_int32 index) const;
	};


	class LightNodeStructure : public NodeStructure
	{
		private:

			bool		shadowFlag[2];

			const LightObjectStructure		*lightObjectStructure;

			const ObjectStructure *GetObjectStructure(void) const override;

		public:

			LightNodeStructure();
			~LightNodeStructure();

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			Node *CreateNode(const OpenGexDataDescription *dataDescription) override;
			Transform4D GetTweakTransform(void) const override;
	};


	class CameraNodeStructure : public NodeStructure
	{
		private:

			const CameraObjectStructure		*cameraObjectStructure;

			const ObjectStructure *GetObjectStructure(void) const override;

		public:

			CameraNodeStructure();
			~CameraNodeStructure();

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			Node *CreateNode(const OpenGexDataDescription *dataDescription) override;
			Transform4D GetTweakTransform(void) const override;
	};


	class VertexArrayStructure : public OpenGexStructure
	{
		private:

			String<>			arrayAttrib;
			unsigned_int32		morphIndex;

			int32				arrayIndex;
			int32				vertexCount;
			int32				componentCount;

			char				*arrayStorage;
			float				*floatStorage;
			const void			*vertexArrayData;

			bool ValidateAttrib(Range<int32> *componentRange);

		public:

			VertexArrayStructure();
			~VertexArrayStructure();

			unsigned_int32 GetMorphIndex(void) const
			{
				return (morphIndex);
			}

			int32 GetArrayIndex(void) const
			{
				return (arrayIndex);
			}

			int32 GetVertexCount(void) const
			{
				return (vertexCount);
			}

			int32 GetComponentCount(void) const
			{
				return (componentCount);
			}

			const void *GetVertexArrayData(void) const
			{
				return (vertexArrayData);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class IndexArrayStructure : public OpenGexStructure, public ListElement<IndexArrayStructure>
	{
		private:

			unsigned_int32			materialIndex;
			unsigned_int64			restartIndex;
			String<>				frontFace;

			int32					triangleCount;
			const Triangle			*triangleArray;
			unsigned_int16			*arrayStorage;

		public:

			IndexArrayStructure();
			~IndexArrayStructure();

			using ListElement<IndexArrayStructure>::Previous;
			using ListElement<IndexArrayStructure>::Next;

			unsigned_int32 GetMaterialIndex(void) const
			{
				return (materialIndex);
			}

			unsigned_int64 GetRestartIndex(void) const
			{
				return (restartIndex);
			}

			const String<>& GetFrontFace(void) const
			{
				return (frontFace);
			}

			int32 GetTriangleCount(void) const
			{
				return (triangleCount);
			}

			const Triangle *GetTriangleArray(void) const
			{
				return (triangleArray);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class BoneRefArrayStructure : public OpenGexStructure
	{
		private:

			int32						boneCount;
			const BoneNodeStructure		**boneNodeArray;

		public:

			BoneRefArrayStructure();
			~BoneRefArrayStructure();

			int32 GetBoneCount(void) const
			{
				return (boneCount);
			}

			const BoneNodeStructure *const *GetBoneNodeArray(void) const
			{
				return (boneNodeArray);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class BoneCountArrayStructure : public OpenGexStructure
	{
		private:

			int32					vertexCount;
			const unsigned_int16	*boneCountArray;
			unsigned_int16			*arrayStorage;

		public:

			BoneCountArrayStructure();
			~BoneCountArrayStructure();

			int32 GetVertexCount(void) const
			{
				return (vertexCount);
			}

			const unsigned_int16 *GetBoneCountArray(void) const
			{
				return (boneCountArray);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class BoneIndexArrayStructure : public OpenGexStructure
	{
		private:

			int32					boneIndexCount;
			const unsigned_int16	*boneIndexArray;
			unsigned_int16			*arrayStorage;

		public:

			BoneIndexArrayStructure();
			~BoneIndexArrayStructure();

			int32 GetBoneIndexCount(void) const
			{
				return (boneIndexCount);
			}

			const unsigned_int16 *GetBoneIndexArray(void) const
			{
				return (boneIndexArray);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class BoneWeightArrayStructure : public OpenGexStructure
	{
		private:

			int32			boneWeightCount;
			const float		*boneWeightArray;

		public:

			BoneWeightArrayStructure();
			~BoneWeightArrayStructure();

			int32 GetBoneWeightCount(void) const
			{
				return (boneWeightCount);
			}

			const float *GetBoneWeightArray(void) const
			{
				return (boneWeightArray);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class SkeletonStructure : public OpenGexStructure
	{
		private:

			const BoneRefArrayStructure		*boneRefArrayStructure;
			const TransformStructure		*transformStructure;

		public:

			SkeletonStructure();
			~SkeletonStructure();

			const BoneRefArrayStructure *GetBoneRefArrayStructure(void) const
			{
				return (boneRefArrayStructure);
			}

			const TransformStructure *GetTransformStructure(void) const
			{
				return (transformStructure);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class SkinStructure : public OpenGexStructure
	{
		private:

			Transform4D							skinTransform;

			const SkeletonStructure				*skeletonStructure;
			const BoneCountArrayStructure		*boneCountArrayStructure;
			const BoneIndexArrayStructure		*boneIndexArrayStructure;
			const BoneWeightArrayStructure		*boneWeightArrayStructure;

			SkinData							skinData;
			char								*skinWeightStorage;

		public:

			SkinStructure();
			~SkinStructure();

			const Transform4D& GetSkinTransform(void) const
			{
				return (skinTransform);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			const SkinData *BuildSkinData(const OpenGexDataDescription *dataDescription);
	};


	class MorphStructure : public OpenGexStructure, public MapElement<MorphStructure>
	{
		private:

			unsigned_int32		morphIndex;

			bool				baseFlag;
			unsigned_int32		baseIndex;

			const char			*morphName;
			unsigned_int32		morphHash;

		public:

			typedef unsigned_int32 KeyType;

			MorphStructure();
			~MorphStructure();

			using MapElement<MorphStructure>::Previous;
			using MapElement<MorphStructure>::Next;

			KeyType GetKey(void) const
			{
				return (morphIndex);
			}

			unsigned_int32 GetMorphIndex(void) const
			{
				return (morphIndex);
			}

			bool GetBaseFlag(void) const
			{
				return (baseFlag);
			}

			unsigned_int32 GetBaseIndex(void) const
			{
				return (baseIndex);
			}

			unsigned_int32 GetMorphHash(void) const
			{
				return (morphHash);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class MeshStructure : public OpenGexStructure, public MapElement<MeshStructure>
	{
		public:

			class MorphArrayData : public MapElement<MorphArrayData>
			{
				friend class MeshStructure;

				private:

					unsigned_int32		morphIndex;

					const Point3D		*positionArrayData;
					const Vector3D		*normalArrayData;

				public:

					typedef unsigned_int32 KeyType;

					MorphArrayData(unsigned_int32 index);
					~MorphArrayData();

					unsigned_int32 GetKey(void) const
					{
						return (morphIndex);
					}
			};

		private:

			unsigned_int32					meshLevel;
			String<>						meshPrimitive;

			List<IndexArrayStructure>		indexArrayList;
			SkinStructure					*skinStructure;

			Map<MorphArrayData>				morphArrayMap;
			MorphData						morphData;

			List<GeometrySurface>			surfaceList;

		public:

			typedef unsigned_int32 KeyType;

			MeshStructure();
			~MeshStructure();

			using MapElement<MeshStructure>::Previous;
			using MapElement<MeshStructure>::Next;

			KeyType GetKey(void) const
			{
				return (meshLevel);
			}

			const List<IndexArrayStructure> *GetIndexArrayList(void) const
			{
				return (&indexArrayList);
			}

			SkinStructure *GetSkinStructure(void) const
			{
				return (skinStructure);
			}

			const Map<MorphArrayData> *GetMorphArrayMap(void) const
			{
				return (&morphArrayMap);
			}

			const List<GeometrySurface> *GetSurfaceList(void) const
			{
				return (&surfaceList);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			const MorphData *BuildMorphData(const Map<MorphStructure> *morphMap);
	};


	class ObjectStructure : public OpenGexStructure
	{
		protected:

			ObjectStructure(StructureType type);

		public:

			~ObjectStructure();
	};


	class GeometryObjectStructure : public ObjectStructure
	{
		private:

			bool					visibleFlag;
			bool					shadowFlag;
			bool					motionBlurFlag;

			GenericGeometryObject	*geometryObject;

			Map<MeshStructure>		meshMap;
			Map<MorphStructure>		morphMap;

		public:

			GeometryObjectStructure();
			~GeometryObjectStructure();

			const Map<MeshStructure> *GetMeshMap(void) const
			{
				return (&meshMap);
			}

			const Map<MorphStructure> *GetMorphMap(void) const
			{
				return (&morphMap);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			GenericGeometryObject *GetGeometryObject(const OpenGexDataDescription *dataDescription);
			const MorphStructure *FindMorphStructure(unsigned_int32 hash) const;
	};


	class LightObjectStructure : public ObjectStructure
	{
		private:

			String<>		typeString;
			bool			shadowFlag;

			LightType		lightType;
			ColorRGB		lightColor;
			float			lightRange;

			float			spotApex;
			const char		*textureName;

		public:

			LightObjectStructure();
			~LightObjectStructure();

			LightType GetLightType(void) const
			{
				return (lightType);
			}

			bool GetShadowFlag(void) const
			{
				return (shadowFlag);
			}

			const ColorRGB& GetLightColor(void) const
			{
				return (lightColor);
			}

			float GetLightRange(void) const
			{
				return (lightRange);
			}

			float GetSpotApex(void) const
			{
				return (spotApex);
			}

			const char *GetTextureName(void) const
			{
				return (textureName);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class CameraObjectStructure : public ObjectStructure
	{
		private:

			float		focalLength;
			float		nearDepth;
			float		farDepth;

		public:

			CameraObjectStructure();
			~CameraObjectStructure();

			float GetFocalLength(void) const
			{
				return (focalLength);
			}

			float GetNearDepth(void) const
			{
				return (nearDepth);
			}

			float GetFarDepth(void) const
			{
				return (farDepth);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class AttribStructure : public OpenGexStructure
	{
		private:

			String<>		attribString;

		protected:

			AttribStructure(StructureType type);

		public:

			~AttribStructure();

			const String<>& GetAttribString(void) const
			{
				return (attribString);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;

			virtual void UpdateMaterial(MaterialObject *materialObject) const = 0;
	};


	class ParamStructure : public AttribStructure
	{
		private:

			float		param;

		public:

			ParamStructure();
			~ParamStructure();

			float GetParam(void) const
			{
				return (param);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateMaterial(MaterialObject *materialObject) const override;
	};


	class ColorStructure : public AttribStructure
	{
		private:

			ColorRGBA	color;

		public:

			ColorStructure();
			~ColorStructure();

			const ColorRGBA& GetColor(void) const
			{
				return (color);
			}

			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateMaterial(MaterialObject *materialObject) const override;
	};


	class TextureStructure : public AttribStructure
	{
		private:

			String<>			textureName;
			unsigned_int32		texcoordIndex;
			Transform4D			texcoordTransform;

			void SetTextureName(const OpenGexDataDescription *dataDescription, const char *name);

		public:

			TextureStructure();
			~TextureStructure();

			const char *GetTextureName(void) const
			{
				return (textureName);
			}

			unsigned_int32 GetTexcoordIndex(void) const
			{
				return (texcoordIndex);
			}

			const Transform4D& GetTexcoordTransform(void) const
			{
				return (texcoordTransform);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateMaterial(MaterialObject *materialObject) const override;
	};


	class AttenStructure : public OpenGexStructure
	{
		private:

			String<>		attenKind;
			String<>		curveType;

			float			beginParam;
			float			endParam;

			float			scaleParam;
			float			offsetParam;

			float			constantParam;
			float			linearParam;
			float			quadraticParam;

			float			powerParam;

		public:

			AttenStructure();
			~AttenStructure();

			const String<>& GetAttenKind(void) const
			{
				return (attenKind);
			}

			const String<>& GetCurveType(void) const
			{
				return (curveType);
			}

			float GetBeginParam(void) const
			{
				return (beginParam);
			}

			float GetEndParam(void) const
			{
				return (endParam);
			}

			float GetScaleParam(void) const
			{
				return (scaleParam);
			}

			float GetOffsetParam(void) const
			{
				return (offsetParam);
			}

			float GetConstantParam(void) const
			{
				return (constantParam);
			}

			float GetLinearParam(void) const
			{
				return (linearParam);
			}

			float GetQuadraticParam(void) const
			{
				return (quadraticParam);
			}

			float GetPowerParam(void) const
			{
				return (powerParam);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class MaterialStructure : public OpenGexStructure
	{
		private:

			bool				twoSidedFlag;
			const char			*materialName;

			MaterialObject		*materialObject;

		public:

			MaterialStructure();
			~MaterialStructure();

			bool GetTwoSidedFlag(void) const
			{
				return (twoSidedFlag);
			}

			MaterialObject *GetMaterialObject(void) const
			{
				return (materialObject);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class KeyStructure : public OpenGexStructure
	{
		private:

			String<>		keyKind;

			bool			scalarFlag;

		public:

			KeyStructure();
			~KeyStructure();

			const String<>& GetKeyKind(void) const
			{
				return (keyKind);
			}

			bool GetScalarFlag(void) const
			{
				return (scalarFlag);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class CurveStructure : public OpenGexStructure
	{
		private:

			String<>				curveType;

			const KeyStructure		*keyValueStructure;
			const KeyStructure		*keyControlStructure[2];
			const KeyStructure		*keyTensionStructure;
			const KeyStructure		*keyContinuityStructure;
			const KeyStructure		*keyBiasStructure;

		protected:

			int32					keyDataElementCount;

			CurveStructure(StructureType type);

		public:

			~CurveStructure();

			const String<>& GetCurveType(void) const
			{
				return (curveType);
			}

			const KeyStructure *GetKeyValueStructure(void) const
			{
				return (keyValueStructure);
			}

			const KeyStructure *GetKeyControlStructure(int32 index) const
			{
				return (keyControlStructure[index]);
			}

			const KeyStructure *GetKeyTensionStructure(void) const
			{
				return (keyTensionStructure);
			}

			const KeyStructure *GetKeyContinuityStructure(void) const
			{
				return (keyContinuityStructure);
			}

			const KeyStructure *GetKeyBiasStructure(void) const
			{
				return (keyBiasStructure);
			}

			int32 GetKeyDataElementCount(void) const
			{
				return (keyDataElementCount);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class TimeStructure : public CurveStructure
	{
		public:

			TimeStructure();
			~TimeStructure();

			DataResult ProcessData(DataDescription *dataDescription) override;

			int32 CalculateInterpolationParameter(float time, float *param) const;
	};


	class ValueStructure : public CurveStructure
	{
		public:

			ValueStructure();
			~ValueStructure();

			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateAnimation(const OpenGexDataDescription *dataDescription, int32 index, float param, AnimatableStructure *target) const;
	};


	class TrackStructure : public OpenGexStructure, public ListElement<TrackStructure>
	{
		private:

			StructureRef			targetRef;

			AnimatableStructure		*targetStructure;
			const TimeStructure		*timeStructure;
			const ValueStructure	*valueStructure;

		public:

			TrackStructure();
			~TrackStructure();

			using ListElement<TrackStructure>::Previous;
			using ListElement<TrackStructure>::Next;

			const StructureRef& GetTargetRef(void) const
			{
				return (targetRef);
			}

			AnimatableStructure *GetTargetStructure(void) const
			{
				return (targetStructure);
			}

			const TimeStructure *GetTimeStructure(void) const
			{
				return (timeStructure);
			}

			const ValueStructure *GetValueStructure(void) const
			{
				return (valueStructure);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			void UpdateAnimation(const OpenGexDataDescription *dataDescription, float time) const;
	};


	class AnimationStructure : public OpenGexStructure, public ListElement<AnimationStructure>
	{
		private:

			unsigned_int32			clipIndex;

			bool					beginFlag;
			bool					endFlag;
			float					beginTime;
			float					endTime;

			List<TrackStructure>	trackList;

		public:

			AnimationStructure();
			~AnimationStructure();

			using ListElement<AnimationStructure>::Previous;
			using ListElement<AnimationStructure>::Next;

			unsigned_int32 GetClipIndex(void) const
			{
				return (clipIndex);
			}

			void AddTrack(TrackStructure *track)
			{
				trackList.Append(track);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;

			Range<float> GetAnimationTimeRange(void) const;
			void UpdateAnimation(const OpenGexDataDescription *dataDescription, float time) const;
	};


	class ClipStructure : public OpenGexStructure
	{
		private:

			unsigned_int32		clipIndex;

			float				frameRate;
			const char			*clipName;

		public:

			ClipStructure();
			~ClipStructure();

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
			DataResult ProcessData(DataDescription *dataDescription) override;
	};


	class ExtensionStructure : public OpenGexStructure
	{
		private:

			String<>	applicationString;
			String<>	typeString;

		public:

			ExtensionStructure();
			~ExtensionStructure();

			const String<>& GetApplicationString(void) const
			{
				return (applicationString);
			}

			const String<>& GetTypeString(void) const
			{
				return (typeString);
			}

			bool ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value) override;
			bool ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const override;
	};


	class OpenGexDataDescription : public DataDescription
	{
		private:

			Editor						*worldEditor;
			ResourcePath				baseImportPath;

			unsigned_int32				importFlags;
			float						importScale;

			float						distanceScale;
			float						angleScale;
			float						timeScale;
			int32						upDirection;

			List<AnimationStructure>	animationList;

			DataResult ProcessData(void) override;

		public:

			OpenGexDataDescription(Editor *editor, const char *file, unsigned_int32 flags, float scale);
			~OpenGexDataDescription();

			Editor *GetEditor(void) const
			{
				return (worldEditor);
			}

			const char *GetBaseImportPath(void) const
			{
				return (baseImportPath);
			}

			unsigned_int32 GetImportFlags(void) const
			{
				return (importFlags);
			}

			float GetDistanceScale(void) const
			{
				return (distanceScale);
			}

			void SetDistanceScale(float scale)
			{
				distanceScale = importScale * scale;
			}

			float GetAngleScale(void) const
			{
				return (angleScale);
			}

			void SetAngleScale(float scale)
			{
				angleScale = scale;
			}

			float GetTimeScale(void) const
			{
				return (timeScale);
			}

			void SetTimeScale(float scale)
			{
				timeScale = scale;
			}

			int32 GetUpDirection(void) const
			{
				return (upDirection);
			}

			void SetUpDirection(int32 direction)
			{
				upDirection = direction;
			}

			void AddAnimation(AnimationStructure *structure)
			{
				animationList.Append(structure);
			}

			const List<AnimationStructure> *GetAnimationList(void) const
			{
				return (&animationList);
			}

			Structure *CreateStructure(const String<>& identifier) const override;
			bool ValidateTopLevelStructure(const Structure *structure) const override;

			void AdjustTransform(Transform4D& transform) const;

			NodeStructure *FindNodeStructure(unsigned_int32 hash) const;

			Range<float> GetAnimationTimeRange(int32 clip) const;
			void UpdateAnimation(int32 clip, float time) const;
	};


	class OpenGexImporter : public SceneImportPlugin, public Singleton<OpenGexImporter>
	{
		private:

			StringTable			stringTable;

			bool GenerateAnimation(const OpenGexDataDescription *dataDescription, const char *resourceName, Model *model, const AnimationImportData *importData);

		public:

			OpenGexImporter();
			~OpenGexImporter();

			const StringTable *GetStringTable(void) const
			{
				return (&stringTable);
			}

			const char *GetPluginName(void) const override;
			const ResourceDescriptor *GetImportResourceDescriptor(SceneImportType type) const override;

			const char *GetOpenGexResultString(DataResult result) const;

			static Node *RemoveDeadNodes(Node *node);

			void ImportGeometry(Editor *editor, const char *importName, const GeometryImportData *importData) override;
			bool ImportAnimation(Window *window, const char *importName, const char *resourceName, Model *model, const AnimationImportData *importData) override;
	};


	extern OpenGexImporter *TheOpenGexImporter;
}


#endif

// ZYUQURM
