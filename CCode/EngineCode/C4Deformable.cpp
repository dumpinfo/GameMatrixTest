 

#include "C4Deformable.h"
#include "C4Simulation.h"
#include "C4Forces.h"
#include "C4Fields.h"
#include "C4Blockers.h"
#include "C4Configuration.h"
#include "C4World.h"


using namespace C4;


namespace
{
	const float kRopeDeltaTime = (float) kRopeTimeStep * 0.001F;
	const float kRopeKilonewtonsDeltaTime = kRopeDeltaTime * 0.001F;
	const float kRopeSquaredDeltaTime = kRopeDeltaTime * kRopeDeltaTime;
	const float kRopeInverseDeltaTime = 1.0F / kRopeDeltaTime;
	const float kRopeInverseMotionDeltaTime = 1.0F / (float) kRopeTimeStep;

	const float kClothDeltaTime = (float) kClothTimeStep * 0.001F;
	const float kClothSquaredDeltaTime = kClothDeltaTime * kClothDeltaTime;
	const float kClothInverseDeltaTime = 1.0F / kClothDeltaTime;
	const float kClothInverseMotionDeltaTime = 1.0F / (float) kClothTimeStep;
}


namespace C4
{
	class ForceFieldThreadData
	{
		private:

			int32					threadFlag;
			Array<Field *, 16>		fieldArray;

		public:

			ForceFieldThreadData(int32 flag);
			~ForceFieldThreadData();

			const ImmutableArray<Field *>& GetFieldArray(void) const
			{
				return (fieldArray);
			}

			bool AddField(Field *field);
	};
}


const char C4::kConnectorKeyBlocker[] = "%Blocker";
const char C4::kConnectorKeyWind[] = "%Wind";


Mutex RopeController::ropeMutex;


ForceFieldThreadData::ForceFieldThreadData(int32 flag)
{
	threadFlag = flag;
}

ForceFieldThreadData::~ForceFieldThreadData()
{
	int32 mask = ~threadFlag;

	for (Field *field : fieldArray)
	{
		AtomicAnd(field->GetQueryThreadFlags(), mask);
	}
}

bool ForceFieldThreadData::AddField(Field *field)
{
	int32 flag = threadFlag;

	volatile int32 *fieldFlags = field->GetQueryThreadFlags();
	if (AtomicOr(fieldFlags, flag) & flag)
	{
		return (false);
	}

	fieldArray.AddElement(field);
	return (true);
}


DeformableBodyController::DeformableBodyController(ControllerType type, Job::ExecuteProc *stepProc, void *cookie) :
		BodyController(type, kBodyDeformable),
		stepSimulationJob(stepProc, cookie)
{
	SetBaseControllerType(kControllerDeformableBody);

	deformableBodyFlags = 0;
	deformableBodyState = 0;

	warmStartTime = 0;
	autoSleepTime = 4000;

	particleMass = 0.5F; 
	particleVolume = 1.0F;
	particleRadius = 0.025F;
 
	volumeMultiplier = 1.0F;
	internalResistance = 0.0F; 
	windVelocity.Set(0.0F, 0.0F, 0.0F);
}
 
DeformableBodyController::DeformableBodyController(const DeformableBodyController& deformableController, Job::ExecuteProc *stepProc, void *cookie) :
		BodyController(deformableController), 
		stepSimulationJob(stepProc, cookie) 
{
	deformableBodyFlags = deformableController.deformableBodyFlags;
	deformableBodyState = 0;
 
	warmStartTime = deformableController.warmStartTime;
	autoSleepTime = deformableController.autoSleepTime;

	particleMass = deformableController.particleMass;
	particleVolume = deformableController.particleVolume;
	particleRadius = deformableController.particleRadius;

	volumeMultiplier = deformableController.volumeMultiplier;
	internalResistance = deformableController.internalResistance;
	windVelocity = deformableController.windVelocity;
}

DeformableBodyController::~DeformableBodyController()
{
}

void DeformableBodyController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	BodyController::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << deformableBodyFlags;

	data << ChunkHeader('STAT', 4);
	data << deformableBodyState;

	data << ChunkHeader('WARM', 4);
	data << warmStartTime;

	data << ChunkHeader('SLEP', 4);
	data << autoSleepTime;

	data << ChunkHeader('MASS', 4);
	data << particleMass;

	data << ChunkHeader('VOLU', 4);
	data << volumeMultiplier;

	data << ChunkHeader('IRES', 4);
	data << internalResistance;

	data << ChunkHeader('WVEL', sizeof(Vector3D));
	data << windVelocity;

	data << TerminatorChunk;
}

void DeformableBodyController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	BodyController::Unpack(data, unpackFlags);
	UnpackChunkList<DeformableBodyController>(data, unpackFlags);
}

bool DeformableBodyController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> deformableBodyFlags;
			return (true);

		case 'STAT':

			data >> deformableBodyState;
			return (true);

		case 'WARM':

			data >> warmStartTime;
			return (true);

		case 'SLEP':

			data >> autoSleepTime;
			return (true);

		case 'MASS':

			data >> particleMass;
			return (true);

		case 'VOLU':

			data >> volumeMultiplier;
			return (true);

		case 'IRES':

			data >> internalResistance;
			return (true);

		case 'WVEL':

			data >> windVelocity;
			return (true);
	}

	return (false);
}

int32 DeformableBodyController::GetSettingCount(void) const
{
	return (5);
}

Setting *DeformableBodyController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'FLAG'));
		return (new HeadingSetting('FLAG', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'FLAG', 'IFRC'));
		return (new BooleanSetting('IFRC', ((deformableBodyFlags & kDeformableBodyForceFieldInhibit) != 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'TIME'));
		return (new HeadingSetting('TIME', title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'TIME', 'WARM'));
		return (new TextSetting('WARM', (float) warmStartTime * 0.001F, title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'TIME', 'INVS'));
		return (new TextSetting('INVS', (float) autoSleepTime * 0.001F, title));
	}

	return (nullptr);
}

void DeformableBodyController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'IFRC')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			deformableBodyFlags |= kDeformableBodyForceFieldInhibit;
		}
		else
		{
			deformableBodyFlags &= ~kDeformableBodyForceFieldInhibit;
		}
	}
	else if (identifier == 'WARM')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		warmStartTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
	else if (identifier == 'INVS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		autoSleepTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
}

void DeformableBodyController::Preprocess(void)
{
	BodyController::Preprocess();

	updateTime = 0;
	halfInverseParticleMass = 0.5F / particleMass;

	const Node *deformableGeometry = GetTargetNode();

	Node *node = deformableGeometry->GetConnectedNode(kConnectorKeyBlocker);
	if ((node) && (node->GetNodeType() == kNodeBlocker))
	{
		blockerNode = node;
	}

	node = deformableGeometry->GetConnectedNode(kConnectorKeyWind);
	if ((node) && (node->GetNodeType() == kNodeField))
	{
		const Force *force = static_cast<Field *>(node)->GetForce();
		if ((force) && (force->GetForceType() == kForceWind))
		{
			windFieldNode = node;
		}
	}
}

void DeformableBodyController::Sleep(void)
{
	deformableBodyState &= ~kDeformableBodyAutoAsleep;
	BodyController::Sleep();
}

void DeformableBodyController::Move(void)
{
	int32 time = updateTime + TheTimeMgr->GetDeltaTime();
	updateTime = time;

	if (time >= autoSleepTime)
	{
		AutoSleep();
	}

	PhysicsController *physicsController = GetPhysicsController();
	if (physicsController)
	{
		physicsController->AddDeformableBody(this);
	}
}

void DeformableBodyController::Update(void)
{
	unsigned_int32 flags = GetControllerFlags();
	SetControllerFlags(flags & ~kControllerUpdate);

	updateTime = 0;

	if ((flags & kControllerAsleep) && (deformableBodyState & kDeformableBodyAutoAsleep))
	{
		BodyController::Wake();

		const Node *field = windFieldNode;
		if ((field) && (static_cast<const Field *>(field)->GetObject()->GetFieldFlags() & kFieldExclusive))
		{
			Controller *controller = field->GetController();
			if ((controller) && (controller->GetControllerFlags() & kControllerLocal))
			{
				controller->Wake();
			}
		}
	}
}

void DeformableBodyController::RecursiveWake(void)
{
	if (Asleep())
	{
		Wake();
	}
}

void DeformableBodyController::AutoSleep(void)
{
	deformableBodyState |= kDeformableBodyAutoAsleep;
	BodyController::Sleep();

	const Node *field = windFieldNode;
	if ((field) && (static_cast<const Field *>(field)->GetObject()->GetFieldFlags() & kFieldExclusive))
	{
		Controller *controller = field->GetController();
		if ((controller) && (controller->GetControllerFlags() & kControllerLocal))
		{
			controller->Sleep();
		}
	}
}

Vector3D DeformableBodyController::CalculateGravityForce(void) const
{
	return (GetTargetNode()->GetInverseWorldTransform() * GetPhysicsController()->GetGravityAcceleration() * (GetGravityMultiplier() * GetParticleMass()));
}

void DeformableBodyController::QueryCellForceFields(Site *site, ForceFieldThreadData *threadData, FieldArray& fieldArray) const
{
	const Node *deformableGeometry = GetTargetNode();

	const Bond *bond = site->GetFirstOutgoingEdge();
	while (bond)
	{
		Site *subsite = bond->GetFinishElement();
		if (subsite->GetWorldBoundingBox().Intersection(deformableGeometry->GetWorldBoundingBox()))
		{
			if (subsite->GetCellIndex() < 0)
			{
				Field *field = static_cast<Field *>(subsite);
				if (threadData->AddField(field))
				{
					if ((field->Enabled()) && (field->GetForce()))
					{
						const BoundingSphere *sphere = deformableGeometry->GetBoundingSphere();
						if (!field->GetObject()->ExteriorSphere(field->GetInverseWorldTransform() * sphere->GetCenter(), sphere->GetRadius()))
						{
							fieldArray.AddElement(field);
						}
					}
				}
			}
			else
			{
				QueryCellForceFields(subsite, threadData, fieldArray);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void DeformableBodyController::QueryForceFields(FieldArray& fieldArray, int32 threadIndex) const
{
	if (!(GetDeformableBodyFlags() & kDeformableBodyForceFieldInhibit))
	{
		ForceFieldThreadData threadData(1 << threadIndex);

		for (Zone *zone : GetTargetNode()->GetZoneMembershipArray())
		{
			QueryCellForceFields(zone->GetCellGraphSite(kCellGraphField), &threadData, fieldArray);
		}
	}
}

void DeformableBodyController::ApplyForceFields(const ImmutableArray<const Field *>& fieldArray, int32 count, const Point3D *position, const SimdVector3D *velocity, SimdVector3D *restrict force)
{
	for (machine a = 0; a < count; a++)
	{
		force[a].Set(0.0F, 0.0F, 0.0F);
	}

	const Node *deformableGeometry = GetTargetNode();
	for (const Field *field : fieldArray)
	{
		field->GetForce()->ApplyForce(this, deformableGeometry, count, position, velocity, force);
	}
}


RopeGeometryObject::RopeGeometryObject() : PrimitiveGeometryObject(kPrimitiveRope)
{
	ropeGeometryType = kRopeGeometryTube;
	ropeGeometryFlags = 0;
	texcoordScale = 1.0F;

	SetStaticSurfaceData(3, staticSurfaceData);
}

RopeGeometryObject::RopeGeometryObject(float length, float radius, int32 count) : PrimitiveGeometryObject(kPrimitiveRope)
{
	ropeLength = length;
	ropeRadius = radius;

	ropeGeometryType = kRopeGeometryTube;
	ropeGeometryFlags = 0;
	texcoordScale = 1.0F;

	SetMaxSubdiv(count - 1, 8);
	SetGeometryFlags(kGeometryMarkingInhibit | kGeometryDynamic);
	SetCollisionExclusionMask(kCollisionExcludeAll);

	SetStaticSurfaceData(3, staticSurfaceData, true);

	for (machine a = 0; a < 3; a++)
	{
		staticSurfaceData[a].textureAlignData[0].alignMode = kTextureAlignNatural;
		staticSurfaceData[a].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
		staticSurfaceData[a].textureAlignData[1].alignMode = kTextureAlignNatural;
		staticSurfaceData[a].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
	}
}

RopeGeometryObject::~RopeGeometryObject()
{
}

void RopeGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('LNTH', 4);
	data << ropeLength;

	data << ChunkHeader('RADI', 4);
	data << ropeRadius;

	data << ChunkHeader('GEOM', 4);
	data << ropeGeometryType;

	data << ChunkHeader('FLAG', 4);
	data << ropeGeometryFlags;

	data << ChunkHeader('TSCL', 4);
	data << texcoordScale;

	data << TerminatorChunk;
}

void RopeGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<RopeGeometryObject>(data, unpackFlags);
}

bool RopeGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'LNTH':

			data >> ropeLength;
			return (true);

		case 'RADI':

			data >> ropeRadius;
			return (true);

		case 'GEOM':

			data >> ropeGeometryType;
			return (true);

		case 'FLAG':

			data >> ropeGeometryFlags;
			return (true);

		case 'TSCL':

			data >> texcoordScale;
			return (true);
	}

	return (false);
}

int32 RopeGeometryObject::GetCategorySettingCount(Type category) const
{
	int32 count = PrimitiveGeometryObject::GetCategorySettingCount(category);
	if (category == kObjectGeometry)
	{
		count += 5;
	}

	return (count);
}

Setting *RopeGeometryObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectGeometry)
	{
		int32 i = index - PrimitiveGeometryObject::GetCategorySettingCount(kObjectGeometry);
		if (i >= 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kPrimitiveRope));
				return (new HeadingSetting(kPrimitiveRope, title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kPrimitiveRope, 'RGEO'));

				MenuSetting *menu = new MenuSetting('RGEO', (ropeGeometryType != kRopeGeometryTube), title, 2);
				menu->SetMenuItemString(0, table->GetString(StringID(kObjectGeometry, kPrimitiveRope, 'RGEO', 'TUBE')));
				menu->SetMenuItemString(1, table->GetString(StringID(kObjectGeometry, kPrimitiveRope, 'RGEO', 'CROS')));
				return (menu);
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kPrimitiveRope, 'RTSC'));
				return (new TextSetting('RTSC', texcoordScale, title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kPrimitiveRope, 'SWAP'));
				return (new BooleanSetting('SWAP', ((ropeGeometryFlags & kRopeGeometrySwapLateralTexcoords) != 0), title));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kPrimitiveRope, 'CROF'));
				return (new BooleanSetting('CROF', ((ropeGeometryFlags & kRopeGeometryOffsetCrossTexcoords) != 0), title));
			}

			return (nullptr);
		}
	}

	return (PrimitiveGeometryObject::GetCategorySetting(category, index, flags));
}

void RopeGeometryObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectGeometry)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'RGEO')
		{
			ropeGeometryType = (static_cast<const MenuSetting *>(setting)->GetMenuSelection() == 0) ? kRopeGeometryTube : kRopeGeometryCross;
		}
		else if (identifier == 'RTSC')
		{
			texcoordScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else if (identifier == 'SWAP')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				ropeGeometryFlags |= kRopeGeometrySwapLateralTexcoords;
			}
			else
			{
				ropeGeometryFlags &= ~kRopeGeometrySwapLateralTexcoords;
			}
		}
		else if (identifier == 'CROF')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				ropeGeometryFlags |= kRopeGeometryOffsetCrossTexcoords;
			}
			else
			{
				ropeGeometryFlags &= ~kRopeGeometryOffsetCrossTexcoords;
			}
		}
		else
		{
			PrimitiveGeometryObject::SetCategorySetting(kObjectGeometry, setting);
		}
	}
	else
	{
		PrimitiveGeometryObject::SetCategorySetting(category, setting);
	}
}

int32 RopeGeometryObject::GetObjectSize(float *size) const
{
	size[0] = ropeLength;
	size[1] = ropeRadius;
	return (2);
}

void RopeGeometryObject::SetObjectSize(const float *size)
{
	ropeLength = size[0];
	ropeRadius = size[1];
}

void RopeGeometryObject::Build(Geometry *geometry)
{
	int32 levelCount = Min(GetBuildLevelCount(), 2);
	SetGeometryLevelCount(levelCount);

	for (machine level = 0; level < levelCount; level++)
	{
		ArrayDescriptor		desc[5];
		Mesh				mesh;

		int32 xdiv = Min(Max(GetMaxSubdivX(), kMinRopeSubdivX), kMaxRopeSubdivX);
		int32 ydiv = Min(Max(GetMaxSubdivY(), kMinRopeSubdivY), kMaxRopeSubdivY);

		if (level < levelCount - 1)
		{
			xdiv *= 2;
		}
		else if (level > 0)
		{
			ydiv = Max(ydiv >> 1, kMinRopeSubdivY);
		}

		if (ropeGeometryType == kRopeGeometryTube)
		{
			int32 tubeVertexCount = (xdiv + 1) * (ydiv + 1);
			int32 vertexCount = tubeVertexCount;
			int32 triangleCount = xdiv * ydiv * 2;

			bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
			if (endcap)
			{
				vertexCount += ydiv * 2;
				triangleCount += ydiv * 2 - 4;
			}

			desc[0].identifier = kArrayPosition;
			desc[0].elementCount = vertexCount;
			desc[0].elementSize = sizeof(Point3D);
			desc[0].componentCount = 3;

			desc[1].identifier = kArrayNormal;
			desc[1].elementCount = vertexCount;
			desc[1].elementSize = sizeof(Vector3D);
			desc[1].componentCount = 3;

			desc[2].identifier = kArrayTexcoord;
			desc[2].elementCount = vertexCount;
			desc[2].elementSize = sizeof(Point2D);
			desc[2].componentCount = 2;

			desc[3].identifier = kArraySurfaceIndex;
			desc[3].elementCount = vertexCount;
			desc[3].elementSize = 2;
			desc[3].componentCount = 1;

			desc[4].identifier = kArrayPrimitive;
			desc[4].elementCount = triangleCount;
			desc[4].elementSize = sizeof(Triangle);
			desc[4].componentCount = 1;

			mesh.AllocateStorage(vertexCount, 5, desc);

			Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
			Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
			Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

			float l = ropeLength;
			float r = ropeRadius;

			float dx = 1.0F / (float) xdiv;
			float dy = 1.0F / (float) ydiv;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) i * dx;
				float x = s * l;

				(++vertex)->Set(x, r, 0.0F);
				(++normal)->Set(0.0F, 1.0F, 0.0F);
				(++texcoord)->Set(s, 0.0F);

				for (machine j = 1; j < ydiv; j++)
				{
					float t = (float) j * dy;
					Vector2D v = CosSin(t * K::tau);
					float y = v.x * r;
					float z = v.y * r;

					(++vertex)->Set(x, y, z);
					(++normal)->Set(0.0F, v.x, v.y);
					(++texcoord)->Set(s, t);
				}

				(++vertex)->Set(x, r, 0.0F);
				(++normal)->Set(0.0F, 1.0F, 0.0F);
				(++texcoord)->Set(s, 1.0F);
			}

			(++vertex)->Set(l, r, 0.0F);
			(++normal)->Set(0.0F, 1.0F, 0.0F);
			(++texcoord)->Set(1.0F, 0.0F);

			for (machine j = 1; j < ydiv; j++)
			{
				float t = (float) j * dy;
				Vector2D v = CosSin(t * K::tau);
				float y = v.x * r;
				float z = v.y * r;

				(++vertex)->Set(l, y, z);
				(++normal)->Set(0.0F, v.x, v.y);
				(++texcoord)->Set(1.0F, t);
			}

			(++vertex)->Set(l, r, 0.0F);
			(++normal)->Set(0.0F, 1.0F, 0.0F);
			(++texcoord)->Set(1.0F, 1.0F);

			texcoord = mesh.GetArray<Point2D>(kArrayTexcoord);

			float scale = texcoordScale;
			for (machine a = 0; a < tubeVertexCount; a++)
			{
				texcoord[a].x *= scale;
			}

			if (ropeGeometryFlags & kRopeGeometrySwapLateralTexcoords)
			{
				for (machine a = 0; a < tubeVertexCount; a++)
				{
					texcoord[a].Set(texcoord[a].y, texcoord[a].x);
				}
			}

			int32 count = (xdiv + 1) * (ydiv + 1);
			unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
			for (machine a = 0; a < count; a++)
			{
				*++surfaceIndex = 0;
			}

			Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = i * (ydiv + 1);
				for (machine j = 0; j < ydiv; j++)
				{
					int32 base = k + j;
					if (((i + j) & 1) == 0)
					{
						triangle[0].Set(base, base + 1, base + ydiv + 1);
						triangle[1].Set(base + 1, base + ydiv + 2, base + ydiv + 1);
					}
					else
					{
						triangle[0].Set(base, base + ydiv + 2, base + ydiv + 1);
						triangle[1].Set(base, base + 1, base + ydiv + 2);
					}

					triangle += 2;
				}
			}

			if (endcap)
			{
				texcoord += tubeVertexCount - 1;

				const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
				float dt = K::tau / (float) ydiv;
				float tex = 0.5F / r;

				for (machine j = ydiv; j > 0; j--)
				{
					Vector2D v = CosSin((float) j * dt) * r;

					*++vertex = vtx[j];
					(++normal)->Set(-1.0F, 0.0F, 0.0F);
					(++texcoord)->Set(1.0F - (v.x + r) * tex, (v.y + r) * tex);
					*++surfaceIndex = 1;
				}

				vtx += xdiv * (ydiv + 1);

				for (machine j = 0; j < ydiv; j++)
				{
					Vector2D v = CosSin((float) j * dt) * r;

					*++vertex = vtx[j];
					(++normal)->Set(1.0F, 0.0F, 0.0F);
					(++texcoord)->Set((v.x + r) * tex, (v.y + r) * tex);
					*++surfaceIndex = 2;
				}

				int32 n = (xdiv + 1) * (ydiv + 1);

				TriangulateDisk(ydiv, n, triangle);
				TriangulateDisk(ydiv, n + ydiv, triangle + (ydiv - 2));
			}
		}
		else
		{
			int32 vertexCount = (xdiv + 1) * 4;
			int32 triangleCount = xdiv * 4;

			desc[0].identifier = kArrayPosition;
			desc[0].elementCount = vertexCount;
			desc[0].elementSize = sizeof(Point3D);
			desc[0].componentCount = 3;

			desc[1].identifier = kArrayNormal;
			desc[1].elementCount = vertexCount;
			desc[1].elementSize = sizeof(Vector3D);
			desc[1].componentCount = 3;

			desc[2].identifier = kArrayTexcoord;
			desc[2].elementCount = vertexCount;
			desc[2].elementSize = sizeof(Point2D);
			desc[2].componentCount = 2;

			desc[3].identifier = kArraySurfaceIndex;
			desc[3].elementCount = vertexCount;
			desc[3].elementSize = 2;
			desc[3].componentCount = 1;

			desc[4].identifier = kArrayPrimitive;
			desc[4].elementCount = triangleCount;
			desc[4].elementSize = sizeof(Triangle);
			desc[4].componentCount = 1;

			mesh.AllocateStorage(vertexCount, 5, desc);

			Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
			Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
			Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

			float l = ropeLength;
			float r = ropeRadius;

			float dx = 1.0F / (float) xdiv;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) i * dx;
				float x = s * l;

				(++vertex)->Set(x, 0.0F, r);
				(++normal)->Set(0.0F, 1.0F, 0.0F);
				(++texcoord)->Set(s, 0.0F);

				(++vertex)->Set(x, 0.0F, -r);
				(++normal)->Set(0.0F, 1.0F, 0.0F);
				(++texcoord)->Set(s, 1.0F);
			}

			(++vertex)->Set(l, 0.0F, r);
			(++normal)->Set(0.0F, 1.0F, 0.0F);
			(++texcoord)->Set(1.0F, 0.0F);

			(++vertex)->Set(l, 0.0F, -r);
			(++normal)->Set(0.0F, 1.0F, 0.0F);
			(++texcoord)->Set(1.0F, 1.0F);

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) i * dx;
				float x = s * l;

				(++vertex)->Set(x, -r, 0.0F);
				(++normal)->Set(0.0F, 0.0F, 1.0F);
				(++texcoord)->Set(s, 0.0F);

				(++vertex)->Set(x, r, 0.0F);
				(++normal)->Set(0.0F, 0.0F, 1.0F);
				(++texcoord)->Set(s, 1.0F);
			}

			(++vertex)->Set(l, -r, 0.0F);
			(++normal)->Set(0.0F, 0.0F, 1.0F);
			(++texcoord)->Set(1.0F, 0.0F);

			(++vertex)->Set(l, r, 0.0F);
			(++normal)->Set(0.0F, 0.0F, 1.0F);
			(++texcoord)->Set(1.0F, 1.0F);

			texcoord = mesh.GetArray<Point2D>(kArrayTexcoord);

			float scale = texcoordScale;
			for (machine a = 0; a < vertexCount; a++)
			{
				texcoord[a].x *= scale;
			}

			if (ropeGeometryFlags & kRopeGeometryOffsetCrossTexcoords)
			{
				int32 count = vertexCount / 2;
				Point2D *restrict crossTexcoord = texcoord + count;

				for (machine a = 0; a < count; a++)
				{
					crossTexcoord[a].x -= 0.5F;
				}
			}

			if (ropeGeometryFlags & kRopeGeometrySwapLateralTexcoords)
			{
				for (machine a = 0; a < vertexCount; a++)
				{
					texcoord[a].Set(texcoord[a].y, texcoord[a].x);
				}
			}

			unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex);

			for (machine a = 0; a <= xdiv; a++)
			{
				surfaceIndex[0] = 0;
				surfaceIndex[1] = 0;
				surfaceIndex += 2;
			}

			for (machine a = 0; a <= xdiv; a++)
			{
				surfaceIndex[0] = 1;
				surfaceIndex[1] = 1;
				surfaceIndex += 2;
			}

			Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

			int32 k = 0;
			for (machine i = 0; i < xdiv; i++)
			{
				if ((i & 1) == 0)
				{
					triangle[0].Set(k, k + 2, k + 1);
					triangle[1].Set(k + 1, k + 2, k + 3);
				}
				else
				{
					triangle[0].Set(k, k + 2, k + 3);
					triangle[1].Set(k, k + 3, k + 1);
				}

				triangle += 2;
				k += 2;
			}

			k += 2;
			for (machine i = 0; i < xdiv; i++)
			{
				if ((i & 1) == 0)
				{
					triangle[0].Set(k, k + 2, k + 1);
					triangle[1].Set(k + 1, k + 2, k + 3);
				}
				else
				{
					triangle[0].Set(k, k + 2, k + 3);
					triangle[1].Set(k, k + 3, k + 1);
				}

				triangle += 2;
				k += 2;
			}
		}

		BuildStandardArrays(&mesh, GetGeometryLevel(level), geometry);
	}
}


RopeGeometry::RopeGeometry() : PrimitiveGeometry(kPrimitiveRope)
{
	ropeBoundingBox = nullptr;
}

RopeGeometry::RopeGeometry(float length, float radius, int32 count) : PrimitiveGeometry(kPrimitiveRope)
{
	SetNewObject(new RopeGeometryObject(length, radius, count));
	SetController(new RopeController);

	ropeBoundingBox = nullptr;
}

RopeGeometry::RopeGeometry(const RopeGeometry& ropeGeometry) : PrimitiveGeometry(ropeGeometry)
{
	ropeBoundingBox = nullptr;
}

RopeGeometry::~RopeGeometry()
{
}

Node *RopeGeometry::Replicate(void) const
{
	return (new RopeGeometry(*this));
}

bool RopeGeometry::CalculateBoundingBox(Box3D *box) const
{
	if (ropeBoundingBox)
	{
		*box = *ropeBoundingBox;
	}
	else
	{
		const RopeGeometryObject *object = GetObject();
		float ropeRadius = object->GetRopeRadius();

		box->min.Set(0.0F, -ropeRadius, -ropeRadius);
		box->max.Set(object->GetRopeLength(), ropeRadius, ropeRadius);
	}

	return (true);
}

bool RopeGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	if (ropeBoundingBox)
	{
		sphere->SetCenter((ropeBoundingBox->min + ropeBoundingBox->max) * 0.5F);
		sphere->SetRadius(Magnitude(ropeBoundingBox->max - ropeBoundingBox->min) * 0.5F);
	}
	else
	{
		const RopeGeometryObject *object = GetObject();
		float ropeRadius = object->GetRopeRadius();

		float l = object->GetRopeLength() * 0.5F;
		sphere->SetCenter(l, 0.0F, 0.0F);
		sphere->SetRadius(Sqrt(l * l + ropeRadius * ropeRadius));
	}

	return (true);
}

int32 RopeGeometry::GetInternalConnectorCount(void) const
{
	return (PrimitiveGeometry::GetInternalConnectorCount() + 2);
}

const char *RopeGeometry::GetInternalConnectorKey(int32 index) const
{
	int32 count = PrimitiveGeometry::GetInternalConnectorCount();
	if (index < count)
	{
		return (PrimitiveGeometry::GetInternalConnectorKey(index));
	}

	if (index == count)
	{
		return (kConnectorKeyBlocker);
	}
	else if (index == count + 1)
	{
		return (kConnectorKeyWind);
	}

	return (nullptr);
}

bool RopeGeometry::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyBlocker)
	{
		return (node->GetNodeType() == kNodeBlocker);
	}
	else if (key == kConnectorKeyWind)
	{
		if (node->GetNodeType() == kNodeField)
		{
			const Field *field = static_cast<const Field *>(node);
			const Force *force = field->GetForce();
			return ((force) && (force->GetForceType() == kForceWind));
		}

		return (false);
	}

	return (PrimitiveGeometry::ValidConnectedNode(key, node));
}

void RopeGeometry::Preprocess(void)
{
	PrimitiveGeometry::Preprocess();

	const Controller *controller = GetController();
	if ((controller) && (controller->GetControllerType() == kControllerRope))
	{
		const RopeController *ropeController = static_cast<const RopeController *>(controller);
		ropeBoundingBox = ropeController->GetRopeBoundingBox();
	}
}


RopeController::RopeController() :
		DeformableBodyController(kControllerRope, &JobStepSimulation, this),
		ropeUpdateJob(nullptr, &FinalizeRopeUpdate, this),
		ropeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	fieldStorage = nullptr;
	vertexStorage = nullptr;
	particleCount = 0;

	stretchSpringConstant = 2000.0F;
	stretchDamperConstant = 0.0F;
	bendSpringConstant = 1000.0F;
	bendDamperConstant = 0.0F;

	attachmentFlags = (1 << kRopeAttachmentCount) - 1;
	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		attachmentConnectorKey[a][0] = 0;
	}
}

RopeController::RopeController(const RopeController& ropeController) :
		DeformableBodyController(ropeController, &JobStepSimulation, this),
		ropeUpdateJob(nullptr, &FinalizeRopeUpdate, this),
		ropeVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	fieldStorage = nullptr;
	vertexStorage = nullptr;
	particleCount = 0;

	stretchSpringConstant = ropeController.stretchSpringConstant;
	stretchDamperConstant = ropeController.stretchDamperConstant;
	bendSpringConstant = ropeController.bendSpringConstant;
	bendDamperConstant = ropeController.bendDamperConstant;

	attachmentFlags = ropeController.attachmentFlags;
	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		attachmentConnectorKey[a] = ropeController.attachmentConnectorKey[a];
	}
}

RopeController::~RopeController()
{
	delete[] vertexStorage;
	delete[] fieldStorage;
}

Controller *RopeController::Replicate(void) const
{
	return (new RopeController(*this));
}

bool RopeController::ValidNode(const Node *node)
{
	if (node->GetNodeType() == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<const Geometry *>(node);
		if ((geometry->GetGeometryType() == kGeometryPrimitive) && (static_cast<const PrimitiveGeometry *>(geometry)->GetPrimitiveType() == kPrimitiveRope))
		{
			return (true);
		}
	}

	return (false);
}

void RopeController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<BreakRopeAttachmentFunction> breakRopeAttachmentRegistration(registration, kFunctionBreakRopeAttachment, table->GetString(StringID('CTRL', kControllerRope, kFunctionBreakRopeAttachment)), kFunctionRemote);
}

void RopeController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	DeformableBodyController::Pack(data, packFlags);

	data << ChunkHeader('SPRG', 8);
	data << stretchSpringConstant;
	data << bendSpringConstant;

	data << ChunkHeader('DAMP', 8);
	data << stretchDamperConstant;
	data << bendDamperConstant;

	data << ChunkHeader('ATCH', 4);
	data << attachmentFlags;

	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		if (attachmentConnectorKey[a][0] != 0)
		{
			PackHandle handle = data.BeginChunk('KEY1' + a);
			data << attachmentConnectorKey[a];
			data.EndChunk(handle);
		}
	}

	if ((fieldStorage) && (!GetTargetNode()->GetManipulator()))
	{
		data << ChunkHeader('RNRM', sizeof(Vector3D));
		data << ropeNormal;

		data << ChunkHeader('FELD', 4 + particleCount * (sizeof(Point3D) * kRopePositionCount));
		data << particleCount;

		for (machine a = 0; a < kRopePositionCount; a++)
		{
			data.WriteArray(particleCount, ropePosition[a]);
		}
	}

	data << TerminatorChunk;
}

void RopeController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	DeformableBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<RopeController>(data, unpackFlags);
}

bool RopeController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 type = chunkHeader->chunkType;
	switch (type)
	{
		case 'SPRG':

			data >> stretchSpringConstant;
			data >> bendSpringConstant;
			return (true);

		case 'DAMP':

			data >> stretchDamperConstant;
			data >> bendDamperConstant;
			return (true);

		case 'ATCH':

			data >> attachmentFlags;
			return (true);

		case 'RNRM':

			data >> ropeNormal;
			return (true);

		case 'FELD':

			data >> particleCount;
			AllocateFieldStorage();

			for (machine a = 0; a < kRopePositionCount; a++)
			{
				data.ReadArray(particleCount, ropePosition[a]);
			}

			return (true);
	}

	unsigned_int32 index = type - 'KEY1';
	if (index < unsigned_int32(kRopeAttachmentCount))
	{
		data >> attachmentConnectorKey[index];
		return (true);
	}

	return (false);
}

void *RopeController::BeginSettingsUnpack(void)
{
	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		attachmentConnectorKey[a][0] = 0;
	}

	return (DeformableBodyController::BeginSettingsUnpack());
}

int32 RopeController::GetSettingCount(void) const
{
	return (DeformableBodyController::GetSettingCount() + 14 + kRopeAttachmentCount * 2);
}

Setting *RopeController::GetSetting(int32 index) const
{
	int32 i = index - DeformableBodyController::GetSettingCount();
	if (i >= 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (i == 0)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP'));
			return (new HeadingSetting('PROP', title));
		}

		if (i == 1)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'MASS'));
			return (new TextSetting('MASS', GetParticleMass(), title));
		}

		if (i == 2)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'VOLU'));
			return (new TextSetting('VOLU', GetVolumeMultiplier(), title));
		}

		if (i == 3)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'GRAV'));
			return (new TextSetting('GRAV', GetGravityMultiplier(), title));
		}

		if (i == 4)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'FLDG'));
			return (new FloatSetting('FLDG', GetFluidDragMultiplier(), title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 5)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'WNDG'));
			return (new FloatSetting('WNDG', GetWindDragMultiplier(), title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 6)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'IRES'));
			return (new FloatSetting('IRES', GetInternalResistance(), title, 0.0F, 1.0F, 0.01F));
		}

		if (i == 7)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'SPRG'));
			return (new HeadingSetting('SPRG', title));
		}

		if (i == 8)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'SPRG', 'STRS'));
			return (new FloatSetting('STRS', stretchSpringConstant * 0.001F, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 9)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'SPRG', 'BNDS'));
			return (new FloatSetting('BNDS', bendSpringConstant * 0.001F, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 10)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'DAMP'));
			return (new HeadingSetting('DAMP', title));
		}

		if (i == 11)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'DAMP', 'STRD'));
			return (new FloatSetting('STRD', stretchDamperConstant, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 12)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'DAMP', 'BNDD'));
			return (new FloatSetting('BNDD', bendDamperConstant, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 13)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'ATCH'));
			return (new HeadingSetting('ATCH', title));
		}

		i -= 14;
		if ((i >= 0) && (i < kRopeAttachmentCount))
		{
			Type identifier = 'END1' + i;
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'ATCH', identifier));
			return (new BooleanSetting(identifier, ((attachmentFlags & (1 << i)) != 0), title));
		}

		i -= kRopeAttachmentCount;
		if ((i >= 0) && (i < kRopeAttachmentCount))
		{
			Type identifier = 'KEY1' + i;
			const char *title = table->GetString(StringID('CTRL', kControllerRope, 'ATCH', identifier));
			return (new TextSetting(identifier, attachmentConnectorKey[i], title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
		}

		return (nullptr);
	}

	return (DeformableBodyController::GetSetting(index));
}

void RopeController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MASS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetParticleMass(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'VOLU')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetVolumeMultiplier(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'GRAV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetGravityMultiplier(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'FLDG')
	{
		SetFluidDragMultiplier(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'WNDG')
	{
		SetWindDragMultiplier(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'IRES')
	{
		SetInternalResistance(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'STRS')
	{
		stretchSpringConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 1000.0F;
	}
	else if (identifier == 'BNDS')
	{
		bendSpringConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 1000.0F;
	}
	else if (identifier == 'STRD')
	{
		stretchDamperConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'BNDD')
	{
		bendDamperConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else
	{
		unsigned_int32 index = identifier - 'END1';
		if (index < unsigned_int32(kRopeAttachmentCount))
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				attachmentFlags |= (1 << index);
			}
			else
			{
				attachmentFlags &= ~(1 << index);
			}
		}
		else
		{
			index = identifier - 'KEY1';
			if (index < unsigned_int32(kRopeAttachmentCount))
			{
				attachmentConnectorKey[index] = static_cast<const TextSetting *>(setting)->GetText();
			}
			else
			{
				DeformableBodyController::SetSetting(setting);
			}
		}
	}
}

void RopeController::AllocateFieldStorage(void)
{
	fieldStorage = new char[particleCount * (sizeof(SimdVector3D) * 4 + sizeof(Point3D) * kRopePositionCount + 4)];

	ropeVelocity = reinterpret_cast<SimdVector3D *>(fieldStorage);
	ropeBaseForce = ropeVelocity + particleCount;
	ropeFieldForce = ropeBaseForce + particleCount;
	ropeUpdatePosition = reinterpret_cast<SimdPoint3D *>(ropeFieldForce + particleCount);

	Point3D *position = reinterpret_cast<Point3D *>(ropeUpdatePosition + particleCount);
	for (machine a = 0; a < kRopePositionCount; a++)
	{
		ropePosition[a] = position;
		position += particleCount;
	}

	ropeFlexibility = reinterpret_cast<float *>(position);
}

void RopeController::InitFlexibility(void)
{
	float *flexibility = ropeFlexibility;
	for (machine a = 0; a < particleCount; a++)
	{
		flexibility[a] = 1.0F;
	}

	unsigned_int32 flags = attachmentFlags;

	if (flags & 1)
	{
		flexibility[0] = 0.0F;
	}

	if (flags & 2)
	{
		flexibility[particleCount - 1] = 0.0F;
	}
}

void RopeController::Preprocess(void)
{
	DeformableBodyController::Preprocess();

	RopeGeometry *ropeGeometry = GetTargetNode();
	ropeGeometry->SetShaderFlags(ropeGeometry->GetShaderFlags() | kShaderNormalizeBasisVectors);
	ropeGeometry->SetVertexBufferArrayFlags((1 << kArrayPosition) | (1 << kArrayVelocity) | (1 << kArrayNormal) | (1 << kArrayTangent));
	ropeGeometry->SetVertexAttributeArray(kArrayPosition, 0, 3);
	ropeGeometry->SetVertexAttributeArray(kArrayVelocity, sizeof(Point3D), 3);
	ropeGeometry->SetVertexAttributeArray(kArrayNormal, sizeof(Point3D) + sizeof(Vector3D), 3);
	ropeGeometry->SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) + sizeof(Vector3D) * 2, 4);

	RopeGeometryObject *object = ropeGeometry->GetObject();
	ropeUpdateJob.SetExecuteProc((object->GetRopeGeometryType() == kRopeGeometryTube) ? &JobUpdateRopeTube : &JobUpdateRopeCross);

	const Mesh *mesh = object->GetGeometryLevel(0);
	int32 vertexCount = mesh->GetVertexCount();

	int32 lowDetailVertexCount = 0;
	const Mesh *lowDetailMesh = nullptr;

	if (object->GetGeometryLevelCount() > 1)
	{
		lowDetailMesh = object->GetGeometryLevel(1);
		lowDetailVertexCount = lowDetailMesh->GetVertexCount();
	}

	ropeVertexBuffer.Establish(vertexCount * sizeof(RopeVertex));
	ropeGeometry->SetVertexBuffer(kVertexBufferAttributeArray1, &ropeVertexBuffer, sizeof(RopeVertex));

	particleCount = Min(Max(object->GetMaxSubdivX(), kMinRopeSubdivX), kMaxRopeSubdivX) + 1;

	stretchSpringDistance = object->GetRopeLength() / (float) (particleCount - 1);
	bendSpringDistance = stretchSpringDistance * 2.0F;

	float radius = object->GetRopeRadius();
	SetParticleVolume(radius * radius * K::tau_over_2 * (stretchSpringDistance * GetVolumeMultiplier()));
	SetParticleRadius(radius);

	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		if (attachmentFlags & (1 << a))
		{
			Node *node = ropeGeometry->GetConnectedNode(attachmentConnectorKey[a]);
			if (node)
			{
				// Do not allow an attachment to the root zone because StepSimulationJob() needs a super node.

				Node *super = node->GetSuperNode();
				if (super)
				{
					attachmentNode[a] = node;

					Controller *controller = super->GetController();
					if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
					{
						rigidBodyNode[a] = super;

						Body *body = static_cast<RigidBodyController *>(controller);
						const Contact *contact = GetFirstIncomingEdge();
						while (contact)
						{
							if (contact->GetStartElement() == body)
							{
								goto next;
							}

							contact = contact->GetNextIncomingEdge();
						}

						new DeformableContact(body, this);
					}
				}
			}
		}

		next:;
	}

	vertexStorage = new char[vertexCount * (sizeof(Point3D) + sizeof(Vector3D) * 2 + sizeof(Vector4D)) + lowDetailVertexCount * sizeof(Vector4D)];
	ropePositionArray = reinterpret_cast<Point3D *>(vertexStorage);
	ropeVelocityArray = ropePositionArray + vertexCount;
	ropeNormalArray = ropeVelocityArray + vertexCount;

	Vector4D *ropeTangent = reinterpret_cast<Vector4D *>(ropeNormalArray + vertexCount);
	ropeTangentArray[0] = ropeTangent;

	const Vector4D *meshTangent = mesh->GetArray<Vector4D>(kArrayTangent);
	for (machine a = 0; a < vertexCount; a++)
	{
		ropeTangent[a].w = meshTangent[a].w;
	}

	if (lowDetailMesh)
	{
		ropeTangent += vertexCount;
		ropeTangentArray[1] = ropeTangent;

		meshTangent = lowDetailMesh->GetArray<Vector4D>(kArrayTangent);
		for (machine a = 0; a < lowDetailVertexCount; a++)
		{
			ropeTangent[a].w = meshTangent[a].w;
		}
	}

	if (ropeGeometry->GetManipulator())
	{
		delete[] fieldStorage;
		fieldStorage = nullptr;
	}

	if (!fieldStorage)
	{
		AllocateFieldStorage();

		float dx = object->GetRopeLength() / (float) (particleCount - 1);
		Point3D *restrict initialPosition = ropePosition[kRopePositionInitial];
		Point3D *restrict finalPosition = ropePosition[kRopePositionFinal];
		Point3D *restrict previousPosition = ropePosition[kRopePositionPrevious];

		for (machine i = 0; i < particleCount; i++)
		{
			float x = (float) i * dx;
			initialPosition[i].Set(x, 0.0F, 0.0F);
			finalPosition[i].Set(x, 0.0F, 0.0F);
			previousPosition[i].Set(x, 0.0F, 0.0F);
		}

		ropeNormal.Set(0.0F, 1.0F, 0.0F);
	}

	InitFlexibility();

	ropeBoundingBox.Calculate(particleCount, ropePosition[kRopePositionInitial]);
	ropeBoundingBox.Expand(GetParticleRadius());

	int32 ydiv = Min(Max(object->GetMaxSubdivY(), kMinRopeSubdivY), kMaxRopeSubdivY);
	float dy = K::tau / (float) ydiv;

	for (machine j = 0; j < ydiv; j++)
	{
		ropeTrigTable[0][j] = CosSin((float) j * dy);
	}

	ydiv = Max(ydiv >> 1, kMinRopeSubdivY);
	dy = K::tau / (float) ydiv;

	for (machine j = 0; j < ydiv; j++)
	{
		ropeTrigTable[1][j] = CosSin((float) j * dy);
	}

	if ((GetPhysicsController()) && (!ropeGeometry->GetManipulator()))
	{
		WarmStart();
	}
}

void RopeController::Neutralize(void)
{
	ropeVertexBuffer.Establish(0);

	delete[] vertexStorage;
	vertexStorage = nullptr;

	delete[] fieldStorage;
	fieldStorage = nullptr;

	DeformableBodyController::Neutralize();
}

void RopeController::AutoSleep(void)
{
	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		const Node *node = attachmentNode[a];
		if (node)
		{
			const Node *rigidBody = rigidBodyNode[a];
			if ((!rigidBody) || (!rigidBody->GetController()->Asleep()))
			{
				ResetUpdateTime();
				return;
			}
		}
	}

	DeformableBodyController::AutoSleep();
}

void RopeController::WarmStart(void)
{
	int32 passCount = GetWarmStartTime() / kRopeTimeStep;
	SetWarmStartTime(0);

	if (passCount > 0)
	{
		for (machine a = 0; a < particleCount; a++)
		{
			ropeFieldForce[a].Set(0.0F, 0.0F, 0.0F);
		}

		Vector3D gravityForce = CalculateGravityForce();

		const Node *leftNode = attachmentNode[0];
		const Node *rightNode = attachmentNode[1];
		if ((leftNode) || (rightNode))
		{
			float dt = 1.0F / (float) passCount;

			Point3D leftStart = ropePosition[kRopePositionFinal][0];
			Point3D rightStart = ropePosition[kRopePositionFinal][particleCount - 1];

			for (machine a = 0; a < passCount; a++)
			{
				float t = (float) (a + 1) * dt;

				if (leftNode)
				{
					ropePosition[kRopePositionFinal][0] = leftStart * (1.0F - t) + GetTargetNode()->GetInverseWorldTransform() * leftNode->GetWorldPosition() * t;
				}

				if (rightNode)
				{
					ropePosition[kRopePositionFinal][particleCount - 1] = rightStart * (1.0F - t) + GetTargetNode()->GetInverseWorldTransform() * rightNode->GetWorldPosition() * t;
				}

				Simulate(1, gravityForce, Zero3D);
			}
		}
		else
		{
			Simulate(passCount, gravityForce, Zero3D);

			if (!Asleep())
			{
				SetDeformableBodyState(GetDeformableBodyState() | kDeformableBodyAutoAsleep);
				BodyController::Sleep();
			}
		}
	}
}

void RopeController::Simulate(int32 passCount, const Vector3D& gravityForce, const Vector3D& windVelocity, int32 threadIndex)
{
	FieldArray		fieldArray;

	QueryForceFields(fieldArray, threadIndex);

	RopeGeometry *ropeGeometry = GetTargetNode();

	Point3D *restrict finalPosition = ropePosition[kRopePositionFinal];
	Point3D *restrict previousPosition = ropePosition[kRopePositionPrevious];
	SimdVector3D *restrict velocity = ropeVelocity;
	SimdVector3D *restrict force = ropeBaseForce;

	#if C4SIMD

		vec_float gravity = VecLoadUnaligned(&gravityForce.x);
		vec_float wind = VecLoadUnaligned(&windVelocity.x);
		vec_float kw = VecLoadSmearScalar(&GetWindDragMultiplier());
		vec_float t2_over_2m = VecSmearX(VecMulScalar(VecLoadScalar(&GetHalfInverseParticleMass()), VecLoadScalar(&kRopeSquaredDeltaTime)));

		const vec_float one = VecLoadVectorConstant<0x3F800000>();
		vec_float velocityFactor = VecSmearX(VecMulScalar(VecSubScalar(one, VecLoadScalar(&GetInternalResistance())), VecLoadScalar(&kRopeDeltaTime)));
		vec_float inverseDeltaTime = VecLoadSmearScalar(&kRopeInverseDeltaTime);

	#else

		float kw = GetWindDragMultiplier();
		float t2_over_2m = GetHalfInverseParticleMass() * kRopeSquaredDeltaTime;
		float velocityFactor = (1.0F - GetInternalResistance()) * kRopeDeltaTime;

	#endif

	do
	{
		#if C4SIMD

			for (machine a = 0; a < particleCount; a++)
			{
				velocity[a] = VecMul(VecSub(VecLoadUnaligned(&finalPosition[a].x), VecLoadUnaligned(&previousPosition[a].x)), inverseDeltaTime);
				force[a] = VecMadd(VecSub(wind, velocity[a]), kw, gravity);
			}

			vec_float ks = VecLoadSmearScalar(&stretchSpringConstant);
			vec_float kd = VecLoadSmearScalar(&stretchDamperConstant);
			vec_float x = VecLoadSmearScalar(&stretchSpringDistance);

			for (machine a = 1; a < particleCount; a++)
			{
				vec_float p1 = VecLoadUnaligned(&finalPosition[a - 1].x);
				vec_float p2 = VecLoadUnaligned(&finalPosition[a].x);

				const vec_float& v1 = velocity[a - 1];
				const vec_float& v2 = velocity[a];

				vec_float dp = VecSub(p2, p1);
				vec_float f = VecMul(dp, VecMul(ks, (VecSub(one, VecMul(x, VecSmearX(VecInverseSqrtScalar(VecDot3D(dp, dp))))))));
				f = VecMadd(VecSub(v2, v1), kd, f);

				force[a - 1] = VecAdd(force[a - 1], f);
				force[a] = VecSub(force[a], f);
			}

			ks = VecLoadSmearScalar(&bendSpringConstant);
			kd = VecLoadSmearScalar(&bendDamperConstant);
			x = VecLoadSmearScalar(&bendSpringDistance);

			for (machine a = 2; a < particleCount; a++)
			{
				vec_float p1 = VecLoadUnaligned(&finalPosition[a - 2].x);
				vec_float p2 = VecLoadUnaligned(&finalPosition[a].x);

				const vec_float& v1 = velocity[a - 2];
				const vec_float& v2 = velocity[a];

				vec_float dp = VecSub(p2, p1);
				vec_float f = VecMul(dp, VecMul(ks, (VecSub(one, VecMul(x, VecSmearX(VecInverseSqrtScalar(VecDot3D(dp, dp))))))));
				f = VecMadd(VecSub(v2, v1), kd, f);

				force[a - 2] = VecAdd(force[a - 2], f);
				force[a] = VecSub(force[a], f);
			}

			ApplyForceFields(fieldArray, particleCount, finalPosition, velocity, ropeFieldForce);

			const float *flexibility = ropeFlexibility;
			for (machine a = 0; a < particleCount; a++)
			{
				vec_float f = VecAdd(force[a], ropeFieldForce[a]);
				vec_float p = VecLoadUnaligned(&finalPosition[a].x);
				vec_float q = VecMadd(VecMadd(velocity[a], velocityFactor, VecMul(f, t2_over_2m)), VecLoadSmearScalar(&flexibility[a]), p);
				VecStore3D(p, &previousPosition[a].x);
				VecStore3D(q, &finalPosition[a].x);
			}

		#else

			for (machine a = 0; a < particleCount; a++)
			{
				velocity[a] = (finalPosition[a] - previousPosition[a]) * kRopeInverseDeltaTime;
				force[a] = (windVelocity - velocity[a]) * kw + gravityForce;
			}

			float ks = stretchSpringConstant;
			float kd = stretchDamperConstant;
			float x = stretchSpringDistance;

			for (machine a = 1; a < particleCount; a++)
			{
				const Point3D& p1 = finalPosition[a - 1];
				const Point3D& p2 = finalPosition[a];

				const Vector3D& v1 = velocity[a - 1];
				const Vector3D& v2 = velocity[a];

				Vector3D dp = p2 - p1;
				Vector3D f = dp * (ks * (1.0F - x * InverseMag(dp))) + (v2 - v1) * kd;

				force[a - 1] += f;
				force[a] -= f;
			}

			ks = bendSpringConstant;
			kd = bendDamperConstant;
			x = bendSpringDistance;

			for (machine a = 2; a < particleCount; a++)
			{
				const Point3D& p1 = finalPosition[a - 2];
				const Point3D& p2 = finalPosition[a];

				const Vector3D& v1 = velocity[a - 2];
				const Vector3D& v2 = velocity[a];

				Vector3D dp = p2 - p1;
				Vector3D f = dp * (ks * (1.0F - x * InverseMag(dp))) + (v2 - v1) * kd;

				force[a - 2] += f;
				force[a] -= f;
			}

			ApplyForceFields(fieldArray, particleCount, finalPosition, velocity, ropeFieldForce);

			const float *flexibility = ropeFlexibility;
			for (machine a = 0; a < particleCount; a++)
			{
				Vector3D f = force[a] + ropeFieldForce[a];
				Point3D q = finalPosition[a] + (velocity[a] * velocityFactor + f * t2_over_2m) * flexibility[a];
				previousPosition[a] = finalPosition[a];
				finalPosition[a] = q;
			}

		#endif

		const Node *blocker = GetBlockerNode();
		if (blocker)
		{
			Transform4D transform = ropeGeometry->GetInverseWorldTransform() * blocker->GetWorldTransform();
			Transform4D inverseTransform = blocker->GetInverseWorldTransform() * ropeGeometry->GetWorldTransform();

			const BlockerObject *blockerObject = static_cast<const Blocker *>(blocker)->GetObject();
			blockerObject->ApplyBlocker(particleCount, GetParticleRadius(), finalPosition, previousPosition, transform, inverseTransform);
		}

		if (threadIndex < JobMgr::kMaxWorkerThreadCount)
		{
			bool mutexFlag = false;

			const Node *node = rigidBodyNode[0];
			if (node)
			{
				RigidBodyController *rigidBody = static_cast<RigidBodyController *>(node->GetController());
				if (!rigidBody->Asleep())
				{
					ropeMutex.Acquire();
					mutexFlag = true;

					Transform4D transform = node->GetInverseWorldTransform() * ropeGeometry->GetWorldTransform();
					rigidBody->ApplyImpulse(transform * force[0] * kRopeKilonewtonsDeltaTime, attachmentNode[0]->GetNodePosition());
				}
			}

			node = rigidBodyNode[1];
			if (node)
			{
				RigidBodyController *rigidBody = static_cast<RigidBodyController *>(node->GetController());
				if (!rigidBody->Asleep())
				{
					if (!mutexFlag)
					{
						ropeMutex.Acquire();
						mutexFlag = true;
					}

					Transform4D transform = node->GetInverseWorldTransform() * ropeGeometry->GetWorldTransform();
					rigidBody->ApplyImpulse(transform * force[particleCount - 1] * kRopeKilonewtonsDeltaTime, attachmentNode[1]->GetNodePosition());
				}
			}

			if (mutexFlag)
			{
				ropeMutex.Release();
			}
		}
	} while (--passCount > 0);

	Vector3D tangent = Normalize(finalPosition[1] - finalPosition[0]);
	ropeNormal = Normalize(ropeNormal - ProjectOnto(ropeNormal, tangent));

	ropeBoundingBox.Calculate(particleCount * 2, ropePosition[kRopePositionInitial]);
	ropeBoundingBox.Expand(GetParticleRadius());
	ropeGeometry->SetWorldBoundingBox(Transform(ropeBoundingBox, ropeGeometry->GetWorldTransform()));

	Invalidate();
}

void RopeController::JobStepSimulation(Job *job, void *cookie)
{
	RopeController *ropeController = static_cast<RopeController *>(cookie);

	for (machine a = 0; a < ropeController->particleCount; a++)
	{
		ropeController->ropePosition[kRopePositionInitial][a] = ropeController->ropePosition[kRopePositionFinal][a];
	}

	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		const Node *node = ropeController->attachmentNode[a];
		if (node)
		{
			int32 k = (a == 0) ? 0 : ropeController->particleCount - 1;

			const Node *rigidBody = ropeController->rigidBodyNode[a];
			if (rigidBody)
			{
				const Transform4D& rigidBodyTransform = static_cast<RigidBodyController *>(rigidBody->GetController())->GetFinalTransform();
				ropeController->ropePosition[kRopePositionFinal][k] = ropeController->GetTargetNode()->GetInverseWorldTransform() * (rigidBodyTransform * node->GetNodePosition());
			}
			else
			{
				const Node *super = node->GetSuperNode();
				Point3D position = super->GetNodeTransform() * node->GetNodePosition();
				for (;;)
				{
					super = super->GetSuperNode();
					if (!super)
					{
						break;
					}

					position = super->GetNodeTransform() * position;
				}

				ropeController->ropePosition[kRopePositionFinal][k] = ropeController->GetTargetNode()->GetInverseWorldTransform() * position;
			}
		}
	}

	Vector3D wind = ropeController->GetWindVelocity();

	const Node *field = ropeController->GetWindFieldNode();
	if ((field) && (field->Enabled()))
	{
		const Transform4D& inverseTransform = ropeController->GetTargetNode()->GetInverseWorldTransform();
		wind += inverseTransform * (field->GetWorldTransform() * static_cast<WindForce *>(static_cast<const Field *>(field)->GetForce())->GetWindVelocity());
	}

	ropeController->Simulate(kRopeStepRatio, ropeController->CalculateGravityForce(), wind, job->GetThreadIndex());
}

void RopeController::Update(void)
{
	DeformableBodyController::Update();

	if ((attachmentNode[0]) || (attachmentNode[1]))
	{
		Invalidate();
		GetTargetNode()->Invalidate();
	}

	PhysicsController *physicsController = GetPhysicsController();
	if (physicsController)
	{
		physicsController->IncrementPhysicsCounter(kPhysicsCounterDeformableBodyUpdate);
	}

	GetTargetNode()->GetWorld()->SubmitWorldJob(&ropeUpdateJob);
}

void RopeController::JobUpdateRopeTube(Job *job, void *cookie)
{
	RopeController *ropeController = static_cast<RopeController *>(cookie);

	const RopeGeometry *rope = ropeController->GetTargetNode();
	const RopeGeometryObject *object = rope->GetObject();

	Point3D *restrict positionArray = ropeController->ropePositionArray;
	Vector3D *restrict velocityArray = ropeController->ropeVelocityArray;
	Vector3D *restrict normalArray = ropeController->ropeNormalArray;
	Vector4D *restrict tangentArray = ropeController->ropeTangentArray[0];

	const Point3D *initialPosition = ropeController->ropePosition[kRopePositionInitial];
	const Point3D *finalPosition = ropeController->ropePosition[kRopePositionFinal];
	const Point3D *previousPosition = ropeController->ropePosition[kRopePositionPrevious];
	SimdPoint3D *restrict updatePosition = ropeController->ropeUpdatePosition;

	const PhysicsController *physicsController = ropeController->GetPhysicsController();
	float t = (physicsController) ? physicsController->GetInterpolationParam() : 1.0F;
	float s = 1.0F - t;

	int32 count = ropeController->particleCount;
	for (machine a = 0; a < count; a++)
	{
		updatePosition[a] = initialPosition[a] * s + finalPosition[a] * t;
	}

	const Vector2D *trigTable = ropeController->ropeTrigTable[0];

	int32 xdiv = Min(Max(object->GetMaxSubdivX(), kMinRopeSubdivX), kMaxRopeSubdivX);
	int32 ydiv = Min(Max(object->GetMaxSubdivY(), kMinRopeSubdivY), kMaxRopeSubdivY);

	int32 level = rope->GetDetailLevel();
	if (level < object->GetGeometryLevelCount() - 1)
	{
		#if C4SIMD

			const vec_float half = VecLoadVectorConstant<0x3F000000>();
			vec_float inverseTime = VecLoadSmearScalar(&kRopeInverseMotionDeltaTime);
			vec_float r = VecLoadSmearScalar(&ropeController->GetParticleRadius());

			vec_float p = updatePosition[0];
			vec_float tangent = VecSub(updatePosition[1], p);
			vec_float unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			vec_float normal = VecLoadUnaligned(&ropeController->ropeNormal.x);
			vec_float binormal = VecCross3D(unitTangent, normal);
			vec_float velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[0].x), VecLoadUnaligned(&previousPosition[0].x)), inverseTime);

			for (machine j = 0; j < ydiv; j++)
			{
				vec_float v = VecLoadUnaligned(&trigTable[j].x);
				vec_float w = VecMul(v, r);

				VecStore3D(VecAdd(p, VecMadd(normal, VecSmearX(w), VecMul(binormal, VecSmearY(w)))), &positionArray[j].x);
				VecStore3D(VecMadd(normal, VecSmearX(v), VecMul(binormal, VecSmearY(v))), &normalArray[j].x);
				VecStore3D(tangent, &tangentArray[j].x);
				VecStore3D(velocity, &velocityArray[j].x);
			}

			VecStore3D(VecLoadUnaligned(&positionArray[0].x), &positionArray[ydiv].x);
			VecStore3D(VecLoadUnaligned(&normalArray[0].x), &normalArray[ydiv].x);
			VecStore3D(tangent, &tangentArray[ydiv].x);
			VecStore3D(velocity, &velocityArray[ydiv].x);

			int32 index = (ydiv + 1) * 2;

			for (machine i = 1; i < xdiv; i++)
			{
				p = updatePosition[i];
				tangent = VecMul(VecSub(updatePosition[i + 1], updatePosition[i - 1]), half);
				unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
				normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
				normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
				binormal = VecCross3D(unitTangent, normal);
				velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[i].x), VecLoadUnaligned(&previousPosition[i].x)), inverseTime);

				for (machine j = 0; j < ydiv; j++)
				{
					vec_float v = VecLoadUnaligned(&trigTable[j].x);
					vec_float w = VecMul(v, r);

					VecStore3D(VecAdd(p, VecMadd(normal, VecSmearX(w), VecMul(binormal, VecSmearY(w)))), &positionArray[index].x);
					VecStore3D(VecMadd(normal, VecSmearX(v), VecMul(binormal, VecSmearY(v))), &normalArray[index].x);
					VecStore3D(tangent, &tangentArray[index].x);
					VecStore3D(velocity, &velocityArray[index].x);
					index++;
				}

				VecStore3D(VecLoadUnaligned(&positionArray[index - ydiv].x), &positionArray[index].x);
				VecStore3D(VecLoadUnaligned(&normalArray[index - ydiv].x), &normalArray[index].x);
				VecStore3D(tangent, &tangentArray[index].x);
				VecStore3D(velocity, &velocityArray[index].x);

				index += ydiv + 2;
			}

			p = updatePosition[xdiv];
			tangent = VecSub(updatePosition[xdiv], updatePosition[xdiv - 1]);
			unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
			normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
			binormal = VecCross3D(unitTangent, normal);
			velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[xdiv].x), VecLoadUnaligned(&previousPosition[xdiv].x)), inverseTime);

			for (machine j = 0; j < ydiv; j++)
			{
				vec_float v = VecLoadUnaligned(&trigTable[j].x);
				vec_float w = VecMul(v, r);

				VecStore3D(VecAdd(p, VecMadd(normal, VecSmearX(w), VecMul(binormal, VecSmearY(w)))), &positionArray[index].x);
				VecStore3D(VecMadd(normal, VecSmearX(v), VecMul(binormal, VecSmearY(v))), &normalArray[index].x);
				VecStore3D(tangent, &tangentArray[index].x);
				VecStore3D(velocity, &velocityArray[index].x);
				index++;
			}

			VecStore3D(VecLoadUnaligned(&positionArray[index - ydiv].x), &positionArray[index].x);
			VecStore3D(VecLoadUnaligned(&normalArray[index - ydiv].x), &normalArray[index].x);
			VecStore3D(tangent, &tangentArray[index].x);
			VecStore3D(velocity, &velocityArray[index].x);

			const vec_float quarter = VecLoadVectorConstant<0x3E800000>();
			const vec_float eighth = VecLoadVectorConstant<0x3E000000>();
			const vec_float three_halves = VecLoadVectorConstant<0x3FC00000>();

			int32 k = (ydiv + 1) * 2;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 ik = i * k;

				vec_float p1 = updatePosition[i];
				vec_float p2 = updatePosition[i + 1];
				vec_float v1 = VecLoadUnaligned(&velocityArray[ik].x);
				vec_float v2 = VecLoadUnaligned(&velocityArray[ik + k].x);
				vec_float t1 = VecLoadUnaligned(&tangentArray[ik].x);
				vec_float t2 = VecLoadUnaligned(&tangentArray[ik + k].x);
				vec_float n1 = VecLoadUnaligned(&normalArray[ik].x);

				vec_float m = ((unsigned_int32) (i - 1) < (unsigned_int32) (xdiv - 2)) ? eighth : quarter;

				vec_float center = VecMadd(VecAdd(p1, p2), half, VecMul(VecSub(t1, t2), m));
				tangent = VecNmsub(VecAdd(t1, t2), quarter, VecMul(VecSub(p2, p1), three_halves));
				tangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
				normal = VecSub(n1, VecProjectOnto3D(n1, tangent));
				normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
				binormal = VecCross3D(tangent, normal);
				velocity = VecMul(VecAdd(v1, v2), half);

				index = ik + ydiv + 1;

				for (machine j = 0; j < ydiv; j++)
				{
					vec_float v = VecLoadUnaligned(&trigTable[j].x);
					vec_float w = VecMul(v, r);

					VecStore3D(VecAdd(center, VecMadd(normal, VecSmearX(w), VecMul(binormal, VecSmearY(w)))), &positionArray[index].x);
					VecStore3D(VecMadd(normal, VecSmearX(v), VecMul(binormal, VecSmearY(v))), &normalArray[index].x);
					VecStore3D(tangent, &tangentArray[index].x);
					VecStore3D(velocity, &velocityArray[index].x);
					index++;
				}

				VecStore3D(VecLoadUnaligned(&positionArray[index - ydiv].x), &positionArray[index].x);
				VecStore3D(VecLoadUnaligned(&normalArray[index - ydiv].x), &normalArray[index].x);
				VecStore3D(tangent, &tangentArray[index].x);
				VecStore3D(velocity, &velocityArray[index].x);
			}

		#else

			float r = ropeController->GetParticleRadius();

			Vector3D tangent = updatePosition[1] - updatePosition[0];
			Vector3D unitTangent = tangent * InverseMag(tangent);
			Vector3D normal = ropeController->ropeNormal;
			Vector3D binormal = unitTangent % normal;
			Vector3D velocity = (finalPosition[0] - previousPosition[0]) * kRopeInverseMotionDeltaTime;

			for (machine j = 0; j < ydiv; j++)
			{
				Vector2D v = trigTable[j];
				float x = v.x * r;
				float y = v.y * r;

				positionArray[j] = updatePosition[0] + normal * x + binormal * y;
				normalArray[j] = normal * v.x + binormal * v.y;
				tangentArray[j].GetVector3D() = tangent;
				velocityArray[j] = velocity;
			}

			positionArray[ydiv] = positionArray[0];
			normalArray[ydiv] = normalArray[0];
			tangentArray[ydiv].GetVector3D() = tangent;
			velocityArray[ydiv] = velocity;

			int32 index = (ydiv + 1) * 2;

			for (machine i = 1; i < xdiv; i++)
			{
				tangent = (updatePosition[i + 1] - updatePosition[i - 1]) * 0.5F;
				unitTangent = tangent * InverseMag(tangent);
				normal = Normalize(normal - ProjectOnto(normal, unitTangent));
				binormal = unitTangent % normal;
				velocity = (finalPosition[i] - previousPosition[i]) * kRopeInverseMotionDeltaTime;

				for (machine j = 0; j < ydiv; j++)
				{
					Vector2D v = trigTable[j];
					float x = v.x * r;
					float y = v.y * r;

					positionArray[index] = updatePosition[i] + normal * x + binormal * y;
					normalArray[index] = normal * v.x + binormal * v.y;
					tangentArray[index].GetVector3D() = tangent;
					velocityArray[index] = velocity;
					index++;
				}

				positionArray[index] = positionArray[index - ydiv];
				normalArray[index] = normalArray[index - ydiv];
				tangentArray[index].GetVector3D() = tangent;
				velocityArray[index] = velocity;

				index += ydiv + 2;
			}

			tangent = updatePosition[xdiv] - updatePosition[xdiv - 1];
			unitTangent = tangent * InverseMag(tangent);
			normal = Normalize(normal - ProjectOnto(normal, tangent));
			binormal = unitTangent % normal;
			velocity = (finalPosition[xdiv] - previousPosition[xdiv]) * kRopeInverseMotionDeltaTime;

			for (machine j = 0; j < ydiv; j++)
			{
				Vector2D v = trigTable[j];
				float x = v.x * r;
				float y = v.y * r;

				positionArray[index] = updatePosition[xdiv] + normal * x + binormal * y;
				normalArray[index] = normal * v.x + binormal * v.y;
				tangentArray[index].GetVector3D() = tangent;
				velocityArray[index] = velocity;
				index++;
			}

			positionArray[index] = positionArray[index - ydiv];
			normalArray[index] = normalArray[index - ydiv];
			tangentArray[index].GetVector3D() = tangent;
			velocityArray[index] = velocity;

			int32 k = (ydiv + 1) * 2;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 ik = i * k;

				const Point3D& p1 = updatePosition[i];
				const Point3D& p2 = updatePosition[i + 1];
				const Vector3D& v1 = velocityArray[ik];
				const Vector3D& v2 = velocityArray[ik + k];
				const Vector3D& t1 = tangentArray[ik].GetVector3D();
				const Vector3D& t2 = tangentArray[ik + k].GetVector3D();
				const Vector3D& n1 = normalArray[ik];

				float m = ((unsigned_int32) (i - 1) < (unsigned_int32) (xdiv - 2)) ? 0.125F : 0.25F;

				Point3D center = (p1 + p2) * 0.5F + (t1 - t2) * m;
				tangent = Normalize((p2 - p1) * 1.5F - (t1 + t2) * 0.25F);
				normal = Normalize(n1 - ProjectOnto(n1, tangent));
				binormal = tangent % normal;
				velocity = (v1 + v2) * 0.5F;

				index = ik + ydiv + 1;

				for (machine j = 0; j < ydiv; j++)
				{
					Vector2D v = trigTable[j];
					float x = v.x * r;
					float y = v.y * r;

					positionArray[index] = center + normal * x + binormal * y;
					normalArray[index] = normal * v.x + binormal * v.y;
					tangentArray[index].GetVector3D() = tangent;
					velocityArray[index] = velocity;
					index++;
				}

				positionArray[index] = positionArray[index - ydiv];
				normalArray[index] = normalArray[index - ydiv];
				tangentArray[index].GetVector3D() = tangent;
				velocityArray[index] = velocity;
			}

		#endif

		if (object->GetPrimitiveFlags() & kPrimitiveEndcap)
		{
			index = (xdiv * 2 + 1) * (ydiv + 1);

			Vector3D endNormal = -tangentArray[0].GetVector3D();
			Vector3D endTangent = updatePosition[0] - positionArray[0];
			const Vector3D *endVelocity = &velocityArray[0];

			for (machine j = ydiv; j > 0; j--)
			{
				positionArray[index] = positionArray[j];
				normalArray[index] = endNormal;
				tangentArray[index].GetVector3D() = endTangent;
				velocityArray[index] = *endVelocity;
				index++;
			}

			int32 base = xdiv * 2 * (ydiv + 1);

			endNormal = tangentArray[base].GetVector3D();
			endTangent = positionArray[base] - updatePosition[xdiv];
			endVelocity = &velocityArray[base];

			for (machine j = 0; j < ydiv; j++)
			{
				positionArray[index] = positionArray[base];
				normalArray[index] = endNormal;
				tangentArray[index].GetVector3D() = endTangent;
				velocityArray[index] = *endVelocity;
				index++;
				base++;
			}
		}
	}
	else
	{
		if (level > 0)
		{
			tangentArray = ropeController->ropeTangentArray[1];

			ydiv = Max(ydiv >> 1, kMinRopeSubdivY);
			trigTable += kMaxRopeSubdivY;
		}

		#if C4SIMD

			vec_float inverseTime = VecLoadSmearScalar(&kRopeInverseMotionDeltaTime);
			vec_float r = VecLoadSmearScalar(&ropeController->GetParticleRadius());

			vec_float p = updatePosition[0];
			vec_float tangent = VecSub(updatePosition[1], p);
			vec_float unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			vec_float normal = VecLoadUnaligned(&ropeController->ropeNormal.x);
			vec_float binormal = VecCross3D(unitTangent, normal);
			vec_float velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[0].x), VecLoadUnaligned(&previousPosition[0].x)), inverseTime);

			for (machine j = 0; j < ydiv; j++)
			{
				vec_float v = VecLoadUnaligned(&trigTable[j].x);
				vec_float w = VecMul(v, r);

				VecStore3D(VecAdd(p, VecMadd(normal, VecSmearX(w), VecMul(binormal, VecSmearY(w)))), &positionArray[j].x);
				VecStore3D(VecMadd(normal, VecSmearX(v), VecMul(binormal, VecSmearY(v))), &normalArray[j].x);
				VecStore3D(tangent, &tangentArray[j].x);
				VecStore3D(velocity, &velocityArray[j].x);
			}

			VecStore3D(VecLoadUnaligned(&positionArray[0].x), &positionArray[ydiv].x);
			VecStore3D(VecLoadUnaligned(&normalArray[0].x), &normalArray[ydiv].x);
			VecStore3D(tangent, &tangentArray[ydiv].x);
			VecStore3D(velocity, &velocityArray[ydiv].x);

			int32 index = ydiv + 1;

			for (machine i = 1; i < xdiv; i++)
			{
				p = updatePosition[i];
				tangent = VecSub(updatePosition[i + 1], updatePosition[i - 1]);
				unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
				normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
				normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
				binormal = VecCross3D(unitTangent, normal);
				velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[i].x), VecLoadUnaligned(&previousPosition[i].x)), inverseTime);

				for (machine j = 0; j < ydiv; j++)
				{
					vec_float v = VecLoadUnaligned(&trigTable[j].x);
					vec_float w = VecMul(v, r);

					VecStore3D(VecAdd(p, VecMadd(normal, VecSmearX(w), VecMul(binormal, VecSmearY(w)))), &positionArray[index].x);
					VecStore3D(VecMadd(normal, VecSmearX(v), VecMul(binormal, VecSmearY(v))), &normalArray[index].x);
					VecStore3D(tangent, &tangentArray[index].x);
					VecStore3D(velocity, &velocityArray[index].x);
					index++;
				}

				VecStore3D(VecLoadUnaligned(&positionArray[index - ydiv].x), &positionArray[index].x);
				VecStore3D(VecLoadUnaligned(&normalArray[index - ydiv].x), &normalArray[index].x);
				VecStore3D(tangent, &tangentArray[index].x);
				VecStore3D(velocity, &velocityArray[index].x);
				index++;
			}

			p = updatePosition[xdiv];
			tangent = VecSub(updatePosition[xdiv], updatePosition[xdiv - 1]);
			unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
			normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
			binormal = VecCross3D(unitTangent, normal);
			velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[xdiv].x), VecLoadUnaligned(&previousPosition[xdiv].x)), inverseTime);

			for (machine j = 0; j < ydiv; j++)
			{
				vec_float v = VecLoadUnaligned(&trigTable[j].x);
				vec_float w = VecMul(v, r);

				VecStore3D(VecAdd(p, VecMadd(normal, VecSmearX(w), VecMul(binormal, VecSmearY(w)))), &positionArray[index].x);
				VecStore3D(VecMadd(normal, VecSmearX(v), VecMul(binormal, VecSmearY(v))), &normalArray[index].x);
				VecStore3D(tangent, &tangentArray[index].x);
				VecStore3D(velocity, &velocityArray[index].x);
				index++;
			}

			VecStore3D(VecLoadUnaligned(&positionArray[index - ydiv].x), &positionArray[index].x);
			VecStore3D(VecLoadUnaligned(&normalArray[index - ydiv].x), &normalArray[index].x);
			VecStore3D(tangent, &tangentArray[index].x);
			VecStore3D(velocity, &velocityArray[index].x);

		#else

			float r = ropeController->GetParticleRadius();

			Vector3D tangent = updatePosition[1] - updatePosition[0];
			Vector3D unitTangent = tangent * InverseMag(tangent);
			Vector3D normal = ropeController->ropeNormal;
			Vector3D binormal = unitTangent % normal;
			Vector3D velocity = (finalPosition[0] - previousPosition[0]) * kRopeInverseMotionDeltaTime;

			for (machine j = 0; j < ydiv; j++)
			{
				Vector2D v = trigTable[j];
				float x = v.x * r;
				float y = v.y * r;

				positionArray[j] = updatePosition[0] + normal * x + binormal * y;
				normalArray[j] = normal * v.x + binormal * v.y;
				tangentArray[j].GetVector3D() = tangent;
				velocityArray[j] = velocity;
			}

			positionArray[ydiv] = positionArray[0];
			normalArray[ydiv] = normalArray[0];
			tangentArray[ydiv].GetVector3D() = tangent;
			velocityArray[ydiv] = velocity;

			int32 index = ydiv + 1;

			for (machine i = 1; i < xdiv; i++)
			{
				tangent = updatePosition[i + 1] - updatePosition[i - 1];
				unitTangent = tangent * InverseMag(tangent);
				normal = Normalize(normal - ProjectOnto(normal, unitTangent));
				binormal = unitTangent % normal;
				velocity = (finalPosition[i] - previousPosition[i]) * kRopeInverseMotionDeltaTime;

				for (machine j = 0; j < ydiv; j++)
				{
					Vector2D v = trigTable[j];
					float x = v.x * r;
					float y = v.y * r;

					positionArray[index] = updatePosition[i] + normal * x + binormal * y;
					normalArray[index] = normal * v.x + binormal * v.y;
					tangentArray[index].GetVector3D() = tangent;
					velocityArray[index] = velocity;
					index++;
				}

				positionArray[index] = positionArray[index - ydiv];
				normalArray[index] = normalArray[index - ydiv];
				tangentArray[index].GetVector3D() = tangent;
				velocityArray[index] = velocity;
				index++;
			}

			tangent = updatePosition[xdiv] - updatePosition[xdiv - 1];
			unitTangent = tangent * InverseMag(tangent);
			normal = Normalize(normal - ProjectOnto(normal, unitTangent));
			binormal = unitTangent % normal;
			velocity = (finalPosition[xdiv] - previousPosition[xdiv]) * kRopeInverseMotionDeltaTime;

			for (machine j = 0; j < ydiv; j++)
			{
				Vector2D v = trigTable[j];
				float x = v.x * r;
				float y = v.y * r;

				positionArray[index] = updatePosition[xdiv] + normal * x + binormal * y;
				normalArray[index] = normal * v.x + binormal * v.y;
				tangentArray[index].GetVector3D() = tangent;
				velocityArray[index] = velocity;
				index++;
			}

			positionArray[index] = positionArray[index - ydiv];
			normalArray[index] = normalArray[index - ydiv];
			tangentArray[index].GetVector3D() = tangent;
			velocityArray[index] = velocity;

		#endif

		if (object->GetPrimitiveFlags() & kPrimitiveEndcap)
		{
			index++;

			Vector3D endNormal = -tangentArray[0].GetVector3D();
			Vector3D endTangent = updatePosition[0] - positionArray[0];
			const Vector3D *endVelocity = &velocityArray[0];

			for (machine j = ydiv; j > 0; j--)
			{
				positionArray[index] = positionArray[j];
				normalArray[index] = endNormal;
				tangentArray[index].GetVector3D() = endTangent;
				velocityArray[index] = *endVelocity;
				index++;
			}

			int32 base = xdiv * (ydiv + 1);

			endNormal = tangentArray[base].GetVector3D();
			endTangent = positionArray[base] - updatePosition[xdiv];
			endVelocity = &velocityArray[base];

			for (machine j = 0; j < ydiv; j++)
			{
				positionArray[index] = positionArray[base];
				normalArray[index] = endNormal;
				tangentArray[index].GetVector3D() = endTangent;
				velocityArray[index] = *endVelocity;
				index++;
				base++;
			}
		}
	}
}

void RopeController::JobUpdateRopeCross(Job *job, void *cookie)
{
	RopeController *ropeController = static_cast<RopeController *>(cookie);

	const RopeGeometry *rope = ropeController->GetTargetNode();
	const RopeGeometryObject *object = rope->GetObject();

	Point3D *restrict positionArray = ropeController->ropePositionArray;
	Vector3D *restrict velocityArray = ropeController->ropeVelocityArray;
	Vector3D *restrict normalArray = ropeController->ropeNormalArray;
	Vector4D *restrict tangentArray = ropeController->ropeTangentArray[0];

	const Point3D *initialPosition = ropeController->ropePosition[kRopePositionInitial];
	const Point3D *finalPosition = ropeController->ropePosition[kRopePositionFinal];
	const Point3D *previousPosition = ropeController->ropePosition[kRopePositionPrevious];
	SimdPoint3D *restrict updatePosition = ropeController->ropeUpdatePosition;

	const PhysicsController *physicsController = ropeController->GetPhysicsController();
	float t = (physicsController) ? physicsController->GetInterpolationParam() : 1.0F;
	float s = 1.0F - t;

	int32 count = ropeController->particleCount;
	for (machine a = 0; a < count; a++)
	{
		updatePosition[a] = initialPosition[a] * s + finalPosition[a] * t;
	}

	int32 xdiv = Min(Max(object->GetMaxSubdivX(), kMinRopeSubdivX), kMaxRopeSubdivX);

	int32 level = rope->GetDetailLevel();
	if (level < object->GetGeometryLevelCount() - 1)
	{
		#if C4SIMD

			const vec_float half = VecLoadVectorConstant<0x3F000000>();
			vec_float inverseTime = VecLoadSmearScalar(&kRopeInverseMotionDeltaTime);
			vec_float r = VecLoadSmearScalar(&ropeController->GetParticleRadius());

			vec_float p = updatePosition[0];
			vec_float tangent = VecSub(updatePosition[1], p);
			vec_float unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			vec_float normal = VecLoadUnaligned(&ropeController->ropeNormal.x);
			vec_float binormal = VecCross3D(unitTangent, normal);
			vec_float velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[0].x), VecLoadUnaligned(&previousPosition[0].x)), inverseTime);

			int32 index2 = xdiv * 4 + 2;

			vec_float b = VecMul(binormal, r);
			vec_float n = VecMul(normal, r);

			VecStore3D(VecAdd(p, b), &positionArray[0].x);
			VecStore3D(VecSub(p, b), &positionArray[1].x);
			VecStore3D(VecSub(p, n), &positionArray[index2].x);
			VecStore3D(VecAdd(p, n), &positionArray[index2 + 1].x);
			VecStore3D(normal, &normalArray[0].x);
			VecStore3D(normal, &normalArray[1].x);
			VecStore3D(binormal, &normalArray[index2].x);
			VecStore3D(binormal, &normalArray[index2 + 1].x);
			VecStore3D(tangent, &tangentArray[0].x);
			VecStore3D(tangent, &tangentArray[1].x);
			VecStore3D(tangent, &tangentArray[index2].x);
			VecStore3D(tangent, &tangentArray[index2 + 1].x);
			VecStore3D(velocity, &velocityArray[0].x);
			VecStore3D(velocity, &velocityArray[1].x);
			VecStore3D(velocity, &velocityArray[index2].x);
			VecStore3D(velocity, &velocityArray[index2 + 1].x);

			int32 index1 = 4;
			index2 += 4;

			for (machine i = 1; i < xdiv; i++)
			{
				p = updatePosition[i];
				tangent = VecMul(VecSub(updatePosition[i + 1], updatePosition[i - 1]), half);
				unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
				normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
				normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
				binormal = VecCross3D(unitTangent, normal);
				velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[i].x), VecLoadUnaligned(&previousPosition[i].x)), inverseTime);

				b = VecMul(binormal, r);
				n = VecMul(normal, r);

				VecStore3D(VecAdd(p, b), &positionArray[index1].x);
				VecStore3D(VecSub(p, b), &positionArray[index1 + 1].x);
				VecStore3D(VecSub(p, n), &positionArray[index2].x);
				VecStore3D(VecAdd(p, n), &positionArray[index2 + 1].x);
				VecStore3D(normal, &normalArray[index1].x);
				VecStore3D(normal, &normalArray[index1 + 1].x);
				VecStore3D(binormal, &normalArray[index2].x);
				VecStore3D(binormal, &normalArray[index2 + 1].x);
				VecStore3D(tangent, &tangentArray[index1].x);
				VecStore3D(tangent, &tangentArray[index1 + 1].x);
				VecStore3D(tangent, &tangentArray[index2].x);
				VecStore3D(tangent, &tangentArray[index2 + 1].x);
				VecStore3D(velocity, &velocityArray[index1].x);
				VecStore3D(velocity, &velocityArray[index1 + 1].x);
				VecStore3D(velocity, &velocityArray[index2].x);
				VecStore3D(velocity, &velocityArray[index2 + 1].x);

				index1 += 4;
				index2 += 4;
			}

			p = updatePosition[xdiv];
			tangent = VecSub(p, updatePosition[xdiv - 1]);
			unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
			normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
			binormal = VecCross3D(unitTangent, normal);
			velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[xdiv].x), VecLoadUnaligned(&previousPosition[xdiv].x)), inverseTime);

			b = VecMul(binormal, r);
			n = VecMul(normal, r);

			VecStore3D(VecAdd(p, b), &positionArray[index1].x);
			VecStore3D(VecSub(p, b), &positionArray[index1 + 1].x);
			VecStore3D(VecSub(p, n), &positionArray[index2].x);
			VecStore3D(VecAdd(p, n), &positionArray[index2 + 1].x);
			VecStore3D(normal, &normalArray[index1].x);
			VecStore3D(normal, &normalArray[index1 + 1].x);
			VecStore3D(binormal, &normalArray[index2].x);
			VecStore3D(binormal, &normalArray[index2 + 1].x);
			VecStore3D(tangent, &tangentArray[index1].x);
			VecStore3D(tangent, &tangentArray[index1 + 1].x);
			VecStore3D(tangent, &tangentArray[index2].x);
			VecStore3D(tangent, &tangentArray[index2 + 1].x);
			VecStore3D(velocity, &velocityArray[index1].x);
			VecStore3D(velocity, &velocityArray[index1 + 1].x);
			VecStore3D(velocity, &velocityArray[index2].x);
			VecStore3D(velocity, &velocityArray[index2 + 1].x);

			const vec_float quarter = VecLoadVectorConstant<0x3E800000>();
			const vec_float eighth = VecLoadVectorConstant<0x3E000000>();
			const vec_float three_halves = VecLoadVectorConstant<0x3FC00000>();

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = i * 4;

				vec_float p1 = updatePosition[i];
				vec_float p2 = updatePosition[i + 1];
				vec_float v1 = VecLoadUnaligned(&velocityArray[k].x);
				vec_float v2 = VecLoadUnaligned(&velocityArray[k + 4].x);
				vec_float t1 = VecLoadUnaligned(&tangentArray[k].x);
				vec_float t2 = VecLoadUnaligned(&tangentArray[k + 4].x);
				vec_float n1 = VecLoadUnaligned(&normalArray[k].x);

				vec_float m = ((unsigned_int32) (i - 1) < (unsigned_int32) (xdiv - 2)) ? eighth : quarter;

				vec_float center = VecMadd(VecAdd(p1, p2), half, VecMul(VecSub(t1, t2), m));
				tangent = VecNmsub(VecAdd(t1, t2), quarter, VecMul(VecSub(p2, p1), three_halves));
				tangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
				normal = VecSub(n1, VecProjectOnto3D(n1, tangent));
				normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
				binormal = VecCross3D(tangent, normal);
				velocity = VecMul(VecAdd(v1, v2), half);

				index1 = k + 2;
				index2 = k + xdiv * 4 + 4;

				b = VecMul(binormal, r);
				n = VecMul(normal, r);

				VecStore3D(VecAdd(center, b), &positionArray[index1].x);
				VecStore3D(VecSub(center, b), &positionArray[index1 + 1].x);
				VecStore3D(VecSub(center, n), &positionArray[index2].x);
				VecStore3D(VecAdd(center, n), &positionArray[index2 + 1].x);
				VecStore3D(normal, &normalArray[index1].x);
				VecStore3D(normal, &normalArray[index1 + 1].x);
				VecStore3D(binormal, &normalArray[index2].x);
				VecStore3D(binormal, &normalArray[index2 + 1].x);
				VecStore3D(tangent, &tangentArray[index1].x);
				VecStore3D(tangent, &tangentArray[index1 + 1].x);
				VecStore3D(tangent, &tangentArray[index2].x);
				VecStore3D(tangent, &tangentArray[index2 + 1].x);
				VecStore3D(velocity, &velocityArray[index1].x);
				VecStore3D(velocity, &velocityArray[index1 + 1].x);
				VecStore3D(velocity, &velocityArray[index2].x);
				VecStore3D(velocity, &velocityArray[index2 + 1].x);
			}

		#else

			float r = ropeController->GetParticleRadius();

			Vector3D tangent = updatePosition[1] - updatePosition[0];
			Vector3D unitTangent = tangent * InverseMag(tangent);
			Vector3D normal = ropeController->ropeNormal;
			Vector3D binormal = unitTangent % normal;
			Vector3D velocity = (finalPosition[0] - previousPosition[0]) * kRopeInverseMotionDeltaTime;

			int32 index2 = xdiv * 4 + 2;

			Vector3D b = binormal * r;
			Vector3D n = normal * r;

			positionArray[0] = updatePosition[0] + b;
			positionArray[1] = updatePosition[0] - b;
			positionArray[index2] = updatePosition[0] - n;
			positionArray[index2 + 1] = updatePosition[0] + n;
			normalArray[0] = normal;
			normalArray[1] = normal;
			normalArray[index2] = binormal;
			normalArray[index2 + 1] = binormal;
			tangentArray[0].GetVector3D() = tangent;
			tangentArray[1].GetVector3D() = tangent;
			tangentArray[index2].GetVector3D() = tangent;
			tangentArray[index2 + 1].GetVector3D() = tangent;
			velocityArray[0] = velocity;
			velocityArray[1] = velocity;
			velocityArray[index2] = velocity;
			velocityArray[index2 + 1] = velocity;

			int32 index1 = 4;
			index2 += 4;

			for (machine i = 1; i < xdiv; i++)
			{
				tangent = (updatePosition[i + 1] - updatePosition[i - 1]) * 0.5F;
				unitTangent = tangent * InverseMag(tangent);
				normal = Normalize(normal - ProjectOnto(normal, unitTangent));
				binormal = unitTangent % normal;
				velocity = (finalPosition[i] - previousPosition[i]) * kRopeInverseMotionDeltaTime;

				b = binormal * r;
				n = normal * r;

				positionArray[index1] = updatePosition[i] + b;
				positionArray[index1 + 1] = updatePosition[i] - b;
				positionArray[index2] = updatePosition[i] - n;
				positionArray[index2 + 1] = updatePosition[i] + n;
				normalArray[index1] = normal;
				normalArray[index1 + 1] = normal;
				normalArray[index2] = binormal;
				normalArray[index2 + 1] = binormal;
				tangentArray[index1].GetVector3D() = tangent;
				tangentArray[index1 + 1].GetVector3D() = tangent;
				tangentArray[index2].GetVector3D() = tangent;
				tangentArray[index2 + 1].GetVector3D() = tangent;
				velocityArray[index1] = velocity;
				velocityArray[index1 + 1] = velocity;
				velocityArray[index2] = velocity;
				velocityArray[index2 + 1] = velocity;

				index1 += 4;
				index2 += 4;
			}

			tangent = updatePosition[xdiv] - updatePosition[xdiv - 1];
			unitTangent = tangent * InverseMag(tangent);
			normal = Normalize(normal - ProjectOnto(normal, unitTangent));
			binormal = unitTangent % normal;
			velocity = (finalPosition[xdiv] - previousPosition[xdiv]) * kRopeInverseMotionDeltaTime;

			b = binormal * r;
			n = normal * r;

			positionArray[index1] = updatePosition[xdiv] + b;
			positionArray[index1 + 1] = updatePosition[xdiv] - b;
			positionArray[index2] = updatePosition[xdiv] - n;
			positionArray[index2 + 1] = updatePosition[xdiv] + n;
			normalArray[index1] = normal;
			normalArray[index1 + 1] = normal;
			normalArray[index2] = binormal;
			normalArray[index2 + 1] = binormal;
			tangentArray[index1].GetVector3D() = tangent;
			tangentArray[index1 + 1].GetVector3D() = tangent;
			tangentArray[index2].GetVector3D() = tangent;
			tangentArray[index2 + 1].GetVector3D() = tangent;
			velocityArray[index1] = velocity;
			velocityArray[index1 + 1] = velocity;
			velocityArray[index2] = velocity;
			velocityArray[index2 + 1] = velocity;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = i * 4;

				const Point3D& p1 = updatePosition[i];
				const Point3D& p2 = updatePosition[i + 1];
				const Vector3D& v1 = velocityArray[k];
				const Vector3D& v2 = velocityArray[k + 4];
				const Vector3D& t1 = tangentArray[k].GetVector3D();
				const Vector3D& t2 = tangentArray[k + 4].GetVector3D();
				const Vector3D& n1 = normalArray[k];

				float m = ((unsigned_int32) (i - 1) < (unsigned_int32) (xdiv - 2)) ? 0.125F : 0.25F;

				Point3D center = (p1 + p2) * 0.5F + (t1 - t2) * m;
				tangent = Normalize((p2 - p1) * 1.5F - (t1 + t2) * 0.25F);
				normal = Normalize(n1 - ProjectOnto(n1, tangent));
				binormal = tangent % normal;
				velocity = (v1 + v2) * 0.5F;

				index1 = k + 2;
				index2 = k + xdiv * 4 + 4;

				b = binormal * r;
				n = normal * r;

				positionArray[index1] = center + b;
				positionArray[index1 + 1] = center - b;
				positionArray[index2] = center - n;
				positionArray[index2 + 1] = center + n;
				normalArray[index1] = normal;
				normalArray[index1 + 1] = normal;
				normalArray[index2] = binormal;
				normalArray[index2 + 1] = binormal;
				tangentArray[index1].GetVector3D() = tangent;
				tangentArray[index1 + 1].GetVector3D() = tangent;
				tangentArray[index2].GetVector3D() = tangent;
				tangentArray[index2 + 1].GetVector3D() = tangent;
				velocityArray[index1] = velocity;
				velocityArray[index1 + 1] = velocity;
				velocityArray[index2] = velocity;
				velocityArray[index2 + 1] = velocity;
			}

		#endif
	}
	else
	{
		if (level > 0)
		{
			tangentArray = ropeController->ropeTangentArray[1];
		}

		#if C4SIMD

			vec_float inverseTime = VecLoadSmearScalar(&kRopeInverseMotionDeltaTime);
			vec_float r = VecLoadSmearScalar(&ropeController->GetParticleRadius());

			vec_float p = updatePosition[0];
			vec_float tangent = VecSub(updatePosition[1], p);
			vec_float unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			vec_float normal = VecLoadUnaligned(&ropeController->ropeNormal.x);
			vec_float binormal = VecCross3D(unitTangent, normal);
			vec_float velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[0].x), VecLoadUnaligned(&previousPosition[0].x)), inverseTime);

			int32 index2 = xdiv * 2 + 2;

			vec_float b = VecMul(binormal, r);
			vec_float n = VecMul(normal, r);

			VecStore3D(VecAdd(p, b), &positionArray[0].x);
			VecStore3D(VecSub(p, b), &positionArray[1].x);
			VecStore3D(VecSub(p, n), &positionArray[index2].x);
			VecStore3D(VecAdd(p, n), &positionArray[index2 + 1].x);
			VecStore3D(normal, &normalArray[0].x);
			VecStore3D(normal, &normalArray[1].x);
			VecStore3D(binormal, &normalArray[index2].x);
			VecStore3D(binormal, &normalArray[index2 + 1].x);
			VecStore3D(tangent, &tangentArray[0].x);
			VecStore3D(tangent, &tangentArray[1].x);
			VecStore3D(tangent, &tangentArray[index2].x);
			VecStore3D(tangent, &tangentArray[index2 + 1].x);
			VecStore3D(velocity, &velocityArray[0].x);
			VecStore3D(velocity, &velocityArray[1].x);
			VecStore3D(velocity, &velocityArray[index2].x);
			VecStore3D(velocity, &velocityArray[index2 + 1].x);

			int32 index1 = 2;
			index2 += 2;

			for (machine i = 1; i < xdiv; i++)
			{
				p = updatePosition[i];
				tangent = VecSub(updatePosition[i + 1], updatePosition[i - 1]);
				unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
				normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
				normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
				binormal = VecCross3D(unitTangent, normal);
				velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[i].x), VecLoadUnaligned(&previousPosition[i].x)), inverseTime);

				b = VecMul(binormal, r);
				n = VecMul(normal, r);

				VecStore3D(VecAdd(p, b), &positionArray[index1].x);
				VecStore3D(VecSub(p, b), &positionArray[index1 + 1].x);
				VecStore3D(VecSub(p, n), &positionArray[index2].x);
				VecStore3D(VecAdd(p, n), &positionArray[index2 + 1].x);
				VecStore3D(normal, &normalArray[index1].x);
				VecStore3D(normal, &normalArray[index1 + 1].x);
				VecStore3D(binormal, &normalArray[index2].x);
				VecStore3D(binormal, &normalArray[index2 + 1].x);
				VecStore3D(tangent, &tangentArray[index1].x);
				VecStore3D(tangent, &tangentArray[index1 + 1].x);
				VecStore3D(tangent, &tangentArray[index2].x);
				VecStore3D(tangent, &tangentArray[index2 + 1].x);
				VecStore3D(velocity, &velocityArray[index1].x);
				VecStore3D(velocity, &velocityArray[index1 + 1].x);
				VecStore3D(velocity, &velocityArray[index2].x);
				VecStore3D(velocity, &velocityArray[index2 + 1].x);

				index1 += 2;
				index2 += 2;
			}

			p = updatePosition[xdiv];
			tangent = VecSub(p, updatePosition[xdiv - 1]);
			unitTangent = VecMul(tangent, VecSmearX(VecInverseSqrtScalar(VecDot3D(tangent, tangent))));
			normal = VecSub(normal, VecProjectOnto3D(normal, unitTangent));
			normal = VecMul(normal, VecSmearX(VecInverseSqrtScalar(VecDot3D(normal, normal))));
			binormal = VecCross3D(unitTangent, normal);
			velocity = VecMul(VecSub(VecLoadUnaligned(&finalPosition[xdiv].x), VecLoadUnaligned(&previousPosition[xdiv].x)), inverseTime);

			b = VecMul(binormal, r);
			n = VecMul(normal, r);

			VecStore3D(VecAdd(p, b), &positionArray[index1].x);
			VecStore3D(VecSub(p, b), &positionArray[index1 + 1].x);
			VecStore3D(VecSub(p, n), &positionArray[index2].x);
			VecStore3D(VecAdd(p, n), &positionArray[index2 + 1].x);
			VecStore3D(normal, &normalArray[index1].x);
			VecStore3D(normal, &normalArray[index1 + 1].x);
			VecStore3D(binormal, &normalArray[index2].x);
			VecStore3D(binormal, &normalArray[index2 + 1].x);
			VecStore3D(tangent, &tangentArray[index1].x);
			VecStore3D(tangent, &tangentArray[index1 + 1].x);
			VecStore3D(tangent, &tangentArray[index2].x);
			VecStore3D(tangent, &tangentArray[index2 + 1].x);
			VecStore3D(velocity, &velocityArray[index1].x);
			VecStore3D(velocity, &velocityArray[index1 + 1].x);
			VecStore3D(velocity, &velocityArray[index2].x);
			VecStore3D(velocity, &velocityArray[index2 + 1].x);

		#else

			float r = ropeController->GetParticleRadius();

			Vector3D tangent = updatePosition[1] - updatePosition[0];
			Vector3D unitTangent = tangent * InverseMag(tangent);
			Vector3D normal = ropeController->ropeNormal;
			Vector3D binormal = unitTangent % normal;
			Vector3D velocity = (finalPosition[0] - previousPosition[0]) * kRopeInverseMotionDeltaTime;

			int32 index2 = xdiv * 2 + 2;

			Vector3D b = binormal * r;
			Vector3D n = normal * r;

			positionArray[0] = updatePosition[0] + b;
			positionArray[1] = updatePosition[0] - b;
			positionArray[index2] = updatePosition[0] - n;
			positionArray[index2 + 1] = updatePosition[0] + n;
			normalArray[0] = normal;
			normalArray[1] = normal;
			normalArray[index2] = binormal;
			normalArray[index2 + 1] = binormal;
			tangentArray[0].GetVector3D() = tangent;
			tangentArray[1].GetVector3D() = tangent;
			tangentArray[index2].GetVector3D() = tangent;
			tangentArray[index2 + 1].GetVector3D() = tangent;
			velocityArray[0] = velocity;
			velocityArray[1] = velocity;
			velocityArray[index2] = velocity;
			velocityArray[index2 + 1] = velocity;

			int32 index1 = 2;
			index2 += 2;

			for (machine i = 1; i < xdiv; i++)
			{
				tangent = updatePosition[i + 1] - updatePosition[i - 1];
				unitTangent = tangent * InverseMag(tangent);
				normal = Normalize(normal - ProjectOnto(normal, unitTangent));
				binormal = unitTangent % normal;
				velocity = (finalPosition[i] - previousPosition[i]) * kRopeInverseMotionDeltaTime;

				b = binormal * r;
				n = normal * r;

				positionArray[index1] = updatePosition[i] + b;
				positionArray[index1 + 1] = updatePosition[i] - b;
				positionArray[index2] = updatePosition[i] - n;
				positionArray[index2 + 1] = updatePosition[i] + n;
				normalArray[index1] = normal;
				normalArray[index1 + 1] = normal;
				normalArray[index2] = binormal;
				normalArray[index2 + 1] = binormal;
				tangentArray[index1].GetVector3D() = tangent;
				tangentArray[index1 + 1].GetVector3D() = tangent;
				tangentArray[index2].GetVector3D() = tangent;
				tangentArray[index2 + 1].GetVector3D() = tangent;
				velocityArray[index1] = velocity;
				velocityArray[index1 + 1] = velocity;
				velocityArray[index2] = velocity;
				velocityArray[index2 + 1] = velocity;

				index1 += 2;
				index2 += 2;
			}

			tangent = updatePosition[xdiv] - updatePosition[xdiv - 1];
			unitTangent = tangent * InverseMag(tangent);
			normal = Normalize(normal - ProjectOnto(normal, unitTangent));
			binormal = unitTangent % normal;
			velocity = (finalPosition[xdiv] - previousPosition[xdiv]) * kRopeInverseMotionDeltaTime;

			b = binormal * r;
			n = normal * r;

			positionArray[index1] = updatePosition[xdiv] + b;
			positionArray[index1 + 1] = updatePosition[xdiv] - b;
			positionArray[index2] = updatePosition[xdiv] - n;
			positionArray[index2 + 1] = updatePosition[xdiv] + n;
			normalArray[index1] = normal;
			normalArray[index1 + 1] = normal;
			normalArray[index2] = binormal;
			normalArray[index2 + 1] = binormal;
			tangentArray[index1].GetVector3D() = tangent;
			tangentArray[index1 + 1].GetVector3D() = tangent;
			tangentArray[index2].GetVector3D() = tangent;
			tangentArray[index2 + 1].GetVector3D() = tangent;
			velocityArray[index1] = velocity;
			velocityArray[index1 + 1] = velocity;
			velocityArray[index2] = velocity;
			velocityArray[index2 + 1] = velocity;

		#endif
	}
}

void RopeController::FinalizeRopeUpdate(Job *job, void *cookie)
{
	RopeController *ropeController = static_cast<RopeController *>(cookie);

	VertexBuffer *vertexBuffer = &ropeController->ropeVertexBuffer;
	if (vertexBuffer->Active())
	{
		volatile RopeVertex *restrict vertex = vertexBuffer->BeginUpdate<RopeVertex>();

		const Geometry *geometry = ropeController->GetTargetNode();
		int32 vertexCount = geometry->GetVertexCount();

		const Point3D *position = ropeController->ropePositionArray;
		const Vector3D *velocity = ropeController->ropeVelocityArray;
		const Vector3D *normal = ropeController->ropeNormalArray;
		const Vector4D *tangent = ropeController->ropeTangentArray[Min(geometry->GetDetailLevel(), 1)];

		for (machine a = 0; a < vertexCount; a++)
		{
			vertex[a].position = position[a];
			vertex[a].velocity = velocity[a];
			vertex[a].normal = normal[a];
			vertex[a].tangent = tangent[a];
		}

		vertexBuffer->EndUpdate();
	}
}

void RopeController::BreakAttachment(int32 index)
{
	attachmentFlags &= ~(1 << index);

	if (fieldStorage)
	{
		int32 k = (index == 0) ? 0 : particleCount - 1;
		ropeFlexibility[k] = 1.0F;

		Point3D position = ropePosition[kRopePositionFinal][k];
		ropePosition[kRopePositionInitial][k] = position;
		ropePosition[kRopePositionPrevious][k] = position;
	}

	attachmentConnectorKey[index][0] = 0;
	attachmentNode[index] = nullptr;

	const Node *node = rigidBodyNode[index];
	rigidBodyNode[index] = nullptr;

	if (node)
	{
		Controller *controller = node->GetController();
		if (controller->Asleep())
		{
			controller->Wake();
		}
	}

	ResetUpdateTime();

	if (Asleep())
	{
		DeformableBodyController::Wake();
	}
}


BreakRopeAttachmentFunction::BreakRopeAttachmentFunction() : Function(kFunctionBreakRopeAttachment, kControllerRope)
{
	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		breakFlag[a] = true;
	}
}

BreakRopeAttachmentFunction::BreakRopeAttachmentFunction(const BreakRopeAttachmentFunction& breakRopeAttachmentFunction) : Function(breakRopeAttachmentFunction)
{
	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		breakFlag[a] = breakRopeAttachmentFunction.breakFlag[a];
	}
}

BreakRopeAttachmentFunction::~BreakRopeAttachmentFunction()
{
}

Function *BreakRopeAttachmentFunction::Replicate(void) const
{
	return (new BreakRopeAttachmentFunction(*this));
}

void BreakRopeAttachmentFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		data << breakFlag[a];
	}
}

void BreakRopeAttachmentFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);

	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		data >> breakFlag[a];
	}
}

void BreakRopeAttachmentFunction::Compress(Compressor& data) const
{
	Function::Compress(data);

	data.Write(breakFlag, kRopeAttachmentCount);
}

bool BreakRopeAttachmentFunction::Decompress(Decompressor& data)
{
	if (Function::Decompress(data))
	{
		data.Read(breakFlag, kRopeAttachmentCount);
		return (true);
	}

	return (false);
}

int32 BreakRopeAttachmentFunction::GetSettingCount(void) const
{
	return (kRopeAttachmentCount);
}

Setting *BreakRopeAttachmentFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index < kRopeAttachmentCount)
	{
		Type identifier = 'BRK1' + index;
		const char *title = table->GetString(StringID('CTRL', kControllerRope, kFunctionBreakRopeAttachment, identifier));
		return (new BooleanSetting(identifier, breakFlag[index], title));
	}

	return (nullptr);
}

void BreakRopeAttachmentFunction::SetSetting(const Setting *setting)
{
	unsigned_int32 index = setting->GetSettingIdentifier() - 'BRK1';

	if (index < unsigned_int32(kRopeAttachmentCount))
	{
		breakFlag[index] = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
}

void BreakRopeAttachmentFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	RopeController *ropeController = static_cast<RopeController *>(controller);

	for (machine a = 0; a < kRopeAttachmentCount; a++)
	{
		if (breakFlag[a])
		{
			ropeController->BreakAttachment(a);
		}
	}

	CallCompletionProc();
}


ClothGeometryObject::ClothGeometryObject() : PrimitiveGeometryObject(kPrimitiveCloth)
{
	SetStaticSurfaceData(2, staticSurfaceData);
}

ClothGeometryObject::ClothGeometryObject(const Vector2D& size, int32 width, int32 height) : PrimitiveGeometryObject(kPrimitiveCloth)
{
	clothSize = size;

	SetMaxSubdiv(width - 1, height - 1);
	SetGeometryFlags(GetGeometryFlags() | (kGeometryMarkingInhibit | kGeometryDynamic));
	SetCollisionExclusionMask(kCollisionExcludeAll);

	SetStaticSurfaceData(2, staticSurfaceData, true);

	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

	staticSurfaceData[1].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[1].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 1.0F);
	staticSurfaceData[1].textureAlignData[1].alignMode = kTextureAlignNatural;
	staticSurfaceData[1].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

ClothGeometryObject::~ClothGeometryObject()
{
}

void ClothGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << clothSize;

	data << TerminatorChunk;
}

void ClothGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<ClothGeometryObject>(data, unpackFlags);
}

bool ClothGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> clothSize;
			return (true);
	}

	return (false);
}

int32 ClothGeometryObject::GetObjectSize(float *size) const
{
	size[0] = clothSize.x;
	size[1] = clothSize.y;
	return (2);
}

void ClothGeometryObject::SetObjectSize(const float *size)
{
	clothSize.x = size[0];
	clothSize.y = size[1];
}

void ClothGeometryObject::Build(Geometry *geometry)
{
	int32 levelCount = Min(GetBuildLevelCount(), 2);
	SetGeometryLevelCount(levelCount);

	int32 materialIndex1 = GetSurfaceData(0)->materialIndex;
	int32 materialIndex2 = GetSurfaceData(1)->materialIndex;

	for (machine level = 0; level < levelCount; level++)
	{
		ArrayDescriptor		desc[7];

		int32 xdiv = Min(Max(GetMaxSubdivX(), kMinClothSubdiv), kMaxClothSubdiv);
		int32 ydiv = Min(Max(GetMaxSubdivY(), kMinClothSubdiv), kMaxClothSubdiv);

		if (level < levelCount - 1)
		{
			xdiv *= 2;
			ydiv *= 2;
		}

		int32 vertexCount = (xdiv + 1) * (ydiv + 1) * 2;
		int32 triangleCount = xdiv * ydiv * 4;

		desc[0].identifier = kArrayPosition;
		desc[0].elementCount = vertexCount;
		desc[0].elementSize = sizeof(Point3D);
		desc[0].componentCount = 3;

		desc[1].identifier = kArrayNormal;
		desc[1].elementCount = vertexCount;
		desc[1].elementSize = sizeof(Vector3D);
		desc[1].componentCount = 3;

		desc[2].identifier = kArrayTangent;
		desc[2].elementCount = vertexCount;
		desc[2].elementSize = sizeof(Vector4D);
		desc[2].componentCount = 4;

		desc[3].identifier = kArrayTexcoord;
		desc[3].elementCount = vertexCount;
		desc[3].elementSize = sizeof(Point2D);
		desc[3].componentCount = 2;

		desc[4].identifier = kArrayPrimitive;
		desc[4].elementCount = triangleCount;
		desc[4].elementSize = sizeof(Triangle);
		desc[4].componentCount = 1;

		desc[5].identifier = kArraySurfaceIndex;
		desc[5].elementCount = vertexCount;
		desc[5].elementSize = 2;
		desc[5].componentCount = 1;

		int32 arrayCount = 6;
		if (materialIndex1 != materialIndex2)
		{
			desc[arrayCount].identifier = kArraySegment;
			desc[arrayCount].elementCount = 2;
			desc[arrayCount].elementSize = sizeof(SegmentData);
			desc[arrayCount].componentCount = 1;
			arrayCount++;
		}

		Mesh *mesh = GetGeometryLevel(level);
		mesh->AllocateStorage(vertexCount, arrayCount, desc);

		Point3D *vertex = mesh->GetArray<Point3D>(kArrayPosition);
		Vector3D *normal = mesh->GetArray<Vector3D>(kArrayNormal);
		Vector4D *tangent = mesh->GetArray<Vector4D>(kArrayTangent);
		Point2D *texcoord = mesh->GetArray<Point2D>(kArrayTexcoord);
		unsigned_int16 *surfaceIndex = mesh->GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;

		float dx = 1.0F / (float) xdiv;
		float dy = 1.0F / (float) ydiv;
		float gx = GetClothSize().x * dx;
		float gy = GetClothSize().y * dy;

		for (machine j = 0; j <= ydiv; j++)
		{
			float fj = (float) j;

			for (machine i = 0; i <= xdiv; i++)
			{
				float fi = (float) i;

				vertex->Set(fi * gx, fj * gy, 0.0F);
				normal->Set(0.0F, 0.0F, 1.0F);
				tangent->Set(1.0F, 0.0F, 0.0F, 1.0F);
				texcoord->Set(fi * dx, fj * dy);
				*++surfaceIndex = 0;

				vertex++;
				normal++;
				tangent++;
				texcoord++;
			}
		}

		for (machine j = 0; j <= ydiv; j++)
		{
			float fj = (float) j;

			for (machine i = 0; i <= xdiv; i++)
			{
				float fi = (float) i;

				vertex->Set(fi * gx, fj * gy, 0.0F);
				normal->Set(0.0F, 0.0F, -1.0F);
				tangent->Set(1.0F, 0.0F, 0.0F, -1.0F);
				texcoord->Set(1.0F - fi * dx, fj * dy);
				*++surfaceIndex = 1;

				vertex++;
				normal++;
				tangent++;
				texcoord++;
			}
		}

		Triangle *triangle = mesh->GetArray<Triangle>(kArrayPrimitive);

		for (machine j = 0; j < ydiv; j++)
		{
			int32 k = j * (xdiv + 1);
			for (machine i = 0; i < xdiv; i++)
			{
				if (((i + j) & 1) == 0)
				{
					triangle[0].Set(k + i, k + xdiv + i + 2, k + xdiv + i + 1);
					triangle[1].Set(k + i, k + i + 1, k + xdiv + i + 2);
				}
				else
				{
					triangle[0].Set(k + i, k + i + 1, k + xdiv + i + 1);
					triangle[1].Set(k + i + 1, k + xdiv + i + 2, k + xdiv + i + 1);
				}

				triangle += 2;
			}
		}

		int32 offset = (xdiv + 1) * (ydiv + 1);
		for (machine j = 0; j < ydiv; j++)
		{
			int32 k = j * (xdiv + 1) + offset;
			for (machine i = 0; i < xdiv; i++)
			{
				if (((i + j) & 1) == 0)
				{
					triangle[0].Set(k + i, k + xdiv + i + 1, k + xdiv + i + 2);
					triangle[1].Set(k + i, k + xdiv + i + 2, k + i + 1);
				}
				else
				{
					triangle[0].Set(k + i, k + xdiv + i + 1, k + i + 1);
					triangle[1].Set(k + i + 1, k + xdiv + i + 1, k + xdiv + i + 2);
				}

				triangle += 2;
			}
		}

		mesh->GenerateTexcoords(geometry, this);
		mesh->TransformTexcoords(geometry, this);

		if (materialIndex1 != materialIndex2)
		{
			SegmentData *segmentData = mesh->GetArray<SegmentData>(kArraySegment);
			int32 count = triangleCount / 2;

			if (materialIndex1 < materialIndex2)
			{
				segmentData[0].materialIndex = materialIndex1;
				segmentData[0].primitiveStart = 0;
				segmentData[0].primitiveCount = count;

				segmentData[1].materialIndex = materialIndex2;
				segmentData[1].primitiveStart = count;
				segmentData[1].primitiveCount = count;
			}
			else
			{
				segmentData[0].materialIndex = materialIndex2;
				segmentData[0].primitiveStart = count;
				segmentData[0].primitiveCount = count;

				segmentData[1].materialIndex = materialIndex1;
				segmentData[1].primitiveStart = 0;
				segmentData[1].primitiveCount = count;
			}
		}
	}
}


ClothGeometry::ClothGeometry() : PrimitiveGeometry(kPrimitiveCloth)
{
	clothBoundingBox = nullptr;
}

ClothGeometry::ClothGeometry(const Vector2D& size, int32 width, int32 height) : PrimitiveGeometry(kPrimitiveCloth)
{
	SetNewObject(new ClothGeometryObject(size, width, height));
	SetController(new ClothController);

	clothBoundingBox = nullptr;
}

ClothGeometry::ClothGeometry(const ClothGeometry& clothGeometry) : PrimitiveGeometry(clothGeometry)
{
	clothBoundingBox = nullptr;
}

ClothGeometry::~ClothGeometry()
{
}

Node *ClothGeometry::Replicate(void) const
{
	return (new ClothGeometry(*this));
}

bool ClothGeometry::CalculateBoundingBox(Box3D *box) const
{
	if (clothBoundingBox)
	{
		*box = *clothBoundingBox;
	}
	else
	{
		const Vector2D& clothSize = GetObject()->GetClothSize();

		box->min.Set(0.0F, 0.0F, 0.0F);
		box->max.Set(clothSize, 0.0F);
	}

	return (true);
}

bool ClothGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	if (clothBoundingBox)
	{
		sphere->SetCenter((clothBoundingBox->min + clothBoundingBox->max) * 0.5F);
		sphere->SetRadius(Magnitude(clothBoundingBox->max - clothBoundingBox->min) * 0.5F);
	}
	else
	{
		const Vector2D& clothSize = GetObject()->GetClothSize();
		float x = clothSize.x * 0.5F;
		float y = clothSize.y * 0.5F;

		sphere->SetCenter(x, y, 0.0F);
		sphere->SetRadius(Sqrt(x * x + y * y));
	}

	return (true);
}

int32 ClothGeometry::GetInternalConnectorCount(void) const
{
	return (PrimitiveGeometry::GetInternalConnectorCount() + 3);
}

const char *ClothGeometry::GetInternalConnectorKey(int32 index) const
{
	int32 count = PrimitiveGeometry::GetInternalConnectorCount();
	if (index < count)
	{
		return (PrimitiveGeometry::GetInternalConnectorKey(index));
	}

	if (index == count)
	{
		return (kConnectorKeyBlocker);
	}
	else if (index == count + 1)
	{
		return (kConnectorKeyWind);
	}
	else if (index == count + 2)
	{
		return (kConnectorKeyPath);
	}

	return (nullptr);
}

bool ClothGeometry::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyBlocker)
	{
		return (node->GetNodeType() == kNodeBlocker);
	}
	else if (key == kConnectorKeyWind)
	{
		if (node->GetNodeType() == kNodeField)
		{
			const Field *field = static_cast<const Field *>(node);
			const Force *force = field->GetForce();
			return ((force) && (force->GetForceType() == kForceWind));
		}

		return (false);
	}
	else if (key == kConnectorKeyPath)
	{
		if (node->GetNodeType() == kNodeMarker)
		{
			return (static_cast<const Marker *>(node)->GetMarkerType() == kMarkerPath);
		}

		return (false);
	}

	return (PrimitiveGeometry::ValidConnectedNode(key, node));
}

void ClothGeometry::Preprocess(void)
{
	PrimitiveGeometry::Preprocess();

	const Controller *controller = GetController();
	if ((controller) && (controller->GetControllerType() == kControllerCloth))
	{
		const ClothController *clothController = static_cast<const ClothController *>(controller);
		clothBoundingBox = clothController->GetClothBoundingBox();
	}
}


ClothController::ClothController() :
		DeformableBodyController(kControllerCloth, &JobStepSimulation, this),
		clothUpdateJob(&JobUpdateCloth, &FinalizeClothUpdate, this),
		clothVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	fieldStorage = nullptr;
	springStorage = nullptr;
	vertexStorage = nullptr;
	particleCount = 0;

	stretchSpringConstant = 2000.0F;
	stretchDamperConstant = 0.0F;
	shearSpringConstant = 2000.0F;
	shearDamperConstant = 0.0F;
	bendSpringConstant = 1000.0F;
	bendDamperConstant = 0.0F;

	attachmentFlags = kClothLowerLeftCorner | kClothLowerRightCorner | kClothUpperRightCorner | kClothUpperLeftCorner;
}

ClothController::ClothController(const ClothController& clothController) :
		DeformableBodyController(clothController, &JobStepSimulation, this),
		clothUpdateJob(&JobUpdateCloth, &FinalizeClothUpdate, this),
		clothVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	fieldStorage = nullptr;
	springStorage = nullptr;
	vertexStorage = nullptr;
	particleCount = 0;

	stretchSpringConstant = clothController.stretchSpringConstant;
	stretchDamperConstant = clothController.stretchDamperConstant;
	shearSpringConstant = clothController.shearSpringConstant;
	shearDamperConstant = clothController.shearDamperConstant;
	bendSpringConstant = clothController.bendSpringConstant;
	bendDamperConstant = clothController.bendDamperConstant;

	attachmentFlags = clothController.attachmentFlags;
}

ClothController::~ClothController()
{
	delete[] vertexStorage;
	delete[] springStorage;
	delete[] fieldStorage;
}

Controller *ClothController::Replicate(void) const
{
	return (new ClothController(*this));
}

bool ClothController::ValidNode(const Node *node)
{
	if (node->GetNodeType() == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<const Geometry *>(node);
		if ((geometry->GetGeometryType() == kGeometryPrimitive) && (static_cast<const PrimitiveGeometry *>(geometry)->GetPrimitiveType() == kPrimitiveCloth))
		{
			return (true);
		}
	}

	return (false);
}

void ClothController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	DeformableBodyController::Pack(data, packFlags);

	data << ChunkHeader('MASS', 4);
	data << GetParticleMass();

	data << ChunkHeader('THCK', 4);
	data << GetParticleRadius();

	data << ChunkHeader('SPRG', 12);
	data << stretchSpringConstant;
	data << shearSpringConstant;
	data << bendSpringConstant;

	data << ChunkHeader('DAMP', 12);
	data << stretchDamperConstant;
	data << shearDamperConstant;
	data << bendDamperConstant;

	data << ChunkHeader('ATCH', 4);
	data << attachmentFlags;

	if ((fieldStorage) && (!GetTargetNode()->GetManipulator()))
	{
		data << ChunkHeader('FELD', 4 + particleCount * (sizeof(Point3D) * kClothPositionCount));
		data << particleCount;

		for (machine a = 0; a < kClothPositionCount; a++)
		{
			data.WriteArray(particleCount, clothPosition[a]);
		}
	}

	data << TerminatorChunk;
}

void ClothController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	DeformableBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<ClothController>(data, unpackFlags);
}

bool ClothController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'THCK':
		{
			float	radius;

			data >> radius;
			SetParticleRadius(radius);
			return (true);
		}

		case 'SPRG':

			data >> stretchSpringConstant;
			data >> shearSpringConstant;
			data >> bendSpringConstant;
			return (true);

		case 'DAMP':

			data >> stretchDamperConstant;
			data >> shearDamperConstant;
			data >> bendDamperConstant;
			return (true);

		case 'ATCH':

			data >> attachmentFlags;
			return (true);

		case 'FELD':

			data >> particleCount;
			AllocateFieldStorage();

			for (machine a = 0; a < kClothPositionCount; a++)
			{
				data.ReadArray(particleCount, clothPosition[a]);
			}

			return (true);
	}

	return (false);
}

int32 ClothController::GetSettingCount(void) const
{
	return (DeformableBodyController::GetSettingCount() + 24);
}

Setting *ClothController::GetSetting(int32 index) const
{
	int32 i = index - DeformableBodyController::GetSettingCount();
	if (i >= 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (i == 0)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP'));
			return (new HeadingSetting('PROP', title));
		}

		if (i == 1)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'PROP', 'THCK'));
			return (new TextSetting('THCK', GetClothThickness(), title));
		}

		if (i == 2)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'MASS'));
			return (new TextSetting('MASS', GetParticleMass(), title));
		}

		if (i == 3)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'GRAV'));
			return (new TextSetting('GRAV', GetGravityMultiplier(), title));
		}

		if (i == 4)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'FLDG'));
			return (new FloatSetting('FLDG', GetFluidDragMultiplier(), title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 5)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'WNDG'));
			return (new FloatSetting('WNDG', GetWindDragMultiplier(), title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 6)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerDeformableBody, 'PROP', 'IRES'));
			return (new FloatSetting('IRES', GetInternalResistance(), title, 0.0F, 1.0F, 0.01F));
		}

		if (i == 7)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'SPRG'));
			return (new HeadingSetting('SPRG', title));
		}

		if (i == 8)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'SPRG', 'STRS'));
			return (new FloatSetting('STRS', stretchSpringConstant * 0.001F, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 9)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'SPRG', 'SHRS'));
			return (new FloatSetting('SHRS', shearSpringConstant * 0.001F, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 10)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'SPRG', 'BNDS'));
			return (new FloatSetting('BNDS', bendSpringConstant * 0.001F, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 11)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'DAMP'));
			return (new HeadingSetting('DAMP', title));
		}

		if (i == 12)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'DAMP', 'STRD'));
			return (new FloatSetting('STRD', stretchDamperConstant, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 13)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'DAMP', 'SHRD'));
			return (new FloatSetting('SHRD', shearDamperConstant, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 14)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'DAMP', 'BNDD'));
			return (new FloatSetting('BNDD', bendDamperConstant, title, 0.0F, 10.0F, 0.1F));
		}

		if (i == 15)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH'));
			return (new HeadingSetting('ATCH', title));
		}

		if (i == 16)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'COR1'));
			return (new BooleanSetting('COR1', ((attachmentFlags & kClothLowerLeftCorner) != 0), title));
		}

		if (i == 17)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'COR2'));
			return (new BooleanSetting('COR2', ((attachmentFlags & kClothLowerRightCorner) != 0), title));
		}

		if (i == 18)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'COR3'));
			return (new BooleanSetting('COR3', ((attachmentFlags & kClothUpperRightCorner) != 0), title));
		}

		if (i == 19)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'COR4'));
			return (new BooleanSetting('COR4', ((attachmentFlags & kClothUpperLeftCorner) != 0), title));
		}

		if (i == 20)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'EDG1'));
			return (new BooleanSetting('EDG1', ((attachmentFlags & kClothBottomEdge) != 0), title));
		}

		if (i == 21)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'EDG2'));
			return (new BooleanSetting('EDG2', ((attachmentFlags & kClothRightEdge) != 0), title));
		}

		if (i == 22)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'EDG3'));
			return (new BooleanSetting('EDG3', ((attachmentFlags & kClothTopEdge) != 0), title));
		}

		if (i == 23)
		{
			const char *title = table->GetString(StringID('CTRL', kControllerCloth, 'ATCH', 'EDG4'));
			return (new BooleanSetting('EDG4', ((attachmentFlags & kClothLeftEdge) != 0), title));
		}

		return (nullptr);
	}

	return (DeformableBodyController::GetSetting(index));
}

void ClothController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'THCK')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetClothThickness(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'MASS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetParticleMass(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'GRAV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		SetGravityMultiplier(FmaxZero(Text::StringToFloat(text)));
	}
	else if (identifier == 'FLDG')
	{
		SetFluidDragMultiplier(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'WNDG')
	{
		SetWindDragMultiplier(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'IRES')
	{
		SetInternalResistance(static_cast<const FloatSetting *>(setting)->GetFloatValue());
	}
	else if (identifier == 'STRS')
	{
		stretchSpringConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 1000.0F;
	}
	else if (identifier == 'SHRS')
	{
		shearSpringConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 1000.0F;
	}
	else if (identifier == 'BNDS')
	{
		bendSpringConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 1000.0F;
	}
	else if (identifier == 'STRD')
	{
		stretchDamperConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'SHRD')
	{
		shearDamperConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'BNDD')
	{
		bendDamperConstant = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'COR1')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothLowerLeftCorner;
		}
		else
		{
			attachmentFlags &= ~kClothLowerLeftCorner;
		}
	}
	else if (identifier == 'COR2')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothLowerRightCorner;
		}
		else
		{
			attachmentFlags &= ~kClothLowerRightCorner;
		}
	}
	else if (identifier == 'COR3')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothUpperRightCorner;
		}
		else
		{
			attachmentFlags &= ~kClothUpperRightCorner;
		}
	}
	else if (identifier == 'COR4')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothUpperLeftCorner;
		}
		else
		{
			attachmentFlags &= ~kClothUpperLeftCorner;
		}
	}
	else if (identifier == 'EDG1')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothBottomEdge;
		}
		else
		{
			attachmentFlags &= ~kClothBottomEdge;
		}
	}
	else if (identifier == 'EDG2')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothRightEdge;
		}
		else
		{
			attachmentFlags &= ~kClothRightEdge;
		}
	}
	else if (identifier == 'EDG3')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothTopEdge;
		}
		else
		{
			attachmentFlags &= ~kClothTopEdge;
		}
	}
	else if (identifier == 'EDG4')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			attachmentFlags |= kClothLeftEdge;
		}
		else
		{
			attachmentFlags &= ~kClothLeftEdge;
		}
	}
	else
	{
		DeformableBodyController::SetSetting(setting);
	}
}

void ClothController::AllocateFieldStorage(void)
{
	fieldStorage = new char[particleCount * (sizeof(SimdVector3D) * 3 + sizeof(Point3D) * kClothPositionCount + sizeof(Vector3D) + 4)];

	clothVelocity = reinterpret_cast<SimdVector3D *>(fieldStorage);
	clothBaseForce = clothVelocity + particleCount;
	clothFieldForce = clothBaseForce + particleCount;

	Point3D *position = reinterpret_cast<Point3D *>(clothFieldForce + particleCount);
	for (machine a = 0; a < kClothPositionCount; a++)
	{
		clothPosition[a] = position;
		position += particleCount;
	}

	clothBitangent = position;
	clothFlexibility = reinterpret_cast<float *>(clothBitangent + particleCount);
}

void ClothController::AllocateSpringStorage(void)
{
	springStorage = new char[(stretchSpringCount + shearSpringCount + bendSpringCount) * sizeof(SpringData)];

	stretchSpringData = reinterpret_cast<SpringData *>(springStorage);
	shearSpringData = stretchSpringData + stretchSpringCount;
	bendSpringData = shearSpringData + shearSpringCount;
}

void ClothController::InitFlexibility(void)
{
	float *flexibility = clothFlexibility;
	for (machine a = 0; a < particleCount; a++)
	{
		flexibility[a] = 1.0F;
	}

	unsigned_int32 flags = attachmentFlags;

	if (flags & kClothLowerLeftCorner)
	{
		flexibility[0] = 0.0F;
	}

	if (flags & kClothLowerRightCorner)
	{
		flexibility[fieldWidth - 1] = 0.0F;
	}

	if (flags & kClothUpperRightCorner)
	{
		flexibility[particleCount - 1] = 0.0F;
	}

	if (flags & kClothUpperLeftCorner)
	{
		flexibility[particleCount - fieldWidth] = 0.0F;
	}

	if (flags & kClothBottomEdge)
	{
		for (machine a = 0; a < fieldWidth; a++)
		{
			flexibility[a] = 0.0F;
		}
	}

	if (flags & kClothRightEdge)
	{
		for (machine a = 1; a <= fieldHeight; a++)
		{
			flexibility[a * fieldWidth - 1] = 0.0F;
		}
	}

	if (flags & kClothTopEdge)
	{
		for (machine a = 0; a < fieldWidth; a++)
		{
			flexibility[particleCount - fieldWidth + a] = 0.0F;
		}
	}

	if (flags & kClothLeftEdge)
	{
		for (machine a = 0; a < fieldHeight; a++)
		{
			flexibility[a * fieldWidth] = 0.0F;
		}
	}
}

void ClothController::Preprocess(void)
{
	DeformableBodyController::Preprocess();

	ClothGeometry *clothGeometry = GetTargetNode();
	clothGeometry->SetShaderFlags(clothGeometry->GetShaderFlags() | kShaderNormalizeBasisVectors);
	clothGeometry->SetVertexBufferArrayFlags((1 << kArrayPosition) | (1 << kArrayVelocity) | (1 << kArrayNormal) | (1 << kArrayTangent));
	clothGeometry->SetVertexAttributeArray(kArrayPosition, 0, 3);
	clothGeometry->SetVertexAttributeArray(kArrayVelocity, sizeof(Point3D), 3);
	clothGeometry->SetVertexAttributeArray(kArrayNormal, sizeof(Point3D) + sizeof(Vector3D), 3);
	clothGeometry->SetVertexAttributeArray(kArrayTangent, sizeof(Point3D) + sizeof(Vector3D) * 2, 4);

	ClothGeometryObject *object = clothGeometry->GetObject();
	const Mesh *mesh = object->GetGeometryLevel(0);
	int32 vertexCount = mesh->GetVertexCount();

	clothVertexBuffer.Establish(vertexCount * sizeof(ClothVertex));
	clothGeometry->SetVertexBuffer(kVertexBufferAttributeArray1, &clothVertexBuffer, sizeof(ClothVertex));

	fieldWidth = Min(Max(object->GetMaxSubdivX(), kMinClothSubdiv), kMaxClothSubdiv) + 1;
	fieldHeight = Min(Max(object->GetMaxSubdivY(), kMinClothSubdiv), kMaxClothSubdiv) + 1;
	particleCount = fieldWidth * fieldHeight;

	const Vector2D& size = object->GetClothSize();
	stretchSpringDistance = Fmin(size.x / (float) (fieldWidth - 1), size.y / (float) (fieldHeight - 1));
	shearSpringDistance = stretchSpringDistance * K::sqrt_2;
	bendSpringDistance = stretchSpringDistance * 2.0F;

	SetParticleVolume(stretchSpringDistance * stretchSpringDistance * (GetClothThickness() * GetVolumeMultiplier()));

	vertexStorage = new char[vertexCount * (sizeof(Point3D) + sizeof(Vector3D) * 2 + sizeof(Vector4D))];
	clothPositionArray = reinterpret_cast<Point3D *>(vertexStorage);
	clothVelocityArray = clothPositionArray + vertexCount;
	clothNormalArray = clothVelocityArray + vertexCount;

	Vector4D *clothTangent = reinterpret_cast<Vector4D *>(clothNormalArray + vertexCount);
	clothTangentArray = clothTangent;

	const Vector4D *meshTangent = mesh->GetArray<Vector4D>(kArrayTangent);
	for (machine a = 0; a < vertexCount; a++)
	{
		clothTangent[a].w = meshTangent[a].w;
	}

	if (clothGeometry->GetManipulator())
	{
		delete[] fieldStorage;
		fieldStorage = nullptr;
	}

	if (!fieldStorage)
	{
		AllocateFieldStorage();

		mesh = object->GetGeometryLevel(object->GetGeometryLevelCount() - 1);
		const Point3D *positionArray = mesh->GetArray<Point3D>(kArrayPosition);

		Point3D *restrict finalPosition = clothPosition[kClothPositionFinal];
		Point3D *restrict previousPosition = clothPosition[kClothPositionPrevious];

		for (machine i = 0; i < particleCount; i++)
		{
			const Point3D& position = positionArray[i];
			finalPosition[i] = position;
			previousPosition[i] = position;
		}
	}

	InitFlexibility();

	const Node *node = clothGeometry->GetConnectedNode(kConnectorKeyPath);
	if ((node) && (node->GetNodeType() == kNodeMarker))
	{
		const Marker *marker = static_cast<const Marker *>(node);
		if (marker->GetMarkerType() == kMarkerPath)
		{
			const Path *path = static_cast<const PathMarker *>(marker)->GetPath();
			Transform4D transform = clothGeometry->GetInverseWorldTransform() * marker->GetWorldTransform();
			Point3D *restrict finalPosition = clothPosition[kClothPositionFinal];
			Point3D *restrict previousPosition = clothPosition[kClothPositionPrevious];

			float ds = 1.0F / (float) (fieldWidth - 1);
			for (machine i = 0; i < fieldWidth; i++)
			{
				finalPosition[i] = transform * path->GetPathState((float) i * ds);
				previousPosition[i] = finalPosition[i];
			}
		}
	}

	clothBoundingBox.Calculate(particleCount, clothPosition[kClothPositionFinal]);

	if (!springStorage)
	{
		stretchSpringCount = (fieldWidth - 1) * fieldHeight + fieldWidth * (fieldHeight - 1);
		shearSpringCount = 2 * (fieldWidth - 1) * (fieldHeight - 1);
		bendSpringCount = stretchSpringCount - fieldWidth - fieldHeight;
		AllocateSpringStorage();

		SpringData *springData = stretchSpringData;
		for (machine j = 0; j < fieldHeight; j++)
		{
			int32 base = j * fieldWidth;
			for (machine i = 1; i < fieldWidth; i++)
			{
				springData->particleIndex1 = (unsigned_int16) (base + i - 1);
				springData->particleIndex2 = (unsigned_int16) (base + i);
				springData++;
			}
		}

		for (machine i = 0; i < fieldWidth; i++)
		{
			for (machine j = 1; j < fieldHeight; j++)
			{
				springData->particleIndex1 = (unsigned_int16) (i + (j - 1) * fieldWidth);
				springData->particleIndex2 = (unsigned_int16) (i + j * fieldWidth);
				springData++;
			}
		}

		springData = shearSpringData;
		for (machine j = 1; j < fieldHeight; j++)
		{
			int32 base = j * fieldWidth;
			for (machine i = 1; i < fieldWidth; i++)
			{
				springData->particleIndex1 = (unsigned_int16) (base - fieldWidth + i - 1);
				springData->particleIndex2 = (unsigned_int16) (base + i);
				springData++;

				springData->particleIndex1 = (unsigned_int16) (base - fieldWidth + i);
				springData->particleIndex2 = (unsigned_int16) (base + i - 1);
				springData++;
			}
		}

		springData = bendSpringData;
		for (machine j = 0; j < fieldHeight; j++)
		{
			int32 base = j * fieldWidth;
			for (machine i = 2; i < fieldWidth; i++)
			{
				springData->particleIndex1 = (unsigned_int16) (base + i - 2);
				springData->particleIndex2 = (unsigned_int16) (base + i);
				springData++;
			}
		}

		for (machine i = 0; i < fieldWidth; i++)
		{
			for (machine j = 2; j < fieldHeight; j++)
			{
				springData->particleIndex1 = (unsigned_int16) (i + (j - 2) * fieldWidth);
				springData->particleIndex2 = (unsigned_int16) (i + j * fieldWidth);
				springData++;
			}
		}
	}

	if ((GetPhysicsController()) && (!clothGeometry->GetManipulator()))
	{
		WarmStart();
	}
}

void ClothController::Neutralize(void)
{
	clothVertexBuffer.Establish(0);

	delete[] vertexStorage;
	vertexStorage = nullptr;

	delete[] springStorage;
	springStorage = nullptr;

	delete[] fieldStorage;
	fieldStorage = nullptr;

	DeformableBodyController::Neutralize();
}

void ClothController::WarmStart(void)
{
	int32 passCount = GetWarmStartTime() / kClothTimeStep;
	SetWarmStartTime(0);

	if (passCount > 0)
	{
		for (machine a = 0; a < particleCount; a++)
		{
			clothFieldForce[a].Set(0.0F, 0.0F, 0.0F);
		}

		Simulate(passCount, CalculateGravityForce(), Zero3D);

		if (!Asleep())
		{
			SetDeformableBodyState(GetDeformableBodyState() | kDeformableBodyAutoAsleep);
			BodyController::Sleep();
		}
	}
}

void ClothController::Simulate(int32 passCount, const Vector3D& gravityForce, const Vector3D& windVelocity, int32 threadIndex)
{
	FieldArray		fieldArray;

	QueryForceFields(fieldArray, threadIndex);

	ClothGeometry *clothGeometry = GetTargetNode();

	Point3D *restrict finalPosition = clothPosition[kClothPositionFinal];
	Point3D *restrict previousPosition = clothPosition[kClothPositionPrevious];
	SimdVector3D *restrict velocity = clothVelocity;
	SimdVector3D *restrict force = clothBaseForce;

	#if C4SIMD

		vec_float gravity = VecLoadUnaligned(&gravityForce.x);
		vec_float wind = VecLoadUnaligned(&windVelocity.x);
		vec_float kw = VecLoadSmearScalar(&GetWindDragMultiplier());
		vec_float t2_over_2m = VecSmearX(VecMulScalar(VecLoadScalar(&GetHalfInverseParticleMass()), VecLoadScalar(&kClothSquaredDeltaTime)));

		const vec_float one = VecLoadVectorConstant<0x3F800000>();
		vec_float velocityFactor = VecSmearX(VecMulScalar(VecSubScalar(one, VecLoadScalar(&GetInternalResistance())), VecLoadScalar(&kClothDeltaTime)));
		vec_float inverseDeltaTime = VecLoadSmearScalar(&kClothInverseDeltaTime);

	#else

		float kw = GetWindDragMultiplier();
		float t2_over_2m = GetHalfInverseParticleMass() * kClothSquaredDeltaTime;
		float velocityFactor = (1.0F - GetInternalResistance()) * kClothDeltaTime;

	#endif

	do
	{
		#if C4SIMD

			for (machine a = 0; a < particleCount; a++)
			{
				velocity[a] = VecMul(VecSub(VecLoadUnaligned(&finalPosition[a].x), VecLoadUnaligned(&previousPosition[a].x)), inverseDeltaTime);
				force[a] = VecMadd(VecSub(wind, velocity[a]), kw, gravity);
			}

			vec_float ks = VecLoadSmearScalar(&stretchSpringConstant);
			vec_float kd = VecLoadSmearScalar(&stretchDamperConstant);
			vec_float x = VecLoadSmearScalar(&stretchSpringDistance);

			const SpringData *springData = stretchSpringData;
			for (machine a = 0; a < stretchSpringCount; a++)
			{
				int32 index1 = springData->particleIndex1;
				int32 index2 = springData->particleIndex2;

				vec_float p1 = VecLoadUnaligned(&finalPosition[index1].x);
				vec_float p2 = VecLoadUnaligned(&finalPosition[index2].x);

				const vec_float& v1 = velocity[index1];
				const vec_float& v2 = velocity[index2];

				vec_float dp = VecSub(p2, p1);
				vec_float f = VecMul(dp, VecMul(ks, (VecSub(one, VecMul(x, VecSmearX(VecInverseSqrtScalar(VecDot3D(dp, dp))))))));
				f = VecMadd(VecSub(v2, v1), kd, f);

				force[index1] = VecAdd(force[index1], f);
				force[index2] = VecSub(force[index2], f);

				springData++;
			}

			ks = VecLoadSmearScalar(&shearSpringConstant);
			kd = VecLoadSmearScalar(&shearDamperConstant);
			x = VecLoadSmearScalar(&shearSpringDistance);

			springData = shearSpringData;
			for (machine a = 0; a < shearSpringCount; a++)
			{
				int32 index1 = springData->particleIndex1;
				int32 index2 = springData->particleIndex2;

				vec_float p1 = VecLoadUnaligned(&finalPosition[index1].x);
				vec_float p2 = VecLoadUnaligned(&finalPosition[index2].x);

				const vec_float& v1 = velocity[index1];
				const vec_float& v2 = velocity[index2];

				vec_float dp = VecSub(p2, p1);
				vec_float f = VecMul(dp, VecMul(ks, (VecSub(one, VecMul(x, VecSmearX(VecInverseSqrtScalar(VecDot3D(dp, dp))))))));
				f = VecMadd(VecSub(v2, v1), kd, f);

				force[index1] = VecAdd(force[index1], f);
				force[index2] = VecSub(force[index2], f);

				springData++;
			}

			ks = VecLoadSmearScalar(&bendSpringConstant);
			kd = VecLoadSmearScalar(&bendDamperConstant);
			x = VecLoadSmearScalar(&bendSpringDistance);

			springData = bendSpringData;
			for (machine a = 0; a < bendSpringCount; a++)
			{
				int32 index1 = springData->particleIndex1;
				int32 index2 = springData->particleIndex2;

				vec_float p1 = VecLoadUnaligned(&finalPosition[index1].x);
				vec_float p2 = VecLoadUnaligned(&finalPosition[index2].x);

				const vec_float& v1 = velocity[index1];
				const vec_float& v2 = velocity[index2];

				vec_float dp = VecSub(p2, p1);
				vec_float f = VecMul(dp, VecMul(ks, (VecSub(one, VecMul(x, VecSmearX(VecInverseSqrtScalar(VecDot3D(dp, dp))))))));
				f = VecMadd(VecSub(v2, v1), kd, f);

				force[index1] = VecAdd(force[index1], f);
				force[index2] = VecSub(force[index2], f);

				springData++;
			}

			ApplyForceFields(fieldArray, particleCount, finalPosition, velocity, clothFieldForce);

			const float *flexibility = clothFlexibility;
			for (machine a = 0; a < particleCount; a++)
			{
				vec_float f = VecAdd(force[a], clothFieldForce[a]);
				vec_float p = VecLoadUnaligned(&finalPosition[a].x);
				vec_float q = VecMadd(VecMadd(velocity[a], velocityFactor, VecMul(f, t2_over_2m)), VecLoadSmearScalar(&flexibility[a]), p);
				VecStore3D(p, &previousPosition[a].x);
				VecStore3D(q, &finalPosition[a].x);
			}

		#else

			for (machine a = 0; a < particleCount; a++)
			{
				velocity[a] = (finalPosition[a] - previousPosition[a]) * kClothInverseDeltaTime;
				force[a] = (windVelocity - velocity[a]) * kw + gravityForce;
			}

			float ks = stretchSpringConstant;
			float kd = stretchDamperConstant;
			float x = stretchSpringDistance;

			const SpringData *springData = stretchSpringData;
			for (machine a = 0; a < stretchSpringCount; a++)
			{
				int32 index1 = springData->particleIndex1;
				int32 index2 = springData->particleIndex2;

				const Point3D& p1 = finalPosition[index1];
				const Point3D& p2 = finalPosition[index2];

				const Vector3D& v1 = velocity[index1];
				const Vector3D& v2 = velocity[index2];

				Vector3D dp = p2 - p1;
				Vector3D f = dp * (ks * (1.0F - x * InverseMag(dp))) + (v2 - v1) * kd;

				force[index1] += f;
				force[index2] -= f;

				springData++;
			}

			ks = shearSpringConstant;
			kd = shearDamperConstant;
			x = shearSpringDistance;

			springData = shearSpringData;
			for (machine a = 0; a < shearSpringCount; a++)
			{
				int32 index1 = springData->particleIndex1;
				int32 index2 = springData->particleIndex2;

				const Point3D& p1 = finalPosition[index1];
				const Point3D& p2 = finalPosition[index2];

				const Vector3D& v1 = velocity[index1];
				const Vector3D& v2 = velocity[index2];

				Vector3D dp = p2 - p1;
				Vector3D f = dp * (ks * (1.0F - x * InverseMag(dp))) + (v2 - v1) * kd;

				force[index1] += f;
				force[index2] -= f;

				springData++;
			}

			ks = bendSpringConstant;
			kd = bendDamperConstant;
			x = bendSpringDistance;

			springData = bendSpringData;
			for (machine a = 0; a < bendSpringCount; a++)
			{
				int32 index1 = springData->particleIndex1;
				int32 index2 = springData->particleIndex2;

				const Point3D& p1 = finalPosition[index1];
				const Point3D& p2 = finalPosition[index2];

				const Vector3D& v1 = velocity[index1];
				const Vector3D& v2 = velocity[index2];

				Vector3D dp = p2 - p1;
				Vector3D f = dp * (ks * (1.0F - x * InverseMag(dp))) + (v2 - v1) * kd;

				force[index1] += f;
				force[index2] -= f;

				springData++;
			}

			ApplyForceFields(fieldArray, particleCount, finalPosition, velocity, clothFieldForce);

			const float *flexibility = clothFlexibility;
			for (machine a = 0; a < particleCount; a++)
			{
				Vector3D f = force[a] + clothFieldForce[a];
				Point3D q = finalPosition[a] + (velocity[a] * velocityFactor + f * t2_over_2m) * flexibility[a];
				previousPosition[a] = finalPosition[a];
				finalPosition[a] = q;
			}

		#endif

		const Node *blocker = GetBlockerNode();
		if (blocker)
		{
			Transform4D transform = clothGeometry->GetInverseWorldTransform() * blocker->GetWorldTransform();
			Transform4D inverseTransform = blocker->GetInverseWorldTransform() * clothGeometry->GetWorldTransform();

			const BlockerObject *blockerObject = static_cast<const Blocker *>(blocker)->GetObject();
			blockerObject->ApplyBlocker(particleCount, GetParticleRadius(), finalPosition, previousPosition, transform, inverseTransform);
		}
	} while (--passCount > 0);

	clothBoundingBox.Calculate(particleCount, finalPosition);
	clothGeometry->SetWorldBoundingBox(Transform(clothBoundingBox, clothGeometry->GetWorldTransform()));

	Invalidate();
}

void ClothController::JobStepSimulation(Job *job, void *cookie)
{
	ClothController *clothController = static_cast<ClothController *>(cookie);
	Vector3D wind = clothController->GetWindVelocity();

	const Node *field = clothController->GetWindFieldNode();
	if ((field) && (field->Enabled()))
	{
		const Transform4D& inverseTransform = clothController->GetTargetNode()->GetInverseWorldTransform();
		wind += inverseTransform * (field->GetWorldTransform() * static_cast<WindForce *>(static_cast<const Field *>(field)->GetForce())->GetWindVelocity());
	}

	clothController->Simulate(kClothStepRatio, clothController->CalculateGravityForce(), wind, job->GetThreadIndex());
}

void ClothController::Update(void)
{
	DeformableBodyController::Update();

	PhysicsController *physicsController = GetPhysicsController();
	if (physicsController)
	{
		physicsController->IncrementPhysicsCounter(kPhysicsCounterDeformableBodyUpdate);
	}

	GetTargetNode()->GetWorld()->SubmitWorldJob(&clothUpdateJob);
}

void ClothController::JobUpdateCloth(Job *job, void *cookie)
{
	ClothController *clothController = static_cast<ClothController *>(cookie);

	const ClothGeometry *cloth = clothController->GetTargetNode();
	const ClothGeometryObject *object = cloth->GetObject();

	int32 level = cloth->GetDetailLevel();
	const Mesh *mesh = object->GetGeometryLevel(level);

	Point3D *restrict positionArray = clothController->clothPositionArray;
	Vector3D *restrict velocityArray = clothController->clothVelocityArray;
	Vector3D *restrict normalArray = clothController->clothNormalArray;
	Vector4D *restrict tangentArray = clothController->clothTangentArray;

	int32 width = clothController->fieldWidth;
	int32 height = clothController->fieldHeight;
	const Point3D *finalPosition = clothController->clothPosition[kClothPositionFinal];
	const Point3D *previousPosition = clothController->clothPosition[kClothPositionPrevious];

	if (level < object->GetGeometryLevelCount() - 1)
	{
		int32 row = width * 2 - 1;
		Vector3D *restrict bitangent = clothController->clothBitangent;

		#if C4SIMD

			const vec_float half = VecLoadVectorConstant<0x3F000000>();
			vec_float inverseTime = VecLoadSmearScalar(&kClothInverseMotionDeltaTime);

			vec_float p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[0].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[0].x);
			vec_float tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), p);
			vec_float btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), p);
			VecStore3D(tang, &tangentArray[0].x);
			VecStore3D(btng, &bitangent[0].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[0].x);

			finalPosition++;
			previousPosition++;
			bitangent++;
			int32 index = 2;

			for (machine i = 1; i < width - 1; i++)
			{
				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecMul(VecSub(VecLoadUnaligned(&finalPosition[1].x), VecLoadUnaligned(&finalPosition[-1].x)), half);
				btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), p);
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(btng, &bitangent[0].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += 2;
			}

			p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[index].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
			tang = VecSub(p, VecLoadUnaligned(&finalPosition[-1].x));
			btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), p);
			VecStore3D(tang, &tangentArray[index].x);
			VecStore3D(btng, &bitangent[0].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

			finalPosition++;
			previousPosition++;
			bitangent++;
			index += row + 1;

			for (machine j = 1; j < height - 1; j++)
			{
				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), p);
				btng = VecMul(VecSub(VecLoadUnaligned(&finalPosition[width].x), VecLoadUnaligned(&finalPosition[-width].x)), half);
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(btng, &bitangent[0].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += 2;

				for (machine i = 1; i < width - 1; i++)
				{
					p = VecLoadUnaligned(&finalPosition[0].x);
					VecStore3D(p, &positionArray[index].x);
					VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
					tang = VecMul(VecSub(VecLoadUnaligned(&finalPosition[1].x), VecLoadUnaligned(&finalPosition[-1].x)), half);
					btng = VecMul(VecSub(VecLoadUnaligned(&finalPosition[width].x), VecLoadUnaligned(&finalPosition[-width].x)), half);
					VecStore3D(tang, &tangentArray[index].x);
					VecStore3D(btng, &bitangent[0].x);
					VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

					finalPosition++;
					previousPosition++;
					bitangent++;
					index += 2;
				}

				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecSub(p, VecLoadUnaligned(&finalPosition[-1].x));
				btng = VecMul(VecSub(VecLoadUnaligned(&finalPosition[width].x), VecLoadUnaligned(&finalPosition[-width].x)), half);
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(btng, &bitangent[0].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += row + 1;
			}

			p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[index].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
			tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), p);
			btng = VecSub(p, VecLoadUnaligned(&finalPosition[-width].x));
			VecStore3D(tang, &tangentArray[index].x);
			VecStore3D(btng, &bitangent[0].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

			finalPosition++;
			previousPosition++;
			bitangent++;
			index += 2;

			for (machine i = 1; i < width - 1; i++)
			{
				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecMul(VecSub(VecLoadUnaligned(&finalPosition[1].x), VecLoadUnaligned(&finalPosition[-1].x)), half);
				btng = VecSub(p, VecLoadUnaligned(&finalPosition[-width].x));
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(btng, &bitangent[0].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += 2;
			}

			p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[index].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
			tang = VecSub(p, VecLoadUnaligned(&finalPosition[-1].x));
			btng = VecSub(p, VecLoadUnaligned(&finalPosition[-width].x));
			VecStore3D(tang, &tangentArray[index].x);
			VecStore3D(btng, &bitangent[0].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

			const vec_float quarter = VecLoadVectorConstant<0x3E800000>();
			const vec_float eighth = VecLoadVectorConstant<0x3E000000>();
			const vec_float three_halves = VecLoadVectorConstant<0x3FC00000>();

			for (machine j = 0; j < height; j++)
			{
				int32 k = j * row * 2 + 1;
				for (machine i = 1; i < width; i++)
				{
					vec_float p1 = VecLoadUnaligned(&positionArray[k - 1].x);
					vec_float p2 = VecLoadUnaligned(&positionArray[k + 1].x);
					vec_float v1 = VecLoadUnaligned(&velocityArray[k - 1].x);
					vec_float v2 = VecLoadUnaligned(&velocityArray[k + 1].x);
					vec_float t1 = VecLoadUnaligned(&tangentArray[k - 1].x);
					vec_float t2 = VecLoadUnaligned(&tangentArray[k + 1].x);

					VecStore3D(VecMadd(VecAdd(p1, p2), half, VecMul(VecSub(t1, t2), eighth)), &positionArray[k].x);
					VecStore3D(VecMul(VecAdd(v1, v2), half), &velocityArray[k].x);
					VecStore3D(VecNmsub(VecAdd(t1, t2), quarter, VecMul(VecSub(p2, p1), three_halves)), &tangentArray[k].x);
					k += 2;
				}
			}

			bitangent = clothController->clothBitangent;
			Vector4D *restrict const& bitangentArray = tangentArray;

			for (machine i = 0; i < width; i++)
			{
				int32 k = i * 2 + row;
				for (machine j = 1; j < height; j++)
				{
					vec_float p1 = VecLoadUnaligned(&positionArray[k - row].x);
					vec_float p2 = VecLoadUnaligned(&positionArray[k + row].x);
					vec_float v1 = VecLoadUnaligned(&velocityArray[k - row].x);
					vec_float v2 = VecLoadUnaligned(&velocityArray[k + row].x);
					vec_float b1 = VecLoadUnaligned(&bitangent[(j - 1) * width + i].x);
					vec_float b2 = VecLoadUnaligned(&bitangent[j * width + i].x);

					VecStore3D(VecMadd(VecAdd(p1, p2), half, VecMul(VecSub(b1, b2), eighth)), &positionArray[k].x);
					VecStore3D(VecMul(VecAdd(v1, v2), half), &velocityArray[k].x);
					VecStore3D(VecNmsub(VecAdd(b1, b2), quarter, VecMul(VecSub(p2, p1), three_halves)), &bitangentArray[k].x);
					k += row * 2;
				}
			}

			int32 m = 1;
			for (machine i = 1; i < width; i++)
			{
				vec_float p1 = VecLoadUnaligned(&positionArray[m].x);
				vec_float p2 = VecLoadUnaligned(&positionArray[m + row * 2].x);
				vec_float t = VecLoadUnaligned(&tangentArray[m].x);

				VecStore3D(VecCross3D(t, VecSub(p2, p1)), &normalArray[m].x);
				m += 2;
			}

			for (machine j = 1; j < height - 1; j++)
			{
				int32 k = j * row * 2 + 1;
				for (machine i = 1; i < width; i++)
				{
					vec_float p1 = VecLoadUnaligned(&positionArray[k - row * 2].x);
					vec_float p2 = VecLoadUnaligned(&positionArray[k + row * 2].x);
					vec_float t = VecLoadUnaligned(&tangentArray[k].x);

					VecStore3D(VecCross3D(t, VecSub(p2, p1)), &normalArray[k].x);
					k += 2;
				}
			}

			m = (height - 1) * row * 2 + 1;
			for (machine i = 1; i < width; i++)
			{
				vec_float p1 = VecLoadUnaligned(&positionArray[m - row * 2].x);
				vec_float p2 = VecLoadUnaligned(&positionArray[m].x);
				vec_float t = VecLoadUnaligned(&tangentArray[m].x);

				VecStore3D(VecCross3D(t, VecSub(p2, p1)), &normalArray[m].x);
				m += 2;
			}

			m = row;
			for (machine j = 1; j < height; j++)
			{
				vec_float p1 = VecLoadUnaligned(&positionArray[m].x);
				vec_float p2 = VecLoadUnaligned(&positionArray[m + 2].x);
				vec_float b = VecLoadUnaligned(&bitangentArray[m].x);
				vec_float t = VecSub(p2, p1);

				VecStore3D(VecCross3D(t, b), &normalArray[m].x);
				VecStore3D(t, &tangentArray[m].x);
				m += row * 2;
			}

			for (machine i = 1; i < width - 1; i++)
			{
				int32 k = i * 2 + row;
				for (machine j = 1; j < height; j++)
				{
					vec_float p1 = VecLoadUnaligned(&positionArray[k - 2].x);
					vec_float p2 = VecLoadUnaligned(&positionArray[k + 2].x);
					vec_float b = VecLoadUnaligned(&bitangentArray[k].x);
					vec_float t = VecMul(VecSub(p2, p1), half);

					VecStore3D(VecCross3D(t, b), &normalArray[k].x);
					VecStore3D(t, &tangentArray[k].x);
					k += row * 2;
				}
			}

			m = (width - 1) * 2 + row;
			for (machine j = 1; j < height; j++)
			{
				vec_float p1 = VecLoadUnaligned(&positionArray[m - 2].x);
				vec_float p2 = VecLoadUnaligned(&positionArray[m].x);
				vec_float b = VecLoadUnaligned(&bitangentArray[m].x);
				vec_float t = VecSub(p2, p1);

				VecStore3D(VecCross3D(t, b), &normalArray[m].x);
				VecStore3D(t, &tangentArray[m].x);
				m += row * 2;
			}

			for (machine j = 1; j < height; j++)
			{
				int32 k = (j * 2 - 1) * row + 1;
				for (machine i = 1; i < width; i++)
				{
					vec_float p1 = VecLoadUnaligned(&positionArray[k - 1].x);
					vec_float p2 = VecLoadUnaligned(&positionArray[k + 1].x);
					vec_float v1 = VecLoadUnaligned(&velocityArray[k - 1].x);
					vec_float v2 = VecLoadUnaligned(&velocityArray[k + 1].x);
					vec_float t1 = VecLoadUnaligned(&tangentArray[k - 1].x);
					vec_float t2 = VecLoadUnaligned(&tangentArray[k + 1].x);

					VecStore3D(VecMadd(VecAdd(p1, p2), half, VecMul(VecSub(t1, t2), eighth)), &positionArray[k].x);
					VecStore3D(VecMul(VecAdd(v1, v2), half), &velocityArray[k].x);
					tang = VecNmsub(VecAdd(t1, t2), quarter, VecMul(VecSub(p2, p1), three_halves));
					VecStore3D(tang, &tangentArray[k].x);
					VecStore3D(VecCross3D(tang, VecSub(VecLoadUnaligned(&positionArray[k + row].x), VecLoadUnaligned(&positionArray[k - row].x))), &normalArray[k].x);
					k += 2;
				}
			}

		#else

			positionArray[0] = finalPosition[0];
			velocityArray[0] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[0].GetVector3D() = finalPosition[1] - finalPosition[0];
			bitangent[0] = finalPosition[width] - finalPosition[0];
			normalArray[0] = tangentArray[0].GetVector3D() % bitangent[0];

			finalPosition++;
			previousPosition++;
			bitangent++;
			int32 index = 2;

			for (machine i = 1; i < width - 1; i++)
			{
				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = (finalPosition[1] - finalPosition[-1]) * 0.5F;
				bitangent[0] = finalPosition[width] - finalPosition[0];
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += 2;
			}

			positionArray[index] = finalPosition[0];
			velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[index].GetVector3D() = finalPosition[0] - finalPosition[-1];
			bitangent[0] = finalPosition[width] - finalPosition[0];
			normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

			finalPosition++;
			previousPosition++;
			bitangent++;
			index += row + 1;

			for (machine j = 1; j < height - 1; j++)
			{
				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = finalPosition[1] - finalPosition[0];
				bitangent[0] = (finalPosition[width] - finalPosition[-width]) * 0.5F;
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += 2;

				for (machine i = 1; i < width - 1; i++)
				{
					positionArray[index] = finalPosition[0];
					velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
					tangentArray[index].GetVector3D() = (finalPosition[1] - finalPosition[-1]) * 0.5F;
					bitangent[0] = (finalPosition[width] - finalPosition[-width]) * 0.5F;
					normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

					finalPosition++;
					previousPosition++;
					bitangent++;
					index += 2;
				}

				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = finalPosition[0] - finalPosition[-1];
				bitangent[0] = (finalPosition[width] - finalPosition[-width]) * 0.5F;
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += row + 1;
			}

			positionArray[index] = finalPosition[0];
			velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[index].GetVector3D() = finalPosition[1] - finalPosition[0];
			bitangent[0] = finalPosition[0] - finalPosition[-width];
			normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

			finalPosition++;
			previousPosition++;
			bitangent++;
			index += 2;

			for (machine i = 1; i < width - 1; i++)
			{
				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = (finalPosition[1] - finalPosition[-1]) * 0.5F;
				bitangent[0] = finalPosition[0] - finalPosition[-width];
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

				finalPosition++;
				previousPosition++;
				bitangent++;
				index += 2;
			}

			positionArray[index] = finalPosition[0];
			velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[index].GetVector3D() = finalPosition[0] - finalPosition[-1];
			bitangent[0] = finalPosition[0] - finalPosition[-width];
			normalArray[index] = tangentArray[index].GetVector3D() % bitangent[0];

			for (machine j = 0; j < height; j++)
			{
				int32 k = j * row * 2 + 1;
				for (machine i = 1; i < width; i++)
				{
					const Point3D& p1 = positionArray[k - 1];
					const Point3D& p2 = positionArray[k + 1];
					const Vector3D& v1 = velocityArray[k - 1];
					const Vector3D& v2 = velocityArray[k + 1];
					const Vector3D& t1 = tangentArray[k - 1].GetVector3D();
					const Vector3D& t2 = tangentArray[k + 1].GetVector3D();

					positionArray[k] = (p1 + p2) * 0.5F + (t1 - t2) * 0.125F;
					velocityArray[k] = (v1 + v2) * 0.5F;
					tangentArray[k].GetVector3D() = (p2 - p1) * 1.5F - (t1 + t2) * 0.25F;
					k += 2;
				}
			}

			bitangent = clothController->clothBitangent;
			Vector4D *restrict const& bitangentArray = tangentArray;

			for (machine i = 0; i < width; i++)
			{
				int32 k = i * 2 + row;
				for (machine j = 1; j < height; j++)
				{
					const Point3D& p1 = positionArray[k - row];
					const Point3D& p2 = positionArray[k + row];
					const Vector3D& v1 = velocityArray[k - row];
					const Vector3D& v2 = velocityArray[k + row];
					const Vector3D& b1 = bitangent[(j - 1) * width + i];
					const Vector3D& b2 = bitangent[j * width + i];

					positionArray[k] = (p1 + p2) * 0.5F + (b1 - b2) * 0.125F;
					velocityArray[k] = (v1 + v2) * 0.5F;
					bitangentArray[k].GetVector3D() = (p2 - p1) * 1.5F - (b1 + b2) * 0.25F;
					k += row * 2;
				}
			}

			int32 m = 1;
			for (machine i = 1; i < width; i++)
			{
				const Point3D& p1 = positionArray[m];
				const Point3D& p2 = positionArray[m + row * 2];

				normalArray[m] = tangentArray[m].GetVector3D() % (p2 - p1);
				m += 2;
			}

			for (machine j = 1; j < height - 1; j++)
			{
				int32 k = j * row * 2 + 1;
				for (machine i = 1; i < width; i++)
				{
					const Point3D& p1 = positionArray[k - row * 2];
					const Point3D& p2 = positionArray[k + row * 2];

					normalArray[k] = tangentArray[k].GetVector3D() % (p2 - p1);
					k += 2;
				}
			}

			m = (height - 1) * row * 2 + 1;
			for (machine i = 1; i < width; i++)
			{
				const Point3D& p1 = positionArray[m - row * 2];
				const Point3D& p2 = positionArray[m];

				normalArray[m] = tangentArray[m].GetVector3D() % (p2 - p1);
				m += 2;
			}

			m = row;
			for (machine j = 1; j < height; j++)
			{
				const Point3D& p1 = positionArray[m];
				const Point3D& p2 = positionArray[m + 2];

				Vector3D t = p2 - p1;
				normalArray[m] = t % bitangentArray[m].GetVector3D();
				tangentArray[m].GetVector3D() = t;
				m += row * 2;
			}

			for (machine i = 1; i < width - 1; i++)
			{
				int32 k = i * 2 + row;
				for (machine j = 1; j < height; j++)
				{
					const Point3D& p1 = positionArray[k - 2];
					const Point3D& p2 = positionArray[k + 2];

					Vector3D t = (p2 - p1) * 0.5F;
					normalArray[k] = t % bitangentArray[k].GetVector3D();
					tangentArray[k].GetVector3D() = t;
					k += row * 2;
				}
			}

			m = (width - 1) * 2 + row;
			for (machine j = 1; j < height; j++)
			{
				const Point3D& p1 = positionArray[m - 2];
				const Point3D& p2 = positionArray[m];

				Vector3D t = p2 - p1;
				normalArray[m] = t % bitangentArray[m].GetVector3D();
				tangentArray[m].GetVector3D() = t;
				m += row * 2;
			}

			for (machine j = 1; j < height; j++)
			{
				int32 k = (j * 2 - 1) * row + 1;
				for (machine i = 1; i < width; i++)
				{
					const Point3D& p1 = positionArray[k - 1];
					const Point3D& p2 = positionArray[k + 1];
					const Vector3D& v1 = velocityArray[k - 1];
					const Vector3D& v2 = velocityArray[k + 1];
					const Vector3D& t1 = tangentArray[k - 1].GetVector3D();
					const Vector3D& t2 = tangentArray[k + 1].GetVector3D();

					positionArray[k] = (p1 + p2) * 0.5F + (t1 - t2) * 0.125F;
					velocityArray[k] = (v1 + v2) * 0.5F;
					tangentArray[k].GetVector3D() = (p2 - p1) * 1.5F - (t1 + t2) * 0.25F;
					normalArray[k] = tangentArray[k].GetVector3D() % (positionArray[k + row] - positionArray[k - row]);
					k += 2;
				}
			}

		#endif
	}
	else
	{
		#if C4SIMD

			vec_float inverseTime = VecLoadSmearScalar(&kClothInverseMotionDeltaTime);

			vec_float p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[0].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[0].x);
			vec_float tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), p);
			vec_float btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), p);
			VecStore3D(tang, &tangentArray[0].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[0].x);

			finalPosition++;
			previousPosition++;
			int32 index = 1;

			for (machine i = 1; i < width - 1; i++)
			{
				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), VecLoadUnaligned(&finalPosition[-1].x));
				btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), p);
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				index++;
			}

			p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[index].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
			tang = VecSub(p, VecLoadUnaligned(&finalPosition[-1].x));
			btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), p);
			VecStore3D(tang, &tangentArray[index].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

			finalPosition++;
			previousPosition++;
			index++;

			for (machine j = 1; j < height - 1; j++)
			{
				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), p);
				btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), VecLoadUnaligned(&finalPosition[-width].x));
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				index++;

				for (machine i = 1; i < width - 1; i++)
				{
					p = VecLoadUnaligned(&finalPosition[0].x);
					VecStore3D(p, &positionArray[index].x);
					VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
					tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), VecLoadUnaligned(&finalPosition[-1].x));
					btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), VecLoadUnaligned(&finalPosition[-width].x));
					VecStore3D(tang, &tangentArray[index].x);
					VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

					finalPosition++;
					previousPosition++;
					index++;
				}

				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecSub(p, VecLoadUnaligned(&finalPosition[-1].x));
				btng = VecSub(VecLoadUnaligned(&finalPosition[width].x), VecLoadUnaligned(&finalPosition[-width].x));
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				index++;
			}

			p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[index].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
			tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), p);
			btng = VecSub(p, VecLoadUnaligned(&finalPosition[-width].x));
			VecStore3D(tang, &tangentArray[index].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

			finalPosition++;
			previousPosition++;
			index++;

			for (machine i = 1; i < width - 1; i++)
			{
				p = VecLoadUnaligned(&finalPosition[0].x);
				VecStore3D(p, &positionArray[index].x);
				VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
				tang = VecSub(VecLoadUnaligned(&finalPosition[1].x), VecLoadUnaligned(&finalPosition[-1].x));
				btng = VecSub(p, VecLoadUnaligned(&finalPosition[-width].x));
				VecStore3D(tang, &tangentArray[index].x);
				VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

				finalPosition++;
				previousPosition++;
				index++;
			}

			p = VecLoadUnaligned(&finalPosition[0].x);
			VecStore3D(p, &positionArray[index].x);
			VecStore3D(VecMul(VecSub(p, VecLoadUnaligned(&previousPosition[0].x)), inverseTime), &velocityArray[index].x);
			tang = VecSub(p, VecLoadUnaligned(&finalPosition[-1].x));
			btng = VecSub(p, VecLoadUnaligned(&finalPosition[-width].x));
			VecStore3D(tang, &tangentArray[index].x);
			VecStore3D(VecCross3D(tang, btng), &normalArray[index].x);

		#else

			positionArray[0] = finalPosition[0];
			velocityArray[0] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[0].GetVector3D() = finalPosition[1] - finalPosition[0];
			Vector3D bitangent = finalPosition[width] - finalPosition[0];
			normalArray[0] = tangentArray[0].GetVector3D() % bitangent;

			finalPosition++;
			previousPosition++;
			int32 index = 1;

			for (machine i = 1; i < width - 1; i++)
			{
				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = finalPosition[1] - finalPosition[-1];
				bitangent = finalPosition[width] - finalPosition[0];
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

				finalPosition++;
				previousPosition++;
				index++;
			}

			positionArray[index] = finalPosition[0];
			velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[index].GetVector3D() = finalPosition[0] - finalPosition[-1];
			bitangent = finalPosition[width] - finalPosition[0];
			normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

			finalPosition++;
			previousPosition++;
			index++;

			for (machine j = 1; j < height - 1; j++)
			{
				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = finalPosition[1] - finalPosition[0];
				bitangent = finalPosition[width] - finalPosition[-width];
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

				finalPosition++;
				previousPosition++;
				index++;

				for (machine i = 1; i < width - 1; i++)
				{
					positionArray[index] = finalPosition[0];
					velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
					tangentArray[index].GetVector3D() = finalPosition[1] - finalPosition[-1];
					bitangent = finalPosition[width] - finalPosition[-width];
					normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

					finalPosition++;
					previousPosition++;
					index++;
				}

				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = finalPosition[0] - finalPosition[-1];
				bitangent = finalPosition[width] - finalPosition[-width];
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

				finalPosition++;
				previousPosition++;
				index++;
			}

			positionArray[index] = finalPosition[0];
			velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[index].GetVector3D() = finalPosition[1] - finalPosition[0];
			bitangent = finalPosition[0] - finalPosition[-width];
			normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

			finalPosition++;
			previousPosition++;
			index++;

			for (machine i = 1; i < width - 1; i++)
			{
				positionArray[index] = finalPosition[0];
				velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
				tangentArray[index].GetVector3D() = finalPosition[1] - finalPosition[-1];
				bitangent = finalPosition[0] - finalPosition[-width];
				normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

				finalPosition++;
				previousPosition++;
				index++;
			}

			positionArray[index] = finalPosition[0];
			velocityArray[index] = (finalPosition[0] - previousPosition[0]) * kClothInverseMotionDeltaTime;
			tangentArray[index].GetVector3D() = finalPosition[0] - finalPosition[-1];
			bitangent = finalPosition[0] - finalPosition[-width];
			normalArray[index] = tangentArray[index].GetVector3D() % bitangent;

		#endif
	}

	int32 vertexCount = mesh->GetVertexCount() / 2;
	for (machine a = 0; a < vertexCount; a++)
	{
		positionArray[vertexCount] = *positionArray;
		velocityArray[vertexCount] = *velocityArray;
		normalArray[vertexCount].Set(-normalArray->x, -normalArray->y, -normalArray->z);
		tangentArray[vertexCount].GetVector3D() = tangentArray->GetVector3D();

		positionArray++;
		velocityArray++;
		normalArray++;
		tangentArray++;
	}
}

void ClothController::FinalizeClothUpdate(Job *job, void *cookie)
{
	ClothController *clothController = static_cast<ClothController *>(cookie);

	VertexBuffer *vertexBuffer = &clothController->clothVertexBuffer;
	if (vertexBuffer->Active())
	{
		volatile ClothVertex *restrict vertex = vertexBuffer->BeginUpdate<ClothVertex>();

		const Geometry *geometry = clothController->GetTargetNode();
		int32 vertexCount = geometry->GetVertexCount();

		const Point3D *position = clothController->clothPositionArray;
		const Vector3D *velocity = clothController->clothVelocityArray;
		const Vector3D *normal = clothController->clothNormalArray;
		const Vector4D *tangent = clothController->clothTangentArray;

		for (machine a = 0; a < vertexCount; a++)
		{
			vertex[a].position = position[a];
			vertex[a].velocity = velocity[a];
			vertex[a].normal = normal[a];
			vertex[a].tangent = tangent[a];
		}

		vertexBuffer->EndUpdate();
	}
}

// ZYUQURM
