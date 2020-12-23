 

#ifndef C4Half_h
#define C4Half_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Memory.h"


namespace C4
{
	class Half
	{
		private:

			unsigned_int16		value;

			Half(unsigned_int16 v)
			{
				value = v;
			}

			C4API float GetFloat(void) const;
			C4API void SetFloat(float v);

		public:

			Half() = default;

			Half(float v)
			{
				SetFloat(v);
			}

			operator float(void) const
			{
				return (GetFloat());
			}

			Half operator -(const Half& h) const
			{
				return (Half(unsigned_int16(h.value ^ 0x8000)));
			}
	};
}


#endif

// ZYUQURM
