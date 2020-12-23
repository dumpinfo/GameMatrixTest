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


#include "MGCameras.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


const float C4::kCameraPositionHeight = 1.6F;


ModelCamera::ModelCamera() : FrustumCamera(TheGame->GetCameraFocalLength(), 1.0F)
{
	targetModel = nullptr;
	SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);
}

ModelCamera::~ModelCamera()
{
}


FirstPersonCamera::FirstPersonCamera()
{
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::Move(void)
{
	Model *model = GetTargetModel();
	if (model)
	{
		FighterController *controller = static_cast<FighterController *>(model->GetController());
		Vector2D t = CosSin(controller->GetLookAzimuth());
		Vector2D u = CosSin(controller->GetLookAltitude());

		Vector3D view(t.x * u.x, t.y * u.x, u.y);
		Vector3D right(t.y, -t.x, 0.0F);
		Vector3D down = view % right;

		const Point3D& position = model->GetWorldPosition();
		Point3D p(position.x, position.y, position.z + kCameraPositionHeight);

		SetNodeTransform(right, down, view, p);

		const Quaternion& q = TheWorldMgr->GetTrackingOrientation();
		SetNodeMatrix3D(GetNodeTransform().GetMatrix3D() * q.GetRotationMatrix());
	}
}


ChaseCamera::ChaseCamera()
{
}

ChaseCamera::~ChaseCamera()
{
}

void ChaseCamera::Move(void)
{
	Model *model = GetTargetModel();
	if (model)
	{
		CollisionData	data;

		FighterController *controller = static_cast<FighterController *>(model->GetController());
		Vector2D t = CosSin(controller->GetLookAzimuth());
		Vector2D u = CosSin(controller->GetLookAltitude());

		Vector3D view(t.x * u.x, t.y * u.x, u.y);
		Vector3D right(t.y, -t.x, 0.0F);
		Vector3D down = view % right;

		const Point3D& position = model->GetWorldPosition();
		Point3D p1(position.x, position.y, position.z + 1.5F);
		Point3D p2 = p1 - view * 4.0F;

		if (GetWorld()->DetectCollision(p1, p2, 0.3F, kCollisionCamera, &data))
		{
			float s = data.param;
			p2 = p1 * (1.0F - s) + p2 * s;
		}

		SetNodeTransform(right, down, view, p2);
	}
}

// ZYUQURM
