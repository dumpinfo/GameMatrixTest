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


#ifndef MGWeather_h
#define MGWeather_h


#include "MGEffects.h"
#include "MGWeapons.h"


namespace C4
{
	enum : ParticleSystemType
	{
		kParticleSystemRain				= 'rain',
		kParticleSystemBurningFlames	= 'bflm',
		kParticleSystemBurningRain		= 'bran',
		kParticleSystemGroundFog		= 'gfog'
	};


	struct RainParticle : Particle
	{
		float		deathHeight;

		RainParticle *Previous(void) const
		{
			return (static_cast<RainParticle *>(prevParticle));
		}

		RainParticle *GetNextParticle(void) const
		{
			return (static_cast<RainParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);
			data << deathHeight;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);
			data >> deathHeight;
		}
	};


	struct FogParticle : TexcoordParticle
	{
		float		expansionRate;
		Fixed		angularVelocity;

		FogParticle *GetPreviousParticle(void) const
		{
			return (static_cast<FogParticle *>(prevParticle));
		}

		FogParticle *GetNextParticle(void) const
		{
			return (static_cast<FogParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			TexcoordParticle::Pack(data);
			data << expansionRate;
			data << angularVelocity;
		}

		void Unpack(Unpacker& data)
		{
			TexcoordParticle::Unpack(data);
			data >> expansionRate;
			data >> angularVelocity;
		}
	};


	class RainParticleSystem : public LineParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 4096
			};

			float						rainIntensity;

			Range<float>				rainRadius;
			Range<ColorRGBA>			rainColor;

			RainParticleSystem(const RainParticleSystem& rainParticleSystem);

			Node *Replicate(void) const override;

		protected:

			ParticlePool<RainParticle>	particlePool;
			RainParticle				particleArray[kMaxParticleCount];
 
		public:

			RainParticleSystem(ParticleSystemType type = kParticleSystemRain); 
			~RainParticleSystem();
 
			float GetRainIntensity(void) const
			{
				return (rainIntensity); 
			}
 
			float GetMinRainRadius(void) const 
			{
				return (rainRadius.min);
			}
 
			float GetMaxRainRadius(void) const
			{
				return (rainRadius.max);
			}

			const ColorRGBA& GetMinRainColor(void) const
			{
				return (rainColor.min);
			}

			const ColorRGBA& GetMaxRainColor(void) const
			{
				return (rainColor.max);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void AnimateParticles(void) override;
	};


	class BurningFlamesParticleSystem : public FireParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 2048
			};

			ParticlePool<FireParticle>		particlePool;
			FireParticle					particleArray[kMaxParticleCount];

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			BurningFlamesParticleSystem();
			~BurningFlamesParticleSystem();

			void Finalize(void)
			{
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
			}

			void NewFlame(const Point3D& position);

			void AnimateParticles(void) override;
	};


	class BurningRainParticleSystem : public RainParticleSystem
	{
		private:

			enum
			{
				kBurningRainFireballs	= 1 << 0
			};

			unsigned_int32		burningRainFlags;
			unsigned_int32		burningRainPercentage;

			int32				minFireballTime;
			int32				maxFireballTime;
			int32				fireballTime;

			BurningFlamesParticleSystem		*burningFlames;

			BurningRainParticleSystem(const BurningRainParticleSystem& burningRainParticleSystem);

			Node *Replicate(void) const override;

			static void BurningFlamesLinkProc(Node *node, void *cookie);

		public:

			BurningRainParticleSystem();
			~BurningRainParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void Preprocess(void) override;
			void Move(void) override;

			void AnimateParticles(void) override;
	};


	class GroundFogParticleSystem : public TexcoordPointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount		= 1024
			};

			enum
			{
				kGroundFogSoftDepth		= 1 << 0,
				kGroundFogDepthRamp		= 1 << 1,
				kGroundFogInitWarm		= 1 << 2
			};

			unsigned_int32		groundFogFlags;
			ColorRGBA			groundFogColor;

			float				deltaDepthScale;
			Range<float>		depthRampRange;

			float				emissionRate;
			float				emissionAccum;
			float				areaEmissionRate;

			Range<int32>		lifeTimeRange;

			int32				fadeInTime;
			int32				fadeOutTime;
			float				inverseFadeInTime;
			float				inverseFadeOutTime;

			Range<float>		radiusRange;
			Range<float>		expansionRateRange;
			Range<float>		rotationRateRange;

			Range<float>		verticalVelocityRange;
			Range<float>		horizontalVelocityRange;

			ParticlePool<FogParticle>	particlePool;
			FogParticle					particleArray[kMaxParticleCount];

			GroundFogParticleSystem(const GroundFogParticleSystem& groundFogParticleSystem);

			Node *Replicate(void) const override;

			void InitParticle(FogParticle *particle) const;

		public:

			GroundFogParticleSystem();
			~GroundFogParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			void Preprocess(void) override;
			void AnimateParticles(void) override;
			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};
}


#endif

// ZYUQURM
