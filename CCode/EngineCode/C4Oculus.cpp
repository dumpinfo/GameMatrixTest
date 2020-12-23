 

#include "C4Oculus.h"

#if C4OCULUS

	#include "OVR.h"


	using namespace C4;


	OVR::DeviceManager *Oculus::deviceManager;
	OVR::HMDDevice *Oculus::hmdDevice;

	OVR::SensorDevice *Oculus::sensorDevice;
	OVR::SensorFusion *Oculus::sensorFusion;

	int Oculus::fullFrameWidth;
	int Oculus::fullFrameHeight;
	int Oculus::displayWidth;
	int Oculus::displayHeight;
	int Oculus::scissorInset;

	float Oculus::fullFrameParam[2][4];
	float Oculus::displayParam[2][4];
	float Oculus::distortionParam[4];
	float Oculus::chromaticParam[4];

	float Oculus::lensCenter;
	float Oculus::lensRadius;
	float Oculus::lensImageScale;
	float Oculus::lensFocalLength;
	float Oculus::interpupillaryDistance;


	void Oculus::Initialize(void)
	{
		OVR::HMDInfo	hmdInfo;

		OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_None));

		deviceManager = OVR::DeviceManager::Create();
		hmdDevice = deviceManager->EnumerateDevices<OVR::HMDDevice>().CreateDevice();
		sensorFusion = nullptr;

		if (hmdDevice)
		{
			sensorDevice = hmdDevice->GetSensor();
			if (sensorDevice)
			{
				sensorFusion = new OVR::SensorFusion(sensorDevice);
				sensorFusion->SetPrediction(0.02F, true);
			}

			hmdDevice->GetDeviceInfo(&hmdInfo);
		}
		else
		{
			hmdInfo.HResolution = 1280;
			hmdInfo.VResolution = 800;
			hmdInfo.HScreenSize = 0.14976F;
			hmdInfo.VScreenSize = 0.0936F;
			hmdInfo.InterpupillaryDistance = 0.064F;
			hmdInfo.LensSeparationDistance = 0.0635F;
			hmdInfo.EyeToScreenDistance = 0.041F;
			hmdInfo.DistortionK[0] = 1.0F;
			hmdInfo.DistortionK[1] = 0.22F;
			hmdInfo.DistortionK[2] = 0.24F;
			hmdInfo.DistortionK[3] = 0.0F;
			hmdInfo.ChromaAbCorrection[0] = 1.0F;
			hmdInfo.ChromaAbCorrection[1] = 0.0F;
			hmdInfo.ChromaAbCorrection[2] = 1.0F;
			hmdInfo.ChromaAbCorrection[3] = 0.0F;
		}

		fullFrameWidth = hmdInfo.HResolution;
		fullFrameHeight = hmdInfo.VResolution;
		displayWidth = (fullFrameWidth * 5) >> 3;
		displayHeight = (fullFrameHeight * 5) >> 2;

		lensCenter = 1.0F - hmdInfo.LensSeparationDistance * 2.0F / hmdInfo.HScreenSize;
		lensRadius = 1.0F + lensCenter;

		float fw = (float) fullFrameWidth;
		float fh = (float) fullFrameHeight;

		fullFrameParam[0][0] = fw * 0.25F * (1.0F + lensCenter);
		fullFrameParam[0][1] = fh * 0.5F;
		fullFrameParam[0][2] = 1.0F / (fw * 0.25F * lensRadius);
		fullFrameParam[0][3] = 0.0F;

		fullFrameParam[1][0] = fw * 0.25F * (3.0F - lensCenter);
		fullFrameParam[1][1] = fullFrameParam[0][1];
		fullFrameParam[1][2] = fullFrameParam[0][2];
		fullFrameParam[1][3] = 0.0F;

		float dw = (float) displayWidth * 0.5F;
		float dh = (float) displayHeight;

		displayParam[0][0] = dw * (1.0F + lensCenter);
		displayParam[0][1] = dh * 0.5F;
		displayParam[0][2] = 0.0F; 
		displayParam[0][3] = 0.0F;

		displayParam[1][0] = dw * (1.0F - lensCenter); 
		displayParam[1][1] = displayParam[0][1];
		displayParam[1][2] = 0.0F; 
		displayParam[1][3] = 0.0F;

		const float *k = hmdInfo.DistortionK; 
		float maxDistortion = k[0] + k[1] + k[2] + k[3];
		lensImageScale = 1.0F / maxDistortion; 
 
		float x = maxDistortion * (hmdInfo.HScreenSize * 0.25F);
		float d = hmdInfo.EyeToScreenDistance;
		lensFocalLength = d / x;
 
		float scale = dw * lensImageScale * lensRadius;
		for (int a = 0; a < 4; a++)
		{
			distortionParam[a] = k[a] * scale;
			chromaticParam[a] = hmdInfo.ChromaAbCorrection[a];
		}

		float c = dh * maxDistortion / (fh * lensRadius);
		float z = (maxDistortion - c) / (k[0] + 3.0F * k[1] + 5.0F * k[2] + 7.0F * k[3]) - 1.0F;

		for (int a = 0; a < 2; a++)
		{
			float z2 = z * z;
			z -= (c + (k[0] + (k[1] + (k[2] + k[3] * z2) * z2) * z2) * z) / (k[0] + (3.0F * k[1] + (5.0F * k[2] + 7.0F * k[3] * z2) * z2) * z2);
		}

		scissorInset = (int) (z * lensRadius * (fw * 0.25F) + fh * 0.5F);

		interpupillaryDistance = hmdInfo.InterpupillaryDistance;
	}

	void Oculus::Terminate(void)
	{
		if (hmdDevice)
		{
			delete sensorFusion;

			if (sensorDevice)
			{
				sensorDevice->Release();
			}

			hmdDevice->Release();
		}

		deviceManager->Release();

		OVR::System::Destroy();
	}

	void Oculus::ReadOrientation(Quaternion *orientation)
	{
		OVR::Quatf *q = reinterpret_cast<OVR::Quatf *>(orientation);

		if (sensorFusion)
		{
			*q = sensorFusion->GetPredictedOrientation();
			q->y = -q->y;
			q->z = -q->z;
		}
		else
		{
			q->x = 0.0F;
			q->y = 0.0F;
			q->z = 0.0F;
			q->w = 1.0F;
		}
	}

#endif

// ZYUQURM
