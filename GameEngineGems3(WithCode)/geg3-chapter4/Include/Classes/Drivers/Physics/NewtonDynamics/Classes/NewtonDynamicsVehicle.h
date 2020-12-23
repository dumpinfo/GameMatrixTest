#pragma once

#include "../NewtonDynamicsPhysicsDriver.h"

namespace Leadwerks
{
	class NewtonDynamicsVehicle;

	class NewtonDynamicsVehicleCollisionFilter : public CustomVehicleControllerTireCollisionFilter
	{
	public:
		NewtonDynamicsVehicleCollisionFilter(const CustomVehicleController* const controller);

		NewtonDynamicsVehicle* vehicle;
		virtual unsigned Prefilter(const NewtonBody* const body, const NewtonCollision* const myCollision);
		dFloat GetTireFrictionCoefficient(const CustomVehicleControllerBodyStateTire& tire, const NewtonBody* const body, const NewtonCollision* const myCollision, dLong contacID) const;
	};
	
	class NewtonDynamicsVehicle : public Vehicle
	{
	public:
		bool transmissionmode;
		CustomVehicleController* vehicle;
		std::vector<CustomVehicleControllerBodyStateTire*> tire;
		CustomVehicleControllerComponentSteering* steering;
		CustomVehicleControllerComponentBrake* brakes;
		CustomVehicleControllerComponentBrake* handbrakes;
		CustomVehicleControllerComponentEngine::dGearBox* gearbox;
		CustomVehicleControllerComponentEngine* engine;
		std::vector<float> forwardGearRatio;
		float reverseGearRatio;
		CustomVehicleControllerComponentEngine::dMultiAxelDifferential* differencial;
		std::vector<int> axle;
		std::vector<CustomVehicleControllerComponentEngine::dSingleAxelDifferential*> newtonaxles;
		float topSpeedKPH, idleTorquePoundPerFoot, idleRPM, peakTorquePoundPerFoot, peakTorqueRPM, peakHorsePower, peakHorsePowerRPM, redLineTorquePoundPerFoot, redLineRPM;
		NewtonDynamicsVehicleCollisionFilter* filter;
		std::map < const CustomVehicleControllerBodyStateTire*, float > tirefriction;

		//Constructor
		NewtonDynamicsVehicle(Entity* entity);
		virtual ~NewtonDynamicsVehicle();

		//Tires
		virtual int AddTire(float x, float y, float z, float mass, float radius, float width, bool steering, float suspensionDamper = 200.0, float suspensionSpring = 2000.0, float suspensionLength = 1.2, float lateralStiffness = 200.0, float longitudinalStiffness = 100000.0, float aligningMOmentTrail = 1.5, float friction=1.0);
		virtual void AddAxle(int lefttire, int righttire);

		//Optional settings
		virtual void SetGearRatio(std::vector<float> forwardGearRatio, float reverseGearRatio);
		virtual void SetTorqueCurve(float topSpeedKPH, float idleTorquePoundPerFoot, float idleRPM, float peakTorquePoundPerFoot, float peakTorqueRPM, float peakHorsePower, float peakHorsePowerRPM, float redLineTorquePoundPerFoot, float redLineRPM);

		//Build
		virtual bool Build();

		//Control
		virtual void SetAcceleration(float engineGasPedal);
		virtual void SetSteering(float steering);
		virtual void SetBrakes(float brakes);
		virtual void SetHandBrakes(float brakes);
		virtual void SetGear(int gear);
		virtual void SetTransmissionMode(bool automatic);
		virtual void SetEngineRunning(const bool running);

		//Information
		virtual Mat4 GetTireMatrix(int tireindex);
		virtual int GetGear();
		virtual float GetRPM();
		virtual float GetSpeed();
		virtual int CountTires();
		virtual float GetTireLateralForce(const int index);
		virtual bool GetEngineRunning();
		virtual float GetTireSlip(const int index);
	};
}