 

#ifndef C4Inertia_h
#define C4Inertia_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Types.h"


namespace C4
{
	class InertiaTensor
	{
		private:

			float	n[6];

		public:

			InertiaTensor() = default;

			C4API InertiaTensor(float n00, float n11, float n22);
			C4API InertiaTensor(float n00, float n01, float n02, float n11, float n12, float n22);

			C4API InertiaTensor& Set(float n00, float n11, float n22);
			C4API InertiaTensor& Set(float n00, float n01, float n02, float n11, float n12, float n22);

			float& operator [](machine k)
			{
				return (n[k]);
			}

			const float& operator [](machine k) const
			{
				return (n[k]);
			}

			InertiaTensor& operator +=(const InertiaTensor& m)
			{
				n[0] += m.n[0];
				n[1] += m.n[1];
				n[2] += m.n[2];
				n[3] += m.n[3];
				n[4] += m.n[4];
				n[5] += m.n[5];

				return (*this);
			}
	};


	C4API Antivector3D operator *(const InertiaTensor& m, const Antivector3D& v);

	C4API InertiaTensor Inverse(const InertiaTensor& m);
	C4API InertiaTensor Rotate(const InertiaTensor& m, const Transform4D& transform);
	C4API InertiaTensor Translate(const InertiaTensor& m, const Vector3D& dp, float mass);
}


#endif

// ZYUQURM
