#include "Leadwerks.h"

namespace Leadwerks
{
	class Entity;

	class Vehicle : public Object//lua
	{
	public:
		Entity* entity;
		std::string GetClassName();
		std::list<Vehicle*>::iterator it;
		std::vector<Vec3> prevtireposition;
		std::vector<Vec3> nexttireposition;
		std::vector<Quat> prevtirerotation;
		std::vector<Quat> nexttirerotation;
		std::vector<Mat4> tirematrix;

		Vehicle();
		virtual ~Vehicle();

		//Tires
		virtual int AddTire(float x, float y, float z, float mass, float radius, float width, bool steering, float suspensionDamper = 200.0, float suspensionSpring = 2000.0, float suspensionLength = 1.2, float lateralStiffness = 20.0, float longitudinalStiffness = 100000.0, float aligningMOmentTrail = 1.5, float friction = 1.0) = 0;//lua
		virtual void AddAxle(int lefttire, int righttire) = 0;//lua

		//Optional settings
		virtual void SetGearRatio(std::vector<float> forwardGearRatio, float reverseGearRatio) = 0;//lua
		virtual void SetTorqueCurve(float topSpeedKPH, float idleTorquePoundPerFoot, float idleRPM, float peakTorquePoundPerFoot, float peakTorqueRPM, float peakHorsePower, float peakHorsePowerRPM, float redLineTorquePoundPerFoot, float redLineRPM) = 0;//lua

		//Build
		virtual bool Build() = 0;//lua

		//Control
		virtual void SetAcceleration(float engineGasPedal) = 0;//lua
		virtual void SetSteering(float steering) = 0;//lua
		virtual void SetBrakes(float brakes) = 0;//lua
		virtual void SetHandBrakes(float brakes) = 0;//lua
		virtual void SetGear(int gear) = 0;//lua
		virtual void SetTransmissionMode(bool automatic) = 0;//lua
		virtual void SetEngineRunning(const bool running) = 0;//lua

		//Information
		virtual Mat4 GetTireMatrix(int tireindex)=0;//lua
		virtual int GetGear() = 0;//lua
		virtual float GetRPM() = 0;//lua
		virtual float GetSpeed() = 0;//lua
		virtual int CountTires()=0;//lua
		virtual float GetTireLateralForce(const int index)=0;//lua
		virtual bool GetEngineRunning()=0;//lua
		virtual float GetTireSlip(const int index) = 0;//lua

		static Vehicle* Create(Entity* entity);//lua
	};
}