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


#include "C4EditorBase.h"
#include "C4WorldEditor.h"
#include "C4World.h"


using namespace C4;


EditorEvent::EditorEvent(EditorEventType type)
{
	eventType = type;
}

EditorEvent::~EditorEvent()
{
}


NodeEditorEvent::NodeEditorEvent(EditorEventType type, Node *node) : EditorEvent(type)
{
	eventNode = node;
}

NodeEditorEvent::~NodeEditorEvent()
{
}


GizmoEditorEvent::GizmoEditorEvent(EditorEventType type, Node *gizmoTarget) : EditorEvent(type)
{
	eventGizmoTarget = gizmoTarget;
}

GizmoEditorEvent::~GizmoEditorEvent()
{
}


MaterialEditorEvent::MaterialEditorEvent(EditorEventType type, MaterialObject *materialObject) : EditorEvent(type)
{
	eventMaterialObject = materialObject;
}

MaterialEditorEvent::~MaterialEditorEvent()
{
}


PlacementAdjuster::PlacementAdjuster()
{
	adjusterFlags = 0;
	groundSinkRadius = 0.0F;
	offsetRange.Set(0.0F, 0.0F);
}

PlacementAdjuster::PlacementAdjuster(const PlacementAdjuster& placementAdjuster)
{
	adjusterFlags = placementAdjuster.adjusterFlags;
	groundSinkRadius = placementAdjuster.groundSinkRadius;
	offsetRange = placementAdjuster.offsetRange;
}

PlacementAdjuster::~PlacementAdjuster()
{
}

void PlacementAdjuster::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << adjusterFlags;

	data << ChunkHeader('SRAD', 4);
	data << groundSinkRadius;

	data << ChunkHeader('OFST', 8);
	data << offsetRange.min;
	data << offsetRange.max;

	data << TerminatorChunk;
}

void PlacementAdjuster::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<PlacementAdjuster>(data, unpackFlags);
}

bool PlacementAdjuster::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> adjusterFlags;
			return (true);

		case 'SRAD':

			data >> groundSinkRadius;
			return (true);

		case 'OFST': 

			data >> offsetRange.min;
			data >> offsetRange.max; 
			return (true);
	} 

	return (false);
} 

int32 PlacementAdjuster::GetSettingCount(void) const 
{ 
	return (6);
}

Setting *PlacementAdjuster::GetSetting(int32 index) const 
{
	const StringTable *table = TheWorldEditor->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('AJST', 'RROT'));
		return (new BooleanSetting('RROT', ((adjusterFlags & kAdjusterRandomRotation) != 0), title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('AJST', 'TANG'));
		return (new BooleanSetting('TANG', ((adjusterFlags & kAdjusterTangentPlane) != 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('AJST', 'SINK'));
		return (new BooleanSetting('SINK', ((adjusterFlags & kAdjusterGroundSink) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('AJST', 'SRAD'));
		return (new TextSetting('SRAD', groundSinkRadius, title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('AJST', 'OMIN'));
		return (new TextSetting('OMIN', offsetRange.min, title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('AJST', 'OMAX'));
		return (new TextSetting('OMAX', offsetRange.max, title));
	}

	return (nullptr);
}

void PlacementAdjuster::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'RROT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			adjusterFlags |= kAdjusterRandomRotation;
		}
		else
		{
			adjusterFlags &= ~kAdjusterRandomRotation;
		}
	}
	else if (identifier == 'TANG')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			adjusterFlags |= kAdjusterTangentPlane;
		}
		else
		{
			adjusterFlags &= ~kAdjusterTangentPlane;
		}
	}
	else if (identifier == 'SINK')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			adjusterFlags |= kAdjusterGroundSink;
		}
		else
		{
			adjusterFlags &= ~kAdjusterGroundSink;
		}
	}
	else if (identifier == 'SRAD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		groundSinkRadius = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'OMIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		offsetRange.min = Text::StringToFloat(text);
	}
	else if (identifier == 'OMAX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		offsetRange.max = Text::StringToFloat(text);
	}
}

void PlacementAdjuster::ApplyRandomRotation(Transform4D *transform) const
{
	if (adjusterFlags & kAdjusterRandomRotation)
	{
		transform->SetMatrix3D(Matrix3D().SetRotationAboutZ(Math::RandomFloat(K::tau)));
	}
}

void PlacementAdjuster::ApplyTangentPlane(const World *world, Transform4D *transform, const Vector3D *normal) const
{
	if (adjusterFlags & kAdjusterTangentPlane)
	{
		CollisionData	data;

		if (!normal)
		{
			const Point3D& position = transform->GetTranslation();
			Point3D p1(position.x, position.y, position.z + 0.25F);
			Point3D p2(position.x, position.y, position.z - 1.0F);

			if (!world->DetectCollision(p1, p2, 0.0F, kCollisionSightPath, &data))
			{
				return;
			}

			normal = &data.normal;
		}

		Matrix3D m = transform->GetMatrix3D();
		float t = Acos(*normal * m[2]);
		if (t * t > K::min_float)
		{
			Antivector3D axis = m[2] % *normal;
			float f = SquaredMag(axis);
			if (f > K::min_float)
			{
				axis *= InverseSqrt(f);
				transform->SetMatrix3D(Matrix3D().SetRotationAboutAxis(t, axis) * m);
			}
		}
	}
}

void PlacementAdjuster::ApplyGroundSink(const World *world, Transform4D *transform) const
{
	if (adjusterFlags & kAdjusterGroundSink)
	{
		CollisionData	data;

		const ConstVector2D *trig = Math::GetTrigTable();

		float radius = groundSinkRadius;
		float maxDepth = radius * 2.0F;
		float maxParam = 0.0F;

		Point3D position = transform->GetTranslation();
		for (machine a = 0; a < 8; a++)
		{
			Vector2D cs = trig[a * 32] * radius;
			Point3D p1 = position + (*transform)[0] * cs.x + (*transform)[1] * cs.y;
			Point3D p2 = p1 - (*transform)[2] * maxDepth;

			if (world->DetectCollision(p1, p2, 0.0F, kCollisionSightPath, &data))
			{
				maxParam = Fmax(maxParam, data.param);
			}
		}

		position -= (*transform)[2] * (maxDepth * maxParam);
		transform->SetTranslation(position);
	}
}

void PlacementAdjuster::ApplyOffsetRange(Transform4D *transform) const
{
	float offset = offsetRange.min;
	float delta = offsetRange.max - offset;
	transform->SetTranslation(transform->GetTranslation() + (*transform)[2] * (delta * Math::RandomFloat(1.0F) + offset));
}

void PlacementAdjuster::AdjustPlacement(const World *world, Transform4D *transform, const Vector3D& normal) const
{
	ApplyRandomRotation(transform);
	ApplyTangentPlane(world, transform, &normal);
	ApplyGroundSink(world, transform);
	ApplyOffsetRange(transform);
}

// ZYUQURM
