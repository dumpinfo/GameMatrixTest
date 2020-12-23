 

#include "C4Shakers.h"
#include "C4Random.h"
#include "C4Time.h"


using namespace C4;


Shaker::Shaker(ShakerType type)
{
	shakerType = type;
	targetCamera = nullptr;
}

Shaker::Shaker(const Shaker& shaker)
{
	shakerType = shaker.shakerType;
	targetCamera = nullptr;
}

Shaker::~Shaker()
{
}


RumbleShaker::RumbleShaker() : Shaker(kShakerRumble)
{
}

RumbleShaker::RumbleShaker(float maxIntensity) : Shaker(kShakerRumble)
{
	maxRumbleIntensity = maxIntensity;

	rumbleIntensity = 0.0F;
	rumbleDuration = 1.0F;
	rumbleTime = 1.0F;
}

RumbleShaker::RumbleShaker(const RumbleShaker& rumbleShaker) : Shaker(rumbleShaker)
{
	maxRumbleIntensity = rumbleShaker.maxRumbleIntensity;

	rumbleIntensity = 0.0F;
	rumbleDuration = 1.0F;
	rumbleTime = 1.0F;
}

RumbleShaker::~RumbleShaker()
{
}

Shaker *RumbleShaker::Replicate(void) const
{
	return (new RumbleShaker(*this));
}

void RumbleShaker::CalculateShakeTransform(Transform4D *transform)
{
	float dt = TheTimeMgr->GetFloatDeltaTime();

	float time = rumbleTime + dt;
	if (time < rumbleDuration)
	{
		rumbleTime = time;

		float phase = rumblePhase + rumbleFrequency * dt;
		if (phase < K::tau)
		{
			rumblePhase = phase;
		}
		else
		{
			NewRumbleAxis();
			phase = Fmin(rumbleFrequency * dt, K::tau_over_2);
		}

		float angle = Sin(phase) * rumbleIntensity * (1.0F - time / rumbleDuration);
		transform->SetRotationAboutAxis(angle, rumbleAxis);
	}
	else
	{
		rumbleIntensity = 0.0F;
		rumbleDuration = 1.0F;
		rumbleTime = 1.0F;

		transform->SetIdentity();
		CallCompletionProc();
	}
}

void RumbleShaker::NewRumbleAxis(void)
{
	rumbleAxis = Math::RandomUnitVector3D();
	rumbleFrequency = (Math::RandomFloat(0.01F) + 0.01F) * K::tau;
}

void RumbleShaker::Rumble(float intensity, int32 duration)
{
	intensity = Fmin(intensity, maxRumbleIntensity);
	if (intensity > rumbleIntensity * (1.0F - rumbleTime / rumbleDuration))
	{ 
		rumbleIntensity = intensity;
		rumbleDuration = (float) duration;
		rumbleTime = 0.0F; 
		rumblePhase = 0.0F;
 
		NewRumbleAxis();
	}
} 

void RumbleShaker::Reset(void) 
{ 
	rumbleIntensity = 0.0F;
	rumbleDuration = 1.0F;
	rumbleTime = 1.0F;
} 

// ZYUQURM
