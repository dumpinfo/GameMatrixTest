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


#include "C4OpenGexImporter.h"
#include "C4TextureImporter.h"
#include "C4EditorSupport.h"
#include "C4ModelViewer.h"


using namespace C4;


/*
	struct $metric (id = "Metric", top_level = true)
	{
		property (id = "key")
		{
			type (string}
		}

		data
		{
			type {float, string}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}
	}

	struct $name (id = "Name")
	{
		data
		{
			type {string}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}
	}

	struct $object_ref (id = "ObjectRef")
	{
		data
		{
			type {ref}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}
	}

	struct $material_ref (id = "MaterialRef")
	{
		property (id = "index")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		data
		{
			type {ref}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}
	}

	struct $transform (id = "Transform")
	{
		property (id = "object")
		{
			type {bool}
			default {bool {false}}
		}

		data
		{
			type {float}
			instances {int32 {1, 1}}
			array_size {int32 {16, 16}}
		}
	}

	struct $translation (id = "Translation")
	{
		property (id = "object")
		{
			type {bool}
			default {bool {false}}
		}

		property (id = "kind")
		{
			type {string}
		}

		data
		{
			type {float}
			instances {int32 {1, 1}}
			elements {int32 {1}}
			array_size {int32 {0, 3}}
		}
	}

	struct $rotation (id = "Rotation")
	{ 
		property (id = "object")
		{
			type {bool} 
			default {bool {false}}
		} 

		property (id = "kind")
		{ 
			type {string}
		} 
 
		data
		{
			type {float}
			instances {int32 {1, 1}} 
			elements {int32 {1}}
			array_size {int32 {0, 4}}
		}
	}

	struct $scale (id = "Scale")
	{
		property (id = "object")
		{
			type {bool}
			default {bool {false}}
		}

		property (id = "kind")
		{
			type {string}
		}

		data
		{
			type {float}
			instances {int32 {1, 1}}
			elements {int32 {1}}
			array_size {int32 {0, 3}}
		}
	}

	struct $morph_weight (id = "MorphWeight")
	{
		property (id = "index")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		data
		{
			type {float}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}
	}

	struct $node (id = "Node", top_level = true)
	{
		sub
		{
			ref {$name}
			instances {int32 {0, 1}}
		}

		sub
		{
			ref {$transform, $translation, $rotation, $scale}
		}

		sub
		{
			ref {$animation}
		}

		sub
		{
			ref {$node, $bone_node, $geometry_node, $light_node, $camera_node}
		}
	}

	struct $bone_node (id = "BoneNode")
	{
		base {ref {$node}}
	}

	struct $geometry_node (id = "GeometryNode")
	{
		base {ref {$node}}

		property (id = "visible")
		{
			type {bool}
			default {bool {true}}
		}

		property (id = "shadow")
		{
			type {bool}
			default {bool {true}}
		}

		property (id = "motion_blur")
		{
			type {bool}
			default {bool {true}}
		}

		sub
		{
			ref ($object_ref}
			instances {int32 {1, 1}}
		}

		sub
		{
			ref ($material_ref}
		}

		sub
		{
			ref ($morph_weight}
		}
	}

	struct $light_node (id = "LightNode")
	{
		base {ref {$node}}

		sub
		{
			ref ($object_ref}
			instances {int32 {1, 1}}
		}
	}

	struct $camera_node (id = "CameraNode")
	{
		base {ref {$node}}

		sub
		{
			ref ($object_ref}
			instances {int32 {1, 1}}
		}
	}

	struct $vertex_array (id = "VertexArray")
	{
		property (id = "attrib")
		{
			type {string}
		}

		property (id = "morph")
		{
			type {unsigned_int32}
			default {unsigned_int32 {0}}
		}

		data
		{
			type {half, float, double}
			instances {int32 {1, 1}}
			array_size {int32 {1, 4}}
		}
	}

	struct $index_array (id = "IndexArray")
	{
		property (id = "material")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		data
		{
			type {unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64}
			instances {int32 {1, 1}}
			array_size {int32 {1, 3}}
		}
	}

	struct $bone_ref_array (id = "BoneRefArray")
	{
		data
		{
			type {ref}
			instances {int32 {1, 1}}
		}
	}

	struct $bone_count_array (id = "BoneCountArray")
	{
		data
		{
			type {unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64}
			instances {int32 {1, 1}}
		}
	}

	struct $bone_index_array (id = "BoneIndexArray")
	{
		data
		{
			type {unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64}
			instances {int32 {1, 1}}
		}
	}

	struct $bone_weight_array (id = "BoneWeightArray")
	{
		data
		{
			type {float, double}
			instances {int32 {1, 1}}
		}
	}

	struct $skeleton (id = "Skeleton")
	{
		sub
		{
			ref {$bone_ref_array}
			instances {int32 {1, 1}}
		}

		sub
		{
			ref {$transform}
			instances {int32 {1, 1}}
		}
	}

	struct $skin (id = "Skin")
	{
		sub
		{
			ref {$transform}
			instances {int32 {0, 1}}
		}

		sub
		{
			ref {$skeleton}
			instances {int32 {1, 1}}
		}

		sub
		{
			ref {$bone_count_array, $bone_index_array, $bone_weight_array}
			instances {int32 {1, 1}}
		}
	}

	struct $morph (id = "Morph")
	{
		property (id = "index")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		property (id = "base")
		{
			type {unsigned_int32}
		}

		sub
		{
			ref {$name}
		}
	}

	struct $mesh (id = "Mesh")
	{
		property (id = "lod")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		property (id = "primitive")
		{
			type {string}
			default {string {"triangles"}}
		}

		sub
		{
			ref {$vertex_array, $index_array}
		}

		sub
		{
			ref {$skin}
			instances {int32 {0, 1}}
		}
	}

	struct $geometry_object (id = "GeometryObject", top_level = true)
	{
		sub
		{
			ref {$mesh}
			instances {int32 {1}}
		}

		sub
		{
			ref {$morph}
		}
	}

	struct $light_object (id = "LightObject", top_level = true)
	{
		sub
		{
			ref {$param, $atten}
		}

		sub
		{
			ref {$color, $texture}
			instances {int32 {0, 1}}
		}
	}

	struct $camera_object (id = "CameraObject", top_level = true)
	{
		sub
		{
			ref {$param}
		}
	}

	struct $param (id = "Param")
	{
		property (id = "attrib")
		{
			type {string}
		}

		data
		{
			type {float}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}
	}

	struct $color (id = "Color")
	{
		property (id = "attrib")
		{
			type {string}
		}

		data
		{
			type {float}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
			array_size {int32 {3, 4}}
		}
	}

	struct $texture (id = "Texture")
	{
		property (id = "attrib")
		{
			type {string}
		}

		property (id = "texcoord")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		data
		{
			type {string}
			instances {int32 {1, 1}}
			elements {int32 {1, 1}}
		}

		sub
		{
			ref {$transform}
			instances {int32 {0, 1}}
		}
	}

	struct $atten (id = "Atten")
	{
		property (id = "kind")
		{
			type {string}
			default {string {"distance"}}
		}

		property (id = "curve")
		{
			type {string}
			default {string {"linear"}}
		}

		sub
		{
			ref {$param}
			instances {int32 {1, 2}}
		}
	}

	struct $material (id = "Material", top_level = true)
	{
		property (id = "two_sided")
		{
			type {bool}
			default {bool {false}}
		}

		sub
		{
			ref {$name}
			instances {int32 {0, 1}}
		}

		sub
		{
			ref {$param, $color, $texture}
		}
	}

	struct $key (id = "Key")
	{
		property (id = "kind")
		{
			type {string}
			default {string {"value"}}
		}

		data
		{
			type {float}
			instances {int32 {1, 1}}
			array_size {int32 {0, 7}}
		}
	}

	struct $time (id = "Time")
	{
		property (id = "curve")
		{
			type {string}
			default {string {"linear"}}
		}

		sub
		{
			ref {$key}
			instances {int32 {1}}
		}
	}

	struct $value (id = "Value")
	{
		property (id = "curve")
		{
			type {string}
			default {string {"linear"}}
		}

		sub
		{
			ref {$key}
			instances {int32 {1}}
		}
	}

	struct $track (id = "Track")
	{
		property (id = "target")
		{
			type {ref}
		}

		sub
		{
			ref {$time}
			instances {int32 {1, 1}}
		}

		sub
		{
			ref {$value}
			instances {int32 {1, 1}}
		}
	}

	struct $animation (id = "Animation")
	{
		property (id = "clip")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		sub
		{
			ref {$track}
			instances {int32 {1}}
		}
	}

	struct $clip (id = "Clip", top_level = true)
	{
		property (id = "index")
		{
			type {unsigned_int32}
			default {int32 {0}}
		}

		sub
		{
			ref {$name, $param}
			instances {int32 {0, 1}}
		}
	}

	struct &extension (id = "Extension", top_level = true)
	{
		property (id = "applic")
		{
			type (string}
		}

		property (id = "type")
		{
			type (string}
		}

		data
		{
			type {bool, int8, int16, int32, int64, unsigned_int8, unsigned_int16, unsigned_int32, unsigned_int64, half, float, double, string, ref, type}
		}

		sub
		{
			ref {$extension}
		}
	}
*/


namespace
{
	enum
	{
		kMaxAnimationBucketCount = 32
	};

	struct AnimatedTransformNode
	{
		const Node				*modelNode;
		NodeStructure			*nodeStructure;

		AnimatedTransformNode(const Node *node, NodeStructure *structure)
		{
			modelNode = node;
			nodeStructure = structure;
		}
	};

	struct AnimatedTransformHash
	{
		unsigned_int32			nodeHash;
		int32					transformIndex;

		AnimatedTransformHash(unsigned_int32 hash, int32 index)
		{
			nodeHash = hash;
			transformIndex = index;
		}
	};

	struct AnimatedMorphWeightNode
	{
		const Geometry			*modelNode;
		GeometryNodeStructure	*nodeStructure;

		AnimatedMorphWeightNode(const Geometry *node, GeometryNodeStructure *structure)
		{
			modelNode = node;
			nodeStructure = structure;
		}
	};

	struct AnimatedMorphWeightHash
	{
		unsigned_int32			nodeHash;
		int32					weightCount;
		int32					dataOffset;

		AnimatedMorphWeightHash(unsigned_int32 hash, int32 count, int32 offset)
		{
			nodeHash = hash;
			weightCount = count;
			dataOffset = offset;
		}
	};
}


OpenGexImporter *C4::TheOpenGexImporter = nullptr;


ResourceDescriptor OpenGexResource::descriptor("ogex", kResourceTerminatorByte);


C4::Plugin *CreatePlugin(void)
{
	return (new OpenGexImporter);
}


OpenGexResource::OpenGexResource(const char *name, ResourceCatalog *catalog) : Resource<OpenGexResource>(name, catalog)
{
}

OpenGexResource::~OpenGexResource()
{
}


OpenGexStructure::OpenGexStructure(StructureType type) : Structure(type)
{
}

OpenGexStructure::~OpenGexStructure()
{
}

Structure *OpenGexStructure::GetFirstCoreSubnode(void) const
{
	Structure *structure = GetFirstSubnode();
	while ((structure) && (structure->GetStructureType() == kStructureExtension))
	{
		structure = structure->Next();
	}

	return (structure);
}

Structure *OpenGexStructure::GetLastCoreSubnode(void) const
{
	Structure *structure = GetLastSubnode();
	while ((structure) && (structure->GetStructureType() == kStructureExtension))
	{
		structure = structure->Previous();
	}

	return (structure);
}

bool OpenGexStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return (structure->GetStructureType() == kStructureExtension);
}


MetricStructure::MetricStructure() : OpenGexStructure(kStructureMetric)
{
}

MetricStructure::~MetricStructure()
{
}

bool MetricStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "key")
	{
		*type = kDataString;
		*value = &metricKey;
		return (true);
	}

	return (false);
}

bool MetricStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kDataFloat) || (type == kDataString))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult MetricStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	if (metricKey == "distance")
	{
		if (structure->GetStructureType() != kDataFloat)
		{
			return (kDataInvalidDataFormat);
		}

		const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
		if (dataStructure->GetDataElementCount() != 1)
		{
			return (kDataInvalidDataFormat);
		}

		static_cast<OpenGexDataDescription *>(dataDescription)->SetDistanceScale(dataStructure->GetDataElement(0));
	}
	else if (metricKey == "angle")
	{
		if (structure->GetStructureType() != kDataFloat)
		{
			return (kDataInvalidDataFormat);
		}

		const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
		if (dataStructure->GetDataElementCount() != 1)
		{
			return (kDataInvalidDataFormat);
		}

		static_cast<OpenGexDataDescription *>(dataDescription)->SetAngleScale(dataStructure->GetDataElement(0));
	}
	else if (metricKey == "time")
	{
		if (structure->GetStructureType() != kDataFloat)
		{
			return (kDataInvalidDataFormat);
		}

		const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
		if (dataStructure->GetDataElementCount() != 1)
		{
			return (kDataInvalidDataFormat);
		}

		static_cast<OpenGexDataDescription *>(dataDescription)->SetTimeScale(dataStructure->GetDataElement(0));
	}
	else if (metricKey == "up")
	{
		int32	direction;

		if (structure->GetStructureType() != kDataString)
		{
			return (kDataInvalidDataFormat);
		}

		const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(structure);
		if (dataStructure->GetDataElementCount() != 1)
		{
			return (kDataInvalidDataFormat);
		}

		const String<>& string = dataStructure->GetDataElement(0);
		if (string == "z")
		{
			direction = 2;
		}
		else if (string == "y")
		{
			direction = 1;
		}
		else
		{
			return (kDataOpenGexInvalidUpDirection);
		}

		static_cast<OpenGexDataDescription *>(dataDescription)->SetUpDirection(direction);
	}

	return (kDataOkay);
}


NameStructure::NameStructure() : OpenGexStructure(kStructureName)
{
}

NameStructure::~NameStructure()
{
}

bool NameStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataString)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult NameStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(structure);
	if (dataStructure->GetDataElementCount() != 1)
	{
		return (kDataInvalidDataFormat);
	}

	name = dataStructure->GetDataElement(0);
	return (kDataOkay);
}


ObjectRefStructure::ObjectRefStructure() : OpenGexStructure(kStructureObjectRef)
{
	targetStructure = nullptr;
}

ObjectRefStructure::~ObjectRefStructure()
{
}

bool ObjectRefStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataRef)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult ObjectRefStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<RefDataType> *dataStructure = static_cast<const DataStructure<RefDataType> *>(structure);
	if (dataStructure->GetDataElementCount() != 0)
	{
		Structure *objectStructure = dataDescription->FindStructure(dataStructure->GetDataElement(0));
		if (objectStructure)
		{
			targetStructure = objectStructure;
			return (kDataOkay);
		}
	}

	return (kDataBrokenRef);
}


MaterialRefStructure::MaterialRefStructure() : OpenGexStructure(kStructureMaterialRef)
{
	materialIndex = 0;
	targetStructure = nullptr;
}

MaterialRefStructure::~MaterialRefStructure()
{
}

bool MaterialRefStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "index")
	{
		*type = kDataUnsignedInt32;
		*value = &materialIndex;
		return (true);
	}

	return (false);
}

bool MaterialRefStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataRef)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult MaterialRefStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<RefDataType> *dataStructure = static_cast<const DataStructure<RefDataType> *>(structure);
	if (dataStructure->GetDataElementCount() != 0)
	{
		const Structure *materialStructure = dataDescription->FindStructure(dataStructure->GetDataElement(0));
		if (materialStructure)
		{
			if (materialStructure->GetStructureType() != kStructureMaterial)
			{
				return (kDataOpenGexInvalidMaterialRef);
			}

			targetStructure = static_cast<const MaterialStructure *>(materialStructure);
			return (kDataOkay);
		}
	}

	return (kDataBrokenRef);
}


AnimatableStructure::AnimatableStructure(StructureType type) : OpenGexStructure(type)
{
}

AnimatableStructure::~AnimatableStructure()
{
}


MatrixStructure::MatrixStructure(StructureType type) : AnimatableStructure(type)
{
	SetBaseStructureType(kStructureMatrix);

	objectFlag = false;
	matrixValue.SetIdentity();
}

MatrixStructure::~MatrixStructure()
{
}

bool MatrixStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "object")
	{
		*type = kDataBool;
		*value = &objectFlag;
		return (true);
	}

	return (false);
}


TransformStructure::TransformStructure() : MatrixStructure(kStructureTransform)
{
}

TransformStructure::~TransformStructure()
{
}

bool TransformStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);
		return (primitiveStructure->GetArraySize() == 16);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult TransformStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);

	transformCount = dataStructure->GetDataElementCount() / 16;
	if (transformCount == 0)
	{
		return (kDataInvalidDataFormat);
	}

	transformArray = reinterpret_cast<const Transform4D *>(&dataStructure->GetDataElement(0));
	matrixValue = transformArray[0];

	return (kDataOkay);
}

void TransformStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data)
{
	matrixValue = *reinterpret_cast<const Transform4D *>(data);
}


TranslationStructure::TranslationStructure() :
		MatrixStructure(kStructureTranslation),
		translationKind("xyz")
{
}

TranslationStructure::~TranslationStructure()
{
}

bool TranslationStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "kind")
	{
		*type = kDataString;
		*value = &translationKind;
		return (true);
	}

	return (false);
}

bool TranslationStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);
		unsigned_int32 arraySize = primitiveStructure->GetArraySize();
		return ((arraySize == 0) || (arraySize == 3));
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult TranslationStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	unsigned_int32 arraySize = dataStructure->GetArraySize();

	if ((translationKind == "x") || (translationKind == "y") || (translationKind == "z"))
	{
		if ((arraySize != 0) || (dataStructure->GetDataElementCount() != 1))
		{
			return (kDataInvalidDataFormat);
		}
	}
	else if (translationKind == "xyz")
	{
		if ((arraySize != 3) || (dataStructure->GetDataElementCount() != 3))
		{
			return (kDataInvalidDataFormat);
		}
	}
	else
	{
		return (kDataOpenGexInvalidTranslationKind);
	}

	TranslationStructure::UpdateAnimation(static_cast<const OpenGexDataDescription *>(dataDescription), &dataStructure->GetDataElement(0));
	return (kDataOkay);
}

void TranslationStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data)
{
	if (translationKind == "x")
	{
		matrixValue.SetTranslation(data[0], 0.0F, 0.0F);
	}
	else if (translationKind == "y")
	{
		matrixValue.SetTranslation(0.0F, data[0], 0.0F);
	}
	else if (translationKind == "z")
	{
		matrixValue.SetTranslation(0.0F, 0.0F, data[0]);
	}
	else
	{
		matrixValue.SetTranslation(data[0], data[1], data[2]);
	}
}


RotationStructure::RotationStructure() :
		MatrixStructure(kStructureRotation),
		rotationKind("axis")
{
}

RotationStructure::~RotationStructure()
{
}

bool RotationStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "kind")
	{
		*type = kDataString;
		*value = &rotationKind;
		return (true);
	}

	return (false);
}

bool RotationStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);
		unsigned_int32 arraySize = primitiveStructure->GetArraySize();
		return ((arraySize == 0) || (arraySize == 4));
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult RotationStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	unsigned_int32 arraySize = dataStructure->GetArraySize();

	if ((rotationKind == "x") || (rotationKind == "y") || (rotationKind == "z"))
	{
		if ((arraySize != 0) || (dataStructure->GetDataElementCount() != 1))
		{
			return (kDataInvalidDataFormat);
		}
	}
	else if ((rotationKind == "axis") || (rotationKind == "quaternion"))
	{
		if ((arraySize != 4) || (dataStructure->GetDataElementCount() != 4))
		{
			return (kDataInvalidDataFormat);
		}
	}
	else
	{
		return (kDataOpenGexInvalidRotationKind);
	}

	RotationStructure::UpdateAnimation(static_cast<const OpenGexDataDescription *>(dataDescription), &dataStructure->GetDataElement(0));
	return (kDataOkay);
}

void RotationStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data)
{
	float scale = dataDescription->GetAngleScale();

	if (rotationKind == "x")
	{
		matrixValue.SetRotationAboutX(data[0] * scale);
	}
	else if (rotationKind == "y")
	{
		matrixValue.SetRotationAboutY(data[0] * scale);
	}
	else if (rotationKind == "z")
	{
		matrixValue.SetRotationAboutZ(data[0] * scale);
	}
	else if (rotationKind == "axis")
	{
		matrixValue.SetRotationAboutAxis(data[0] * scale, Antivector3D(data[1], data[2], data[3]).Normalize());
	}
	else
	{
		matrixValue.SetMatrix3D(Quaternion(data[0], data[1], data[2], data[3]).Normalize().GetRotationMatrix());
	}
}


ScaleStructure::ScaleStructure() :
		MatrixStructure(kStructureScale),
		scaleKind("xyz")
{
}

ScaleStructure::~ScaleStructure()
{
}

bool ScaleStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "kind")
	{
		*type = kDataString;
		*value = &scaleKind;
		return (true);
	}

	return (false);
}

bool ScaleStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);
		unsigned_int32 arraySize = primitiveStructure->GetArraySize();
		return ((arraySize == 0) || (arraySize == 3));
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult ScaleStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	unsigned_int32 arraySize = dataStructure->GetArraySize();

	if ((scaleKind == "x") || (scaleKind == "y") || (scaleKind == "z"))
	{
		if ((arraySize != 0) || (dataStructure->GetDataElementCount() != 1))
		{
			return (kDataInvalidDataFormat);
		}
	}
	else if (scaleKind == "xyz")
	{
		if ((arraySize != 3) || (dataStructure->GetDataElementCount() != 3))
		{
			return (kDataInvalidDataFormat);
		}
	}
	else
	{
		return (kDataOpenGexInvalidScaleKind);
	}

	ScaleStructure::UpdateAnimation(static_cast<const OpenGexDataDescription *>(dataDescription), &dataStructure->GetDataElement(0));
	return (kDataOkay);
}

void ScaleStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data)
{
	if (scaleKind == "x")
	{
		matrixValue.SetScale(data[0], 1.0F, 1.0F);
	}
	else if (scaleKind == "y")
	{
		matrixValue.SetScale(1.0F, data[0], 1.0F);
	}
	else if (scaleKind == "z")
	{
		matrixValue.SetScale(1.0F, 1.0F, data[0]);
	}
	else if (scaleKind == "xyz")
	{
		matrixValue.SetScale(data[0], data[1], data[2]);
	}
}


MorphWeightStructure::MorphWeightStructure() : AnimatableStructure(kStructureMorphWeight)
{
	morphIndex = 0;
	morphWeight = 0.0F;
}

MorphWeightStructure::~MorphWeightStructure()
{
}

bool MorphWeightStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "index")
	{
		*type = kDataUnsignedInt32;
		*value = &morphIndex;
		return (true);
	}

	return (false);
}

bool MorphWeightStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
		unsigned_int32 arraySize = dataStructure->GetArraySize();
		return (arraySize == 0);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult MorphWeightStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	if (dataStructure->GetDataElementCount() == 1)
	{
		morphWeight = dataStructure->GetDataElement(0);
	}
	else
	{
		return (kDataInvalidDataFormat);
	}

	return (kDataOkay);
}

void MorphWeightStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, const float *data)
{
	morphWeight = data[0];
}


NodeStructure::NodeStructure() : OpenGexStructure(kStructureNode)
{
	SetBaseStructureType(kStructureNode);
}

NodeStructure::NodeStructure(StructureType type) : OpenGexStructure(type)
{
	SetBaseStructureType(kStructureNode);
}

NodeStructure::~NodeStructure()
{
}

bool NodeStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetBaseStructureType();
	if ((type == kStructureNode) || (type == kStructureMatrix))
	{
		return (true);
	}

	type = structure->GetStructureType();
	if ((type == kStructureName) || (type == kStructureAnimation))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult NodeStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	const Structure *structure = GetFirstSubstructure(kStructureName);
	if (structure)
	{
		if (GetLastSubstructure(kStructureName) != structure)
		{
			return (kDataExtraneousSubstructure);
		}

		nodeName = static_cast<const NameStructure *>(structure)->GetName();
		nodeHash = Text::Hash(nodeName);
	}
	else
	{
		nodeName = nullptr;
		nodeHash = 0;
	}

	return (kDataOkay);
}

const ObjectStructure *NodeStructure::GetObjectStructure(void) const
{
	return (nullptr);
}

void NodeStructure::CalculateTransforms(const OpenGexDataDescription *dataDescription)
{
	nodeTransform.SetIdentity();
	objectTransform.SetIdentity();

	const ObjectStructure *objectStructure = GetObjectStructure();

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetBaseStructureType() == kStructureMatrix)
		{
			const MatrixStructure *matrixStructure = static_cast<const MatrixStructure *>(structure);
			if (!matrixStructure->GetObjectFlag())
			{
				nodeTransform = nodeTransform * matrixStructure->GetMatrix();
			}
			else if (objectStructure)
			{
				objectTransform = objectTransform * matrixStructure->GetMatrix();
			}
		}

		structure = structure->Next();
	}

	dataDescription->AdjustTransform(nodeTransform);
	dataDescription->AdjustTransform(objectTransform);

	objectTransform *= GetTweakTransform();
	inverseObjectTransform = Inverse(objectTransform);
}

Node *NodeStructure::CreateNode(const OpenGexDataDescription *dataDescription)
{
	return (new Node);
}

Transform4D NodeStructure::GetTweakTransform(void) const
{
	return (Identity4D);
}

Transform4D NodeStructure::CalculateFinalNodeTransform(void) const
{
	Transform4D transform = nodeTransform * objectTransform;

	const Structure *superStructure = GetSuperNode();
	if (superStructure->GetBaseStructureType() == kStructureNode)
	{
		transform = static_cast<const NodeStructure *>(superStructure)->GetInverseObjectTransform() * transform;
	}

	return (transform);
}

void NodeStructure::UpdateNodeTransforms(const OpenGexDataDescription *dataDescription)
{
	CalculateTransforms(dataDescription);

	Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetBaseStructureType() == kStructureNode)
		{
			static_cast<NodeStructure *>(structure)->UpdateNodeTransforms(dataDescription);
		}

		structure = structure->Next();
	}
}

Node *NodeStructure::BuildNodeTree(const OpenGexDataDescription *dataDescription)
{
	Node *root = CreateNode(dataDescription);

	if (nodeName)
	{
		root->SetNodeName(nodeName);
	}

	root->SetNodeTransform(CalculateFinalNodeTransform());

	Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetBaseStructureType() == kStructureNode)
		{
			Node *node = static_cast<NodeStructure *>(structure)->BuildNodeTree(dataDescription);
			root->AppendSubnode(node);
		}

		structure = structure->Next();
	}

	return (root);
}


BoneNodeStructure::BoneNodeStructure() : NodeStructure(kStructureBoneNode)
{
}

BoneNodeStructure::~BoneNodeStructure()
{
}

Node *BoneNodeStructure::CreateNode(const OpenGexDataDescription *dataDescription)
{
	return (new Bone);
}


GeometryNodeStructure::GeometryNodeStructure() : NodeStructure(kStructureGeometryNode)
{
	visibleFlag[0] = false;
	shadowFlag[0] = false;
	motionBlurFlag[0] = false;
}

GeometryNodeStructure::~GeometryNodeStructure()
{
	morphWeightList.RemoveAll();
}

bool GeometryNodeStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "visible")
	{
		*type = kDataBool;
		*value = &visibleFlag[1];
		visibleFlag[0] = true;
		return (true);
	}

	if (identifier == "shadow")
	{
		*type = kDataBool;
		*value = &shadowFlag[1];
		shadowFlag[0] = true;
		return (true);
	}

	if (identifier == "motion_blur")
	{
		*type = kDataBool;
		*value = &motionBlurFlag[1];
		motionBlurFlag[0] = true;
		return (true);
	}

	return (false);
}

bool GeometryNodeStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kStructureObjectRef) || (type == kStructureMaterialRef) || (type == kStructureMorphWeight))
	{
		return (true);
	}

	return (NodeStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult GeometryNodeStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = NodeStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	bool objectFlag = false;
	bool materialFlag[256] = {false};
	int32 maxMaterialIndex = -1;

	Structure *structure = GetFirstSubnode();
	while (structure)
	{
		StructureType type = structure->GetStructureType();
		if (type == kStructureObjectRef)
		{
			if (objectFlag)
			{
				return (kDataExtraneousSubstructure);
			}

			objectFlag = true;

			Structure *objectStructure = static_cast<ObjectRefStructure *>(structure)->GetTargetStructure();
			if (objectStructure->GetStructureType() != kStructureGeometryObject)
			{
				return (kDataOpenGexInvalidObjectRef);
			}

			geometryObjectStructure = static_cast<GeometryObjectStructure *>(objectStructure);
		}
		else if (type == kStructureMaterialRef)
		{
			const MaterialRefStructure *materialRefStructure = static_cast<MaterialRefStructure *>(structure);

			unsigned_int32 index = materialRefStructure->GetMaterialIndex();
			if (index > 255)
			{
				return (kDataOpenGexMaterialIndexUnsupported);
			}

			if (materialFlag[index])
			{
				return (kDataOpenGexDuplicateMaterialRef);
			}

			materialFlag[index] = true;
			maxMaterialIndex = Max(maxMaterialIndex, int32(index));
		}
		else if (type == kStructureMorphWeight)
		{
			morphWeightList.Append(static_cast<MorphWeightStructure *>(structure));
		}

		structure = structure->Next();
	}

	if (!objectFlag)
	{
		return (kDataMissingSubstructure);
	}

	if (maxMaterialIndex >= 0)
	{
		for (machine a = 0; a <= maxMaterialIndex; a++)
		{
			if (!materialFlag[a])
			{
				return (kDataOpenGexMissingMaterialRef);
			}
		}

		materialStructureArray.SetElementCount(maxMaterialIndex + 1);

		structure = GetFirstSubnode();
		while (structure)
		{
			if (structure->GetStructureType() == kStructureMaterialRef)
			{
				const MaterialRefStructure *materialRefStructure = static_cast<const MaterialRefStructure *>(structure);
				materialStructureArray[materialRefStructure->GetMaterialIndex()] = materialRefStructure->GetTargetStructure();
			}

			structure = structure->Next();
		}
	}

	return (kDataOkay);
}

const ObjectStructure *GeometryNodeStructure::GetObjectStructure(void) const
{
	return (geometryObjectStructure);
}

Node *GeometryNodeStructure::CreateNode(const OpenGexDataDescription *dataDescription)
{
	GenericGeometry *geometry = new GenericGeometry;
	GenericGeometryObject *geometryObject = geometryObjectStructure->GetGeometryObject(dataDescription);
	geometry->SetObject(geometryObject);

	unsigned_int32 geometryFlags = geometryObject->GetGeometryFlags();

	if (visibleFlag[0])
	{
		if (visibleFlag[1])
		{
			geometryFlags &= ~kGeometryInvisible;
		}
		else
		{
			geometryFlags |= kGeometryInvisible;
		}
	}

	if (shadowFlag[0])
	{
		if (shadowFlag[1])
		{
			geometryFlags |= kGeometryCastShadows;
		}
		else
		{
			geometryFlags &= ~kGeometryCastShadows;
		}
	}

	if (motionBlurFlag[0])
	{
		if (motionBlurFlag[1])
		{
			geometryFlags &= ~kGeometryMotionBlurInhibit;
		}
		else
		{
			geometryFlags |= kGeometryMotionBlurInhibit;
		}
	}

	geometryObject->SetGeometryFlags(geometryFlags);

	const Mesh *mesh = geometryObject->GetGeometryLevel(0);
	const SkinWeight *skinWeightData = mesh->GetSkinWeightData();

	int32 morphWeightCount = 0;
	const Map<MeshStructure::MorphArrayData> *morphArrayMap = geometryObjectStructure->GetMeshMap()->First()->GetMorphArrayMap();

	MorphWeightStructure *morphWeightStructure = morphWeightList.First();
	while (morphWeightStructure)
	{
		MorphWeightStructure *next = morphWeightStructure->Next();

		if (morphArrayMap->Find(morphWeightStructure->GetMorphIndex()))
		{
			morphWeightCount++;
		}
		else
		{
			morphWeightList.Remove(morphWeightStructure);
		}

		morphWeightStructure = next;
	}

	const MorphAttrib *morphTargetData = (morphWeightCount != 0) ? mesh->GetMorphTargetData() : nullptr;

	if ((skinWeightData) || (morphTargetData))
	{
		MorphController *controller = (skinWeightData) ? new SkinController : new MorphController;
		geometry->SetController(controller);

		if (morphTargetData)
		{
			controller->SetMorphWeightCount(morphWeightCount);

			const Map<MorphStructure> *morphStructureMap = geometryObjectStructure->GetMorphMap();

			morphWeightCount = 0;
			morphWeightStructure = morphWeightList.First();
			while (morphWeightStructure)
			{
				unsigned_int32 index = morphWeightStructure->GetMorphIndex();
				const MorphStructure *morphStructure = morphStructureMap->Find(index);
				unsigned_int32 hash = (morphStructure) ? morphStructure->GetMorphHash() : index;

				controller->SetMorphHash(morphWeightCount, hash);
				controller->SetMorphWeight(morphWeightCount, morphWeightStructure->GetMorphWeight());

				morphWeightCount++;
				morphWeightStructure = morphWeightStructure->Next();
			}
		}

		if (skinWeightData)
		{
			geometry->SetNodeFlags(kNodeAnimateInhibit);
		}

		geometryObject->SetGeometryFlags(geometryObject->GetGeometryFlags() | kGeometryDeformModelFlags);
		geometryObject->SetCollisionExclusionMask(kCollisionExcludeAll);
	}

	int32 materialStructureCount = materialStructureArray.GetElementCount();
	if (materialStructureCount != 0)
	{
		int32 materialCount = 1;

		const MeshStructure *meshStructure = geometryObjectStructure->GetMeshMap()->First();
		while (meshStructure)
		{
			const IndexArrayStructure *indexArrayStructure = meshStructure->GetIndexArrayList()->First();
			while (indexArrayStructure)
			{
				materialCount = Max(materialCount, indexArrayStructure->GetMaterialIndex() + 1);
				indexArrayStructure = indexArrayStructure->Next();
			}

			meshStructure = meshStructure->Next();
		}

		geometry->SetMaterialCount(materialCount);

		meshStructure = geometryObjectStructure->GetMeshMap()->First();
		while (meshStructure)
		{
			const IndexArrayStructure *indexArrayStructure = meshStructure->GetIndexArrayList()->First();
			while (indexArrayStructure)
			{
				int32 materialIndex = indexArrayStructure->GetMaterialIndex();
				geometry->SetMaterialObject(materialIndex, materialStructureArray[materialIndex % materialStructureCount]->GetMaterialObject());

				indexArrayStructure = indexArrayStructure->Next();
			}

			meshStructure = meshStructure->Next();
		}
	}

	geometry->OptimizeMaterials();
	return (geometry);
}

const MorphWeightStructure *GeometryNodeStructure::FindMorphWeightStructure(unsigned_int32 index) const
{
	const MorphWeightStructure *morphWeightStructure = morphWeightList.First();
	while (morphWeightStructure)
	{
		if (morphWeightStructure->GetMorphIndex() == index)
		{
			return (morphWeightStructure);
		}

		morphWeightStructure = morphWeightStructure->Next();
	}

	return (nullptr);
}


LightNodeStructure::LightNodeStructure() : NodeStructure(kStructureLightNode)
{
	shadowFlag[0] = false;
}

LightNodeStructure::~LightNodeStructure()
{
}

bool LightNodeStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "shadow")
	{
		*type = kDataBool;
		*value = &shadowFlag[1];
		shadowFlag[0] = true;
		return (true);
	}

	return (false);
}

bool LightNodeStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kStructureObjectRef)
	{
		return (true);
	}

	return (NodeStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult LightNodeStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = NodeStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	bool objectFlag = false;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kStructureObjectRef)
		{
			if (objectFlag)
			{
				return (kDataExtraneousSubstructure);
			}

			objectFlag = true;

			const Structure *objectStructure = static_cast<const ObjectRefStructure *>(structure)->GetTargetStructure();
			if (objectStructure->GetStructureType() != kStructureLightObject)
			{
				return (kDataOpenGexInvalidObjectRef);
			}

			lightObjectStructure = static_cast<const LightObjectStructure *>(objectStructure);
		}

		structure = structure->Next();
	}

	if (!objectFlag)
	{
		return (kDataMissingSubstructure);
	}

	return (kDataOkay);
}

const ObjectStructure *LightNodeStructure::GetObjectStructure(void) const
{
	return (lightObjectStructure);
}

Node *LightNodeStructure::CreateNode(const OpenGexDataDescription *dataDescription)
{
	Light	*light;

	LightType type = lightObjectStructure->GetLightType();
	const ColorRGB& color = lightObjectStructure->GetLightColor();

	if (type == kLightInfinite)
	{
		light = new InfiniteLight(color);
	}
	else if (type == kLightPoint)
	{
		light = new PointLight(color, lightObjectStructure->GetLightRange());
	}
	else
	{
		const char *name = lightObjectStructure->GetTextureName();
		if ((!name) || (name[0] == 0))
		{
			name = "C4/spot";
		}

		light = new SpotLight(color, lightObjectStructure->GetLightRange(), lightObjectStructure->GetSpotApex(), name);
	}

	LightObject *lightObject = light->GetObject();
	unsigned_int32 lightFlags = lightObject->GetLightFlags();

	if (!((shadowFlag[0]) ? shadowFlag[1] : lightObjectStructure->GetShadowFlag()))
	{
		lightFlags |= kLightShadowInhibit;
	}

	lightObject->SetLightFlags(lightFlags);

	return (light);
}

Transform4D LightNodeStructure::GetTweakTransform(void) const
{
	if (lightObjectStructure->GetLightType() == kLightSpot)
	{
		return (Transform4D(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F));
	}

	return (Identity4D);
}


CameraNodeStructure::CameraNodeStructure() : NodeStructure(kStructureCameraNode)
{
}

CameraNodeStructure::~CameraNodeStructure()
{
}

bool CameraNodeStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kStructureObjectRef)
	{
		return (true);
	}

	return (NodeStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult CameraNodeStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = NodeStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	bool objectFlag = false;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kStructureObjectRef)
		{
			if (objectFlag)
			{
				return (kDataExtraneousSubstructure);
			}

			objectFlag = true;

			const Structure *objectStructure = static_cast<const ObjectRefStructure *>(structure)->GetTargetStructure();
			if (objectStructure->GetStructureType() != kStructureCameraObject)
			{
				return (kDataOpenGexInvalidObjectRef);
			}

			cameraObjectStructure = static_cast<const CameraObjectStructure *>(objectStructure);
		}

		structure = structure->Next();
	}

	if (!objectFlag)
	{
		return (kDataMissingSubstructure);
	}

	return (kDataOkay);
}

const ObjectStructure *CameraNodeStructure::GetObjectStructure(void) const
{
	return (cameraObjectStructure);
}

Node *CameraNodeStructure::CreateNode(const OpenGexDataDescription *dataDescription)
{
	FrustumCamera *camera = new FrustumCamera(cameraObjectStructure->GetFocalLength(), 1.0F);
	FrustumCameraObject *object = camera->GetObject();

	float near = cameraObjectStructure->GetNearDepth();
	float far = cameraObjectStructure->GetFarDepth();

	if (far > near)
	{
		object->SetNearDepth(near);
		object->SetFarDepth(far);
	}

	return (camera);
}

Transform4D CameraNodeStructure::GetTweakTransform(void) const
{
	return (Transform4D(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F));
}


VertexArrayStructure::VertexArrayStructure() : OpenGexStructure(kStructureVertexArray)
{
	morphIndex = 0;

	arrayIndex = -1;
	arrayStorage = nullptr;
	floatStorage = nullptr;
}

VertexArrayStructure::~VertexArrayStructure()
{
	delete[] arrayStorage;
	delete[] floatStorage;
}

bool VertexArrayStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "attrib")
	{
		*type = kDataString;
		*value = &arrayAttrib;
		return (true);
	}

	if (identifier == "morph")
	{
		*type = kDataUnsignedInt32;
		*value = &morphIndex;
		return (true);
	}

	return (false);
}

bool VertexArrayStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kDataHalf) || (type == kDataFloat) || (type == kDataDouble))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

bool VertexArrayStructure::ValidateAttrib(Range<int32> *componentRange)
{
	enum
	{
		kArrayAttribCount = 6
	};

	static const char *const attribName[kArrayAttribCount] =
	{
		"position", "normal", "color", "tangent", "bitangent", "texcoord"
	};

	static const int8 attribIndex[kArrayAttribCount] =
	{
		kArrayPosition, kArrayNormal, kArrayColor, -1, -1, kArrayTexcoord
	};

	static const int8 minComponentCount[kArrayAttribCount] =
	{
		3, 3, 3, 3, 3, 2
	};

	static const int8 maxComponentCount[kArrayAttribCount] =
	{
		3, 3, 4, 3, 3, 2
	};

	const char *text = arrayAttrib;

	for (machine a = 0; a < kArrayAttribCount; a++)
	{
		const char *name = attribName[a];
		int32 length = Text::GetTextLength(name);

		if (Text::CompareText(arrayAttrib, name, length))
		{
			text += length;
			int32 c = text[0];
			if (c == 0)
			{
				arrayIndex = attribIndex[a];
				componentRange->Set(minComponentCount[a], maxComponentCount[a]);
				return (true);
			}

			if (c == '[')
			{
				unsigned_int64		value;

				text++;
				if (Data::ReadUnsignedLiteral(text, &length, &value) == kDataOkay)
				{
					text += length;
					if ((text[0] == ']') && (text[1] == 0))
					{
						int32 index = attribIndex[a];
						if (value != 0)
						{
							if ((index == kArrayTexcoord) && (value < 2))
							{
								index += (int32) value;
							}
							else
							{
								index = -1;
							}
						}

						arrayIndex = index;
						componentRange->Set(minComponentCount[a], maxComponentCount[a]);
						return (true);
					}
				}
			}

			break;
		}
	}

	return (false);
}

DataResult VertexArrayStructure::ProcessData(DataDescription *dataDescription)
{
	Range<int32>	componentRange;

	if (ValidateAttrib(&componentRange))
	{
		int32			elementCount;
		const float		*data;

		const Structure *structure = GetFirstCoreSubnode();
		if (!structure)
		{
			return (kDataMissingSubstructure);
		}

		if (GetLastCoreSubnode() != structure)
		{
			return (kDataExtraneousSubstructure);
		}

		const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);

		int32 arraySize = primitiveStructure->GetArraySize();
		if ((arraySize < componentRange.min) || (arraySize > componentRange.max))
		{
			return (kDataInvalidDataFormat);
		}

		StructureType type = primitiveStructure->GetStructureType();
		if (type == kDataFloat)
		{
			const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
			elementCount = dataStructure->GetDataElementCount();
			data = &dataStructure->GetDataElement(0);
		}
		else if (type == kDataDouble)
		{
			const DataStructure<DoubleDataType> *dataStructure = static_cast<const DataStructure<DoubleDataType> *>(structure);
			elementCount = dataStructure->GetDataElementCount();

			float *floatElement = new float[elementCount];
			floatStorage = floatElement;
			data = floatElement;

			const double *doubleElement = &dataStructure->GetDataElement(0);
			for (machine a = 0; a < elementCount; a++)
			{
				floatElement[a] = (float) doubleElement[a];
			}
		}
		else
		{
			const DataStructure<HalfDataType> *dataStructure = static_cast<const DataStructure<HalfDataType> *>(structure);
			elementCount = dataStructure->GetDataElementCount();

			float *floatElement = new float[elementCount];
			floatStorage = floatElement;
			data = floatElement;

			const Half *halfElement = &dataStructure->GetDataElement(0);
			for (machine a = 0; a < elementCount; a++)
			{
				floatElement[a] = halfElement[a];
			}
		}

		vertexCount = elementCount / arraySize;
		componentCount = arraySize;
		vertexArrayData = data;

		if (arrayIndex == kArrayPosition)
		{
			const OpenGexDataDescription *openGexDataDescription = static_cast<OpenGexDataDescription *>(dataDescription);
			float scale = openGexDataDescription->GetDistanceScale();
			int32 up = openGexDataDescription->GetUpDirection();

			if ((scale != 1.0F) || (up != 2))
			{
				Transform4D		transform;

				if (up == 2)
				{
					transform.Set(scale, 0.0F, 0.0F, 0.0F,
									0.0F, scale, 0.0F, 0.0F,
									0.0F, 0.0F, scale, 0.0F);
				}
				else
				{
					transform.Set(scale, 0.0F, 0.0F, 0.0F,
									0.0F, 0.0F, -scale, 0.0F,
									0.0F, scale, 0.0F, 0.0F);
				}

				arrayStorage = new char[vertexCount * sizeof(Point3D)];
				vertexArrayData = arrayStorage;

				const Point3D *inputPosition = reinterpret_cast<const Point3D *>(data);
				Point3D *outputPosition = reinterpret_cast<Point3D *>(arrayStorage);

				for (machine a = 0; a < vertexCount; a++)
				{
					outputPosition[a] = transform * inputPosition[a];
				}
			}
		}
		else if (arrayIndex == kArrayNormal)
		{
			const OpenGexDataDescription *openGexDataDescription = static_cast<OpenGexDataDescription *>(dataDescription);
			if (openGexDataDescription->GetUpDirection() != 2)
			{
				arrayStorage = new char[vertexCount * sizeof(Vector3D)];
				vertexArrayData = arrayStorage;

				const Vector3D *inputNormal = reinterpret_cast<const Vector3D *>(data);
				Vector3D *outputNormal = reinterpret_cast<Vector3D *>(arrayStorage);

				for (machine a = 0; a < vertexCount; a++)
				{
					const Vector3D& normal = inputNormal[a];
					outputNormal[a].Set(normal.x, -normal.z, normal.y);
				}
			}
		}
		else if (arrayIndex == kArrayColor)
		{
			if (arraySize == 3)
			{
				componentCount = 4;
				arrayStorage = new char[vertexCount * sizeof(ColorRGBA)];
				vertexArrayData = arrayStorage;

				const ColorRGB *inputColor = reinterpret_cast<const ColorRGB *>(data);
				ColorRGBA *outputColor = reinterpret_cast<ColorRGBA *>(arrayStorage);

				for (machine a = 0; a < vertexCount; a++)
				{
					outputColor[a] = inputColor[a];
				}
			}
		}
	}

	return (kDataOkay);
}


IndexArrayStructure::IndexArrayStructure() : OpenGexStructure(kStructureIndexArray)
{
	materialIndex = 0;
	restartIndex = 0;
	frontFace = "ccw";

	arrayStorage = nullptr;
}

IndexArrayStructure::~IndexArrayStructure()
{
	delete[] arrayStorage;
}

bool IndexArrayStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "material")
	{
		*type = kDataUnsignedInt32;
		*value = &materialIndex;
		return (true);
	}

	if (identifier == "restart")
	{
		*type = kDataUnsignedInt64;
		*value = &restartIndex;
		return (true);
	}

	if (identifier == "front")
	{
		*type = kDataString;
		*value = &frontFace;
		return (true);
	}

	return (false);
}

bool IndexArrayStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kDataUnsignedInt8) || (type == kDataUnsignedInt16) || (type == kDataUnsignedInt32) || (type == kDataUnsignedInt64))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult IndexArrayStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);
	if (primitiveStructure->GetArraySize() != 3)
	{
		return (kDataInvalidDataFormat);
	}

	StructureType type = primitiveStructure->GetStructureType();
	if (type == kDataUnsignedInt16)
	{
		const DataStructure<UnsignedInt16DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt16DataType> *>(primitiveStructure);
		triangleCount = dataStructure->GetDataElementCount() / 3;
		triangleArray = reinterpret_cast<const Triangle *>(&dataStructure->GetDataElement(0));
	}
	else if (type == kDataUnsignedInt8)
	{
		const DataStructure<UnsignedInt8DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt8DataType> *>(primitiveStructure);
		int32 elementCount = dataStructure->GetDataElementCount();
		triangleCount = elementCount / 3;

		const unsigned_int8 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[elementCount];
		triangleArray = reinterpret_cast<const Triangle *>(arrayStorage);

		for (machine a = 0; a < elementCount; a++)
		{
			arrayStorage[a] = data[a];
		}
	}
	else if (type == kDataUnsignedInt32)
	{
		const DataStructure<UnsignedInt32DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt32DataType> *>(primitiveStructure);
		int32 elementCount = dataStructure->GetDataElementCount();
		triangleCount = elementCount / 3;

		const unsigned_int32 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[elementCount];
		triangleArray = reinterpret_cast<const Triangle *>(arrayStorage);

		for (machine a = 0; a < elementCount; a++)
		{
			unsigned_int32 index = data[a];
			if (index > 65535)
			{
				return (kDataOpenGexIndexValueUnsupported);
			}

			arrayStorage[a] = (unsigned_int16) index;
		}
	}
	else
	{
		const DataStructure<UnsignedInt64DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt64DataType> *>(primitiveStructure);
		int32 elementCount = dataStructure->GetDataElementCount();
		triangleCount = elementCount / 3;

		const unsigned_int64 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[elementCount];
		triangleArray = reinterpret_cast<const Triangle *>(arrayStorage);

		for (machine a = 0; a < elementCount; a++)
		{
			unsigned_int64 index = data[a];
			if (index > 65535)
			{
				return (kDataOpenGexIndexValueUnsupported);
			}

			arrayStorage[a] = (unsigned_int16) index;
		}
	}

	return (kDataOkay);
}


BoneRefArrayStructure::BoneRefArrayStructure() : OpenGexStructure(kStructureBoneRefArray)
{
	boneNodeArray = nullptr;
}

BoneRefArrayStructure::~BoneRefArrayStructure()
{
	delete[] boneNodeArray;
}

bool BoneRefArrayStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataRef)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult BoneRefArrayStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<RefDataType> *dataStructure = static_cast<const DataStructure<RefDataType> *>(structure);
	boneCount = dataStructure->GetDataElementCount();

	if (boneCount != 0)
	{
		boneNodeArray = new const BoneNodeStructure *[boneCount];

		for (machine a = 0; a < boneCount; a++)
		{
			const StructureRef& reference = dataStructure->GetDataElement(a);
			const Structure *boneStructure = dataDescription->FindStructure(reference);
			if (!boneStructure)
			{
				return (kDataBrokenRef);
			}

			if (boneStructure->GetStructureType() != kStructureBoneNode)
			{
				return (kDataOpenGexInvalidBoneRef);
			}

			boneNodeArray[a] = static_cast<const BoneNodeStructure *>(boneStructure);
		}
	}

	return (kDataOkay);
}


BoneCountArrayStructure::BoneCountArrayStructure() : OpenGexStructure(kStructureBoneCountArray)
{
	arrayStorage = nullptr;
}

BoneCountArrayStructure::~BoneCountArrayStructure()
{
	delete[] arrayStorage;
}

bool BoneCountArrayStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kDataUnsignedInt8) || (type == kDataUnsignedInt16) || (type == kDataUnsignedInt32) || (type == kDataUnsignedInt64))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult BoneCountArrayStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);
	if (primitiveStructure->GetArraySize() != 0)
	{
		return (kDataInvalidDataFormat);
	}

	StructureType type = primitiveStructure->GetStructureType();
	if (type == kDataUnsignedInt16)
	{
		const DataStructure<UnsignedInt16DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt16DataType> *>(primitiveStructure);
		vertexCount = dataStructure->GetDataElementCount();
		boneCountArray = &dataStructure->GetDataElement(0);
	}
	else if (type == kDataUnsignedInt8)
	{
		const DataStructure<UnsignedInt8DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt8DataType> *>(primitiveStructure);
		vertexCount = dataStructure->GetDataElementCount();

		const unsigned_int8 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[vertexCount];
		boneCountArray = arrayStorage;

		for (machine a = 0; a < vertexCount; a++)
		{
			arrayStorage[a] = data[a];
		}
	}
	else if (type == kDataUnsignedInt32)
	{
		const DataStructure<UnsignedInt32DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt32DataType> *>(primitiveStructure);
		vertexCount = dataStructure->GetDataElementCount();

		const unsigned_int32 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[vertexCount];
		boneCountArray = arrayStorage;

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int32 index = data[a];
			if (index > 65535)
			{
				return (kDataOpenGexIndexValueUnsupported);
			}

			arrayStorage[a] = (unsigned_int16) index;
		}
	}
	else
	{
		const DataStructure<UnsignedInt64DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt64DataType> *>(primitiveStructure);
		vertexCount = dataStructure->GetDataElementCount();

		const unsigned_int64 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[vertexCount];
		boneCountArray = arrayStorage;

		for (machine a = 0; a < vertexCount; a++)
		{
			unsigned_int64 index = data[a];
			if (index > 65535)
			{
				return (kDataOpenGexIndexValueUnsupported);
			}

			arrayStorage[a] = (unsigned_int16) index;
		}
	}

	return (kDataOkay);
}


BoneIndexArrayStructure::BoneIndexArrayStructure() : OpenGexStructure(kStructureBoneIndexArray)
{
	arrayStorage = nullptr;
}

BoneIndexArrayStructure::~BoneIndexArrayStructure()
{
	delete[] arrayStorage;
}

bool BoneIndexArrayStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kDataUnsignedInt8) || (type == kDataUnsignedInt16) || (type == kDataUnsignedInt32) || (type == kDataUnsignedInt64))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult BoneIndexArrayStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const PrimitiveStructure *primitiveStructure = static_cast<const PrimitiveStructure *>(structure);
	if (primitiveStructure->GetArraySize() != 0)
	{
		return (kDataInvalidDataFormat);
	}

	StructureType type = primitiveStructure->GetStructureType();
	if (type == kDataUnsignedInt16)
	{
		const DataStructure<UnsignedInt16DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt16DataType> *>(primitiveStructure);
		boneIndexCount = dataStructure->GetDataElementCount();
		boneIndexArray = &dataStructure->GetDataElement(0);
	}
	else if (type == kDataUnsignedInt8)
	{
		const DataStructure<UnsignedInt8DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt8DataType> *>(primitiveStructure);
		boneIndexCount = dataStructure->GetDataElementCount();

		const unsigned_int8 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[boneIndexCount];
		boneIndexArray = arrayStorage;

		for (machine a = 0; a < boneIndexCount; a++)
		{
			arrayStorage[a] = data[a];
		}
	}
	else if (type == kDataUnsignedInt32)
	{
		const DataStructure<UnsignedInt32DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt32DataType> *>(primitiveStructure);
		boneIndexCount = dataStructure->GetDataElementCount();

		const unsigned_int32 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[boneIndexCount];
		boneIndexArray = arrayStorage;

		for (machine a = 0; a < boneIndexCount; a++)
		{
			unsigned_int32 index = data[a];
			if (index > 65535)
			{
				return (kDataOpenGexIndexValueUnsupported);
			}

			arrayStorage[a] = (unsigned_int16) index;
		}
	}
	else
	{
		const DataStructure<UnsignedInt64DataType> *dataStructure = static_cast<const DataStructure<UnsignedInt64DataType> *>(primitiveStructure);
		boneIndexCount = dataStructure->GetDataElementCount();

		const unsigned_int64 *data = &dataStructure->GetDataElement(0);
		arrayStorage = new unsigned_int16[boneIndexCount];
		boneIndexArray = arrayStorage;

		for (machine a = 0; a < boneIndexCount; a++)
		{
			unsigned_int64 index = data[a];
			if (index > 65535)
			{
				return (kDataOpenGexIndexValueUnsupported);
			}

			arrayStorage[a] = (unsigned_int16) index;
		}
	}

	return (kDataOkay);
}


BoneWeightArrayStructure::BoneWeightArrayStructure() : OpenGexStructure(kStructureBoneWeightArray)
{
}

BoneWeightArrayStructure::~BoneWeightArrayStructure()
{
}

bool BoneWeightArrayStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult BoneWeightArrayStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	if (dataStructure->GetArraySize() != 0)
	{
		return (kDataInvalidDataFormat);
	}

	boneWeightCount = dataStructure->GetDataElementCount();
	boneWeightArray = &dataStructure->GetDataElement(0);
	return (kDataOkay);
}


SkeletonStructure::SkeletonStructure() : OpenGexStructure(kStructureSkeleton)
{
}

SkeletonStructure::~SkeletonStructure()
{
}

bool SkeletonStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kStructureBoneRefArray) || (type == kStructureTransform))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult SkeletonStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	const Structure *structure = GetFirstSubstructure(kStructureBoneRefArray);
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastSubstructure(kStructureBoneRefArray) != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	boneRefArrayStructure = static_cast<const BoneRefArrayStructure *>(structure);

	structure = GetFirstSubstructure(kStructureTransform);
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastSubstructure(kStructureTransform) != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	transformStructure = static_cast<const TransformStructure *>(structure);

	if (boneRefArrayStructure->GetBoneCount() != transformStructure->GetTransformCount())
	{
		return (kDataOpenGexBoneCountMismatch);
	}

	return (kDataOkay);
}


SkinStructure::SkinStructure() : OpenGexStructure(kStructureSkin)
{
	skinData.boneHashArray = nullptr;
	skinData.inverseBindTransformArray = nullptr;
	skinData.skinWeightTable = nullptr;

	skinWeightStorage = nullptr;
}

SkinStructure::~SkinStructure()
{
	delete[] skinWeightStorage;

	delete[] skinData.skinWeightTable;
	delete[] skinData.inverseBindTransformArray;
	delete[] skinData.boneHashArray;
}

bool SkinStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kStructureTransform) || (type == kStructureSkeleton) || (type == kStructureBoneCountArray) || (type == kStructureBoneIndexArray) || (type == kStructureBoneWeightArray))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult SkinStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	const Structure *structure = GetFirstSubstructure(kStructureTransform);
	if (structure)
	{
		if (GetLastSubstructure(kStructureTransform) != structure)
		{
			return (kDataExtraneousSubstructure);
		}

		skinTransform = static_cast<const TransformStructure *>(structure)->GetTransform();
		static_cast<OpenGexDataDescription *>(dataDescription)->AdjustTransform(skinTransform);
	}
	else
	{
		skinTransform.SetIdentity();
	}

	structure = GetFirstSubstructure(kStructureSkeleton);
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastSubstructure(kStructureSkeleton) != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	skeletonStructure = static_cast<const SkeletonStructure *>(structure);

	structure = GetFirstSubstructure(kStructureBoneCountArray);
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastSubstructure(kStructureBoneCountArray) != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	boneCountArrayStructure = static_cast<const BoneCountArrayStructure *>(structure);

	structure = GetFirstSubstructure(kStructureBoneIndexArray);
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastSubstructure(kStructureBoneIndexArray) != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	boneIndexArrayStructure = static_cast<const BoneIndexArrayStructure *>(structure);

	structure = GetFirstSubstructure(kStructureBoneWeightArray);
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastSubstructure(kStructureBoneWeightArray) != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	boneWeightArrayStructure = static_cast<const BoneWeightArrayStructure *>(structure);

	int32 boneIndexCount = boneIndexArrayStructure->GetBoneIndexCount();
	if (boneWeightArrayStructure->GetBoneWeightCount() != boneIndexCount)
	{
		return (kDataOpenGexBoneWeightCountMismatch);
	}

	int32 vertexCount = boneCountArrayStructure->GetVertexCount();
	const unsigned_int16 *boneCountArray = boneCountArrayStructure->GetBoneCountArray();

	int32 boneWeightCount = 0;
	for (machine a = 0; a < vertexCount; a++)
	{
		unsigned_int32 count = boneCountArray[a];
		boneWeightCount += count;
	}

	if (boneWeightCount != boneIndexCount)
	{
		return (kDataOpenGexBoneWeightCountMismatch);
	}

	return (kDataOkay);
}

const SkinData *SkinStructure::BuildSkinData(const OpenGexDataDescription *dataDescription)
{
	const BoneRefArrayStructure *boneRefArrayStructure = skeletonStructure->GetBoneRefArrayStructure();
	const TransformStructure *boneTransformStructure = skeletonStructure->GetTransformStructure();

	int32 boneCount = boneRefArrayStructure->GetBoneCount();
	skinData.boneCount = boneCount;

	int32 vertexCount = boneCountArrayStructure->GetVertexCount();
	int32 boneWeightCount = boneWeightArrayStructure->GetBoneWeightCount();

	unsigned_int32 *boneHashArray = new unsigned_int32[boneCount];
	skinData.boneHashArray = boneHashArray;

	Transform4D *inverseBindTransformArray = new Transform4D[boneCount];
	skinData.inverseBindTransformArray = inverseBindTransformArray;

	const SkinWeight **skinWeightTable = new const SkinWeight *[vertexCount];
	skinData.skinWeightTable = skinWeightTable;

	unsigned_int32 skinWeightSize = vertexCount * (sizeof(SkinWeight) - sizeof(BoneWeight)) + boneWeightCount * sizeof(BoneWeight);
	skinWeightStorage = new char[skinWeightSize];

	const BoneNodeStructure *const *boneNodeArray = boneRefArrayStructure->GetBoneNodeArray();
	for (machine a = 0; a < boneCount; a++)
	{
		const BoneNodeStructure *boneNode = boneNodeArray[a];
		boneHashArray[a] = boneNode->GetNodeHash();

		Transform4D transform = boneTransformStructure->GetTransform(a);
		dataDescription->AdjustTransform(transform);

		inverseBindTransformArray[a] = Inverse(transform);
	}

	const unsigned_int16 *boneCountArray = boneCountArrayStructure->GetBoneCountArray();
	const unsigned_int16 *boneIndexArray = boneIndexArrayStructure->GetBoneIndexArray();
	const float *boneWeightArray = boneWeightArrayStructure->GetBoneWeightArray();

	SkinWeight *skinWeight = reinterpret_cast<SkinWeight *>(skinWeightStorage);
	for (machine a = 0; a < vertexCount; a++)
	{
		skinWeightTable[a] = skinWeight;

		int32 count = boneCountArray[a];
		skinWeight->boneCount = count;

		BoneWeight *boneWeight = skinWeight->boneWeight;
		for (machine b = 0; b < count; b++)
		{
			boneWeight->boneIndex = boneIndexArray[b];
			boneWeight->weight = boneWeightArray[b];
			boneWeight++;
		}

		skinWeight = reinterpret_cast<SkinWeight *>(boneWeight);
		boneIndexArray += count;
		boneWeightArray += count;
	}

	return (&skinData);
}


MorphStructure::MorphStructure() : OpenGexStructure(kStructureMorph)
{
	morphIndex = 0;
	baseFlag = false;
}

MorphStructure::~MorphStructure()
{
}

bool MorphStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "index")
	{
		*type = kDataUnsignedInt32;
		*value = &morphIndex;
		return (true);
	}

	if (identifier == "base")
	{
		*type = kDataUnsignedInt32;
		*value = &baseIndex;
		baseFlag = true;
		return (true);
	}

	return (false);
}

bool MorphStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kStructureName)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult MorphStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	morphName = nullptr;
	morphHash = 0;

	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	morphName = static_cast<const NameStructure *>(structure)->GetName();
	morphHash = Text::Hash(morphName);

	return (kDataOkay);
}


MeshStructure::MeshStructure() : OpenGexStructure(kStructureMesh)
{
	meshLevel = 0;

	skinStructure = nullptr;

	morphData.relativeArray = nullptr;
	morphData.baseIndexArray = nullptr;
	morphData.morphHashArray = nullptr;
	morphData.morphPositionTable = nullptr;
	morphData.morphNormalTable = nullptr;
}

MeshStructure::~MeshStructure()
{
	delete[] morphData.morphNormalTable;
	delete[] morphData.morphPositionTable;
	delete[] morphData.morphHashArray;
	delete[] morphData.baseIndexArray;
	delete[] morphData.relativeArray;

	indexArrayList.RemoveAll();
}

MeshStructure::MorphArrayData::MorphArrayData(unsigned_int32 index)
{
	morphIndex = index;

	positionArrayData = nullptr;
	normalArrayData = nullptr;
}

MeshStructure::MorphArrayData::~MorphArrayData()
{
}

bool MeshStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "lod")
	{
		*type = kDataUnsignedInt32;
		*value = &meshLevel;
		return (true);
	}

	if (identifier == "primitive")
	{
		*type = kDataString;
		*value = &meshPrimitive;
		return (true);
	}

	return (false);
}

bool MeshStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kStructureVertexArray) || (type == kStructureIndexArray) || (type == kStructureSkin))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult MeshStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	const Point2D	*texcoordArray[kMaxGeometryTexcoordCount];

	const void *arrayData[kMaxAttributeArrayCount] = {nullptr};
	int32 vertexCount = 0;

	Structure *structure = GetFirstSubnode();
	while (structure)
	{
		StructureType type = structure->GetStructureType();
		if (type == kStructureVertexArray)
		{
			const VertexArrayStructure *vertexArrayStructure = static_cast<const VertexArrayStructure *>(structure);
			int32 arrayIndex = vertexArrayStructure->GetArrayIndex();
			if (arrayIndex != -1)
			{
				int32 count = vertexArrayStructure->GetVertexCount();
				if (vertexCount == 0)
				{
					vertexCount = count;
				}
				else if (vertexCount != count)
				{
					return (kDataOpenGexVertexCountMismatch);
				}

				unsigned_int32 morphIndex = vertexArrayStructure->GetMorphIndex();
				if (morphIndex == 0)
				{
					if (arrayData[arrayIndex])
					{
						return (kDataOpenGexDuplicateVertexArray);
					}

					arrayData[arrayIndex] = vertexArrayStructure->GetVertexArrayData();
				}
				else if ((arrayIndex == kArrayPosition) || (arrayIndex == kArrayNormal))
				{
					MorphArrayData *morphArrayData = morphArrayMap.Find(morphIndex);
					if (morphArrayData)
					{
						if (((arrayIndex == kArrayPosition) && (morphArrayData->positionArrayData)) || (morphArrayData->normalArrayData))
						{
							return (kDataOpenGexDuplicateVertexArray);
						}
					}
					else
					{
						morphArrayData = new MorphArrayData(morphIndex);
						morphArrayMap.Insert(morphArrayData);
					}

					if (arrayIndex == kArrayPosition)
					{
						morphArrayData->positionArrayData = static_cast<const Point3D *>(vertexArrayStructure->GetVertexArrayData());
					}
					else
					{
						morphArrayData->normalArrayData = static_cast<const Vector3D *>(vertexArrayStructure->GetVertexArrayData());
					}
				}
			}
		}
		else if (type == kStructureIndexArray)
		{
			IndexArrayStructure *indexArrayStructure = static_cast<IndexArrayStructure *>(structure);
			indexArrayList.Append(indexArrayStructure);
		}
		else if (type == kStructureSkin)
		{
			if (skinStructure)
			{
				return (kDataExtraneousSubstructure);
			}

			skinStructure = static_cast<SkinStructure *>(structure);
		}

		structure = structure->Next();
	}

	if ((vertexCount < 3) || (vertexCount > 65535))
	{
		return (kDataOpenGexVertexCountUnsupported);
	}

	int32 surfaceCount = indexArrayList.GetElementCount();
	if (surfaceCount == 0)
	{
		return (kDataOpenGexIndexArrayRequired);
	}

	const Point3D *positionArray = static_cast<const Point3D *>(arrayData[kArrayPosition]);
	if (!positionArray)
	{
		return (kDataOpenGexPositionArrayRequired);
	}

	unsigned_int32 surfaceFlags = kSurfaceValidTangents;

	const Vector3D *normalArray = static_cast<const Vector3D *>(arrayData[kArrayNormal]);
	if (normalArray)
	{
		surfaceFlags |= kSurfaceValidNormals;
	}

	const ColorRGBA *colorArray = static_cast<const ColorRGBA *>(arrayData[kArrayColor]);
	if (colorArray)
	{
		surfaceFlags |= kSurfaceValidColors;
	}

	int32 texcoordCount = 1;
	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		texcoordArray[a] = static_cast<const Point2D *>(arrayData[kArrayTexcoord + a]);
		if (texcoordArray[a])
		{
			texcoordCount = (int32) (a + 1);
		}
	}

	MorphArrayData *morphArrayData = morphArrayMap.First();
	while (morphArrayData)
	{
		if (!morphArrayData->positionArrayData)
		{
			morphArrayData->positionArrayData = positionArray;
		}

		if (!morphArrayData->normalArrayData)
		{
			morphArrayData->normalArrayData = normalArray;
		}

		morphArrayData = morphArrayData->Next();
	}

	Transform4D skinTransform = Identity4D;
	Transform4D inverseSkinTransform = Identity4D;

	if (skinStructure)
	{
		skinTransform = skinStructure->GetSkinTransform();
		inverseSkinTransform = Inverse(skinTransform);
	}

	const IndexArrayStructure *indexArrayStructure = indexArrayList.First();
	do
	{
		static const unsigned_int8 ccwIndex[3] = {0, 1, 2};
		static const unsigned_int8 cwIndex[3] = {0, 2, 1};

		const String<>& front = indexArrayStructure->GetFrontFace();
		const unsigned_int8 *windingRemap = (front == "ccw") ? ccwIndex : cwIndex;

		GeometrySurface *surface = new GeometrySurface(surfaceFlags, indexArrayStructure->GetMaterialIndex(), texcoordCount);
		surfaceList.Append(surface);

		int32 triangleCount = indexArrayStructure->GetTriangleCount();
		const Triangle *triangle = indexArrayStructure->GetTriangleArray();

		for (machine a = 0; a < triangleCount; a++)
		{
			GeometryVertex		*vertex[3];
			Vector4D			tangent;

			GeometryPolygon *polygon = new GeometryPolygon;
			surface->polygonList.Append(polygon);

			for (machine b = 0; b < 3; b++)
			{
				GeometryVertex *gv = new GeometryVertex;
				polygon->vertexList.Append(gv);
				vertex[b] = gv;

				unsigned_int32 index = triangle->index[windingRemap[b]];
				gv->index = index;

				gv->position = skinTransform * positionArray[index];

				if (normalArray)
				{
					gv->normal = Normalize(normalArray[index] * inverseSkinTransform);
				}

				if (colorArray)
				{
					gv->color = colorArray[index];
				}

				for (machine c = 0; c < kMaxGeometryTexcoordCount; c++)
				{
					if (texcoordArray[c])
					{
						gv->texcoord[c] = texcoordArray[c][index];
					}
				}
			}

			if (Math::CalculateTangent(vertex[0]->position, vertex[1]->position, vertex[2]->position, vertex[0]->texcoord[0], vertex[1]->texcoord[0], vertex[2]->texcoord[0], &tangent))
			{
				vertex[0]->tangent = tangent;
				vertex[1]->tangent = tangent;
				vertex[2]->tangent = tangent;
			}

			triangle++;
		}

		indexArrayStructure = indexArrayStructure->Next();
	} while (indexArrayStructure);

	return (kDataOkay);
}

const MorphData *MeshStructure::BuildMorphData(const Map<MorphStructure> *morphMap)
{
	int32 morphCount = morphArrayMap.GetElementCount();
	if (morphCount == 0)
	{
		return (nullptr);
	}

	morphData.morphCount = morphCount;

	bool *relativeArray = new bool[morphCount];
	morphData.relativeArray = relativeArray;

	int32 *baseIndexArray = new int32[morphCount];
	morphData.baseIndexArray = baseIndexArray;

	unsigned_int32 *morphHashArray = new unsigned_int32[morphCount];
	morphData.morphHashArray = morphHashArray;

	const Point3D **morphPositionTable = new const Point3D *[morphCount];
	morphData.morphPositionTable = morphPositionTable;

	const Vector3D **morphNormalTable = new const Vector3D *[morphCount];
	morphData.morphNormalTable = morphNormalTable;

	const MorphArrayData *morphArrayData = morphArrayMap.First();
	while (morphArrayData)
	{
		unsigned_int32 morphIndex = morphArrayData->morphIndex;
		const MorphStructure *morphStructure = morphMap->Find(morphIndex);
		if (morphStructure)
		{
			bool relative = morphStructure->GetBaseFlag();
			if (relative)
			{
				*relativeArray = true;
				*baseIndexArray = -1;

				unsigned_int32 baseIndex = morphStructure->GetBaseIndex();
				if (baseIndex != 0)
				{
					int32 index = 0;
					const MorphArrayData *data = morphArrayMap.First();
					do
					{
						if (data->morphIndex == baseIndex)
						{
							*baseIndexArray = index;
							break;
						}

						index++;
						data = data->Next();
					} while (data);
				}
			}
			else
			{
				*relativeArray = false;
			}

			*morphHashArray = morphStructure->GetMorphHash();
		}
		else
		{
			*morphHashArray = morphIndex;
		}

		*morphPositionTable = morphArrayData->positionArrayData;
		*morphNormalTable = morphArrayData->normalArrayData;

		relativeArray++;
		baseIndexArray++;
		morphHashArray++;
		morphPositionTable++;
		morphNormalTable++;

		morphArrayData = morphArrayData->Next();
	}

	return (&morphData);
}


ObjectStructure::ObjectStructure(StructureType type) : OpenGexStructure(type)
{
	SetBaseStructureType(kStructureObject);
}

ObjectStructure::~ObjectStructure()
{
}


GeometryObjectStructure::GeometryObjectStructure() : ObjectStructure(kStructureGeometryObject)
{
	visibleFlag = true;
	shadowFlag = true;
	motionBlurFlag = true;

	geometryObject = nullptr;
}

GeometryObjectStructure::~GeometryObjectStructure()
{
	morphMap.RemoveAll();
	meshMap.RemoveAll();

	if (geometryObject)
	{
		geometryObject->Release();
	}
}

bool GeometryObjectStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "visible")
	{
		*type = kDataBool;
		*value = &visibleFlag;
		return (true);
	}

	if (identifier == "shadow")
	{
		*type = kDataBool;
		*value = &shadowFlag;
		return (true);
	}

	if (identifier == "motion_blur")
	{
		*type = kDataBool;
		*value = &motionBlurFlag;
		return (true);
	}

	return (false);
}

bool GeometryObjectStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kStructureMesh) || (type == kStructureMorph))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult GeometryObjectStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	int32 meshCount = 0;
	int32 skinCount = 0;
	int32 morphCount = 0;

	Structure *structure = GetFirstCoreSubnode();
	while (structure)
	{
		StructureType type = structure->GetStructureType();
		if (type == kStructureMesh)
		{
			MeshStructure *meshStructure = static_cast<MeshStructure *>(structure);
			if (!meshMap.Insert(meshStructure))
			{
				return (kDataOpenGexDuplicateLod);
			}

			meshCount++;
			skinCount += (meshStructure->GetSkinStructure() != nullptr);
			morphCount += (!meshStructure->GetMorphArrayMap()->Empty());
		}
		else if (type == kStructureMorph)
		{
			MorphStructure *morphStructure = static_cast<MorphStructure *>(structure);
			if (!morphMap.Insert(morphStructure))
			{
				return (kDataOpenGexDuplicateMorph);
			}
		}

		structure = structure->Next();
	}

	if (meshCount == 0)
	{
		return (kDataMissingSubstructure);
	}

	if ((skinCount != 0) && (skinCount != meshCount))
	{
		return (kDataOpenGexMissingLodSkin);
	}

	if ((morphCount != 0) && (morphCount != meshCount))
	{
		return (kDataOpenGexMissingLodMorph);
	}

	return (kDataOkay);
}

GenericGeometryObject *GeometryObjectStructure::GetGeometryObject(const OpenGexDataDescription *dataDescription)
{
	if (!geometryObject)
	{
		Array<const List<GeometrySurface> *, 8>		surfaceListArray;
		Array<int32, 8>								materialIndexArray;
		Array<const SkinData *, 4>					skinDataArray;
		Array<const MorphData *, 4>					morphDataArray;

		MeshStructure *meshStructure = meshMap.First();
		const List<GeometrySurface> *surfaceList = meshStructure->GetSurfaceList();
		surfaceListArray.AddElement(surfaceList);

		const GeometrySurface *surface = surfaceList->First();
		while (surface)
		{
			materialIndexArray.AddElement(surface->materialIndex);
			surface = surface->Next();
		}

		SkinStructure *skinStructure = meshStructure->GetSkinStructure();
		if (skinStructure)
		{
			skinDataArray.AddElement(skinStructure->BuildSkinData(dataDescription));
		}

		const MorphData *morphData = meshStructure->BuildMorphData(&morphMap);
		if (morphData)
		{
			morphDataArray.AddElement(morphData);
		}

		for (;;)
		{
			meshStructure = meshStructure->Next();
			if (!meshStructure)
			{
				break;
			}

			surfaceListArray.AddElement(meshStructure->GetSurfaceList());

			skinStructure = meshStructure->GetSkinStructure();
			if (skinStructure)
			{
				skinDataArray.AddElement(skinStructure->BuildSkinData(dataDescription));
			}

			morphData = meshStructure->BuildMorphData(&morphMap);
			if (morphData)
			{
				morphDataArray.AddElement(morphData);
			}
		}

		const SkinData *const *skinDataTable = (!skinDataArray.Empty()) ? skinDataArray : static_cast<const SkinData *const *>(nullptr);
		const MorphData *const *morphDataTable = (!morphDataArray.Empty()) ? morphDataArray : static_cast<const MorphData *const *>(nullptr);
		geometryObject = new GenericGeometryObject(surfaceListArray.GetElementCount(), surfaceListArray, surfaceList->GetElementCount(), materialIndexArray, skinDataTable, morphDataTable);

		unsigned_int32 geometryFlags = geometryObject->GetGeometryFlags();

		if (!visibleFlag)
		{
			geometryFlags |= kGeometryInvisible;
		}

		if (!shadowFlag)
		{
			geometryFlags &= ~kGeometryCastShadows;
		}

		if (!motionBlurFlag)
		{
			geometryFlags |= kGeometryMotionBlurInhibit;
		}

		geometryObject->SetGeometryFlags(geometryFlags);
	}

	return (geometryObject);
}

const MorphStructure *GeometryObjectStructure::FindMorphStructure(unsigned_int32 hash) const
{
	const MorphStructure *morphStructure = morphMap.First();
	while (morphStructure)
	{
		if (morphStructure->GetMorphHash() == hash)
		{
			return (morphStructure);
		}

		morphStructure = morphStructure->Next();
	}

	return (nullptr);
}


LightObjectStructure::LightObjectStructure() : ObjectStructure(kStructureLightObject)
{
	shadowFlag = true;
	spotApex = 1.0F;
	textureName = nullptr;
}

LightObjectStructure::~LightObjectStructure()
{
}

bool LightObjectStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "type")
	{
		*type = kDataString;
		*value = &typeString;
		return (true);
	}

	if (identifier == "shadow")
	{
		*type = kDataBool;
		*value = &shadowFlag;
		return (true);
	}

	return (false);
}

bool LightObjectStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if ((structure->GetBaseStructureType() == kStructureAttrib) || (structure->GetStructureType() == kStructureAtten))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult LightObjectStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (typeString == "infinite")
	{
		lightType = kLightInfinite;
	}
	else if (typeString == "point")
	{
		lightType = kLightPoint;
	}
	else if (typeString == "spot")
	{
		lightType = kLightSpot;
	}
	else
	{
		return (kDataOpenGexUndefinedLightType);
	}

	ColorRGB color(1.0F, 1.0F, 1.0F);
	float intensity = 1.0F;
	float range = 0.0F;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		StructureType type = structure->GetStructureType();
		if (type == kStructureColor)
		{
			const ColorStructure *colorStructure = static_cast<const ColorStructure *>(structure);
			if (colorStructure->GetAttribString() == "light")
			{
				color = colorStructure->GetColor().GetColorRGB();
			}
		}
		else if (type == kStructureParam)
		{
			const ParamStructure *paramStructure = static_cast<const ParamStructure *>(structure);
			if (paramStructure->GetAttribString() == "intensity")
			{
				intensity = paramStructure->GetParam();
			}
		}
		else if (type == kStructureTexture)
		{
			const TextureStructure *textureStructure = static_cast<const TextureStructure *>(structure);
			if (textureStructure->GetAttribString() == "projection")
			{
				textureName = textureStructure->GetTextureName();
			}
		}
		else if (type == kStructureAtten)
		{
			const AttenStructure *attenStructure = static_cast<const AttenStructure *>(structure);
			const String<>& attenKind = attenStructure->GetAttenKind();
			const String<>& curveType = attenStructure->GetCurveType();

			if (attenKind == "distance")
			{
				if ((curveType == "linear") || (curveType == "smooth"))
				{
					float beginParam = attenStructure->GetBeginParam();
					float endParam = attenStructure->GetEndParam();

					if (endParam > beginParam)
					{
						range = (range == 0.0F) ? endParam : Fmin(range, endParam);
					}
				}
				else if (curveType == "inverse")
				{
					float scaleParam = attenStructure->GetScaleParam();
					float linearParam = attenStructure->GetLinearParam();

					if ((scaleParam > K::min_float) && (Fabs(linearParam) > K::min_float))
					{
						float offsetParam = attenStructure->GetOffsetParam();
						float constantParam = attenStructure->GetConstantParam();

						float f = (Fabs(offsetParam) < K::min_float) ? -256.0F : 1.0F / offsetParam;

						float r = -scaleParam * (constantParam + f) / linearParam;
						if (r > 0.0F)
						{
							range = (range == 0.0F) ? r : Fmin(range, r);
						}
					}
				}
				else if (curveType == "inverse_square")
				{
					float scaleParam = attenStructure->GetScaleParam();
					float quadraticParam = attenStructure->GetQuadraticParam();

					if ((scaleParam > K::min_float) && (Fabs(quadraticParam) > K::min_float))
					{
						float offsetParam = attenStructure->GetOffsetParam();
						float constantParam = attenStructure->GetConstantParam();
						float linearParam = attenStructure->GetLinearParam();

						float f = (Fabs(offsetParam) < K::min_float) ? -256.0F : 1.0F / offsetParam;

						float d = linearParam * linearParam - 4.0F * quadraticParam * (constantParam + f);
						if (d > K::min_float)
						{
							float r = -scaleParam * 0.5F / quadraticParam * (linearParam - Sqrt(d));
							if (r > 0.0F)
							{
								range = (range == 0.0F) ? r : Fmin(range, r);
							}
						}
					}
				}
				else
				{
					return (kDataOpenGexUndefinedCurve);
				}
			}
			else if (attenKind == "angle")
			{
				float endParam = attenStructure->GetEndParam();
				spotApex = Tan(endParam);
			}
			else if (attenKind == "cos_angle")
			{
				float endParam = attenStructure->GetEndParam();
				if (endParam > K::min_float)
				{
					spotApex = endParam * InverseSqrt(1.0F - endParam * endParam);
				}
			}
			else
			{
				return (kDataOpenGexUndefinedAtten);
			}
		}

		structure = structure->Next();
	}

	lightColor = color * intensity;
	lightRange = (range > 0.0F) ? range : 1.0F;

	return (kDataOkay);
}


CameraObjectStructure::CameraObjectStructure() : ObjectStructure(kStructureCameraObject)
{
}

CameraObjectStructure::~CameraObjectStructure()
{
}

bool CameraObjectStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kStructureParam)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult CameraObjectStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	focalLength = 2.0F;
	nearDepth = 0.1F;
	farDepth = 1000.0F;

	const OpenGexDataDescription *openGexDataDescription = static_cast<OpenGexDataDescription *>(dataDescription);
	float distanceScale = openGexDataDescription->GetDistanceScale();
	float angleScale = openGexDataDescription->GetAngleScale();

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kStructureParam)
		{
			const ParamStructure *paramStructure = static_cast<const ParamStructure *>(structure);
			const String<>& attribString = paramStructure->GetAttribString();
			float param = paramStructure->GetParam();

			if (attribString == "fov")
			{
				float t = Tan(param * angleScale * 0.5F);
				if (t > K::min_float)
				{
					focalLength = 1.0F / t;
				}
			}
			else if (attribString == "near")
			{
				if (param > K::min_float)
				{
					nearDepth = param * distanceScale;
				}
			}
			else if (attribString == "far")
			{
				if (param > K::min_float)
				{
					farDepth = param * distanceScale;
				}
			}
		}

		structure = structure->Next();
	}

	return (kDataOkay);
}


AttribStructure::AttribStructure(StructureType type) : OpenGexStructure(type)
{
	SetBaseStructureType(kStructureAttrib);
}

AttribStructure::~AttribStructure()
{
}

bool AttribStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "attrib")
	{
		*type = kDataString;
		*value = &attribString;
		return (true);
	}

	return (false);
}


ParamStructure::ParamStructure() : AttribStructure(kStructureParam)
{
}

ParamStructure::~ParamStructure()
{
}

bool ParamStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
		unsigned_int32 arraySize = dataStructure->GetArraySize();
		return (arraySize == 0);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult ParamStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	if (dataStructure->GetDataElementCount() == 1)
	{
		param = dataStructure->GetDataElement(0);
	}
	else
	{
		return (kDataInvalidDataFormat);
	}

	return (kDataOkay);
}

void ParamStructure::UpdateMaterial(MaterialObject *materialObject) const
{
	if (GetAttribString() == "specular_power")
	{
		Attribute *attribute = materialObject->FindAttribute(kAttributeSpecular);
		if (!attribute)
		{
			materialObject->AddAttribute(new SpecularAttribute(K::white, param));
		}
		else
		{
			static_cast<SpecularAttribute *>(attribute)->SetSpecularExponent(param);
		}
	}
}


ColorStructure::ColorStructure() : AttribStructure(kStructureColor)
{
}

ColorStructure::~ColorStructure()
{
}

bool ColorStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
		unsigned_int32 arraySize = dataStructure->GetArraySize();
		return ((arraySize >= 3) && (arraySize <= 4));
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult ColorStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	unsigned_int32 arraySize = dataStructure->GetArraySize();
	if (dataStructure->GetDataElementCount() == arraySize)
	{
		const float *data = &dataStructure->GetDataElement(0);

		if (arraySize == 3)
		{
			color.Set(data[0], data[1], data[2], 1.0F);
		}
		else
		{
			color.Set(data[0], data[1], data[2], data[3]);
		}
	}
	else
	{
		return (kDataInvalidDataFormat);
	}

	return (kDataOkay);
}

void ColorStructure::UpdateMaterial(MaterialObject *materialObject) const
{
	const String<>& attribString = GetAttribString();

	if (attribString == "diffuse")
	{
		Attribute *attribute = materialObject->FindAttribute(kAttributeDiffuse);
		if (!attribute)
		{
			materialObject->AddAttribute(new DiffuseAttribute(color));
		}
		else
		{
			static_cast<DiffuseAttribute *>(attribute)->SetDiffuseColor(color);
		}
	}
	else if (attribString == "specular")
	{
		Attribute *attribute = materialObject->FindAttribute(kAttributeSpecular);
		if (!attribute)
		{
			materialObject->AddAttribute(new SpecularAttribute(color, 50.0F));
		}
		else
		{
			static_cast<SpecularAttribute *>(attribute)->SetSpecularColor(color);
		}
	}
	else if (attribString == "emission")
	{
		Attribute *attribute = materialObject->FindAttribute(kAttributeEmission);
		if (!attribute)
		{
			materialObject->AddAttribute(new EmissionAttribute(color));
		}
		else
		{
			static_cast<EmissionAttribute *>(attribute)->SetEmissionColor(color);
		}
	}
}


TextureStructure::TextureStructure() : AttribStructure(kStructureTexture)
{
	texcoordIndex = 0;
	texcoordTransform.SetIdentity();
}

TextureStructure::~TextureStructure()
{
}

bool TextureStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "texcoord")
	{
		*type = kDataUnsignedInt32;
		*value = &texcoordIndex;
		return (true);
	}

	return (AttribStructure::ValidateProperty(dataDescription, identifier, type, value));
}

bool TextureStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kDataString) || (type == kStructureAnimation) || (structure->GetBaseStructureType() == kStructureMatrix))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult TextureStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = AttribStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	bool nameFlag = false;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kDataString)
		{
			if (!nameFlag)
			{
				nameFlag = true;

				const DataStructure<StringDataType> *dataStructure = static_cast<const DataStructure<StringDataType> *>(structure);
				if (dataStructure->GetDataElementCount() == 1)
				{
					SetTextureName(static_cast<OpenGexDataDescription *>(dataDescription), dataStructure->GetDataElement(0));
				}
				else
				{
					return (kDataInvalidDataFormat);
				}
			}
			else
			{
				return (kDataExtraneousSubstructure);
			}
		}
		else if (structure->GetBaseStructureType() == kStructureMatrix)
		{
			const MatrixStructure *matrixStructure = static_cast<const MatrixStructure *>(structure);
			texcoordTransform = texcoordTransform * matrixStructure->GetMatrix();
		}

		structure = structure->Next();
	}

	if (!nameFlag)
	{
		return (kDataMissingSubstructure);
	}

	return (kDataOkay);
}

void TextureStructure::SetTextureName(const OpenGexDataDescription *dataDescription, const char *name)
{
	const char *importName = nullptr;

	if (name[0] == '/')
	{
		int32 start = Text::FindTextCaseless(name, "/Import/");
		if (start >= 0)
		{
			importName = name + (start + 8);
			const char *virtualName = importName + Text::GetPrefixDirectoryLength(importName);

			if (virtualName[0] != 0)
			{
				name = virtualName;
			}
		}

		textureName = name;
	}
	else
	{
		textureName = dataDescription->GetBaseImportPath();
		textureName += name;
	}

	for (machine a = textureName.Length() - 1; a > 0; a--)
	{
		char c = textureName[a];
		if (c == '.')
		{
			textureName[a] = 0;
			break;
		}
		else if (c == '/')
		{
			break;
		}
	}

	if ((dataDescription->GetImportFlags() & kGeometryImportTextures) && (importName))
	{
		TextureResource *resource = TextureResource::Get(textureName, kResourceNoDefault);
		if (!resource)
		{
			ResourcePath importPath(importName, Text::GetPrefixDirectoryLength(importName));
			importPath += textureName;

			TextureImporter textureImporter(importPath);
			if (textureImporter.SetTextureImage(0, importPath) == kEngineOkay)
			{
				if (GetAttribString() == "normal")
				{
					unsigned_int32 flags = textureImporter.GetTextureImportFlags();
					textureImporter.SetTextureImportFlags(flags | kTextureImportNormalMap);
				}

				textureImporter.ImportTextureImage();
			}
		}
		else
		{
			resource->Release();
		}
	}
}

void TextureStructure::UpdateMaterial(MaterialObject *materialObject) const
{
	if (textureName[0] != 0)
	{
		TextureAttribute *attribute = nullptr;

		const String<>& attribString = GetAttribString();

		if (attribString == "diffuse")
		{
			attribute = new DiffuseTextureAttribute(textureName);
		}
		else if (attribString == "specular")
		{
			attribute = new SpecularTextureAttribute(textureName);
		}
		else if (attribString == "emission")
		{
			attribute = new EmissionTextureAttribute(textureName);
		}
		else if (attribString == "normal")
		{
			attribute = new NormalTextureAttribute(textureName);
		}
		else if (attribString == "opacity")
		{
			attribute = new OpacityTextureAttribute(textureName);
		}

		if (attribute)
		{
			materialObject->AddAttribute(attribute);

			int32 index = Min(texcoordIndex, kMaxMaterialTexcoordCount);
			attribute->SetTexcoordIndex(index);

			materialObject->SetTexcoordScale(index, Vector2D(texcoordTransform(0,0), texcoordTransform(1,1)));
			materialObject->SetTexcoordOffset(index, Vector2D(texcoordTransform(0,3), texcoordTransform(1,3)));
		}
	}
}


AttenStructure::AttenStructure() :
		OpenGexStructure(kStructureAtten),
		attenKind("distance"),
		curveType("linear")
{
	beginParam = 0.0F;
	endParam = 1.0F;

	scaleParam = 1.0F;
	offsetParam = 0.0F;

	constantParam = 0.0F;
	linearParam = 0.0F;
	quadraticParam = 1.0F;

	powerParam = 1.0F;
}

AttenStructure::~AttenStructure()
{
}

bool AttenStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "kind")
	{
		*type = kDataString;
		*value = &attenKind;
		return (true);
	}

	if (identifier == "curve")
	{
		*type = kDataString;
		*value = &curveType;
		return (true);
	}

	return (false);
}

bool AttenStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kStructureParam)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult AttenStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (curveType == "inverse")
	{
		linearParam = 1.0F;
	}

	const OpenGexDataDescription *openGexDataDescription = static_cast<OpenGexDataDescription *>(dataDescription);
	float distanceScale = openGexDataDescription->GetDistanceScale();
	float angleScale = openGexDataDescription->GetAngleScale();

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kStructureParam)
		{
			const ParamStructure *paramStructure = static_cast<const ParamStructure *>(structure);
			const String<>& attribString = paramStructure->GetAttribString();

			if (attribString == "begin")
			{
				beginParam = paramStructure->GetParam();

				if (attenKind == "distance")
				{
					beginParam *= distanceScale;
				}
				else if (attenKind == "angle")
				{
					beginParam *= angleScale;
				}
			}
			else if (attribString == "end")
			{
				endParam = paramStructure->GetParam();

				if (attenKind == "distance")
				{
					endParam *= distanceScale;
				}
				else if (attenKind == "angle")
				{
					endParam *= angleScale;
				}
			}
			else if (attribString == "scale")
			{
				scaleParam = paramStructure->GetParam();

				if (attenKind == "distance")
				{
					scaleParam *= distanceScale;
				}
				else if (attenKind == "angle")
				{
					scaleParam *= angleScale;
				}
			}
			else if (attribString == "offset")
			{
				offsetParam = paramStructure->GetParam();
			}
			else if (attribString == "constant")
			{
				constantParam = paramStructure->GetParam();
			}
			else if (attribString == "linear")
			{
				linearParam = paramStructure->GetParam();
			}
			else if (attribString == "quadratic")
			{
				quadraticParam = paramStructure->GetParam();
			}
			else if (attribString == "power")
			{
				powerParam = paramStructure->GetParam();
			}
		}

		structure = structure->Next();
	}

	return (kDataOkay);
}


MaterialStructure::MaterialStructure() : OpenGexStructure(kStructureMaterial)
{
	twoSidedFlag = false;
	materialName = nullptr;
	materialObject = nullptr;
}

MaterialStructure::~MaterialStructure()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

bool MaterialStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "two_sided")
	{
		*type = kDataBool;
		*value = &twoSidedFlag;
		return (true);
	}

	return (false);
}

bool MaterialStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if ((structure->GetBaseStructureType() == kStructureAttrib) || (structure->GetStructureType() == kStructureName))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult MaterialStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	const OpenGexDataDescription *openGexDataDescription = static_cast<OpenGexDataDescription *>(dataDescription);
	unsigned_int32 importFlags = openGexDataDescription->GetImportFlags();
	Editor *editor = openGexDataDescription->GetEditor();

	const Structure *structure = GetFirstSubstructure(kStructureName);
	if (structure)
	{
		if (GetLastSubstructure(kStructureName) != structure)
		{
			return (kDataExtraneousSubstructure);
		}

		if ((importFlags & kGeometryImportReuseNamedMaterials) && (editor))
		{
			materialName = static_cast<const NameStructure *>(structure)->GetName();

			materialObject = editor->GetEditorObject()->FindNamedMaterial(materialName);
			if (materialObject)
			{
				materialObject->Retain();
				return (kDataOkay);
			}
		}
	}

	materialObject = new MaterialObject;

	if (twoSidedFlag)
	{
		materialObject->SetMaterialFlags(materialObject->GetMaterialFlags() | kMaterialTwoSided);
	}

	structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetBaseStructureType() == kStructureAttrib)
		{
			static_cast<const AttribStructure *>(structure)->UpdateMaterial(materialObject);
		}

		structure = structure->Next();
	}

	if (editor)
	{
		EditorObject *editorObject = editor->GetEditorObject();

		if (importFlags & kGeometryImportMergeMaterials)
		{
			MaterialObject *object = editorObject->FindMatchingMaterial(materialObject);
			if (object)
			{
				materialObject->Release();
				materialObject = object;
				object->Retain();

				return (kDataOkay);
			}
		}

		editorObject->AddMaterial(materialObject, materialName);
	}

	return (kDataOkay);
}


KeyStructure::KeyStructure() :
		OpenGexStructure(kStructureKey),
		keyKind("value")
{
}

KeyStructure::~KeyStructure()
{
}

bool KeyStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "kind")
	{
		*type = kDataString;
		*value = &keyKind;
		return (true);
	}

	return (false);
}

bool KeyStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kDataFloat)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult KeyStructure::ProcessData(DataDescription *dataDescription)
{
	const Structure *structure = GetFirstCoreSubnode();
	if (!structure)
	{
		return (kDataMissingSubstructure);
	}

	if (GetLastCoreSubnode() != structure)
	{
		return (kDataExtraneousSubstructure);
	}

	const DataStructure<FloatDataType> *dataStructure = static_cast<const DataStructure<FloatDataType> *>(structure);
	if (dataStructure->GetDataElementCount() == 0)
	{
		return (kDataOpenGexEmptyKeyStructure);
	}

	if ((keyKind == "value") || (keyKind == "-control") || (keyKind == "+control"))
	{
		scalarFlag = false;
	}
	else if ((keyKind == "tension") || (keyKind == "continuity") || (keyKind == "bias"))
	{
		scalarFlag = true;

		if (dataStructure->GetArraySize() != 0)
		{
			return (kDataInvalidDataFormat);
		}
	}
	else
	{
		return (kDataOpenGexInvalidKeyKind);
	}

	return (kDataOkay);
}


CurveStructure::CurveStructure(StructureType type) :
		OpenGexStructure(type),
		curveType("linear")
{
	SetBaseStructureType(kStructureCurve);
}

CurveStructure::~CurveStructure()
{
}

bool CurveStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "curve")
	{
		*type = kDataString;
		*value = &curveType;
		return (true);
	}

	return (false);
}

bool CurveStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kStructureKey)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult CurveStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	keyValueStructure = nullptr;
	keyControlStructure[0] = nullptr;
	keyControlStructure[1] = nullptr;
	keyTensionStructure = nullptr;
	keyContinuityStructure = nullptr;
	keyBiasStructure = nullptr;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kStructureKey)
		{
			const KeyStructure *keyStructure = static_cast<const KeyStructure *>(structure);
			const String<>& keyKind = keyStructure->GetKeyKind();

			if (keyKind == "value")
			{
				if (!keyValueStructure)
				{
					keyValueStructure = keyStructure;
				}
				else
				{
					return (kDataExtraneousSubstructure);
				}
			}
			else if (keyKind == "-control")
			{
				if (curveType != "bezier")
				{
					return (kDataOpenGexInvalidKeyKind);
				}

				if (!keyControlStructure[0])
				{
					keyControlStructure[0] = keyStructure;
				}
				else
				{
					return (kDataExtraneousSubstructure);
				}
			}
			else if (keyKind == "+control")
			{
				if (curveType != "bezier")
				{
					return (kDataOpenGexInvalidKeyKind);
				}

				if (!keyControlStructure[1])
				{
					keyControlStructure[1] = keyStructure;
				}
				else
				{
					return (kDataExtraneousSubstructure);
				}
			}
			else if (keyKind == "tension")
			{
				if (curveType != "tcb")
				{
					return (kDataOpenGexInvalidKeyKind);
				}

				if (!keyTensionStructure)
				{
					keyTensionStructure = keyStructure;
				}
				else
				{
					return (kDataExtraneousSubstructure);
				}
			}
			else if (keyKind == "continuity")
			{
				if (curveType != "tcb")
				{
					return (kDataOpenGexInvalidKeyKind);
				}

				if (!keyContinuityStructure)
				{
					keyContinuityStructure = keyStructure;
				}
				else
				{
					return (kDataExtraneousSubstructure);
				}
			}
			else if (keyKind == "bias")
			{
				if (curveType != "tcb")
				{
					return (kDataOpenGexInvalidKeyKind);
				}

				if (!keyBiasStructure)
				{
					keyBiasStructure = keyStructure;
				}
				else
				{
					return (kDataExtraneousSubstructure);
				}
			}
		}

		structure = structure->Next();
	}

	if (!keyValueStructure)
	{
		return (kDataMissingSubstructure);
	}

	if (curveType == "bezier")
	{
		if ((!keyControlStructure[0]) || (!keyControlStructure[1]))
		{
			return (kDataMissingSubstructure);
		}
	}
	else if (curveType == "tcb")
	{
		if ((!keyTensionStructure) || (!keyContinuityStructure) || (!keyBiasStructure))
		{
			return (kDataMissingSubstructure);
		}
	}

	return (kDataOkay);
}


TimeStructure::TimeStructure() : CurveStructure(kStructureTime)
{
}

TimeStructure::~TimeStructure()
{
}

DataResult TimeStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = CurveStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	const String<>& curveType = GetCurveType();
	if ((curveType != "linear") && (curveType != "bezier"))
	{
		return (kDataOpenGexInvalidCurveType);
	}

	int32 elementCount = 0;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		if (structure->GetStructureType() == kStructureKey)
		{
			const KeyStructure *keyStructure = static_cast<const KeyStructure *>(structure);
			const DataStructure<FloatDataType> *dataStructure = static_cast<DataStructure<FloatDataType> *>(keyStructure->GetFirstCoreSubnode());
			if (dataStructure->GetArraySize() != 0)
			{
				return (kDataInvalidDataFormat);
			}

			int32 count = dataStructure->GetDataElementCount();
			if (elementCount == 0)
			{
				elementCount = count;
			}
			else if (count != elementCount)
			{
				return (kDataOpenGexKeyCountMismatch);
			}
		}

		structure = structure->Next();
	}

	keyDataElementCount = elementCount;
	return (kDataOkay);
}

int32 TimeStructure::CalculateInterpolationParameter(float time, float *param) const
{
	const DataStructure<FloatDataType> *valueStructure = static_cast<DataStructure<FloatDataType> *>(GetKeyValueStructure()->GetFirstCoreSubnode());
	const float *value = &valueStructure->GetDataElement(0);

	int32 count = keyDataElementCount;
	int32 index = 0;

	for (; index < count; index++)
	{
		if (time < value[index])
		{
			break;
		}
	}

	if ((index > 0) && (index < count))
	{
		float t0 = value[index - 1];
		float t3 = value[index];

		float u = 0.0F;
		float dt = t3 - t0;
		if (dt > K::min_float)
		{
			u = (time - t0) / dt;
		}

		if (GetCurveType() == "bezier")
		{
			float t1 = static_cast<DataStructure<FloatDataType> *>(GetKeyControlStructure(1)->GetFirstCoreSubnode())->GetDataElement(index - 1);
			float t2 = static_cast<DataStructure<FloatDataType> *>(GetKeyControlStructure(0)->GetFirstCoreSubnode())->GetDataElement(index);

			float a0 = dt + (t1 - t2) * 3.0F;
			float a1 = a0 * 3.0F;
			float b0 = 3.0F * (t0 - t1 * 2.0F + t2);
			float b1 = b0 * 2.0F;
			float c = (t1 - t0) * 3.0F;
			float d = t0 - time;

			for (machine k = 0; k < 3; k++)
			{
				u = Saturate(u - ((((a0 * u) + b0) * u + c) * u + d) / (((a1 * u) + b1) * u + c));
			}
		}

		*param = u;
	}
	else
	{
		*param = 0.0F;
	}

	return (index - 1);
}


ValueStructure::ValueStructure() : CurveStructure(kStructureValue)
{
}

ValueStructure::~ValueStructure()
{
}

DataResult ValueStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = CurveStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	const String<>& curveType = GetCurveType();
	if ((curveType != "constant") && (curveType != "linear") && (curveType != "bezier") && (curveType != "tcb"))
	{
		return (kDataOpenGexInvalidCurveType);
	}

	const AnimatableStructure *targetStructure = static_cast<TrackStructure *>(GetSuperNode())->GetTargetStructure();
	const Structure *targetDataStructure = targetStructure->GetFirstCoreSubnode();
	if ((targetDataStructure) && (targetDataStructure->GetStructureType() == kDataFloat))
	{
		unsigned_int32 targetArraySize = static_cast<const PrimitiveStructure *>(targetDataStructure)->GetArraySize();
		int32 elementCount = 0;

		const Structure *structure = GetFirstSubnode();
		while (structure)
		{
			if (structure->GetStructureType() == kStructureKey)
			{
				const KeyStructure *keyStructure = static_cast<const KeyStructure *>(structure);
				const DataStructure<FloatDataType> *dataStructure = static_cast<DataStructure<FloatDataType> *>(keyStructure->GetFirstCoreSubnode());
				unsigned_int32 arraySize = dataStructure->GetArraySize();

				if ((!keyStructure->GetScalarFlag()) && (arraySize != targetArraySize))
				{
					return (kDataInvalidDataFormat);
				}

				int32 count = dataStructure->GetDataElementCount() / Max(arraySize, 1);
				if (elementCount == 0)
				{
					elementCount = count;
				}
				else if (count != elementCount)
				{
					return (kDataOpenGexKeyCountMismatch);
				}
			}

			structure = structure->Next();
		}

		keyDataElementCount = elementCount;
	}

	return (kDataOkay);
}

void ValueStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, int32 index, float param, AnimatableStructure *target) const
{
	float	data[16];

	const String<>& curveType = GetCurveType();
	const float *value = &static_cast<DataStructure<FloatDataType> *>(GetKeyValueStructure()->GetFirstCoreSubnode())->GetDataElement(0);

	int32 count = keyDataElementCount;
	int32 arraySize = Max(static_cast<PrimitiveStructure *>(target->GetFirstCoreSubnode())->GetArraySize(), 1);

	if (index < 0)
	{
		target->UpdateAnimation(dataDescription, value);
	}
	else if (index >= count - 1)
	{
		target->UpdateAnimation(dataDescription, value + arraySize * (count - 1));
	}
	else
	{
		const float *p1 = value + arraySize * index;

		if (curveType == "constant")
		{
			for (machine k = 0; k < arraySize; k++)
			{
				data[k] = p1[k];
			}
		}
		else
		{
			const float *p2 = p1 + arraySize;
			const float u = 1.0F - param;

			if (curveType == "linear")
			{
				for (machine k = 0; k < arraySize; k++)
				{
					data[k] = p1[k] * u + p2[k] * param;
				}
			}
			else if (curveType == "bezier")
			{
				const float *c1 = &static_cast<DataStructure<FloatDataType> *>(GetKeyControlStructure(1)->GetFirstCoreSubnode())->GetDataElement(arraySize * index);
				const float *c2 = &static_cast<DataStructure<FloatDataType> *>(GetKeyControlStructure(0)->GetFirstCoreSubnode())->GetDataElement(arraySize * (index + 1));

				float u2 = u * u;
				float u3 = u2 * u;
				float v2 = param * param;
				float v3 = v2 * param;
				float f1 = u2 * param * 3.0F;
				float f2 = u * v2 * 3.0F;

				for (machine k = 0; k < arraySize; k++)
				{
					data[k] = p1[k] * u3 + c1[k] * f1 + c2[k] * f2 + p2[k] * v3;
				}
			}
			else
			{
				const float *p0 = value + arraySize * MaxZero(index - 1);
				const float *p3 = value + arraySize * Min(index + 2, count - 1);

				const float *tension = &static_cast<DataStructure<FloatDataType> *>(GetKeyTensionStructure()->GetFirstCoreSubnode())->GetDataElement(0);
				const float *continuity = &static_cast<DataStructure<FloatDataType> *>(GetKeyContinuityStructure()->GetFirstCoreSubnode())->GetDataElement(0);
				const float *bias = &static_cast<DataStructure<FloatDataType> *>(GetKeyBiasStructure()->GetFirstCoreSubnode())->GetDataElement(0);

				float m1 = (1.0F - tension[index]) * (1.0F + continuity[index]) * (1.0F + bias[index]) * 0.5F;
				float n1 = (1.0F - tension[index]) * (1.0F - continuity[index]) * (1.0F - bias[index]) * 0.5F;
				float m2 = (1.0F - tension[index + 1]) * (1.0F - continuity[index + 1]) * (1.0F + bias[index + 1]) * 0.5F;
				float n2 = (1.0F - tension[index + 1]) * (1.0F + continuity[index + 1]) * (1.0F - bias[index + 1]) * 0.5F;

				float u2 = u * u;
				float v2 = param * param;
				float v3 = v2 * param;
				float f1 = 1.0F - v2 * 3.0F + v3 * 2.0F;
				float f2 = v2 * (3.0F - param * 2.0F);
				float f3 = param * u2;
				float f4 = u * v2;

				for (machine k = 0; k < arraySize; k++)
				{
					float t1 = (p1[k] - p0[k]) * m1 + (p2[k] - p1[k]) * n1;
					float t2 = (p2[k] - p1[k]) * m2 + (p3[k] - p2[k]) * n2;
					data[k] = p1[k] * f1 + p2[k] * f2 + t1 * f3 - t2 * f4;
				}
			}
		}

		target->UpdateAnimation(dataDescription, data);
	}
}


TrackStructure::TrackStructure() : OpenGexStructure(kStructureTrack)
{
	targetStructure = nullptr;
}

TrackStructure::~TrackStructure()
{
}

bool TrackStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "target")
	{
		*type = kDataRef;
		*value = &targetRef;
		return (true);
	}

	return (false);
}

bool TrackStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetBaseStructureType() == kStructureCurve)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult TrackStructure::ProcessData(DataDescription *dataDescription)
{
	if (targetRef.GetGlobalRefFlag())
	{
		return (kDataOpenGexTargetRefNotLocal);
	}

	Structure *target = GetSuperNode()->GetSuperNode()->FindStructure(targetRef);
	if (!target)
	{
		return (kDataBrokenRef);
	}

	if ((target->GetBaseStructureType() != kStructureMatrix) && (target->GetStructureType() != kStructureMorphWeight))
	{
		return (kDataOpenGexInvalidTargetStruct);
	}

	targetStructure = static_cast<AnimatableStructure *>(target);

	timeStructure = nullptr;
	valueStructure = nullptr;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		StructureType type = structure->GetStructureType();
		if (type == kStructureTime)
		{
			if (!timeStructure)
			{
				timeStructure = static_cast<const TimeStructure *>(structure);
			}
			else
			{
				return (kDataExtraneousSubstructure);
			}
		}
		else if (type == kStructureValue)
		{
			if (!valueStructure)
			{
				valueStructure = static_cast<const ValueStructure *>(structure);
			}
			else
			{
				return (kDataExtraneousSubstructure);
			}
		}

		structure = structure->Next();
	}

	if ((!timeStructure) || (!valueStructure))
	{
		return (kDataMissingSubstructure);
	}

	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (timeStructure->GetKeyDataElementCount() != valueStructure->GetKeyDataElementCount())
	{
		return (kDataOpenGexKeyCountMismatch);
	}

	static_cast<AnimationStructure *>(GetSuperNode())->AddTrack(this);
	return (kDataOkay);
}

void TrackStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, float time) const
{
	float	param;

	int32 index = timeStructure->CalculateInterpolationParameter(time, &param);
	valueStructure->UpdateAnimation(dataDescription, index, param, targetStructure);
}


AnimationStructure::AnimationStructure() : OpenGexStructure(kStructureAnimation)
{
	clipIndex = 0;
	beginFlag = false;
	endFlag = false;
}

AnimationStructure::~AnimationStructure()
{
	trackList.RemoveAll();
}

bool AnimationStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "clip")
	{
		*type = kDataUnsignedInt32;
		*value = &clipIndex;
		return (true);
	}

	if (identifier == "begin")
	{
		beginFlag = true;
		*type = kDataFloat;
		*value = &beginTime;
		return (true);
	}

	if (identifier == "end")
	{
		endFlag = true;
		*type = kDataFloat;
		*value = &endTime;
		return (true);
	}

	return (false);
}

bool AnimationStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	if (structure->GetStructureType() == kStructureTrack)
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult AnimationStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	if (trackList.Empty())
	{
		return (kDataMissingSubstructure);
	}

	static_cast<OpenGexDataDescription *>(dataDescription)->AddAnimation(this);
	return (kDataOkay);
}

Range<float> AnimationStructure::GetAnimationTimeRange(void) const
{
	float min = K::infinity;
	float max = 0.0F;

	const TrackStructure *trackStructure = trackList.First();
	while (trackStructure)
	{
		const KeyStructure *keyStructure = trackStructure->GetTimeStructure()->GetKeyValueStructure();
		const DataStructure<FloatDataType> *dataStructure = static_cast<DataStructure<FloatDataType> *>(keyStructure->GetFirstCoreSubnode());

		min = Fmin(min, dataStructure->GetDataElement(0));
		max = Fmax(max, dataStructure->GetDataElement(dataStructure->GetDataElementCount() - 1));

		trackStructure = trackStructure->Next();
	}

	if (beginFlag)
	{
		min = beginTime;
	}

	if (endFlag)
	{
		max = endTime;
	}

	return (Range<float>(min, Fmax(min, max)));
}

void AnimationStructure::UpdateAnimation(const OpenGexDataDescription *dataDescription, float time) const
{
	if (beginFlag)
	{
		time = Fmax(time, beginTime);
	}

	if (endFlag)
	{
		time = Fmin(time, endTime);
	}

	const TrackStructure *trackStructure = trackList.First();
	while (trackStructure)
	{
		trackStructure->UpdateAnimation(dataDescription, time);
		trackStructure = trackStructure->Next();
	}
}


ClipStructure::ClipStructure() : OpenGexStructure(kStructureClip)
{
	clipIndex = 0;
}

ClipStructure::~ClipStructure()
{
}

bool ClipStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "index")
	{
		*type = kDataUnsignedInt32;
		*value = &clipIndex;
		return (true);
	}

	return (false);
}

bool ClipStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	StructureType type = structure->GetStructureType();
	if ((type == kStructureName) || (type == kStructureParam))
	{
		return (true);
	}

	return (OpenGexStructure::ValidateSubstructure(dataDescription, structure));
}

DataResult ClipStructure::ProcessData(DataDescription *dataDescription)
{
	DataResult result = OpenGexStructure::ProcessData(dataDescription);
	if (result != kDataOkay)
	{
		return (result);
	}

	frameRate = 0.0F;
	clipName = nullptr;

	const Structure *structure = GetFirstSubnode();
	while (structure)
	{
		StructureType type = structure->GetStructureType();
		if (type == kStructureName)
		{
			if (clipName)
			{
				return (kDataExtraneousSubstructure);
			}

			clipName = static_cast<const NameStructure *>(structure)->GetName();
		}
		else if (type == kStructureParam)
		{
			const ParamStructure *paramStructure = static_cast<const ParamStructure *>(structure);
			if (paramStructure->GetAttribString() == "rate")
			{
				frameRate = paramStructure->GetParam();
			}
		}

		structure = structure->Next();
	}

	return (kDataOkay);
}


ExtensionStructure::ExtensionStructure() : OpenGexStructure(kStructureExtension)
{
}

ExtensionStructure::~ExtensionStructure()
{
}

bool ExtensionStructure::ValidateProperty(const DataDescription *dataDescription, const String<>& identifier, DataType *type, void **value)
{
	if (identifier == "applic")
	{
		*type = kDataString;
		*value = &applicationString;
		return (true);
	}

	if (identifier == "type")
	{
		*type = kDataString;
		*value = &typeString;
		return (true);
	}

	return (false);
}

bool ExtensionStructure::ValidateSubstructure(const DataDescription *dataDescription, const Structure *structure) const
{
	return ((structure->GetBaseStructureType() == kStructurePrimitive) || (structure->GetStructureType() == kStructureExtension));
}


OpenGexDataDescription::OpenGexDataDescription(Editor *editor, const char *file, unsigned_int32 flags, float scale) :
		worldEditor(editor),
		baseImportPath(&file[Text::GetPrefixDirectoryLength(file)])
{
	baseImportPath[Text::GetDirectoryPathLength(baseImportPath)] = 0;

	importFlags = flags;
	importScale = scale;

	distanceScale = scale;
	angleScale = 1.0F;
	timeScale = 1.0F;
	upDirection = 2;
}

OpenGexDataDescription::~OpenGexDataDescription()
{
	animationList.RemoveAll();
}

Structure *OpenGexDataDescription::CreateStructure(const String<>& identifier) const
{
	if (identifier == "Metric")
	{
		return (new MetricStructure);
	}

	if (identifier == "Name")
	{
		return (new NameStructure);
	}

	if (identifier == "ObjectRef")
	{
		return (new ObjectRefStructure);
	}

	if (identifier == "MaterialRef")
	{
		return (new MaterialRefStructure);
	}

	if (identifier == "Transform")
	{
		return (new TransformStructure);
	}

	if (identifier == "Translation")
	{
		return (new TranslationStructure);
	}

	if (identifier == "Rotation")
	{
		return (new RotationStructure);
	}

	if (identifier == "Scale")
	{
		return (new ScaleStructure);
	}

	if (identifier == "MorphWeight")
	{
		return (new MorphWeightStructure);
	}

	if (identifier == "Node")
	{
		return (new NodeStructure);
	}

	if (identifier == "BoneNode")
	{
		return (new BoneNodeStructure);
	}

	if (identifier == "GeometryNode")
	{
		return (new GeometryNodeStructure);
	}

	if (identifier == "LightNode")
	{
		return (new LightNodeStructure);
	}

	if (identifier == "CameraNode")
	{
		return (new CameraNodeStructure);
	}

	if (identifier == "VertexArray")
	{
		return (new VertexArrayStructure);
	}

	if (identifier == "IndexArray")
	{
		return (new IndexArrayStructure);
	}

	if (identifier == "BoneRefArray")
	{
		return (new BoneRefArrayStructure);
	}

	if (identifier == "BoneCountArray")
	{
		return (new BoneCountArrayStructure);
	}

	if (identifier == "BoneIndexArray")
	{
		return (new BoneIndexArrayStructure);
	}

	if (identifier == "BoneWeightArray")
	{
		return (new BoneWeightArrayStructure);
	}

	if (identifier == "Skeleton")
	{
		return (new SkeletonStructure);
	}

	if (identifier == "Skin")
	{
		return (new SkinStructure);
	}

	if (identifier == "Morph")
	{
		return (new MorphStructure);
	}

	if (identifier == "Mesh")
	{
		return (new MeshStructure);
	}

	if (identifier == "GeometryObject")
	{
		return (new GeometryObjectStructure);
	}

	if (identifier == "LightObject")
	{
		return (new LightObjectStructure);
	}

	if (identifier == "CameraObject")
	{
		return (new CameraObjectStructure);
	}

	if (identifier == "Param")
	{
		return (new ParamStructure);
	}

	if (identifier == "Color")
	{
		return (new ColorStructure);
	}

	if (identifier == "Texture")
	{
		return (new TextureStructure);
	}

	if (identifier == "Atten")
	{
		return (new AttenStructure);
	}

	if (identifier == "Material")
	{
		return (new MaterialStructure);
	}

	if (identifier == "Key")
	{
		return (new KeyStructure);
	}

	if (identifier == "Time")
	{
		return (new TimeStructure);
	}

	if (identifier == "Value")
	{
		return (new ValueStructure);
	}

	if (identifier == "Track")
	{
		return (new TrackStructure);
	}

	if (identifier == "Animation")
	{
		return (new AnimationStructure);
	}

	if (identifier == "Clip")
	{
		return (new ClipStructure);
	}

	if (identifier == "Extension")
	{
		return (new ExtensionStructure);
	}

	return (nullptr);
}

bool OpenGexDataDescription::ValidateTopLevelStructure(const Structure *structure) const
{
	StructureType type = structure->GetBaseStructureType();
	if ((type == kStructureNode) || (type == kStructureObject))
	{
		return (true);
	}

	type = structure->GetStructureType();
	return ((type == kStructureMetric) || (type == kStructureMaterial) || (type == kStructureClip) || (type == kStructureExtension));
}

DataResult OpenGexDataDescription::ProcessData(void)
{
	DataResult result = DataDescription::ProcessData();
	if (result == kDataOkay)
	{
		Structure *structure = GetRootStructure()->GetFirstSubnode();
		while (structure)
		{
			if (structure->GetBaseStructureType() == kStructureNode)
			{
				static_cast<NodeStructure *>(structure)->UpdateNodeTransforms(this);
			}

			structure = structure->Next();
		}
	}

	return (result);
}

void OpenGexDataDescription::AdjustTransform(Transform4D& transform) const
{
	transform.SetTranslation(transform.GetTranslation() * distanceScale);

	if (upDirection == 1)
	{
		transform.Set( transform(0,0), -transform(0,2),  transform(0,1),  transform(0,3),
					  -transform(2,0),  transform(2,2), -transform(2,1), -transform(2,3),
					   transform(1,0), -transform(1,2),  transform(1,1),  transform(1,3));
	}
}

NodeStructure *OpenGexDataDescription::FindNodeStructure(unsigned_int32 hash) const
{
	if (hash != 0)
	{
		const Structure *rootStructure = GetRootStructure();
		Structure *structure = rootStructure->GetFirstSubnode();
		while (structure)
		{
			if (structure->GetBaseStructureType() == kStructureNode)
			{
				NodeStructure *nodeStructure = static_cast<NodeStructure *>(structure);
				if (nodeStructure->GetNodeHash() == hash)
				{
					return (nodeStructure);
				}
			}

			structure = rootStructure->GetNextNode(structure);
		}
	}

	return (nullptr);
}

Range<float> OpenGexDataDescription::GetAnimationTimeRange(int32 clip) const
{
	Range<float>	timeRange;

	bool animationFlag = false;

	const AnimationStructure *animationStructure = animationList.First();
	while (animationStructure)
	{
		if (animationStructure->GetClipIndex() == clip)
		{
			if (animationFlag)
			{
				Range<float> range = animationStructure->GetAnimationTimeRange();
				timeRange.min = Fmin(timeRange.min, range.min);
				timeRange.max = Fmax(timeRange.max, range.max);
			}
			else
			{
				animationFlag = true;
				timeRange = animationStructure->GetAnimationTimeRange();
			}
		}

		animationStructure = animationStructure->Next();
	}

	if (animationFlag)
	{
		timeRange.min *= timeScale;
		timeRange.max *= timeScale;
		return (timeRange);
	}

	return (Range<float>(0.0F, 0.0F));
}

void OpenGexDataDescription::UpdateAnimation(int32 clip, float time) const
{
	time /= timeScale;

	const AnimationStructure *animationStructure = animationList.First();
	while (animationStructure)
	{
		if (animationStructure->GetClipIndex() == clip)
		{
			animationStructure->UpdateAnimation(this, time);
		}

		animationStructure = animationStructure->Next();
	}

	Structure *structure = GetRootStructure()->GetFirstSubnode();
	while (structure)
	{
		if (structure->GetBaseStructureType() == kStructureNode)
		{
			static_cast<NodeStructure *>(structure)->UpdateNodeTransforms(this);
		}

		structure = structure->Next();
	}
}


OpenGexImporter::OpenGexImporter() :
		Singleton<OpenGexImporter>(TheOpenGexImporter),
		stringTable("OpenGexImporter/strings")
{
}

OpenGexImporter::~OpenGexImporter()
{
}

const char *OpenGexImporter::GetPluginName(void) const
{
	return ("OpenGEX Import Tool");
}

const ResourceDescriptor *OpenGexImporter::GetImportResourceDescriptor(SceneImportType type) const
{
	return (OpenGexResource::GetDescriptor());
}

const char *OpenGexImporter::GetOpenGexResultString(DataResult result) const
{
	const char *string = Engine::GetDataResultString(result);
	if (StringTable::MissingString(string))
	{
		string = stringTable.GetString(StringID('OGEX', result));
	}

	return (string);
}

Node *OpenGexImporter::RemoveDeadNodes(Node *node)
{
	Node *subnode = node->GetFirstSubnode();
	while (subnode)
	{
		Node *next = subnode->Next();
		RemoveDeadNodes(subnode);
		subnode = next;
	}

	if ((node->GetNodeType() == kNodeGeneric) && (!node->GetFirstSubnode()))
	{
		delete node;
		node = nullptr;
	}

	return (node);
}

void OpenGexImporter::ImportGeometry(Editor *editor, const char *importName, const GeometryImportData *importData)
{
	OpenGexResource *resource = OpenGexResource::Get(importName, 0, ThePluginMgr->GetImportCatalog());
	if (resource)
	{
		OpenGexDataDescription openGexDataDescription(editor, importName, importData->importFlags, importData->importScale);

		DataResult result = openGexDataDescription.ProcessText(resource->GetText());
		if (result == kDataOkay)
		{
			Node *root = new Node;

			Structure *structure = openGexDataDescription.GetRootStructure()->GetFirstSubnode();
			while (structure)
			{
				if (structure->GetBaseStructureType() == kStructureNode)
				{
					Node *node = static_cast<NodeStructure *>(structure)->BuildNodeTree(&openGexDataDescription);
					root->AppendSubnode(node);
				}

				structure = structure->Next();
			}

			if (root->GetFirstSubnode())
			{
				root = OpenGexImporter::RemoveDeadNodes(root);
				if (root)
				{
					editor->ImportScene(root);
				}
			}

			delete root;
		}
		else
		{
			const char *title = stringTable.GetString(StringID('ERRR'));
			const char *message = stringTable.GetString(StringID('EMES'));

			String<> error(stringTable.GetString(StringID('LINE')));
			((error += openGexDataDescription.GetErrorLine()) += ": ") += TheOpenGexImporter->GetOpenGexResultString(result);

			ErrorDialog *dialog = new ErrorDialog(title, message, error, ResourcePath(importName) += OpenGexResource::GetDescriptor()->GetExtension());
			editor->AddSubwindow(dialog);
		}

		resource->Release();
	}
}

bool OpenGexImporter::GenerateAnimation(const OpenGexDataDescription *dataDescription, const char *resourceName, Model *model, const AnimationImportData *importData)
{
	Matrix3D		rotationMatrix;
	File			outputFile;
	ResourcePath	outputPath;

	// Prepare node data for transform track.

	int32 transformNodeCount = model->GetAnimatedTransformNodeCount();
	const Node *const *transformNodeTable = model->GetAnimatedTransformNodeTable();
	Array<AnimatedTransformNode> transformNodeArray(transformNodeCount);

	for (machine a = 0; a < transformNodeCount; a++)
	{
		const Node *modelNode = transformNodeTable[a];
		NodeStructure *nodeStructure = dataDescription->FindNodeStructure(modelNode->GetNodeHash());
		if ((nodeStructure) || (importData->importFlags & kAnimationImportPreserveMissing))
		{
			transformNodeArray.AddElement(AnimatedTransformNode(modelNode, nodeStructure));
		}
	}

	transformNodeCount = transformNodeArray.GetElementCount();
	int32 transformBucketCount = Min(Power2Ceil(Max(transformNodeCount, 1)), kMaxAnimationBucketCount);
	unsigned_int32 transformBucketMask = transformBucketCount - 1;

	Array<AnimatedTransformHash> *transformBucketTable = new Array<AnimatedTransformHash>[transformBucketCount];
	for (machine a = 0; a < transformNodeCount; a++)
	{
		unsigned_int32 hash = transformNodeArray[a].modelNode->GetNodeHash();
		transformBucketTable[hash & transformBucketMask].AddElement(AnimatedTransformHash(hash, a));
	}

	// Prepare node data for morph weight track.

	int32 morphWeightNodeCount = model->GetAnimatedMorphNodeCount();
	const Geometry *const *morphWeightNodeTable = model->GetAnimatedMorphNodeTable();
	Array<AnimatedMorphWeightNode> morphWeightNodeArray(morphWeightNodeCount);

	for (machine a = 0; a < morphWeightNodeCount; a++)
	{
		const Geometry *modelNode = morphWeightNodeTable[a];
		NodeStructure *nodeStructure = dataDescription->FindNodeStructure(modelNode->GetNodeHash());
		if ((nodeStructure) && (nodeStructure->GetStructureType() == kStructureGeometryNode))
		{
			morphWeightNodeArray.AddElement(AnimatedMorphWeightNode(modelNode, static_cast<GeometryNodeStructure *>(nodeStructure)));
		}
	}

	morphWeightNodeCount = morphWeightNodeArray.GetElementCount();
	int32 morphWeightBucketCount = Min(Power2Ceil(Max(morphWeightNodeCount, 1)), kMaxAnimationBucketCount);
	unsigned_int32 morphWeightBucketMask = morphWeightBucketCount - 1;

	int32 frameMorphWeightCount = 0;
	Array<AnimatedMorphWeightHash> *morphWeightBucketTable = new Array<AnimatedMorphWeightHash>[morphWeightBucketCount];
	for (machine a = 0; a < morphWeightNodeCount; a++)
	{
		const Geometry *modelNode = morphWeightNodeArray[a].modelNode;
		int32 weightCount = static_cast<MorphController *>(modelNode->GetController())->GetMorphWeightCount() + 1;

		unsigned_int32 hash = modelNode->GetNodeHash();
		morphWeightBucketTable[hash & morphWeightBucketMask].AddElement(AnimatedMorphWeightHash(hash, weightCount, frameMorphWeightCount));

		frameMorphWeightCount += weightCount;
	}

	// Generate animation header.

	float frameDuration = 33.0F;
	Range<float> timeRange = dataDescription->GetAnimationTimeRange(0);
	float duration = (timeRange.max - timeRange.min) * 1000.0F;
	int32 frameCount = (int32) (duration / frameDuration) + 1;

	int32 trackCount = 0;
	unsigned_int32 transformTrackHeaderSize = 0;
	unsigned_int32 transformTrackSize = 0;
	unsigned_int32 morphWeightTrackHeaderSize = 0;
	unsigned_int32 morphWeightTrackSize = 0;

	if (transformNodeCount != 0)
	{
		trackCount++;
		transformTrackHeaderSize = sizeof(TransformTrackHeader) + (transformBucketCount - 1) * sizeof(TransformTrackHeader::NodeBucket) + transformNodeCount * sizeof(TransformTrackHeader::NodeData);
		transformTrackSize = transformTrackHeaderSize + transformNodeCount * frameCount * sizeof(TransformFrameData);
	}

	if (morphWeightNodeCount != 0)
	{
		trackCount++;
		morphWeightTrackHeaderSize = sizeof(MorphWeightTrackHeader) + (morphWeightBucketCount - 1) * sizeof(MorphWeightTrackHeader::NodeBucket) + morphWeightNodeCount * sizeof(MorphWeightTrackHeader::NodeData);
		morphWeightTrackSize = morphWeightTrackHeaderSize + frameMorphWeightCount * 4 + frameMorphWeightCount * frameCount * 4;
	}

	unsigned_int32 animationHeaderSize = sizeof(AnimationHeader) + (trackCount - 1) * sizeof(AnimationHeader::TrackData);
	unsigned_int32 bufferSize = animationHeaderSize + transformTrackSize + morphWeightTrackSize;
	Buffer buffer(bufferSize);

	AnimationHeader *animationHeader = buffer.GetPtr<AnimationHeader>();
	animationHeader->frameCount = frameCount;
	animationHeader->frameDuration = frameDuration;
	animationHeader->trackCount = trackCount;

	trackCount = 0;
	unsigned_int32 trackOffset = animationHeaderSize;
	AnimationHeader::TrackData *trackData = animationHeader->trackData;

	// Generate transform track.

	if (transformNodeCount != 0)
	{
		trackData[trackCount].trackType = kTrackTransform;
		trackData[trackCount].trackOffset = trackOffset;

		TransformTrackHeader *transformTrackHeader = static_cast<TransformTrackHeader *>(const_cast<void *>(animationHeader->GetTrackHeader(trackCount)));
		transformTrackHeader->transformNodeCount = transformNodeCount;
		transformTrackHeader->transformFrameDataOffset = transformTrackHeaderSize;
		transformTrackHeader->bucketCount = transformBucketCount;

		unsigned_int32 bucketOffset = 0;
		TransformTrackHeader::NodeBucket *transformNodeBucket = transformTrackHeader->bucketData;
		TransformTrackHeader::NodeData *transformNodeData = const_cast<TransformTrackHeader::NodeData *>(transformTrackHeader->GetNodeData());

		for (machine a = 0; a < transformBucketCount; a++)
		{
			const Array<AnimatedTransformHash>& bucketArray = transformBucketTable[a];

			int32 bucketSize = bucketArray.GetElementCount();
			transformNodeBucket->bucketNodeCount = (unsigned_int16) bucketSize;
			transformNodeBucket->nodeDataOffset = (unsigned_int16) bucketOffset;

			const AnimatedTransformHash *transformHash = bucketArray;
			for (machine b = 0; b < bucketSize; b++)
			{
				transformNodeData->nodeHash = transformHash->nodeHash;
				transformNodeData->transformIndex = transformHash->transformIndex;

				transformNodeData++;
				transformHash++;
			}

			bucketOffset += bucketSize;
			transformNodeBucket++;
		}

		Point3D *positionTable = new Point3D[transformNodeCount];
		rotationMatrix.SetRotationAboutZ(importData->importRotation);

		for (machine frame = 0; frame < frameCount; frame++)
		{
			dataDescription->UpdateAnimation(0, timeRange.min + (float) (frame * frameDuration) * 0.001F);

			TransformFrameData *transformFrameData = const_cast<TransformFrameData *>(transformTrackHeader->GetTransformFrameData() + frame * transformNodeCount);
			for (machine a = 0; a < transformNodeCount; a++)
			{
				const AnimatedTransformNode *transformNode = &transformNodeArray[a];
				const Node *modelNode = transformNode->modelNode;
				const NodeStructure *nodeStructure = transformNode->nodeStructure;

				Transform4D transform = (nodeStructure) ? nodeStructure->CalculateFinalNodeTransform() : modelNode->GetNodeTransform();
				transformFrameData[a].transform = transform.GetMatrix3D();

				if (modelNode->GetSuperNode()->GetNodeType() != kNodeModel)
				{
					transformFrameData[a].position = transform.GetTranslation();
				}
				else
				{
					Point3D position = transform.GetTranslation();

					unsigned_int32 flags = importData->importFlags;
					if (flags & (kAnimationImportAnchorXY | kAnimationImportAnchorZ | kAnimationImportFreezeRoot))
					{
						if (frame != 0)
						{
							const Point3D& p = positionTable[a];
							if (flags & kAnimationImportFreezeRoot)
							{
								position -= p;
							}
							else
							{
								if (flags & kAnimationImportAnchorXY)
								{
									position.x = p.x;
									position.y = p.y;
								}

								if (flags & kAnimationImportAnchorZ)
								{
									position.z = p.z;
								}
							}
						}
						else
						{
							if (flags & kAnimationImportFreezeRoot)
							{
								positionTable[a] = position - modelNode->GetNodePosition();
								position = modelNode->GetNodePosition();
							}
							else
							{
								positionTable[a] = position;
							}
						}
					}

					if (modelNode->GetNodeType() == kNodeGeometry)
					{
						const Controller *controller = modelNode->GetController();
						if ((controller) && (controller->GetControllerType() == kControllerSkin))
						{
							transformFrameData[a].position = position;
							continue;
						}
					}

					transformFrameData[a].position = rotationMatrix * position;
					transformFrameData[a].transform = rotationMatrix * transformFrameData[a].transform;
				}
			}
		}

		delete[] positionTable;

		trackCount++;
		trackOffset += transformTrackSize;
	}

	// Generate morph weight track.

	if (morphWeightNodeCount != 0)
	{
		trackData[trackCount].trackType = kTrackMorphWeight;
		trackData[trackCount].trackOffset = trackOffset;

		MorphWeightTrackHeader *morphWeightTrackHeader = static_cast<MorphWeightTrackHeader *>(const_cast<void *>(animationHeader->GetTrackHeader(trackCount)));
		morphWeightTrackHeader->morphWeightNodeCount = morphWeightNodeCount;
		morphWeightTrackHeader->frameMorphWeightCount = frameMorphWeightCount;
		morphWeightTrackHeader->morphHashArrayOffset = morphWeightTrackHeaderSize;
		morphWeightTrackHeader->morphWeightFrameDataOffset = morphWeightTrackHeaderSize + frameMorphWeightCount * 4;
		morphWeightTrackHeader->bucketCount = morphWeightBucketCount;

		unsigned_int32 bucketOffset = 0;
		MorphWeightTrackHeader::NodeBucket *morphWeightNodeBucket = morphWeightTrackHeader->bucketData;
		MorphWeightTrackHeader::NodeData *morphWeightNodeData = const_cast<MorphWeightTrackHeader::NodeData *>(morphWeightTrackHeader->GetNodeData());

		for (machine a = 0; a < morphWeightBucketCount; a++)
		{
			const Array<AnimatedMorphWeightHash>& bucketArray = morphWeightBucketTable[a];

			int32 bucketSize = bucketArray.GetElementCount();
			morphWeightNodeBucket->bucketNodeCount = (unsigned_int16) bucketSize;
			morphWeightNodeBucket->nodeDataOffset = (unsigned_int16) bucketOffset;

			const AnimatedMorphWeightHash *morphWeightHash = bucketArray;
			for (machine b = 0; b < bucketSize; b++)
			{
				morphWeightNodeData->nodeHash = morphWeightHash->nodeHash;
				morphWeightNodeData->weightCount = morphWeightHash->weightCount;
				morphWeightNodeData->dataOffset = morphWeightHash->dataOffset;

				morphWeightNodeData++;
				morphWeightHash++;
			}

			bucketOffset += bucketSize;
			morphWeightNodeBucket++;
		}

		unsigned_int32 *morphHashArray = const_cast<unsigned_int32 *>(morphWeightTrackHeader->GetMorphHashArray());
		for (machine a = 0; a < morphWeightNodeCount; a++)
		{
			*morphHashArray++ = 0;

			const MorphController *controller = static_cast<MorphController *>(morphWeightNodeArray[a].modelNode->GetController());
			int32 weightCount = controller->GetMorphWeightCount();
			for (machine b = 0; b < weightCount; b++)
			{
				*morphHashArray++ = controller->GetMorphHash(b);
			}
		}

		float *morphWeightFrameData = const_cast<float *>(morphWeightTrackHeader->GetMorphWeightFrameData());
		for (machine frame = 0; frame < frameCount; frame++)
		{
			dataDescription->UpdateAnimation(0, timeRange.min + (float) (frame * frameDuration) * 0.001F);

			for (machine a = 0; a < morphWeightNodeCount; a++)
			{
				const AnimatedMorphWeightNode *morphWeightNode = &morphWeightNodeArray[a];
				const GeometryNodeStructure *geometryNodeStructure = morphWeightNode->nodeStructure;
				const GeometryObjectStructure *geometryObjectStructure = geometryNodeStructure->GetGeometryObjectStructure();

				const MorphWeightStructure *morphWeightStructure = geometryNodeStructure->FindMorphWeightStructure(0);
				*morphWeightFrameData++ = (morphWeightStructure) ? morphWeightStructure->GetMorphWeight() : 0.0F;

				const MorphController *controller = static_cast<MorphController *>(morphWeightNode->modelNode->GetController());
				int32 weightCount = controller->GetMorphWeightCount();
				for (machine b = 0; b < weightCount; b++)
				{
					float weight = 0.0F;

					const MorphStructure *morphStructure = geometryObjectStructure->FindMorphStructure(controller->GetMorphHash(b));
					if (morphStructure)
					{
						morphWeightStructure = geometryNodeStructure->FindMorphWeightStructure(morphStructure->GetMorphIndex());
						if (morphWeightStructure)
						{
							weight = morphWeightStructure->GetMorphWeight();
						}
					}

					*morphWeightFrameData++ = weight;
				}
			}
		}

		trackCount++;
		trackOffset += morphWeightTrackSize;
	}

	delete[] morphWeightBucketTable;
	delete[] transformBucketTable;

	// Write animation resource.

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(AnimationResource::GetDescriptor(), resourceName, &outputPath);
	TheResourceMgr->CreateDirectoryPath(outputPath);

	if (outputFile.Open(outputPath, kFileCreate) == kFileOkay)
	{
		outputFile.Write(AnimationResource::resourceSignature, 8);
		outputFile.Write(buffer, bufferSize);
		return (true);
	}

	return (false);
}

bool OpenGexImporter::ImportAnimation(Window *window, const char *importName, const char *resourceName, Model *model, const AnimationImportData *importData)
{
	bool success = false;

	OpenGexResource *resource = OpenGexResource::Get(importName, 0, ThePluginMgr->GetImportCatalog());
	if (resource)
	{
		OpenGexDataDescription openGexDataDescription(nullptr, importName, 0, 1.0F);

		DataResult result = openGexDataDescription.ProcessText(resource->GetText());
		if (result == kDataOkay)
		{
			if (!openGexDataDescription.GetAnimationList()->Empty())
			{
				success = GenerateAnimation(&openGexDataDescription, resourceName, model, importData);
			}
		}
		else
		{
			const char *title = stringTable.GetString(StringID('ERRR'));
			const char *message = stringTable.GetString(StringID('EMES'));

			String<> error(stringTable.GetString(StringID('LINE')));
			((error += openGexDataDescription.GetErrorLine()) += ": ") += GetOpenGexResultString(result);

			ErrorDialog *dialog = new ErrorDialog(title, message, error, ResourcePath(importName) += OpenGexResource::GetDescriptor()->GetExtension());
			window->AddSubwindow(dialog);
		}

		resource->Release();
	}

	return (success);
}

// ZYUQURM
