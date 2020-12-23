 

#include "C4Manipulator.h"
#include "C4Node.h"


using namespace C4;


namespace C4
{
	template class Creatable<Manipulator, Node *>;
}


Manipulator::Manipulator(Node *node)
{
	targetNode = node;
	manipulatorState = (node->GetNodeFlags() & kNodeInstanceHidden) ? kManipulatorHidden : 0;
}

Manipulator::~Manipulator()
{
}

Manipulator *Manipulator::Create(Node *node)
{
	return (Creatable<Manipulator, Node *>::Create(node));
}

void Manipulator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('STAT', 4);

	unsigned_int32 state = manipulatorState & kManipulatorHidden;
	data << state;

	data << TerminatorChunk;
}

void Manipulator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Manipulator>(data, unpackFlags);
}

bool Manipulator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> manipulatorState;
			return (true);
	}

	return (false);
}

void Manipulator::Preprocess(void)
{
}

void Manipulator::Neutralize(void)
{
}

void Manipulator::Invalidate(void)
{
}

// ZYUQURM
