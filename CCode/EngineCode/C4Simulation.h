 

#ifndef C4Simulation_h
#define C4Simulation_h


namespace C4
{
	enum
	{
		kSlowMotionMultiplier				= 1,
		kPhysicsTimeStep					= 16 * kSlowMotionMultiplier,
		kRopeStepRatio						= 2,
		kClothStepRatio						= 2,
		kRopeTimeStep						= kPhysicsTimeStep / kRopeStepRatio,
		kClothTimeStep						= kPhysicsTimeStep / kClothStepRatio,
		kMaxPhysicsStepCount				= 4,
		kMaxConstraintIterationCount		= 24,
		kMaxInactiveSubcontactStepCount		= 4,
		kRigidBodySleepStepCount			= 16
	};


	const float kTimeStep									= (float) (kPhysicsTimeStep / kSlowMotionMultiplier) * 0.001F;
	const float kInverseTimeStep							= 1.0F / kTimeStep;
	const float kInversePhysicsTimeStep						= 1.0F / (float) kPhysicsTimeStep;


	const float kContactStabilizeFactor						= kInverseTimeStep * 0.25F;
	const float kJointStabilizeFactor						= kInverseTimeStep * 0.25F;
	const float kStaticJointStabilizeFactor					= kInverseTimeStep * 0.5F;
	const float kConstraintCorrectionThreshold				= 1.0e-4F;


	const float kMinRigidBodyMass							= 0.0009765625F;
	const float kDefaultMaxLinearSpeed						= 200.0F;
	const float kDefaultMaxAngularSpeed						= 25.132741F;		// 4τ
	const float kDefaultFrictionCoefficient					= 0.25F;


	const float kRigidBodySleepBoxSize						= 2.0e-3F;
	const float kCollisionSweepEpsilon						= 1.0e-3F;
	const float kContactEpsilon								= -0.005F;


	const float kMaxShapeShrinkSize							= 0.03125F;
	const float kSupportPointTolerance						= 1.0e-5F;
	const float kShapeSeparationEpsilon						= 1.0e-6F;
	const float kSimplexVertexEpsilon						= 1.0e-5F;
	const float kSimplexDimensionEpsilon					= 1.0e-3F;
	const float kIntersectionDisplacementEpsilon			= 1.0e-4F;
	const float kConservativeAdvancementEpsilon				= 0.01F;
	const float kSemiInfiniteIntersectionDepth				= -64.0F;


	const float kMaxSubcontactSquaredDelta					= 1.0e-4F;
	const float kMaxSubcontactSquaredTangentialSeparation	= 0.1F;
};


#endif

// ZYUQURM
