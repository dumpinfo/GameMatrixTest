 

#ifndef C4Skybox_h
#define C4Skybox_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Node.h"
#include "C4Attributes.h"


namespace C4
{
	//# \enum	SkyboxFlags

	enum
	{
		kSkyboxHorizon			= 1 << 0,		//## The skybox is rendered only above the horizon.
		kSkyboxGlowEnable		= 1 << 1,		//## Glow is enabled for the skybox, and the glow intensity is stored in the alpha channel.
		kSkyboxFogInhibit		= 1 << 2,		//## Fog is not applied to the skybox.
		kSkyboxSmearTexture		= 1 << 3		//## The skybox texture is only mapped to the area above the horizon.
	};


	class FrustumCamera;


	//# \class	Skybox		Represents a skybox node in a world.
	//
	//# The $Skybox$ class represents a skybox node in a world.
	//
	//# \def	class Skybox final : public Node
	//
	//# \ctor	Skybox(unsigned_int32 flags = 0);
	//
	//# \param	flags		The skybox flags.
	//
	//# \desc
	//# The $Skybox$ class represents a skybox node consisting of the six faces of a cube rendered at infinity. A skybox node
	//# should always be a direct subnode of the infinite root zone of the world. If a skybox is present, it is rendered whenever
	//# the camera can see into a zone having the $kZoneRenderSkybox$ flag set.
	//#
	//# The $flags$ parameter controls various options applied when rendering the skybox. It can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	SkyboxFlags
	//
	//# \base	Node		A $Skybox$ node is a scene graph node.
	//
	//# \wiki	Skyboxes


	//# \function	Skybox::GetSkyboxFlags		Returns the skybox flags.
	//
	//# \proto	unsigned_int32 GetSkyboxFlags(void) const;
	//
	//# \desc
	//# The $GetSkyboxFlags$ function returns the skybox flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	SkyboxFlags
	//
	//# \also	$@Skybox::SetSkyboxFlags@$


	//# \function	Skybox::SetSkyboxFlags		Returns the skybox flags.
	//
	//# \proto	void SetSkyboxFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new skybox flags.
	//
	//# \desc
	//# The $GetSkyboxFlags$ function sets the skybox flags, which can be a combination (through logical OR) of the following constants.
	//
	//# \table	SkyboxFlags
	//
	//# \also	$@Skybox::GetSkyboxFlags@$


	class Skybox final : public Node
	{
		friend class Node;

		private:

			struct SkyboxVertex
			{
				Point3D		position;
				Point2D		texcoord;
			};

			unsigned_int32				skyboxFlags;
			float						texcoordAdjustment;

			MaterialObject				*materialObject;

			bool						faceRenderFlag[6];

			VertexBuffer				vertexBuffer;
			Storage<Renderable>			faceRenderable[6];
			List<Attribute>				attributeList[6]; 
			DiffuseTextureAttribute		textureAttribute[6];

			Skybox(const Skybox& skybox); 

			Node *Replicate(void) const override; 

			static void MaterialObjectLinkProc(Object *object, void *cookie);
 
		public:
 
			C4API Skybox(unsigned_int32 flags = 0); 
			C4API ~Skybox();

			unsigned_int32 GetSkyboxFlags(void) const
			{ 
				return (skyboxFlags);
			}

			void SetSkyboxFlags(unsigned_int32 flags)
			{
				skyboxFlags = flags;
			}

			MaterialObject *GetMaterialObject(void) const
			{
				return (materialObject);
			}

			const char *GetFaceTextureName(int32 index) const
			{
				return (textureAttribute[index].GetTextureName());
			}

			void SetFaceTextureName(int32 index, const char *name)
			{
				textureAttribute[index].SetTexture(name);
			}

			void Prepack(List<Object> *linkList) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;

			C4API void SetMaterialObject(MaterialObject *object);
			C4API void InvalidateShaderData(void);

			void Preprocess(void) override;
			void Neutralize(void) override;

			void Render(const FrustumCamera *camera, Array<Renderable *, 6> *renderArray);
	};
}


#endif

// ZYUQURM
