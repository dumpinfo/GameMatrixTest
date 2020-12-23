 

#include "C4Lights.h"
#include "C4World.h"
#include "C4Cameras.h"
#include "C4Configuration.h"


using namespace C4;


const char C4::kConnectorKeyShadow[] = "%Shadow";


Light::Light(LightType type, LightType base) : Node(kNodeLight)
{
	lightType = type;
	baseLightType = base;

	connectedShadowSpace = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
}

Light::Light(const Light& light) : Node(light)
{
	lightType = light.lightType;
	baseLightType = light.baseLightType;

	connectedShadowSpace = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
}

Light::~Light()
{
}

Light *Light::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kLightInfinite:

		#if C4LEGACY

			case 'DPTH':
			case 'LAND':

		#endif

			return (new InfiniteLight);

		case kLightPoint:

			return (new PointLight);

		case kLightCube:

			return (new CubeLight);

		case kLightSpot:

			return (new SpotLight);
	}

	return (nullptr);
}

void Light::PackType(Packer& data) const
{
	Node::PackType(data);
	data << lightType;
}

void Light::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	const Node *exclusion = excludedNode;
	if ((exclusion) && (exclusion->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('EXCL', 4);
		data << exclusion->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void Light::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Light>(data, unpackFlags);
}

bool Light::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'EXCL':
		{
			int32	excludedIndex;
 
			data >> excludedIndex;
			data.AddNodeLink(excludedIndex, &ExcludedLinkProc, this);
			return (true); 
		}
	} 

	return (false);
} 

void Light::ExcludedLinkProc(Node *node, void *cookie) 
{ 
	Light *light = static_cast<Light *>(cookie);
	light->excludedNode = node;
}
 
int32 Light::GetCategorySettingCount(Type category) const
{
	int32 count = Node::GetCategorySettingCount(category);
	if (category == 'NODE')
	{
		count += 2 + kPerspectiveMaskSettingCount;
	}

	return (count);
}

Setting *Light::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == 'NODE')
	{
		int32 i = index - Node::GetCategorySettingCount(category);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kNodeLight, 'LITE'));
				return (new HeadingSetting('LITE', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kNodeLight, 'LITE', 'SHAR'));
				return (new BooleanSetting('LSHR', ((GetNodeFlags() & kNodeUnsharedObject) != 0), title));
			}

			return (GetPerspectiveMaskSetting(i - 2));
		}
	}

	return (Node::GetCategorySetting(category, index, flags));
}

void Light::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == 'NODE')
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'LSHR')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetNodeFlags(GetNodeFlags() | kNodeUnsharedObject);
			}
			else
			{
				SetNodeFlags(GetNodeFlags() & ~kNodeUnsharedObject);
			}
		}
		else if (!SetPerspectiveMaskSetting(setting))
		{
			Node::SetCategorySetting('NODE', setting);
		}
	}
}

int32 Light::GetInternalConnectorCount(void) const
{
	return (1);
}

const char *Light::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyShadow);
	}

	return (nullptr);
}

void Light::ProcessInternalConnectors(void)
{
	Node *node = GetConnectedNode(kConnectorKeyShadow);
	if ((!node) && (GetObject()->GetLightFlags() & kLightInstanceShadowSpace))
	{
		const Node *super = GetSuperNode();
		do
		{
			if (super->GetNodeType() == kNodeInstance)
			{
				node = super->GetConnectedNode(kConnectorKeyShadow);
				if (node)
				{
					break;
				}
			}

			super = super->GetSuperNode();
		} while (super);
	}

	connectedShadowSpace = static_cast<ShadowSpace *>(node);
}

bool Light::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyShadow)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpaceShadow);
		}

		return (false);
	}

	return (Node::ValidConnectedNode(key, node));
}

void Light::SetConnectedShadowSpace(ShadowSpace *shadowSpace)
{
	connectedShadowSpace = shadowSpace;
	SetConnectedNode(kConnectorKeyShadow, shadowSpace);
}

void Light::Neutralize(void)
{
	rootRegionList.Purge();
	Node::Neutralize();
}

void Light::InvalidateLightRegions(void)
{
	rootRegionList.Purge();
	Invalidate();
}


InfiniteLight::InfiniteLight() : Light(kLightInfinite, kLightInfinite)
{
	connectedMemberNode = nullptr;
}

InfiniteLight::InfiniteLight(LightType type) : Light(type, kLightInfinite)
{
	connectedMemberNode = nullptr;
}

InfiniteLight::InfiniteLight(const ColorRGB& color) : Light(kLightInfinite, kLightInfinite)
{
	SetNewObject(new InfiniteLightObject(color));

	connectedMemberNode = nullptr;
}

InfiniteLight::InfiniteLight(const InfiniteLight& infiniteLight) : Light(infiniteLight)
{
	connectedMemberNode = nullptr;
}

InfiniteLight::~InfiniteLight()
{
}

Node *InfiniteLight::Replicate(void) const
{
	return (new InfiniteLight(*this));
}

int32 InfiniteLight::GetInternalConnectorCount(void) const
{
	return (Light::GetInternalConnectorCount() + 1);
}

const char *InfiniteLight::GetInternalConnectorKey(int32 index) const
{
	int32 count = Light::GetInternalConnectorCount();
	if (index < count)
	{
		return (Light::GetInternalConnectorKey(index));
	}

	if (index == count)
	{
		return (kConnectorKeyMember);
	}

	return (nullptr);
}

void InfiniteLight::ProcessInternalConnectors(void)
{
	connectedMemberNode = GetConnectedNode(kConnectorKeyMember);

	Light::ProcessInternalConnectors();
}

void InfiniteLight::SetConnectedMemberNode(Node *node)
{
	connectedMemberNode = node;
	SetConnectedNode(kConnectorKeyMember, node);
}

void InfiniteLight::CalculateIllumination(LightRegion *region)
{
	Zone *zone = region->GetZone();
	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() | kZoneTraversalLocal);

	const Vector3D& lightDirection = GetWorldTransform()[2];

	unsigned_int32 portalFlagsMask = kPortalLightInhibit;
	if (GetObject()->GetLightFlags() & kLightStatic)
	{
		portalFlagsMask |= kPortalStaticLightInhibit;
	}

	const Portal *portal = zone->GetFirstPortal();
	while (portal)
	{
		if (portal->Enabled())
		{
			PortalType type = portal->GetPortalType();
			const PortalObject *portalObject = portal->GetObject();
			unsigned_int32 portalFlags = portalObject->GetPortalFlags();

			if ((type == kPortalDirect) || ((type == kPortalRemote) && (portalFlags & kPortalAllowRemoteLight)))
			{
				if ((portalFlags & portalFlagsMask) == 0)
				{
					Zone *connectedZone = portal->GetConnectedZone();
					if ((connectedZone) && (connectedZone->GetTraversalExclusionMask() == 0))
					{
						if ((portal->GetWorldPlane() ^ lightDirection) > 0.0F)
						{
							Point3D		temp[2][kMaxPortalVertexCount + kMaxPolyhedronFaceCount];
							int8		location[kMaxPortalVertexCount + kMaxPolyhedronFaceCount];

							int32 vertexCount = portalObject->GetVertexCount();
							const Point3D *vertex = portal->GetWorldVertexArray();

							int32 planeCount = region->GetPlaneCount();
							if (planeCount > 0)
							{
								const Antivector4D *planeArray = region->GetPlaneArray();
								for (machine a = 0; a < planeCount; a++)
								{
									Point3D *result = temp[a & 1];
									vertexCount = Math::ClipPolygon(vertexCount, vertex, planeArray[a], location, result);
									if (vertexCount == 0)
									{
										goto nextPortal;
									}

									vertex = result;
								}
							}

							vertexCount = Min(vertexCount, kMaxClippedPortalVertexCount);

							LightRegion *newRegion = new LightRegion(this, connectedZone, portal);
							newRegion->SetInfinitePolygonExtrusion(vertexCount, vertex, -lightDirection, -portal->GetWorldPlane());

							const ShadowSpace *shadowSpace = connectedZone->GetConnectedShadowSpace();
							if ((shadowSpace) || ((shadowSpace = GetConnectedShadowSpace()) != nullptr))
							{
								Polyhedron		polyhedron;

								if (Math::ClipPolyhedron(newRegion->GetRegionPolyhedron(), shadowSpace->GetInverseWorldTransform().GetRow(2), &polyhedron))
								{
									*newRegion->GetRegionPolyhedron() = polyhedron;
								}
							}

							region->AppendSubnode(newRegion);
							connectedZone->AddLightRegion(newRegion);

							CalculateIllumination(newRegion);
						}
					}
				}
			}
		}

		nextPortal:
		portal = portal->Next();
	}

	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() & ~kZoneTraversalLocal);
}

void InfiniteLight::EstablishRootRegions(Zone *zone, int32 forcedDepth)
{
	Zone *subzone = zone->GetFirstSubzone();
	while (subzone)
	{
		RootLightRegion *region = new RootLightRegion(this, subzone);
		rootRegionList.Append(region);
		subzone->AddLightRegion(region);
		region->SetPlaneCount(0);

		if (forcedDepth > 0)
		{
			EstablishRootRegions(subzone, forcedDepth - 1);
		}

		subzone = subzone->Next();
	}
}

void InfiniteLight::ProcessConnectedZoneMemberships(void)
{
	Node *node = connectedMemberNode;
	if (node)
	{
		if (node->GetNodeType() == kNodeZone)
		{
			Zone *zone = static_cast<Zone *>(node);
			if (zone->GetTraversalExclusionMask() == 0)
			{
				RootLightRegion *region = new RootLightRegion(this, zone);
				rootRegionList.Append(region);
				zone->AddLightRegion(region);
				region->SetPlaneCount(0);

				zone->SetTraversalExclusionMask(kZoneTraversalGlobal);
			}
		}
		else
		{
			const Hub *hub = node->GetHub();
			if (hub)
			{
				const Connector *connector = hub->GetFirstOutgoingEdge();
				while (connector)
				{
					node = connector->GetConnectorTarget();
					if ((node) && (node->GetNodeType() == kNodeZone))
					{
						Zone *zone = static_cast<Zone *>(node);
						if (zone->GetTraversalExclusionMask() == 0)
						{
							RootLightRegion *region = new RootLightRegion(this, zone);
							rootRegionList.Append(region);
							zone->AddLightRegion(region);
							region->SetPlaneCount(0);

							zone->SetTraversalExclusionMask(kZoneTraversalGlobal);
						}
					}

					connector = connector->GetNextOutgoingEdge();
				}
			}
		}
	}
}

void InfiniteLight::HandlePostprocessUpdate(void)
{
	rootRegionList.Purge();

	Zone *zone = GetOwningZone();

	unsigned_int32 lightFlags = GetObject()->GetLightFlags();
	if (!(lightFlags & kLightRootZoneInhibit))
	{
		RootLightRegion *region = new RootLightRegion(this, zone);
		rootRegionList.Append(region);
		zone->AddLightRegion(region);
		region->SetPlaneCount(0);

		zone->SetTraversalExclusionMask(kZoneTraversalGlobal);
	}

	int32 forcedDepth = GetForcedSubzoneDepth();
	if (forcedDepth > 0)
	{
		EstablishRootRegions(zone, forcedDepth - 1);
	}

	ProcessConnectedZoneMemberships();

	if (!(lightFlags & kLightPortalInhibit))
	{
		RootLightRegion *region = rootRegionList.First();
		while (region)
		{
			CalculateIllumination(region);
			region = region->Next();
		}
	}

	RootLightRegion *region = rootRegionList.First();
	while (region)
	{
		region->GetZone()->SetTraversalExclusionMask(0);
		region = region->Next();
	}
}

const LightShadowData *InfiniteLight::CalculateShadowData(const FrustumCamera *camera)
{
	const Transform4D& lightTransform = GetInverseWorldTransform();
	float zlimit = 0.0F;

	const ShadowSpace *shadowSpace = GetConnectedShadowSpace();
	if (shadowSpace)
	{
		const Transform4D& shadowTransform = shadowSpace->GetWorldTransform();
		const Vector3D& size = shadowSpace->GetObject()->GetBoxSize();

		Point3D p0 = lightTransform * shadowTransform.GetTranslation();
		Vector3D x = lightTransform * shadowTransform[0] * size.x;
		Vector3D y = lightTransform * shadowTransform[1] * size.y;
		Vector3D z = lightTransform * shadowTransform[2] * size.z;

		Point3D p1 = p0 + x;
		Point3D p2 = p1 + y;
		Point3D p3 = p0 + y;
		Point3D p4 = p0 + z;
		Point3D p5 = p1 + z;
		Point3D p6 = p2 + z;
		Point3D p7 = p3 + z;

		zlimit = Fmax(Fmax(p0.z, p1.z, p2.z, p3.z), Fmax(p4.z, p5.z, p6.z, p7.z));
	}
	else
	{
		const BoundingSphere *sphere = GetOwningZone()->GetBoundingSphere();
		zlimit = (GetInverseWorldTransform().GetRow(2) ^ sphere->GetCenter()) + sphere->GetRadius();
	}

	const Transform4D& worldTransform = GetWorldTransform();
	const Transform4D& cameraTransform = camera->GetWorldTransform();

	Vector3D right = lightTransform * cameraTransform[0];
	Vector3D down = lightTransform * cameraTransform[1];
	Vector3D view = lightTransform * cameraTransform[2];
	Point3D p0 = lightTransform * camera->GetWorldPosition();

	const FrustumCameraObject *cameraObject = camera->GetObject();
	float inverseFocal = 1.0F / cameraObject->GetFocalLength();
	float aspect = cameraObject->GetAspectRatio();

	float inverseShadowMapSize = 1.0F / (float) TheGraphicsMgr->GetInfiniteShadowMapSize();

	const Range<float> *cascadeRange = GetObject()->GetCascadeRangeArray();

	for (machine a = 0; a < kMaxShadowCascadeCount; a++)
	{
		float e1 = cascadeRange[a].min;
		float e2 = cascadeRange[a].max;

		Point3D center1 = p0 + view * e1;
		Point3D center2 = p0 + view * e2;
		Vector3D v1 = right * (e1 * inverseFocal);
		Vector3D w1 = down * (e1 * aspect * inverseFocal);
		Vector3D v2 = right * (e2 * inverseFocal);
		Vector3D w2 = down * (e2 * aspect * inverseFocal);

		Point3D p1 = center1 + v1 + w1;
		Point3D p2 = center1 + v1 - w1;
		Point3D p3 = center1 - v1 + w1;
		Point3D p4 = center1 - v1 - w1;

		shadowData[a].cascadePolygon[0] = worldTransform * p1;
		shadowData[a].cascadePolygon[1] = worldTransform * p2;
		shadowData[a].cascadePolygon[2] = worldTransform * p3;
		shadowData[a].cascadePolygon[3] = worldTransform * p4;

		Point3D q1 = center2 + v2 + w2;
		Point3D q2 = center2 + v2 - w2;
		Point3D q3 = center2 - v2 + w2;
		Point3D q4 = center2 - v2 - w2;

		float xmin = Fmin(Fmin(p1.x, p2.x, p3.x, p4.x), Fmin(q1.x, q2.x, q3.x, q4.x));
		float xmax = Fmax(Fmax(p1.x, p2.x, p3.x, p4.x), Fmax(q1.x, q2.x, q3.x, q4.x));
		float ymin = Fmin(Fmin(p1.y, p2.y, p3.y, p4.y), Fmin(q1.y, q2.y, q3.y, q4.y));
		float ymax = Fmax(Fmax(p1.y, p2.y, p3.y, p4.y), Fmax(q1.y, q2.y, q3.y, q4.y));
		float zmin = Fmin(Fmin(p1.z, p2.z, p3.z, p4.z), Fmin(q1.z, q2.z, q3.z, q4.z));
		float zmax = Fmax(Fmax(p1.z, p2.z, p3.z, p4.z), Fmax(q1.z, q2.z, q3.z, q4.z));

		zmax = Fmax(zmax, zlimit);
		float dz = zmax - zmin;

		float size = PositiveCeil(Fmax(Magnitude(p1 - q4), Magnitude(q1 - q4)));
		shadowData[a].shadowSize.Set(size, size, dz);

		float inverseSize = 1.0F / size;
		shadowData[a].inverseShadowSize.Set(inverseSize, inverseSize, 1.0F / dz);

		float texel = size * inverseShadowMapSize;
		float inverseTexel = 1.0F / texel;
		shadowData[a].texelSize = texel;

		float x = (xmin + xmax) * 0.5F;
		float y = (ymin + ymax) * 0.5F;
		x = Floor(x * inverseTexel + 0.5F) * texel;
		y = Floor(y * inverseTexel + 0.5F) * texel;
		shadowData[a].shadowPosition.Set(x, y, zmax);

		shadowData[a].minDepth = e1;
		shadowData[a].nearPlane.Set(cameraTransform[2], worldTransform * center1);
		shadowData[a].farPlane.Set(-cameraTransform[2], worldTransform * center2);

		if (a < kMaxShadowCascadeCount - 1)
		{
			float nextStart = cascadeRange[a + 1].min;
			shadowData[a].cascadePlane.Set(cameraTransform[2] / (e2 - nextStart), worldTransform * (p0 + view * nextStart));
		}
		else
		{
			shadowData[a].cascadePlane.Set(0.0F, 0.0F, 0.0F, 0.0F);
		}
	}

	return (shadowData);
}


PointLight::PointLight() : Light(kLightPoint, kLightPoint)
{
}

PointLight::PointLight(LightType type) : Light(type, kLightPoint)
{
}

PointLight::PointLight(const ColorRGB& color, float range) : Light(kLightPoint, kLightPoint)
{
	SetNewObject(new PointLightObject(color, range));
}

PointLight::PointLight(const PointLight& pointLight) : Light(pointLight)
{
}

PointLight::~PointLight()
{
}

Node *PointLight::Replicate(void) const
{
	return (new PointLight(*this));
}

void PointLight::Preprocess(void)
{
	Light::Preprocess();

	const PointLightObject *object = GetObject();
	const Range<float> fadeDistance = object->GetFadeDistance();

	float m = fadeDistance.max;
	if (m < K::min_float)
	{
		fadeScale = 1.0F;
		fadeOffset = 0.0F;
	}
	else
	{
		float f = 1.0F / (m - fadeDistance.min);
		fadeScale = -f;
		fadeOffset = f * m;
	}
}

void PointLight::CalculateIllumination(LightRegion *region)
{
	Zone *zone = region->GetZone();
	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() | kZoneTraversalLocal);

	const Point3D& lightPosition = GetWorldPosition();
	const PointLightObject *lightObject = GetObject();
	float lightRange = lightObject->GetLightRange() + lightObject->GetConfinementRadius();

	if ((zone->GetFirstSubzone()) || (!zone->GetObject()->InteriorSphere(zone->GetInverseWorldTransform() * lightPosition, lightRange)))
	{
		unsigned_int32 lightFlags = lightObject->GetLightFlags();
		unsigned_int32 portalFlagsMask = kPortalLightInhibit;
		if ((lightFlags & (kLightStatic | kLightConfined)) != 0)
		{
			portalFlagsMask |= kPortalStaticLightInhibit;
		}

		const Portal *portal = zone->GetFirstPortal();
		while (portal)
		{
			if (portal->Enabled())
			{
				PortalType type = portal->GetPortalType();
				const PortalObject *portalObject = portal->GetObject();
				unsigned_int32 portalFlags = portalObject->GetPortalFlags();

				if ((type == kPortalDirect) || ((type == kPortalRemote) && (portalFlags & kPortalAllowRemoteLight)))
				{
					if ((portalFlags & portalFlagsMask) == 0)
					{
						Zone *connectedZone = portal->GetConnectedZone();
						if ((connectedZone) && (connectedZone->GetTraversalExclusionMask() == 0))
						{
							float distance = portal->GetWorldPlane() ^ lightPosition;
							if ((distance < lightRange) && (!(distance < 0.0F)))
							{
								const BoundingSphere *sphere = portal->GetBoundingSphere();
								float r = sphere->GetRadius() + lightRange;
								if (SquaredMag(sphere->GetCenter() - lightPosition) < r * r)
								{
									LightRegion		*newRegion;

									float confinementRadius = lightObject->GetConfinementRadius();
									if (distance - confinementRadius > kMinPortalClipDistance)
									{
										Point3D		temp[2][kMaxPortalVertexCount + kMaxPolyhedronFaceCount];
										int8		location[kMaxPortalVertexCount + kMaxPolyhedronFaceCount];

										int32 vertexCount = portalObject->GetVertexCount();
										const Point3D *vertex = portal->GetWorldVertexArray();

										int32 planeCount = region->GetPlaneCount();
										if (planeCount > 0)
										{
											const Antivector4D *planeArray = region->GetPlaneArray();
											for (machine a = 0; a < planeCount; a++)
											{
												Point3D *result = temp[a & 1];
												vertexCount = Math::ClipPolygon(vertexCount, vertex, planeArray[a], location, result);
												if (vertexCount == 0)
												{
													goto nextPortal;
												}

												vertex = result;
											}
										}

										vertexCount = Min(vertexCount, kMaxClippedPortalVertexCount);

										newRegion = new LightRegion(this, connectedZone, portal);
										newRegion->SetPointPolygonExtrusion(vertexCount, vertex, lightPosition, lightRange, confinementRadius, portal);
									}
									else
									{
										newRegion = new LightRegion(this, connectedZone, portal, region);
									}

									region->AppendSubnode(newRegion);
									connectedZone->AddLightRegion(newRegion);

									CalculateIllumination(newRegion);
								}
							}
						}
					}
				}
			}

			nextPortal:
			portal = portal->Next();
		}
	}

	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() & ~kZoneTraversalLocal);
}

bool PointLight::EstablishRootRegions(Zone *zone, int32 maxDepth, int32 forcedDepth)
{
	if (forcedDepth < 0)
	{
		if (zone->GetObject()->InteriorPoint(zone->GetInverseWorldTransform() * GetWorldPosition()))
		{
			bool covered = false;

			if (maxDepth > 0)
			{
				Zone *subzone = zone->GetFirstSubzone();
				while (subzone)
				{
					covered |= EstablishRootRegions(subzone, maxDepth - 1, -1);
					subzone = subzone->Next();
				}
			}

			if (!covered)
			{
				RootLightRegion *region = new RootLightRegion(this, zone);
				rootRegionList.Append(region);
				zone->AddLightRegion(region);
				region->SetPlaneCount(0);
			}

			return (true);
		}
	}
	else
	{
		RootLightRegion *region = new RootLightRegion(this, zone);
		rootRegionList.Append(region);
		zone->AddLightRegion(region);
		region->SetPlaneCount(0);

		if (maxDepth > 0)
		{
			Zone *subzone = zone->GetFirstSubzone();
			while (subzone)
			{
				EstablishRootRegions(subzone, maxDepth - 1, forcedDepth - 1);
				subzone = subzone->Next();
			}
		}
	}

	return (false);
}

void PointLight::HandlePostprocessUpdate(void)
{
	const PointLightObject *object = GetObject();
	unsigned_int32 lightFlags = object->GetLightFlags();

	if (!rootRegionList.Empty())
	{
		if (lightFlags & kLightConfined)
		{
			return;
		}

		rootRegionList.Purge();
	}

	float lightRange = object->GetLightRange() + object->GetConfinementRadius();

	const Point3D& lightPosition = GetWorldPosition();
	SetWorldBoundingBox(Point3D(lightPosition.x - lightRange, lightPosition.y - lightRange, lightPosition.z - lightRange), Point3D(lightPosition.x + lightRange, lightPosition.y + lightRange, lightPosition.z + lightRange));

	EstablishRootRegions(GetOwningZone(), GetMaxSubzoneDepth(), GetForcedSubzoneDepth());

	if (!(lightFlags & kLightPortalInhibit))
	{
		RootLightRegion *region = rootRegionList.First();
		while (region)
		{
			region->GetZone()->SetTraversalExclusionMask(kZoneTraversalGlobal);
			region = region->Next();
		}

		region = rootRegionList.First();
		while (region)
		{
			CalculateIllumination(region);
			region = region->Next();
		}

		region = rootRegionList.First();
		while (region)
		{
			region->GetZone()->SetTraversalExclusionMask(0);
			region = region->Next();
		}
	}
}


CubeLight::CubeLight() : PointLight(kLightCube)
{
}

CubeLight::CubeLight(const ColorRGB& color, float range, const char *name) : PointLight(kLightCube)
{
	SetNewObject(new CubeLightObject(color, range, name));
}

CubeLight::CubeLight(const CubeLight& cubeLight) : PointLight(cubeLight)
{
}

CubeLight::~CubeLight()
{
}

Node *CubeLight::Replicate(void) const
{
	return (new CubeLight(*this));
}


SpotLight::SpotLight() : PointLight(kLightSpot)
{
}

SpotLight::SpotLight(const ColorRGB& color, float range, float apex, const char *name) : PointLight(kLightSpot)
{
	SetNewObject(new SpotLightObject(color, range, apex, name));
}

SpotLight::SpotLight(const SpotLight& spotLight) : PointLight(spotLight)
{
}

SpotLight::~SpotLight()
{
}

Node *SpotLight::Replicate(void) const
{
	return (new SpotLight(*this));
}

void SpotLight::HandlePostprocessUpdate(void)
{
	Point3D		lightVertex[4];
	Box3D		lightBox;

	rootRegionList.Purge();

	const SpotLightObject *object = GetObject();
	float e = object->GetApexTangent();
	float a = object->GetAspectRatio();
	float lightRange = object->GetLightRange();
	float x = lightRange / e;
	float y = x * a;

	const Transform4D& lightTransform = GetWorldTransform();
	lightVertex[0] = lightTransform * Point3D(-x, y, lightRange);
	lightVertex[1] = lightTransform * Point3D(-x, -y, lightRange);
	lightVertex[2] = lightTransform * Point3D(x, -y, lightRange);
	lightVertex[3] = lightTransform * Point3D(x, y, lightRange);

	const Point3D& lightPosition = GetWorldPosition();
	lightBox.Calculate(lightPosition, 4, lightVertex);
	SetWorldBoundingBox(lightBox);

	EstablishRootRegions(GetOwningZone(), GetMaxSubzoneDepth(), GetForcedSubzoneDepth());

	float r1 = InverseSqrt(e * e + 1.0F);
	float r2 = InverseSqrt(e * e + a * a);

	RootLightRegion *region = rootRegionList.First();
	while (region)
	{
		region->GetZone()->SetTraversalExclusionMask(kZoneTraversalGlobal);
		region = region->Next();
	}

	region = rootRegionList.First();
	while (region)
	{
		region->SetAuxiliaryPlaneCount(1);

		Polyhedron *polyhedron = region->GetRegionPolyhedron();
		polyhedron->planeCount = 6;
		polyhedron->vertexCount = 5;
		polyhedron->edgeCount = 8;
		polyhedron->faceCount = 5;

		polyhedron->plane[0].Set(e * r1, 0.0F, r1, 0.0F);
		polyhedron->plane[1].Set(0.0F, e * r2, a * r2, 0.0F);
		polyhedron->plane[2].Set(-e * r1, 0.0F, r1, 0.0F);
		polyhedron->plane[3].Set(0.0F, -e * r2, a * r2, 0.0F);

		const Transform4D& transform = GetInverseWorldTransform();
		for (machine i = 0; i < 4; i++)
		{
			polyhedron->plane[i] = polyhedron->plane[i] * transform;
			polyhedron->vertex[i] = lightVertex[i];

			Edge *e1 = &polyhedron->edge[i];
			e1->vertexIndex[0] = 4;
			e1->vertexIndex[1] = (unsigned_int8) i;
			e1->faceIndex[0] = (unsigned_int8) i;
			e1->faceIndex[1] = (unsigned_int8) ((i - 1) & 3);

			Edge *e2 = e1 + 4;
			e2->vertexIndex[0] = (unsigned_int8) ((i + 1) & 3);
			e2->vertexIndex[1] = (unsigned_int8) i;
			e2->faceIndex[0] = 4;
			e2->faceIndex[1] = (unsigned_int8) i;

			Face *face = &polyhedron->face[i];
			face->edgeCount = 3;
			face->edgeIndex[0] = (unsigned_int8) i;
			face->edgeIndex[1] = (unsigned_int8) (i + 4);
			face->edgeIndex[2] = (unsigned_int8) ((i + 1) & 3);
		}

		float d = lightTransform[2] * GetWorldPosition();
		polyhedron->plane[4].Set(-lightTransform[2], d + lightRange);
		polyhedron->plane[5].Set(lightTransform[2], -d);
		polyhedron->vertex[4] = lightPosition;

		polyhedron->face[4].edgeCount = 4;
		polyhedron->face[4].edgeIndex[0] = 4;
		polyhedron->face[4].edgeIndex[1] = 5;
		polyhedron->face[4].edgeIndex[2] = 6;
		polyhedron->face[4].edgeIndex[3] = 7;

		CalculateIllumination(region);

		region = region->Next();
	}

	region = rootRegionList.First();
	while (region)
	{
		region->GetZone()->SetTraversalExclusionMask(0);
		region = region->Next();
	}
}

// ZYUQURM
