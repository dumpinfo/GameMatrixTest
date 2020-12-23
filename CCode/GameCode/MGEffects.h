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


#ifndef MGEffects_h
#define MGEffects_h


#include "C4Particles.h"
#include "C4Effects.h"
#include "C4Controller.h"
#include "C4Forces.h"
#include "MGBase.h"


namespace C4
{
	enum : ForceType
	{
		kForceExplosion						= 'expn'
	};


	enum : ControllerType
	{
		kControllerExplosion				= 'expn'
	};


	enum : ParticleSystemType
	{
		kParticleSystemSparks				= 'sprk',
		kParticleSystemDust					= 'dust',
		kParticleSystemSmokeTrail			= 'smtr',
		kParticleSystemBlood				= 'blud',
		kParticleSystemMaterialize			= 'mtlz',
		kParticleSystemSpiralHelix			= 'hlix',
		kParticleSystemTorchSmoke			= 'smok',
		kParticleSystemChimneySmoke			= 'chim',
		kParticleSystemHeatWaves			= 'hwav',
		kParticleSystemBubblingSteam		= 'bstm',
		kParticleSystemFlowingGoo			= 'fgoo',
		kParticleSystemLeakingGoo			= 'leak',
		kParticleSystemGooSplash			= 'gspl',
		kParticleSystemBurstingGoo			= 'brst'
	};


	enum : EffectType
	{
		kEffectShell						= 'shel'
	};


	struct RotateParticle : Particle
	{
		Fixed		angularVelocity;

		RotateParticle *GetPreviousParticle(void) const
		{
			return (static_cast<RotateParticle *>(prevParticle));
		}

		RotateParticle *GetNextParticle(void) const
		{
			return (static_cast<RotateParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);
			data << angularVelocity;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);
			data >> angularVelocity;
		}
	};


	struct OrbitParticle : Particle
	{
		float		angularPosition;
		float		angularVelocity;

		OrbitParticle *GetPreviousParticle(void) const
		{
			return (static_cast<OrbitParticle *>(prevParticle));
		}

		OrbitParticle *GetNextParticle(void) const
		{
			return (static_cast<OrbitParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);
			data << angularPosition;
			data << angularVelocity;
		}

		void Unpack(Unpacker& data) 
		{
			Particle::Unpack(data);
			data >> angularPosition; 
			data >> angularVelocity;
		} 
	};

 
	struct HelixParticle : PolyParticle
	{ 
		float		angularPosition; 
		float		angularVelocity;
		float		revolveRadius;

		HelixParticle *GetPreviousParticle(void) const 
		{
			return (static_cast<HelixParticle *>(prevParticle));
		}

		HelixParticle *GetNextParticle(void) const
		{
			return (static_cast<HelixParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);

			data << angularPosition;
			data << angularVelocity;
			data << revolveRadius;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);

			data >> angularPosition;
			data >> angularVelocity;
			data >> revolveRadius;
		}
	};


	struct SmokeParticle : TexcoordParticle
	{
		Fixed		smokeCount;

		SmokeParticle *GetPreviousParticle(void) const
		{
			return (static_cast<SmokeParticle *>(prevParticle));
		}

		SmokeParticle *GetNextParticle(void) const
		{
			return (static_cast<SmokeParticle *>(nextParticle));
		}

		void Pack(Packer& data) const
		{
			Particle::Pack(data);
			data << smokeCount;
		}

		void Unpack(Unpacker& data)
		{
			Particle::Unpack(data);
			data >> smokeCount;
		}
	};


	class ExplosionForce : public Force
	{
		friend class Game;

		private:

			float				maxExplosionForce;
			float				forceFalloff;

			unsigned_int32		forceExclusionMask;

			ExplosionForce();
			ExplosionForce(const ExplosionForce& explosionForce);

			Force *Replicate(void) const override;

		public:

			ExplosionForce(float maxForce, unsigned_int32 mask);
			~ExplosionForce();

			static bool ValidField(const Field *field);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			bool ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque) override;
			void ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force) override;
	};


	class ExplosionController final : public Controller
	{
		friend class Game;

		private:

			int32	explosionTime;
			bool	initFlag;

			ExplosionController();
			ExplosionController(const ExplosionController& explosionController);

		public:

			ExplosionController(int32 time);
			~ExplosionController();

			Controller *Replicate(void) const override;

			static bool ValidNode(const Node *node);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Move(void) override;
	};


	class SparksParticleSystem : public LineParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 64
			};

			int32				particleCount;
			float				particleSpeed;

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			SparksParticleSystem();

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			SparksParticleSystem(int32 count, float speed = 1.0F);
			~SparksParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class DustParticleSystem : public PointParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxCenterCount		= 5,
				kMaxDustCount		= 5,
				kMaxParticleCount	= kMaxCenterCount * kMaxDustCount
			};

			int32				centerCount;
			Point3D				centerArray[kMaxCenterCount];

			ColorRGBA			dustColor;

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			DustParticleSystem();

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			DustParticleSystem(int32 count, const Point3D *center, const ColorRGBA& color);
			~DustParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class BloodParticleSystem : public TexcoordPointParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 128
			};

			ColorRGB						bloodColor;
			int32							bloodCount;

			ParticlePool<TexcoordParticle>	particlePool;
			TexcoordParticle				particleArray[kMaxParticleCount];

			BloodParticleSystem();

			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			BloodParticleSystem(const ColorRGB& color, int32 count);
			~BloodParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class SmokeTrailParticleSystem : public PointParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 160
			};

			ColorRGBA			smokeColor;

			ParticlePool<>		particlePool;
			Particle			particleArray[kMaxParticleCount];

			SmokeTrailParticleSystem();

		public:

			SmokeTrailParticleSystem(const ColorRGBA& color);
			~SmokeTrailParticleSystem();

			void Finalize(void)
			{
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;

			void CreateSmoke(const Point3D& position, int32 life, float radius);
	};


	class MaterializeParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 1024,
				kParticleLifeTime	= 1000
			};

			ColorRGB		effectColor;
			float			effectRadius;

			ParticlePool<OrbitParticle>		particlePool;
			OrbitParticle					particleArray[kMaxParticleCount];

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			MaterializeParticleSystem(const ColorRGB& color, float radius);
			~MaterializeParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class SpiralHelixParticleSystem : public PolyboardParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 256,
				kParticleLifeTime	= 1000
			};

			ColorRGB		effectColor;
			float			effectRadius;
			float			upwardVelocity;

			int32			emitTime;

			ParticlePool<HelixParticle>		particlePool;
			HelixParticle					particleArray[kMaxParticleCount];

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			SpiralHelixParticleSystem(const ColorRGB& color, float radius, float velocity);
			~SpiralHelixParticleSystem();

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class TorchSmokeParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 32
			};

			int32		smokeTime;

			ParticlePool<RotateParticle>	particlePool;
			RotateParticle					particleArray[kMaxParticleCount];

			TorchSmokeParticleSystem(const TorchSmokeParticleSystem& torchSmokeParticleSystem);

			Node *Replicate(void) const override;

		public:

			TorchSmokeParticleSystem();
			~TorchSmokeParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class ChimneySmokeParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 64
			};

			int32		smokeTime;

			ParticlePool<RotateParticle>	particlePool;
			RotateParticle					particleArray[kMaxParticleCount];

			ChimneySmokeParticleSystem(const ChimneySmokeParticleSystem& chimneySmokeParticleSystem);

			Node *Replicate(void) const override;

		public:

			ChimneySmokeParticleSystem();
			~ChimneySmokeParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class HeatWavesParticleSystem : public PointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 32
			};

			int32		waveTime;

			ParticlePool<RotateParticle>	particlePool;
			RotateParticle					particleArray[kMaxParticleCount];

			HeatWavesParticleSystem(const HeatWavesParticleSystem& heatWavesParticleSystem);

			Node *Replicate(void) const override;

		public:

			HeatWavesParticleSystem();
			~HeatWavesParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class BubblingSteamParticleSystem : public TexcoordPointParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount	= 64,
				kSteamEmitTime		= 100
			};

			ColorRGBA		steamColor;
			int32			steamTime;

			ParticlePool<TexcoordParticle>	particlePool;
			TexcoordParticle				particleArray[kMaxParticleCount];

			BubblingSteamParticleSystem(const BubblingSteamParticleSystem& bubblingSteamParticleSystem);

			Node *Replicate(void) const override;

		public:

			BubblingSteamParticleSystem();
			~BubblingSteamParticleSystem();

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
	};


	class FlowingGooParticleSystem : public BlobParticleSystem
	{
		private:

			enum
			{
				kMaxParticleCount = 64
			};

			float		gravityMultiplier;
			int32		particleLifeTime;

			ParticlePool<BlobParticle>		particlePool;
			BlobParticle					particleArray[kMaxParticleCount];

			FlowingGooParticleSystem(const FlowingGooParticleSystem& flowingGooParticleSystem);

			Node *Replicate(void) const override;

		public:

			FlowingGooParticleSystem();
			~FlowingGooParticleSystem();

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


	class LeakingGooParticleSystem : public BlobParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 64
			};

			const RigidBodyController		*rigidBodyController;

			float							leakRadius;
			float							leakSpeed;

			float							leakTime;
			float							inverseLeakTime;

			float							emissionCount;
			float							speedMultiplier;

			ColorRGB						splatterColor;

			ParticlePool<BlobParticle>		particlePool;
			BlobParticle					particleArray[kMaxParticleCount];

			LeakingGooParticleSystem();

			static void RigidBodyLinkProc(Node *node, void *cookie);

		public:

			LeakingGooParticleSystem(const RigidBodyController *rigidBody, float radius, float speed, float time);
			~LeakingGooParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class GooSplashParticleSystem : public TexcoordPointParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 128
			};

			ColorRGB						splashColor;

			ParticlePool<TexcoordParticle>	particlePool;
			TexcoordParticle				particleArray[kMaxParticleCount];

			GooSplashParticleSystem();

		public:

			GooSplashParticleSystem(const ColorRGB& color);
			~GooSplashParticleSystem();

			void Finalize(void)
			{
				SetParticleSystemFlags(GetParticleSystemFlags() | kParticleSystemSelfDestruct);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;

			void AddSplash(const Point3D& center, int32 count, float intensity = 1.0F);
	};


	class BurstingGooParticleSystem : public BlobParticleSystem
	{
		friend class Game;

		private:

			enum
			{
				kMaxParticleCount = 32
			};

			Vector3D						burstVelocity;
			ColorRGB						splatterColor;

			GooSplashParticleSystem			*gooSplash;

			ParticlePool<BlobParticle>		particlePool;
			BlobParticle					particleArray[kMaxParticleCount];

			BurstingGooParticleSystem();

			static void GooSplashLinkProc(Node *node, void *cookie);

		public:

			BurstingGooParticleSystem(const Vector3D& velocity);
			~BurstingGooParticleSystem();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void AnimateParticles(void) override;
	};


	class ShellEffect : public Effect
	{
		friend class Game;

		private:

			Geometry			*shellGeometry;

			Vector4D			scaleVector;

			List<Attribute>		attributeList;
			DiffuseAttribute	diffuseAttribute;

			ShellEffect();
			ShellEffect(const ShellEffect& shellEffect);

			Node *Replicate(void) const override;

			void HandlePostprocessUpdate(void) override;

		public:

			ShellEffect(Geometry *geometry, float size, const ColorRGBA& color);
			~ShellEffect();

			const Geometry *GetShellGeometry(void) const
			{
				return (shellGeometry);
			}

			float GetShellSize(void) const
			{
				return (scaleVector.x);
			}

			void SetShellSize(float size)
			{
				scaleVector.x = size;
			}

			const ColorRGBA& GetShellColor(void) const
			{
				return (diffuseAttribute.GetDiffuseColor());
			}

			void SetShellColor(const ColorRGBA& color)
			{
				diffuseAttribute.SetDiffuseColor(color);
			}

			void Preprocess(void) override;

			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};
}


#endif

// ZYUQURM
