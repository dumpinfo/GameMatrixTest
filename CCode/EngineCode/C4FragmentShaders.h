 

#ifndef C4FragmentShaders_h
#define C4FragmentShaders_h


#include "C4Programs.h"


namespace C4
{
	enum
	{
		kFragmentParamConstant0						= 0,
		kFragmentParamConstant1						= 1,
		kFragmentParamConstant2						= 2,
		kFragmentParamConstant3						= 3,
		kFragmentParamConstant4						= 4,
		kFragmentParamConstant5						= 5,
		kFragmentParamConstant6						= 6,
		kFragmentParamConstant7						= 7,

		kFragmentParamAmbientColor					= 8,
		kFragmentParamDetailLevel					= 9,		// (detail level parameter, 0.0, 0.0, 0.0)
		kFragmentParamParallaxScale					= 10,		// Parallax scale (s scale, t scale, 0.0, 0.0)
		kFragmentParamImpostorDistance				= 11,		// (impostor distance, 0.0, 0.0, 0.0)

		kFragmentParamCount
	};

	static_assert(kFragmentParamCount == Render::kMaxFragmentParamCount, "Fragment param count mismatch");


	#define FRAGMENT_PARAM_CONSTANT0				"0"
	#define FRAGMENT_PARAM_CONSTANT1				"1"
	#define FRAGMENT_PARAM_CONSTANT2				"2"
	#define FRAGMENT_PARAM_CONSTANT3				"3"
	#define FRAGMENT_PARAM_CONSTANT4				"4"
	#define FRAGMENT_PARAM_CONSTANT5				"5"
	#define FRAGMENT_PARAM_CONSTANT6				"6"
	#define FRAGMENT_PARAM_CONSTANT7				"7"

	#define FRAGMENT_PARAM_AMBIENT_COLOR			"8"
	#define FRAGMENT_PARAM_DETAIL_LEVEL				"9"
	#define FRAGMENT_PARAM_PARALLAX_SCALE			"10"
	#define FRAGMENT_PARAM_IMPOSTOR_DISTANCE		"11"

	#define FRAGMENT_PARAM_COUNT					"12"


	#if C4OPENGL

		#define FPARAM(index)			"fparam[" index "]"

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --

	#endif //]


	class FragmentShader : public Render::FragmentShaderObject, public Shared, public HashTableElement<FragmentShader>
	{
		public:

			typedef ShaderSignature KeyType;

		private:

			static Storage<HashTable<FragmentShader>>		hashTable;

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]

			unsigned_int32		shaderSignature[1];

			FragmentShader(const char *source, unsigned_int32 size, const unsigned_int32 *signature);

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]

			~FragmentShader();

		public:

			static const char		prolog1Text[];
			static const char		prolog2Text[];

			#if C4PS4 //[ PS4

				// -- PS4 code hidden --

			#endif //] 

			static const char		epilogText[];
 
			static const char		copyZero[];
			static const char		copyConstant[]; 

			KeyType GetKey(void) const
			{ 
				return (ShaderSignature(shaderSignature));
			} 
 
			static void Initialize(void);
			static void Terminate(void);

			static unsigned_int32 Hash(const KeyType& key); 

			static FragmentShader *Find(const unsigned_int32 *signature);
			static FragmentShader *New(const char *source, unsigned_int32 size, const unsigned_int32 *signature);

			static void ReleaseCache(void);

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]
	};
}


#endif

// ZYUQURM
