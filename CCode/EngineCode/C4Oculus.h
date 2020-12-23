 

#ifndef C4Oculus_h
#define C4Oculus_h


#define C4OCULUS	0


#if C4OCULUS

	namespace OVR
	{
		class DeviceManager;
		class HMDDevice;
		class SensorDevice;
		class SensorFusion;
	}


	namespace C4
	{
		class Quaternion;


		struct Oculus
		{
			private:

				static OVR::DeviceManager		*deviceManager;
				static OVR::HMDDevice			*hmdDevice;

				static OVR::SensorDevice		*sensorDevice;
				static OVR::SensorFusion		*sensorFusion;

				static int						fullFrameWidth;
				static int						fullFrameHeight;
				static int						displayWidth;
				static int						displayHeight;
				static int						scissorInset;

				static float					fullFrameParam[2][4];
				static float					displayParam[2][4];
				static float					distortionParam[4];
				static float					chromaticParam[4];

				static float					lensCenter;
				static float					lensRadius;
				static float					lensImageScale;
				static float					lensFocalLength;
				static float					interpupillaryDistance;

			public:

				static void Initialize(void);
				static void Terminate(void);

				static int GetFullFrameWidth(void)
				{
					return (fullFrameWidth);
				}

				static int GetFullFrameHeight(void)
				{
					return (fullFrameHeight);
				}

				static int GetDisplayWidth(void)
				{
					return (displayWidth);
				}

				static int GetDisplayHeight(void)
				{
					return (displayHeight);
				}

				static int GetScissorInset(void)
				{
					return (scissorInset);
				}

				static const float (& GetFullFrameParam(int index))[4]
				{
					return (fullFrameParam[index]);
				}

				static const float (& GetDisplayParam(int index))[4]
				{
					return (displayParam[index]);
				}

				static const float (& GetDistortionParam(void))[4]
				{
					return (distortionParam);
				}

				static const float (& GetChromaticParam(void))[4]
				{
					return (chromaticParam);
				}

				static float GetLensCenter(void) 
				{
					return (lensCenter);
				} 

				static float GetLensRadius(void) 
				{
					return (lensRadius);
				} 

				static float GetLensImageScale(void) 
				{ 
					return (lensImageScale);
				}

				static float GetLensFocalLength(void) 
				{
					return (lensFocalLength);
				}

				static float GetInterpupillaryDistance(void)
				{
					return (interpupillaryDistance);
				}

				static void ReadOrientation(Quaternion *orientation);
		};
	}

#endif


#endif

// ZYUQURM
