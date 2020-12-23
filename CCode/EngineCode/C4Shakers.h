 

#ifndef C4Shakers_h
#define C4Shakers_h


#include "C4Types.h"


namespace C4
{
	typedef Type	ShakerType;


	enum : ShakerType
	{
		kShakerRumble		= 'RUMB'
	};


	class FrustumCamera;


	class Shaker
	{
		friend class FrustumCamera;

		private:

			ShakerType			shakerType;

			FrustumCamera		*targetCamera;

			virtual Shaker *Replicate(void) const = 0;

		protected:

			C4API Shaker(ShakerType type);
			C4API Shaker(const Shaker& shaker);

		public:

			C4API virtual ~Shaker();

			ShakerType GetShakerType(void) const
			{
				return (shakerType);
			}

			Shaker *Clone(void) const
			{
				return (Replicate());
			}

			FrustumCamera *GetTargetCamera(void) const
			{
				return (targetCamera);
			}

			virtual void CalculateShakeTransform(Transform4D *transform) = 0;
	};


	class RumbleShaker final : public Shaker, public Completable<RumbleShaker>
	{
		private:

			float			maxRumbleIntensity;
			float			rumbleIntensity;

			float			rumbleDuration;
			float			rumbleTime;

			Antivector3D	rumbleAxis;
			float			rumbleFrequency;
			float			rumblePhase;

			RumbleShaker();
			RumbleShaker(const RumbleShaker& rumbleShaker);

			Shaker *Replicate(void) const override;

			void NewRumbleAxis(void);

		public:

			C4API RumbleShaker(float maxIntensity);
			C4API ~RumbleShaker();

			void CalculateShakeTransform(Transform4D *transform) override;

			C4API void Rumble(float intensity, int32 duration);
			C4API void Reset(void);
	};
}


#endif

// ZYUQURM
