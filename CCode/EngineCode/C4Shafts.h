 

#ifndef C4Shafts_h
#define C4Shafts_h


//# \component	Effect Manager
//# \prefix		EffectMgr/


#include "C4Effects.h"
#include "C4Volumes.h"
#include "C4Shaders.h"


namespace C4
{
	typedef Type	ShaftType;


	enum : ProcessType
	{
		kProcessBoxShaft				= 'BSHF',
		kProcessCylinderShaft			= 'CSHF',
		kProcessTruncatedPyramidShaft	= 'TPSH',
		kProcessTruncatedConeShaft		= 'TCSH'
	};


	enum
	{
		kShaftProcessGradient			= 1 << 0,
		kShaftProcessNoise				= 1 << 1
	};


	//# \enum	ShaftType

	enum : ShaftType
	{
		kShaftBox						= 'BOX ',		//## Box shaft.
		kShaftCylinder					= 'CYLD',		//## Cylinder shaft.
		kShaftTruncatedPyramid			= 'TPYR',		//## Truncated pyramid shaft.
		kShaftTruncatedCone				= 'TCON'		//## Truncated cone shaft.
	};


	//# \enum	ShaftFlags

	enum
	{
		kShaftAnimateIntensity			= 1 << 0,		//## The intensity scale of the shaft effect is animated.
		kShaftRandomNoise				= 1 << 1,		//## Render the shaft effect with a little random noise to hide banding.
		kShaftTopPlaneCull				= 1 << 2		//## Cull the shaft effect if the camera is above the top plane by more than a specified amount.
	};


	class ShaftProcess : public Process
	{
		private:

			unsigned_int32		shaftProcessFlags;
			const Vector4D		(*shaftParams)[4];

			Texture				*textureObject;

			static void StateProc_LoadShaftParams(const Renderable *renderable, const void *cookie);

		protected:

			ShaftProcess(ProcessType type, unsigned_int32 flags, const Vector4D (*params)[4]);
			ShaftProcess(const ShaftProcess& shaftProcess);

		public:

			~ShaftProcess();

			unsigned_int32 GetShaftProcessFlags(void) const
			{
				return (shaftProcessFlags);
			}

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class BoxShaftProcess final : public ShaftProcess
	{
		private:

			BoxShaftProcess(const BoxShaftProcess& boxShaftProcess);

			Process *Replicate(void) const override;

		public:

			BoxShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]);
			~BoxShaftProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	}; 


	class CylinderShaftProcess final : public ShaftProcess 
	{
		private: 

			CylinderShaftProcess(const CylinderShaftProcess& cylinderShaftProcess);
 
			Process *Replicate(void) const override;
 
		public: 

			CylinderShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]);
			~CylinderShaftProcess();
 
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TruncatedPyramidShaftProcess final : public ShaftProcess
	{
		private:

			TruncatedPyramidShaftProcess(const TruncatedPyramidShaftProcess& truncatedPyramidShaftProcess);

			Process *Replicate(void) const override;

		public:

			TruncatedPyramidShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]);
			~TruncatedPyramidShaftProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TruncatedConeShaftProcess final : public ShaftProcess
	{
		private:

			TruncatedConeShaftProcess(const TruncatedConeShaftProcess& truncatedConeShaftProcess);

			Process *Replicate(void) const override;

		public:

			TruncatedConeShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]);
			~TruncatedConeShaftProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	//# \class	ShaftEffectObject		Encapsulates data pertaining to a light shaft effect.
	//
	//# The $ShaftEffectObject$ class encapsulates data pertaining to a light shaft effect.
	//
	//# \def	class ShaftEffectObject : public EffectObject, public VolumeObject
	//
	//# \ctor	ShaftEffectObject(ShaftType type, const Vector2D& size, float height, const ColorRGBA& color);
	//
	//# The constructor has protected access. The $ShaftEffectObject$ class can only exist as the base class for another class.
	//
	//# \param	type		The type of the light shaft effect.
	//# \param	size		The size of the light shaft's cross section.
	//# \param	height		The height of the light shaft.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $ShaftEffectObject$ class is the base class for all types of shaft effect objects, and it stores
	//# data that is common to all of them. Each specific type of shaft effect has an associated object
	//# that is a subclass of the $ShaftEffectObject$ class.
	//
	//# \base		EffectObject		A $ShaftEffectObject$ is an object that can be shared by multiple shaft effect nodes.
	//# \privbase	VolumeObject		Used internally by the engine for generic volume objects.
	//
	//# \also	$@BoxShaftEffectObject@$
	//# \also	$@CylinderShaftEffectObject@$
	//# \also	$@TruncatedPyramidShaftEffectObject@$
	//# \also	$@TruncatedConeShaftEffectObject@$
	//# \also	$@ShaftEffect@$
	//
	//# \wiki	Light_Shafts	Light Shafts


	class ShaftEffectObject : public EffectObject, public VolumeObject
	{
		friend class EffectObject;

		private:

			ShaftType			shaftType;

			unsigned_int32		shaftFlags;
			ColorRGBA			shaftColor;
			float				shaftDensity[2];

			Range<float>		intensityScale;
			Range<float>		intensityHoldTime[2];
			Range<float>		intensityTransitionTime;

			float				topPlaneCullMargin;

			static ShaftEffectObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			ShaftEffectObject(ShaftType type, Volume *volume);
			ShaftEffectObject(ShaftType type, Volume *volume, const ColorRGBA& color);
			~ShaftEffectObject();

		public:

			ShaftType GetShaftType(void) const
			{
				return (shaftType);
			}

			unsigned_int32 GetShaftFlags(void) const
			{
				return (shaftFlags);
			}

			void SetShaftFlags(unsigned_int32 flags)
			{
				shaftFlags = flags;
			}

			const ColorRGBA& GetShaftColor(void) const
			{
				return (shaftColor);
			}

			void SetShaftColor(const ColorRGBA& color)
			{
				shaftColor = color;
			}

			float GetShaftDensity(int32 index) const
			{
				return (shaftDensity[index]);
			}

			void SetShaftDensity(int32 index, float density)
			{
				shaftDensity[index] = density;
			}

			const Range<float>& GetIntensityScale(void) const
			{
				return (intensityScale);
			}

			void SetIntensityScale(const Range<float>& scale)
			{
				intensityScale = scale;
			}

			const Range<float>& GetIntensityHoldTime(int32 index) const
			{
				return (intensityHoldTime[index]);
			}

			void SetIntensityHoldTime(int32 index, const Range<float>& time)
			{
				intensityHoldTime[index] = time;
			}

			const Range<float>& GetIntensityTransitionTime(void) const
			{
				return (intensityTransitionTime);
			}

			void SetIntensityTransitionTime(const Range<float>& time)
			{
				intensityTransitionTime = time;
			}

			float GetTopPlaneCullMargin(void) const
			{
				return (topPlaneCullMargin);
			}

			void SetTopPlaneCullMargin(float margin)
			{
				topPlaneCullMargin = margin;
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	//# \class	BoxShaftEffectObject	Encapsulates data pertaining to a box light shaft effect.
	//
	//# The $BoxShaftEffectObject$ class encapsulates data pertaining to a box light shaft effect.
	//
	//# \def	class BoxShaftEffectObject final : public ShaftEffectObject, public BoxVolume
	//
	//# \ctor	BoxShaftEffectObject(const Vector3D& size, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section in the <i>x</i> and <i>y</i> components, and the height of the light shaft in the <i>z</i> component.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $BoxShaftEffectObject$ class encapsulates data pertaining to a box light shaft effect.
	//# In its local coordinate space, the rectangular cross section of a box light shaft has one
	//# of its corners at the origin, and the components of the $size$ parameter define the extents
	//# of the rectangle in the positive <i>x</i> and <i>y</i> directions. The $height$ parameter
	//# defines the extent of the light shaft in the <i>z</i> direction, but this is only used for
	//# constructing the polygonal geometry of the light shaft. The interior of the light shaft
	//# itself is rendered as if it had infinite extent in the <i>z</i> direction.
	//
	//# \base	ShaftEffectObject		A $BoxShaftEffectObject$ is an object that can be shared by multiple box light shaft effect nodes.
	//# \base	WorldMgr/BoxVolume		A $BoxShaftEffectObject$ is represented by a generic box volume.
	//
	//# \also	$@BoxShaftEffect@$


	class BoxShaftEffectObject final : public ShaftEffectObject, public BoxVolume
	{
		friend class ShaftEffectObject;

		private:

			BoxShaftEffectObject();
			~BoxShaftEffectObject();

		public:

			BoxShaftEffectObject(const Vector3D& size, const ColorRGBA& color);
	};


	//# \class	CylinderShaftEffectObject	Encapsulates data pertaining to a cylinder light shaft effect.
	//
	//# The $CylinderShaftEffectObject$ class encapsulates data pertaining to a cylinder light shaft effect.
	//
	//# \def	class CylinderShaftEffectObject final : public ShaftEffectObject, public CylinderVolume
	//
	//# \ctor	CylinderShaftEffectObject(const Vector2D& size, float height, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section.
	//# \param	height		The height of the light shaft.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $CylinderShaftEffectObject$ class encapsulates data pertaining to a cylinder light shaft effect.
	//# In its local coordinate space, the elliptical cross section of a cylinder light shaft has its center
	//# at the origin, and the components of the $size$ parameter define the semiaxis lengths in the positive
	//# <i>x</i> and <i>y</i> directions. The $height$ parameter defines the extent of the light shaft in the
	//# <i>z</i> direction, but this is only used for constructing the polygonal geometry of the light shaft.
	//# The interior of the light shaft itself is rendered as if it had infinite extent in the <i>z</i> direction.
	//
	//# \base	ShaftEffectObject			A $CylinderShaftEffectObject$ is an object that can be shared by multiple cylinder light shaft effect nodes.
	//# \base	WorldMgr/CylinderVolume		A $CylinderShaftEffectObject$ is represented by a generic cylinder volume.
	//
	//# \also	$@CylinderShaftEffect@$


	class CylinderShaftEffectObject final : public ShaftEffectObject, public CylinderVolume
	{
		friend class ShaftEffectObject;

		private:

			CylinderShaftEffectObject();
			~CylinderShaftEffectObject();

		public:

			CylinderShaftEffectObject(const Vector2D& size, float height, const ColorRGBA& color);
	};


	//# \class	TruncatedPyramidShaftEffectObject	Encapsulates data pertaining to a truncated pyramid light shaft effect.
	//
	//# The $TruncatedPyramidShaftEffectObject$ class encapsulates data pertaining to a truncated pyramid light shaft effect.
	//
	//# \def	class TruncatedPyramidShaftEffectObject final : public ShaftEffectObject, public TruncatedPyramidVolume
	//
	//# \ctor	TruncatedPyramidShaftEffectObject(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section at the base of the pyramid.
	//# \param	height		The height of the light shaft.
	//# \param	ratio		The ratio of the upper size to the base size.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $TruncatedPyramidShaftEffectObject$ class encapsulates data pertaining to a truncated pyramid light shaft effect.
	//# In its local coordinate space, the rectangular cross section of the base of a truncated pyramid light shaft has one
	//# of its corners at the origin, and the components of the $size$ parameter define the extents of the rectangle in the
	//# positive <i>x</i> and <i>y</i> directions. The $height$ parameter defines the extent of the light shaft in the <i>z</i>
	//# direction, but this is only used for constructing the polygonal geometry of the light shaft. The $ratio$ parameter
	//# defines the ratio between the size of the top face and the bottom face (the base) of the truncated pyramid and should
	//# be in the range (0,1). The interior of the light shaft itself is rendered as if it extends to the apex of the
	//# untruncated pyramid in the <i>z</i> direction and as if it had infinite extent in the <i>&minus;z</i> direction.
	//
	//# \base	ShaftEffectObject					A $TruncatedPyramidShaftEffectObject$ is an object that can be shared by multiple truncated pyramid light shaft effect nodes.
	//# \base	WorldMgr/TruncatedPyramidVolume		A $TruncatedPyramidShaftEffectObject$ is represented by a generic truncated pyramid volume.
	//
	//# \also	$@TruncatedPyramidShaftEffect@$


	class TruncatedPyramidShaftEffectObject final : public ShaftEffectObject, public TruncatedPyramidVolume
	{
		friend class ShaftEffectObject;

		private:

			TruncatedPyramidShaftEffectObject();
			~TruncatedPyramidShaftEffectObject();

		public:

			TruncatedPyramidShaftEffectObject(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
	};


	//# \class	TruncatedConeShaftEffectObject		Encapsulates data pertaining to a truncated cone light shaft effect.
	//
	//# The $TruncatedConeShaftEffectObject$ class encapsulates data pertaining to a truncated cone light shaft effect.
	//
	//# \def	class TruncatedConeShaftEffectObject final : public ShaftEffectObject, public TruncatedConeVolume
	//
	//# \ctor	TruncatedConeShaftEffectObject(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section at the base of the cone.
	//# \param	height		The height of the light shaft.
	//# \param	ratio		The ratio of the upper size to the base size.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $TruncatedConeShaftEffectObject$ class encapsulates data pertaining to a truncated cone light shaft effect.
	//# In its local coordinate space, the elliptical cross section of the base of a truncated cone light shaft has its
	//# center at the origin, and the components of the $size$ parameter define the semiaxis lengths in the positive
	//# <i>x</i> and <i>y</i> directions. The $height$ parameter defines the extent of the light shaft in the <i>z</i>
	//# direction, but this is only used for constructing the polygonal geometry of the light shaft. The $ratio$ parameter
	//# defines the ratio between the size of the top face and the bottom face (the base) of the truncated cone and should
	//# be in the range (0,1). The interior of the light shaft itself is rendered as if it extends to the apex of the
	//# untruncated cone in the <i>z</i> direction and as if it had infinite extent in the <i>&minus;z</i> direction.
	//
	//# \base	ShaftEffectObject				A $TruncatedConeShaftEffectObject$ is an object that can be shared by multiple truncated cone light shaft effect nodes.
	//# \base	WorldMgr/TruncatedConeVolume	A $TruncatedConeShaftEffectObject$ is represented by a generic truncated cone volume.
	//
	//# \also	$@TruncatedConeShaftEffect@$


	class TruncatedConeShaftEffectObject final : public ShaftEffectObject, public TruncatedConeVolume
	{
		friend class ShaftEffectObject;

		private:

			TruncatedConeShaftEffectObject();
			~TruncatedConeShaftEffectObject();

		public:

			TruncatedConeShaftEffectObject(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
	};


	//# \class	ShaftEffect		Represents a light shaft effect node in a world.
	//
	//# The $ShaftEffect$ class represents a light shaft effect node in a world.
	//
	//# \def	class ShaftEffect : public Effect
	//
	//# \ctor	ShaftEffect(ShaftType type);
	//
	//# The constructor has protected access. The $ShaftEffect$ class can only exist as the base class for another class.
	//
	//# \param	type	The type of the light shaft effect.
	//
	//# \desc
	//# The $ShaftEffect$ class is the base class for all light shaft effect nodes. Each specific type of
	//# light shaft effect is a subclass of the $ShaftEffect$ class.
	//
	//# \base	Effect		A shaft effect is a specific type of effect.
	//
	//# \also	$@BoxShaftEffect@$
	//# \also	$@CylinderShaftEffect@$
	//# \also	$@TruncatedPyramidShaftEffect@$
	//# \also	$@TruncatedConeShaftEffect@$
	//# \also	$@ShaftEffectObject@$
	//
	//# \wiki	Light_Shafts	Light Shafts


	class ShaftEffect : public Effect
	{
		friend class Effect;

		private:

			enum
			{
				kShaftStatic,
				kShaftHold1,
				kShaftHold2,
				kShaftTransition1,
				kShaftTransition2
			};

			ShaftType			shaftType;
			unsigned_int32		shaftState;

			float				maxCameraHeight;

			float				densityAlpha;
			float				densityBeta;

			float				intensityScale;
			float				intensityHoldTime;
			float				transitionAngle;
			float				transitionSpeed;

			List<Attribute>		attributeList;

			static ShaftEffect *Create(Unpacker& data, unsigned_int32 unpackFlags);

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		protected:

			ShaderAttribute		shaderAttribute;
			VertexBuffer		vertexBuffer;

			Vector4D			shaftParams[4];

			ShaftEffect(ShaftType type);
			ShaftEffect(const ShaftEffect& shaftEffect);

		public:

			~ShaftEffect();

			ShaftType GetShaftType(void) const
			{
				return (shaftType);
			}

			ShaftEffectObject *GetObject(void) const
			{
				return (static_cast<ShaftEffectObject *>(Node::GetObject()));
			}

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Neutralize(void) override;

			void ProcessObjectSettings(void) override;
			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	BoxShaftEffect		Represents a box light shaft effect node in a world.
	//
	//# The $BoxShaftEffect$ class represents a box light shaft effect node in a world.
	//
	//# \def	class BoxShaftEffect final : public ShaftEffect
	//
	//# \ctor	BoxShaftEffect(const Vector3D& size, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section in the <i>x</i> and <i>y</i> components, and the height of the light shaft in the <i>z</i> component.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $BoxShaftEffect$ class represents a light shaft effect node having a rectangular
	//# cross section whose dimensions are specified by the $size$ parameter.
	//
	//# \base	ShaftEffect		A box shaft effect is a specific type of shaft effect.
	//
	//# \also	$@BoxShaftEffectObject@$


	class BoxShaftEffect final : public ShaftEffect
	{
		friend class ShaftEffect;

		private:

			Point3D						worldCenter;
			Vector3D					worldAxis[3];

			static SharedVertexBuffer	indexBuffer;

			BoxShaftEffect();
			BoxShaftEffect(const BoxShaftEffect& boxShaftEffect);

			Node *Replicate(void) const override;

			void Initialize(void);

			void HandleTransformUpdate(void) override;

			static bool ShaftVisible(const Node *node, const VisibilityRegion *region);
			static bool ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

		public:

			C4API BoxShaftEffect(const Vector3D& size, const ColorRGBA& color);
			C4API ~BoxShaftEffect();

			BoxShaftEffectObject *GetObject(void) const
			{
				return (static_cast<BoxShaftEffectObject *>(Node::GetObject()));
			}

			C4API void Preprocess(void) override;
			C4API void UpdateEffectGeometry(void) override;
	};


	//# \class	CylinderShaftEffect		Represents a cylinder light shaft effect node in a world.
	//
	//# The $CylinderShaftEffect$ class represents a cylinder light shaft effect node in a world.
	//
	//# \def	class CylinderShaftEffect final : public ShaftEffect
	//
	//# \ctor	CylinderShaftEffect(const Vector2D& size, float height, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section.
	//# \param	height		The height of the light shaft.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $CylinderShaftEffect$ class represents a light shaft effect node having an elliptical
	//# cross section whose dimensions are specified by the $size$ parameter.
	//
	//# \base	ShaftEffect		A cylinder shaft effect is a specific type of shaft effect.
	//
	//# \also	$@CylinderShaftEffectObject@$


	class CylinderShaftEffect final : public ShaftEffect
	{
		friend class ShaftEffect;

		private:

			Point3D						worldEndpoint;
			float						cylinderRadius;

			static SharedVertexBuffer	indexBuffer;

			CylinderShaftEffect();
			CylinderShaftEffect(const CylinderShaftEffect& cylinderShaftEffect);

			Node *Replicate(void) const override;

			void Initialize(void);

			void HandleTransformUpdate(void) override;

			static bool ShaftVisible(const Node *node, const VisibilityRegion *region);
			static bool ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

		public:

			C4API CylinderShaftEffect(const Vector2D& size, float height, const ColorRGBA& color);
			C4API ~CylinderShaftEffect();

			CylinderShaftEffectObject *GetObject(void) const
			{
				return (static_cast<CylinderShaftEffectObject *>(Node::GetObject()));
			}

			C4API void Preprocess(void) override;
			C4API void UpdateEffectGeometry(void) override;
	};


	//# \class	TruncatedPyramidShaftEffect		Represents a truncated pyramid light shaft effect node in a world.
	//
	//# The $TruncatedPyramidShaftEffect$ class represents a truncated pyramid light shaft effect node in a world.
	//
	//# \def	class TruncatedPyramidShaftEffect final : public ShaftEffect
	//
	//# \ctor	TruncatedPyramidShaftEffect(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section.
	//# \param	height		The height of the light shaft.
	//# \param	ratio		The ratio of the upper size to the base size.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $TruncatedPyramidShaftEffect$ class represents a light shaft effect node having a rectangular
	//# cross section at the base whose dimensions are specified by the $size$ parameter. The size of
	//# the light shaft tapers over its height to the ratio specified by the $ratio$ parameter.
	//
	//# \base	ShaftEffect		A truncated pyramid shaft effect is a specific type of shaft effect.
	//
	//# \also	$@TruncatedPyramidShaftEffectObject@$


	class TruncatedPyramidShaftEffect final : public ShaftEffect
	{
		friend class ShaftEffect;

		private:

			Point3D						worldCenter;
			Vector3D					worldAxis[3];

			static SharedVertexBuffer	indexBuffer;

			TruncatedPyramidShaftEffect();
			TruncatedPyramidShaftEffect(const TruncatedPyramidShaftEffect& truncatedPyramidShaftEffect);

			Node *Replicate(void) const override;

			void Initialize(void);

			void HandleTransformUpdate(void) override;

			static bool ShaftVisible(const Node *node, const VisibilityRegion *region);
			static bool ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

		public:

			C4API TruncatedPyramidShaftEffect(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
			C4API ~TruncatedPyramidShaftEffect();

			TruncatedPyramidShaftEffectObject *GetObject(void) const
			{
				return (static_cast<TruncatedPyramidShaftEffectObject *>(Node::GetObject()));
			}

			C4API void Preprocess(void) override;
			C4API void UpdateEffectGeometry(void) override;
	};


	//# \class	TruncatedConeShaftEffect		Represents a truncated cone light shaft effect node in a world.
	//
	//# The $TruncatedConeShaftEffect$ class represents a truncated cone light shaft effect node in a world.
	//
	//# \def	class TruncatedConeShaftEffect final : public ShaftEffect
	//
	//# \ctor	TruncatedConeShaftEffect(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
	//
	//# \param	size		The size of the light shaft's cross section at the base.
	//# \param	height		The height of the light shaft.
	//# \param	ratio		The ratio of the upper size to the base size.
	//# \param	color		The color of the light shaft.
	//
	//# \desc
	//# The $TruncatedConeShaftEffect$ class represents a light shaft effect node having an elliptical
	//# cross section at the base whose dimensions are specified by the $size$ parameter. The size of
	//# the light shaft tapers over its height to the ratio specified by the $ratio$ parameter.
	//
	//# \base	ShaftEffect		A truncated cone shaft effect is a specific type of shaft effect.
	//
	//# \also	$@TruncatedConeShaftEffectObject@$


	class TruncatedConeShaftEffect final : public ShaftEffect
	{
		friend class ShaftEffect;

		private:

			Point3D						worldEndpoint;
			float						cylinderRadius;

			static SharedVertexBuffer	indexBuffer;

			TruncatedConeShaftEffect();
			TruncatedConeShaftEffect(const TruncatedConeShaftEffect& truncatedConeShaftEffect);

			Node *Replicate(void) const override;

			void Initialize(void);

			void HandleTransformUpdate(void) override;

			static bool ShaftVisible(const Node *node, const VisibilityRegion *region);
			static bool ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

		public:

			C4API TruncatedConeShaftEffect(const Vector2D& size, float height, float ratio, const ColorRGBA& color);
			C4API ~TruncatedConeShaftEffect();

			TruncatedConeShaftEffectObject *GetObject(void) const
			{
				return (static_cast<TruncatedConeShaftEffectObject *>(Node::GetObject()));
			}

			C4API void Preprocess(void) override;
			C4API void UpdateEffectGeometry(void) override;
	};
}


#endif

// ZYUQURM
