 

#ifndef C4Effects_h
#define C4Effects_h


//# \component	Effect Manager
//# \prefix		EffectMgr/


#include "C4Random.h"
#include "C4Renderable.h"
#include "C4Attributes.h"
#include "C4Paths.h"


namespace C4
{
	typedef Type	EffectType;


	enum : ObjectType
	{
		kObjectEffect			= 'EFCT'
	};


	//# \enum	EffectType

	enum : EffectType
	{
		kEffectParticleSystem	= 'PART',		//## Particle system effect.
		kEffectMarking			= 'MARK',		//## Surface marking effect.
		kEffectShaft			= 'SHFT',		//## Light shaft effect.
		kEffectQuad				= 'QUAD',		//## Generic billboarded quad effect.
		kEffectFlare			= 'FLAR',		//## Fractional-occlusion flare effect.
		kEffectBeam				= 'BEAM',		//## Polyboard beam effect.
		kEffectTube				= 'TUBE',		//## Polyboard tube effect.
		kEffectBolt				= 'BOLT',		//## Lightning bolt effect.
		kEffectFire				= 'FIRE',		//## Procedural fire effect.
		kEffectShockwave		= 'SHCK',		//## Shockwave distortion effect.
		kEffectPanel			= 'PANL'		//## Interface panel effect.
	};


	//# \enum	EffectList

	enum
	{
		kEffectListLight,			//## Fully lit effects. These are rendered during the ambient pass and lighting passes.
		kEffectListOpaque,			//## Opaque effects. These are rendered after the final lighting pass and before any transparent effects.
		kEffectListTransparent,		//## Transparent effects. These are rendered after all opaque effects and are sorted back to front.
		kEffectListFrontmost,		//## Frontmost effects. These are render after all transparent effects and are not sorted.
		kEffectListOcclusion,		//## Occlusion queries. These are rendered after the ambient pass and before the first lighting pass.
		kEffectListDistortion,		//## Distortion effects. These are rendered into the distortion buffer for post-processing.
		kEffectListVelocity,
		kEffectListCover,
		kEffectListCount
	};


	//# \enum	EffectFlags

	enum
	{
		kEffectCubeLightInhibit		= 1 << 0		//## Render with point light shaders when illuminated by a cube light.
	};


	//# \enum	QuadFlags

	enum
	{
		kQuadInfinite				= 1 << 0,		//## The quad effect is rendered at infinity.
		kQuadSoftDepth				= 1 << 1,		//## The quad effect fades out as it gets close to scene geometry to avoid depth-testing artifacts.
		kQuadFogInhibit				= 1 << 2		//## Fog is not applied to the quad effect.
	};


	//# \enum	FlareFlags

	enum
	{
		kFlareInfinite				= 1 << 0,		//## The flare effect is rendered at infinity.
		kFlareFogInhibit			= 1 << 1		//## Fog is not applied to the flare effect.
	};


	//# \enum	BeamFlags

	enum
	{
		kBeamSoftDepth				= 1 << 0		//## The beam effect fades out as it gets close to scene geometry to avoid depth-testing artifacts.
	};


	class Effect;
	class FrustumCamera;
	class PointLight;


	//# \class	EffectRegistration		Manages internal registration information for a custom effect type.
	// 
	//# The $EffectRegistration$ class manages internal registration information for a custom effect type.
	//
	//# \def	class EffectRegistration : public Registration<Effect, EffectRegistration> 
	//
	//# \ctor	EffectRegistration(EffectType type, const char *name); 
	//
	//# \param	type		The effect type.
	//# \param	name		The effect name. 
	//
	//# \desc 
	//# The $EffectRegistration$ class is abstract and serves as the common base class for the template class 
	//# $@EffectReg@$. A custom effect is registered with the engine by instantiating an object of type
	//# $EffectReg<classType>$, where $classType$ is the type of the effect subclass being registered.
	//
	//# \base	System/Registration<Effect, EffectRegistration>		An effect registration is a specific type of registration object. 
	//
	//# \also	$@EffectReg@$
	//# \also	$@Effect@$


	//# \function	EffectRegistration::GetEffectType		Returns the registered effect type.
	//
	//# \proto	EffectType GetEffectType(void) const;
	//
	//# \desc
	//# The $GetEffectType$ function returns the effect type for a particular effect registration.
	//# The effect type is established when the effect registration is constructed.
	//
	//# \also	$@EffectRegistration::GetEffectName@$


	//# \function	EffectRegistration::GetEffectName		Returns the human-readable effect name.
	//
	//# \proto	const char *GetEffectName(void) const;
	//
	//# \desc
	//# The $GetEffectName$ function returns the human-readable effect name for a particular effect registration.
	//# The effect name is established when the effect registration is constructed.
	//
	//# \also	$@EffectRegistration::GetEffectType@$


	class EffectRegistration : public Registration<Effect, EffectRegistration>
	{
		private:

			const char		*effectName;

		public:

			C4API EffectRegistration(EffectType type, const char *name);
			C4API ~EffectRegistration();

			EffectType GetEffectType(void) const
			{
				return (GetRegistrableType());
			}

			const char *GetEffectName(void) const
			{
				return (effectName);
			}
	};


	//# \class	EffectReg	 Represents a custom effect type.
	//
	//# The $EffectReg$ class represents a custom effect type.
	//
	//# \def	template <class classType> class EffectReg : public EffectRegistration
	//
	//# \tparam	classType	The custom effect class.
	//
	//# \ctor	EffectReg(EffectType type, const char *name);
	//
	//# \param	type		The effect type.
	//# \param	name		The effect name.
	//
	//# \desc
	//# The $EffectReg$ template class is used to advertise the existence of a custom effect type.
	//# The Effect Manager uses an effect registration to construct a custom effect. The act of instantiating an
	//# $EffectReg$ object automatically registers the corresponding effect type. The effect type is unregistered
	//# when the $EffectReg$ object is destroyed.
	//#
	//# No more than one effect registration should be created for each distinct effect type.
	//
	//# \base	EffectRegistration		All specific effect registration classes share the common base class $EffectRegistration$.
	//
	//# \also	$@Effect@$


	template <class classType> class EffectReg : public EffectRegistration
	{
		public:

			EffectReg(EffectType type, const char *name) : EffectRegistration(type, name)
			{
			}

			Effect *Create(void) const
			{
				return (new classType);
			}
	};


	//# \class	EffectObject		Encapsulates data pertaining to a special effect.
	//
	//# The $EffectObject$ class encapsulates data pertaining to a special effect.
	//
	//# \def	class EffectObject : public Object
	//
	//# \ctor	EffectObject(EffectType type);
	//
	//# The constructor has protected access. The $EffectObject$ class can only exist as the base class for another class.
	//
	//# \param	type	The type of the effect. See below for a list of possible types.
	//
	//# \desc
	//# The $EffectObject$ class is the base class for all types of effect objects, and it stores
	//# data that is common to all of them. Each specific type of effect has an associated object
	//# that is a subclass of the $EffectObject$ class.
	//#
	//# An effect object can have one of the following types.
	//
	//# \table	EffectType
	//
	//# \base	WorldMgr/Object		An $EffectObject$ is an object that can be shared by multiple effect nodes.
	//
	//# \also	$@Effect@$


	//# \function	EffectObject::GetEffectType		Returns the specific type of an effect.
	//
	//# \proto	EffectType GetEffectType(void) const;
	//
	//# \desc
	//# The $GetEffectType$ function returns the specific effect type, which can be one of the following constants
	//# or an application-defined type.
	//
	//# \table	EffectType


	class EffectObject : public Object
	{
		friend class WorldMgr;

		private:

			EffectType		effectType;

			static EffectObject *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API EffectObject(EffectType type);
			C4API ~EffectObject();

		public:

			EffectType GetEffectType(void) const
			{
				return (effectType);
			}

			C4API void PackType(Packer& data) const override;
	};


	//# \class	Effect		Represents a special effect node in a world.
	//
	//# The $Effect$ class represents a special effect node in a world.
	//
	//# \def	class Effect : public RenderableNode, public ListElement<Effect>, public Registrable<Effect, EffectRegistration>
	//
	//# \ctor	Effect(EffectType type, RenderType renderType, unsigned_int32 renderState = 0);
	//
	//# The constructor has protected access. The $Effect$ class can only exist as the base class for a more specific type of effect.
	//
	//# \param	type			The effect type.
	//# \param	renderType		The render type passed to the $Renderable$ base class.
	//# \param	renderState		The render state passed to the $Renderable$ base class.
	//
	//# \desc
	//#
	//
	//# \base	WorldMgr/RenderableNode							An $Effect$ node is a renderable scene graph node.
	//# \base	Utilities/ListElement<Effect>					Used internally by the World Manager.
	//# \base	System/Registrable<Effect, EffectRegistration>	Custom effect types can be registered with the engine.


	//# \function	Effect::GetEffectType		Returns the effect type.
	//
	//# \proto	EffectType GetEffectType(void) const;
	//
	//# \desc
	//# The $GetEffectType$ function returns the specific effect type, which can be one of the following constants
	//# or an application-defined type.
	//
	//# \table	EffectType


	//# \function	Effect::GetEffectListIndex		Returns the effect list index.
	//
	//# \proto	unsigned_int32 GetEffectListIndex(void) const;
	//
	//# \desc
	//# The $GetEffectListIndex$ function returns the index of the render list in which the effect is placed when
	//# it is visible. The list index can be one of the following constants.
	//
	//# \table	EffectList
	//
	//# By default, an effect is placed in the $kEffectListTransparent$ list, but some effect subclasses change this value.
	//
	//# \also	$@Effect::SetEffectListIndex@$


	//# \function	Effect::SetEffectListIndex		Sets the effect list index.
	//
	//# \proto	void SetEffectListIndex(unsigned_int32 index);
	//
	//# \param	index	The effect list index. This may be one of the values listed below.
	//
	//# \desc
	//# The $SetEffectListIndex$ function sets the index of the render list in which the effect is placed when
	//# it is visible. The $index$ parameter can be one of the following constants.
	//
	//# \table	EffectList
	//
	//# By default, an effect is placed in the $kEffectListTransparent$ list, but some effect subclasses change this value.
	//
	//# \also	$@Effect::GetEffectListIndex@$


	//# \function	Effect::SetDistortionState		Sets the state necessary for rendering into the distortion buffer.
	//
	//# \proto	void SetDistortionState(void);
	//
	//# \desc
	//# The $SetDistortionState$ function sets all of the state necessary for rendering an effect into the distortion
	//# buffer. It sets the effect list to $kEffectListDistortion$, sets the $kShaderDistortion$ shader flag, and sets
	//# the blend state to $kBlendAccumulate$. This function should be called by any subclass of $Effect$ that renders
	//# into the distortion buffer.
	//
	//# \also	$@Effect::SetEffectListIndex@$


	//# \function	Effect::Wake		Wakes a sleeping effect.
	//
	//# \proto	void Wake(void);
	//
	//# \desc
	//# The $Wake$ function wakes an effect. The $@Effect::Move@$ function is called only for effects that are currently
	//# in the non-sleeping state. Effects are in the sleeping state by default, so the $Wake$ function must be called
	//# for any effect that needs to have its $Move$ function called. The $Wake$ function would normally be called from
	//# inside a custom effect's $@WorldMgr/Node::Preprocess@$ function.
	//
	//# \also	$@Effect::Sleep@$
	//# \also	$@Effect::Move@$


	//# \function	Effect::Sleep		Puts an effect to sleep.
	//
	//# \proto	void Sleep(void);
	//
	//# \desc
	//# The $Sleep$ function puts an effect to sleep. The $@Effect::Move@$ function is not called for effects that are
	//# currently in the sleeping state. An effect can be put in the non-sleeping state by calling the $@Effect::Wake@$
	//# function. Effects are in the sleeping state by default, so the $Sleep$ function does not need to be called for
	//# custom effects that do not implement a $Move$ function.
	//
	//# \also	$@Effect::Wake@$
	//# \also	$@Effect::Move@$


	//# \function	Effect::Move		Called once per frame to move an effect.
	//
	//# \proto	virtual void Move(void);
	//
	//# \desc
	//# The $Move$ function is called once per frame to allow an effect to perform any necessary movement.
	//# An effect may safely delete itself inside this function if desired.
	//#
	//# The $Move$ function is only called for effects that are in a non-sleeping state. Effects are in the sleeping state
	//# by default, so the $@Effect::Wake@$ function must be called for any effect that needs to have its $Move$ function
	//# called. The $Wake$ function would normally be called from inside a custom effect's $@WorldMgr/Node::Preprocess@$ function.
	//#
	//# Any type of processing that only needs to be done when the effect is known to be visible should be
	//# postponed until the $@Effect::Render@$ function is called. This saves computation when the effect
	//# is not visible or is occluded.
	//
	//# \also	$@Effect::Wake@$
	//# \also	$@Effect::Sleep@$
	//# \also	$@Effect::Render@$


	//# \function	Effect::Render		Called when an effect should be rendered.
	//
	//# \proto	virtual void Render(const FrustumCamera *camera, List<Renderable> *effectList) = 0;
	//
	//# \param	camera		The camera for which the effect is being rendered.
	//# \param	effectList	An array of render lists to which the effect should add its renderables.
	//
	//# \desc
	//# The $Render$ function is called when the World Manager has determined that an effect needs to be rendered.
	//# This function is only called for an effect node that is enabled and has already passed the visibility
	//# and occlusion tests. (An effect can be disabled by setting the $kNodeDisabled$ flag with the
	//# $@WorldMgr/Node::SetNodeFlags@$ function.)
	//#
	//# When the $Render$ function is called, an $Effect$ node should use the $@Utilities/List::Append@$ function
	//# to add itself and any additional renderable objects to one or more of the lists in the array specified by
	//# the $effectList$ parameter. This array should be indexed using the following constants.
	//
	//# \table	EffectList
	//
	//# For example, to add an effect renderable to the transparent effect list, the $Render$ function should
	//# make the following call.
	//
	//# \source
	//# effectList[kEffectListTransparent].Append(this);
	//
	//# \special
	//# Note that the $Render$ function can be called multiple times during the same frame for an effect that
	//# is visible from multiple cameras. The $Render$ function should not perform any iterative movement under
	//# the assumption that the $Render$ function is called only once. Instead, this type of computation should
	//# be peformed in the $@Effect::Move@$ function.
	//
	//# \also	$@Utilities/List@$
	//# \also	$@GraphicsMgr/Renderable@$
	//# \also	$@Effect::Move@$
	//# \also	$@WorldMgr/Node::SetVisibilityProc@$
	//# \also	$@WorldMgr/Node::SetOcclusionProc@$


	class Effect : public RenderableNode, public ListElement<Effect>, public Registrable<Effect, EffectRegistration>
	{
		friend class Node;

		private:

			EffectType			effectType;

			unsigned_int32		effectFlags;
			unsigned_int32		effectListIndex;

			unsigned_int32		renderStamp;
			unsigned_int32		lightStamp;

			static Effect *Create(Unpacker& data, unsigned_int32 unpackFlags = 0);

		protected:

			C4API Effect(EffectType type, RenderType renderType, unsigned_int32 renderState = 0);
			C4API Effect(const Effect& effect);

			C4API void HandleVisibilityUpdate(void) override;

		public:

			C4API virtual ~Effect();

			using ListElement<Effect>::Previous;
			using ListElement<Effect>::Next;

			using Creatable<Effect>::InstallCreator;

			EffectType GetEffectType(void) const
			{
				return (effectType);
			}

			EffectObject *GetObject(void) const
			{
				return (static_cast<EffectObject *>(Node::GetObject()));
			}

			bool Rendering(void) const
			{
				return (ListElement<Renderable>::GetOwningList() != nullptr);
			}

			unsigned_int32 GetEffectFlags(void) const
			{
				return (effectFlags);
			}

			void SetEffectFlags(unsigned_int32 flags)
			{
				effectFlags = flags;
			}

			unsigned_int32 GetEffectListIndex(void) const
			{
				return (effectListIndex);
			}

			void SetEffectListIndex(unsigned_int32 index)
			{
				effectListIndex = index;
			}

			unsigned_int32 GetRenderStamp(void) const
			{
				return (renderStamp);
			}

			void SetRenderStamp(unsigned_int32 stamp)
			{
				renderStamp = stamp;
			}

			unsigned_int32 GetLightStamp(void) const
			{
				return (lightStamp);
			}

			void SetLightStamp(unsigned_int32 stamp)
			{
				lightStamp = stamp;
			}

			void SetDistortionState(void)
			{
				SetAmbientBlendState(kBlendAccumulate);
				SetEffectListIndex(kEffectListDistortion);
				SetShaderFlags(GetShaderFlags() | (kShaderAmbientEffect | kShaderDistortion));
			}

			C4API static Effect *New(EffectType type);

			C4API void PackType(Packer& data) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API int32 GetCategorySettingCount(Type category) const override;
			C4API Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			C4API void SetCategorySetting(Type category, const Setting *setting) override;

			C4API void Preprocess(void) override;
			C4API void Neutralize(void) override;

			C4API virtual void UpdateEffectGeometry(void);

			C4API void Wake(void);
			C4API void Sleep(void);

			C4API virtual void Move(void);
			C4API virtual void Render(const FrustumCamera *camera, List<Renderable> *effectList);
	};


	//# \class	QuadEffectObject		Encapsulates data pertaining to a generic billboarded quad effect.
	//
	//# The $QuadEffectObject$ class encapsulates data pertaining to a generic billboarded quad effect.
	//
	//# \def	class QuadEffectObject : public EffectObject
	//
	//# \ctor	QuadEffectObject(float radius, const ColorRGBA& color, const char *textureName);
	//
	//# \param	radius			The radius of the beam.
	//# \param	color			The color of the beam.
	//# \param	textureName		The name of the quad texture.
	//
	//# \desc
	//#
	//
	//# \base	EffectObject		A $QuadEffectObject$ is an object that can be shared by multiple quad effect nodes.
	//
	//# \also	$@QuadEffect@$


	class QuadEffectObject : public EffectObject
	{
		friend class EffectObject;

		private:

			unsigned_int32		quadFlags;
			float				quadRadius;

			ColorRGBA			quadColor;
			unsigned_int32		quadBlendState;
			float				quadDeltaScale;
			ResourceName		quadTextureName;

			QuadEffectObject();
			~QuadEffectObject();

		public:

			QuadEffectObject(float radius, const ColorRGBA& color, const char *textureName);

			unsigned_int32 GetQuadFlags(void) const
			{
				return (quadFlags);
			}

			void SetQuadFlags(unsigned_int32 flags)
			{
				quadFlags = flags;
			}

			float GetQuadRadius(void) const
			{
				return (quadRadius);
			}

			void SetQuadRadius(float radius)
			{
				quadRadius = radius;
			}

			const ColorRGBA& GetQuadColor(void) const
			{
				return (quadColor);
			}

			void SetQuadColor(const ColorRGBA& color)
			{
				quadColor = color;
			}

			void SetQuadAlpha(float alpha)
			{
				quadColor.alpha = alpha;
			}

			unsigned_int32 GetQuadBlendState(void) const
			{
				return (quadBlendState);
			}

			void SetQuadBlendState(unsigned_int32 blendState)
			{
				quadBlendState = blendState;
			}

			float GetQuadSoftDepthScale(void) const
			{
				return (quadDeltaScale);
			}

			void SetQuadSoftDepthScale(float scale)
			{
				quadDeltaScale = scale;
			}

			const ResourceName& GetQuadTextureName(void) const
			{
				return (quadTextureName);
			}

			void SetQuadTextureName(const char *name)
			{
				quadTextureName = name;
			}

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


	//# \class	QuadEffect		Represents a generic billboarded quad effect node in a world.
	//
	//# The $QuadEffect$ class represents a generic billboarded quad effect node in a world.
	//
	//# \def	class QuadEffect : public Effect
	//
	//# \ctor	QuadEffect(float radius, const ColorRGBA& color, const char *textureName);
	//
	//# \param	radius			The radius of the quad.
	//# \param	color			The color of the quad.
	//# \param	textureName		The name of the quad texture.
	//
	//# \desc
	//#
	//
	//# \base	Effect		A quad effect is a specific type of effect.
	//
	//# \also	$@QuadEffectObject@$


	class QuadEffect : public Effect
	{
		friend class Effect;

		private:

			struct QuadVertex
			{
				Point3D		position;
				Point2D		texcoord;
				Vector2D	billboard;
			};

			int32						quadOrientation;

			QuadVertex					quadVertex[4];
			VertexBuffer				vertexBuffer;

			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;
			DeltaDepthAttribute			deltaDepthAttribute;

			QuadEffect();
			QuadEffect(const QuadEffect& quadEffect);

			Node *Replicate(void) const override;

			void HandlePostprocessUpdate(void) override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

			static bool DirectionVisible(const Node *node, const VisibilityRegion *region);
			static bool DirectionVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

		public:

			C4API QuadEffect(float radius, const ColorRGBA& color, const char *textureName);
			C4API ~QuadEffect();

			QuadEffectObject *GetObject(void) const
			{
				return (static_cast<QuadEffectObject *>(Node::GetObject()));
			}

			int32 GetQuadOrientation(void) const
			{
				return (quadOrientation);
			}

			void SetQuadOrientation(int32 orientation)
			{
				quadOrientation = orientation;
				QuadEffect::UpdateEffectGeometry();
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preload(void) override;
			void Preprocess(void) override;
			void ProcessObjectSettings(void) override;

			C4API void UpdateEffectGeometry(void) override;

			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	FlareEffectObject		Encapsulates data pertaining to a fractional-occlusion flare effect.
	//
	//# The $FlareEffectObject$ class encapsulates data pertaining to a fractional-occlusion flare effect.
	//
	//# \def	class FlareEffectObject : public EffectObject
	//
	//# \ctor	FlareEffectObject(float flare, float occlusion, float rotation, const char *textureName);
	//
	//# \param	flare			The radius of the flare.
	//# \param	occlusion		The radius of the occlusion query.
	//# \param	rotation		The occlusion rotation radius.
	//# \param	textureName		The name of the flare texture.
	//
	//# \desc
	//#
	//# \base	EffectObject		A $FlareEffectObject$ is an object that can be shared by multiple flare effect nodes.
	//
	//# \also	$@FlareEffect@$


	//# \function	FlareEffectObject::GetFlareRadius		Returns the flare radius.
	//
	//# \proto	float GetFlareRadius(void) const;
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::SetFlareRadius@$
	//# \also	$@FlareEffectObject::GetOcclusionRadius@$
	//# \also	$@FlareEffectObject::SetOcclusionRadius@$
	//# \also	$@FlareEffectObject::GetRotationRadius@$
	//# \also	$@FlareEffectObject::SetRotationRadius@$


	//# \function	FlareEffectObject::SetFlareRadius		Sets the flare radius.
	//
	//# \proto	void SetFlareRadius(float radius);
	//
	//# \param	radius		The new flare radius.
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::GetFlareRadius@$
	//# \also	$@FlareEffectObject::GetOcclusionRadius@$
	//# \also	$@FlareEffectObject::SetOcclusionRadius@$
	//# \also	$@FlareEffectObject::GetRotationRadius@$
	//# \also	$@FlareEffectObject::SetRotationRadius@$


	//# \function	FlareEffectObject::GetOcclusionRadius		Returns the occlusion query radius.
	//
	//# \proto	float GetOcclusionRadius(void) const;
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::SetOcclusionRadius@$
	//# \also	$@FlareEffectObject::GetRotationRadius@$
	//# \also	$@FlareEffectObject::SetRotationRadius@$
	//# \also	$@FlareEffectObject::GetFlareRadius@$
	//# \also	$@FlareEffectObject::SetFlareRadius@$


	//# \function	FlareEffectObject::SetOcclusionRadius		Sets the occlusion query radius.
	//
	//# \proto	void SetOcclusionRadius(float radius);
	//
	//# \param	radius		The new occlusion query radius.
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::GetOcclusionRadius@$
	//# \also	$@FlareEffectObject::GetRotationRadius@$
	//# \also	$@FlareEffectObject::SetRotationRadius@$
	//# \also	$@FlareEffectObject::GetFlareRadius@$
	//# \also	$@FlareEffectObject::SetFlareRadius@$


	//# \function	FlareEffectObject::GetRotationRadius		Returns the occlusion rotation radius.
	//
	//# \proto	float GetRotationRadius(void) const;
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::SetRotationRadius@$
	//# \also	$@FlareEffectObject::GetOcclusionRadius@$
	//# \also	$@FlareEffectObject::SetOcclusionRadius@$
	//# \also	$@FlareEffectObject::GetFlareRadius@$
	//# \also	$@FlareEffectObject::SetFlareRadius@$


	//# \function	FlareEffectObject::SetRotationRadius		Sets the occlusion rotation radius.
	//
	//# \proto	void SetRotationRadius(float radius);
	//
	//# \param	radius		The new occlusion rotation radius.
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::GetRotationRadius@$
	//# \also	$@FlareEffectObject::GetOcclusionRadius@$
	//# \also	$@FlareEffectObject::SetOcclusionRadius@$
	//# \also	$@FlareEffectObject::GetFlareRadius@$
	//# \also	$@FlareEffectObject::SetFlareRadius@$


	//# \function	FlareEffectObject::GetFlareTextureName		Returns the name of the flare texture.
	//
	//# \proto	const ResourceName& GetFlareTextureName(void) const;
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::SetFlareTextureName@$


	//# \function	FlareEffectObject::SetFlareTextureName		Sets the name of the flare texture.
	//
	//# \proto	void SetFlareTextureName(const char *name);
	//
	//# \param	name	The name of the flare texture.
	//
	//# \desc
	//
	//# \also	$@FlareEffectObject::GetFlareTextureName@$


	class FlareEffectObject : public EffectObject
	{
		friend class EffectObject;

		private:

			unsigned_int32		flareFlags;
			float				flareRadius;
			float				occlusionRadius;
			float				rotationRadius;

			ColorRGB			flareColor;
			ResourceName		flareTextureName;

			FlareEffectObject();
			~FlareEffectObject();

		public:

			FlareEffectObject(float flare, float occlusion, float rotation, const char *textureName);

			unsigned_int32 GetFlareFlags(void) const
			{
				return (flareFlags);
			}

			void SetFlareFlags(unsigned_int32 flags)
			{
				flareFlags = flags;
			}

			float GetFlareRadius(void) const
			{
				return (flareRadius);
			}

			void SetFlareRadius(float radius)
			{
				flareRadius = radius;
			}

			float GetOcclusionRadius(void) const
			{
				return (occlusionRadius);
			}

			void SetOcclusionRadius(float radius)
			{
				occlusionRadius = radius;
			}

			float GetRotationRadius(void) const
			{
				return (rotationRadius);
			}

			void SetRotationRadius(float radius)
			{
				rotationRadius = radius;
			}

			const ColorRGB& GetFlareColor(void) const
			{
				return (flareColor);
			}

			void SetFlareColor(const ColorRGB& color)
			{
				flareColor = color;
			}

			const ResourceName& GetFlareTextureName(void) const
			{
				return (flareTextureName);
			}

			void SetFlareTextureName(const char *name)
			{
				flareTextureName = name;
			}

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


	//# \class	FlareEffect		Represents a fractional-occlusion flare effect node in a world.
	//
	//# The $FlareEffect$ class represents a fractional-occlusion flare effect node in a world.
	//
	//# \def	class FlareEffect : public Effect
	//
	//# \ctor	FlareEffect(float flare, float occlusion, float rotation, const char *textureName);
	//
	//# \param	flare			The radius of the flare.
	//# \param	occlusion		The radius of the occlusion query.
	//# \param	rotation		The occlusion rotation radius.
	//# \param	textureName		The name of the flare texture.
	//
	//# \desc
	//#
	//
	//# \base	Effect		A flare effect is a specific type of effect.
	//
	//# \also	$@FlareEffectObject@$


	class FlareEffect : public Effect
	{
		friend class Effect;

		private:

			struct FlareVertex
			{
				Point3D		position;
				Point2D		texcoord;
				Vector2D	billboard;
			};

			struct OcclusionVertex
			{
				Point3D		position;
				Vector2D	billboard;
			};

			float						inverseOcclusionSize;

			FlareVertex					flareVertex[4];
			VertexBuffer				vertexBuffer;

			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;

			OcclusionVertex				occlusionVertex[4];
			Renderable					occlusionRenderable;
			VertexBuffer				occlusionVertexBuffer;
			OcclusionQuery				occlusionQuery;

			FlareEffect();
			FlareEffect(const FlareEffect& flareEffect);

			Node *Replicate(void) const override;

			void HandlePostprocessUpdate(void) override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

			static bool DirectionVisible(const Node *node, const VisibilityRegion *region);
			static bool DirectionVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList);

			static void RenderFlare(OcclusionQuery *query, List<Renderable> *renderList, void *cookie);

		public:

			C4API FlareEffect(float flare, float occlusion, float rotation, const char *textureName);
			C4API ~FlareEffect();

			FlareEffectObject *GetObject(void) const
			{
				return (static_cast<FlareEffectObject *>(Node::GetObject()));
			}

			void Preload(void) override;
			void Preprocess(void) override;
			void ProcessObjectSettings(void) override;

			C4API void UpdateEffectGeometry(void) override;

			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	BeamEffectObject		Encapsulates data pertaining to a polyboard beam effect.
	//
	//# The $BeamEffectObject$ class encapsulates data pertaining to a polyboard beam effect.
	//
	//# \def	class BeamEffectObject : public EffectObject
	//
	//# \ctor	BeamEffectObject(float radius, float height, const ColorRGBA& color, const char *textureName = nullptr);
	//
	//# \param	radius			The radius of the beam.
	//# \param	height			The height (or length) of the beam.
	//# \param	color			The color of the beam.
	//# \param	textureName		The name of the texture applied to the beam.
	//
	//# \desc
	//#
	//# \base	EffectObject		A $BeamEffectObject$ is an object that can be shared by multiple beam effect nodes.
	//
	//# \also	$@BeamEffect@$


	class BeamEffectObject : public EffectObject
	{
		friend class EffectObject;

		private:

			unsigned_int32		beamFlags;

			float				beamRadius;
			float				beamHeight;

			ColorRGBA			beamColor;
			float				beamDeltaScale;
			float				beamTexcoordScale;
			ResourceName		beamTextureName;

			BeamEffectObject();
			~BeamEffectObject();

		public:

			BeamEffectObject(float radius, float height, const ColorRGBA& color, const char *textureName = nullptr);

			unsigned_int32 GetBeamFlags(void) const
			{
				return (beamFlags);
			}

			void SetBeamFlags(unsigned_int32 flags)
			{
				beamFlags = flags;
			}

			float GetBeamRadius(void) const
			{
				return (beamRadius);
			}

			void SetBeamRadius(float radius)
			{
				beamRadius = radius;
			}

			float GetBeamHeight(void) const
			{
				return (beamHeight);
			}

			void SetBeamHeight(float height)
			{
				beamHeight = height;
			}

			const ColorRGBA& GetBeamColor(void) const
			{
				return (beamColor);
			}

			void SetBeamColor(const ColorRGBA& color)
			{
				beamColor = color;
			}

			float GetBeamSoftDepthScale(void) const
			{
				return (beamDeltaScale);
			}

			void SetBeamSoftDepthScale(float scale)
			{
				beamDeltaScale = scale;
			}

			float GetBeamTexcoordScale(void) const
			{
				return (beamTexcoordScale);
			}

			void SetBeamTexcoordScale(float scale)
			{
				beamTexcoordScale = scale;
			}

			const ResourceName& GetBeamTextureName(void) const
			{
				return (beamTextureName);
			}

			void SetBeamTextureName(const char *name)
			{
				beamTextureName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;
	};


	//# \class	BeamEffect		Represents a polyboard beam effect node in a world.
	//
	//# The $BeamEffect$ class represents a polyboard beam effect node in a world.
	//
	//# \def	class BeamEffect : public Effect
	//
	//# \ctor	BeamEffect(float radius, float height, const ColorRGBA& color, const char *textureName = nullptr);
	//
	//# \param	radius			The radius of the beam.
	//# \param	height			The height (or length) of the beam.
	//# \param	color			The color of the beam.
	//# \param	textureName		The name of the texture applied to the beam.
	//
	//# \desc
	//#
	//
	//# \base	Effect		A beam effect is a specific type of effect.
	//
	//# \also	$@BeamEffectObject@$


	class BeamEffect : public Effect
	{
		friend class Effect;

		private:

			struct BeamVertex
			{
				Point3D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;
			DeltaDepthAttribute			deltaDepthAttribute;

			BeamEffect();
			BeamEffect(const BeamEffect& beamEffect);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API BeamEffect(float radius, float height, const ColorRGBA& color, const char *textureName = nullptr);
			C4API ~BeamEffect();

			BeamEffectObject *GetObject(void) const
			{
				return (static_cast<BeamEffectObject *>(Node::GetObject()));
			}

			void Preload(void) override;
			void Preprocess(void) override;
			void ProcessObjectSettings(void) override;

			C4API void UpdateEffectGeometry(void) override;

			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	TubeEffectObject		Encapsulates data pertaining to a polyboard tube effect.
	//
	//# The $TubeEffectObject$ class encapsulates data pertaining to a polyboard tube effect.
	//
	//# \def	class TubeEffectObject : public EffectObject
	//
	//# \ctor	TubeEffectObject(const Path *path, float radius, const ColorRGBA& color, const char *textureName = nullptr);
	//
	//# \param	path			The path along which the tube is created.
	//# \param	radius			The radius of the tube.
	//# \param	color			The color of the tube.
	//# \param	textureName		The name of the texture applied to the tube.
	//
	//# \desc
	//#
	//# \base	EffectObject		A $TubeEffectObject$ is an object that can be shared by multiple tube effect nodes.
	//
	//# \also	$@TubeEffect@$


	class TubeEffectObject : public EffectObject
	{
		friend class EffectObject;

		public:

			struct TubeVertex
			{
				Point3D		position;
				Vector4D	tangent;
				Point2D		texcoord;
			};

		private:

			float				tubeRadius;

			Path				tubePath;
			Box3D				pathBoundingBox;

			ColorRGBA			tubeColor;
			ResourceName		tubeTextureName;
			float				texcoordScale;
			int32				maxSubdiv;

			int32				tubeVertexCount;
			TubeVertex			*tubeVertex;
			VertexBuffer		vertexBuffer;

		protected:

			TubeEffectObject(EffectType type = kEffectTube);
			TubeEffectObject(EffectType type, const Path *path, float radius, const ColorRGBA& color, const char *textureName = nullptr);
			~TubeEffectObject();

			TubeVertex *GetVertexArray(void)
			{
				return (tubeVertex);
			}

			void AllocateStorage(int32 vertexCount);

		public:

			TubeEffectObject(const Path *path, float radius, const ColorRGBA& color, const char *textureName = nullptr);

			float GetTubeRadius(void) const
			{
				return (tubeRadius);
			}

			void SetTubeRadius(float radius)
			{
				tubeRadius = radius;
			}

			Path *GetTubePath(void)
			{
				return (&tubePath);
			}

			const Path *GetTubePath(void) const
			{
				return (&tubePath);
			}

			const Box3D& GetPathBoundingBox(void) const
			{
				return (pathBoundingBox);
			}

			const ColorRGBA& GetTubeColor(void) const
			{
				return (tubeColor);
			}

			void SetTubeColor(const ColorRGBA& color)
			{
				tubeColor = color;
			}

			const ResourceName& GetTubeTextureName(void) const
			{
				return (tubeTextureName);
			}

			void SetTubeTextureName(const char *name)
			{
				tubeTextureName = name;
			}

			float GetTexcoordScale(void) const
			{
				return (texcoordScale);
			}

			void SetTexcoordScale(float scale)
			{
				texcoordScale = scale;
			}

			int32 GetMaxSubdiv(void) const
			{
				return (maxSubdiv);
			}

			void SetMaxSubdiv(int32 subdiv)
			{
				maxSubdiv = subdiv;
			}

			int32 GetVertexCount(void) const
			{
				return (tubeVertexCount);
			}

			const TubeVertex *GetVertexArray(void) const
			{
				return (tubeVertex);
			}

			VertexBuffer *GetVertexBuffer(void)
			{
				return (&vertexBuffer);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			int32 GetObjectSize(float *size) const override;
			void SetObjectSize(const float *size) override;

			C4API void SetTubePath(const Path *path);
			C4API virtual void Build(void);
	};


	//# \class	TubeEffect		Represents a polyboard tube effect node in a world.
	//
	//# The $TubeEffect$ class represents a polyboard tube effect node in a world.
	//
	//# \def	class TubeEffect : public Effect
	//
	//# \ctor	TubeEffect(const Path *path, float radius, const ColorRGBA& color, const char *textureName = nullptr);
	//
	//# \param	path			The path along which the tube is created.
	//# \param	radius			The radius of the tube.
	//# \param	color			The color of the tube.
	//# \param	textureName		The name of the texture applied to the tube.
	//
	//# \desc
	//#
	//
	//# \base	Effect		A tube effect is a specific type of effect.
	//
	//# \also	$@TubeEffectObject@$


	class TubeEffect : public Effect
	{
		friend class Effect;

		private:

			List<Attribute>				attributeList;
			DiffuseAttribute			colorAttribute;
			DiffuseTextureAttribute		textureAttribute;

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		protected:

			TubeEffect(EffectType type = kEffectTube);
			TubeEffect(EffectType type, const Path *path, float radius, const ColorRGBA& color, const char *textureName = nullptr);
			TubeEffect(const TubeEffect& tubeEffect);

		public:

			C4API TubeEffect(const Path *path, float radius, const ColorRGBA& color, const char *textureName = nullptr);
			C4API ~TubeEffect();

			TubeEffectObject *GetObject(void) const
			{
				return (static_cast<TubeEffectObject *>(Node::GetObject()));
			}

			void SetTubeColor(const ColorRGBA& color)
			{
				colorAttribute.SetDiffuseColor(color);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetInternalConnectorCount(void) const override;
			const char *GetInternalConnectorKey(int32 index) const override;
			bool ValidConnectedNode(const ConnectorKey& key, const Node *node) const override;
			C4API PathMarker *GetConnectedPathMarker(void) const;
			C4API void SetConnectedPathMarker(PathMarker *marker);

			void Preload(void) override;
			void Preprocess(void) override;
			void ProcessObjectSettings(void) override;
	};


	class BoltEffectObject : public TubeEffectObject
	{
		friend class EffectObject;

		private:

			float			maxPathDeviation;

			int32			branchingDepth;
			int32			branchCount;
			float			branchRadiusScale;
			Range<float>	branchLengthRange;

			BoltEffectObject();
			~BoltEffectObject();

			void BuildBranch(const Path *path, float radius, float deviation, int32 zdiv, TubeVertex *vertex);

		public:

			BoltEffectObject(const Path *path, float radius, float deviation, const ColorRGBA& color, const char *textureName = nullptr);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			C4API void Build(void) override;
	};


	class BoltEffect : public TubeEffect
	{
		friend class Effect;

		private:

			BoltEffect();
			BoltEffect(const BoltEffect& boltEffect);

			Node *Replicate(void) const override;

		public:

			C4API BoltEffect(const Path *path, float radius, float deviation, const ColorRGBA& color, const char *textureName = nullptr);
			C4API ~BoltEffect();

			BoltEffectObject *GetObject(void) const
			{
				return (static_cast<BoltEffectObject *>(Node::GetObject()));
			}
	};


	//# \class	FireEffectObject		Encapsulates data pertaining to a procedural fire effect.
	//
	//# The $FireEffectObject$ class encapsulates data pertaining to a procedural fire effect.
	//
	//# \def	class FireEffectObject : public EffectObject
	//
	//# \ctor	FireEffectObject(float radius, float height, float intensity, int32 speed, const char *textureName);
	//
	//# \param	radius			The radius of the fire.
	//# \param	height			The height of the fire.
	//# \param	intensity		The fire animation intensity.
	//# \param	speed			The fire animation speed.
	//# \param	textureName		The name of the fire texture.
	//
	//# \desc
	//#
	//# \base	EffectObject		A $FireEffectObject$ is an object that can be shared by multiple fire effect nodes.
	//
	//# \also	$@FireEffect@$


	//# \function	FireEffectObject::GetFireRadius		Returns the fire radius.
	//
	//# \proto	float GetFireRadius(void) const;
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::SetFireRadius@$
	//# \also	$@FireEffectObject::GetFireHeight@$
	//# \also	$@FireEffectObject::SetFireHeight@$


	//# \function	FireEffectObject::SetFireRadius		Sets the fire radius.
	//
	//# \proto	void SetFireRadius(float radius);
	//
	//# \param	radius		The new fire radius.
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::GetFireRadius@$
	//# \also	$@FireEffectObject::GetFireHeight@$
	//# \also	$@FireEffectObject::SetFireHeight@$


	//# \function	FireEffectObject::GetFireHeight		Returns the fire height.
	//
	//# \proto	float GetFireHeight(void) const;
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::SetFireHeight@$
	//# \also	$@FireEffectObject::GetFireRadius@$
	//# \also	$@FireEffectObject::SetFireRadius@$


	//# \function	FireEffectObject::SetFireHeight		Sets the fire height.
	//
	//# \proto	void SetFireHeight(float height);
	//
	//# \param	height		The new fire height.
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::GetFireHeight@$
	//# \also	$@FireEffectObject::GetFireRadius@$
	//# \also	$@FireEffectObject::SetFireRadius@$


	//# \function	FireEffectObject::GetFireIntensity		Returns the fire animation intensity.
	//
	//# \proto	float GetFireIntensity(void) const;
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::SetFireIntensity@$
	//# \also	$@FireEffectObject::GetFireSpeed@$
	//# \also	$@FireEffectObject::SetFireSpeed@$


	//# \function	FireEffectObject::SetFireIntensity		Sets the fire animation intensity.
	//
	//# \proto	void SetFireIntensity(float intensity);
	//
	//# \param	intensity	The new fire animation intensity.
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::GetFireIntensity@$
	//# \also	$@FireEffectObject::GetFireSpeed@$
	//# \also	$@FireEffectObject::SetFireSpeed@$


	//# \function	FireEffectObject::GetFireSpeed		Returns the fire animation speed.
	//
	//# \proto	float GetFireSpeed(void) const;
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::SetFireSpeed@$
	//# \also	$@FireEffectObject::GetFireIntensity@$
	//# \also	$@FireEffectObject::SetFireIntensity@$


	//# \function	FireEffectObject::SetFireSpeed		Sets the fire animation speed.
	//
	//# \proto	void SetFireSpeed(int32 speed);
	//
	//# \param	speed		The new fire animation speed. This must be a value between 0 and 24, inclusive.
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::GetFireSpeed@$
	//# \also	$@FireEffectObject::GetFireIntensity@$
	//# \also	$@FireEffectObject::SetFireIntensity@$


	//# \function	FireEffectObject::GetFireTextureName		Returns the name of the fire texture.
	//
	//# \proto	const ResourceName& GetFireTextureName(void) const;
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::SetFireTextureName@$


	//# \function	FireEffectObject::SetFireTextureName		Sets the name of the fire texture.
	//
	//# \proto	void SetFireTextureName(const char *name);
	//
	//# \param	name	The name of the fire texture.
	//
	//# \desc
	//
	//# \also	$@FireEffectObject::GetFireTextureName@$


	class FireEffectObject : public EffectObject
	{
		friend class EffectObject;

		private:

			float			fireRadius;
			float			fireHeight;

			float			fireIntensity;
			int32			fireSpeed;

			ResourceName	fireTextureName;

			FireEffectObject();
			~FireEffectObject();

		public:

			FireEffectObject(float radius, float height, float intensity, int32 speed, const char *textureName);

			float GetFireRadius(void) const
			{
				return (fireRadius);
			}

			void SetFireRadius(float radius)
			{
				fireRadius = radius;
			}

			float GetFireHeight(void) const
			{
				return (fireHeight);
			}

			void SetFireHeight(float height)
			{
				fireHeight = height;
			}

			float GetFireIntensity(void) const
			{
				return (fireIntensity);
			}

			void SetFireIntensity(float intensity)
			{
				fireIntensity = intensity;
			}

			int32 GetFireSpeed(void) const
			{
				return (fireSpeed);
			}

			void SetFireSpeed(int32 speed)
			{
				fireSpeed = speed;
			}

			const ResourceName& GetFireTextureName(void) const
			{
				return (fireTextureName);
			}

			void SetFireTextureName(const char *name)
			{
				fireTextureName = name;
			}

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


	//# \class	FireEffect		Represents a procedural fire effect node in a world.
	//
	//# The $FireEffect$ class represents a procedural fire effect node in a world.
	//
	//# \def	class FireEffect : public Effect
	//
	//# \ctor	FireEffect(float radius, float height, float intensity, int32 speed, const char *textureName);
	//
	//# \param	radius			The radius of the fire.
	//# \param	height			The height of the fire.
	//# \param	intensity		The fire animation intensity.
	//# \param	speed			The fire animation speed.
	//# \param	textureName		The name of the fire texture.
	//
	//# \desc
	//#
	//
	//# \base	Effect		A fire effect is a specific type of effect.
	//
	//# \also	$@FireEffectObject@$


	class FireEffect : public Effect
	{
		friend class Effect;

		private:

			struct FireVertex
			{
				Point3D		position;
				Vector4D	texcoord;
			};

			FireVertex					fireVertex[4];

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseTextureAttribute		textureAttribute;
			FireTextureAttribute		fireAttribute;

			FireEffect();
			FireEffect(const FireEffect& fireEffect);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API FireEffect(float radius, float height, float intensity, int32 speed, const char *textureName);
			C4API ~FireEffect();

			FireEffectObject *GetObject(void) const
			{
				return (static_cast<FireEffectObject *>(Node::GetObject()));
			}

			void Preload(void) override;
			void Preprocess(void) override;
			void ProcessObjectSettings(void) override;

			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};


	//# \class	ShockwaveEffect		Represents a shockwave effect node in a world.
	//
	//# The $ShockwaveEffect$ class represents a shockwave effect node in a world.
	//
	//# \def	class ShockwaveEffect : public Effect
	//
	//# \ctor	ShockwaveEffect(const char *textureName, float radius, float width, float speed);
	//
	//# \param	textureName		The name of the distortion texture to use for the effect.
	//# \param	radius			The maximum radius to which the effect will grow in size.
	//# \param	width			The width of the shockwave ring, the difference between the outer radius and inner radius.
	//# \param	speed			The speed at which the shockwave radius increases, in units per millisecond.
	//
	//# \desc
	//# The $ShockwaveEffect$ class produces the effect of an expanding shockwave ring that is rendered into
	//# the distortion buffer. The shockwave begins with a radius of zero and expands at the rate specified by
	//# the $speed$ parameter until the radius exceeds the maximum size given by the $radius$ parameter.
	//# When the maximum radius is reached, the shockwave effect is automatically destroyed.
	//#
	//# The texture map specified by the $textureName$ parameter should contain a distortion pattern in the red and
	//# green channels. The shockwave effect uses the center pixel of the texture for the entire inner edge of the
	//# expanding ring, and the boundary of an inscribed circle in the texture image corresponds to the outer edge
	//# of the ring.
	//
	//# \base	Effect		A shockwave effect is a specific type of effect.


	class ShockwaveEffect : public Effect
	{
		friend class Effect;

		private:

			struct ShockwaveVertex
			{
				Point2D		position;
				Point2D		texcoord;
			};

			float						shockRadius;
			float						maxShockRadius;
			float						shockWidth;
			float						shockSpeed;

			VertexBuffer				vertexBuffer;
			List<Attribute>				attributeList;
			DiffuseTextureAttribute		textureAttribute;

			ShockwaveEffect();
			ShockwaveEffect(const ShockwaveEffect& shockwaveEffect);

			Node *Replicate(void) const override;

			bool CalculateBoundingBox(Box3D *box) const override;
			bool CalculateBoundingSphere(BoundingSphere *sphere) const override;

		public:

			C4API ShockwaveEffect(const char *textureName, float radius, float width, float speed);
			C4API ~ShockwaveEffect();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;

			void Move(void) override;
			void Render(const FrustumCamera *camera, List<Renderable> *effectList) override;
	};
}


#endif

// ZYUQURM
