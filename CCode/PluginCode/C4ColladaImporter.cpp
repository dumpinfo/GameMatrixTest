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


#include "C4ColladaImporter.h"
#include "C4TextureImporter.h"
#include "C4WorldEditor.h"
#include "C4EditorSupport.h"


using namespace C4;


ColladaImporter *C4::TheColladaImporter = nullptr;


ResourceDescriptor ColladaResource::descriptor("dae", kResourceTerminatorByte);


C4::Plugin *CreatePlugin(void)
{
	return (new ColladaImporter);
}


static bool ReadWhitespace(const char *& text)
{
	bool b = false;
	for (;;)
	{
		char c = *text;
		if (c == 0)
		{
			break;
		}

		if (c <= 32)
		{
			text++;
			b = true;
			continue;
		}
		else if (c == '<')
		{
			if (text[1] == '!')
			{
				if (Text::CompareText(text + 2, "--", 2))
				{
					text += 4;
					b = true;

					for (;; text++)
					{
						if (*text == 0)
						{
							return (true);
						}

						if (Text::CompareText(text, "-->", 3))
						{
							text += 3;
							break;
						}
					}

					continue;
				}
				else if (Text::CompareText(text + 2, "[CDATA[", 7))
				{
					text += 9;
					b = true;

					for (;; text++)
					{
						if (*text == 0)
						{
							return (true);
						}

						if (Text::CompareText(text, "]]>", 3))
						{
							text += 3;
							break;
						}
					}

					continue;
				}
			}
		}

		break;
	}

	return (b);
}

static bool FindTag(const char *& text)
{
	bool quote = false;
	for (;;)
	{
		ReadWhitespace(text);

		char c = *text; 
		if (c == 0)
		{
			break; 
		}
 
		text++;

		if ((!quote) && (c == '<')) 
		{
			ReadWhitespace(text); 
			return (true); 
		}

		if (c == 34)
		{ 
			quote = !quote;
		}
	}

	return (false);
}

static bool SkipTag(const char *& text)
{
	bool quote = false;
	for (;;)
	{
		char c = *text;
		if (c == 0)
		{
			break;
		}

		text++;

		if ((!quote) && (c == '>'))
		{
			if (text[-2] == '/')
			{
				return (true);
			}

			break;
		}

		if (c == 34)
		{
			quote = !quote;
		}
	}

	return (false);
}

static bool ReadIdentifier(const char *& text, String<Collada::kMaxIdentifierLength> *identifier)
{
	int32	length;

	if ((Data::ReadIdentifier(text, &length) == kDataOkay) && (length <= Collada::kMaxIdentifierLength))
	{
		Data::ReadIdentifier(text, &length, *identifier);
		text += length;
		return (true);
	}

	return (false);
}

static bool ReadAttribute(const char *& text, String<Collada::kMaxIdentifierLength> *identifier)
{
	int32 length = 0;
	bool quote = false;
	const char *start = text;

	for (;;)
	{
		unsigned_int32 c = *text;
		if (c == 34)
		{
			text++;

			if ((length == 0) && (!quote))
			{
				quote = true;
				start = text;
				continue;
			}
			else if (quote)
			{
				break;
			}
		}

		if (c == 0)
		{
			break;
		}

		if ((!quote) && (c < 33))
		{
			break;
		}

		length++;
		text++;
	}

	if (length != 0)
	{
		identifier->Set(start, length);
		return (true);
	}

	return (false);
}

static bool ReadString(const char *& text, String<Collada::kMaxStringLength> *string)
{
	int32 length = 0;
	for (;; length++)
	{
		unsigned_int32 c = text[length];
		if ((c < 33) || (c == '<'))
		{
			break;
		}
	}

	if (length != 0)
	{
		string->Set(text, length);
		text += length;
		return (true);
	}

	return (false);
}

static bool ReadContent(const char *& text, String<Collada::kMaxStringLength> *string)
{
	int32 length = 0;
	for (;; length++)
	{
		unsigned_int32 c = text[length];
		if ((c < 32) || (c == '<'))
		{
			break;
		}
	}

	if (length != 0)
	{
		string->Set(text, length);
		text += length;
		return (true);
	}

	(*string)[0] = 0;
	return (false);
}

static int32 ReadInteger(const char *& text)
{
	int32 value = 0;

	bool negative = (*text == '-');
	text += negative;

	for (;;)
	{
		unsigned_int32 x = *text - '0';

		if (x < 10U)
		{
			value = value * 10 + x;
		}
		else
		{
			break;
		}

		text++;
	}

	return ((negative) ? -value : value);
}


Collada::Element::Element(Collada::ElementType type, ColladaContext *context)
{
	elementType = type;
	colladaContext = context;

	identifier[0] = 0;
	subIdentifier[0] = 0;
}

Collada::Element::~Element()
{
}

Collada::Scene *Collada::Element::GetOwningScene(void) const
{
	Collada::Element *element = GetSuperNode();
	while ((element) && (element->GetElementType() != Collada::kElementScene))
	{
		element = element->GetSuperNode();
	}

	return (static_cast<Collada::Scene *>(element));
}

Collada::Element *Collada::Element::FindElement(const char *id, Collada::Element *root)
{
	int32 c = id[0];
	if (c == 0)
	{
		return (nullptr);
	}

	if (c == '#')
	{
		id++;
	}

	if (id[0] == 0)
	{
		return (nullptr);
	}

	if (!root)
	{
		root = GetRootNode();
	}

	Collada::Element *element = root;
	do
	{
		if (element->GetIdentifier() == id)
		{
			return (element);
		}

		element = root->GetNextNode(element);
	} while (element);

	// The call below looks for a matching name attribute if a matching id attribute wasn't found.
	// This is necessary to deal with buggy Collada exporters that don't identify elements correctly.

	return (FindNamedElement(id, root));
}

Collada::Element *Collada::Element::FindSubElement(const char *id, Collada::Element *root)
{
	int32 c = id[0];
	if (c == 0)
	{
		return (nullptr);
	}

	if (c == '#')
	{
		id++;
	}

	if (id[0] == 0)
	{
		return (nullptr);
	}

	if (!root)
	{
		root = GetRootNode();
	}

	Collada::Element *element = root;
	do
	{
		if (element->GetSubIdentifier() == id)
		{
			break;
		}

		element = root->GetNextNode(element);
	} while (element);

	return (element);
}

Collada::Node *Collada::Element::FindNamedElement(const char *name, Collada::Element *root)
{
	if (!root)
	{
		root = GetRootNode();
	}

	Collada::Element *element = root;
	do
	{
		if (element->GetElementType() == Collada::kElementNode)
		{
			Collada::Node *node = static_cast<Collada::Node *>(element);
			if (node->GetNodeName() == name)
			{
				return (node);
			}
		}

		element = root->GetNextNode(element);
	} while (element);

	return (nullptr);
}

void Collada::Element::Preprocess(void)
{
	Collada::Element *subelement = GetFirstSubnode();
	while (subelement)
	{
		subelement->Preprocess();
		subelement = subelement->Next();
	}
}

void Collada::Element::ProcessContent(const char *& text)
{
}


Collada::TargetingElement::TargetingElement(Collada::ElementType type) : Collada::Element(type)
{
	targetElement = nullptr;
	targetSubElement = nullptr;

	targetComponent = -1;

	targetIdentifier[0] = 0;
}

Collada::TargetingElement::~TargetingElement()
{
}

void Collada::TargetingElement::Preprocess(void)
{
	Collada::Element::Preprocess();

	if (targetIdentifier[0] != 0)
	{
		int32 i = Text::FindChar(targetIdentifier, '/');
		if (i < 1)
		{
			targetElement = FindElement(targetIdentifier);
		}
		else
		{
			targetElement = FindElement(String<kMaxIdentifierLength>(targetIdentifier, i));

			const char *subIdentifier = &targetIdentifier[i + 1];
			i = Text::FindChar(subIdentifier, '.');
			if (i < 1)
			{
				i = Text::FindChar(subIdentifier, '(');
				if (i < 1)
				{
					targetSubElement = FindSubElement(subIdentifier, targetElement);
				}
				else
				{
					targetSubElement = FindSubElement(String<kMaxIdentifierLength>(subIdentifier, i), targetElement);

					const char *text = &subIdentifier[i + 1];
					int32 n = ReadInteger(text);

					if ((text[0] == ')') && (text[1] == '('))
					{
						int32 m = ReadInteger(text += 2);
						targetComponent = m * 4 + n;
					}
					else
					{
						targetComponent = n;
					}
				}
			}
			else
			{
				targetSubElement = FindSubElement(String<kMaxIdentifierLength>(subIdentifier, i), targetElement);

				String<kMaxIdentifierLength> comp(&subIdentifier[i + 1]);

				if ((comp == "X") || (comp == "S") || (comp == "R") || (comp == "U"))
				{
					targetComponent = 0;
				}
				else if ((comp == "Y") || (comp == "T") || (comp == "G") || (comp == "V"))
				{
					targetComponent = 1;
				}
				else if ((comp == "Z") || (comp == "P") || (comp == "B"))
				{
					targetComponent = 2;
				}
				else if ((comp == "W") || (comp == "Q") || (comp == "A") || (comp == "ANGLE"))
				{
					targetComponent = 3;
				}
			}
		}
	}
}


Collada::InterleavingElement::InterleavingElement(Collada::ElementType type) : Element(type)
{
	itemCount = 0;
	inputCount = 0;
	countArray = nullptr;
}

Collada::InterleavingElement::~InterleavingElement()
{
}

void Collada::InterleavingElement::Preprocess(void)
{
	List<Collada::Input>	inputList;

	Collada::Element::Preprocess();

	int32 count = 0;
	int32 texcoordBase = kMaxGeometryTexcoordCount;

	Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementCountArray)
		{
			countArray = static_cast<const Collada::CountArray *>(element);
		}
		else if (type == Collada::kElementInput)
		{
			Collada::Input *input = static_cast<Collada::Input *>(element);
			int32 index = input->GetInputIndex();

			const Collada::Input *prev = inputList.First();
			while (prev)
			{
				if (prev->GetInputIndex() == index)
				{
					goto next;
				}

				prev = prev->ListElement<Collada::Input>::Next();
			}

			count = Max(count, index + 1);
			inputList.Append(input);

			if (input->GetInputSemantic() == Collada::kSemanticTexcoord)
			{
				texcoordBase = Min(texcoordBase, input->GetTexcoordSet());
			}
		}
		else
		{
			break;
		}

		next:
		element = element->Next();
	}

	inputCount = count;
	for (;;)
	{
		Collada::Input *input = inputList.First();
		if (!input)
		{
			break;
		}

		if (input->GetInputSemantic() == Collada::kSemanticTexcoord)
		{
			input->SetTexcoordSet(input->GetTexcoordSet() - texcoordBase);
		}

		ProcessInput(input->GetInputIndex(), input);
		inputList.Remove(input);
	}
}


Collada::MaterialAttributeElement::MaterialAttributeElement(Collada::ElementType type, ColladaContext *context) : Element(type, context)
{
}

Collada::MaterialAttributeElement::~MaterialAttributeElement()
{
}


Collada::Asset::Asset() : Collada::Element(Collada::kElementAsset)
{
}

Collada::Asset::~Asset()
{
}


Collada::Technique::Technique(TechniqueType type) : Collada::Element(Collada::kElementTechnique)
{
	techniqueType = type;
}

Collada::Technique::~Technique()
{
}


Collada::Mesh::Mesh() : Collada::Element(Collada::kElementMesh)
{
}

Collada::Mesh::~Mesh()
{
}

GenericGeometry *Collada::Mesh::BuildGeometry(Collada::Scene *scene, const List<Collada::BindMaterial> *materialBindingList, Collada::Skin *skin) const
{
	List<GeometrySurface>				surfaceList;
	Array<C4::MaterialObject *, 4>		materialArray;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementFaces)
		{
			const Collada::Faces *faces = static_cast<const Collada::Faces *>(element);
			GeometrySurface *surface = faces->BuildSurface(scene, skin);
			if (surface)
			{
				surfaceList.Append(surface);

				C4::MaterialObject *materialObject = nullptr;
				if (!materialBindingList->Empty())
				{
					const char *identifier = faces->GetMaterialIdentifier();

					const Collada::BindMaterial *binding = materialBindingList->First();
					while (binding)
					{
						const Collada::Instance *instance = binding->GetFirstInstance();
						while (instance)
						{
							if (instance->GetSymbolIdentifier() == identifier)
							{
								Collada::Element *material = instance->GetResourceElement();
								if ((material) && (material->GetElementType() == Collada::kElementMaterial))
								{
									materialObject = static_cast<Collada::Material *>(material)->GetMaterialObject();
								}

								goto found;
							}

							instance = instance->ListElement<Collada::Instance>::Next();
						}

						binding = binding->ListElement<Collada::BindMaterial>::Next();
					}

					found:;
				}
				else
				{
					Collada::Element *material = faces->GetMaterialElement();
					if ((material) && (material->GetElementType() == Collada::kElementMaterial))
					{
						materialObject = static_cast<Collada::Material *>(material)->GetMaterialObject();
					}
				}

				materialArray.AddElement(materialObject);
			}
		}

		element = element->Next();
	}

	if (!surfaceList.Empty())
	{
		const List<GeometrySurface> *surfaceTable = &surfaceList;
		const C4::SkinData *skinData = (skin) ? skin->GetSkinData(scene) : nullptr;

		if ((!skin) || (skinData))
		{
			GenericGeometry *geometry = new GenericGeometry(1, &surfaceTable, materialArray, &skinData);

			if (skinData)
			{
				geometry->SetNodeFlags(kNodeAnimateInhibit);

				GeometryObject *object = geometry->GetObject();
				object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDeformModelFlags);
				object->SetCollisionExclusionMask(kCollisionExcludeAll);
			}

			return (geometry);
		}
	}

	return (nullptr);
}


Collada::Geometry::Geometry() : Collada::Element(Collada::kElementGeometry)
{
	geometryName[0] = 0;
}

Collada::Geometry::~Geometry()
{
}

GenericGeometry *Collada::Geometry::BuildGeometry(Collada::Scene *scene, const List<Collada::BindMaterial> *materialBindingList, Collada::Skin *skin) const
{
	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementMesh)
		{
			GenericGeometry *geometry = static_cast<const Collada::Mesh *>(element)->BuildGeometry(scene, materialBindingList, skin);
			if (geometry)
			{
				return (geometry);
			}
		}

		element = element->Next();
	}

	return (nullptr);
}

PathMarker *Collada::Geometry::BuildPath(void) const
{
	PathMarker *pathMarker = new PathMarker(K::z_unit);
	Path *path = pathMarker->GetPath();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementSpline)
		{
			static_cast<const Collada::Spline *>(element)->BuildPathComponents(path);
		}

		element = element->Next();
	}

	if (path->GetFirstPathComponent())
	{
		return (pathMarker);
	}

	delete pathMarker;
	return (nullptr);
}


Collada::Controller::Controller() : Collada::TargetingElement(Collada::kElementController)
{
}

Collada::Controller::~Controller()
{
}


Collada::NameArray::NameArray() : Collada::Element(Collada::kElementNameArray)
{
	nameCount = 0;
	arraySize = 0;
	nameArray = nullptr;
}

Collada::NameArray::~NameArray()
{
	delete[] nameArray;
}

void Collada::NameArray::SetArraySize(int32 size)
{
	nameCount = 0;
	arraySize = size;
	delete[] nameArray;
	nameArray = new String<Collada::kMaxIdentifierLength>[size];
}

void Collada::NameArray::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	int32 count = 0;
	for (; count < arraySize; count++)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			break;
		}

		nameArray[count] = string;
	}

	nameCount = count;
}


Collada::FloatArray::FloatArray() : Collada::Element(Collada::kElementFloatArray)
{
	floatCount = 0;
	arraySize = 0;
	floatArray = nullptr;
}

Collada::FloatArray::~FloatArray()
{
	delete[] floatArray;
}

void Collada::FloatArray::SetArraySize(int32 size)
{
	floatCount = 0;
	arraySize = size;
	delete[] floatArray;
	floatArray = new float[size];
}

void Collada::FloatArray::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	int32 count = 0;
	for (; count < arraySize; count++)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			break;
		}

		floatArray[count] = Text::StringToFloat(string);
	}

	floatCount = count;
}


Collada::Accessor::Accessor() : Collada::Element(Collada::kElementAccessor)
{
	accessCount = 0;
	accessStride = 1;

	sourceElement = nullptr;
	sourceIdentifier[0] = 0;
}

Collada::Accessor::~Accessor()
{
}

void Collada::Accessor::Preprocess(void)
{
	Collada::Element::Preprocess();

	Collada::Element *root = GetSuperNode();
	if (root)
	{
		root = root->GetSuperNode();
	}

	sourceElement = FindElement(sourceIdentifier, root);
}

const char *Collada::Accessor::GetNameValue(int32 index) const
{
	if ((unsigned_int32) index < (unsigned_int32) accessCount)
	{
		Collada::Element *source = sourceElement;
		if ((source) && (source->GetElementType() == Collada::kElementNameArray))
		{
			const Collada::NameArray *array = static_cast<Collada::NameArray *>(source);
			return (*(array->GetArray() + index * accessStride));
		}
	}

	return (nullptr);
}

const float *Collada::Accessor::GetFloatValue(int32 index) const
{
	if ((unsigned_int32) index < (unsigned_int32) accessCount)
	{
		Collada::Element *source = sourceElement;
		if ((source) && (source->GetElementType() == Collada::kElementFloatArray))
		{
			const Collada::FloatArray *array = static_cast<Collada::FloatArray *>(source);
			return (array->GetArray() + index * accessStride);
		}
	}

	return (nullptr);
}


Collada::Source::Source() : Collada::Element(Collada::kElementSource)
{
	newParamElement = nullptr;
}

Collada::Source::~Source()
{
	accessorList.RemoveAll();
}

void Collada::Source::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementTechnique)
		{
			Collada::Element *subelement = element->GetFirstSubnode();
			while (subelement)
			{
				if (subelement->GetElementType() == Collada::kElementAccessor)
				{
					accessorList.Append(static_cast<Collada::Accessor *>(subelement));
				}

				subelement = subelement->Next();
			}
		}

		element = element->Next();
	}

	if (paramIdentifier[0] != 0)
	{
		Collada::Element *super = GetSuperNode();
		if (super)
		{
			super = super->GetSuperNode();
			if (super)
			{
				super = super->GetSuperNode();
				if (super)
				{
					Collada::Element *subelement = FindSubElement(paramIdentifier, super);
					if ((subelement) && (subelement->GetElementType() == Collada::kElementNewParam))
					{
						newParamElement = static_cast<Collada::NewParam *>(subelement);
					}
				}
			}
		}
	}
}

void Collada::Source::ProcessContent(const char *& text)
{
	ReadWhitespace(text);
	if (!ReadString(text, &paramIdentifier))
	{
		paramIdentifier[0] = 0;
	}
}


Collada::Input::Input() : Collada::Element(Collada::kElementInput)
{
	inputIndex = 0;
	inputSemantic = 0;
	texcoordSet = 0;
	sourceElement = nullptr;
	sourceIdentifier[0] = 0;
}

Collada::Input::~Input()
{
}

void Collada::Input::Preprocess(void)
{
	Collada::Element::Preprocess();

	Collada::Element *root = GetSuperNode();
	if (root)
	{
		root = root->GetSuperNode();
	}

	sourceElement = FindElement(sourceIdentifier, root);
	if (!sourceElement)
	{
		sourceElement = FindElement(sourceIdentifier);
	}
}


Collada::Sampler::Sampler() : Collada::Element(Collada::kElementSampler)
{
	inputAccessor = nullptr;
	outputAccessor = nullptr;
	interpolationAccessor = nullptr;
	inTangentAccessor = nullptr;
	outTangentAccessor = nullptr;
}

Collada::Sampler::~Sampler()
{
}

void Collada::Sampler::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementInput)
		{
			const Collada::Input *input = static_cast<const Collada::Input *>(element);
			const Collada::Element *source = input->GetSourceElement();
			if ((source) && (source->GetElementType() == Collada::kElementSource))
			{
				const Collada::Accessor *accessor = static_cast<const Collada::Source *>(source)->GetFirstAccessor();
				if (accessor)
				{
					switch (input->GetInputSemantic())
					{
						case Collada::kSemanticInput:

							inputAccessor = accessor;
							break;

						case Collada::kSemanticOutput:

							outputAccessor = accessor;
							break;

						case Collada::kSemanticInterpolation:

							interpolationAccessor = accessor;
							break;

						case Collada::kSemanticInTangent:

							inTangentAccessor = accessor;
							break;

						case Collada::kSemanticOutTangent:

							outTangentAccessor = accessor;
							break;
					}
				}
			}
		}

		element = element->Next();
	}

	beginTime = 0.0F;
	endTime = 0.0F;

	if (inputAccessor)
	{
		int32 count = inputAccessor->GetAccessCount();
		if (count > 0)
		{
			beginTime = *inputAccessor->GetFloatValue(0);
			endTime = *inputAccessor->GetFloatValue(inputAccessor->GetAccessCount() - 1);
		}
	}
}

bool Collada::Sampler::CalculateValue(float time, float *value, ElementType type) const
{
	if ((inputAccessor) && (outputAccessor))
	{
		int32 timeCount = inputAccessor->GetAccessCount();
		if ((timeCount > 0) && (outputAccessor->GetAccessCount() > 0))
		{
			int32 compCount = Min(outputAccessor->GetAccessStride(), 32);

			if (time <= beginTime)
			{
				const float *beginValue = outputAccessor->GetFloatValue(0);
				for (machine comp = 0; comp < compCount; comp++)
				{
					value[comp] = beginValue[comp];
				}
			}
			else if (time >= endTime)
			{
				const float *endValue = outputAccessor->GetFloatValue(timeCount - 1);
				for (machine comp = 0; comp < compCount; comp++)
				{
					value[comp] = endValue[comp];
				}
			}
			else
			{
				float t1 = beginTime;
				for (machine i = 1; i < timeCount; i++)
				{
					float t2 = *inputAccessor->GetFloatValue(i);
					if (time < t2)
					{
						float	adjustedValue[4];

						const float *v1 = outputAccessor->GetFloatValue(i - 1);
						const float *v2 = outputAccessor->GetFloatValue(i);
						float t = (time - t1) / (t2 - t1);

						if ((type == Collada::kElementRotate) && ((compCount == 1) || (compCount == 4)))
						{
							float r1 = v1[compCount - 1];
							float r2 = v2[compCount - 1];

							if (Fabs(r2 - r1) > 180.0F)
							{
								if (r1 < r2)
								{
									r2 -= 360.0F;
								}
								else
								{
									r2 += 360.0F;
								}

								adjustedValue[compCount - 1] = r2;
								for (machine a = 0; a < compCount - 1; a++)
								{
									adjustedValue[a] = v2[a];
								}

								v2 = adjustedValue;
							}
						}

						float s = 1.0F - t;

						if ((interpolationAccessor) && (Text::CompareText(interpolationAccessor->GetNameValue(i), "BEZIER")) && (inTangentAccessor) && (outTangentAccessor))
						{
							const float *c1 = outTangentAccessor->GetFloatValue(i - 1);
							const float *c2 = inTangentAccessor->GetFloatValue(i);

							int32 m1 = outTangentAccessor->GetAccessStride() / outputAccessor->GetAccessStride();
							int32 m2 = inTangentAccessor->GetAccessStride() / outputAccessor->GetAccessStride();

							float u1 = s * s * s;
							float u2 = t * s * s * 3.0F;
							float u3 = t * t * s * 3.0F;
							float u4 = t * t * t;

							for (machine comp = 0; comp < compCount; comp++)
							{
								value[comp] = v1[comp] * u1 + c1[comp * m1 + (m1 - 1)] * u2 + c2[comp * m2 + (m2 - 1)] * u3 + v2[comp] * u4;
							}
						}
						else
						{
							for (machine comp = 0; comp < compCount; comp++)
							{
								value[comp] = v1[comp] * s + v2[comp] * t;
							}
						}

						break;
					}

					t1 = t2;
				}
			}

			return (true);
		}
	}

	return (false);
}


Collada::Channel::Channel() : Collada::TargetingElement(Collada::kElementChannel)
{
}

Collada::Channel::~Channel()
{
}

void Collada::Channel::Preprocess(void)
{
	Collada::TargetingElement::Preprocess();

	sourceElement = FindElement(sourceIdentifier, GetSuperNode());
}

void Collada::Channel::SetTargetValue(float time) const
{
	float	value[32];

	Collada::Element *target = GetTargetSubElement();
	if (target)
	{
		Collada::ElementType targetType = target->GetElementType();

		const Collada::Sampler *sampler = static_cast<Collada::Sampler *>(sourceElement);
		if ((sampler) && (sampler->CalculateValue(time, value, targetType)))
		{
			int32 component = GetTargetComponent();

			switch (targetType)
			{
				case Collada::kElementMatrix:
				{
					Collada::Matrix *matrix = static_cast<Collada::Matrix *>(target);
					if (component == -1)
					{
						matrix->SetTransformMatrix(Transform4D(value[0], value[1], value[2], value[3],
															   value[4], value[5], value[6], value[7],
															   value[8], value[9], value[10], value[11]));
					}
					else if (component < 12)
					{
						Transform4D transform = matrix->GetTransformMatrix();
						transform(component / 4, component & 3) = value[0];
						matrix->SetTransformMatrix(transform);
					}

					break;
				}

				case Collada::kElementTranslate:
				{
					Collada::Translate *translate = static_cast<Collada::Translate *>(target);
					if (component == -1)
					{
						translate->SetTranslatePosition(Point3D(value[0], value[1], value[2]));
					}
					else
					{
						Point3D p = translate->GetTranslatePosition();
						p[component] = value[0];
						translate->SetTranslatePosition(p);
					}

					break;
				}

				case Collada::kElementRotate:
				{
					Collada::Rotate *rotate = static_cast<Collada::Rotate *>(target);
					if (component == -1)
					{
						rotate->SetRotateAxis(Vector3D(value[0], value[1], value[2]));
						rotate->SetRotateAngle(value[3]);
					}
					else if (component < 3)
					{
						Vector3D v = rotate->GetRotateAxis();
						v[component] = value[0];
						rotate->SetRotateAxis(v);
					}
					else
					{
						rotate->SetRotateAngle(value[0]);
					}

					break;
				}

				case Collada::kElementScale:
				{
					Collada::Scale *scale = static_cast<Collada::Scale *>(target);
					if (component == -1)
					{
						scale->SetScaleFactor(Vector3D(value[0], value[1], value[2]));
					}
					else
					{
						Vector3D s = scale->GetScaleFactor();
						s[component] = value[0];
						scale->SetScaleFactor(s);
					}

					break;
				}
			}
		}
	}
}


Collada::Animation::Animation() : Collada::Element(Collada::kElementAnimation)
{
}

Collada::Animation::~Animation()
{
}

void Collada::Animation::Preprocess(void)
{
	Collada::Element::Preprocess();

	float t1 = K::infinity;
	float t2 = K::minus_infinity;

	Collada::Element *element = Tree<Collada::Element>::GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementAnimation)
		{
			Collada::Animation *animation = static_cast<Collada::Animation *>(element);
			Tree<Collada::Animation>::AppendSubnode(animation);

			t1 = Fmin(t1, animation->GetBeginTime());
			t2 = Fmax(t2, animation->GetEndTime());
		}
		else if (type == Collada::kElementChannel)
		{
			channelList.Append(static_cast<Collada::Channel *>(element));
		}
		else if (type == Collada::kElementSampler)
		{
			const Collada::Sampler *sampler = static_cast<Collada::Sampler *>(element);
			t1 = Fmin(t1, sampler->GetBeginTime());
			t2 = Fmax(t2, sampler->GetEndTime());
		}

		element = element->Next();
	}

	beginTime = t1;
	endTime = t2;
}

void Collada::Animation::SetAnimationTransforms(float time) const
{
	const Collada::Channel *channel = channelList.First();
	while (channel)
	{
		channel->SetTargetValue(time);
		channel = channel->ListElement<Collada::Channel>::Next();
	}

	const Collada::Animation *animation = Tree<Collada::Animation>::GetFirstSubnode();
	while (animation)
	{
		animation->SetAnimationTransforms(time);
		animation = animation->Tree<Collada::Animation>::Next();
	}
}


Collada::Library::Library(LibraryType type) : Collada::Element(Collada::kElementLibrary)
{
	libraryType = type;
}

Collada::Library::~Library()
{
}

void Collada::Library::Preprocess(void)
{
	Collada::Element::Preprocess();

	if (libraryType == Collada::kLibraryAnimation)
	{
		float t1 = K::infinity;
		float t2 = K::minus_infinity;

		Collada::Element *element = GetFirstSubnode();
		while (element)
		{
			Collada::ElementType type = element->GetElementType();
			if (type == Collada::kElementAnimation)
			{
				Collada::Animation *animation = static_cast<Collada::Animation *>(element);
				animationList.Append(animation);

				t1 = Fmin(t1, animation->GetBeginTime());
				t2 = Fmax(t2, animation->GetEndTime());
			}

			element = element->Next();
		}

		beginTime = t1;
		endTime = t2;
	}
}

void Collada::Library::SetAnimationTransforms(float time) const
{
	const Collada::Animation *animation = animationList.First();
	while (animation)
	{
		animation->SetAnimationTransforms(time);
		animation = animation->ListElement<Collada::Animation>::Next();
	}
}


Collada::IndexArray::IndexArray() : Collada::Element(Collada::kElementIndexArray), indexArray(256)
{
}

Collada::IndexArray::~IndexArray()
{
}

void Collada::IndexArray::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	for (;;)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			break;
		}

		AddIndex(Text::StringToInteger(string));
	}
}


Collada::CountArray::CountArray() : Collada::Element(Collada::kElementCountArray), countArray(256)
{
}

Collada::CountArray::~CountArray()
{
}

void Collada::CountArray::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	for (;;)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			break;
		}

		countArray.AddElement(Text::StringToInteger(string));
	}
}


Collada::Joints::Joints() : Collada::Element(Collada::kElementJoints)
{
}

Collada::Joints::~Joints()
{
}


Collada::Combiner::Combiner() : Collada::InterleavingElement(Collada::kElementCombiner)
{
	jointAccessor = nullptr;
	weightAccessor = nullptr;
}

Collada::Combiner::~Combiner()
{
}

void Collada::Combiner::ProcessInput(int32 index, const Collada::Input *input)
{
	const Collada::Element *source = input->GetSourceElement();
	if (source)
	{
		const Collada::Accessor *accessor = nullptr;

		Collada::ElementType type = source->GetElementType();
		if (type == Collada::kElementAccessor)
		{
			accessor = static_cast<const Collada::Accessor *>(source);
		}
		else if (type == Collada::kElementSource)
		{
			accessor = static_cast<const Collada::Source *>(source)->GetFirstAccessor();
		}

		if (accessor)
		{
			Collada::SemanticType semantic = input->GetInputSemantic();
			if (semantic == Collada::kSemanticJoint)
			{
				jointIndex = index;
				jointAccessor = accessor;
			}
			else if (semantic == Collada::kSemanticWeight)
			{
				weightIndex = index;
				weightAccessor = accessor;
			}
		}
	}
}

void Collada::Combiner::Preprocess(void)
{
	Collada::InterleavingElement::Preprocess();

	if ((jointAccessor) && (weightAccessor))
	{
		int32 inputCount = GetInputCount();
		int32 vertexCount = 0;
		unsigned_int32 skinSize = 0;

		const Collada::CountArray *countArray = GetCountArray();
		if (countArray)
		{
			const Collada::Element *element = GetFirstSubnode();
			while (element)
			{
				if (element->GetElementType() == Collada::kElementIndexArray)
				{
					vertexCount = GetItemCount();
					skinSize = vertexCount;

					const Collada::IndexArray *indexArray = static_cast<const Collada::IndexArray *>(element);
					const int32 *weightCount = countArray->GetArray();

					int32 start = 0;
					for (machine a = 0; a < vertexCount; a++)
					{
						int32 count = weightCount[a];
						skinSize += CountBoneWeights(start, count, indexArray) * 2;
						start += count;
					}

					break;
				}

				element = element->Next();
			}
		}
		else
		{
			const Collada::Element *element = GetFirstSubnode();
			while (element)
			{
				if (element->GetElementType() == Collada::kElementIndexArray)
				{
					vertexCount++;
					skinSize++;

					const Collada::IndexArray *indexArray = static_cast<const Collada::IndexArray *>(element);
					int32 count = indexArray->GetIndexCount() / inputCount;
					skinSize += CountBoneWeights(0, count, indexArray) * 2;
				}

				element = element->Next();
			}
		}

		skinVertexCount = vertexCount;
		skinDataSize = skinSize * 4;
	}
}

int32 Collada::Combiner::CountBoneWeights(int32 start, int32 count, const Collada::IndexArray *indexArray) const
{
	int32 inputCount = GetInputCount();

	int32 end = start + count;
	count = 0;

	for (machine a = start; a < end; a++)
	{
		const float *weight = weightAccessor->GetFloatValue(indexArray->GetIndexValue(a * inputCount + weightIndex));
		if ((weight) && (*weight != 0.0F))
		{
			int32 index = indexArray->GetIndexValue(a * inputCount + jointIndex);
			for (machine b = start; b < a; b++)
			{
				if (indexArray->GetIndexValue(b * inputCount + jointIndex) == index)
				{
					goto skip;
				}
			}

			count++;
		}

		skip:;
	}

	return (Max(count, 1));
}

C4::SkinWeight *Collada::Combiner::GenerateBoneWeights(int32 maxBoneIndex, C4::SkinWeight *skinWeight, int32 start, int32 count, const Collada::IndexArray *indexArray) const
{
	int32 inputCount = GetInputCount();

	int32 end = start + count;
	int32 finalCount = 0;

	BoneWeight *boneWeight = skinWeight->boneWeight;
	for (machine a = start; a < end; a++)
	{
		const float *weight = weightAccessor->GetFloatValue(indexArray->GetIndexValue(a * inputCount + weightIndex));
		if ((weight) && (*weight != 0.0F))
		{
			int32 index = indexArray->GetIndexValue(a * inputCount + jointIndex);

			for (machine b = start; b < a; b++)
			{
				if (indexArray->GetIndexValue(b * inputCount + jointIndex) == index)
				{
					goto skip;
				}
			}

			float w = *weight;
			for (machine b = a + 1; b < end; b++)
			{
				if (indexArray->GetIndexValue(b * inputCount + jointIndex) == index)
				{
					w += *weightAccessor->GetFloatValue(indexArray->GetIndexValue(b * inputCount + weightIndex));
				}
			}

			boneWeight->weight = w;
			boneWeight->boneIndex = Min(MaxZero(index), maxBoneIndex);
			boneWeight++;

			finalCount++;
		}

		skip:;
	}

	if (finalCount != 0)
	{
		skinWeight->boneCount = finalCount;
	}
	else
	{
		skinWeight->boneCount = 1;
		boneWeight->weight = 1.0F;
		boneWeight->boneIndex = 0;
		boneWeight++;
	}

	return (reinterpret_cast<C4::SkinWeight *>(boneWeight));
}

bool Collada::Combiner::GenerateWeightData(int32 maxBoneIndex, C4::SkinWeight *skinWeight, const C4::SkinWeight **skinWeightTable) const
{
	int32 inputCount = GetInputCount();

	const Collada::CountArray *countArray = GetCountArray();
	if (countArray)
	{
		const Collada::Element *element = GetFirstSubnode();
		while (element)
		{
			if (element->GetElementType() == Collada::kElementIndexArray)
			{
				const Collada::IndexArray *indexArray = static_cast<const Collada::IndexArray *>(element);
				const int32 *boneCount = countArray->GetArray();

				int32 vertexCount = GetItemCount();
				if (vertexCount == 0)
				{
					return (false);
				}

				int32 start = 0;
				for (machine a = 0; a < vertexCount; a++)
				{
					*skinWeightTable++ = skinWeight;

					int32 count = boneCount[a];
					if (count == 0)
					{
						return (false);
					}

					skinWeight = GenerateBoneWeights(maxBoneIndex, skinWeight, start, count, indexArray);
					start += count;
				}

				break;
			}

			element = element->Next();
		}
	}
	else
	{
		const Collada::Element *element = GetFirstSubnode();
		while (element)
		{
			if (element->GetElementType() == Collada::kElementIndexArray)
			{
				const Collada::IndexArray *indexArray = static_cast<const Collada::IndexArray *>(element);
				int32 count = indexArray->GetIndexCount() / inputCount;

				*skinWeightTable++ = skinWeight;
				skinWeight = GenerateBoneWeights(maxBoneIndex, skinWeight, 0, count, indexArray);
			}

			element = element->Next();
		}
	}

	return (true);
}


Collada::Vertices::Vertices() : Collada::Element(Collada::kElementVertices)
{
}

Collada::Vertices::~Vertices()
{
}


Collada::BindShapeMatrix::BindShapeMatrix() : Collada::Element(Collada::kElementBindShapeMatrix)
{
	bindShapeMatrix.SetIdentity();
}

Collada::BindShapeMatrix::~BindShapeMatrix()
{
}

void Collada::BindShapeMatrix::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Root *root = static_cast<const Collada::Root *>(GetRootNode());

	bindShapeMatrix.SetTranslation(bindShapeMatrix.GetTranslation() * root->GetSceneScale());

	if (root->GetUpAxisType() == Collada::kAxisY)
	{
		bindShapeMatrix = Transform4D(K::x_unit, K::z_unit, K::minus_y_unit, Zero3D) * bindShapeMatrix * Transform4D(K::x_unit, K::minus_z_unit, K::y_unit, Zero3D);
	}
}

void Collada::BindShapeMatrix::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	for (machine count = 0; count < 12; count++)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			break;
		}

		bindShapeMatrix(count >> 2, count & 3) = Text::StringToFloat(string);
	}
}


Collada::Skin::Skin() : Collada::Element(Collada::kElementSkin)
{
	bindPositionAccessor = nullptr;
	bindNormalAccessor = nullptr;
	invBindMatrixAccessor = nullptr;
	jointNameAccessor = nullptr;
	jointWeightCombiner = nullptr;
	bindShapeMatrix = nullptr;

	skinStorage = nullptr;

	sourceElement = nullptr;
	sourceIdentifier[0] = 0;
}

Collada::Skin::~Skin()
{
	delete[] skinStorage;
}

void Collada::Skin::ProcessInputs(const Collada::Element *root)
{
	const Collada::Element *element = root->GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementInput)
		{
			const Collada::Input *input = static_cast<const Collada::Input *>(element);
			const Collada::Element *source = input->GetSourceElement();
			if (source)
			{
				const Collada::Accessor *accessor = nullptr;

				Collada::ElementType type = source->GetElementType();
				if (type == Collada::kElementAccessor)
				{
					accessor = static_cast<const Collada::Accessor *>(source);
				}
				else if (type == Collada::kElementSource)
				{
					accessor = static_cast<const Collada::Source *>(source)->GetFirstAccessor();
				}

				if (accessor)
				{
					Collada::SemanticType semantic = input->GetInputSemantic();
					if (semantic == Collada::kSemanticBindPosition)
					{
						bindPositionAccessor = accessor;
					}
					else if (semantic == Collada::kSemanticBindNormal)
					{
						bindNormalAccessor = accessor;
					}
					else if (semantic == Collada::kSemanticInvBindMatrix)
					{
						invBindMatrixAccessor = accessor;
					}
					else if (semantic == Collada::kSemanticJoint)
					{
						jointNameAccessor = accessor;
					}
					else if (semantic == Collada::kSemanticJointWeight)
					{
						PreprocessJointWeightSource(source);
					}
				}
			}
		}

		element = element->Next();
	}
}

void Collada::Skin::PreprocessJointWeightSource(const Collada::Element *source)
{
	const Collada::Element *element = source->GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementTechnique)
		{
			Collada::Element *subelement = element->GetFirstSubnode();
			while (subelement)
			{
				type = subelement->GetElementType();
				if (type == Collada::kElementJoints)
				{
					ProcessInputs(subelement);
				}
				else if (type == Collada::kElementCombiner)
				{
					jointWeightCombiner = static_cast<Collada::Combiner *>(subelement);
				}

				subelement = subelement->Next();
			}
		}

		element = element->Next();
	}
}

void Collada::Skin::Preprocess(void)
{
	Collada::Element::Preprocess();

	sourceElement = FindElement(sourceIdentifier);

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if ((type == Collada::kElementVertices) || (type == Collada::kElementJoints))
		{
			ProcessInputs(element);
		}
		else if (type == Collada::kElementCombiner)
		{
			jointWeightCombiner = static_cast<const Collada::Combiner *>(element);
		}
		else if (type == Collada::kElementBindShapeMatrix)
		{
			bindShapeMatrix = static_cast<const Collada::BindShapeMatrix *>(element);
		}

		element = element->Next();
	}

	if (jointNameAccessor)
	{
		int32 boneCount = jointNameAccessor->GetAccessCount();
		for (machine a = 0; a < boneCount; a++)
		{
			const char *name = jointNameAccessor->GetNameValue(a);
			if (name)
			{
				Collada::Node *node = FindNamedElement(name);
				if (node)
				{
					node->SetNodeType(Collada::kNodeJoint);
				}
			}
		}
	}
}

const C4::SkinData *Collada::Skin::GetSkinData(Collada::Scene *scene)
{
	if (!skinStorage)
	{
		if ((jointWeightCombiner) && (jointNameAccessor) && (invBindMatrixAccessor))
		{
			Transform4D		bindTransform;

			int32 boneCount = jointNameAccessor->GetAccessCount();
			skinData.boneCount = boneCount;

			int32 vertexCount = jointWeightCombiner->GetSkinVertexCount();
			unsigned_int32 dataSize = (jointWeightCombiner->GetSkinDataSize() + 15) & ~15;

			skinStorage = new char[boneCount * (sizeof(Transform4D) + 4) + vertexCount * sizeof(C4::SkinWeight *) + dataSize];
			Transform4D *inverseBindTransform = reinterpret_cast<Transform4D *>(skinStorage);
			unsigned_int32 *boneHash = reinterpret_cast<unsigned_int32 *>(inverseBindTransform + boneCount);
			const C4::SkinWeight **skinWeightTable = reinterpret_cast<const C4::SkinWeight **>(boneHash + boneCount);
			C4::SkinWeight *skinWeight = reinterpret_cast<C4::SkinWeight *>(skinWeightTable + vertexCount);

			skinData.boneHashArray = boneHash;
			skinData.inverseBindTransformArray = inverseBindTransform;
			skinData.skinWeightTable = skinWeightTable;

			const Collada::Root *root = static_cast<const Collada::Root *>(GetRootNode());
			Collada::AxisType upAxis = root->GetUpAxisType();
			float scale = root->GetSceneScale();

			if (bindShapeMatrix)
			{
				bindTransform = bindShapeMatrix->GetMatrix();
			}
			else
			{
				bindTransform.SetIdentity();
			}

			for (machine a = 0; a < boneCount; a++)
			{
				const char *name = jointNameAccessor->GetNameValue(a);
				if (name)
				{
					Collada::Node *node = scene->FindNode(name);
					if (node)
					{
						boneHash[a] = node->GetNodeHash();

						const float *matrix = invBindMatrixAccessor->GetFloatValue(a);
						if (matrix)
						{
							Transform4D transform(matrix[0], matrix[1], matrix[2], matrix[3],
												  matrix[4], matrix[5], matrix[6], matrix[7],
												  matrix[8], matrix[9], matrix[10], matrix[11]);

							transform = Inverse(transform);

							transform.SetTranslation(transform.GetTranslation() * scale);

							if (upAxis == Collada::kAxisY)
							{
								transform = Transform4D(K::x_unit, K::z_unit, K::minus_y_unit, Zero3D) * transform * Transform4D(K::x_unit, K::minus_z_unit, K::y_unit, Zero3D);
							}

							inverseBindTransform[a] = Inverse(transform) * bindTransform;
						}
					}
				}
			}

			if (!jointWeightCombiner->GenerateWeightData(boneCount - 1, skinWeight, skinWeightTable))
			{
				delete[] skinStorage;
				skinStorage = nullptr;
				return (nullptr);
			}
		}
		else
		{
			return (nullptr);
		}
	}

	return (&skinData);
}


Collada::Targets::Targets() : Collada::Element(Collada::kElementTargets)
{
}

Collada::Targets::~Targets()
{
}


Collada::Morph::Morph() : Collada::Element(Collada::kElementMorph)
{
	methodType = Collada::kMethodNormalized;
}

Collada::Morph::~Morph()
{
}


Collada::Faces::Faces(Collada::FacesType type) : Collada::InterleavingElement(Collada::kElementFaces)
{
	facesType = type;

	positionAccessor = nullptr;
	normalAccessor = nullptr;
	colorAccessor = nullptr;

	for (machine a = 0; a < kMaxGeometryTexcoordCount; a++)
	{
		texcoordAccessor[a] = nullptr;
	}

	materialElement = nullptr;
	materialIdentifier[0] = 0;
}

Collada::Faces::~Faces()
{
}

void Collada::Faces::ProcessInput(int32 index, const Collada::Input *input)
{
	const Collada::Element *source = input->GetSourceElement();
	if (source)
	{
		Collada::ElementType type = source->GetElementType();
		if (type == Collada::kElementSource)
		{
			const Collada::Accessor *accessor = static_cast<const Collada::Source *>(source)->GetFirstAccessor();
			if (accessor)
			{
				Collada::SemanticType semantic = input->GetInputSemantic();
				if (semantic == Collada::kSemanticPosition)
				{
					if (!positionAccessor)
					{
						positionIndex = index;
						positionAccessor = accessor;
					}
				}
				else if (semantic == Collada::kSemanticNormal)
				{
					if (!normalAccessor)
					{
						normalIndex = index;
						normalAccessor = accessor;
					}
				}
				else if (semantic == Collada::kSemanticColor)
				{
					if (!colorAccessor)
					{
						colorIndex = index;
						colorAccessor = accessor;
					}
				}
				else if (semantic == Collada::kSemanticTexcoord)
				{
					int32 set = input->GetTexcoordSet();
					if ((set >= 0) && (set < kMaxGeometryTexcoordCount))
					{
						if (!texcoordAccessor[set])
						{
							texcoordIndex[set] = index;
							texcoordAccessor[set] = accessor;
						}
					}
				}
			}
		}
		else if (type == Collada::kElementVertices)
		{
			const Collada::Element *element = source->GetFirstSubnode();
			while (element)
			{
				type = element->GetElementType();
				if (type == Collada::kElementInput)
				{
					input = static_cast<const Collada::Input *>(element);
					ProcessInput(index, input);
				}

				element = element->Next();
			}
		}
	}
}

void Collada::Faces::Preprocess(void)
{
	Collada::InterleavingElement::Preprocess();

	materialElement = FindElement(materialIdentifier);
}

GeometryPolygon *Collada::Faces::BuildPolygon(const int32 *entry, const Collada::IndexArray *indexArray, const Collada::Accessor *posAccessor, const Collada::Accessor *nrmAccessor) const
{
	GeometryVertex	*gv[3];
	Vector4D		tangent;

	GeometryPolygon *polygon = new GeometryPolygon;

	int32 inputCount = GetInputCount();
	for (machine a = 0; a < 3; a++)
	{
		GeometryVertex *vertex = new GeometryVertex;

		int32 index = indexArray->GetIndexValue(entry[a] * inputCount + positionIndex);
		vertex->index = index;

		const float *value = posAccessor->GetFloatValue(index);
		if (value)
		{
			vertex->position.Set(value[0], value[1], value[2]);
		}
		else
		{
			vertex->position.Set(0.0F, 0.0F, 0.0F);
		}

		if (nrmAccessor)
		{
			index = indexArray->GetIndexValue(entry[a] * inputCount + normalIndex);
			value = nrmAccessor->GetFloatValue(index);
			if (value)
			{
				vertex->normal.Set(value[0], value[1], value[2]);
			}
		}

		if (colorAccessor)
		{
			index = indexArray->GetIndexValue(entry[a] * inputCount + colorIndex);
			value = colorAccessor->GetFloatValue(index);
			if (value)
			{
				vertex->color.Set(value[0], value[1], value[2], 1.0F);
			}
		}

		for (machine b = 0; b < kMaxGeometryTexcoordCount; b++)
		{
			vertex->texcoord[b].Set(0.0F, 0.0F);
			if (texcoordAccessor[b])
			{
				index = indexArray->GetIndexValue(entry[a] * inputCount + texcoordIndex[b]);
				value = texcoordAccessor[b]->GetFloatValue(index);
				if (value)
				{
					vertex->texcoord[b].Set(value[0], value[1]);
				}
			}
		}

		gv[a] = vertex;
		polygon->vertexList.Append(vertex);
	}

	if (Math::CalculateTangent(gv[0]->position, gv[1]->position, gv[2]->position, gv[0]->texcoord[0], gv[1]->texcoord[0], gv[2]->texcoord[0], &tangent))
	{
		gv[0]->tangent = tangent;
		gv[1]->tangent = tangent;
		gv[2]->tangent = tangent;
	}

	return (polygon);
}

GeometrySurface *Collada::Faces::BuildSurface(const Collada::Scene *scene, const Collada::Skin *skin) const
{
	Transform4D		transform;

	const Collada::Accessor *posAccessor = positionAccessor;
	if (skin)
	{
		const Collada::Accessor *bindPositionAccessor = skin->GetBindPositionAccessor();
		if (bindPositionAccessor)
		{
			posAccessor = bindPositionAccessor;
		}
	}

	if (!posAccessor)
	{
		return (nullptr);
	}

	const Collada::Accessor *nrmAccessor = normalAccessor;
	if (skin)
	{
		const Collada::Accessor *bindNormalAccessor = skin->GetBindNormalAccessor();
		if (bindNormalAccessor)
		{
			nrmAccessor = bindNormalAccessor;
		}
	}

	const int32 *vertexCount = nullptr;
	if (facesType == Collada::kFacesPolylist)
	{
		const Collada::CountArray *countArray = GetCountArray();
		if ((!countArray) || (countArray->GetArraySize() != GetItemCount()))
		{
			return (nullptr);
		}

		vertexCount = countArray->GetArray();
	}

	GeometrySurface *surface = new GeometrySurface(kSurfaceValidTangents);

	if (nrmAccessor)
	{
		surface->surfaceFlags |= kSurfaceValidNormals;
	}

	if (colorAccessor)
	{
		surface->surfaceFlags |= kSurfaceValidColors;
	}

	for (machine a = 1; a < kMaxGeometryTexcoordCount; a++)
	{
		if (texcoordAccessor[a])
		{
			surface->texcoordCount = (int32) (a + 1);
		}
	}

	int32 inputCount = GetInputCount();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementIndexArray)
		{
			int32	entry[3];

			const Collada::IndexArray *indexArray = static_cast<const Collada::IndexArray *>(element);
			int32 totalCount = indexArray->GetIndexCount() / inputCount;

			if (facesType == Collada::kFacesTriangles)
			{
				for (machine a = 0; a < totalCount; a += 3)
				{
					entry[0] = a;
					entry[1] = a + 1;
					entry[2] = a + 2;
					surface->polygonList.Append(BuildPolygon(entry, indexArray, posAccessor, nrmAccessor));
				}
			}
			else if (facesType == Collada::kFacesPolygons)
			{
				for (machine a = 2; a < totalCount; a++)
				{
					entry[0] = 0;
					entry[1] = a - 1;
					entry[2] = a;
					surface->polygonList.Append(BuildPolygon(entry, indexArray, posAccessor, nrmAccessor));
				}
			}
			else
			{
				int32 count = 0;
				while (count < totalCount)
				{
					int32 k = *vertexCount++;
					for (machine a = 2; a < k; a++)
					{
						entry[0] = count;
						entry[1] = count + a - 1;
						entry[2] = count + a;
						surface->polygonList.Append(BuildPolygon(entry, indexArray, posAccessor, nrmAccessor));
					}

					count += k;
				}
			}
		}

		element = element->Next();
	}

	const Collada::Root *root = static_cast<const Collada::Root *>(GetRootNode());
	float scale = root->GetSceneScale();

	if (root->GetUpAxisType() == Collada::kAxisY)
	{
		transform.Set(scale, 0.0F, 0.0F, 0.0F,
					  0.0F, 0.0F, -scale, 0.0F,
					  0.0F, scale, 0.0F, 0.0F);
	}
	else
	{
		transform.Set(scale, 0.0F, 0.0F, 0.0F,
					  0.0F, scale, 0.0F, 0.0F,
					  0.0F, 0.0F, scale, 0.0F);
	}

	GeometryPolygon *polygon = surface->polygonList.First();
	while (polygon)
	{
		GeometryPolygon *next = polygon->Next();

		GeometryVertex *v1 = polygon->vertexList.First();
		if (v1)
		{
			GeometryVertex *v2 = v1->Next();
			if (v2)
			{
				GeometryVertex *v3 = v2->Next();
				if (v3)
				{
					if (SquaredMag((v2->position - v1->position) % (v3->position - v1->position)) > 0.0F)
					{
						v1->position = transform * v1->position;
						v2->position = transform * v2->position;
						v3->position = transform * v3->position;

						v1->normal = Normalize(transform * v1->normal);
						v2->normal = Normalize(transform * v2->normal);
						v3->normal = Normalize(transform * v3->normal);

						goto keep;
					}
				}
			}
		}

		delete polygon;

		keep:
		polygon = next;
	}

	if (!surface->polygonList.Empty())
	{
		return (surface);
	}

	delete surface;
	return (nullptr);
}


Collada::ControlVertices::ControlVertices() : Collada::Element(Collada::kElementControlVertices)
{
}

Collada::ControlVertices::~ControlVertices()
{
}


Collada::Spline::Spline() : Collada::Element(Collada::kElementSpline)
{
	closedFlag = false;
	positionAccessor = nullptr;
}

Collada::Spline::~Spline()
{
}

void Collada::Spline::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementControlVertices)
		{
			const Collada::Element *subelement = element->GetFirstSubnode();
			while (subelement)
			{
				if (subelement->GetElementType() == Collada::kElementInput)
				{
					const Collada::Input *input = static_cast<const Collada::Input *>(subelement);
					if (input->GetInputSemantic() == Collada::kSemanticPosition)
					{
						const Collada::Element *source = input->GetSourceElement();
						if ((source) && (source->GetElementType() == Collada::kElementSource))
						{
							positionAccessor = static_cast<const Collada::Source *>(source)->GetFirstAccessor();
						}

						break;
					}
				}

				subelement = subelement->Next();
			}

			break;
		}

		element = element->Next();
	}
}

void Collada::Spline::BuildPathComponents(Path *path) const
{
	const Collada::Accessor *accessor = positionAccessor;
	if (accessor)
	{
		int32 count = accessor->GetAccessCount();
		int32 stride = accessor->GetAccessStride();
		int32 dimension = Min(stride, 3);

		Point3D *positionArray = new Point3D[count];
		Point3D *p = positionArray;

		for (machine a = 0; a < count; a++)
		{
			p->Set(0.0F, 0.0F, 0.0F);
			const float *value = accessor->GetFloatValue(a);
			for (machine b = 0; b < dimension; b++)
			{
				(*p)[b] = value[b];
			}

			p++;
		}

		p = positionArray;
		int32 componentCount = (count - 1) / 3;

		for (machine a = 0; a < componentCount; a++)
		{
			BezierPathComponent *component = new BezierPathComponent(p[0], p[1], p[2], p[3]);
			path->AppendPathComponent(component);
			p += 3;
		}

		delete[] positionArray;
	}
}


Collada::Instance::Instance() : Collada::Element(Collada::kElementInstance)
{
	resourceElement = nullptr;
	resourceIdentifier[0] = 0;

	symbolIdentifier[0] = 0;
}

Collada::Instance::~Instance()
{
}

void Collada::Instance::Preprocess(void)
{
	Collada::Element::Preprocess();

	resourceElement = FindElement(resourceIdentifier);
	if (!resourceElement)
	{
		Collada::Element *element = GetSuperNode();
		if ((element) && (element->GetElementType() == Collada::kElementScene))
		{
			for (;;)
			{
				element = element->Previous();
				if (!element)
				{
					break;
				}

				if ((element->GetElementType() == kElementLibrary) && (static_cast<Collada::Library *>(element)->GetLibraryType() == Collada::kLibraryVisualScene))
				{
					element = element->GetFirstSubnode();
					while (element)
					{
						if ((element->GetElementType() == Collada::kElementScene) && (element->GetIdentifier()[0] == 0))
						{
							resourceElement = element;
							break;
						}

						element = element->Next();
					}

					break;
				}
			}
		}
	}

	Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementBindMaterial)
		{
			materialBindingList.Append(static_cast<Collada::BindMaterial *>(element));
		}

		element = element->Next();
	}
}

C4::Node *Collada::Instance::BuildInstance(Collada::Scene *scene) const
{
	const Collada::Element *resource = resourceElement;
	if (resource)
	{
		Collada::ElementType type = resource->GetElementType();
		if (type == Collada::kElementGeometry)
		{
			const Collada::Geometry *geometry = static_cast<const Collada::Geometry *>(resource);

			const Collada::Element *element = resource->GetFirstSubnode();
			if (element->GetElementType() != Collada::kElementSpline)
			{
				return (geometry->BuildGeometry(scene, &materialBindingList));
			}
			else
			{
				return (geometry->BuildPath());
			}
		}
		else if (type == Collada::kElementLight)
		{
			return (static_cast<const Collada::Light *>(resource)->BuildLight());
		}
		else if (type == Collada::kElementCamera)
		{
			return (static_cast<const Collada::Camera *>(resource)->BuildCamera());
		}
		else if (type == Collada::kElementController)
		{
			Collada::Skin *skin = nullptr;

			Collada::Element *element = resource->GetFirstSubnode();
			while (element)
			{
				if (element->GetElementType() == Collada::kElementSkin)
				{
					skin = static_cast<Collada::Skin *>(element);
					break;
				}

				element = element->Next();
			}

			if (skin)
			{
				const Collada::Element *target = static_cast<const Collada::Controller *>(resource)->GetTargetElement();
				if (!target)
				{
					target = skin->GetSourceElement();
				}

				if ((target) && (target->GetElementType() == Collada::kElementGeometry))
				{
					GenericGeometry *geometry = static_cast<const Collada::Geometry *>(target)->BuildGeometry(scene, &materialBindingList, skin);
					if (geometry)
					{
						C4::SkinController *skinController = new C4::SkinController;
						geometry->SetController(skinController);
						return (geometry);
					}
				}
			}
		}
	}

	return (nullptr);
}


Collada::Node::Node() : Collada::Element(Collada::kElementNode)
{
	nodeType = Collada::kNodeNode;
	nodeHash = 0;
	nodeName[0] = 0;
}

Collada::Node::~Node()
{
}

void Collada::Node::Preprocess(void)
{
	Collada::Element::Preprocess();

	if (Text::CompareText(nodeName, "MARKER_", 7))
	{
		Text::CopyText(&nodeName[7], nodeName);
		nodeType = Collada::kNodeMarker;
	}
}

Transform4D Collada::Node::EvaluateTransform(void) const
{
	Transform4D		transform;

	transform.SetIdentity();

	const Collada::Element *element = GetLastSubnode();
	while (element)
	{
		switch (element->GetElementType())
		{
			case Collada::kElementMatrix:
			{
				const Collada::Matrix *matrix = static_cast<const Collada::Matrix *>(element);
				transform = matrix->GetTransformMatrix() * transform;
				break;
			}

			case Collada::kElementTranslate:
			{
				const Collada::Translate *translate = static_cast<const Collada::Translate *>(element);
				transform.SetTranslation(transform.GetTranslation() + translate->GetTranslatePosition());
				break;
			}

			case Collada::kElementRotate:
			{
				const Collada::Rotate *rotate = static_cast<const Collada::Rotate *>(element);
				transform = Transform4D().SetRotationAboutAxis(rotate->GetRotateAngle() * K::radians, rotate->GetRotateAxis()) * transform;
				break;
			}

			case Collada::kElementScale:
			{
				const Collada::Scale *scale = static_cast<const Collada::Scale *>(element);
				const Vector3D& s = scale->GetScaleFactor();
				transform.Set(transform(0,0) * s.x, transform(0,1) * s.x, transform(0,2) * s.x, transform(0,3) * s.x,
							  transform(1,0) * s.y, transform(1,1) * s.y, transform(1,2) * s.y, transform(1,3) * s.y,
							  transform(2,0) * s.z, transform(2,1) * s.z, transform(2,2) * s.z, transform(2,3) * s.z);
				break;
			}

			case Collada::kElementLookat:
			{
				const Collada::Lookat *lookat = static_cast<const Collada::Lookat *>(element);
				const Point3D& p = lookat->GetCameraPosition();
				Vector3D z = Normalize(p - lookat->GetTargetPosition());
				Vector3D x = Normalize(lookat->GetUpDirection() % z);
				transform.Set(x, z % x, z, p);
				break;
			}
		}

		element = element->Previous();
	}

	const Collada::Root *root = static_cast<const Collada::Root *>(GetRootNode());

	transform.SetTranslation(transform.GetTranslation() * root->GetSceneScale());

	if (root->GetUpAxisType() == Collada::kAxisY)
	{
		transform = Transform4D(K::x_unit, K::z_unit, K::minus_y_unit, Zero3D) * transform * Transform4D(K::x_unit, K::minus_z_unit, K::y_unit, Zero3D);
	}

	return (transform);
}

void Collada::Node::AddToScene(C4::Node *node, C4::Node *superNode) const
{
	Transform4D transform = EvaluateTransform();

	NodeType type = node->GetNodeType();
	if ((type != C4::kNodeLight) && (type != C4::kNodeCamera))
	{
		if ((type == C4::kNodeGeometry) && (Fabs(Determinant(transform) - 1.0F) > 0.015625F))
		{
			const C4::Controller *controller = node->GetController();
			if ((!controller) || (controller->GetControllerType() != kControllerSkin))
			{
				node->SetNodePosition(transform.GetTranslation());
				transform.SetTranslation(Zero3D);

				C4::GeometryObject *object = static_cast<C4::Geometry *>(node)->GetObject();
				int32 levelCount = object->GetGeometryLevelCount();
				for (machine level = 0; level < levelCount; level++)
				{
					C4::Mesh *mesh = object->GetGeometryLevel(level);
					mesh->TransformMesh(transform);
				}

				static_cast<GenericGeometryObject *>(object)->UpdateBounds();
				object->BuildCollisionData();
				goto end;
			}
		}

		node->SetNodeTransform(transform);
	}
	else
	{
		node->SetNodeTransform(transform[0], -transform[1], -transform[2], transform.GetTranslation());
	}

	end:
	if (nodeName[0] != 0)
	{
		node->SetNodeName(nodeName);
	}
	else
	{
		const String<kMaxIdentifierLength>& identifier = GetIdentifier();
		if (identifier[0] != 0)
		{
			node->SetNodeName(identifier);
		}
	}

	superNode->AppendSubnode(node);
}

void Collada::Node::BuildNode(Collada::Scene *scene, C4::Node *superNode)
{
	C4::Node *primaryNode = nullptr;

	if (nodeType == Collada::kNodeNode)
	{
		const Collada::Element *element = GetFirstSubnode();
		while (element)
		{
			if (element->GetElementType() == Collada::kElementInstance)
			{
				C4::Node *node = static_cast<const Collada::Instance *>(element)->BuildInstance(scene);
				if (node)
				{
					primaryNode = node;
					break;
				}
			}

			element = element->Next();
		}
	}
	else if (nodeType == Collada::kNodeJoint)
	{
		primaryNode = new C4::Bone;
	}
	else if (nodeType == Collada::kNodeMarker)
	{
		primaryNode = new C4::LocatorMarker(0);
	}

	if (!primaryNode)
	{
		primaryNode = new C4::Node;
	}

	AddToScene(primaryNode, superNode);
	scene->BuildNodes(this, primaryNode);
}


Collada::Scene::Scene() : Collada::Element(Collada::kElementScene)
{
	nodeCount = 0;
}

Collada::Scene::~Scene()
{
}

void Collada::Scene::Preprocess(void)
{
	Collada::Element::Preprocess();

	int32 count = 0;
	Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementNode)
		{
			Collada::Node *node = static_cast<Collada::Node *>(element);
			node->SetNodeHash(Text::Hash(node->GetNodeName()));
			count++;
		}

		element = GetNextNode(element);
	}

	nodeCount = count;
}

Collada::Node *Collada::Scene::FindNode(const char *name)
{
	Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementNode)
		{
			Collada::Node *node = static_cast<Collada::Node *>(element);
			if (node->GetNodeName() == name)
			{
				return (node);
			}
		}
		else if (type == Collada::kElementInstance)
		{
			Collada::Element *resource = static_cast<Collada::Instance *>(element)->GetResourceElement();
			if ((resource) && (resource->GetElementType() == Collada::kElementScene))
			{
				Collada::Node *node = static_cast<Collada::Scene *>(resource)->FindNode(name);
				if (node)
				{
					return (node);
				}
			}
		}

		element = GetNextNode(element);
	}

	element = FindElement(name, this);
	if ((element) && (element->GetElementType() == Collada::kElementNode))
	{
		return (static_cast<Collada::Node *>(element));
	}

	element = FindSubElement(name, this);
	if ((element) && (element->GetElementType() == Collada::kElementNode))
	{
		return (static_cast<Collada::Node *>(element));
	}

	return (nullptr);
}

void Collada::Scene::BuildNodes(const Collada::Element *rootElement, C4::Node *superNode)
{
	Collada::Element *element = rootElement->GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementNode)
		{
			static_cast<Collada::Node *>(element)->BuildNode(this, superNode);
		}
		else if (type == Collada::kElementInstance)
		{
			const Collada::Element *subelement = static_cast<const Collada::Instance *>(element)->GetResourceElement();
			if ((subelement) && (subelement->GetElementType() == Collada::kElementScene))
			{
				BuildNodes(subelement, superNode);
			}
		}

		element = element->Next();
	}
}

void Collada::Scene::RemoveDeadNodes(C4::Node *rootNode)
{
	C4::Node *node = rootNode->GetFirstSubnode();
	while (node)
	{
		C4::Node *next = node->Next();
		RemoveDeadNodes(node);
		node = next;
	}

	if ((rootNode->GetNodeType() == kNodeGeneric) && (!rootNode->GetFirstSubnode()))
	{
		delete rootNode;
	}
}

void Collada::Scene::BuildScene(C4::Node *rootNode)
{
	BuildNodes(this, rootNode);
	RemoveDeadNodes(rootNode);
}


Collada::Matrix::Matrix() : Collada::Element(Collada::kElementMatrix)
{
	transformMatrix.SetIdentity();
}

Collada::Matrix::~Matrix()
{
}

void Collada::Matrix::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	for (machine i = 0; i < 3; i++)
	{
		for (machine j = 0; j < 4; j++)
		{
			ReadWhitespace(text);
			if (!ReadString(text, &string))
			{
				return;
			}

			transformMatrix(i,j) = Text::StringToFloat(string);
		}
	}
}


Collada::Translate::Translate() : Collada::Element(Collada::kElementTranslate)
{
	translatePosition.Set(0.0F, 0.0F, 0.0F);
}

Collada::Translate::~Translate()
{
}

void Collada::Translate::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	ReadWhitespace(text);
	if (ReadString(text, &string))
	{
		translatePosition.x = Text::StringToFloat(string);

		ReadWhitespace(text);
		if (ReadString(text, &string))
		{
			translatePosition.y = Text::StringToFloat(string);

			ReadWhitespace(text);
			if (ReadString(text, &string))
			{
				translatePosition.z = Text::StringToFloat(string);
			}
		}
	}
}


Collada::Rotate::Rotate() : Collada::Element(Collada::kElementRotate)
{
	rotateAxis.Set(0.0F, 0.0F, 1.0F);
	rotateAngle = 0.0F;
}

Collada::Rotate::~Rotate()
{
}

void Collada::Rotate::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	ReadWhitespace(text);
	if (ReadString(text, &string))
	{
		rotateAxis.x = Text::StringToFloat(string);

		ReadWhitespace(text);
		if (ReadString(text, &string))
		{
			rotateAxis.y = Text::StringToFloat(string);

			ReadWhitespace(text);
			if (ReadString(text, &string))
			{
				rotateAxis.z = Text::StringToFloat(string);

				ReadWhitespace(text);
				if (ReadString(text, &string))
				{
					rotateAngle = Text::StringToFloat(string);
				}
			}
		}
	}
}


Collada::Scale::Scale() : Collada::Element(Collada::kElementScale)
{
	scaleFactor.Set(1.0F, 1.0F, 1.0F);
}

Collada::Scale::~Scale()
{
}

void Collada::Scale::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	ReadWhitespace(text);
	if (ReadString(text, &string))
	{
		scaleFactor.x = Text::StringToFloat(string);

		ReadWhitespace(text);
		if (ReadString(text, &string))
		{
			scaleFactor.y = Text::StringToFloat(string);

			ReadWhitespace(text);
			if (ReadString(text, &string))
			{
				scaleFactor.z = Text::StringToFloat(string);
			}
		}
	}
}


Collada::Lookat::Lookat() : Collada::Element(Collada::kElementLookat)
{
	cameraPosition.Set(0.0F, 0.0F, 0.0F);
	targetPosition.Set(1.0F, 0.0F, 0.0F);
	upDirection.Set(0.0F, 0.0F, 1.0F);
}

Collada::Lookat::~Lookat()
{
}

void Collada::Lookat::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	for (machine i = 0; i < 3; i++)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			return;
		}

		cameraPosition[i] = Text::StringToFloat(string);
	}

	for (machine i = 0; i < 3; i++)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			return;
		}

		targetPosition[i] = Text::StringToFloat(string);
	}

	for (machine i = 0; i < 3; i++)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			return;
		}

		upDirection[i] = Text::StringToFloat(string);
	}
}


Collada::UpAxis::UpAxis() : Collada::Element(Collada::kElementUpAxis)
{
	upAxis = kAxisY;
}

Collada::UpAxis::~UpAxis()
{
}

void Collada::UpAxis::Preprocess(void)
{
	Collada::Element::Preprocess();

	static_cast<Collada::Root *>(GetRootNode())->SetUpAxisElement(this);
}

void Collada::UpAxis::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	ReadWhitespace(text);
	if (ReadString(text, &string))
	{
		if (string == "Z_UP")
		{
			upAxis = kAxisZ;
		}
		else if (string == "Y_UP")
		{
			upAxis = kAxisY;
		}
		else if (string == "X_UP")
		{
			upAxis = kAxisX;
		}
	}
}


Collada::Unit::Unit() : Collada::Element(Collada::kElementUnit)
{
	meterScale = 1.0F;
}

Collada::Unit::~Unit()
{
}

void Collada::Unit::Preprocess(void)
{
	Collada::Element::Preprocess();

	static_cast<Collada::Root *>(GetRootNode())->SetUnitElement(this);
}


Collada::InitFrom::InitFrom() : Collada::Element(Collada::kElementInitFrom)
{
	initString[0] = 0;
}

Collada::InitFrom::~InitFrom()
{
}

void Collada::InitFrom::ProcessContent(const char *& text)
{
	ReadWhitespace(text);
	if (ReadContent(text, &initString))
	{
		static const char remap[129] = "________________________________ !_#$%&'()_+,-./0123456789_;_=__@ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_`abcdefghijklmnopqrstuvwxyz{_}~_";

		for (char *string = initString;; string++)
		{
			char c = *string;
			if (c == 0)
			{
				break;
			}

			*string = remap[Min(c, 127)];
		}
	}
}


Collada::Image::Image() : Collada::Element(Collada::kElementImage)
{
	sourceString[0] = 0;
	imageName[0] = 0;
}

Collada::Image::~Image()
{
}

unsigned_int32 Collada::Image::GetHexValue(unsigned_int32 c)
{
	if (c - '0' < 10U)
	{
		return (c - '0');
	}

	if (c - 'A' < 6U)
	{
		return (c - 'A' + 10);
	}

	if (c - 'a' < 6U)
	{
		return (c - 'a' + 10);
	}

	return (0);
}

void Collada::Image::Preprocess(void)
{
	Collada::Element::Preprocess();

	const char *name = sourceString;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementInitFrom)
		{
			name = static_cast<const Collada::InitFrom *>(element)->GetInitString();
			break;
		}

		element = element->Next();
	}

	int32 len = Text::GetTextLength(name);
	int32 end = len;
	int32 start = end - 1;

	for (; start >= 0; start--)
	{
		unsigned_int32 c = name[start];
		if (c == '/')
		{
			break;
		}

		if ((c == '.') && (end == len))
		{
			end = start;
		}
	}

	for (machine a = 0; a < start; a++)
	{
		if (Text::CompareTextCaseless(&name[a], "/Import/", 8) != 0)
		{
			start = a + 7;
			break;
		}
	}

	len = Min(end - ++start, kMaxIdentifierLength);
	name += start;

	char *output = imageName;
	for (machine a = 0; a < len; a++)
	{
		unsigned_int32 c = name[a];
		if ((c == '%') && (a <= len - 3))
		{
			*output = (char) ((GetHexValue(name[a + 1]) << 4) + GetHexValue(name[a + 2]));
			a += 2;
		}
		else
		{
			*output = (char) c;
		}

		output++;
	}

	*output = 0;
}


Collada::Texture::Texture(ColladaContext *context) : Collada::MaterialAttributeElement(Collada::kElementTexture, context)
{
	imageElement = nullptr;
	imageIdentifier[0] = 0;
}

Collada::Texture::~Texture()
{
}

void Collada::Texture::Preprocess(void)
{
	Collada::MaterialAttributeElement::Preprocess();

	imageElement = FindElement(imageIdentifier);
	if (!imageElement)
	{
		imageElement = FindSubElement(imageIdentifier);
	}
}

void Collada::Texture::BuildAttributeList(List<C4::Attribute> *attributeList) const
{
	Collada::ParamType textureType = kParamDiffuse;
	const Collada::Image *textureImage = nullptr;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementParam)
		{
			textureType = static_cast<const Collada::Param *>(element)->GetParamType();
		}
		else if (type == Collada::kElementTechnique)
		{
			const Collada::Technique *technique = static_cast<const Collada::Technique *>(element);
			if (technique->GetTechniqueType() == Collada::kTechniqueCommon)
			{
				const Collada::Element *subelement = technique->GetFirstSubnode();
				while (subelement)
				{
					if (subelement->GetElementType() == Collada::kElementInput)
					{
						const Collada::Input *input = static_cast<const Collada::Input *>(subelement);
						if (input->GetInputSemantic() == Collada::kSemanticImage)
						{
							const Collada::Element *image = input->GetSourceElement();
							if (image)
							{
								textureImage = static_cast<const Collada::Image *>(image);
								break;
							}
						}
					}

					subelement = subelement->Next();
				}
			}
		}

		element = element->Next();
	}

	if (textureImage)
	{
		C4::Attribute *attribute = GetColladaContext()->NewTextureAttribute(textureType, textureImage->GetImageName(), nullptr);
		if (attribute)
		{
			attributeList->Append(attribute);
		}
	}
}


Collada::Material::Material(ColladaContext *context) : Collada::Element(Collada::kElementMaterial, context)
{
	materialObject = nullptr;
	materialName[0] = 0;
}

Collada::Material::~Material()
{
	if (materialObject)
	{
		materialObject->Release();
	}
}

C4::MaterialObject *Collada::Material::GetMaterialObject(void)
{
	if (!materialObject)
	{
		const ColladaContext *context = GetColladaContext();
		unsigned_int32 flags = context->GetImportFlags();

		if ((flags & kGeometryImportReuseNamedMaterials) && (materialName[0] != 0))
		{
			materialObject = context->GetEditorObject()->FindNamedMaterial(materialName);
			if (materialObject)
			{
				materialObject->Retain();
				return (materialObject);
			}
		}

		const Collada::Element *element = GetFirstSubnode();
		while (element)
		{
			Collada::ElementType type = element->GetElementType();
			if (type == Collada::kElementInstance)
			{
				const Collada::Instance *instance = static_cast<const Collada::Instance *>(element);
				const Collada::Element *resource = instance->GetResourceElement();
				if ((resource) && (resource->GetElementType() == Collada::kElementEffect))
				{
					const Collada::Effect *effect = static_cast<const Collada::Effect *>(resource);
					materialObject = effect->BuildMaterial();
					break;
				}
			}
			else if (type == Collada::kElementShader)
			{
				const Collada::Shader *shader = static_cast<const Collada::Shader *>(element);
				materialObject = shader->BuildMaterial();
				break;
			}

			element = element->Next();
		}

		if ((flags & kGeometryImportMergeMaterials) && (materialObject))
		{
			MaterialObject *object = context->GetEditorObject()->FindMatchingMaterial(materialObject);
			if (object)
			{
				materialObject->Release();
				materialObject = object;
				object->Retain();
			}
		}
	}

	return (materialObject);
}


Collada::BindMaterial::BindMaterial() : Collada::Element(Collada::kElementBindMaterial)
{
}

Collada::BindMaterial::~BindMaterial()
{
}

void Collada::BindMaterial::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementTechnique)
		{
			Collada::Element *subelement = element->GetFirstSubnode();
			while (subelement)
			{
				if (subelement->GetElementType() == Collada::kElementInstance)
				{
					Collada::Instance *instance = static_cast<Collada::Instance *>(subelement);
					const Collada::Element *resource = instance->GetResourceElement();
					if ((resource) && (resource->GetElementType() == Collada::kElementMaterial))
					{
						instanceList.Append(instance);
					}
				}

				subelement = subelement->Next();
			}
		}

		element = element->Next();
	}
}


Collada::Shader::Shader() : Collada::Element(Collada::kElementShader)
{
}

Collada::Shader::~Shader()
{
}

C4::MaterialObject *Collada::Shader::BuildMaterial(void) const
{
	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementTechnique)
		{
			const Collada::Technique *technique = static_cast<const Collada::Technique *>(element);
			if (technique->GetTechniqueType() == Collada::kTechniqueCommon)
			{
				const Collada::Element *subelement = technique->GetFirstSubnode();
				while (subelement)
				{
					if (subelement->GetElementType() == Collada::kElementPass)
					{
						const Collada::Pass *pass = static_cast<const Collada::Pass *>(subelement);
						return (pass->BuildMaterial());
					}

					subelement = subelement->Next();
				}
			}
		}

		element = element->Next();
	}

	return (nullptr);
}


Collada::Pass::Pass() : Collada::Element(Collada::kElementPass)
{
}

Collada::Pass::~Pass()
{
}

C4::MaterialObject *Collada::Pass::BuildMaterial(void) const
{
	C4::MaterialObject *material = new C4::MaterialObject;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		const Collada::Element *builder = nullptr;

		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementProgram)
		{
			builder = element;
		}
		else if (type == Collada::kElementInput)
		{
			const Collada::Input *input = static_cast<const Collada::Input *>(element);
			if (input->GetInputSemantic() == Collada::kSemanticTexture)
			{
				const Collada::Element *texture = input->GetSourceElement();
				if ((texture) && (texture->GetElementType() == Collada::kElementTexture))
				{
					builder = texture;
				}
			}
		}

		if (builder)
		{
			static_cast<const MaterialAttributeElement *>(builder)->BuildAttributeList(material->GetAttributeList());
		}

		element = element->Next();
	}

	return (material);
}


Collada::Program::Program() : Collada::MaterialAttributeElement(Collada::kElementProgram)
{
}

Collada::Program::~Program()
{
}

void Collada::Program::BuildAttributeList(List<C4::Attribute> *attributeList) const
{
	C4::DiffuseAttribute *diffuse = nullptr;
	C4::SpecularAttribute *specular = nullptr;
	C4::EmissionAttribute *emission = nullptr;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementParam)
		{
			const Collada::Param *param = static_cast<const Collada::Param *>(element);
			ParamType type = param->GetParamType();

			if (type == Collada::kParamDiffuse)
			{
				const ColorRGB& color = param->GetColorValue();
				if (color != K::white)
				{
					if (diffuse)
					{
						diffuse->SetDiffuseColor(color);
					}
					else
					{
						diffuse = new DiffuseAttribute(color);
						attributeList->Append(diffuse);
					}
				}
			}
			else if (type == Collada::kParamSpecular)
			{
				const ColorRGB& color = param->GetColorValue();
				if (color != K::black)
				{
					if (specular)
					{
						specular->SetSpecularColor(color);
					}
					else
					{
						specular = new SpecularAttribute(color, 50.0F);
						attributeList->Append(specular);
					}
				}
			}
			else if (type == Collada::kParamShininess)
			{
				float exponent = param->GetFloatValue(0);
				if (exponent >= 10.0F)
				{
					if (specular)
					{
						specular->SetSpecularExponent(exponent);
					}
					else
					{
						specular = new SpecularAttribute(K::white, exponent);
						attributeList->Append(specular);
					}
				}
			}
			else if (type == Collada::kParamEmission)
			{
				const ColorRGB& color = param->GetColorValue();
				if (color != K::black)
				{
					if (emission)
					{
						emission->SetEmissionColor(color);
					}
					else
					{
						emission = new EmissionAttribute(color);
						attributeList->Append(emission);
					}
				}
			}
		}

		element = element->Next();
	}
}


Collada::ShadingModel::ShadingModel(ColladaContext *context) : Collada::Element(Collada::kElementShadingModel, context)
{
}

Collada::ShadingModel::~ShadingModel()
{
}

C4::MaterialObject *Collada::ShadingModel::BuildMaterial(void) const
{
	C4::MaterialObject *material = new C4::MaterialObject;

	C4::DiffuseAttribute *diffuse = nullptr;
	C4::SpecularAttribute *specular = nullptr;
	C4::EmissionAttribute *emission = nullptr;

	const Collada::Element *root = this;
	for (machine a = 0; a < 2; a++)
	{
		const Collada::Element *element = root->GetFirstSubnode();
		while (element)
		{
			if (element->GetElementType() == Collada::kElementMaterialProperty)
			{
				Collada::ParamType propertyType = static_cast<const Collada::MaterialProperty *>(element)->GetPropertyType();

				const Collada::Element *subelement = element->GetFirstSubnode();
				while (subelement)
				{
					Collada::ElementType type = subelement->GetElementType();
					if (type == Collada::kElementParam)
					{
						const Collada::Param *param = static_cast<const Collada::Param *>(subelement);
						Collada::ParamType paramType = param->GetParamType();

						if (paramType == Collada::kParamFloat)
						{
							if (propertyType == Collada::kParamShininess)
							{
								float exponent = param->GetFloatValue(0);
								if (exponent >= 10.0F)
								{
									if (specular)
									{
										specular->SetSpecularExponent(exponent);
									}
									else
									{
										specular = new SpecularAttribute(K::white, exponent);
										material->AddAttribute(specular);
									}
								}
							}
						}
						else if (paramType == Collada::kParamColor)
						{
							const ColorRGB& color = param->GetColorValue();

							if (propertyType == Collada::kParamDiffuse)
							{
								if (color != K::white)
								{
									if (diffuse)
									{
										diffuse->SetDiffuseColor(color);
									}
									else
									{
										diffuse = new DiffuseAttribute(color);
										material->AddAttribute(diffuse);
									}
								}
							}
							else if (propertyType == Collada::kParamSpecular)
							{
								if (color != K::black)
								{
									if (specular)
									{
										specular->SetSpecularColor(color);
									}
									else
									{
										specular = new SpecularAttribute(color, 50.0F);
										material->AddAttribute(specular);
									}
								}
							}
							else if (propertyType == Collada::kParamEmission)
							{
								if (color != K::black)
								{
									if (emission)
									{
										emission->SetEmissionColor(color);
									}
									else
									{
										emission = new EmissionAttribute(color);
										material->AddAttribute(emission);
									}
								}
							}
						}
					}
					else if (type == Collada::kElementTexture)
					{
						const Collada::Texture *texture = static_cast<const Collada::Texture *>(subelement);
						const Collada::Element *image = texture->GetImageElement();
						if (image)
						{
							const Collada::Sampler2D *sampler2D = nullptr;

							if (image->GetElementType() == Collada::kElementNewParam)
							{
								sampler2D = static_cast<const Collada::NewParam *>(image)->GetSampler2DElement();
								if (sampler2D)
								{
									const Collada::Surface *surface = sampler2D->GetSurfaceElement();
									if (surface)
									{
										image = surface->GetImageElement();
									}
								}
							}

							if ((image) && (image->GetElementType() == Collada::kElementImage))
							{
								const char *name = static_cast<const Collada::Image *>(image)->GetImageName();
								C4::Attribute *attribute = GetColladaContext()->NewTextureAttribute(propertyType, name, sampler2D);
								if (attribute)
								{
									material->AddAttribute(attribute);
								}
							}
						}
					}

					subelement = subelement->Next();
				}
			}

			element = element->Next();
		}

		root = root->Next();
		if ((!root) || (root->GetElementType() != Collada::kElementExtra))
		{
			break;
		}

		root = root->GetFirstSubnode();
		if ((!root) || (root->GetElementType() != Collada::kElementTechnique))
		{
			break;
		}
	}

	return (material);
}


Collada::Effect::Effect() : Collada::Element(Collada::kElementEffect)
{
}

Collada::Effect::~Effect()
{
}

C4::MaterialObject *Collada::Effect::BuildMaterial(void) const
{
	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementCommonProfile)
		{
			const Collada::Element *subelement = element->GetFirstSubnode();
			while (subelement)
			{
				if (subelement->GetElementType() == Collada::kElementTechnique)
				{
					break;
				}

				subelement = subelement->Next();
			}

			if (subelement)
			{
				subelement = subelement->GetFirstSubnode();
				while (subelement)
				{
					if (subelement->GetElementType() == Collada::kElementShadingModel)
					{
						const Collada::ShadingModel *shadingModel = static_cast<const Collada::ShadingModel *>(subelement);
						return (shadingModel->BuildMaterial());
					}

					subelement = subelement->Next();
				}
			}
		}

		element = element->Next();
	}

	return (nullptr);
}


Collada::CommonProfile::CommonProfile() : Collada::Element(Collada::kElementCommonProfile)
{
}

Collada::CommonProfile::~CommonProfile()
{
}


Collada::MaterialProperty::MaterialProperty(Collada::ParamType type) : Collada::Element(Collada::kElementMaterialProperty)
{
	propertyType = type;
}

Collada::MaterialProperty::~MaterialProperty()
{
}


Collada::Param::Param(ParamType type) : Collada::Element(Collada::kElementParam)
{
	paramType = type;

	for (machine a = 0; a < 4; a++)
	{
		floatValue[a] = 0.0F;
	}
}

Collada::Param::~Param()
{
}

void Collada::Param::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	for (machine count = 0; count < 4; count++)
	{
		ReadWhitespace(text);
		if (!ReadString(text, &string))
		{
			break;
		}

		floatValue[count] = Text::StringToFloat(string);
	}
}


Collada::Surface::Surface() : Collada::Element(Collada::kElementSurface)
{
	imageElement = nullptr;
}

Collada::Surface::~Surface()
{
}

void Collada::Surface::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementInitFrom)
		{
			const char *string = static_cast<const Collada::InitFrom *>(element)->GetInitString();
			const Collada::Element *initElement = FindElement(string);
			if ((initElement) && (initElement->GetElementType() == Collada::kElementImage))
			{
				imageElement = static_cast<const Collada::Image *>(initElement);
			}

			break;
		}

		element = element->Next();
	}
}


Collada::Sampler2D::Sampler2D() : Collada::Element(Collada::kElementSampler2D)
{
	surfaceElement = nullptr;

	wrapMode[0] = kTextureRepeat;
	wrapMode[1] = kTextureRepeat;
}

Collada::Sampler2D::~Sampler2D()
{
}

void Collada::Sampler2D::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();

		if (type == Collada::kElementSource)
		{
			if (!surfaceElement)
			{
				const Collada::NewParam *newParam = static_cast<const Collada::Source *>(element)->GetNewParamElement();
				if (newParam)
				{
					surfaceElement = newParam->GetSurfaceElement();
				}
			}
		}
		else if (type == Collada::kElementWrap)
		{
			const Collada::Wrap *wrap = static_cast<const Collada::Wrap *>(element);
			wrapMode[wrap->GetTextureCoord()] = wrap->GetWrapMode();
		}

		element = element->Next();
	}
}


Collada::Wrap::Wrap(int32 coord) : Collada::Element(Collada::kElementWrap)
{
	textureCoord = coord;
	wrapMode = kTextureRepeat;
}

Collada::Wrap::~Wrap()
{
}

void Collada::Wrap::ProcessContent(const char *& text)
{
	String<Collada::kMaxStringLength>	string;

	ReadWhitespace(text);
	if (ReadString(text, &string))
	{
		if (string == "CLAMP")
		{
			wrapMode = kTextureClamp;
		}
		else if (string == "BORDER")
		{
			wrapMode = kTextureClampBorder;
		}
		else if (string == "MIRROR")
		{
			wrapMode = kTextureMirrorRepeat;
		}
	}
}


Collada::NewParam::NewParam() : Collada::Element(Collada::kElementNewParam)
{
	surfaceElement = nullptr;
	sampler2DElement = nullptr;
}

Collada::NewParam::~NewParam()
{
}

void Collada::NewParam::Preprocess(void)
{
	Collada::Element::Preprocess();

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementSurface)
		{
			surfaceElement = static_cast<const Collada::Surface *>(element);
		}
		else if (type == Collada::kElementSampler2D)
		{
			sampler2DElement = static_cast<const Collada::Sampler2D *>(element);
		}

		element = element->Next();
	}
}


Collada::Light::Light() : Collada::Element(Collada::kElementLight)
{
}

Collada::Light::~Light()
{
}

C4::Light *Collada::Light::BuildLight(void) const
{
	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementTechnique)
		{
			const Collada::Technique *technique = static_cast<const Collada::Technique *>(element);
			if (technique->GetTechniqueType() == Collada::kTechniqueCommon)
			{
				const Collada::Element *subelement = technique->GetFirstSubnode();
				while (subelement)
				{
					if (subelement->GetElementType() == Collada::kElementLightData)
					{
						return (static_cast<const Collada::LightData *>(subelement)->BuildLight());
					}

					subelement = subelement->Next();
				}
			}
		}

		element = element->Next();
	}

	return (nullptr);
}


Collada::LightData::LightData(LightDataType type) : Collada::Element(Collada::kElementLightData)
{
	lightDataType = type;
}

Collada::LightData::~LightData()
{
}


Collada::Directional::Directional() : Collada::LightData(Collada::kLightDataDirectional)
{
}

Collada::Directional::~Directional()
{
}

C4::Light *Collada::Directional::BuildLight(void) const
{
	ColorRGB color(1.0F, 1.0F, 1.0F);

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementParam)
		{
			const Collada::Param *param = static_cast<const Collada::Param *>(element);
			ParamType type = param->GetParamType();

			if (type == Collada::kParamColor)
			{
				color = param->GetColorValue();
			}
		}

		element = element->Next();
	}

	return (new C4::InfiniteLight(color));
}


Collada::Point::Point() : Collada::LightData(Collada::kLightDataPoint)
{
}

Collada::Point::~Point()
{
}

C4::Light *Collada::Point::BuildLight(void) const
{
	ColorRGB color(1.0F, 1.0F, 1.0F);
	float range = 2.0F;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementParam)
		{
			const Collada::Param *param = static_cast<const Collada::Param *>(element);
			ParamType type = param->GetParamType();

			if (type == Collada::kParamColor)
			{
				color = param->GetColorValue();
			}
		}

		element = element->Next();
	}

	return (new C4::PointLight(color, range));
}


Collada::Spot::Spot() : Collada::LightData(Collada::kLightDataSpot)
{
}

Collada::Spot::~Spot()
{
}

C4::Light *Collada::Spot::BuildLight(void) const
{
	ColorRGB color(1.0F, 1.0F, 1.0F);
	float range = 2.0F;
	float apex = 1.0F;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementParam)
		{
			const Collada::Param *param = static_cast<const Collada::Param *>(element);
			ParamType type = param->GetParamType();

			if (type == Collada::kParamColor)
			{
				color = param->GetColorValue();
			}
			else if (type == Collada::kParamFalloffAngle)
			{
				apex = Tan(param->GetFloatValue(0) * 0.5F);
			}
		}

		element = element->Next();
	}

	return (new C4::SpotLight(color, range, apex, "C4/spot"));
}


Collada::Camera::Camera() : Collada::Element(Collada::kElementCamera)
{
}

Collada::Camera::~Camera()
{
}

C4::Camera *Collada::Camera::BuildCamera(void) const
{
	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementOptics)
		{
			const Collada::Element *subelement = element->GetFirstSubnode();
			while (subelement)
			{
				if (subelement->GetElementType() == Collada::kElementPerspective)
				{
					return (static_cast<const Collada::Perspective *>(subelement)->BuildCamera());
				}

				subelement = element->GetNextNode(subelement);
			}
		}

		element = element->Next();
	}

	return (nullptr);
}


Collada::Optics::Optics() : Collada::Element(Collada::kElementOptics)
{
}

Collada::Optics::~Optics()
{
}


Collada::Perspective::Perspective() : Collada::Element(Collada::kElementPerspective)
{
}

Collada::Perspective::~Perspective()
{
}

C4::Camera *Collada::Perspective::BuildCamera(void) const
{
	const Collada::Param *aspectElement = nullptr;
	const Collada::Param *xfovElement = nullptr;
	const Collada::Param *yfovElement = nullptr;

	float nearDepth = 0.1F;
	float farDepth = 10.0F;

	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		if (element->GetElementType() == Collada::kElementParam)
		{
			const Collada::Param *param = static_cast<const Collada::Param *>(element);
			switch (param->GetParamType())
			{
				case Collada::kParamAspectRatio:

					aspectElement = param;
					break;

				case Collada::kParamXFov:

					xfovElement = param;
					break;

				case Collada::kParamYFov:

					yfovElement = param;
					break;

				case Collada::kParamZNear:

					nearDepth = param->GetFloatValue(0);
					break;

				case Collada::kParamZFar:

					farDepth = param->GetFloatValue(0);
					break;
			}
		}

		element = element->Next();
	}

	float focal = 1.0F;
	float aspect = 1.0F;

	if (aspectElement)
	{
		aspect = 1.0F / Fmax(aspectElement->GetFloatValue(0), 0.125F);

		if (xfovElement)
		{
			float angle = Fmax(xfovElement->GetFloatValue(0), 1.0F);
			focal = 1.0F / Tan(angle * K::radians * 0.5F);
		}
		else if (yfovElement)
		{
			float angle = Fmax(yfovElement->GetFloatValue(0), 1.0F);
			focal = aspect / Tan(angle * K::radians * 0.5F);
		}
	}
	else if (xfovElement)
	{
		if (yfovElement)
		{
			float x = Fmax(xfovElement->GetFloatValue(0), 1.0F);
			float y = Fmax(yfovElement->GetFloatValue(0), 1.0F);

			focal = 1.0F / Tan(x * K::radians * 0.5F);
			aspect = Tan(y * K::radians * 0.5F) * focal;
		}
		else
		{
			float angle = Fmax(xfovElement->GetFloatValue(0), 1.0F);
			focal = 1.0F / Tan(angle * K::radians * 0.5F);
		}
	}

	C4::FrustumCamera *camera = new C4::FrustumCamera(focal, aspect);
	C4::FrustumCameraObject *object = camera->GetObject();
	object->SetFrustumFlags(kFrustumInfinite);
	object->SetNearDepth(Fmax(nearDepth, 0.1F));
	object->SetFarDepth(Fmax(farDepth, 1.0F));

	return (camera);
}


Collada::Root::Root() : Collada::Element(Collada::kElementRoot)
{
	upAxisElement = nullptr;
	unitElement = nullptr;
}

Collada::Root::~Root()
{
}

Collada::Scene *Collada::Root::FindSceneElement(void) const
{
	Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementScene)
		{
			return (static_cast<Collada::Scene *>(element));
		}

		element = element->Next();
	}

	return (nullptr);
}

const Collada::Library *Collada::Root::FindAnimationElement(void) const
{
	const Collada::Element *element = GetFirstSubnode();
	while (element)
	{
		Collada::ElementType type = element->GetElementType();
		if (type == Collada::kElementLibrary)
		{
			const Collada::Library *library = static_cast<const Collada::Library *>(element);
			if (library->GetLibraryType() == Collada::kLibraryAnimation)
			{
				return (library);
			}
		}

		element = element->Next();
	}

	return (nullptr);
}


ColladaResource::ColladaResource(const char *name, ResourceCatalog *catalog) : Resource<ColladaResource>(name, catalog)
{
}

ColladaResource::~ColladaResource()
{
}


ColladaContext::ColladaContext(const EditorObject *object, unsigned_int32 flags, float scale)
{
	editorObject = object;
	importFlags = flags;
	importScale = scale;
}

ColladaContext::~ColladaContext()
{
}

Collada::Element *ColladaContext::CreateElement(const String<Collada::kMaxIdentifierLength>& elementType)
{
	if (elementType == "asset") return (new Collada::Asset);
	if ((elementType == "scene") || (elementType == "visual_scene")) return (new Collada::Scene);
	if (elementType == "technique") return (new Collada::Technique);
	if (elementType == "technique_common") return (new Collada::Technique(Collada::kTechniqueCommon));
	if (elementType == "extra") return (new Collada::Element(Collada::kElementExtra));
	if (elementType == "mesh") return (new Collada::Mesh);
	if (elementType == "geometry") return (new Collada::Geometry);
	if (elementType == "controller") return (new Collada::Controller);
	if ((elementType == "Name_array") || (elementType == "IDREF_array")) return (new Collada::NameArray);
	if (elementType == "float_array") return (new Collada::FloatArray);
	if (elementType == "accessor") return (new Collada::Accessor);
	if (elementType == "source") return (new Collada::Source);
	if (elementType == "input") return (new Collada::Input);
	if (elementType == "sampler") return (new Collada::Sampler);
	if (elementType == "channel") return (new Collada::Channel);
	if (elementType == "animation") return (new Collada::Animation);

	if (Text::CompareText(elementType, "library", 7))
	{
		const char *type = &elementType[7];
		if (type[0] == 0)
		{
			return (new Collada::Library);
		}

		if (Text::CompareText(type, "_geometries")) return (new Collada::Library(Collada::kLibraryGeometry));
		if (Text::CompareText(type, "_materials")) return (new Collada::Library(Collada::kLibraryMaterial));
		if (Text::CompareText(type, "_images")) return (new Collada::Library(Collada::kLibraryImage));
		if (Text::CompareText(type, "_effects")) return (new Collada::Library(Collada::kLibraryEffect));
		if (Text::CompareText(type, "_lights")) return (new Collada::Library(Collada::kLibraryLight));
		if (Text::CompareText(type, "_controllers")) return (new Collada::Library(Collada::kLibraryController));
		if (Text::CompareText(type, "_animations")) return (new Collada::Library(Collada::kLibraryAnimation));
		if (Text::CompareText(type, "_visual_scenes")) return (new Collada::Library(Collada::kLibraryVisualScene));
	}

	if (elementType == "joints") return (new Collada::Joints);
	if ((elementType == "combiner") || (elementType == "vertex_weights")) return (new Collada::Combiner);
	if (elementType == "vertices") return (new Collada::Vertices);
	if (elementType == "bind_shape_matrix") return (new Collada::BindShapeMatrix);
	if (elementType == "skin") return (new Collada::Skin);
	if (elementType == "targets") return (new Collada::Targets);
	if (elementType == "morph") return (new Collada::Morph);
	if (elementType == "triangles") return (new Collada::Faces(Collada::kFacesTriangles));
	if (elementType == "polygons") return (new Collada::Faces(Collada::kFacesPolygons));
	if (elementType == "polylist") return (new Collada::Faces(Collada::kFacesPolylist));
	if ((elementType == "p") || (elementType == "v")) return (new Collada::IndexArray);
	if (elementType == "vcount") return (new Collada::CountArray);
	if (elementType == "control_vertices") return (new Collada::ControlVertices);
	if (elementType == "spline") return (new Collada::Spline);

	if (Text::CompareText(elementType, "instance", 8))
	{
		const char *type = &elementType[8];
		if ((type[0] == 0) || (Text::CompareText(type, "_geometry"))
						   || (Text::CompareText(type, "_visual_scene"))
						   || (Text::CompareText(type, "_controller"))
						   || (Text::CompareText(type, "_light"))
						   || (Text::CompareText(type, "_camera"))
						   || (Text::CompareText(type, "_effect"))
						   || (Text::CompareText(type, "_material")))
		{
			return (new Collada::Instance);
		}
	}

	if (elementType == "node") return (new Collada::Node);
	if (elementType == "matrix") return (new Collada::Matrix);
	if (elementType == "translate") return (new Collada::Translate);
	if (elementType == "rotate") return (new Collada::Rotate);
	if (elementType == "scale") return (new Collada::Scale);
	if (elementType == "lookat") return (new Collada::Lookat);
	if (elementType == "up_axis") return (new Collada::UpAxis);
	if (elementType == "unit") return (new Collada::Unit);
	if (elementType == "init_from") return (new Collada::InitFrom);
	if (elementType == "image") return (new Collada::Image);
	if (elementType == "texture") return (new Collada::Texture(this));
	if (elementType == "material") return (new Collada::Material(this));
	if (elementType == "bind_material") return (new Collada::BindMaterial);
	if (elementType == "shader") return (new Collada::Shader);
	if (elementType == "pass") return (new Collada::Pass);
	if (elementType == "program") return (new Collada::Program);
	if ((elementType == "lambert") || (elementType == "phong") || (elementType == "blinn")) return (new Collada::ShadingModel(this));
	if (elementType == "effect") return (new Collada::Effect);
	if (elementType == "profile_COMMON") return (new Collada::CommonProfile);
	if (elementType == "diffuse") return (new Collada::MaterialProperty(Collada::kParamDiffuse));
	if (elementType == "specular") return (new Collada::MaterialProperty(Collada::kParamSpecular));
	if (elementType == "shininess") return (new Collada::MaterialProperty(Collada::kParamShininess));
	if (elementType == "emission") return (new Collada::MaterialProperty(Collada::kParamEmission));
	if (elementType == "bump") return (new Collada::MaterialProperty(Collada::kParamBump));
	if (elementType == "param") return (new Collada::Param);
	if (elementType == "float") return (new Collada::Param(Collada::kParamFloat));
	if (elementType == "color") return (new Collada::Param(Collada::kParamColor));
	if (elementType == "falloff_angle") return (new Collada::Param(Collada::kParamFalloffAngle));
	if (elementType == "surface") return (new Collada::Surface);
	if (elementType == "sampler2D") return (new Collada::Sampler2D);
	if (elementType == "wrap_s") return (new Collada::Wrap(0));
	if (elementType == "wrap_t") return (new Collada::Wrap(1));
	if (elementType == "newparam") return (new Collada::NewParam);
	if (elementType == "light") return (new Collada::Light);
	if (elementType == "directional") return (new Collada::Directional);
	if (elementType == "point") return (new Collada::Point);
	if (elementType == "spot") return (new Collada::Spot);
	if (elementType == "camera") return (new Collada::Camera);
	if (elementType == "optics") return (new Collada::Optics);
	if (elementType == "perspective") return (new Collada::Perspective);
	if (elementType == "aspect_ratio") return (new Collada::Param(Collada::kParamAspectRatio));
	if (elementType == "xfov") return (new Collada::Param(Collada::kParamXFov));
	if (elementType == "yfov") return (new Collada::Param(Collada::kParamYFov));
	if (elementType == "znear") return (new Collada::Param(Collada::kParamZNear));
	if (elementType == "zfar") return (new Collada::Param(Collada::kParamZNear));
	if (elementType == "COLLADA") return (new Collada::Root);

	return (new Collada::Element);
}

bool ColladaContext::ProcessAttributes(const char *& text, Collada::Element *element)
{
	for (;;)
	{
		String<Collada::kMaxIdentifierLength>	identifier;
		String<Collada::kMaxIdentifierLength>	attribute;

		ReadWhitespace(text);
		if (!ReadIdentifier(text, &identifier))
		{
			break;
		}

		ReadWhitespace(text);
		if (*text != '=')
		{
			break;
		}

		text++;

		ReadWhitespace(text);
		if (!ReadAttribute(text, &attribute))
		{
			break;
		}

		Collada::ElementType type = element->GetElementType();

		if (identifier == "id")
		{
			element->SetIdentifier(attribute);
		}
		else if (identifier == "sid")
		{
			element->SetSubIdentifier(attribute);
		}
		else if (identifier == "type")
		{
			if (type == Collada::kElementLibrary)
			{
				Collada::LibraryType libraryType = 0;

				if (attribute == "GEOMETRY") libraryType = Collada::kLibraryGeometry;
				else if (attribute == "MATERIAL") libraryType = Collada::kLibraryMaterial;
				else if (attribute == "IMAGE") libraryType = Collada::kLibraryImage;
				else if (attribute == "TEXTURE") libraryType = Collada::kLibraryTexture;
				else if (attribute == "LIGHT") libraryType = Collada::kLibraryLight;
				else if (attribute == "CONTROLLER") libraryType = Collada::kLibraryController;
				else if (attribute == "ANIMATION") libraryType = Collada::kLibraryAnimation;

				static_cast<Collada::Library *>(element)->SetLibraryType(libraryType);
			}
			else if (type == Collada::kElementNode)
			{
				if (attribute == "JOINT")
				{
					static_cast<Collada::Node *>(element)->SetNodeType(Collada::kNodeJoint);
				}
			}
		}
		else if (identifier == "name")
		{
			if (type == Collada::kElementGeometry)
			{
				static_cast<Collada::Geometry *>(element)->SetGeometryName(attribute);
			}
			else if (type == Collada::kElementNode)
			{
				static_cast<Collada::Node *>(element)->SetNodeName(attribute);
			}
			else if (type == Collada::kElementMaterial)
			{
				static_cast<Collada::Material *>(element)->SetMaterialName(attribute);
			}
			else if (type == Collada::kElementParam)
			{
				Collada::ParamType paramType = 0;

				if (attribute == "COLOR") paramType = Collada::kParamColor;
				else if (attribute == "DIFFUSE") paramType = Collada::kParamDiffuse;
				else if (attribute == "SPECULAR") paramType = Collada::kParamSpecular;
				else if (attribute == "EMISSION") paramType = Collada::kParamEmission;
				else if (attribute == "SHININESS") paramType = Collada::kParamShininess;
				else if (attribute == "BUMP") paramType = Collada::kParamBump;

				if (paramType != 0)
				{
					static_cast<Collada::Param *>(element)->SetParamType(paramType);
				}
			}
		}
		else if (identifier == "count")
		{
			if (type == Collada::kElementNameArray)
			{
				static_cast<Collada::NameArray *>(element)->SetArraySize(Text::StringToInteger(attribute));
			}
			else if (type == Collada::kElementFloatArray)
			{
				static_cast<Collada::FloatArray *>(element)->SetArraySize(Text::StringToInteger(attribute));
			}
			else if (type == Collada::kElementAccessor)
			{
				static_cast<Collada::Accessor *>(element)->SetAccessCount(Text::StringToInteger(attribute));
			}
			else if ((type == Collada::kElementFaces) || (type == Collada::kElementCombiner))
			{
				static_cast<Collada::InterleavingElement *>(element)->SetItemCount(Text::StringToInteger(attribute));
			}
		}
		else if (identifier == "stride")
		{
			if (type == Collada::kElementAccessor)
			{
				static_cast<Collada::Accessor *>(element)->SetAccessStride(Text::StringToInteger(attribute));
			}
		}
		else if (identifier == "source")
		{
			if (type == Collada::kElementAccessor)
			{
				static_cast<Collada::Accessor *>(element)->SetSourceIdentifier(attribute);
			}
			else if (type == Collada::kElementInput)
			{
				static_cast<Collada::Input *>(element)->SetSourceIdentifier(attribute);
			}
			else if (type == Collada::kElementChannel)
			{
				static_cast<Collada::Channel *>(element)->SetSourceIdentifier(attribute);
			}
			else if (type == Collada::kElementSkin)
			{
				static_cast<Collada::Skin *>(element)->SetSourceIdentifier(attribute);
			}
			else if (type == Collada::kElementImage)
			{
				static_cast<Collada::Image *>(element)->SetSourceString(attribute);
			}
		}
		else if ((identifier == "idx") || (identifier == "offset"))
		{
			if (type == Collada::kElementInput)
			{
				static_cast<Collada::Input *>(element)->SetInputIndex(Text::StringToInteger(attribute));
			}
		}
		else if (identifier == "semantic")
		{
			if (type == Collada::kElementInput)
			{
				Collada::SemanticType semantic = 0;

				if (attribute == "VERTEX") semantic = Collada::kSemanticVertex;
				else if (attribute == "POSITION") semantic = Collada::kSemanticPosition;
				else if (attribute == "NORMAL") semantic = Collada::kSemanticNormal;
				else if (attribute == "COLOR") semantic = Collada::kSemanticColor;
				else if (attribute == "TEXCOORD") semantic = Collada::kSemanticTexcoord;
				else if (attribute == "JOINT") semantic = Collada::kSemanticJoint;
				else if (attribute == "WEIGHT") semantic = Collada::kSemanticWeight;
				else if (attribute == "INV_BIND_MATRIX") semantic = Collada::kSemanticInvBindMatrix;
				else if (attribute == "BIND_SHAPE_POSITION") semantic = Collada::kSemanticBindPosition;
				else if (attribute == "BIND_SHAPE_NORMAL") semantic = Collada::kSemanticBindNormal;
				else if (attribute == "JOINTS_AND_WEIGHTS") semantic = Collada::kSemanticJointWeight;
				else if (attribute == "MORPH_TARGET") semantic = Collada::kSemanticMorphTarget;
				else if (attribute == "MORPH_WEIGHT") semantic = Collada::kSemanticMorphWeight;
				else if (attribute == "IMAGE") semantic = Collada::kSemanticImage;
				else if (attribute == "TEXTURE") semantic = Collada::kSemanticTexture;
				else if (attribute == "INPUT") semantic = Collada::kSemanticInput;
				else if (attribute == "OUTPUT") semantic = Collada::kSemanticOutput;
				else if (attribute == "INTERPOLATION") semantic = Collada::kSemanticInterpolation;
				else if (attribute == "IN_TANGENT") semantic = Collada::kSemanticInTangent;
				else if (attribute == "OUT_TANGENT") semantic = Collada::kSemanticOutTangent;

				static_cast<Collada::Input *>(element)->SetInputSemantic(semantic);
			}
		}
		else if (identifier == "set")
		{
			if (type == Collada::kElementInput)
			{
				static_cast<Collada::Input *>(element)->SetTexcoordSet(Text::StringToInteger(attribute));
			}
		}
		else if (identifier == "symbol")
		{
			if (type == Collada::kElementInstance)
			{
				static_cast<Collada::Instance *>(element)->SetSymbolIdentifier(attribute);
			}
		}
		else if (identifier == "texture")
		{
			if (type == Collada::kElementTexture)
			{
				static_cast<Collada::Texture *>(element)->SetImageIdentifier(attribute);
			}
		}
		else if (identifier == "profile")
		{
			if (type == Collada::kElementTechnique)
			{
				Collada::TechniqueType profile = 0;
				if (attribute == "COMMON")
				{
					profile = Collada::kTechniqueCommon;
				}

				static_cast<Collada::Technique *>(element)->SetTechniqueType(profile);
			}
		}
		else if (identifier == "url")
		{
			if (type == Collada::kElementInstance)
			{
				static_cast<Collada::Instance *>(element)->SetResourceIdentifier(attribute);
			}
		}
		else if (identifier == "material")
		{
			if (type == Collada::kElementFaces)
			{
				static_cast<Collada::Faces *>(element)->SetMaterialIdentifier(attribute);
			}
		}
		else if (identifier == "target")
		{
			if (type == Collada::kElementController)
			{
				static_cast<Collada::Controller *>(element)->SetTargetIdentifier(attribute);
			}
			else if (type == Collada::kElementChannel)
			{
				static_cast<Collada::Channel *>(element)->SetTargetIdentifier(attribute);
			}
			else if (type == Collada::kElementInstance)
			{
				static_cast<Collada::Instance *>(element)->SetResourceIdentifier(attribute);
			}
		}
		else if (identifier == "method")
		{
			if (type == Collada::kElementMorph)
			{
				Collada::MethodType method = Collada::kMethodNormalized;
				if (attribute == "RELATIVE")
				{
					method = Collada::kMethodRelative;
				}

				static_cast<Collada::Morph *>(element)->SetMorphMethod(method);
			}
		}
		else if (identifier == "closed")
		{
			if (type == Collada::kElementSpline)
			{
				bool closed = ((attribute == "true") || (attribute == "1"));
				static_cast<Collada::Spline *>(element)->SetClosedFlag(closed);
			}
		}
		else if (identifier == "meter")
		{
			if (type == Collada::kElementUnit)
			{
				static_cast<Collada::Unit *>(element)->SetMeterScale(Text::StringToFloat(attribute));
			}
		}
	}

	return (SkipTag(text));
}

bool ColladaContext::ProcessContent(const char *& text, Collada::Element *element)
{
	element->ProcessContent(text);
	for (;;)
	{
		if (!FindTag(text))
		{
			break;
		}

		if (*text == '/')
		{
			text++;
			return (true);
		}

		Collada::Element *subelement = ProcessElement(text);
		if (subelement)
		{
			element->AppendSubnode(subelement);
		}
	}

	return (false);
}

Collada::Element *ColladaContext::ProcessElement(const char *& text)
{
	String<Collada::kMaxIdentifierLength>	elementType;

	if (ReadIdentifier(text, &elementType))
	{
		Collada::Element *element = CreateElement(elementType);
		if (!ProcessAttributes(text, element))
		{
			ProcessContent(text, element);
			SkipTag(text);
		}

		return (element);
	}

	return (nullptr);
}

Collada::Root *ColladaContext::ProcessFile(const char *text)
{
	ReadWhitespace(text);
	if ((text[0] == '<') && (text[1] == '?'))
	{
		text += 2;
		SkipTag(text);
	}

	Collada::Element *element = nullptr;
	if (FindTag(text))
	{
		element = ProcessElement(text);
	}

	if ((element) && (element->GetElementType() == Collada::kElementRoot))
	{
		Collada::Root *root = static_cast<Collada::Root *>(element);
		root->SetSceneScale(importScale);
		root->Preprocess();
		return (root);
	}

	delete element;
	return (nullptr);
}

C4::Attribute *ColladaContext::NewTextureAttribute(Collada::ParamType type, const char *name, const Collada::Sampler2D *sampler)
{
	C4::Attribute *attribute = nullptr;

	if (name[0] != 0)
	{
		const char *virtualName = name + Text::GetPrefixDirectoryLength(name);

		if (importFlags & kGeometryImportTextures)
		{
			TextureResource *resource = TextureResource::Get(virtualName, kResourceNoDefault);
			if (!resource)
			{
				TextureImporter textureImporter(name);
				if (textureImporter.SetTextureImage(0, name) == kEngineOkay)
				{
					if (sampler)
					{
						TextureHeader *textureHeader = textureImporter.GetTextureHeader();
						textureHeader->wrapMode[0] = sampler->GetWrapMode(0);
						textureHeader->wrapMode[1] = sampler->GetWrapMode(1);
					}

					if (type == Collada::kParamBump)
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

		if (type == Collada::kParamDiffuse)
		{
			attribute = new DiffuseTextureAttribute(virtualName);
		}
		else if (type == Collada::kParamSpecular)
		{
			attribute = new SpecularTextureAttribute(virtualName);
		}
		else if (type == Collada::kParamEmission)
		{
			attribute = new EmissionTextureAttribute(virtualName);
		}
		else if (type == Collada::kParamBump)
		{
			attribute = new NormalTextureAttribute(virtualName);
		}
	}

	return (attribute);
}


ColladaImporter::ColladaImporter() : Singleton<ColladaImporter>(TheColladaImporter)
{
}

ColladaImporter::~ColladaImporter()
{
}

const char *ColladaImporter::GetPluginName(void) const
{
	return ("Collada Import Tool");
}

const ResourceDescriptor *ColladaImporter::GetImportResourceDescriptor(SceneImportType type) const
{
	return (ColladaResource::GetDescriptor());
}

void ColladaImporter::ImportGeometry(Editor *editor, const char *importName, const GeometryImportData *importData)
{
	ColladaResource *collada = ColladaResource::Get(importName, 0, ThePluginMgr->GetImportCatalog());
	if (collada)
	{
		ColladaContext context(editor->GetEditorObject(), importData->importFlags, importData->importScale);
		Collada::Root *root = context.ProcessFile(collada->GetText());

		if (root)
		{
			Collada::Scene *scene = root->FindSceneElement();
			if (scene)
			{
				InfiniteZone *zone = new InfiniteZone;
				scene->BuildScene(zone);

				if (zone->GetFirstSubnode())
				{
					editor->ImportScene(zone);
				}

				delete zone;
			}

			delete root;
		}

		collada->Release();
	}
}

bool ColladaImporter::WriteAnimationResource(const char *resourceName, const void *buffer, unsigned_int32 size)
{
	File			file;
	ResourcePath	path;

	TheResourceMgr->GetGenericCatalog()->GetResourcePath(AnimationResource::GetDescriptor(), resourceName, &path);
	TheResourceMgr->CreateDirectoryPath(path);

	if (file.Open(path, kFileCreate) == kFileOkay)
	{
		file.Write(AnimationResource::resourceSignature, 8);
		file.Write(buffer, size);
		return (true);
	}

	return (false);
}

bool ColladaImporter::GenerateAnimation(const char *resourceName, const Model *model, const AnimationImportData *importData, Collada::Scene *scene, const Collada::Library *animation)
{
	Matrix3D		rotation;

	int32 transformNodeCount = model->GetAnimatedTransformNodeCount();
	const Node *const *transformNodeTable = model->GetAnimatedTransformNodeTable();
	Array<AnimatedTransformNode> transformNodeArray(transformNodeCount);

	for (machine a = 0; a < transformNodeCount; a++)
	{
		const Node *modelNode = transformNodeTable[a];
		const char *nodeName = modelNode->GetNodeName();
		if (nodeName)
		{
			const Collada::Node *importNode = scene->FindNode(nodeName);
			if ((importNode) || (importData->importFlags & kAnimationImportPreserveMissing))
			{
				transformNodeArray.AddElement(AnimatedTransformNode(modelNode, importNode, Text::Hash(nodeName)));
			}
		}
	}

	transformNodeCount = transformNodeArray.GetElementCount();
	int32 transformBucketCount = Min(Power2Ceil(Max(transformNodeCount, 1)), kMaxAnimationBucketCount);
	unsigned_int32 transformBucketMask = transformBucketCount - 1;

	Array<AnimatedTransformHash> *transformBucketTable = new Array<AnimatedTransformHash>[transformBucketCount];
	for (machine a = 0; a < transformNodeCount; a++)
	{
		unsigned_int32 hash = transformNodeArray[a].hashValue;
		transformBucketTable[hash & transformBucketMask].AddElement(AnimatedTransformHash(hash, a));
	}

	float frameDuration = 33.0F;
	float duration = FmaxZero((animation->GetEndTime() - animation->GetBeginTime())) * 1000.0F;
	int32 frameCount = (int32) (duration / frameDuration);

	unsigned_int32 animationHeaderSize = sizeof(AnimationHeader);
	unsigned_int32 transformTrackHeaderSize = sizeof(TransformTrackHeader) + (transformBucketCount - 1) * sizeof(TransformTrackHeader::NodeBucket) + transformNodeCount * sizeof(TransformTrackHeader::NodeData);
	unsigned_int32 transformTrackSize = transformTrackHeaderSize + transformNodeCount * frameCount * sizeof(TransformFrameData);
	unsigned_int32 bufferSize = animationHeaderSize + transformTrackSize;
	Buffer buffer(bufferSize);

	AnimationHeader *animationHeader = buffer.GetPtr<AnimationHeader>();
	animationHeader->frameCount = frameCount;
	animationHeader->frameDuration = frameDuration;
	animationHeader->trackCount = 1;
	animationHeader->trackData[0].trackType = kTrackTransform;
	animationHeader->trackData[0].trackOffset = animationHeaderSize;

	TransformTrackHeader *transformTrackHeader = static_cast<TransformTrackHeader *>(const_cast<void *>(animationHeader->GetTrackHeader(0)));
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

	delete[] transformBucketTable;

	Point3D *positionTable = new Point3D[transformNodeCount];
	rotation.SetRotationAboutZ(importData->importRotation);

	for (machine frame = 0; frame < frameCount; frame++)
	{
		animation->SetAnimationTransforms(animation->GetBeginTime() + (float) (frame * frameDuration) * 0.001F);
		TransformFrameData *transformFrameData = const_cast<TransformFrameData *>(transformTrackHeader->GetTransformFrameData() + frame * transformNodeCount);

		for (machine a = 0; a < transformNodeCount; a++)
		{
			const AnimatedTransformNode *transformNode = &transformNodeArray[a];
			const C4::Node *modelNode = transformNode->modelNode;
			const Collada::Node *importNode = transformNode->importNode;

			Transform4D transform = (importNode) ? importNode->EvaluateTransform() : modelNode->GetNodeTransform();
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

				transformFrameData[a].position = rotation * position;
				transformFrameData[a].transform = rotation * transformFrameData[a].transform;
			}
		}
	}

	delete[] positionTable;

	return (WriteAnimationResource(resourceName, buffer, bufferSize));
}

bool ColladaImporter::ImportAnimation(Window *window, const char *importName, const char *resourceName, Model *model, const AnimationImportData *importData)
{
	bool result = false;

	ColladaResource *collada = ColladaResource::Get(importName, 0, ThePluginMgr->GetImportCatalog());
	if (collada)
	{
		ColladaContext context(nullptr);
		Collada::Root *root = context.ProcessFile(collada->GetText());

		if (root)
		{
			Collada::Scene *scene = root->FindSceneElement();
			const Collada::Library *animation = root->FindAnimationElement();
			if ((scene) && (animation))
			{
				result = GenerateAnimation(resourceName, model, importData, scene, animation);
			}

			delete root;
		}

		collada->Release();
	}

	return (result);
}

// ZYUQURM
