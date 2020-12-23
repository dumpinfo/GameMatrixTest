 

#ifndef C4Lights_h
#define C4Lights_h


//# \component	World Manager
//# \prefix		WorldMgr/

//# \import		C4LightObjects.h


#include "C4LightObjects.h"
#include "C4Regions.h"
#include "C4Spaces.h"


namespace C4
{
	C4API extern const char kConnectorKeyShadow[];


	class FrustumCamera;


	//# \class	Light	Represents a light node in a world.
	//
	//# The $Light$ class represents a light node in a world.
	//
	//# \def	class Light : public Node
	//
	//# \ctor	Light(LightType type, LightType base);
	//
	//# The constructor has protected access. A $Light$ class can only exist as the base class for a more specific type of light.
	//
	//# \param	type	The type of the light source. See below for a list of possible types.
	//# \param	base	The base type of the light source. This should be $kLightInfinite$ or $kLightPoint$.
	//
	//# \desc
	//# The $Light$ class is the base class for all light nodes. The $type$ parameter can be one of the following
	//# constants, and it is specified by the subclass's constructor.
	//
	//# \table	LightType
	//
	//# \base	Node	A $Light$ node is a scene graph node.
	//
	//# \also	$@GraphicsMgr/LightObject@$
	//
	//# \wiki	Lights


	//# \function	Light::GetLightType		Returns the specific type of a light.
	//
	//# \proto	LightType GetLightType(void) const;
	//
	//# \desc
	//# The $GetLightType$ function returns the specific light type, which can be one of the following constants.
	//
	//# \table	LightType
	//
	//# All of the light types are divided into two categories, and the general category that a light object
	//# falls into can be determined by calling the $@Light::GetBaseLightType@$ function.
	//
	//# \also	$@Light::GetBaseLightType@$


	//# \function	Light::GetBaseLightType		Returns the base type of a light.
	//
	//# \proto	LightType GetBaseLightType(void) const;
	//
	//# \desc
	//# The $GetBaseLightType$ function returns the base light type. See the $@GraphicsMgr/LightObject::GetBaseLightType@$
	//# function for details about the base type.
	//
	//# \also	$@Light::GetLightType@$


	class Light : public Node
	{
		friend class Node;

		private:

			LightType					lightType;
			LightType					baseLightType;

			ShadowSpace					*connectedShadowSpace;
			Link<Node>					excludedNode;

			Array<LightRegion *, 8>		activeLightRegionArray;
			Array<CameraRegion *, 8>	activeCameraRegionArray;

			static Light *Create(Unpacker& data, unsigned_int32 unpackFlags);

			static void ExcludedLinkProc(Node *node, void *cookie);

		protected:

			List<RootLightRegion>		rootRegionList;

			Light(LightType type, LightType base);
			Light(const Light& light);
 
		public:

			virtual ~Light(); 

			LightType GetLightType(void) const 
			{
				return (lightType);
			} 

			LightType GetBaseLightType(void) const 
			{ 
				return (baseLightType);
			}

			LightObject *GetObject(void) const 
			{
				return (static_cast<LightObject *>(Node::GetObject()));
			}

			ShadowSpace *GetConnectedShadowSpace(void) const
			{
				return (connectedShadowSpace);
			}

			Node *GetExcludedNode(void) const
			{
				return (excludedNode);
			}

			void SetExcludedNode(Node *node)
			{
				excludedNode = node;
			}

			const ImmutableArray<LightRegion *>& GetActiveLightRegionArray(void) const
			{
				return (activeLightRegionArray);
			}

			void AddActiveLightRegion(LightRegion *region)
			{
				activeLightRegionArray.AddElement(region);
			}

			const ImmutableArray<CameraRegion *>& GetActiveCameraRegionArray(void) const
			{
				return (activeCameraRegionArray);
			}

			void AddActiveCameraRegion(CameraRegion *region)
			{
				activeCameraRegionArray.AddElement(region);
			}

			void ClearActiveRegions(void)
			{
				activeLightRegionArray.Clear();
				activeCameraRegionArray.Clear();
			}

			RootLightRegion *GetFirstRootRegion(void)
			{
				return (rootRegionList.First());
			}

			const RootLightRegion *GetFirstRootRegion(void) const
			{
				return (rootRegionList.First());
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			void ProcessInternalConnectors(void) override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API void SetConnectedShadowSpace(ShadowSpace *shadowSpace);

			void Neutralize(void) override;

			C4API void InvalidateLightRegions(void);
	};


	//# \class	InfiniteLight	Represents an infinite light node in a world.
	//
	//# The $InfiniteLight$ class represents an infinite light node in a world.
	//
	//# \def	class InfiniteLight : public Light
	//
	//# \ctor	InfiniteLight(const ColorRGB& color);
	//
	//# \param	color	The color of light emitted by the light source.
	//
	//# \desc
	//#
	//
	//# \base	Light	An infinite light is a type of light.
	//
	//# \also	$@GraphicsMgr/InfiniteLightObject@$
	//
	//# \wiki	Infinite_Light	Infinite Light


	class InfiniteLight final : public Light
	{
		friend class Light;

		private:

			Node				*connectedMemberNode;

			LightShadowData		shadowData[kMaxShadowCascadeCount];

			InfiniteLight();

			Node *Replicate(void) const override;

			void CalculateIllumination(LightRegion *region);
			void EstablishRootRegions(Zone *zone, int32 forcedDepth);
			void ProcessConnectedZoneMemberships(void);

		protected:

			InfiniteLight(LightType type);
			InfiniteLight(const InfiniteLight& infiniteLight);

			void HandlePostprocessUpdate(void) override;

		public:

			C4API InfiniteLight(const ColorRGB& color);
			C4API ~InfiniteLight();

			InfiniteLightObject *GetObject(void) const
			{
				return (static_cast<InfiniteLightObject *>(Node::GetObject()));
			}

			Node *GetConnectedMemberNode(void) const
			{
				return (connectedMemberNode);
			}

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			void ProcessInternalConnectors(void) override;
			C4API void SetConnectedMemberNode(Node *node);

			const LightShadowData *CalculateShadowData(const FrustumCamera *camera);
	};


	//# \class	PointLight		Represents a point light node in a world.
	//
	//# The $PointLight$ class represents a point light node in a world.
	//
	//# \def	class PointLight : public Light
	//
	//# \ctor	PointLight(const ColorRGB& color, float range);
	//
	//# \param	color	The color of light emitted by the light source.
	//# \param	range	The spherical range of the light source.
	//
	//# \desc
	//#
	//
	//# \base	Light	A point light is a type of light.
	//
	//# \also	$@GraphicsMgr/PointLightObject@$


	class PointLight : public Light
	{
		friend class Light;

		private:

			float		fadeScale;
			float		fadeOffset;
			float		colorMultiplier;

			PointLight();

			Node *Replicate(void) const override;

			void HandlePostprocessUpdate(void) override;

		protected:

			PointLight(LightType type);
			PointLight(const PointLight& pointLight);

			void CalculateIllumination(LightRegion *region);
			bool EstablishRootRegions(Zone *zone, int32 maxDepth, int32 forcedDepth);

		public:

			C4API PointLight(const ColorRGB& color, float range);
			C4API ~PointLight();

			PointLightObject *GetObject(void) const
			{
				return (static_cast<PointLightObject *>(Node::GetObject()));
			}

			float CalculateColorMultiplier(float distance)
			{
				float multiplier = Clamp(distance * fadeScale + fadeOffset, 0.0F, 1.0F);
				colorMultiplier = multiplier;
				return (multiplier);
			}

			float GetColorMultiplier(void) const
			{
				return (colorMultiplier);
			}

			void Preprocess(void) override;
	};


	//# \class	CubeLight	Represents a cube light node in a world.
	//
	//# The $CubeLight$ class represents a cube light node in a world.
	//
	//# \def	class CubeLight final : public PointLight
	//
	//# \ctor	CubeLight(const ColorRGB& color, float range, const char *name);
	//
	//# \param	color	The color of light emitted by the light source.
	//# \param	range	The spherical range of the light source.
	//# \param	name	The name of the projected shadow texture map.
	//
	//# \desc
	//#
	//
	//# \base	PointLight	A cube light is a special type of point light.
	//
	//# \also	$@GraphicsMgr/CubeLightObject@$


	class CubeLight final : public PointLight
	{
		friend class Light;

		private:

			CubeLight();
			CubeLight(const CubeLight& cubeLight);

			Node *Replicate(void) const override;

		public:

			C4API CubeLight(const ColorRGB& color, float range, const char *name);
			C4API ~CubeLight();

			CubeLightObject *GetObject(void) const
			{
				return (static_cast<CubeLightObject *>(Node::GetObject()));
			}
	};


	//# \class	SpotLight	Represents a spot light node in a world.
	//
	//# The $SpotLight$ class represents a spot light node in a world.
	//
	//# \def	class SpotLight final : public PointLight
	//
	//# \ctor	SpotLight(const ColorRGB& color, float range, float apex, const char *name);
	//
	//# \param	color	The color of light emitted by the light source.
	//# \param	range	The spherical range of the light source.
	//# \param	apex	The tangent of half the apex angle for the spot light. This determines the light's angle of illumination.
	//# \param	name	The name of the projected shadow texture map.
	//
	//# \desc
	//#
	//
	//# \base	PointLight	A spot light is a special type of point light.
	//
	//# \also	$@GraphicsMgr/SpotLightObject@$


	class SpotLight final : public PointLight
	{
		friend class Light;

		private:

			SpotLight();
			SpotLight(const SpotLight& spotLight);

			Node *Replicate(void) const override;

			void HandlePostprocessUpdate(void) override;

		public:

			C4API SpotLight(const ColorRGB& color, float range, float apex, const char *name);
			C4API ~SpotLight();

			SpotLightObject *GetObject(void) const
			{
				return (static_cast<SpotLightObject *>(Node::GetObject()));
			}
	};
}


#endif

// ZYUQURM
