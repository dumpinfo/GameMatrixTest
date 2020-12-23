 

#include "C4Forces.h"
#include "C4Fields.h"
#include "C4Physics.h"
#include "C4Water.h"
#include "C4Configuration.h"


using namespace C4;


namespace C4
{
	template class Registrable<Force, ForceRegistration>;
}


ForceRegistration::ForceRegistration(ForceType type, const char *name) : Registration<Force, ForceRegistration>(type)
{
	forceName = name;
}

ForceRegistration::~ForceRegistration()
{
}


Force::Force(ForceType type)
{
	forceType = type;

	targetField = nullptr;
}

Force::Force(const Force& force)
{
	forceType = force.forceType;

	targetField = nullptr;
}

Force::~Force()
{
}

Force *Force::New(ForceType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

Force *Force::CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags)
{
	return (new UnknownForce(data.GetType()));
}

bool Force::ValidField(const Field *field)
{
	return (true);
}

void Force::RegisterStandardForces(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static ForceReg<UnknownForce> unknownRegistration(kForceUnknown, table->GetString(StringID('FORC', kForceUnknown)));
	static ForceReg<GravityForce> gravityRegistration(kForceGravity, table->GetString(StringID('FORC', kForceGravity)));
	static ForceReg<FluidForce> fluidRegistration(kForceFluid, table->GetString(StringID('FORC', kForceFluid)));
	static ForceReg<WindForce> windRegistration(kForceWind, table->GetString(StringID('FORC', kForceWind)));

	SetUnknownCreator(&CreateUnknown);
}

void Force::PackType(Packer& data) const
{
	data << forceType;
}

void Force::Preprocess(void)
{
}

bool Force::ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque)
{
	return (false);
}

void Force::ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force)
{
}


UnknownForce::UnknownForce() : Force(kForceUnknown)
{
	unknownType = 0;
	unknownSize = 0;
	unknownData = nullptr; 
}

UnknownForce::UnknownForce(ForceType type) : Force(kForceUnknown) 
{
	unknownType = type; 
	unknownSize = 0;
	unknownData = nullptr;
} 

UnknownForce::UnknownForce(const UnknownForce& unknownForce) : Force(unknownForce) 
{ 
	unknownType = unknownForce.unknownType;
	unknownSize = unknownForce.unknownSize;
	unknownData = new char[unknownSize];
	MemoryMgr::CopyMemory(unknownForce.unknownData, unknownData, unknownSize); 
}

UnknownForce::~UnknownForce()
{
	delete[] unknownData;
}

Force *UnknownForce::Replicate(void) const
{
	return (new UnknownForce(*this));
}

bool UnknownForce::ValidField(const Field *field)
{
	const Force *force = field->GetForce();
	return ((force) && (force->GetForceType() == kForceUnknown));
}

void UnknownForce::PackType(Packer& data) const
{
	data << unknownType;
}

void UnknownForce::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data.WriteData(unknownData, unknownSize);
}

void UnknownForce::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 size = data.GetUnknownSize() - sizeof(ForceType);
	unknownSize = size;
	unknownData = new char[size];
	data.ReadData(unknownData, size);
}

int32 UnknownForce::GetSettingCount(void) const
{
	return (1);
}

Setting *UnknownForce::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('FORC', kForceUnknown, 'TYPE'));
		return (new InfoSetting('TYPE', Text::TypeToHexCharString(unknownType), title));
	}

	return (nullptr);
}

void UnknownForce::SetSetting(const Setting *setting)
{
}


GravityForce::GravityForce() : Force(kForceGravity)
{
	gravityAcceleration = 9.8F;
}

GravityForce::GravityForce(const GravityForce& gravityForce) : Force(gravityForce)
{
	gravityAcceleration = gravityForce.gravityAcceleration;
}

GravityForce::~GravityForce()
{
}

Force *GravityForce::Replicate(void) const
{
	return (new GravityForce(*this));
}

void GravityForce::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Force::Pack(data, packFlags);

	data << ChunkHeader('ACCL', 4);
	data << gravityAcceleration;

	data << TerminatorChunk;
}

void GravityForce::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Force::Unpack(data, unpackFlags);
	UnpackChunkList<GravityForce>(data, unpackFlags);
}

bool GravityForce::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'ACCL':

			data >> gravityAcceleration;
			return (true);
	}

	return (false);
}

int32 GravityForce::GetSettingCount(void) const
{
	return (1);
}

Setting *GravityForce::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('FORC', kForceGravity, 'ACCL'));
		return (new TextSetting('ACCL', gravityAcceleration, title));
	}

	return (nullptr);
}

void GravityForce::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'ACCL')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		gravityAcceleration = Text::StringToFloat(text);
	}
}

bool GravityForce::ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque)
{
	*force = GetTargetField()->GetWorldTransform()[2] * (-gravityAcceleration * rigidBody->GetBodyMass() * rigidBody->GetGravityMultiplier());
	torque->Set(0.0F, 0.0F, 0.0F);
	return (true);
}

void GravityForce::ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force)
{
	Vector3D direction = transformable->GetInverseWorldTransform() * GetTargetField()->GetWorldTransform()[2];
	Vector3D f = direction * (-gravityAcceleration * deformableBody->GetParticleMass() * deformableBody->GetGravityMultiplier());

	for (machine a = 0; a < count; a++)
	{
		force[a] += f;
	}
}


FluidForce::FluidForce() : Force(kForceFluid)
{
	fluidDensity = 1.0F;
	linearDrag = 0.25F;
	angularDrag = 0.1F;
	fluidCurrent.Set(0.0F, 0.0F, 0.0F);

	waterConnectorKey[0] = 0;
}

FluidForce::FluidForce(const FluidForce& fluidForce) : Force(fluidForce)
{
	fluidDensity = fluidForce.fluidDensity;
	linearDrag = fluidForce.linearDrag;
	angularDrag = fluidForce.angularDrag;
	fluidCurrent = fluidForce.fluidCurrent;

	waterConnectorKey = fluidForce.waterConnectorKey;
}

FluidForce::~FluidForce()
{
}

Force *FluidForce::Replicate(void) const
{
	return (new FluidForce(*this));
}

bool FluidForce::ValidField(const Field *field)
{
	FieldType type = field->GetFieldType();
	return ((type == kFieldBox) || (type == kFieldCylinder));
}

void FluidForce::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Force::Pack(data, packFlags);

	data << ChunkHeader('DENS', 4);
	data << fluidDensity;

	data << ChunkHeader('DRAG', 8);
	data << linearDrag;
	data << angularDrag;

	data << ChunkHeader('CRNT', sizeof(Vector3D));
	data << fluidCurrent;

	PackHandle handle = data.BeginChunk('WCON');
	data << waterConnectorKey;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void FluidForce::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Force::Unpack(data, unpackFlags);
	UnpackChunkList<FluidForce>(data, unpackFlags);
}

bool FluidForce::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DENS':

			data >> fluidDensity;
			return (true);

		case 'DRAG':

			data >> linearDrag;
			data >> angularDrag;
			return (true);

		case 'CRNT':

			data >> fluidCurrent;
			return (true);

		case 'WCON':

			data >> waterConnectorKey;
			return (true);
	}

	return (false);
}

int32 FluidForce::GetSettingCount(void) const
{
	return (6);
}

Setting *FluidForce::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('FORC', kForceFluid, 'DENS'));
		return (new TextSetting('DENS', fluidDensity, title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('FORC', kForceFluid, 'LDRG'));
		return (new TextSetting('LDRG', linearDrag, title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('FORC', kForceFluid, 'ADRG'));
		return (new TextSetting('ADRG', angularDrag, title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('FORC', kForceFluid, 'SPED'));
		return (new TextSetting('SPED', Magnitude(fluidCurrent), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('FORC', kForceFluid, 'CDIR'));
		return (new TextSetting('CDIR', Atan(fluidCurrent.y, fluidCurrent.x) * K::degrees, title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('FORC', kForceFluid, 'WCON'));
		return (new TextSetting('WCON', waterConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void FluidForce::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DENS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		fluidDensity = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'LDRG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		linearDrag = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'ADRG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		angularDrag = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'SPED')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		fluidCurrent.Set(FmaxZero(Text::StringToFloat(text)), 0.0F, 0.0F);
	}
	else if (identifier == 'CDIR')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		fluidCurrent.RotateAboutZ(Text::StringToFloat(text) * K::radians);
	}
	else if (identifier == 'WCON')
	{
		waterConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void FluidForce::Preprocess(void)
{
	const Node *node = GetTargetField()->GetConnectedNode(waterConnectorKey);
	if ((node) && (node->GetNodeType() == kNodeWaterBlock) && (node->GetController()))
	{
		waterBlock = static_cast<const WaterBlock *>(node);
	}
	else
	{
		waterBlock = nullptr;
	}
}

bool FluidForce::ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque)
{
	Antivector4D	plane;
	Point3D			centroid;

	if (waterBlock)
	{
		const Transform4D& inverseWaterTransform = waterBlock->GetInverseWorldTransform();
		float elevation = waterBlock->GetFilteredWaterElevation((inverseWaterTransform * rigidBody->GetWorldCenterOfMass()).GetPoint2D());
		plane = Antivector4D(-inverseWaterTransform(2,0), -inverseWaterTransform(2,1), -inverseWaterTransform(2,2), elevation - inverseWaterTransform(2,3)) * worldTransform;
	}
	else
	{
		plane = GetTargetField()->GetSurfacePlane() * worldTransform;
	}

	float volume = rigidBody->CalculateSubmergedVolume(plane, &centroid);
	if (volume > 0.0F)
	{
		PhysicsController *physicsController = rigidBody->GetPhysicsController();
		physicsController->IncrementPhysicsCounter(kPhysicsCounterBuoyancy);

		Vector3D buoyantForce = physicsController->GetGravityAcceleration() * -(volume * fluidDensity * rigidBody->GetGravityMultiplier());
		Antivector3D buoyantTorque = (worldTransform * centroid - rigidBody->GetWorldCenterOfMass()) ^ buoyantForce;

		Vector3D dv = GetTargetField()->GetWorldTransform() * fluidCurrent - rigidBody->GetLinearVelocity();
		const Antivector3D& dw = rigidBody->GetAngularVelocity();

		volume *= rigidBody->GetFluidDragMultiplier();

		*force = buoyantForce + dv * (Magnitude(dv) * linearDrag * volume);
		*torque = buoyantTorque - dw * (Magnitude(dw) * angularDrag * volume);

		if (waterBlock)
		{
			rigidBody->SetSubmergedWaterBlock(waterBlock);
		}

		return (true);
	}

	return (false);
}

void FluidForce::ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force)
{
	const PhysicsController *physicsController = deformableBody->GetPhysicsController();
	Vector3D gravity = transformable->GetInverseWorldTransform() * physicsController->GetGravityAcceleration() * deformableBody->GetGravityMultiplier();
	float drag = linearDrag * deformableBody->GetFluidDragMultiplier();

	Vector3D current = transformable->GetInverseWorldTransform() * (GetTargetField()->GetWorldTransform() * fluidCurrent);

	float particleVolume = deformableBody->GetParticleVolume();
	float particleRadius = deformableBody->GetParticleRadius();
	float crossSection = 0.5F * particleVolume / particleRadius;

	if (waterBlock)
	{
		const Transform4D& inverseWaterTransform = waterBlock->GetInverseWorldTransform();
		Antivector4D plane(-inverseWaterTransform(2,0), -inverseWaterTransform(2,1), -inverseWaterTransform(2,2), 0.0F);

		for (machine a = 0; a < count; a++)
		{
			float elevation = waterBlock->GetFilteredWaterElevation((inverseWaterTransform * (transformable->GetWorldTransform() * position[a])).GetPoint2D());
			plane.w = elevation - inverseWaterTransform(2,3);

			float d = ((plane * transformable->GetWorldTransform()) ^ position[a]) + particleRadius;
			if (d > 0.0F)
			{
				float volume = Fmin(d * crossSection, particleVolume);
				Vector3D buoyantForce = gravity * -(volume * fluidDensity);
				Vector3D dv = current - velocity[a];
				force[a] += buoyantForce + dv * (Magnitude(dv) * drag * volume);
			}
		}
	}
	else
	{
		Antivector4D plane = GetTargetField()->GetSurfacePlane() * transformable->GetWorldTransform();

		for (machine a = 0; a < count; a++)
		{
			float d = (plane ^ position[a]) + particleRadius;
			if (d > 0.0F)
			{
				float volume = Fmin(d * crossSection, particleVolume);
				Vector3D buoyantForce = gravity * -(volume * fluidDensity);
				Vector3D dv = current - velocity[a];
				force[a] += buoyantForce + dv * (Magnitude(dv) * drag * volume);
			}
		}
	}
}


WindForce::WindForce() : Force(kForceWind)
{
	windVelocity.Set(1.0F, 0.0F, 0.0F);
	windDrag = 1.0F;
}

WindForce::WindForce(const WindForce& windForce) : Force(windForce)
{
	windVelocity = windForce.windVelocity;
	windDrag = windForce.windDrag;
}

WindForce::~WindForce()
{
}

Force *WindForce::Replicate(void) const
{
	return (new WindForce(*this));
}

void WindForce::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Force::Pack(data, packFlags);

	data << ChunkHeader('VELO', sizeof(Vector3D));
	data << windVelocity;

	data << ChunkHeader('DRAG', 4);
	data << windDrag;

	data << TerminatorChunk;
}

void WindForce::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Force::Unpack(data, unpackFlags);
	UnpackChunkList<WindForce>(data, unpackFlags);
}

bool WindForce::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VELO':

			data >> windVelocity;
			return (true);

		case 'DRAG':

			data >> windDrag;
			return (true);
	}

	return (false);
}

int32 WindForce::GetSettingCount(void) const
{
	return (2);
}

Setting *WindForce::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('FORC', kForceWind, 'SPED'));
		return (new TextSetting('SPED', Magnitude(windVelocity), title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('FORC', kForceWind, 'DRAG'));
		return (new TextSetting('DRAG', windDrag, title));
	}

	return (nullptr);
}

void WindForce::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SPED')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windVelocity.Set(FmaxZero(Text::StringToFloat(text)), 0.0F, 0.0F);
	}
	else if (identifier == 'DRAG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		windDrag = FmaxZero(Text::StringToFloat(text));
	}
}

bool WindForce::ApplyForce(RigidBodyController *rigidBody, const Transform4D& worldTransform, Vector3D *restrict force, Antivector3D *restrict torque)
{
	Vector3D dv = GetTargetField()->GetWorldTransform() * windVelocity - rigidBody->GetLinearVelocity();
	*force = dv * (Magnitude(dv) * windDrag * (rigidBody->GetBodyVolume() * rigidBody->GetWindDragMultiplier()));
	torque->Set(0.0F, 0.0F, 0.0F);
	return (true);
}

void WindForce::ApplyForce(DeformableBodyController *deformableBody, const Transformable *transformable, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force)
{
	Vector3D wind = transformable->GetInverseWorldTransform() * (GetTargetField()->GetWorldTransform() * windVelocity);
	float drag = windDrag * deformableBody->GetWindDragMultiplier();

	for (machine a = 0; a < count; a++)
	{
		force[a] += (wind - velocity[a]) * drag;
	}
}

// ZYUQURM
