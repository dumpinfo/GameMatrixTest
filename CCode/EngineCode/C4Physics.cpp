 

#include "C4Physics.h"
#include "C4Simulation.h"
#include "C4Forces.h"
#include "C4Fields.h"
#include "C4Joints.h"
#include "C4Water.h"
#include "C4World.h"
#include "C4Terrain.h"
#include "C4Primitives.h"
#include "C4Configuration.h"


using namespace C4;


namespace C4
{
	class ShapeCollisionJob : public BatchJob, public Memory<ShapeCollisionJob>
	{
		public:

			RigidBodyController		*alphaBody;
			RigidBodyController		*betaBody;
			const Shape				*alphaShape;
			const Shape				*betaShape;
			unsigned_int32			alphaIndex;
			unsigned_int32			betaIndex;

			RigidBodyContact		*rigidBodyContact;
			IntersectionData		intersectionData;

			ShapeCollisionJob(ExecuteProc *execProc, PhysicsController *data, RigidBodyController *body1, RigidBodyController *body2, const Shape *shape1, const Shape *shape2, unsigned_int32 index1, unsigned_int32 index2);
	};


	class GeometryCollisionJob : public BatchJob, public Memory<GeometryCollisionJob>
	{
		public:

			RigidBodyController		*rigidBody;
			const Shape				*shapeNode;
			unsigned_int32			shapeIndex;
			Geometry				*geometryNode;

			int32					contactCount;
			GeometryContact			*geometryContact;
			IntersectionData		intersectionData[kMaxIntersectionContactCount];

			GeometryCollisionJob(ExecuteProc *execProc, RigidBodyController *body, const Shape *shape, unsigned_int32 index, Geometry *geometry);
	};


	class ConstraintSolverJob : public BatchJob, public ListElement<ConstraintSolverJob>, public Memory<ConstraintSolverJob>
	{
		public:

			List<RigidBodyController>		rigidBodyList;
			int32							solverMultiplier;
			bool							repeatCollisionFlag;

			ConstraintSolverJob(ExecuteProc *execProc);

			using ListElement<ConstraintSolverJob>::Previous;
			using ListElement<ConstraintSolverJob>::Next;
	};


	template <> Heap Memory<ShapeCollisionJob>::heap("ShapeCollisionJob", 65536, kHeapMutexless);
	template class Memory<ShapeCollisionJob>;

	template <> Heap Memory<GeometryCollisionJob>::heap("GeometryCollisionJob", 65536, kHeapMutexless);
	template class Memory<GeometryCollisionJob>;

	template <> Heap Memory<ConstraintSolverJob>::heap("ConstraintSolverJob", 65536, kHeapMutexless);
	template class Memory<ConstraintSolverJob>;
}


const char C4::kConnectorKeyPhysics[] = "%Physics";


#if C4DIAGS

	SharedVertexBuffer RigidBodyRenderable::indexBuffer(kVertexBufferIndex | kVertexBufferStatic);

#endif


RigidBodyController::RigidBodyController() : RigidBodyController(kControllerRigidBody)
{
	SetControllerFlags(kControllerMoveInhibit);
}

RigidBodyController::RigidBodyController(ControllerType type) : BodyController(type, kBodyRigid)
{
	SetBaseControllerType(kControllerRigidBody);

	rigidBodyType = kRigidBodyGeneric;
	rigidBodyFlags = 0;
	rigidBodyState = 0;
 
	queryThreadFlags = 0;

	restitutionCoefficient = 0.0F; 
	frictionCoefficient = kDefaultFrictionCoefficient;
	spinFrictionMultiplier = 0.0F; 
	rollingResistance = 0.0F;

	collisionKind = kCollisionRigidBody; 
	collisionExclusionMask = 0;
 
	sleepState[0].boxMultiplier = 1.0F; 
	sleepState[0].axisMultiplier = 0.5F;
	sleepState[1].boxMultiplier = 2.0F;
	sleepState[1].axisMultiplier = 0.5F;
 
	submergedWaterBlock = nullptr;
	bodyVolume = 0.0F;
}

RigidBodyController::RigidBodyController(const RigidBodyController& rigidBodyController) : BodyController(rigidBodyController)
{
	rigidBodyType = rigidBodyController.rigidBodyType;
	rigidBodyFlags = rigidBodyController.rigidBodyFlags;
	rigidBodyState = 0;

	queryThreadFlags = 0;

	restitutionCoefficient = rigidBodyController.restitutionCoefficient;
	frictionCoefficient = rigidBodyController.frictionCoefficient;
	spinFrictionMultiplier = rigidBodyController.spinFrictionMultiplier;
	rollingResistance = rigidBodyController.rollingResistance;

	collisionKind = rigidBodyController.collisionKind;
	collisionExclusionMask = rigidBodyController.collisionExclusionMask;

	sleepState[0].boxMultiplier = rigidBodyController.sleepState[0].boxMultiplier;
	sleepState[0].axisMultiplier = rigidBodyController.sleepState[0].axisMultiplier;
	sleepState[1].boxMultiplier = rigidBodyController.sleepState[1].boxMultiplier;
	sleepState[1].axisMultiplier = rigidBodyController.sleepState[1].axisMultiplier;

	submergedWaterBlock = nullptr;
	bodyVolume = 0.0F;
}

RigidBodyController::~RigidBodyController()
{
	shapeList.RemoveAll();
	internalShapeList.RemoveAll();

	#if C4DIAGS

		delete rigidBodyRenderable.GetTarget();

	#endif
}

Controller *RigidBodyController::Replicate(void) const
{
	return (new RigidBodyController(*this));
}

void RigidBodyController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	BodyController::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << rigidBodyFlags;

	data << ChunkHeader('STAT', 4);
	data << rigidBodyState;

	data << ChunkHeader('REST', 4);
	data << restitutionCoefficient;

	data << ChunkHeader('FRIC', 4);
	data << frictionCoefficient;

	data << ChunkHeader('SPIN', 4);
	data << spinFrictionMultiplier;

	data << ChunkHeader('ROLL', 4);
	data << rollingResistance;

	data << ChunkHeader('COLL', 8);
	data << collisionKind;
	data << collisionExclusionMask;

	data << ChunkHeader('SNAP', 4);
	data << GetSnapshotPeriod();

	data << ChunkHeader('SMLT', 16);
	data << sleepState[0].boxMultiplier;
	data << sleepState[0].axisMultiplier;
	data << sleepState[1].boxMultiplier;
	data << sleepState[1].axisMultiplier;

	if (bodyVolume != 0.0F)
	{
		data << ChunkHeader('VOLU', 4);
		data << bodyVolume;

		data << ChunkHeader('MASS', 8 + sizeof(Point3D) + sizeof(InertiaTensor));
		data << bodyMass;
		data << inverseBodyMass;
		data << bodyCenterOfMass;
		data << bodyInertiaTensor;

		data << ChunkHeader('VELO', sizeof(Vector3D) + sizeof(Antivector3D));
		data << linearVelocity;
		data << angularVelocity;

		data << ChunkHeader('XFRM', sizeof(Transform4D) * 2);
		data << initialTransform;
		data << finalTransform;

		data << ChunkHeader('CENT', sizeof(Point3D) * 2);
		data << initialCenterOfMass;
		data << finalCenterOfMass;

		data << ChunkHeader('MOVE', sizeof(Vector3D) * 2 + 4);
		data << motionDisplacement;
		data << motionRotationAxis;
		data << motionRotationAngle;

		data << ChunkHeader('EXTN', sizeof(Vector3D) + sizeof(Antivector3D));
		data << externalForce;
		data << externalTorque;

		data << ChunkHeader('LRES', sizeof(Vector3D));
		data << externalLinearResistance;

		data << ChunkHeader('ARES', sizeof(Vector3D));
		data << externalAngularResistance;

		data << ChunkHeader('IMPL', sizeof(Vector3D) + sizeof(Antivector3D));
		data << impulseForce;
		data << impulseTorque;

		data << ChunkHeader('SLP0', 4 + sizeof(Point3D) * 6);
		data << sleepState[0].sleepStepCount;
		data << sleepState[0].centerSleepBox.min;
		data << sleepState[0].centerSleepBox.max;
		data << sleepState[0].axisSleepBox[0].min;
		data << sleepState[0].axisSleepBox[0].max;
		data << sleepState[0].axisSleepBox[1].min;
		data << sleepState[0].axisSleepBox[1].max;

		data << ChunkHeader('SLP1', 4 + sizeof(Point3D) * 6);
		data << sleepState[1].sleepStepCount;
		data << sleepState[1].centerSleepBox.min;
		data << sleepState[1].centerSleepBox.max;
		data << sleepState[1].axisSleepBox[0].min;
		data << sleepState[1].axisSleepBox[0].max;
		data << sleepState[1].axisSleepBox[1].min;
		data << sleepState[1].axisSleepBox[1].max;

		if ((submergedWaterBlock) && (submergedWaterBlock->LinkedNodePackable(packFlags)))
		{
			data << ChunkHeader('WBLK', 4);
			data << submergedWaterBlock->GetNodeIndex();
		}
	}

	data << TerminatorChunk;
}

void RigidBodyController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BodyController::Unpack(data, unpackFlags);
	UnpackChunkList<RigidBodyController>(data, unpackFlags);
}

bool RigidBodyController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> rigidBodyFlags;
			return (true);

		case 'STAT':

			data >> rigidBodyState;
			return (true);

		case 'REST':

			data >> restitutionCoefficient;
			return (true);

		case 'FRIC':

			data >> frictionCoefficient;

			#if C4LEGACY

				if (data.GetVersion() < 64)
				{
					frictionCoefficient *= 25.0F;
				}

			#endif

			return (true);

		case 'SPIN':

			data >> spinFrictionMultiplier;
			return (true);

		case 'ROLL':

			data >> rollingResistance;

			#if C4LEGACY

				if (data.GetVersion() < 65)
				{
					rollingResistance *= 0.02F;
				}

			#endif

			return (true);

		case 'COLL':

			data >> collisionKind;
			data >> collisionExclusionMask;
			return (true);

		case 'SNAP':
		{
			int32	period;

			data >> period;
			SetSnapshotPeriod(period);
			return (true);
		}

		case 'SMLT':

			data >> sleepState[0].boxMultiplier;
			data >> sleepState[0].axisMultiplier;
			data >> sleepState[1].boxMultiplier;
			data >> sleepState[1].axisMultiplier;
			return (true);

		case 'VOLU':

			data >> bodyVolume;
			return (true);

		case 'MASS':

			data >> bodyMass;
			data >> inverseBodyMass;
			data >> bodyCenterOfMass;
			data >> bodyInertiaTensor;
			return (true);

		case 'VELO':

			data >> linearVelocity;
			data >> angularVelocity;
			return (true);

		case 'XFRM':

			data >> initialTransform;
			data >> finalTransform;
			return (true);

		case 'CENT':

			data >> initialCenterOfMass;
			data >> finalCenterOfMass;
			return (true);

		case 'MOVE':

			data >> motionDisplacement;
			data >> motionRotationAxis;
			data >> motionRotationAngle;
			return (true);

		case 'EXTN':

			data >> externalForce;
			data >> externalTorque;
			return (true);

		case 'LRES':

			data >> externalLinearResistance;
			return (true);

		case 'ARES':

			data >> externalAngularResistance;
			return (true);

		case 'IMPL':

			data >> impulseForce;
			data >> impulseTorque;
			return (true);

		case 'SLP0':

			data >> sleepState[0].sleepStepCount;
			data >> sleepState[0].centerSleepBox.min;
			data >> sleepState[0].centerSleepBox.max;
			data >> sleepState[0].axisSleepBox[0].min;
			data >> sleepState[0].axisSleepBox[0].max;
			data >> sleepState[0].axisSleepBox[1].min;
			data >> sleepState[0].axisSleepBox[1].max;
			return (true);

		case 'SLP1':

			data >> sleepState[1].sleepStepCount;
			data >> sleepState[1].centerSleepBox.min;
			data >> sleepState[1].centerSleepBox.max;
			data >> sleepState[1].axisSleepBox[0].min;
			data >> sleepState[1].axisSleepBox[0].max;
			data >> sleepState[1].axisSleepBox[1].min;
			data >> sleepState[1].axisSleepBox[1].max;
			return (true);

		case 'WBLK':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &WaterBlockLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void RigidBodyController::WaterBlockLinkProc(Node *node, void *cookie)
{
	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(cookie);
	rigidBody->submergedWaterBlock = static_cast<WaterBlock *>(node);
}

int32 RigidBodyController::GetSettingCount(void) const
{
	return (16);
}

Setting *RigidBodyController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'FLAG'));
		return (new HeadingSetting('FLAG', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'FLAG', 'SLEP'));
		return (new BooleanSetting('SLEP', Asleep(), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'FLAG', 'IFRC'));
		return (new BooleanSetting('IFRC', ((rigidBodyFlags & kRigidBodyForceFieldInhibit) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP'));
		return (new HeadingSetting('PROP', title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP', 'GRAV'));
		return (new TextSetting('GRAV', GetGravityMultiplier(), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP', 'FLDG'));
		return (new TextSetting('FLDG', GetFluidDragMultiplier(), title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP', 'WNDG'));
		return (new TextSetting('WNDG', GetWindDragMultiplier(), title));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP', 'REST'));
		return (new FloatSetting('REST', restitutionCoefficient, title, 0.0F, 1.0F, 0.05F));
	}

	if (index == 8)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP', 'FRIC'));
		return (new TextSetting('FRIC', frictionCoefficient, title));
	}

	if (index == 9)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP', 'SPIN'));
		return (new TextSetting('SPIN', spinFrictionMultiplier, title));
	}

	if (index == 10)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'PROP', 'ROLL'));
		return (new TextSetting('ROLL', rollingResistance, title));
	}

	if (index == 11)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'SPRM'));
		return (new HeadingSetting('SPRM', title));
	}

	if (index == 12)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'SPRM', 'BOXM'));
		return (new TextSetting('BOXM', sleepState[0].boxMultiplier, title));
	}

	if (index == 13)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'SPRM', 'ROTM'));
		return (new TextSetting('ROTM', sleepState[0].axisMultiplier, title));
	}

	if (index == 14)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'NTWK'));
		return (new HeadingSetting('NTWK', title));
	}

	if (index == 15)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRigidBody, 'NTWK', 'SNAP'));
		return (new TextSetting('SNAP', Text::IntegerToString(GetSnapshotPeriod()), title, 3, &EditTextWidget::NumberFilter));
	}

	return (nullptr);
}

void RigidBodyController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SLEP')
	{
		unsigned_int32 flags = GetControllerFlags();
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			SetControllerFlags(flags | kControllerAsleep);
		}
		else
		{
			SetControllerFlags(flags & ~kControllerAsleep);
		}
	}
	else if (identifier == 'IFRC')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			rigidBodyFlags |= kRigidBodyForceFieldInhibit;
		}
		else
		{
			rigidBodyFlags &= ~kRigidBodyForceFieldInhibit;
		}
	}
	else if (identifier == 'GRAV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetGravityMultiplier(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'FLDG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetFluidDragMultiplier(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'WNDG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetWindDragMultiplier(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'REST')
	{
		restitutionCoefficient = FmaxZero(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'FRIC')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		frictionCoefficient = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'SPIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		spinFrictionMultiplier = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'ROLL')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		rollingResistance = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'BOXM')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		sleepState[0].boxMultiplier = Fmax(Text::StringToFloat(text), 1.0F);
	}
	else if (identifier == 'ROTM')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		sleepState[0].axisMultiplier = Clamp(Text::StringToFloat(text), 0.0F, 1.0F);
	}
	else if (identifier == 'SNAP')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetSnapshotPeriod(Max(Text::StringToInteger(text), 1));
	}
}

void RigidBodyController::Preprocess(void)
{
	Node *node = GetTargetNode();

	NodeType type = node->GetNodeType();
	if ((type == kNodeGeometry) || (type == kNodeEffect))
	{
		node->SetNodeFlags(node->GetNodeFlags() | kNodeVisibilitySite);
	}

	if (!node->GetManipulator())
	{
		if (Asleep())
		{
			rigidBodyState |= kRigidBodyAsleep;
		}

		Node *subnode = node;
		do
		{
			if (subnode->GetNodeType() == kNodeGeometry)
			{
				GeometryObject *object = static_cast<Geometry *>(subnode)->GetObject();
				object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
			}

			subnode = node->GetNextNode(subnode);
		} while (subnode);

		subnode = node->GetFirstSubnode();
		while (subnode)
		{
			if (subnode->GetNodeType() == kNodeShape)
			{
				Shape *shape = static_cast<Shape *>(subnode);
				shapeList.Append(shape);
			}

			subnode = subnode->Next();
		}

		if (bodyVolume == 0.0F)
		{
			bodyMass = 0.0F;
			bodyCenterOfMass.Set(0.0F, 0.0F, 0.0F);

			Shape *shape = shapeList.First();
			while (shape)
			{
				const ShapeObject *shapeObject = shape->GetObject();

				float volume = shapeObject->CalculateVolume();
				float density = shapeObject->GetShapeDensity();
				float mass = volume * density;
				bodyVolume += volume * NonzeroFsgn(density);
				bodyMass += mass;

				Point3D center = shapeObject->CalculateCenterOfMass();
				bodyCenterOfMass += shape->GetNodeTransform() * center * mass;

				shape = shape->Next();
			}

			bodyMass = Fmax(bodyMass, kMinRigidBodyMass);
			inverseBodyMass = 1.0F / bodyMass;
			bodyCenterOfMass *= inverseBodyMass;

			bodyInertiaTensor.Set(0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F);

			shape = shapeList.First();
			while (shape)
			{
				Shape *next = shape->Next();

				const ShapeObject *shapeObject = shape->GetObject();
				if (shapeObject->GetShapeFlags() & kShapeCollisionInhibit)
				{
					internalShapeList.Append(shape);
				}

				InertiaTensor inertia = shapeObject->CalculateInertiaTensor();
				const Transform4D& transform = shape->GetNodeTransform();

				inertia = Rotate(inertia, transform);
				inertia = Translate(inertia, bodyCenterOfMass - transform * shapeObject->CalculateCenterOfMass(), shapeObject->CalculateMass());
				bodyInertiaTensor += inertia;

				shape = next;
			}

			linearVelocity.Set(0.0F, 0.0F, 0.0F);
			angularVelocity.Set(0.0F, 0.0F, 0.0F);
			movementVelocity.Set(0.0F, 0.0F, 0.0F);

			motionDisplacement.Set(0.0F, 0.0F, 0.0F);
			motionRotationAxis.Set(0.0F, 0.0F, 1.0F);
			motionRotationAngle = 0.0F;

			initialTransform = node->GetNodeTransform();
			finalTransform = node->GetNodeTransform();

			externalForce.Set(0.0F, 0.0F, 0.0F);
			externalTorque.Set(0.0F, 0.0F, 0.0F);
			externalLinearResistance.Set(0.0F, 0.0F, 0.0F);
			externalAngularResistance = 0.0F;

			impulseForce.Set(0.0F, 0.0F, 0.0F);
			impulseTorque.Set(0.0F, 0.0F, 0.0F);

			sleepState[0].sleepStepCount = 0;
			sleepState[0].centerSleepBox.min = initialTransform * bodyCenterOfMass;
			sleepState[0].centerSleepBox.max = sleepState[0].centerSleepBox.min;
			sleepState[0].axisSleepBox[0].min = sleepState[0].centerSleepBox.min + initialTransform[0];
			sleepState[0].axisSleepBox[0].max = sleepState[0].axisSleepBox[0].min;
			sleepState[0].axisSleepBox[1].min = sleepState[0].centerSleepBox.min + initialTransform[1];
			sleepState[0].axisSleepBox[1].max = sleepState[0].axisSleepBox[1].min;

			sleepState[1].sleepStepCount = 0;
			sleepState[1].centerSleepBox.min = sleepState[0].centerSleepBox.min;
			sleepState[1].centerSleepBox.max = sleepState[0].centerSleepBox.min;
			sleepState[1].axisSleepBox[0].min = sleepState[0].axisSleepBox[0].min;
			sleepState[1].axisSleepBox[0].max = sleepState[0].axisSleepBox[0].min;
			sleepState[1].axisSleepBox[1].min = sleepState[0].axisSleepBox[1].min;
			sleepState[1].axisSleepBox[1].max = sleepState[0].axisSleepBox[1].min;
		}

		BodyController::Preprocess();

		Shape *shape = shapeList.First();
		const Node *super = node->GetSuperNode();
		if ((shape) && (super) && (super->GetNodeType() == kNodeZone))
		{
			PhysicsController *physicsController = GetPhysicsController();
			if (physicsController)
			{
				physicsController->AddRigidBody(this);
			}

			shape->CalculateBoundingBox(&boundingBox);
			boundingBox.Transform(shape->GetNodeTransform());

			for (;;)
			{
				Box3D	shapeBounds;

				shape = shape->Next();
				if (!shape)
				{
					break;
				}

				shape->CalculateBoundingBox(&shapeBounds);
				shapeBounds.Transform(shape->GetNodeTransform());
				boundingBox.Union(shapeBounds);
			}

			Vector3D p1 = boundingBox.min - bodyCenterOfMass;
			Vector3D p2 = boundingBox.max - bodyCenterOfMass;

			float x = Fmax(Fabs(p1.x), Fabs(p2.x));
			float y = Fmax(Fabs(p1.y), Fabs(p2.y));
			float z = Fmax(Fabs(p1.z), Fabs(p2.z));
			boundingRadius = Sqrt(x * x + y * y + z * z);

			bodyCollisionBox = Transform(boundingBox, finalTransform);
		}
		else
		{
			boundingBox.min.Set(0.0F, 0.0F, 0.0F);
			boundingBox.max.Set(0.0F, 0.0F, 0.0F);
			boundingRadius = 0.0F;
		}

		motionTransform.SetIdentity();

		initialCenterOfMass = initialTransform * bodyCenterOfMass;
		finalCenterOfMass = finalTransform * bodyCenterOfMass;

		networkDelta[0].Set(0.0F, 0.0F, 0.0F);
		networkDelta[1].Set(0.0F, 0.0F, 0.0F);
		networkDecay[0] = 0.0F;
		networkDecay[1] = 0.0F;
		networkParity = 0;
	}
	else
	{
		BodyController::Preprocess();
	}

	if (collisionKind == 0)
	{
		collisionKind = kCollisionRigidBody;
	}

	velocityMultiplier = 1.0F;
}

void RigidBodyController::Neutralize(void)
{
	PhysicsController *physicsController = GetPhysicsController();
	if (physicsController)
	{
		physicsController->RemoveRigidBody(this);
	}

	BodyController::Neutralize();
}

ControllerMessage *RigidBodyController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kRigidBodyMessageSnapshot:

			return (new RigidBodySnapshotMessage(GetControllerIndex()));

		case kRigidBodyMessageWake:

			return (new ControllerMessage(kRigidBodyMessageWake, GetControllerIndex()));

		case kRigidBodyMessageSleep:

			return (new RigidBodySleepMessage(GetControllerIndex()));
	}

	return (BodyController::CreateMessage(type));
}

void RigidBodyController::ReceiveMessage(const ControllerMessage *message)
{
	switch (message->GetControllerMessageType())
	{
		case kRigidBodyMessageSnapshot:
		{
			const RigidBodySnapshotMessage *m = static_cast<const RigidBodySnapshotMessage *>(message);

			Transform4D serverTransform(m->GetRigidBodyRotation().GetRotationMatrix(), m->GetRigidBodyPosition());

			unsigned_int32 parity = networkParity;
			networkDelta[parity] = serverTransform.GetTranslation() - finalTransform.GetTranslation();
			networkDecay[parity] = 1.0F;

			initialTransform.SetMatrix3D(serverTransform);
			initialTransform[3] += networkDelta[parity];
			finalTransform = serverTransform;

			networkParity = parity ^ 1;

			linearVelocity = m->GetRigidBodyLinearVelocity();
			angularVelocity = m->GetRigidBodyAngularVelocity();

			finalCenterOfMass = finalTransform * bodyCenterOfMass;
			motionDisplacement = linearVelocity * kTimeStep;

			float w = SquaredMag(angularVelocity);
			if (w > K::min_float)
			{
				float r = InverseSqrt(w);
				motionRotationAxis = angularVelocity * r;
				motionRotationAngle = w * r * kTimeStep;
			}
			else
			{
				motionRotationAxis.Set(0.0F, 0.0F, 1.0F);
				motionRotationAngle = 0.0F;
			}

			break;
		}

		case kRigidBodyMessageWake:

			Wake();
			break;

		case kRigidBodyMessageSleep:
		{
			const RigidBodySleepMessage *m = static_cast<const RigidBodySleepMessage *>(message);

			finalTransform.Set(m->GetRigidBodyRotation().GetRotationMatrix(), m->GetRigidBodyPosition());
			Sleep();
			break;
		}

		default:

			BodyController::ReceiveMessage(message);
			break;
	}
}

void RigidBodyController::SendInitialStateMessages(Player *player) const
{
	Quaternion	rotation;

	const Point3D& position = finalTransform.GetTranslation();
	rotation.SetRotationMatrix(finalTransform);

	if (RigidBodyAsleep())
	{
		player->SendMessage(RigidBodySleepMessage(GetControllerIndex(), position, rotation));
	}
	else
	{
		player->SendMessage(ControllerMessage(kRigidBodyMessageWake, GetControllerIndex()));
		player->SendMessage(RigidBodySnapshotMessage(GetControllerIndex(), position, rotation, linearVelocity, angularVelocity));
	}
}

void RigidBodyController::SendSnapshot(void)
{
	RigidBodySnapshotMessage message(GetControllerIndex(), finalTransform.GetTranslation(), Quaternion().SetRotationMatrix(finalTransform), linearVelocity, angularVelocity);
	message.SetMessageFlags(kMessageUnreliable);
	TheMessageMgr->SendMessageClients(message);
}

void RigidBodyController::Wake(void)
{
	BodyController::Wake();

	PhysicsController *physicsController = GetPhysicsController();
	if ((physicsController) && (!shapeList.Empty()))
	{
		sleepState[0].sleepStepCount = 0;
		sleepState[1].sleepStepCount = 0;

		physicsController->WakeRigidBody(this);

		if ((!(rigidBodyFlags & kRigidBodyLocalSimulation)) && (TheMessageMgr->Server()))
		{
			TheMessageMgr->AddSnapshotSender(this);
			TheMessageMgr->SendMessageClients(ControllerMessage(kRigidBodyMessageWake, GetControllerIndex()));
		}

		Node *node = GetTargetNode();
		Node *subnode = node;
		do
		{
			NodeType type = subnode->GetNodeType();
			if (type == kNodeGeometry)
			{
				Geometry *geometry = static_cast<Geometry *>(subnode);
				if (geometry->GetGeometryType() == kGeometryGeneric)
				{
					geometry->SetRenderableFlags(geometry->GetRenderableFlags() | kRenderableMotionBlurGradient);
				}
			}

			subnode = node->GetNextNode(subnode);
		} while (subnode);
	}
}

void RigidBodyController::Sleep(void)
{
	SetRigidBodyTransform(finalTransform);

	bodyCollisionBox = Transform(boundingBox, finalTransform);
	linearVelocity.Set(0.0F, 0.0F, 0.0F);
	angularVelocity.Set(0.0F, 0.0F, 0.0F);

	PhysicsController *physicsController = GetPhysicsController();
	if (physicsController)
	{
		physicsController->SleepRigidBody(this);

		if ((!(rigidBodyFlags & kRigidBodyLocalSimulation)) && (TheMessageMgr->Server()))
		{
			TheMessageMgr->RemoveSnapshotSender(this);
			TheMessageMgr->SendMessageClients(RigidBodySleepMessage(GetControllerIndex(), finalTransform.GetTranslation(), Quaternion().SetRotationMatrix(finalTransform)));
		}
	}

	if (!(rigidBodyFlags & kRigidBodyPartialSleep))
	{
		BodyController::Sleep();
	}

	Node *node = GetTargetNode();
	node->Invalidate();

	Node *subnode = node;
	do
	{
		NodeType type = subnode->GetNodeType();
		if (type == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(subnode);
			if (geometry->GetGeometryType() == kGeometryGeneric)
			{
				geometry->SetRenderableFlags(geometry->GetRenderableFlags() & ~kRenderableMotionBlurGradient);
			}
		}

		subnode = node->GetNextNode(subnode);
	} while (subnode);

	#if C4DIAGS

		delete rigidBodyRenderable.GetTarget();

	#endif
}

void RigidBodyController::RecursiveWake(void)
{
	if (TheMessageMgr->Server())
	{
		ExpediteSnapshot();

		if (RigidBodyAsleep())
		{
			Wake();

			PhysicsController *physicsController = GetPhysicsController();
			if (physicsController->simulationList)
			{
				CalculateAppliedForces(physicsController->gravityAcceleration);
				DetectWorldCollisions(++physicsController->geometryCollisionStamp);
			}

			const Contact *contact = GetFirstOutgoingEdge();
			while (contact)
			{
				contact->GetFinishElement()->RecursiveWake();
				contact = contact->GetNextOutgoingEdge();
			}

			contact = GetFirstIncomingEdge();
			while (contact)
			{
				contact->GetStartElement()->RecursiveWake();
				contact = contact->GetNextIncomingEdge();
			}
		}
	}
}

void RigidBodyController::RecursiveKeepAwake(void)
{
	sleepState[0].sleepStepCount = Min(sleepState[0].sleepStepCount, kRigidBodySleepStepCount - 1);
	sleepState[1].sleepStepCount = Min(sleepState[1].sleepStepCount, kRigidBodySleepStepCount * 2 - 1);

	const Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		Body *body = contact->GetFinishElement();
		if (body->GetBodyType() == kBodyRigid)
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(body);
			if ((rigidBody->sleepState[0].sleepStepCount >= kRigidBodySleepStepCount) || (rigidBody->sleepState[1].sleepStepCount >= kRigidBodySleepStepCount * 2))
			{
				rigidBody->RecursiveKeepAwake();
			}
		}

		contact = contact->GetNextOutgoingEdge();
	}

	contact = GetFirstIncomingEdge();
	while (contact)
	{
		Body *body = contact->GetStartElement();
		if (body->GetBodyType() == kBodyRigid)
		{
			RigidBodyController *rigidBody = static_cast<RigidBodyController *>(body);
			if ((rigidBody->sleepState[0].sleepStepCount >= kRigidBodySleepStepCount) || (rigidBody->sleepState[1].sleepStepCount >= kRigidBodySleepStepCount * 2))
			{
				rigidBody->RecursiveKeepAwake();
			}
		}

		contact = contact->GetNextIncomingEdge();
	}
}

RigidBodyContact *RigidBodyController::FindOutgoingBodyContact(const RigidBodyController *rigidBody, unsigned_int32 startSignature, unsigned_int32 finishSignature) const
{
	Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		if ((contact->GetFinishElement() == rigidBody) && (contact->GetContactType() == kContactRigidBody))
		{
			RigidBodyContact *bodyContact = static_cast<RigidBodyContact *>(contact);
			if ((bodyContact->GetStartSignature() == startSignature) && (bodyContact->GetFinishSignature() == finishSignature))
			{
				return (bodyContact);
			}
		}

		contact = contact->GetNextOutgoingEdge();
	}

	return (nullptr);
}

RigidBodyContact *RigidBodyController::FindIncomingBodyContact(const RigidBodyController *rigidBody, unsigned_int32 startSignature, unsigned_int32 finishSignature) const
{
	Contact *contact = GetFirstIncomingEdge();
	while (contact)
	{
		if ((contact->GetStartElement() == rigidBody) && (contact->GetContactType() == kContactRigidBody))
		{
			RigidBodyContact *bodyContact = static_cast<RigidBodyContact *>(contact);
			if ((bodyContact->GetStartSignature() == startSignature) && (bodyContact->GetFinishSignature() == finishSignature))
			{
				return (bodyContact);
			}
		}

		contact = contact->GetNextIncomingEdge();
	}

	return (nullptr);
}

bool RigidBodyController::FindGeometryContact(const Geometry *geometry, unsigned_int32 signature, GeometryContact **matchingContact) const
{
	bool result = false;

	Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		if (contact->GetContactType() == kContactGeometry)
		{
			GeometryContact *geometryContact = static_cast<GeometryContact *>(contact);
			if (geometryContact->GetContactGeometry() == geometry)
			{
				if (geometryContact->GetContactSignature() == signature)
				{
					*matchingContact = geometryContact;
					return (true);
				}

				result = true;
			}
		}

		contact = contact->GetNextOutgoingEdge();
	}

	*matchingContact = nullptr;
	return (result);
}

inline void RigidBodyController::AdjustDisplacement(float param)
{
	finalTransform[3] -= motionDisplacement * (1.0F - param);
	motionDisplacement *= param;

	finalCenterOfMass = initialCenterOfMass + motionDisplacement;
	transientLinearVelocity = motionDisplacement * kInverseTimeStep;
}

void RigidBodyController::KillLaterContacts(float param)
{
	Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		Contact *next = contact->GetNextOutgoingEdge();

		if ((contact->GetNotificationMask() != 0) && (contact->GetContactParam() > param))
		{
			contact->Kill();
		}

		contact = next;
	}

	contact = GetFirstIncomingEdge();
	while (contact)
	{
		Contact *next = contact->GetNextIncomingEdge();

		if ((contact->GetNotificationMask() != 0) && (contact->GetContactParam() > param))
		{
			contact->Kill();
		}

		contact = next;
	}
}

void RigidBodyController::JobDetectGeometryCollision(Job *job, void *cookie)
{
	GeometryCollisionJob *geometryJob = static_cast<GeometryCollisionJob *>(job);

	const Geometry *geometry = geometryJob->geometryNode;
	const GeometryObject *object = geometry->GetObject();

	if (geometryJob->rigidBody->FindGeometryContact(geometry, geometryJob->shapeIndex, &geometryJob->geometryContact))
	{
		if (object->GetConvexPrimitiveFlag())
		{
			if (geometryJob->shapeNode->StaticIntersectPrimitive(static_cast<const PrimitiveGeometry *>(geometry), geometryJob->geometryContact, &geometryJob->intersectionData[0]))
			{
				if (geometryJob->geometryContact)
				{
					geometryJob->SetFinalizeProc(&FinalizeExistingStaticGeometrySingleContact);
				}
				else
				{
					geometryJob->SetFinalizeProc(&FinalizeNewStaticGeometrySingleContact);
				}
			}
		}
		else if (object->GetGeometryFlags() & kGeometryConvexHull)
		{
			if (geometryJob->shapeNode->StaticIntersectConvexHull(geometry, geometryJob->geometryContact, &geometryJob->intersectionData[0]))
			{
				if (geometryJob->geometryContact)
				{
					geometryJob->SetFinalizeProc(&FinalizeExistingStaticGeometrySingleContact);
				}
				else
				{
					geometryJob->SetFinalizeProc(&FinalizeNewStaticGeometrySingleContact);
				}
			}
		}
		else
		{
			GeometryContact *geometryContact = geometryJob->geometryContact;
			if (geometryContact)
			{
				unsigned_int32	triangleIndex[ConstraintContact::kMaxSubcontactCount];

				int32 triangleCount = geometryContact->GetSubcontactCount();
				for (machine a = 0; a < triangleCount; a++)
				{
					triangleIndex[a] = geometryContact->GetSubcontact(a)->triangleIndex;
				}

				int32 contactCount = geometryJob->shapeNode->MixedIntersectGeometry(geometry, geometryJob->rigidBody->motionDisplacement, triangleCount, triangleIndex, geometryJob->intersectionData);
				if (contactCount != 0)
				{
					geometryJob->contactCount = contactCount;
					geometryJob->SetFinalizeProc(&FinalizeExistingStaticGeometryMultipleContact);
				}
			}
			else
			{
				int32 contactCount = geometryJob->shapeNode->StaticIntersectGeometry(geometry, geometryJob->intersectionData);
				if (contactCount != 0)
				{
					geometryJob->contactCount = contactCount;
					geometryJob->SetFinalizeProc(&FinalizeNewStaticGeometryMultipleContact);
				}
			}
		}
	}
	else
	{
		if (object->GetConvexPrimitiveFlag())
		{
			if (geometryJob->shapeNode->DynamicIntersectPrimitive(static_cast<const PrimitiveGeometry *>(geometry), geometryJob->rigidBody->motionDisplacement, &geometryJob->intersectionData[0]))
			{
				geometryJob->SetFinalizeProc(&FinalizeNewDynamicGeometrySingleContact);
			}
		}
		else if (object->GetGeometryFlags() & kGeometryConvexHull)
		{
			if (geometryJob->shapeNode->DynamicIntersectConvexHull(geometry, geometryJob->rigidBody->motionDisplacement, &geometryJob->intersectionData[0]))
			{
				geometryJob->SetFinalizeProc(&FinalizeNewDynamicGeometrySingleContact);
			}
		}
		else
		{
			int32 contactCount = geometryJob->shapeNode->DynamicIntersectGeometry(geometry, geometryJob->rigidBody->motionDisplacement, geometryJob->intersectionData);
			if (contactCount != 0)
			{
				geometryJob->contactCount = contactCount;
				geometryJob->SetFinalizeProc(&FinalizeNewDynamicGeometryMultipleContact);
			}
		}
	}
}

void RigidBodyController::FinalizeExistingStaticGeometrySingleContact(Job *job, void *cookie)
{
	const GeometryCollisionJob *geometryJob = static_cast<GeometryCollisionJob *>(job);
	geometryJob->geometryContact->UpdateContact(&geometryJob->intersectionData[0]);
}

void RigidBodyController::FinalizeNewStaticGeometrySingleContact(Job *job, void *cookie)
{
	const GeometryCollisionJob *geometryJob = static_cast<GeometryCollisionJob *>(job);
	RigidBodyController *rigidBody = geometryJob->rigidBody;

	new GeometryContact(geometryJob->geometryNode, rigidBody, &geometryJob->intersectionData[0], geometryJob->shapeIndex);

	GeometryCollisionJob *repeatJob = new GeometryCollisionJob(&RigidBodyController::JobDetectGeometryCollision, rigidBody, geometryJob->shapeNode, geometryJob->shapeIndex, geometryJob->geometryNode);
	rigidBody->GetPhysicsController()->repeatCollisionJobArray.AddElement(repeatJob);
	rigidBody->repeatCollisionFlag = true;
}

void RigidBodyController::FinalizeExistingStaticGeometryMultipleContact(Job *job, void *cookie)
{
	const GeometryCollisionJob *geometryJob = static_cast<GeometryCollisionJob *>(job);
	for (machine a = 0; a < geometryJob->contactCount; a++)
	{
		geometryJob->geometryContact->UpdateContact(&geometryJob->intersectionData[a]);
	}
}

void RigidBodyController::FinalizeNewStaticGeometryMultipleContact(Job *job, void *cookie)
{
	const GeometryCollisionJob *geometryJob = static_cast<GeometryCollisionJob *>(job);
	RigidBodyController *rigidBody = geometryJob->rigidBody;

	GeometryContact *geometryContact = new GeometryContact(geometryJob->geometryNode, rigidBody, geometryJob->intersectionData, geometryJob->shapeIndex);
	for (machine a = 1; a < geometryJob->contactCount; a++)
	{
		geometryContact->UpdateContact(&geometryJob->intersectionData[a]);
	}

	GeometryCollisionJob *repeatJob = new GeometryCollisionJob(&RigidBodyController::JobDetectGeometryCollision, rigidBody, geometryJob->shapeNode, geometryJob->shapeIndex, geometryJob->geometryNode);
	rigidBody->GetPhysicsController()->repeatCollisionJobArray.AddElement(repeatJob);
	rigidBody->repeatCollisionFlag = true;
}

void RigidBodyController::FinalizeNewDynamicGeometrySingleContact(Job *job, void *cookie)
{
	const GeometryCollisionJob *geometryJob = static_cast<GeometryCollisionJob *>(job);
	RigidBodyController *rigidBody = geometryJob->rigidBody;

	float param = geometryJob->intersectionData[0].contactParam;
	if (param < 1.0F)
	{
		rigidBody->AdjustDisplacement(param);
		rigidBody->KillLaterContacts(param);
	}

	new GeometryContact(geometryJob->geometryNode, rigidBody, &geometryJob->intersectionData[0], geometryJob->shapeIndex);

	GeometryCollisionJob *repeatJob = new GeometryCollisionJob(&RigidBodyController::JobDetectGeometryCollision, rigidBody, geometryJob->shapeNode, geometryJob->shapeIndex, geometryJob->geometryNode);
	rigidBody->GetPhysicsController()->repeatCollisionJobArray.AddElement(repeatJob);
	rigidBody->repeatCollisionFlag = true;
}

void RigidBodyController::FinalizeNewDynamicGeometryMultipleContact(Job *job, void *cookie)
{
	const GeometryCollisionJob *geometryJob = static_cast<GeometryCollisionJob *>(job);
	RigidBodyController *rigidBody = geometryJob->rigidBody;

	float param = geometryJob->intersectionData[0].contactParam;
	for (machine a = 1; a < geometryJob->contactCount; a++)
	{
		param = Fmin(param, geometryJob->intersectionData[a].contactParam);
	}

	if (param < 1.0F)
	{
		rigidBody->AdjustDisplacement(param);
		rigidBody->KillLaterContacts(param);
	}

	GeometryContact *geometryContact = new GeometryContact(geometryJob->geometryNode, rigidBody, geometryJob->intersectionData, geometryJob->shapeIndex);
	for (machine a = 1; a < geometryJob->contactCount; a++)
	{
		geometryContact->UpdateContact(&geometryJob->intersectionData[a]);
	}

	GeometryCollisionJob *repeatJob = new GeometryCollisionJob(&RigidBodyController::JobDetectGeometryCollision, rigidBody, geometryJob->shapeNode, geometryJob->shapeIndex, geometryJob->geometryNode);
	rigidBody->GetPhysicsController()->repeatCollisionJobArray.AddElement(repeatJob);
	rigidBody->repeatCollisionFlag = true;
}

void RigidBodyController::DetectGeometryCollision(Geometry *geometry, const Point3D& p1, const Point3D& p2)
{
	const GeometryObject *object = geometry->GetObject();
	if ((object->GetCollisionOctree()) && (ValidGeometryCollision(geometry)))
	{
		const Transform4D& inverseTransform = geometry->GetInverseWorldTransform();
		Point3D q1 = inverseTransform * p1;
		Point3D q2 = inverseTransform * p2;

		if (!object->ExteriorSweptSphere(q1, q2, boundingRadius))
		{
			unsigned_int32 shapeIndex = 0;
			Shape *shape = shapeList.First();
			while (shape)
			{
				PhysicsController *physicsController = GetPhysicsController();
				physicsController->IncrementPhysicsCounter(kPhysicsCounterGeometryIntersection);

				GeometryCollisionJob *job = new GeometryCollisionJob(&JobDetectGeometryCollision, this, shape, shapeIndex, geometry);
				TheJobMgr->SubmitJob(job, &physicsController->collisionBatch);

				shapeIndex++;
				shape = shape->Next();
			}
		}
	}
}

void RigidBodyController::DetectNodeCollision(Node *node, unsigned_int32 stamp, const Point3D& p1, const Point3D& p2)
{
	if (node->Enabled())
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			Geometry *geometry = static_cast<Geometry *>(node);
			if (geometry->GetCollisionStamp() != stamp)
			{
				geometry->SetCollisionStamp(stamp);
				DetectGeometryCollision(geometry, p1, p2);
			}
		}

		const Bond *bond = node->GetFirstOutgoingEdge();
		while (bond)
		{
			Site *site = bond->GetFinishElement();
			if (site->GetWorldBoundingBox().Intersection(bodyCollisionBox))
			{
				DetectNodeCollision(static_cast<Node *>(site), stamp, p1, p2);
			}

			bond = bond->GetNextOutgoingEdge();
		}
	}
}

void RigidBodyController::DetectCellCollision(const Site *cell, unsigned_int32 stamp, const Point3D& p1, const Point3D& p2)
{
	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(bodyCollisionBox))
		{
			if (site->GetCellIndex() < 0)
			{
				DetectNodeCollision(static_cast<Node *>(site), stamp, p1, p2);
			}
			else
			{
				DetectCellCollision(site, stamp, p1, p2);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void RigidBodyController::DetectZoneCollision(const Zone *zone, unsigned_int32 stamp, const Point3D& p1, const Point3D& p2)
{
	DetectCellCollision(zone->GetCellGraphSite(kCellGraphGeometry), stamp, p1, p2);

	Zone *subzone = zone->GetFirstSubzone();
	while (subzone)
	{
		const Transform4D& transform = subzone->GetInverseWorldTransform();
		if (!subzone->GetObject()->ExteriorSweptSphere(transform * p1, transform * p2, boundingRadius))
		{
			DetectZoneCollision(subzone, stamp, p1, p2);
		}

		subzone = subzone->Next();
	}
}

void RigidBodyController::DetectWorldCollisions(unsigned_int32 stamp)
{
	repeatCollisionFlag = false;
	bodyCollisionBox = Union(Transform(boundingBox, initialTransform), Transform(boundingBox, finalTransform));

	#if C4DIAGS

		World *world = GetTargetNode()->GetWorld();
		if (world->GetDiagnosticFlags() & kDiagnosticRigidBodies)
		{
			RigidBodyRenderable *renderable = rigidBodyRenderable;
			if (renderable)
			{
				renderable->SetCollisionBox(bodyCollisionBox);
			}
			else
			{
				renderable = new RigidBodyRenderable(bodyCollisionBox);
				rigidBodyRenderable = renderable;
				world->AddRigidBodyRenderable(renderable);
			}
		}

	#endif

	Shape *shape = shapeList.First();
	while (shape)
	{
		shape->CalculateCollisionBox(initialTransform, finalTransform);
		shape = shape->Next();
	}

	if (inverseBodyScalarInertia > K::min_float)
	{
		Zone *zone = static_cast<Zone *>(GetTargetNode()->GetSuperNode());
		DetectZoneCollision(zone, stamp, initialCenterOfMass, finalCenterOfMass);
	}
}

void RigidBodyController::ApplyCellForceFields(Site *cell, const Box3D& box, unsigned_int32 fieldStamp)
{
	const Bond *bond = cell->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(box))
		{
			if (site->GetCellIndex() < 0)
			{
				Field *field = static_cast<Field *>(site);

				unsigned_int32 stamp = fieldStamp;
				if (field->GetSiteStamp() != stamp)
				{
					field->SetSiteStamp(stamp);

					if ((field->Enabled()) && (!field->GetObject()->ExteriorSphere(field->GetInverseWorldTransform() * initialCenterOfMass, boundingRadius)))
					{
						Vector3D		force;
						Antivector3D	torque;

						if (field->GetForce()->ApplyForce(this, initialTransform, &force, &torque))
						{
							appliedForce += force;
							appliedTorque += torque;
						}
					}
				}
			}
			else
			{
				ApplyCellForceFields(site, box, fieldStamp);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void RigidBodyController::CalculateAppliedForces(const Vector3D& gravity)
{
	movementVelocity.Set(0.0F, 0.0F, 0.0F);
	transientLinearVelocity.Set(0.0F, 0.0F, 0.0F);
	transientAngularVelocity.Set(0.0F, 0.0F, 0.0F);
	originalLinearVelocity = linearVelocity;
	originalAngularVelocity = angularVelocity;
	linearCorrection.Set(0.0F, 0.0F, 0.0F);
	angularCorrection.Set(0.0F, 0.0F, 0.0F);

	initialTransform = finalTransform;
	initialCenterOfMass = initialTransform * bodyCenterOfMass;

	appliedForce = gravity * (bodyMass * GetGravityMultiplier()) + externalForce - (linearVelocity & externalLinearResistance) + initialTransform * impulseForce;
	appliedTorque = externalTorque - angularVelocity * externalAngularResistance + initialTransform * impulseTorque;

	impulseForce.Set(0.0F, 0.0F, 0.0F);
	impulseTorque.Set(0.0F, 0.0F, 0.0F);

	const WaterBlock *waterBlock = submergedWaterBlock;
	submergedWaterBlock = nullptr;

	unsigned_int32 flags = rigidBodyFlags;
	if (!(flags & kRigidBodyForceFieldInhibit))
	{
		unsigned_int32 fieldStamp = GetPhysicsController()->IncrementFieldStamp();
		for (Zone *zone : GetTargetNode()->GetZoneMembershipArray())
		{
			ApplyCellForceFields(zone->GetCellGraphSite(kCellGraphField), Transform(boundingBox, initialTransform), fieldStamp);
		}
	}

	if (flags & (kRigidBodyFixedOrientation | kRigidBodyImmovable))
	{
		if (flags & kRigidBodyImmovable)
		{
			inverseBodyScalarInertia = 0.0F;
		}
		else
		{
			inverseBodyScalarInertia = inverseBodyMass;
		}

		inverseWorldInertiaTensor.Set(0.0F, 0.0F, 0.0F);
	}
	else
	{
		inverseBodyScalarInertia = inverseBodyMass;
		inverseWorldInertiaTensor = Inverse(Rotate(bodyInertiaTensor, initialTransform));
	}

	linearVelocity = (linearVelocity + appliedForce * (inverseBodyScalarInertia * kTimeStep)) * velocityMultiplier;
	angularVelocity += (inverseWorldInertiaTensor * appliedTorque) * kTimeStep;

	initialLinearVelocity = linearVelocity;
	initialAngularVelocity = angularVelocity;

	Integrate();

	if ((submergedWaterBlock != waterBlock) && (submergedWaterBlock))
	{
		HandleWaterSubmergence();
	}
}

void RigidBodyController::Integrate(void)
{
	motionDisplacement = (linearVelocity + movementVelocity + transientLinearVelocity) * kTimeStep;
	finalCenterOfMass = initialCenterOfMass + motionDisplacement;

	Antivector3D angular = angularVelocity + transientAngularVelocity;
	float w = SquaredMag(angular);
	if (w > K::min_float)
	{
		Matrix3D	rotation;

		float r = InverseSqrt(w);
		motionRotationAxis = angular * r;
		motionRotationAngle = w * r * kTimeStep;
		rotation.SetRotationAboutAxis(motionRotationAngle, motionRotationAxis);

		motionTransform.Set(rotation, finalCenterOfMass - rotation * initialCenterOfMass);
		finalTransform = motionTransform * initialTransform;
	}
	else
	{
		motionRotationAxis.Set(0.0F, 0.0F, 1.0F);
		motionRotationAngle = 0.0F;

		motionTransform.SetDisplacement(motionDisplacement);
		finalTransform.Set(initialTransform[0], initialTransform[1], initialTransform[2], initialTransform.GetTranslation() + motionDisplacement);
	}
}

void RigidBodyController::InitializeConstraints(void)
{
	Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		Contact *next = contact->GetNextOutgoingEdge();

		if (contact->Enabled())
		{
			contact->InitializeConstraints();
		}
		else if (contact->Dead())
		{
			delete contact;
		}

		contact = next;
	}
}

void RigidBodyController::InitializeRepeatConstraints(void)
{
	Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		Contact *next = contact->GetNextOutgoingEdge();

		if (contact->Enabled())
		{
			contact->InitializeRepeatConstraints();
		}
		else if (contact->Dead())
		{
			delete contact;
		}

		contact = next;
	}
}

void RigidBodyController::Finalize(const Box3D *physicsBoundingBox)
{
	Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		if (contact->TakeNotificationFlag(kContactNotificationOutgoing))
		{
			RigidBodyStatus		status;

			if (contact->GetContactType() == kContactRigidBody)
			{
				RigidBodyContact *rigidBodyContact = static_cast<RigidBodyContact *>(contact);
				status = HandleNewRigidBodyContact(rigidBodyContact, static_cast<RigidBodyController *>(rigidBodyContact->GetFinishElement()));

				if (status == kRigidBodyUnchanged)
				{
					continue;
				}
			}
			else
			{
				const GeometryContact *geometryContact = static_cast<GeometryContact *>(contact);
				status = HandleNewGeometryContact(geometryContact);

				if (status == kRigidBodyUnchanged)
				{
					linearVelocity -= geometryContact->GetContactGeometry()->GetGeometryVelocity();
					continue;
				}
			}

			if (status == kRigidBodyContactsBroken)
			{
				contact = GetFirstOutgoingEdge();
				continue;
			}

			// status == kRigidBodyDestroyed

			return;
		}

		contact = contact->GetNextOutgoingEdge();
	}

	contact = GetFirstIncomingEdge();
	while (contact)
	{
		if (contact->TakeNotificationFlag(kContactNotificationIncoming))
		{
			RigidBodyContact *rigidBodyContact = static_cast<RigidBodyContact *>(contact);
			RigidBodyStatus status = HandleNewRigidBodyContact(rigidBodyContact, static_cast<RigidBodyController *>(rigidBodyContact->GetStartElement()));

			if (status != kRigidBodyUnchanged)
			{
				if (status == kRigidBodyContactsBroken)
				{
					contact = GetFirstIncomingEdge();
					continue;
				}

				// status == kRigidBodyDestroyed

				return;
			}
		}

		contact = contact->GetNextIncomingEdge();
	}

	PhysicsController *physicsController = GetPhysicsController();

	float maxLinearSpeed = physicsController->GetMaxLinearSpeed();
	float linearSpeed = Magnitude(linearVelocity);
	if (linearSpeed > maxLinearSpeed)
	{
		linearVelocity *= maxLinearSpeed / linearSpeed;
	}

	float maxAngularSpeed = physicsController->GetMaxAngularSpeed();
	float angularSpeed = Magnitude(angularVelocity);
	if (angularSpeed > maxAngularSpeed)
	{
		angularVelocity *= maxAngularSpeed / angularSpeed;
	}

	if (!(rigidBodyFlags & kRigidBodyKeepAwake))
	{
		for (machine a = 0; a < 2; a++)
		{
			SleepState *state = &sleepState[a];

			#if C4SIMD

				float	maxBoxSize;

				vec_float c1 = VecLoad(&finalTransform(0,0));
				vec_float c2 = VecLoad(&finalTransform(0,0), 4);

				vec_float centerPoint = VecTransformPoint3D(c1, c2, VecLoad(&finalTransform(0,0), 8), VecLoad(&finalTransform(0,0), 12), VecLoadUnaligned(&bodyCenterOfMass.x));
				vec_float centerDiff = state->centerSleepBox.IncludePoint(centerPoint);
				centerDiff = VecMax(VecMax(VecSmearX(centerDiff), VecSmearY(centerDiff)), VecSmearZ(centerDiff));

				vec_float axisPoint1 = VecAdd(centerPoint, c1);
				vec_float axisDiff1 = state->axisSleepBox[0].IncludePoint(axisPoint1);
				axisDiff1 = VecMax(VecMax(VecSmearX(axisDiff1), VecSmearY(axisDiff1)), VecSmearZ(axisDiff1));

				vec_float axisPoint2 = VecAdd(centerPoint, c2);
				vec_float axisDiff2 = state->axisSleepBox[1].IncludePoint(axisPoint2);
				axisDiff2 = VecMax(VecMax(VecSmearX(axisDiff2), VecSmearY(axisDiff2)), VecSmearZ(axisDiff2));

				VecStoreX(VecMax(VecMul(VecMax(axisDiff1, axisDiff2), VecLoadScalar(&state->axisMultiplier)), centerDiff), &maxBoxSize);

			#else

				Point3D centerPoint = finalTransform * bodyCenterOfMass;
				Vector3D centerDiff = state->centerSleepBox.IncludePoint(centerPoint);
				float centerBoxSize = Fmax(centerDiff.x, centerDiff.y, centerDiff.z);

				Point3D axisPoint1 = centerPoint + finalTransform[0];
				Vector3D axisDiff1 = state->axisSleepBox[0].IncludePoint(axisPoint1);
				float axisBoxSize1 = Fmax(axisDiff1.x, axisDiff1.y, axisDiff1.z);

				Point3D axisPoint2 = centerPoint + finalTransform[1];
				Vector3D axisDiff2 = state->axisSleepBox[1].IncludePoint(axisPoint2);
				float axisBoxSize2 = Fmax(axisDiff2.x, axisDiff2.y, axisDiff2.z);

				float maxBoxSize = Fmax(Fmax(axisBoxSize1, axisBoxSize2) * state->axisMultiplier, centerBoxSize);

			#endif

			if (maxBoxSize > state->boxMultiplier * kRigidBodySleepBoxSize)
			{
				#if C4SIMD

					VecStore3D(centerPoint, &state->centerSleepBox.min.x);
					VecStore3D(centerPoint, &state->centerSleepBox.max.x);
					VecStore3D(axisPoint1, &state->axisSleepBox[0].min.x);
					VecStore3D(axisPoint1, &state->axisSleepBox[0].max.x);
					VecStore3D(axisPoint2, &state->axisSleepBox[1].min.x);
					VecStore3D(axisPoint2, &state->axisSleepBox[1].max.x);

				#else

					state->centerSleepBox.min = centerPoint;
					state->centerSleepBox.max = centerPoint;
					state->axisSleepBox[0].min = axisPoint1;
					state->axisSleepBox[0].max = axisPoint1;
					state->axisSleepBox[1].min = axisPoint2;
					state->axisSleepBox[1].max = axisPoint2;

				#endif

				state->sleepStepCount = 0;
			}
			else
			{
				state->sleepStepCount++;
			}
		}
	}

	if ((physicsBoundingBox) && (!physicsBoundingBox->Contains(GetFinalPosition())))
	{
		HandlePhysicsSpaceExit();
	}
}

void RigidBodyController::ApplyVelocityCorrection(const Jacobian& jacobian, float impulse)
{
	Vector3D linear = jacobian.linear * (impulse * inverseBodyScalarInertia);
	maxLinearCorrection = Fmax(maxLinearCorrection, SquaredMag(linear));
	linearCorrection += linear;
	linearVelocity = initialLinearVelocity + linearCorrection;

	Vector3D angular = inverseWorldInertiaTensor * (jacobian.angular * impulse);
	maxAngularCorrection = Fmax(maxAngularCorrection, SquaredMag(angular));
	angularCorrection += angular;
	angularVelocity = initialAngularVelocity + angularCorrection;
}

void RigidBodyController::ApplyLinearVelocityCorrection(const Vector3D& jacobian, float impulse)
{
	Vector3D linear = jacobian * (impulse * inverseBodyScalarInertia);
	maxLinearCorrection = Fmax(maxLinearCorrection, SquaredMag(linear));
	linearCorrection += linear;
	linearVelocity = initialLinearVelocity + linearCorrection;
}

void RigidBodyController::ApplyAngularVelocityCorrection(const Vector3D& jacobian, float impulse)
{
	Vector3D angular = inverseWorldInertiaTensor * (jacobian * impulse);
	maxAngularCorrection = Fmax(maxAngularCorrection, SquaredMag(angular));
	angularCorrection += angular;
	angularVelocity = initialAngularVelocity + angularCorrection;
}

void RigidBodyController::ApplyPositionCorrection(const Jacobian& jacobian, float impulse)
{
	Vector3D linear = jacobian.linear * (impulse * inverseBodyScalarInertia);
	maxLinearCorrection = Fmax(maxLinearCorrection, SquaredMag(linear));
	transientLinearVelocity += linear;

	Vector3D angular = inverseWorldInertiaTensor * (jacobian.angular * impulse);
	maxAngularCorrection = Fmax(maxAngularCorrection, SquaredMag(angular));
	transientAngularVelocity += angular;
}

void RigidBodyController::ApplyLinearPositionCorrection(const Vector3D& jacobian, float impulse)
{
	Vector3D linear = jacobian * (impulse * inverseBodyScalarInertia);
	maxLinearCorrection = Fmax(maxLinearCorrection, SquaredMag(linear));
	transientLinearVelocity += linear;
}

void RigidBodyController::ApplyAngularPositionCorrection(const Vector3D& jacobian, float impulse)
{
	Vector3D angular = inverseWorldInertiaTensor * (jacobian * impulse);
	maxAngularCorrection = Fmax(maxAngularCorrection, SquaredMag(angular));
	transientAngularVelocity += angular;
}

void RigidBodyController::SetRigidBodyTransform(const Transform4D& transform)
{
	Node *node = GetTargetNode();
	node->SetNodeTransform(transform);
	node->StopMotion();

	initialTransform = transform;
	finalTransform = transform;

	finalCenterOfMass = transform * bodyCenterOfMass;

	motionDisplacement.Set(0.0F, 0.0F, 0.0F);
	motionRotationAxis.Set(1.0F, 0.0F, 0.0F);
	motionRotationAngle = 0.0F;
}

void RigidBodyController::SetRigidBodyMatrix3D(const Matrix3D& matrix)
{
	Node *node = GetTargetNode();
	node->SetNodeMatrix3D(matrix);

	initialTransform.SetMatrix3D(matrix);
	finalTransform.SetMatrix3D(matrix);
}

void RigidBodyController::SetRigidBodyPosition(const Point3D& position)
{
	Node *node = GetTargetNode();
	node->SetNodePosition(position);
	node->StopMotion();

	initialTransform.SetTranslation(position);
	finalTransform.SetTranslation(position);

	finalCenterOfMass = finalTransform * bodyCenterOfMass;

	motionDisplacement.Set(0.0F, 0.0F, 0.0F);
	motionRotationAxis.Set(1.0F, 0.0F, 0.0F);
	motionRotationAngle = 0.0F;
}

void RigidBodyController::PurgeContacts(void)
{
	PurgeOutgoingEdges();
	PurgeIncomingEdges();

	linearVelocity += movementVelocity;
}

void RigidBodyController::ApplyImpulse(const Vector3D& impulse)
{
	impulseForce += impulse * kInverseTimeStep;

	RigidBodyController::RecursiveWake();
}

void RigidBodyController::ApplyImpulse(const Vector3D& impulse, const Point3D& position)
{
	impulseForce += impulse * kInverseTimeStep;
	impulseTorque += ((position - bodyCenterOfMass) ^ impulse) * kInverseTimeStep;

	RigidBodyController::RecursiveWake();
}

bool RigidBodyController::DetectSegmentIntersection(const Point3D& p1, const Point3D& p2, float radius, BodyHitData *bodyHitData) const
{
	bool intersection = false;
	float tmax = 1.0F;

	const Shape *shape = shapeList.First();
	while (shape)
	{
		ShapeHitData	shapeHitData;

		const Transform4D& inverseShapeTransform = shape->GetInverseWorldTransform();
		if (shape->GetObject()->DetectSegmentIntersection(inverseShapeTransform * p1, inverseShapeTransform * p2, radius, &shapeHitData))
		{
			float t = shapeHitData.param;
			if ((!intersection) || (t < tmax))
			{
				intersection = true;
				tmax = t;

				bodyHitData->param = t;
				bodyHitData->position = shape->GetWorldTransform() * shapeHitData.position;
				bodyHitData->normal = shapeHitData.normal * inverseShapeTransform;
				bodyHitData->shape = shape;
			}
		}

		shape = shape->Next();
	}

	return (intersection);
}

float RigidBodyController::CalculateSubmergedVolume(const Antivector4D& plane, Point3D *submergedCentroid) const
{
	Point3D		centroid;

	float submergedVolume = 0.0F;
	submergedCentroid->Set(0.0F, 0.0F, 0.0F);

	const Shape *shape = shapeList.First();
	while (shape)
	{
		const ShapeObject *object = shape->GetObject();
		const Transform4D& shapeTransform = shape->GetNodeTransform();

		float volume = object->CalculateSubmergedVolume(plane * shapeTransform, &centroid);
		if (volume > 0.0F)
		{
			submergedVolume += volume;
			*submergedCentroid += shapeTransform * centroid * volume;
		}

		shape = shape->Next();
	}

	shape = internalShapeList.First();
	while (shape)
	{
		const ShapeObject *object = shape->GetObject();
		const Transform4D& shapeTransform = shape->GetNodeTransform();

		float volume = object->CalculateSubmergedVolume(plane * shapeTransform, &centroid);
		if (volume > 0.0F)
		{
			volume *= NonzeroFsgn(object->GetShapeDensity());

			submergedVolume += volume;
			*submergedCentroid += shapeTransform * centroid * volume;
		}

		shape = shape->Next();
	}

	*submergedCentroid /= submergedVolume;
	return (submergedVolume);
}

bool RigidBodyController::ValidRigidBodyCollision(const RigidBodyController *body) const
{
	return ((collisionKind & body->GetCollisionExclusionMask()) == 0);
}

bool RigidBodyController::ValidGeometryCollision(const Geometry *geometry) const
{
	return ((collisionKind & geometry->GetObject()->GetCollisionExclusionMask()) == 0);
}

RigidBodyStatus RigidBodyController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	return (GetTargetNode()->GetWorld()->HandleNewRigidBodyContact(this, contact, contactBody));
}

RigidBodyStatus RigidBodyController::HandleNewGeometryContact(const GeometryContact *contact)
{
	return (GetTargetNode()->GetWorld()->HandleNewGeometryContact(this, contact));
}

void RigidBodyController::HandlePhysicsSpaceExit(void)
{
	GetTargetNode()->GetWorld()->HandlePhysicsSpaceExit(this);
}

void RigidBodyController::HandleWaterSubmergence(void)
{
	GetTargetNode()->GetWorld()->HandleWaterSubmergence(this);
}


RigidBodySnapshotMessage::RigidBodySnapshotMessage(int32 controllerIndex) : ControllerMessage(RigidBodyController::kRigidBodyMessageSnapshot, controllerIndex)
{
}

RigidBodySnapshotMessage::RigidBodySnapshotMessage(int32 controllerIndex, const Point3D& position, const Quaternion& rotation, const Vector3D& linearVelocity, const Antivector3D& angularVelocity) : ControllerMessage(RigidBodyController::kRigidBodyMessageSnapshot, controllerIndex)
{
	rigidBodyPosition = position;
	rigidBodyRotation = rotation;
	rigidBodyLinearVelocity = linearVelocity;
	rigidBodyAngularVelocity = angularVelocity;
}

RigidBodySnapshotMessage::~RigidBodySnapshotMessage()
{
}

void RigidBodySnapshotMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << rigidBodyPosition;
	data << rigidBodyRotation;
	data << rigidBodyLinearVelocity;
	data << rigidBodyAngularVelocity;
}

bool RigidBodySnapshotMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> rigidBodyPosition;
		data >> rigidBodyRotation;
		data >> rigidBodyLinearVelocity;
		data >> rigidBodyAngularVelocity;
		return (true);
	}

	return (false);
}


RigidBodySleepMessage::RigidBodySleepMessage(int32 controllerIndex) : ControllerMessage(RigidBodyController::kRigidBodyMessageSleep, controllerIndex)
{
}

RigidBodySleepMessage::RigidBodySleepMessage(int32 controllerIndex, const Point3D& position, const Quaternion& rotation) : ControllerMessage(RigidBodyController::kRigidBodyMessageSleep, controllerIndex)
{
	rigidBodyPosition = position;
	rigidBodyRotation = rotation;
}

RigidBodySleepMessage::~RigidBodySleepMessage()
{
}

void RigidBodySleepMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << rigidBodyPosition;
	data << rigidBodyRotation;
}

bool RigidBodySleepMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> rigidBodyPosition;
		data >> rigidBodyRotation;
		return (true);
	}

	return (false);
}


RagdollController::RagdollController() : RigidBodyController(kControllerRagdoll)
{
	targetModel = nullptr;
}

RagdollController::RagdollController(const RagdollController& ragdollController) : RigidBodyController(ragdollController)
{
	targetModel = nullptr;
}

RagdollController::~RagdollController()
{
}

Controller *RagdollController::Replicate(void) const
{
	return (new RagdollController(*this));
}

void RagdollController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RigidBodyController::Pack(data, packFlags);

	if (targetModel)
	{
		data << ChunkHeader('MODL', 4);
		data << targetModel->GetNodeIndex();

		for (const Association& association : associationArray)
		{
			const Node *node = association.ragdollNode;
			if (node)
			{
				data << ChunkHeader('NODE', 4);
				data << node->GetNodeIndex();
			}
		}
	}

	data << TerminatorChunk;
}

void RagdollController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RigidBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<RagdollController>(data, unpackFlags);
}

bool RagdollController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'MODL':
		{
			int32	index;

			data >> index;
			data.AddNodeLink(index, &ModelLinkProc, this);
			return (true);
		}

		case 'NODE':
		{
			int32	index;

			data >> index;
			data.AddNodeLink(index, &AssociationLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void RagdollController::ModelLinkProc(Node *node, void *cookie)
{
	RagdollController *ragdoll = static_cast<RagdollController *>(cookie);
	ragdoll->targetModel = static_cast<Model *>(node);
}

void RagdollController::AssociationLinkProc(Node *node, void *cookie)
{
	RagdollController *ragdoll = static_cast<RagdollController *>(cookie);
	ragdoll->associationArray.AddElement(Association(node));
}

void RagdollController::ReassociateNodes(void)
{
	Node *modelNode = targetModel->GetFirstSubnode();
	while (modelNode)
	{
		unsigned_int32 hash = modelNode->GetNodeHash();
		if (hash != 0)
		{
			for (Association& association : associationArray)
			{
				const Node *node = association.ragdollNode;
				if ((node) && (node->GetNodeHash() == hash))
				{
					association.modelNode = modelNode;
					break;
				}
			}
		}

		modelNode = targetModel->GetNextNode(modelNode);
	}
}

void RagdollController::AttachModel(const ImmutableArray<Node *>& nodeArray, Model *model)
{
	targetModel = model;

	Node *modelNode = model->GetFirstSubnode();
	while (modelNode)
	{
		unsigned_int32 hash = modelNode->GetNodeHash();
		if (hash != 0)
		{
			for (Node *ragdollNode : nodeArray)
			{
				if (ragdollNode->GetNodeHash() == hash)
				{
					ragdollNode->SetNodeTransform(modelNode->GetWorldTransform());
					associationArray.AddElement(Association(ragdollNode, modelNode));
					break;
				}
			}
		}

		modelNode = model->GetNextNode(modelNode);
	}
}

void RagdollController::UpdateModel(void)
{
	for (const Association& association : associationArray)
	{
		Node *modelNode = association.modelNode;
		if (modelNode)
		{
			const Node *ragdollNode = association.ragdollNode;
			if (ragdollNode)
			{
				const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(ragdollNode->GetController());
				const Transform4D& worldTransform = rigidBody->GetFinalTransform();

				const Node *super = modelNode->GetSuperNode();
				if (super->GetNodeType() != kNodeModel)
				{
					Transform4D transform = super->GetNodeTransform();
					for (;;)
					{
						super = super->GetSuperNode();
						transform = super->GetNodeTransform() * transform;

						if (super->GetNodeType() == kNodeModel)
						{
							break;
						}
					}

					modelNode->SetNodeTransform(Inverse(transform) * worldTransform);
				}
				else
				{
					modelNode->SetNodeTransform(Inverse(super->GetNodeTransform()) * worldTransform);
				}
			}
		}
	}

	targetModel->Invalidate();
}

void RagdollController::HandlePhysicsSpaceExit(void)
{
	delete targetModel;
}

void RagdollController::Delete(void)
{
	const Node *targetNode = GetTargetNode();

	for (const Association& association : associationArray)
	{
		const Node *ragdollNode = association.ragdollNode;
		if (ragdollNode != targetNode)
		{
			delete ragdollNode;
		}
	}

	delete targetNode;
}


ShapeCollisionJob::ShapeCollisionJob(ExecuteProc *execProc, PhysicsController *cookie, RigidBodyController *body1, RigidBodyController *body2, const Shape *shape1, const Shape *shape2, unsigned_int32 index1, unsigned_int32 index2) : BatchJob(execProc, cookie, kJobNonpersistent)
{
	alphaBody = body1;
	betaBody = body2;
	alphaShape = shape1;
	betaShape = shape2;
	alphaIndex = index1;
	betaIndex = index2;
}


GeometryCollisionJob::GeometryCollisionJob(ExecuteProc *execProc, RigidBodyController *body, const Shape *shape, unsigned_int32 index, Geometry *geometry) : BatchJob(execProc, nullptr, kJobNonpersistent)
{
	rigidBody = body;
	shapeNode = shape;
	shapeIndex = index;
	geometryNode = geometry;
}


ConstraintSolverJob::ConstraintSolverJob(ExecuteProc *execProc) : BatchJob(execProc)
{
	solverMultiplier = 1;
	repeatCollisionFlag = false;
}


PhysicsController::PhysicsController() : Controller(kControllerPhysics)
{
	physicsGraph.AddElement(&nullBody);

	simulationStep = 0;
	simulationTime = kPhysicsTimeStep;
	interpolationParam = 1.0F;

	maxLinearSpeed = kDefaultMaxLinearSpeed;
	maxAngularSpeed = kDefaultMaxAngularSpeed;

	gravityAcceleration.Set(0.0F, 0.0F, -9.8F);

	rigidBodyParity = 0;
	sleepingParity = 0;

	geometryCollisionStamp = 0xFFFFFFFF;
	fieldApplicationStamp = 0xFFFFFFFF;

	simulationList = nullptr;

	for (machine a = 0; a < kPhysicsCounterCount; a++)
	{
		physicsCounter[a] = 0;
	}
}

PhysicsController::~PhysicsController()
{
}

bool PhysicsController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodePhysics);
}

void PhysicsController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<SetGravityFunction> setGravityRegistration(registration, kFunctionSetGravity, table->GetString(StringID('CTRL', kControllerPhysics, kFunctionSetGravity)), kFunctionRemote | kFunctionJournaled);
}

void PhysicsController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('STEP', 4);
	data << simulationStep;

	data << ChunkHeader('GRAV', sizeof(Vector3D));
	data << gravityAcceleration;

	data << ChunkHeader('MAXS', 8);
	data << maxLinearSpeed;
	data << maxAngularSpeed;

	if (!(packFlags & kPackSettings))
	{
		const Body *body = physicsGraph.GetFirstElement();
		while (body)
		{
			if ((body->GetBodyType() != kBodyRigid) || (!(static_cast<const RigidBodyController *>(body)->GetTargetNode()->GetNodeFlags() & kNodeNonpersistent)))
			{
				const Contact *contact = body->GetFirstOutgoingEdge();
				while (contact)
				{
					if (!contact->NonpersistentFinishNode())
					{
						PackHandle handle = data.BeginChunk('CTAC');
						contact->PackType(data);
						contact->Pack(data, packFlags);
						data.EndChunk(handle);
					}

					contact = contact->GetNextOutgoingEdge();
				}
			}

			body = body->GetNextElement();
		}
	}

	data << TerminatorChunk;
}

void PhysicsController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<PhysicsController>(data, unpackFlags);
}

bool PhysicsController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STEP':

			data >> simulationStep;
			return (true);

		case 'GRAV':

			data >> gravityAcceleration;
			return (true);

		case 'MAXS':

			data >> maxLinearSpeed;
			data >> maxAngularSpeed;
			return (true);

		case 'CTAC':
		{
			Contact *contact = Contact::Create(data, unpackFlags, &nullBody);
			if (contact)
			{
				contact->Unpack(++data, unpackFlags);
				return (true);
			}

			break;
		}
	}

	return (false);
}

int32 PhysicsController::GetSettingCount(void) const
{
	return (1);
}

Setting *PhysicsController::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('CTRL', kControllerPhysics, 'GRAV'));
		return (new TextSetting('GRAV', -gravityAcceleration.z, title));
	}

	return (nullptr);
}

void PhysicsController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'GRAV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		gravityAcceleration.z = -Text::StringToFloat(text);
	}
}

void PhysicsController::Preprocess(void)
{
	SetControllerFlags(kControllerPhysicsSimulation);
	Controller::Preprocess();
}

void PhysicsController::AddRigidBody(RigidBodyController *rigidBody)
{
	physicsGraph.AddElement(rigidBody);

	if ((rigidBody->RigidBodyAsleep()) && (rigidBody->GetTargetNode()->Enabled()))
	{
		sleepingList[sleepingParity].Append(rigidBody);
	}
}

void PhysicsController::RemoveRigidBody(RigidBodyController *rigidBody)
{
	rigidBody->ListElement<RigidBodyController>::Detach();
	physicsGraph.RemoveElement(rigidBody);
}

void PhysicsController::WakeRigidBody(RigidBodyController *rigidBody)
{
	const List<RigidBodyController> *list = rigidBody->ListElement<RigidBodyController>::GetOwningList();
	if ((!list) || (list - sleepingList < 2))
	{
		if (simulationList)
		{
			simulationList->Append(rigidBody);
		}
		else
		{
			rigidBodyList[rigidBodyParity].Append(rigidBody);
		}
	}

	rigidBody->rigidBodyState &= ~kRigidBodyAsleep;
}

void PhysicsController::SleepRigidBody(RigidBodyController *rigidBody)
{
	if (rigidBody->GetTargetNode()->Enabled())
	{
		sleepingList[sleepingParity].Append(rigidBody);
	}
	else
	{
		rigidBody->ListElement<RigidBodyController>::Detach();
	}

	rigidBody->rigidBodyState |= kRigidBodyAsleep;
}

void PhysicsController::WakeFieldRigidBodies(const Field *field)
{
	unsigned_int32 parity = sleepingParity;
	List<RigidBodyController> *oldSleepingList = &sleepingList[parity];

	parity ^= 1;
	List<RigidBodyController> *newSleepingList = &sleepingList[parity];

	const Transform4D& fieldInverseTransform = field->GetInverseWorldTransform();
	const Box3D& fieldBoundingBox = field->GetWorldBoundingBox();
	const FieldObject *fieldObject = field->GetObject();

	for (;;)
	{
		RigidBodyController *rigidBody = oldSleepingList->First();
		if (!rigidBody)
		{
			break;
		}

		// It's possible that the rigid body won't wake up (if it has no shapes),
		// so move it to the new sleeping list first.

		newSleepingList->Append(rigidBody);

		Box3D rigidBodyBoundingBox = Transform(rigidBody->boundingBox, rigidBody->finalTransform);
		if ((fieldBoundingBox.Intersection(rigidBodyBoundingBox)) && (!fieldObject->ExteriorSphere(fieldInverseTransform * rigidBody->finalCenterOfMass, rigidBody->boundingRadius)))
		{
			rigidBody->RigidBodyController::RecursiveWake();
		}
	}

	// The new sleeping parity needs to be stored after the loop is finished
	// because WakeRigidBody() reads it.

	sleepingParity = parity;
}

float PhysicsController::SortRigidBodyList(List<RigidBodyController> *inputList, int32 depth, float minValue, float maxValue, int32 index, List<RigidBodyController> *outputList)
{
	RigidBodyController *rigidBody = inputList->First();
	if (rigidBody == inputList->Last())
	{
		if (rigidBody)
		{
			outputList->Append(rigidBody);
		}

		return (0.0F);
	}

	float distance = maxValue - minValue;
	if ((distance < kCollisionSweepEpsilon) || (--depth == 0))
	{
		do
		{
			RigidBodyController *next = rigidBody->Next();
			outputList->Append(rigidBody);
			rigidBody = next;
		} while (rigidBody);

		return (distance);
	}

	List<RigidBodyController>	lowerList;

	float maxLower = minValue;
	float minHigher = maxValue;
	float center = (minValue + maxValue) * 0.5F;

	do
	{
		RigidBodyController *next = rigidBody->Next();
		float v = rigidBody->bodyCollisionBox.min[index];
		if (v < center)
		{
			maxLower = Fmax(maxLower, v);
			lowerList.Append(rigidBody);
		}
		else
		{
			minHigher = Fmin(minHigher, v);
		}

		rigidBody = next;
	} while (rigidBody);

	distance = SortRigidBodyList(&lowerList, depth, minValue, maxLower, index, outputList);
	distance = Fmax(SortRigidBodyList(inputList, depth, minHigher, maxValue, index, outputList), distance);
	return (distance);
}

void PhysicsController::CollideRigidBodiesX(List<RigidBodyController> *inputList, int32 depth, float xmin, float xmax, List<RigidBodyController> *outputList)
{
	List<RigidBodyController>	sortedList;

	float overlap = SortRigidBodyList(inputList, depth, xmin, xmax, 0, &sortedList);

	RigidBodyController *lowerBody = sortedList.First();
	if (lowerBody)
	{
		for (;;)
		{
			RigidBodyController *higherBody = lowerBody->Next();
			if (!higherBody)
			{
				outputList->Append(lowerBody);
				break;
			}

			xmax = lowerBody->bodyCollisionBox.max.x;
			if (higherBody->bodyCollisionBox.min.x - overlap < xmax)
			{
				List<RigidBodyController>	clusterList;

				float y = lowerBody->bodyCollisionBox.min.y;
				float ymin = y;
				float ymax = y;

				clusterList.Append(lowerBody);
				do
				{
					lowerBody = higherBody;
					higherBody = higherBody->Next();
					clusterList.Append(lowerBody);

					y = lowerBody->bodyCollisionBox.min.y;
					ymin = Fmin(ymin, y);
					ymax = Fmax(ymax, y);

					xmax = Fmax(xmax, lowerBody->bodyCollisionBox.max.x);
				} while ((higherBody) && (higherBody->bodyCollisionBox.min.x - overlap < xmax));

				CollideRigidBodiesY(&clusterList, depth, ymin, ymax, outputList);

				if (!higherBody)
				{
					break;
				}
			}
			else
			{
				outputList->Append(lowerBody);
			}

			lowerBody = higherBody;
		}
	}
}

void PhysicsController::CollideRigidBodiesY(List<RigidBodyController> *inputList, int32 depth, float ymin, float ymax, List<RigidBodyController> *outputList)
{
	List<RigidBodyController>	sortedList;

	float overlap = SortRigidBodyList(inputList, depth, ymin, ymax, 1, &sortedList);

	RigidBodyController *lowerBody = sortedList.First();
	for (;;)
	{
		RigidBodyController *higherBody = lowerBody->Next();
		if (!higherBody)
		{
			outputList->Append(lowerBody);
			break;
		}

		ymax = lowerBody->bodyCollisionBox.max.y;
		if (higherBody->bodyCollisionBox.min.y - overlap < ymax)
		{
			List<RigidBodyController>	clusterList;

			float z = lowerBody->bodyCollisionBox.min.z;
			float zmin = z;
			float zmax = z;

			clusterList.Append(lowerBody);
			do
			{
				lowerBody = higherBody;
				higherBody = higherBody->Next();
				clusterList.Append(lowerBody);

				z = lowerBody->bodyCollisionBox.min.z;
				zmin = Fmin(zmin, z);
				zmax = Fmax(zmax, z);

				ymax = Fmax(ymax, lowerBody->bodyCollisionBox.max.y);
			} while ((higherBody) && (higherBody->bodyCollisionBox.min.y - overlap < ymax));

			CollideRigidBodiesZ(&clusterList, depth, zmin, zmax, outputList);

			if (!higherBody)
			{
				break;
			}
		}
		else
		{
			outputList->Append(lowerBody);
		}

		lowerBody = higherBody;
	}
}

void PhysicsController::CollideRigidBodiesZ(List<RigidBodyController> *inputList, int32 depth, float zmin, float zmax, List<RigidBodyController> *outputList)
{
	List<RigidBodyController>	sortedList;

	float overlap = SortRigidBodyList(inputList, depth, zmin, zmax, 2, &sortedList);

	RigidBodyController *lowerBody = sortedList.First();
	for (;;)
	{
		RigidBodyController *higherBody = lowerBody->Next();
		if (!higherBody)
		{
			outputList->Append(lowerBody);
			break;
		}

		zmax = lowerBody->bodyCollisionBox.max.z;
		if (higherBody->bodyCollisionBox.min.z - overlap < zmax)
		{
			List<RigidBodyController>	clusterList;

			int32 bodyCount = 1;
			int32 sleepingCount = lowerBody->RigidBodyAsleep();
			clusterList.Append(lowerBody);

			do
			{
				lowerBody = higherBody;
				higherBody = higherBody->Next();

				bodyCount++;
				sleepingCount += lowerBody->RigidBodyAsleep();
				clusterList.Append(lowerBody);

				zmax = Fmax(zmax, lowerBody->bodyCollisionBox.max.z);
			} while ((higherBody) && (higherBody->bodyCollisionBox.min.z - overlap < zmax));

			if (sleepingCount < bodyCount)
			{
				for (;;)
				{
					RigidBodyController *alphaBody = clusterList.First();
					outputList->Append(alphaBody);

					RigidBodyController *betaBody = clusterList.First();
					if (!betaBody)
					{
						break;
					}

					do
					{
						if (alphaBody->bodyCollisionBox.Intersection(betaBody->bodyCollisionBox))
						{
							if ((!alphaBody->RigidBodyAsleep()) || (!betaBody->RigidBodyAsleep()))
							{
								if ((alphaBody->ValidRigidBodyCollision(betaBody)) && (betaBody->ValidRigidBodyCollision(alphaBody)))
								{
									DetectBodyCollision(alphaBody, betaBody);
								}
							}
						}

						betaBody = betaBody->Next();
					} while ((betaBody) && (betaBody->bodyCollisionBox.min.z - overlap < alphaBody->bodyCollisionBox.max.z));
				}
			}
			else
			{
				RigidBodyController *rigidBody = clusterList.First();
				do
				{
					RigidBodyController *next = rigidBody->Next();
					outputList->Append(rigidBody);
					rigidBody = next;
				} while (rigidBody);
			}

			if (!higherBody)
			{
				break;
			}
		}
		else
		{
			outputList->Append(lowerBody);
		}

		lowerBody = higherBody;
	}
}

void PhysicsController::DetectBodyCollision(RigidBodyController *alphaBody, RigidBodyController *betaBody)
{
	unsigned_int32 alphaIndex = 0;
	const Shape *alphaShape = alphaBody->shapeList.First();
	while (alphaShape)
	{
		const Box3D& alphaBox = alphaShape->GetCollisionBox();

		unsigned_int32 betaIndex = 0;
		const Shape *betaShape = betaBody->shapeList.First();
		while (betaShape)
		{
			const Box3D& betaBox = betaShape->GetCollisionBox();
			if (alphaBox.Intersection(betaBox))
			{
				physicsCounter[kPhysicsCounterShapeIntersection]++;

				ShapeCollisionJob *job = new ShapeCollisionJob(&JobDetectShapeCollision, this, alphaBody, betaBody, alphaShape, betaShape, alphaIndex, betaIndex);
				TheJobMgr->SubmitJob(job, &collisionBatch);
			}

			betaIndex++;
			betaShape = betaShape->Next();
		}

		alphaIndex++;
		alphaShape = alphaShape->Next();
	}
}

void PhysicsController::JobDetectShapeCollision(Job *job, void *cookie)
{
	ShapeCollisionJob *shapeJob = static_cast<ShapeCollisionJob *>(job);

	RigidBodyContact *alphaContact = shapeJob->alphaBody->FindOutgoingBodyContact(shapeJob->betaBody, shapeJob->alphaIndex, shapeJob->betaIndex);
	if (alphaContact)
	{
		if (shapeJob->alphaShape->StaticIntersectShape(shapeJob->betaShape, alphaContact, &shapeJob->intersectionData))
		{
			shapeJob->rigidBodyContact = alphaContact;
			shapeJob->SetFinalizeProc(&FinalizeExistingShapeContact);
		}
	}
	else
	{
		RigidBodyContact *betaContact = shapeJob->alphaBody->FindIncomingBodyContact(shapeJob->betaBody, shapeJob->betaIndex, shapeJob->alphaIndex);
		if (betaContact)
		{
			if (shapeJob->betaShape->StaticIntersectShape(shapeJob->alphaShape, betaContact, &shapeJob->intersectionData))
			{
				shapeJob->rigidBodyContact = betaContact;
				shapeJob->SetFinalizeProc(&FinalizeExistingShapeContact);
			}
		}
		else
		{
			if (shapeJob->alphaShape->DynamicIntersectShape(shapeJob->betaShape, shapeJob->alphaBody->motionDisplacement, shapeJob->betaBody->motionDisplacement, &shapeJob->intersectionData))
			{
				shapeJob->SetFinalizeProc(&FinalizeNewShapeContact);
			}
		}
	}
}

void PhysicsController::FinalizeExistingShapeContact(Job *job, void *cookie)
{
	const ShapeCollisionJob *shapeJob = static_cast<ShapeCollisionJob *>(job);
	shapeJob->rigidBodyContact->UpdateContact(&shapeJob->intersectionData);
}

void PhysicsController::FinalizeNewShapeContact(Job *job, void *cookie)
{
	const ShapeCollisionJob *shapeJob = static_cast<ShapeCollisionJob *>(job);
	RigidBodyController *alphaBody = shapeJob->alphaBody;
	RigidBodyController *betaBody = shapeJob->betaBody;

	float param = shapeJob->intersectionData.contactParam;
	if (param < 1.0F)
	{
		alphaBody->AdjustDisplacement(param);
		betaBody->AdjustDisplacement(param);

		betaBody->KillLaterContacts(param);
		alphaBody->KillLaterContacts(param);
	}

	alphaBody->RigidBodyController::RecursiveWake();
	betaBody->RigidBodyController::RecursiveWake();

	new RigidBodyContact(alphaBody, betaBody, shapeJob->alphaShape, shapeJob->betaShape, shapeJob->alphaIndex, shapeJob->betaIndex, &shapeJob->intersectionData);

	PhysicsController *physicsController = static_cast<PhysicsController *>(cookie);
	ShapeCollisionJob *repeatJob = new ShapeCollisionJob(&PhysicsController::JobDetectShapeCollision, physicsController, alphaBody, betaBody, shapeJob->alphaShape, shapeJob->betaShape, shapeJob->alphaIndex, shapeJob->betaIndex);
	physicsController->repeatCollisionJobArray.AddElement(repeatJob);
	alphaBody->repeatCollisionFlag = true;
	betaBody->repeatCollisionFlag = true;
}

void PhysicsController::PurgeGeometryContacts(const Geometry *geometry)
{
	Contact *contact = nullBody.GetFirstIncomingEdge();
	while (contact)
	{
		Contact *next = contact->GetNextIncomingEdge();

		if (contact->GetContactType() == kContactGeometry)
		{
			GeometryContact *geometryContact = static_cast<GeometryContact *>(contact);
			if (geometryContact->GetContactGeometry() == geometry)
			{
				delete geometryContact;
			}
		}

		contact = next;
	}
}

void PhysicsController::AddBrokenJoint(Joint *joint)
{
	brokenJointMutex.Acquire();
	brokenJointArray.AddElement(joint);
	brokenJointMutex.Release();
}

void PhysicsController::BuildConstraintIsland(RigidBodyController *rigidBody, ConstraintSolverJob *solverJob)
{
	List<RigidBodyController> *list = &solverJob->rigidBodyList;
	list->Append(rigidBody);

	solverJob->repeatCollisionFlag |= rigidBody->repeatCollisionFlag;
	rigidBody->repeatCollisionFlag = false;

	const Contact *contact = rigidBody->GetFirstOutgoingEdge();
	while (contact)
	{
		solverJob->solverMultiplier = Max(solverJob->solverMultiplier, contact->GetConstraintSolverMultiplier());

		Body *body = contact->GetFinishElement();
		if (body->GetBodyType() == kBodyRigid)
		{
			RigidBodyController *rb = static_cast<RigidBodyController *>(body);
			if (rb->GetOwningList() != list)
			{
				BuildConstraintIsland(rb, solverJob);
			}
		}

		contact = contact->GetNextOutgoingEdge();
	}

	contact = rigidBody->GetFirstIncomingEdge();
	while (contact)
	{
		solverJob->solverMultiplier = Max(solverJob->solverMultiplier, contact->GetConstraintSolverMultiplier());

		Body *body = contact->GetStartElement();
		if (body->GetBodyType() == kBodyRigid)
		{
			RigidBodyController *rb = static_cast<RigidBodyController *>(body);
			if (rb->GetOwningList() != list)
			{
				BuildConstraintIsland(rb, solverJob);
			}
		}

		contact = contact->GetNextIncomingEdge();
	}
}

void PhysicsController::JobSolveIslandConstraints(Job *job, void *cookie)
{
	ConstraintSolverJob *solverJob = static_cast<ConstraintSolverJob *>(job);

	machine iterationCount = solverJob->solverMultiplier * kMaxConstraintIterationCount;
	for (machine iteration = 0; iteration < iterationCount; iteration++)
	{
		RigidBodyController *rigidBody = solverJob->rigidBodyList.First();
		while (rigidBody)
		{
			rigidBody->maxLinearCorrection = 0.0F;
			rigidBody->maxAngularCorrection = 0.0F;
			rigidBody = rigidBody->Next();
		} while (rigidBody);

		rigidBody = solverJob->rigidBodyList.First();
		while (rigidBody)
		{
			Contact *contact = rigidBody->GetFirstOutgoingEdge();
			while (contact)
			{
				if (contact->Enabled())
				{
					contact->ApplyVelocityConstraints();
				}

				contact = contact->GetNextOutgoingEdge();
			}

			rigidBody = rigidBody->Next();
		}

		rigidBody = solverJob->rigidBodyList.First();
		while (rigidBody)
		{
			Contact *contact = rigidBody->GetFirstOutgoingEdge();
			while (contact)
			{
				if (contact->Enabled())
				{
					contact->ApplyFrictionConstraints();
				}

				contact = contact->GetNextOutgoingEdge();
			}

			rigidBody = rigidBody->Next();
		}

		rigidBody = solverJob->rigidBodyList.First();
		while (rigidBody)
		{
			Contact *contact = rigidBody->GetFirstOutgoingEdge();
			while (contact)
			{
				if (contact->Enabled())
				{
					contact->ApplyPositionConstraints();
				}

				contact = contact->GetNextOutgoingEdge();
			}

			rigidBody = rigidBody->Next();
		}

		float maxCorrection = 0.0F;

		rigidBody = solverJob->rigidBodyList.First();
		while (rigidBody)
		{
			maxCorrection = Fmax(maxCorrection, rigidBody->maxLinearCorrection, rigidBody->maxAngularCorrection);
			rigidBody = rigidBody->Next();
		}

		if (maxCorrection < kConstraintCorrectionThreshold)
		{
			break;
		}
	}
}

void PhysicsController::Move(void)
{
	int32 time = simulationTime + TheTimeMgr->GetDeltaTime();
	int32 stepCount = time / kPhysicsTimeStep;
	time -= stepCount * kPhysicsTimeStep;

	simulationTime = time;
	interpolationParam = (float) time * kInversePhysicsTimeStep;

	stepCount = Min(stepCount, kMaxPhysicsStepCount);
	for (machine a = 0; a < stepCount; a++)
	{
		List<RigidBodyController>	bodyList[2];

		int32 parity = rigidBodyParity;
		rigidBodyParity = parity ^ 1;

		List<RigidBodyController> *currentList = &rigidBodyList[parity];

		for (machine b = kPhysicsCounterRigidBody + 1; b < kPhysicsCounterCount; b++)
		{
			physicsCounter[b] = 0;
		}

		int32 bodyCount = 0;
		float xmin = K::infinity;
		float xmax = K::minus_infinity;

		simulationList = &bodyList[0];
		for (;;)
		{
			RigidBodyController *rigidBody = currentList->First();
			if (!rigidBody)
			{
				break;
			}

			rigidBody->CalculateAppliedForces(gravityAcceleration);
			rigidBody->DetectWorldCollisions(++geometryCollisionStamp);

			float x = rigidBody->bodyCollisionBox.min.x;
			xmin = Fmin(xmin, x);
			xmax = Fmax(xmax, x);

			bodyCount++;
			bodyList[0].Append(rigidBody);
		}

		if (bodyCount != 0)
		{
			List<ConstraintSolverJob>	solverList;

			List<RigidBodyController> *sleepList = &sleepingList[sleepingParity];
			for (;;)
			{
				RigidBodyController *rigidBody = sleepList->First();
				if (!rigidBody)
				{
					break;
				}

				float x = rigidBody->bodyCollisionBox.min.x;
				xmin = Fmin(xmin, x);
				xmax = Fmax(xmax, x);

				bodyCount++;
				bodyList[0].Append(rigidBody);
			}

			simulationList = &bodyList[1];

			CollideRigidBodiesX(&bodyList[0], Max(33 - Cntlz(bodyCount), 8), xmin, xmax, &bodyList[1]);
			TheJobMgr->FinishBatch(&collisionBatch);

			RigidBodyController *rigidBody = bodyList[1].First();
			while (rigidBody)
			{
				RigidBodyController *next = rigidBody->Next();

				if (!rigidBody->RigidBodyAsleep())
				{
					rigidBody->InitializeConstraints();
				}
				else
				{
					sleepList->Append(rigidBody);
				}

				rigidBody = next;
			} while (rigidBody);

			for (;;)
			{
				rigidBody = bodyList[1].First();
				if (!rigidBody)
				{
					break;
				}

				if (rigidBody->Isolated())
				{
					bodyList[0].Append(rigidBody);
				}
				else
				{
					ConstraintSolverJob *solverJob = new ConstraintSolverJob(&JobSolveIslandConstraints);
					solverList.Append(solverJob);

					BuildConstraintIsland(rigidBody, solverJob);
					TheJobMgr->SubmitJob(solverJob, &constraintSolverBatch);

					physicsCounter[kPhysicsCounterConstraintSolverIsland]++;
				}
			}

			TheJobMgr->FinishBatch(&constraintSolverBatch);

			for (;;)
			{
				ConstraintSolverJob *solverJob = solverList.First();
				while (solverJob)
				{
					ConstraintSolverJob *next = solverJob->Next();

					bool repeat = solverJob->repeatCollisionFlag;
					solverJob->repeatCollisionFlag = false;

					if (!repeat)
					{
						for (;;)
						{
							rigidBody = solverJob->rigidBodyList.First();
							if (!rigidBody)
							{
								break;
							}

							bodyList[0].Append(rigidBody);
						}

						delete solverJob;
					}

					solverJob = next;
				}

				if (repeatCollisionJobArray.Empty())
				{
					break;
				}

				for (BatchJob *job : repeatCollisionJobArray)
				{
					TheJobMgr->SubmitJob(job, &collisionBatch);
				}

				repeatCollisionJobArray.Clear();
				TheJobMgr->FinishBatch(&collisionBatch);

				solverJob = solverList.First();
				while (solverJob)
				{
					rigidBody = solverJob->rigidBodyList.First();
					while (rigidBody)
					{
						solverJob->repeatCollisionFlag |= rigidBody->repeatCollisionFlag;
						rigidBody->repeatCollisionFlag = false;

						rigidBody->InitializeRepeatConstraints();
						rigidBody = rigidBody->Next();
					} while (rigidBody);

					TheJobMgr->SubmitJob(solverJob, &constraintSolverBatch);
					solverJob = solverJob->Next();
				}

				TheJobMgr->FinishBatch(&constraintSolverBatch);
			}
		}

		simulationList = nullptr;

		for (Joint *joint : brokenJointArray)
		{
			Controller *controller = joint->GetController();
			if (controller)
			{
				TheMessageMgr->SendMessageJournal(new BreakJointMessage(controller->GetControllerIndex()));
				controller->Activate(nullptr, nullptr);
			}
		}

		brokenJointArray.Clear();

		const PhysicsNode *physicsNode = static_cast<PhysicsNode *>(GetTargetNode());
		const PhysicsSpace *physicsSpace = physicsNode->GetConnectedPhysicsSpace();
		const Box3D *physicsBoundingBox = (physicsSpace) ? &physicsNode->GetPhysicsBoundingBox() : nullptr;

		List<RigidBodyController> *nextList = &rigidBodyList[parity ^ 1];
		for (;;)
		{
			RigidBodyController *rigidBody = bodyList[0].First();
			if (!rigidBody)
			{
				break;
			}

			if (!rigidBody->RigidBodyAsleep())
			{
				nextList->Append(rigidBody);
				rigidBody->Finalize(physicsBoundingBox);
			}
			else
			{
				sleepingList[sleepingParity].Append(rigidBody);
			}
		}

		if (TheMessageMgr->Server())
		{
			RigidBodyController *rigidBody = nextList->First();
			while (rigidBody)
			{
				if ((rigidBody->sleepState[0].sleepStepCount < kRigidBodySleepStepCount) && (rigidBody->sleepState[1].sleepStepCount < kRigidBodySleepStepCount * 2))
				{
					rigidBody->RecursiveKeepAwake();
				}

				rigidBody = rigidBody->Next();
			}

			rigidBody = nextList->First();
			while (rigidBody)
			{
				RigidBodyController *next = rigidBody->Next();
				if ((rigidBody->sleepState[0].sleepStepCount >= kRigidBodySleepStepCount) || (rigidBody->sleepState[1].sleepStepCount >= kRigidBodySleepStepCount * 2))
				{
					rigidBody->Sleep();
				}

				rigidBody = next;
			}
		}

		DeformableBodyController *deformableBody = deformableBodyList.First();
		while (deformableBody)
		{
			physicsCounter[kPhysicsCounterDeformableBodyMove]++;
			TheJobMgr->SubmitJob(&deformableBody->stepSimulationJob, &deformableBatch);

			deformableBody = deformableBody->Next();
		}

		TheJobMgr->FinishBatch(&deformableBatch);

		simulationStep++;
	}

	deformableBodyList.RemoveAll();

	List<RigidBodyController> *moveList = &rigidBodyList[rigidBodyParity];

	float param = interpolationParam;
	float decay = TheTimeMgr->GetFloatDeltaTime() * 0.01F;

	int32 bodyCount = 0;
	RigidBodyController *rigidBody = moveList->First();
	while (rigidBody)
	{
		Matrix3D	rotation;

		if (rigidBody->GetControllerType() == kControllerRagdoll)
		{
			ragdollArray.AddElement(static_cast<RagdollController *>(rigidBody));
		}

		const Point3D& cm = rigidBody->initialCenterOfMass;
		rotation.SetRotationAboutAxis(rigidBody->motionRotationAngle * param, rigidBody->motionRotationAxis);
		Transform4D transform(rotation, cm - rotation * cm + rigidBody->motionDisplacement * param);

		Vector3D delta = rigidBody->networkDelta[0] * rigidBody->networkDecay[0] + rigidBody->networkDelta[1] * rigidBody->networkDecay[1];
		rigidBody->networkDecay[0] = FmaxZero(rigidBody->networkDecay[0] - decay);
		rigidBody->networkDecay[1] = FmaxZero(rigidBody->networkDecay[1] - decay);

		Node *node = rigidBody->GetTargetNode();
		node->SetNodeTransform(transform * rigidBody->initialTransform);
		node->SetNodePosition(node->GetNodePosition() - delta);
		node->Invalidate();

		bodyCount++;
		rigidBody = rigidBody->Next();
	}

	for (RagdollController *ragdoll : ragdollArray)
	{
		ragdoll->UpdateModel();
	}

	ragdollArray.Clear();

	physicsCounter[kPhysicsCounterRigidBody] = bodyCount;
}


SetGravityFunction::SetGravityFunction() : Function(kFunctionSetGravity, kControllerPhysics)
{
	gravityAcceleration = 9.8F;
}

SetGravityFunction::SetGravityFunction(const SetGravityFunction& setGravityFunction) : Function(setGravityFunction)
{
	gravityAcceleration = setGravityFunction.gravityAcceleration;
}

SetGravityFunction::~SetGravityFunction()
{
}

Function *SetGravityFunction::Replicate(void) const
{
	return (new SetGravityFunction(*this));
}

void SetGravityFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	data << gravityAcceleration;
}

void SetGravityFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);

	data >> gravityAcceleration;
}

void SetGravityFunction::Compress(Compressor& data) const
{
	Function::Compress(data);

	data << gravityAcceleration;
}

bool SetGravityFunction::Decompress(Decompressor& data)
{
	if (Function::Decompress(data))
	{
		data >> gravityAcceleration;
		return (gravityAcceleration >= 0.0F);
	}

	return (false);
}

int32 SetGravityFunction::GetSettingCount(void) const
{
	return (1);
}

Setting *SetGravityFunction::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('CTRL', kControllerPhysics, kFunctionSetGravity, 'GRAV'));
		return (new TextSetting('GRAV', gravityAcceleration, title));
	}

	return (nullptr);
}

void SetGravityFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'GRAV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		gravityAcceleration = FmaxZero(Text::StringToFloat(text));
	}
}

void SetGravityFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	static_cast<PhysicsController *>(controller)->SetGravityAcceleration(Vector3D(0.0F, 0.0F, -gravityAcceleration));

	CallCompletionProc();
}


PhysicsNode::PhysicsNode() : Node(kNodePhysics)
{
	connectedPhysicsSpace = nullptr;
}

PhysicsNode::~PhysicsNode()
{
}

int32 PhysicsNode::GetInternalConnectorCount(void) const
{
	return (1);
}

const char *PhysicsNode::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyPhysics);
	}

	return (nullptr);
}

void PhysicsNode::ProcessInternalConnectors(void)
{
	Node *node = GetConnectedNode(kConnectorKeyPhysics);
	if (node)
	{
		PhysicsSpace *physicsSpace = static_cast<PhysicsSpace *>(node);
		connectedPhysicsSpace = physicsSpace;

		physicsSpace->GetObject()->BoxVolume::CalculateBoundingBox(&physicsBoundingBox);
		physicsBoundingBox.Transform(physicsSpace->GetWorldTransform());
	}
	else
	{
		connectedPhysicsSpace = nullptr;
	}
}

bool PhysicsNode::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyPhysics)
	{
		return ((node->GetNodeType() == kNodeSpace) && (static_cast<const Space *>(node)->GetSpaceType() == kSpacePhysics));
	}

	return (Node::ValidConnectedNode(key, node));
}

void PhysicsNode::SetConnectedPhysicsSpace(PhysicsSpace *physicsSpace)
{
	connectedPhysicsSpace = physicsSpace;
	SetConnectedNode(kConnectorKeyPhysics, physicsSpace);
}

void PhysicsNode::Preprocess(void)
{
	Node::Preprocess();

	World *world = GetWorld();
	if (world)
	{
		Zone *zone = world->GetRootNode();
		if (!zone->GetPhysicsNode())
		{
			zone->SetPhysicsNode(this);
		}
	}
}


#if C4DIAGS

	RigidBodyRenderable::RigidBodyRenderable(const Box3D& box) :
			Renderable(kRenderIndexedLines, kRenderDepthInhibit),
			vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
			diffuseColor(ColorRGBA(0.25F, 1.0F, 0.25F, 1.0F))
	{
		SetVertexCount(8);
		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point3D));
		SetVertexAttributeArray(kArrayPosition, 0, 3);
		vertexBuffer.Establish(sizeof(Point3D) * 8);

		if (indexBuffer.Retain() == 1)
		{
			static const Line rigidBodyLine[12] =
			{
				{{0, 1}}, {{2, 3}}, {{0, 2}}, {{1, 3}}, {{4, 5}}, {{6, 7}}, {{4, 6}}, {{5, 7}}, {{0, 4}}, {{1, 5}}, {{2, 6}}, {{3, 7}}
			};

			indexBuffer.Establish(sizeof(Line) * 12, rigidBodyLine);
		}

		SetPrimitiveCount(12);
		SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

		attributeList.Append(&diffuseColor);
		SetMaterialAttributeList(&attributeList);

		SetCollisionBox(box);
	}

	RigidBodyRenderable::~RigidBodyRenderable()
	{
		indexBuffer.Release();
	}

	void RigidBodyRenderable::SetCollisionBox(const Box3D& box)
	{
		volatile Point3D *restrict vertex = vertexBuffer.BeginUpdate<Point3D>();

		vertex[0] = box.min;
		vertex[1].Set(box.max.x, box.min.y, box.min.z);
		vertex[2].Set(box.min.x, box.max.y, box.min.z);
		vertex[3].Set(box.max.x, box.max.y, box.min.z);
		vertex[4].Set(box.min.x, box.min.y, box.max.z);
		vertex[5].Set(box.max.x, box.min.y, box.max.z);
		vertex[6].Set(box.min.x, box.max.y, box.max.z);
		vertex[7].Set(box.max.x, box.max.y, box.max.z);

		vertexBuffer.EndUpdate();
	}

#endif

// ZYUQURM
