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


#ifndef MGMagicBroom_h
#define MGMagicBroom_h


#include "C4Particles.h"
#include "MGMonster.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerMagicBroom		= 'brom'
	};


	enum : ModelType
	{
		kModelMagicBroom			= 'brom'
	};


	enum : ParticleSystemType
	{
		kParticleSystemBroomSmoke	= 'bsmk',
		kParticleSystemMagicBroom	= 'brom'
	};


	enum : MethodType
	{
		kMethodGenerateMagicBroom	= 'brom',
		kMethodGetMagicBroomCount	= 'brmc'
	};


	class BroomSmokeParticleSystem : public TexcoordPointParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 160
			};

			ColorRGBA						smokeColor;
			float							smokeDistance;

			Point3D							broomPosition;
			Vector3D						broomVelocity;

			ParticlePool<TexcoordParticle>	particlePool;
			TexcoordParticle				particleArray[kMaxParticleCount];

			BroomSmokeParticleSystem();

		public:

			BroomSmokeParticleSystem(const ColorRGBA& color);
			~BroomSmokeParticleSystem();

			void SetBroomState(const Point3D& position, const Vector3D& velocity)
			{
				broomPosition = position;
				broomVelocity = velocity;
			}

			void Finalize(void)
			{
				smokeDistance = -1.0F;
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class MagicBroomParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 64
			};

			int32				emissionTime;

			Point3D				broomPosition;
			Vector3D			broomVelocity;

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];
 
		public:

			MagicBroomParticleSystem(); 
			~MagicBroomParticleSystem();
 
			void SetBroomState(const Point3D& position, const Vector3D& velocity)
			{
				broomPosition = position; 
				broomVelocity = velocity;
			} 
 
			void Finalize(void)
			{
				emissionTime = -1;
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct); 
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};
}


#endif

// ZYUQURM
