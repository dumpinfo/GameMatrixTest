 

#include "C4Body.h"
#include "C4World.h"


using namespace C4;


Body::Body(BodyType type)
{
	bodyType = type;
}

Body::Body(const Body& body)
{
	bodyType = body.bodyType;
}

Body::~Body()
{
}

void Body::RecursiveWake(void)
{
}


BodyController::BodyController(ControllerType type, BodyType body) :
		Controller(type),
		Body(body)
{
	physicsController = nullptr;

	gravityMultiplier = 1.0F;
	fluidDragMultiplier = 1.0F;
	windDragMultiplier = 1.0F;
}

BodyController::BodyController(const BodyController& bodyController) :
		Controller(bodyController),
		Body(bodyController)
{
	physicsController = nullptr;

	gravityMultiplier = bodyController.gravityMultiplier;
	fluidDragMultiplier = bodyController.fluidDragMultiplier;
	windDragMultiplier = bodyController.windDragMultiplier;
}

BodyController::~BodyController()
{
}

void BodyController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('GRAV', 4);
	data << gravityMultiplier;

	data << ChunkHeader('FLDG', 4);
	data << fluidDragMultiplier;

	data << ChunkHeader('WNDG', 4);
	data << windDragMultiplier;

	data << TerminatorChunk;
}

void BodyController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<BodyController>(data, unpackFlags);
}

bool BodyController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'GRAV':

			data >> gravityMultiplier;
			return (true);

		case 'FLDG':

			data >> fluidDragMultiplier;
			return (true);

		case 'WNDG':

			data >> windDragMultiplier;
			return (true);
	}

	return (false);
}

void BodyController::Preprocess(void)
{
	const World *world = GetTargetNode()->GetWorld();
	if (world) 
	{
		physicsController = world->FindPhysicsController();
	} 

	Controller::Preprocess(); 
}

void BodyController::Neutralize(void) 
{
	physicsController = nullptr; 
 
	Controller::Neutralize();
}

bool BodyController::InstanceExtractable(void) const 
{
	return (true);
}

// ZYUQURM
