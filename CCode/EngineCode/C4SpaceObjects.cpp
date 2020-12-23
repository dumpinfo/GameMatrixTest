 

#include "C4SpaceObjects.h"
#include "C4World.h"
#include "C4Paint.h"


using namespace C4;


SpaceObject::SpaceObject(SpaceType type, Volume *volume) :
		Object(kObjectSpace),
		VolumeObject(volume)
{
	spaceType = type;
}

SpaceObject::~SpaceObject()
{
}

SpaceObject *SpaceObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kSpaceFog:

			return (new FogSpaceObject);

		case kSpaceShadow:

			return (new ShadowSpaceObject);

		case kSpaceRadiosity:

			return (new RadiositySpaceObject);

		case kSpaceAcoustics:

			return (new AcousticsSpaceObject);

		case kSpaceOcclusion:

			return (new OcclusionSpaceObject);

		case kSpacePaint:

			return (new PaintSpaceObject);

		case kSpacePhysics:

			return (new PhysicsSpaceObject);
	}

	return (nullptr);
}

void SpaceObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << spaceType;
}

void SpaceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PackVolume(data, packFlags);
}

void SpaceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackVolume(data, unpackFlags);
}

int32 SpaceObject::GetObjectSize(float *size) const
{
	return (GetVolumeObjectSize(size));
}

void SpaceObject::SetObjectSize(const float *size)
{
	SetVolumeObjectSize(size);
}


FogSpaceObject::FogSpaceObject() : SpaceObject(kSpaceFog, this)
{
}

FogSpaceObject::FogSpaceObject(const Vector2D& size) :
		SpaceObject(kSpaceFog, this),
		PlateVolume(size)
{
	fogSpaceFlags = 0;

	fogColor.Set(1.0F, 1.0F, 1.0F, 1.0F);
	fogDensity = 0.05F;
	fogFunction = kFogFunctionConstant;

	SetOcclusionIntensity(252.0F / 256.0F);
}

FogSpaceObject::~FogSpaceObject()
{ 
}

void FogSpaceObject::Pack(Packer& data, unsigned_int32 packFlags) const 
{
	SpaceObject::Pack(data, packFlags); 

	data << ChunkHeader('FLAG', 4);
	data << fogSpaceFlags; 

	data << ChunkHeader('PARM', sizeof(ColorRGBA) + 4); 
	data << fogColor; 
	data << fogDensity;

	data << ChunkHeader('FUNC', 4);
	data << fogFunction; 

	data << ChunkHeader('OCCL', 4);
	data << occlusionIntensity;

	data << TerminatorChunk;
}

void FogSpaceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	SpaceObject::Unpack(data, unpackFlags);
	UnpackChunkList<FogSpaceObject>(data, unpackFlags);
}

bool FogSpaceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> fogSpaceFlags;
			return (true);

		case 'PARM':

			data >> fogColor;
			data >> fogDensity;
			return (true);

		case 'FUNC':

			data >> fogFunction;
			return (true);

		case 'OCCL':
		{
			float	intensity;

			data >> intensity;
			SetOcclusionIntensity(intensity);
			return (true);
		}
	}

	return (false);
}

int32 FogSpaceObject::GetCategoryCount(void) const
{
	return (1);
}

Type FogSpaceObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kSpaceFog));
		return (kSpaceFog);
	}

	return (0);
}

int32 FogSpaceObject::GetCategorySettingCount(Type category) const
{
	if (category == kSpaceFog)
	{
		return (7);
	}

	return (0);
}

Setting *FogSpaceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kSpaceFog)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kSpaceFog, 'FOG '));
			return (new HeadingSetting(kSpaceFog, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kSpaceFog, 'FOG ', 'COLR'));
			const char *picker = table->GetString(StringID(kSpaceFog, 'FOG ', 'PICK'));
			return (new ColorSetting('COLR', fogColor, title, picker));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kSpaceFog, 'FOG ', 'DENS'));
			return (new TextSetting('DENS', fogDensity, title));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kSpaceFog, 'FOG ', 'FUNC'));
			MenuSetting *menu = new MenuSetting('FUNC', (fogFunction != kFogFunctionConstant), title, 2);

			menu->SetMenuItemString(0, table->GetString(StringID(kSpaceFog, 'FOG ', 'FUNC', kFogFunctionConstant)));
			menu->SetMenuItemString(1, table->GetString(StringID(kSpaceFog, 'FOG ', 'FUNC', kFogFunctionLinear)));

			return (menu);
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kSpaceFog, 'FOG ', 'DSTO'));
			return (new BooleanSetting('DSTO', ((fogSpaceFlags & kFogSpaceDistanceOcclusion) != 0), title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID(kSpaceFog, 'FOG ', 'DEPO'));
			return (new BooleanSetting('DEPO', ((fogSpaceFlags & kFogSpaceDepthOcclusion) != 0), title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID(kSpaceFog, 'FOG ', 'OINT'));
			return (new IntegerSetting('OINT', (int32) (occlusionIntensity * 256.0F + 0.5F), title, 224, 255, 1));
		}
	}

	return (nullptr);
}

void FogSpaceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kSpaceFog)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			fogColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'DENS')
		{
			fogDensity = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), 1.0e-10F);
		}
		else if (identifier == 'FUNC')
		{
			fogFunction = (static_cast<const MenuSetting *>(setting)->GetMenuSelection() == 0) ? kFogFunctionConstant : kFogFunctionLinear;
		}
		else if (identifier == 'DSTO')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				fogSpaceFlags |= kFogSpaceDistanceOcclusion;
			}
			else
			{
				fogSpaceFlags &= ~kFogSpaceDistanceOcclusion;
			}
		}
		else if (identifier == 'DEPO')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				fogSpaceFlags |= kFogSpaceDepthOcclusion;
			}
			else
			{
				fogSpaceFlags &= ~kFogSpaceDepthOcclusion;
			}
		}
		else if (identifier == 'OINT')
		{
			int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
			SetOcclusionIntensity((float) value * 0.00390625F);
		}
	}
}

void FogSpaceObject::SetOcclusionIntensity(float intensity)
{
	occlusionIntensity = intensity;
	occlusionConstant = -Log(1.0F - intensity);
}

float FogSpaceObject::CalculateOcclusionDistance(float F_wedge_C) const
{
	/*	This function calculates the horizontal distance (relative to the fog plane) at which points
		are completely fogged and do not need to be rendered. If the fog space uses a constant density
		function, then this distance d simply solves the equation

			f = exp(-rho * d),

		where f is some fraction of light that is considered small enough to be fully fogged. If the
		fog space uses a linear density function, then things get much more complicated.

		F = fog plane with normal pointing away from the fog
		P = rendered point
		C = camera position
		V = unnormalized direction to camera C - P
		rho = linear fog density

		See "Unified Distance Formulas for Halfspace Fog" for the derivation of the following formula
		that calculates the log fraction g(P) of light reaching the camera, where the density rho has
		been substituted for a/2:

			g(P) = -rho * |V| * (F ∧ P + F ∧ C)

		For given values F ∧ C and g, we find the minimum distance parallel to the fog plane at which
		all points P further away are fogged at least as much as g by determining the depth of a point
		P beneath the fog plane at which the projection of V onto the fog plane has the maximum length.
		This depth is always either zero or some value greater than half the depth of the camera, and we
		must take the greater of the distances calculated at these two depths when they can both be calculated.

		Let d be the length of the projection of V onto the fog plane. Then d and the difference in
		depths F ∧ P - F ∧ C form the sides of a right triangle where V is the hypotenuse. If we set
		F ∧ P = t(F ∧ C), then we have:

			(t - 1)²(F ∧ C)² + d² = V²

		The formula for g(P) can then be written as:

			g(P) = -rho (t + 1)(F ∧ C) √((t - 1)²(F ∧ C)² + d²)

		Squaring both sides and solving for d² gives us the following, where m = g(P) / rho:

						 m²
			d² = ────────────────── - (t - 1)²(F ∧ C)²
				  (t + 1)²(F ∧ C)²

		To determine the value of t for which d² is maximized, we set the derivative equal to zero as
		follows, where k = m² / (F ∧ C)⁴. After simplifying, we name this function φ(t):

			φ(t) = t⁴ + 2t³ - 2t + k - 1 = 0

		This is always positive for t = 1, and it has a local minimum at exactly t = 1/2, so there is a
		root in (1/2, 1) if φ(t) evaluates to a negative value at t = 1/2, which happens when k < 27/16.
		A couple iterations of Newton's method starting at t = 1 quickly converges to this root.
		The slope at t = 1 is always 8, so the first iteration can be calculated explicitly:

			t₁ = 1 - k / 8

		The second iteration is then calculated normally:

			t₂ = t₁ - φ(t₁) / φ′(t₁)

		The value of t₂ is plugged into the formula for d² to calculate the maximum distance. If it's the
		case that m > (F ∧ C)², then we also calculate d for t = 0 and take it instead if it's greater.
		Note that both solutions exist precisely when k is in the range (1, 27/16).
	*/

	float m = occlusionConstant / fogDensity;
	if (fogFunction == kFogFunctionConstant)
	{
		return (m);
	}

	float d = 0.0F;
	float m2 = m * m;
	float z2 = F_wedge_C * F_wedge_C;
	float zinv = 1.0F / F_wedge_C;
	float zinv2 = zinv * zinv;
	float k = m2 * zinv2 * zinv2;

	if (k < 1.6875F)
	{
		float t = 1.0F - k * 0.125F;
		float t2 = t * t;
		t -= (((t + 2.0F) * t2 - 2.0F) * t + (k - 1.0F)) / ((t * 4.0F + 6.0F) * t2 - 2.0F);

		float tp = t + 1.0F;
		float tm = t - 1.0F;
		d = Sqrt(m2 * zinv2 / (tp * tp) - tm * tm * z2);
	}

	if (m > z2)
	{
		d = Fmax(Sqrt(m2 * zinv2 - z2), d);
	}

	return (d);
}

float FogSpaceObject::CalculateOcclusionDepth(float F_wedge_C) const
{
	/*	This function returns the minimum depth d perpendicular to the fog plane F and beneath the
		camera position C at which points P are completely fogged, assuming that F ∧ C ≤ 0. For a
		linear density function, d must solve this equation:

			g(P) = -rho * d * (F ∧ P + F ∧ C)

		Since d = F ∧ C - F ∧ P, we can make the following substitution:

			F ∧ P + F ∧ C = 2(F ∧ C) - d,

		This give us the following, where m = g(P) / rho:

			d² - 2d(F ∧ C) - m = 0.

		This has the following solution:

			d = F ∧ C + √((F ∧ C)² + m)

		The value of the radical is always greater than -F ∧ C, so d is always positive.
	*/

	float m = occlusionConstant / fogDensity;
	if (fogFunction == kFogFunctionConstant)
	{
		return (m);
	}

	return (F_wedge_C + Sqrt(F_wedge_C * F_wedge_C + m));
}


ShadowSpaceObject::ShadowSpaceObject() : SpaceObject(kSpaceShadow, this)
{
}

ShadowSpaceObject::ShadowSpaceObject(const Vector3D& size) :
		SpaceObject(kSpaceShadow, this),
		BoxVolume(size)
{
}

ShadowSpaceObject::~ShadowSpaceObject()
{
}


RadiositySpaceObject::RadiositySpaceObject() : SpaceObject(kSpaceRadiosity, this)
{
	intensityScale = 1.0F;
	saturationScale = 1.0F;

	radiosityImage[0] = nullptr;
	radiosityImage[1] = nullptr;
	radiosityTexture[0] = nullptr;
	radiosityTexture[1] = nullptr;
}

RadiositySpaceObject::RadiositySpaceObject(const Vector3D& size, const Integer3D& resolution) :
		SpaceObject(kSpaceRadiosity, this),
		BoxVolume(size)
{
	radiosityResolution = resolution;
	UpdateRadiosityResolution();

	intensityScale = 1.0F;
	saturationScale = 1.0F;

	CreateRadiosityImages();
	CreateRadiosityTextures();
}

RadiositySpaceObject::~RadiositySpaceObject()
{
	if (radiosityTexture[0])
	{
		radiosityTexture[1]->Release();
		radiosityTexture[0]->Release();
	}

	delete[] radiosityImage[0];
}

void RadiositySpaceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	SpaceObject::Pack(data, packFlags);

	data << ChunkHeader('RESO', sizeof(Integer3D));
	data << radiosityResolution;

	data << ChunkHeader('INSC', 4);
	data << intensityScale;

	data << ChunkHeader('STSC', 4);
	data << saturationScale;

	if (radiosityImage[0])
	{
		unsigned_int32 dataSize = imageVoxelCount * (sizeof(Color4C) * 2);
		unsigned_int8 *compressedData = new unsigned_int8[dataSize];

		unsigned_int32 compressedSize = Comp::CompressData(radiosityImage[0], dataSize, compressedData);
		if (compressedSize != 0)
		{
			data << ChunkHeader('ICMP', 4 + compressedSize);

			data << compressedSize;
			data.WriteData(compressedData, compressedSize);
		}
		else
		{
			data << ChunkHeader('IRAW', dataSize);
			data.WriteData(radiosityImage[0], dataSize);
		}

		delete[] compressedData;
	}

	data << TerminatorChunk;
}

void RadiositySpaceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	SpaceObject::Unpack(data, unpackFlags);
	UnpackChunkList<RadiositySpaceObject>(data, unpackFlags);

	#if C4LEGACY

		if (data.GetVersion() < 68)
		{
			UnpackChunkList<RadiositySpaceObject>(data, unpackFlags);
		}

	#endif
}

bool RadiositySpaceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RESO':

			data >> radiosityResolution;
			UpdateRadiosityResolution();
			return (true);

		case 'INSC':

			data >> intensityScale;
			return (true);

		case 'STSC':

			data >> saturationScale;
			return (true);

		case 'ICMP':
		{
			unsigned_int32	compressedSize;

			data >> compressedSize;
			unsigned_int8 *compressedData = new unsigned_int8[compressedSize];
			data.ReadData(compressedData, compressedSize);

			radiosityImage[0] = new Color4C[imageVoxelCount * 2];
			radiosityImage[1] = radiosityImage[0] + imageVoxelCount;
			Comp::DecompressData(compressedData, compressedSize, radiosityImage[0]);

			delete[] compressedData;

			CreateRadiosityTextures();
			return (true);
		}

		case 'IRAW':
		{
			radiosityImage[0] = new Color4C[imageVoxelCount * 2];
			radiosityImage[1] = radiosityImage[0] + imageVoxelCount;
			data.ReadData(radiosityImage[0], imageVoxelCount * (sizeof(Color4C) * 2));

			CreateRadiosityTextures();
			return (true);
		}
	}

	return (false);
}

void *RadiositySpaceObject::BeginSettingsUnpack(void)
{
	if (radiosityTexture[0])
	{
		radiosityTexture[1]->Release();
		radiosityTexture[1] = nullptr;

		radiosityTexture[0]->Release();
		radiosityTexture[0] = nullptr;
	}

	delete[] radiosityImage[0];
	radiosityImage[0] = nullptr;
	radiosityImage[1] = nullptr;

	return (SpaceObject::BeginSettingsUnpack());
}

int32 RadiositySpaceObject::GetCategoryCount(void) const
{
	return (1);
}

Type RadiositySpaceObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kSpaceRadiosity));
		return (kSpaceRadiosity);
	}

	return (0);
}

int32 RadiositySpaceObject::GetCategorySettingCount(Type category) const
{
	if (category == kSpaceRadiosity)
	{
		return (6);
	}

	return (0);
}

Setting *RadiositySpaceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kSpaceRadiosity)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kSpaceRadiosity, 'RDSY'));
			return (new HeadingSetting('LGHT', title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kSpaceRadiosity, 'RDSY', 'XRES'));
			return (new IntegerSetting('XRES', radiosityResolution.x, title, 2, kMaxRadiositySpaceResolution, 1));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kSpaceRadiosity, 'RDSY', 'YRES'));
			return (new IntegerSetting('YRES', radiosityResolution.y, title, 2, kMaxRadiositySpaceResolution, 1));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kSpaceRadiosity, 'RDSY', 'ZRES'));
			return (new IntegerSetting('ZRES', radiosityResolution.z, title, 2, kMaxRadiositySpaceResolution, 1));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kSpaceRadiosity, 'RDSY', 'INSC'));
			return (new TextSetting('INSC', intensityScale, title));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID(kSpaceRadiosity, 'RDSY', 'STSC'));
			return (new TextSetting('STSC', saturationScale, title));
		}
	}

	return (nullptr);
}

void RadiositySpaceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kSpaceRadiosity)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'XRES')
		{
			radiosityResolution.x = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'YRES')
		{
			radiosityResolution.y = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'ZRES')
		{
			radiosityResolution.z = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'INSC')
		{
			intensityScale = Fmax(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), 0.0F);
		}
		else if (identifier == 'STSC')
		{
			saturationScale = Clamp(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()), 0.0F, 1.0F);
		}
	}
}

void *RadiositySpaceObject::BeginSettings(void)
{
	Integer3D *resolution = new Integer3D(radiosityResolution);
	return (resolution);
}

void RadiositySpaceObject::EndSettings(void *cookie)
{
	Integer3D *resolution = static_cast<Integer3D *>(cookie);
	if (radiosityResolution != *resolution)
	{
		UpdateRadiosityResolution();

		if (radiosityTexture[0])
		{
			radiosityTexture[1]->Release();
			radiosityTexture[1] = nullptr;

			radiosityTexture[0]->Release();
			radiosityTexture[0] = nullptr;
		}

		if (radiosityImage[0])
		{
			delete[] radiosityImage[0];
			radiosityImage[0] = nullptr;
			radiosityImage[1] = nullptr;
		}

		CreateRadiosityImages();
		CreateRadiosityTextures();
	}

	delete resolution;
}

void RadiositySpaceObject::UpdateRadiosityResolution(void)
{
	int32 x = radiosityResolution.x;
	int32 y = radiosityResolution.y;
	int32 z = radiosityResolution.z;
	imageVoxelCount = x * y * z;

	float w = (float) x;
	float h = (float) y;
	float d = (float) z;
	texcoordScale.Set((w - 1.0F) / w, (h - 1.0F) / h, (d - 1.0F) / d);
	texcoordOffset.Set(0.5F / (w - 1.0F), 0.5F / (h - 1.0F), 0.5F / (d - 1.0F));
}

void RadiositySpaceObject::CreateRadiosityImages(void)
{
	radiosityImage[0] = new Color4C[imageVoxelCount * 2];
	radiosityImage[1] = radiosityImage[0] + imageVoxelCount;

	MemoryMgr::ClearMemory(radiosityImage[0], imageVoxelCount * (sizeof(Color4C) * 2));
}

void RadiositySpaceObject::CreateRadiosityTextures(void)
{
	textureHeader[0].textureType = kTexture3D;
	textureHeader[0].textureFlags = 0;
	textureHeader[0].colorSemantic = kTextureSemanticRadiosity;
	textureHeader[0].alphaSemantic = kTextureSemanticData;
	textureHeader[0].imageFormat = kTextureRGBA8;
	textureHeader[0].imageWidth = radiosityResolution.x;
	textureHeader[0].imageHeight = radiosityResolution.y;
	textureHeader[0].imageDepth = radiosityResolution.z;
	textureHeader[0].wrapMode[0] = kTextureClamp;
	textureHeader[0].wrapMode[1] = kTextureClamp;
	textureHeader[0].wrapMode[2] = kTextureClamp;
	textureHeader[0].mipmapCount = 1;
	textureHeader[0].mipmapDataOffset = 0;
	textureHeader[0].auxiliaryDataSize = 0;
	textureHeader[0].auxiliaryDataOffset = 0;

	textureHeader[1].textureType = kTexture3D;
	textureHeader[1].textureFlags = 0;
	textureHeader[1].colorSemantic = kTextureSemanticRadiosity;
	textureHeader[1].alphaSemantic = kTextureSemanticData;
	textureHeader[1].imageFormat = kTextureRGBA8;
	textureHeader[1].imageWidth = radiosityResolution.x;
	textureHeader[1].imageHeight = radiosityResolution.y;
	textureHeader[1].imageDepth = radiosityResolution.z;
	textureHeader[1].wrapMode[0] = kTextureClamp;
	textureHeader[1].wrapMode[1] = kTextureClamp;
	textureHeader[1].wrapMode[2] = kTextureClamp;
	textureHeader[1].mipmapCount = 1;
	textureHeader[1].mipmapDataOffset = 0;
	textureHeader[1].auxiliaryDataSize = 0;
	textureHeader[1].auxiliaryDataOffset = 0;

	radiosityTexture[0] = Texture::Get(&textureHeader[0], radiosityImage[0]);
	radiosityTexture[1] = Texture::Get(&textureHeader[1], radiosityImage[1]);
}

void RadiositySpaceObject::ClearTextureData(void) const
{
	MemoryMgr::ClearMemory(radiosityImage[0], imageVoxelCount * (sizeof(Color4C) * 2));

	radiosityTexture[0]->UpdateImage();
	radiosityTexture[1]->UpdateImage();
}

void RadiositySpaceObject::UpdateTextureData(void) const
{
	radiosityTexture[0]->UpdateImage();
	radiosityTexture[1]->UpdateImage();
}


AcousticsSpaceObject::AcousticsSpaceObject() : SpaceObject(kSpaceAcoustics, this)
{
}

AcousticsSpaceObject::AcousticsSpaceObject(const Vector3D& size) :
		SpaceObject(kSpaceAcoustics, this),
		BoxVolume(size)
{
	reflectionVolume = 1.0F;
	reflectionHFVolume = 0.5F;
	reverbDecayTime = 500.0F;
	mediumHFAbsorption = 1.0F;
}

AcousticsSpaceObject::~AcousticsSpaceObject()
{
}

void AcousticsSpaceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	SpaceObject::Pack(data, packFlags);

	data << ChunkHeader('REFL', 8);
	data << reflectionVolume;
	data << reflectionHFVolume;

	data << ChunkHeader('RVRB', 4);
	data << reverbDecayTime;

	data << ChunkHeader('ABSP', 4);
	data << mediumHFAbsorption;

	data << TerminatorChunk;
}

void AcousticsSpaceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	SpaceObject::Unpack(data, unpackFlags);
	UnpackChunkList<AcousticsSpaceObject>(data, unpackFlags);
}

bool AcousticsSpaceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'REFL':

			data >> reflectionVolume;
			data >> reflectionHFVolume;
			return (true);

		case 'RVRB':

			data >> reverbDecayTime;
			return (true);

		case 'ABSP':

			data >> mediumHFAbsorption;
			return (true);
	}

	return (false);
}

int32 AcousticsSpaceObject::GetCategoryCount(void) const
{
	return (1);
}

Type AcousticsSpaceObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kSpaceAcoustics));
		return (kSpaceAcoustics);
	}

	return (0);
}

int32 AcousticsSpaceObject::GetCategorySettingCount(Type category) const
{
	if (category == kSpaceAcoustics)
	{
		return (5);
	}

	return (0);
}

Setting *AcousticsSpaceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kSpaceAcoustics)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kSpaceAcoustics, 'ACST'));
			return (new HeadingSetting(kSpaceAcoustics, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kSpaceAcoustics, 'ACST', 'REFV'));
			return (new IntegerSetting('REFV', (int32) (reflectionVolume * 100.0 + 0.5F), title, 0, 200, 1));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kSpaceAcoustics, 'ACST', 'REFH'));
			return (new IntegerSetting('REFH', (int32) (reflectionHFVolume * 100.0 + 0.5F), title, 0, 100, 1));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kSpaceAcoustics, 'ACST', 'RVBT'));
			return (new FloatSetting('RVBT', reverbDecayTime * 0.001F, title, 0.0F, kMaxReverbDecayTime * 0.001F, 0.01F));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kSpaceAcoustics, 'ACST', 'ABSP'));
			return (new IntegerSetting('ABSP', (int32) ((1.0F - mediumHFAbsorption) * 1000.0F + 0.5F), title, 0, 100, 1));
		}
	}

	return (nullptr);
}

void AcousticsSpaceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kSpaceAcoustics)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'REFV')
		{
			reflectionVolume = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
		else if (identifier == 'REFH')
		{
			reflectionHFVolume = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
		else if (identifier == 'RVBT')
		{
			reverbDecayTime = static_cast<const FloatSetting *>(setting)->GetFloatValue() * 1000.0F;
		}
		else if (identifier == 'ABSP')
		{
			mediumHFAbsorption = 1.0F - (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.001F;
		}
	}
}


OcclusionSpaceObject::OcclusionSpaceObject() : SpaceObject(kSpaceOcclusion, this)
{
}

OcclusionSpaceObject::OcclusionSpaceObject(const Vector3D& size) :
		SpaceObject(kSpaceOcclusion, this),
		BoxVolume(size)
{
}

OcclusionSpaceObject::~OcclusionSpaceObject()
{
}


PaintSpaceObject::PaintSpaceObject() : SpaceObject(kSpacePaint, this)
{
	paintImageData.imageFlags = 0;

	paintImage = nullptr;
	paintTexture = nullptr;

	preprocessCount = 0;
}

PaintSpaceObject::PaintSpaceObject(const Vector3D& size, const Integer2D& resolution, int32 count) :
		SpaceObject(kSpacePaint, this),
		BoxVolume(size)
{
	paintImageData.imageFlags = 0;

	paintImageData.paintResolution = resolution;
	paintImageData.channelCount = count;

	paintImage = nullptr;
	paintTexture = nullptr;

	preprocessCount = 0;
}

PaintSpaceObject::~PaintSpaceObject()
{
	if (paintTexture)
	{
		paintTexture->Release();
	}

	delete[] paintImage;
}

void PaintSpaceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	SpaceObject::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << paintImageData.imageFlags;

	data << ChunkHeader('RESO', sizeof(Integer2D));
	data << paintImageData.paintResolution;

	data << ChunkHeader('CHAN', 4);
	data << paintImageData.channelCount;

	if (imageMipmapCount != 1)
	{
		data << ChunkHeader('MIPS', 8);
		data << imagePixelCount;
		data << imageMipmapCount;
	}

	if (paintImage)
	{
		unsigned_int32 dataSize = (imagePixelCount * paintImageData.channelCount + 3) & ~3;
		unsigned_int8 *compressedData = new unsigned_int8[dataSize];

		unsigned_int32 compressedSize = Comp::CompressData(paintImage, dataSize, compressedData);
		if (compressedSize != 0)
		{
			data << ChunkHeader('ICMP', 4 + compressedSize);

			data << compressedSize;
			data.WriteData(compressedData, compressedSize);
		}
		else
		{
			data << ChunkHeader('IRAW', dataSize);
			data.WriteData(paintImage, dataSize);
		}

		delete[] compressedData;
	}

	data << TerminatorChunk;
}

void PaintSpaceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	SpaceObject::Unpack(data, unpackFlags);
	UnpackChunkList<PaintSpaceObject>(data, unpackFlags);
}

bool PaintSpaceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> paintImageData.imageFlags;
			return (true);

		case 'RESO':

			data >> paintImageData.paintResolution;
			imagePixelCount = paintImageData.paintResolution.x * paintImageData.paintResolution.y;
			imageMipmapCount = 1;
			return (true);

		case 'CHAN':

			data >> paintImageData.channelCount;
			return (true);

		case 'MIPS':

			data >> imagePixelCount;
			data >> imageMipmapCount;
			return (true);

		case 'ICMP':
		{
			unsigned_int32	compressedSize;

			data >> compressedSize;
			unsigned_int8 *compressedData = new unsigned_int8[compressedSize];
			data.ReadData(compressedData, compressedSize);

			unsigned_int32 dataSize = (imagePixelCount * paintImageData.channelCount + 3) & ~3;
			paintImage = new unsigned_int8[dataSize];
			Comp::DecompressData(compressedData, compressedSize, paintImage);

			delete[] compressedData;
			return (true);
		}

		case 'IRAW':
		{
			unsigned_int32 dataSize = (imagePixelCount * paintImageData.channelCount + 3) & ~3;
			paintImage = new unsigned_int8[dataSize];
			data.ReadData(paintImage, dataSize);
			return (true);
		}
	}

	return (false);
}

void *PaintSpaceObject::BeginSettingsUnpack(void)
{
	if (paintTexture)
	{
		paintTexture->Release();
		paintTexture = nullptr;
	}

	delete[] paintImage;
	paintImage = nullptr;

	return (SpaceObject::BeginSettingsUnpack());
}

int32 PaintSpaceObject::GetCategoryCount(void) const
{
	return (1);
}

Type PaintSpaceObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kSpacePaint));
		return (kSpacePaint);
	}

	return (0);
}

int32 PaintSpaceObject::GetCategorySettingCount(Type category) const
{
	if (category == kSpacePaint)
	{
		return (5);
	}

	return (0);
}

Setting *PaintSpaceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kSpacePaint)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kSpacePaint, 'PANT'));
			return (new HeadingSetting(kSpacePaint, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kSpacePaint, 'PANT', 'MIPS'));
			return (new BooleanSetting('MIPS', ((paintImageData.imageFlags & kPaintImageMipmaps) != 0), title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kSpacePaint, 'PANT', 'XRES'));
			return (new PowerTwoSetting('XRES', paintImageData.paintResolution.x, title, kPaintMinResolution, kPaintMaxResolution));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kSpacePaint, 'PANT', 'YRES'));
			return (new PowerTwoSetting('YRES', paintImageData.paintResolution.y, title, kPaintMinResolution, kPaintMaxResolution));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kSpacePaint, 'PANT', 'CHAN'));
			return (new PowerTwoSetting('CHAN', paintImageData.channelCount, title, 1, 4));
		}
	}

	return (nullptr);
}

void PaintSpaceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kSpacePaint)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'MIPS')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				paintImageData.imageFlags |= kPaintImageMipmaps;
			}
			else
			{
				paintImageData.imageFlags &= ~kPaintImageMipmaps;
			}
		}
		else if (identifier == 'XRES')
		{
			paintImageData.paintResolution.x = static_cast<const PowerTwoSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'YRES')
		{
			paintImageData.paintResolution.y = static_cast<const PowerTwoSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'CHAN')
		{
			paintImageData.channelCount = static_cast<const PowerTwoSetting *>(setting)->GetIntegerValue();
		}
	}
}

void *PaintSpaceObject::BeginSettings(void)
{
	PaintImageData *imageData = new PaintImageData(paintImageData);
	return (imageData);
}

void PaintSpaceObject::EndSettings(void *cookie)
{
	PaintImageData *imageData = static_cast<PaintImageData *>(cookie);

	unsigned_int8 *image = paintImage;
	if (image)
	{
		if ((paintImageData.paintResolution != imageData->paintResolution) || (paintImageData.channelCount != imageData->channelCount))
		{
			delete[] image;
			CreatePaintImage();

			if (paintTexture)
			{
				paintTexture->Release();
				CreatePaintTexture();
			}
		}
		else if (paintImageData.imageFlags != imageData->imageFlags)
		{
			CreatePaintImage();
			MemoryMgr::CopyMemory(image, paintImage, paintImageData.paintResolution.x * paintImageData.paintResolution.y * paintImageData.channelCount);
			delete[] image;

			GenerateMipmaps();

			if (paintTexture)
			{
				paintTexture->Release();
				CreatePaintTexture();
			}
		}
	}

	delete imageData;
}

void PaintSpaceObject::GenerateMipmaps(void)
{
	if (paintImageData.imageFlags & kPaintImageMipmaps)
	{
		int32 channelCount = paintImageData.channelCount;
		if (channelCount == 4)
		{
			Image::GenerateMipmaps2D(1, paintImageData.paintResolution.x, paintImageData.paintResolution.y, reinterpret_cast<Color4C *>(paintImage));
		}
		else if (channelCount == 2)
		{
			Image::GenerateMipmaps2D(1, paintImageData.paintResolution.x, paintImageData.paintResolution.y, reinterpret_cast<Color2C *>(paintImage));
		}
		else
		{
			Image::GenerateMipmaps2D(1, paintImageData.paintResolution.x, paintImageData.paintResolution.y, reinterpret_cast<Color1C *>(paintImage));
		}
	}
}

void PaintSpaceObject::CreatePaintImage(void)
{
	if (paintImageData.imageFlags & kPaintImageMipmaps)
	{
		imageMipmapCount = Image::CalculateMipmapChainSize2D(paintImageData.paintResolution.x, paintImageData.paintResolution.y, &imagePixelCount);
	}
	else
	{
		imageMipmapCount = 1;
		imagePixelCount = paintImageData.paintResolution.x * paintImageData.paintResolution.y;
	}

	unsigned_int32 dataSize = (imagePixelCount * paintImageData.channelCount + 3) & ~3;
	paintImage = new unsigned_int8[dataSize];
	MemoryMgr::ClearMemory(paintImage, dataSize);
}

void PaintSpaceObject::CreatePaintTexture(void)
{
	textureHeader.textureType = kTexture2D;
	textureHeader.textureFlags = 0;
	textureHeader.colorSemantic = kTextureSemanticData;
	textureHeader.alphaSemantic = kTextureSemanticData;

	int32 channelCount = paintImageData.channelCount;
	if (channelCount == 4)
	{
		textureHeader.imageFormat = kTextureRGBA8;
	}
	else if (channelCount == 2)
	{
		textureHeader.imageFormat = kTextureLA8;
	}
	else
	{
		textureHeader.imageFormat = kTextureI8;
	}

	textureHeader.imageWidth = paintImageData.paintResolution.x;
	textureHeader.imageHeight = paintImageData.paintResolution.y;
	textureHeader.imageDepth = 1;
	textureHeader.wrapMode[0] = kTextureClamp;
	textureHeader.wrapMode[1] = kTextureClamp;
	textureHeader.wrapMode[2] = kTextureClamp;
	textureHeader.mipmapCount = imageMipmapCount;
	textureHeader.mipmapDataOffset = 0;
	textureHeader.auxiliaryDataSize = 0;
	textureHeader.auxiliaryDataOffset = 0;

	paintTexture = Texture::Get(&textureHeader, paintImage);
}

void PaintSpaceObject::Preprocess(void)
{
	preprocessCount++;

	if (!paintTexture)
	{
		if (!paintImage)
		{
			CreatePaintImage();
		}

		CreatePaintTexture();
	}
}

void PaintSpaceObject::Neutralize(void)
{
	if (--preprocessCount == 0)
	{
		if (paintTexture)
		{
			paintTexture->Release();
			paintTexture = nullptr;
		}
	}
}

void PaintSpaceObject::UpdatePaintTexture(const Rect& bounds)
{
	GenerateMipmaps();
	paintTexture->UpdateRect(bounds);
}


PhysicsSpaceObject::PhysicsSpaceObject() : SpaceObject(kSpacePhysics, this)
{
}

PhysicsSpaceObject::PhysicsSpaceObject(const Vector3D& size) :
		SpaceObject(kSpacePhysics, this),
		BoxVolume(size)
{
}

PhysicsSpaceObject::~PhysicsSpaceObject()
{
}

// ZYUQURM
