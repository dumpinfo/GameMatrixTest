 

#include "C4Random.h"
#include "C4Constants.h"


using namespace C4;


namespace
{
	struct RandomSeed
	{
		unsigned_int32	n[4];

		RandomSeed();
	};


	RandomSeed	seed;
}


RandomSeed::RandomSeed()
{
	#if C4WINDOWS

		SYSTEMTIME			time;

		unsigned_int32 a = GetTickCount();
		GetSystemTime(&time);
		unsigned_int32 b = (time.wMinute << 16) | time.wSecond;

	#elif C4MACOS || C4IOS

		unsigned_int32 a = -(unsigned_int32) (mach_absolute_time() >> 4);
		unsigned_int32 b = (unsigned_int32) CFAbsoluteTimeGetCurrent();

	#elif C4LINUX

		timeval				value;

		gettimeofday(&value, nullptr);
		unsigned_int32 a = (unsigned_int32) value.tv_sec;
		unsigned_int32 b = (unsigned_int32) value.tv_usec;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	n[0] = a;
	n[1] = ~a;
	n[2] = b;
	n[3] = ~b;
}

void Math::GetRandomSeed(unsigned_int32 *n)
{
	n[0] = seed.n[0];
	n[1] = seed.n[1];
	n[2] = seed.n[2];
	n[3] = seed.n[3];
}

void Math::SetRandomSeed(const unsigned_int32 *n)
{
	seed.n[0] = n[0];
	seed.n[1] = n[1];
	seed.n[2] = n[2];
	seed.n[3] = n[3];
}

unsigned_int32 Math::Random(unsigned_int32 n)
{
	unsigned_int32 a = seed.n[0] * 0xBC658A9D + 1;
	unsigned_int32 b = seed.n[1] * 0x102F38E5 + 1;
	unsigned_int32 c = seed.n[2] * 0x8712D6BD + 1;
	unsigned_int32 d = seed.n[3] * 0x9DEA7405 + 1;

	seed.n[0] = a;
	seed.n[1] = b;
	seed.n[2] = c;
	seed.n[3] = d;

	return ((((a + b + c + d) >> 16) * n) >> 16);
}

float Math::RandomFloat(float n)
{
	unsigned_int32 a = seed.n[0] * 0xBC658A9D + 1;
	unsigned_int32 b = seed.n[1] * 0x102F38E5 + 1;
	unsigned_int32 c = seed.n[2] * 0x8712D6BD + 1;
	unsigned_int32 d = seed.n[3] * 0x9DEA7405 + 1;

	seed.n[0] = a;
	seed.n[1] = b;
	seed.n[2] = c; 
	seed.n[3] = d;

	return ((float) ((a + b + c + d) >> 16) * (n * K::one_over_65536)); 
}
 
Vector2D Math::RandomUnitVector2D(void)
{
	return (CosSin(RandomFloat(K::tau))); 
}
 
Vector3D Math::RandomUnitVector3D(void) 
{
	float z = RandomFloat(2.0F) - 1.0F;
	float sp = Sqrt(1.0F - z * z);
	return (Vector3D(CosSin(RandomFloat(K::tau)) * sp, z)); 
}

Quaternion Math::RandomUnitQuaternion(void)
{
	float z = RandomFloat(2.0F) - 1.0F;
	float sp = Sqrt(1.0F - z * z);

	Vector2D t = CosSin(RandomFloat(K::tau)) * sp;
	Vector2D u = CosSin(RandomFloat(K::tau_over_2));

	return (Quaternion(u.y * t.x, u.y * t.y, u.y * z, u.x));
}

// ZYUQURM
