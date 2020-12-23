 

#include "C4Objects.h"
#include "C4CameraObjects.h"
#include "C4LightObjects.h"
#include "C4SpaceObjects.h"


using namespace C4;


namespace C4
{
	template class Creatable<Object>;
}


Object::Object(ObjectType type)
{
	objectType = type;
	objectIndex = -1;
	modifiedFlag = false;
}

Object::~Object()
{
}

Object *Object::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kObjectCamera:

			return (CameraObject::Create(++data, unpackFlags));

		case kObjectLight:

			return (LightObject::Create(++data, unpackFlags));

		case kObjectSpace:

			return (SpaceObject::Create(++data, unpackFlags));
	}

	return (Creatable<Object>::Create(data, unpackFlags));
}

void Object::PackType(Packer& data) const
{
	data << objectType;
}

Object *Object::Replicate(void) const
{
	return (nullptr);
}

int32 Object::GetObjectSize(float *size) const
{
	return (0);
}

void Object::SetObjectSize(const float *size)
{
}

// ZYUQURM
