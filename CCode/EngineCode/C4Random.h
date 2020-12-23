 

#ifndef C4Random_h
#define C4Random_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Types.h"


namespace C4
{
	namespace Math
	{
		C4API void GetRandomSeed(unsigned_int32 *n);
		C4API void SetRandomSeed(const unsigned_int32 *n);

		C4API unsigned_int32 Random(unsigned_int32 n);
		C4API float RandomFloat(float n);

		inline unsigned_int32 Random(int32 min, int32 max)
		{
			return (Random(max - min) + min);
		}

		inline unsigned_int32 Random(const Range<int32>& range)
		{
			return (Random(range.max - range.min) + range.min);
		}

		inline float RandomFloat(float min, float max)
		{
			return (RandomFloat(max - min) + min);
		}

		inline float RandomFloat(const Range<float>& range)
		{
			return (RandomFloat(range.max - range.min) + range.min);
		}

		C4API Vector2D RandomUnitVector2D(void);
		C4API Vector3D RandomUnitVector3D(void);
		C4API Quaternion RandomUnitQuaternion(void);
	}
}


#endif

// ZYUQURM
