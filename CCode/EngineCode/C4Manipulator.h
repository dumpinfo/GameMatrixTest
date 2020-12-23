 

#ifndef C4Manipulator_h
#define C4Manipulator_h


#include "C4Creation.h"


namespace C4
{
	enum
	{
		kManipulatorSelected	= 1 << 0,
		kManipulatorHidden		= 1 << 1,
		kManipulatorBaseState	= 1 << 2
	};


	class Node;


	class Manipulator : public Packable, public Creatable<Manipulator, Node *>
	{
		private:

			Node				*targetNode;
			unsigned_int32		manipulatorState;

		public:

			C4API Manipulator(Node *node);
			C4API virtual ~Manipulator();

			C4API static Manipulator *Create(Node *node);

			Node *GetTargetNode(void) const
			{
				return (targetNode);
			}

			unsigned_int32 GetManipulatorState(void) const
			{
				return (manipulatorState);
			}

			void SetManipulatorState(unsigned_int32 state)
			{
				manipulatorState = state;
			}

			bool Selected(void) const
			{
				return ((manipulatorState & kManipulatorSelected) != 0);
			}

			bool Hidden(void) const
			{
				return ((manipulatorState & kManipulatorHidden) != 0);
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API virtual void Preprocess(void);
			C4API virtual void Neutralize(void);
			C4API virtual void Invalidate(void);
	};
}


#endif

// ZYUQURM
