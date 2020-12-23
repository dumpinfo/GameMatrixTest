#include "../Leadwerks.h"

namespace Leadwerks
{
	class Heightfield
	{
		float smoothedNoise(float x, float y);
		float euclideanSquared(float x1, float y1, float x2, float y2);
		float euclidean(float x1, float y1, float x2, float y2);
		float manhattan(float x1, float y1, float x2, float y2);
		float chebychev(float x1, float y1, float x2, float y2);
		float quadratic(float x1, float y1, float x2, float y2);
		float random(int x, int y, int z);
		int ranInt();
	public:
		void* data;
		int resolution;
		int datatype;
		int permutations[1024];
		bool owndata;
		Vec3 scale;

		Heightfield();
		Heightfield(const int datatype, const int resolution);
		Heightfield(unsigned char* data, const int resolution);
		Heightfield(unsigned short* data, const int resolution);
		Heightfield(float* data, const int resolution);
		virtual ~Heightfield();

		virtual void Normalize();
		virtual void Crater(const float strength=0.5);
		virtual float GetHeight4(const float x, const float y);
		virtual void PerturbationFilter(const float strength);
		virtual void Floor(const float height=0.25, const float strength=0.5);
		virtual void Ceil(const float height=0.75, const float strength=0.5);
		virtual float GetSlope(const int x, const int y);
		virtual Vec3 GetNormal(const int x,const int y);
		virtual void SimplexNoise(const float lacunarity=2, const int octaves=6);
		virtual void Step(const float stepsize=0.1, const float strength=0.5);
		virtual void SeaBed();
		virtual float GetHeight(const int x, const int y);
		virtual void SetHeight(const int x, const int y, const float height);
		virtual void DiamondSquare(const float smoothness = 0.65, const float amplitude=1);
		virtual void ThermalErosion(const float maxslope, const int iterations);
		virtual void HydraulicErosion(const float radius, const float rain_amount=0.01, const float solubility=0.01, const float evaporation=0.9, const int iterations=1, const int randrops=1000);
		virtual void Smooth();
		virtual void CellNoise();
		virtual void Mix(Heightfield& hf, const float p, const bool slopesonly=false);
	};
}
