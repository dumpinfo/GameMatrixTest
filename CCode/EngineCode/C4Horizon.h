 

#ifndef C4Horizon_h
#define C4Horizon_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Processes.h"


namespace C4
{
	enum : ProcessType
	{
		kProcessHorizon		= 'HRZN'
	};


	//# \enum	HorizonFlags

	enum
	{
		kHorizonExcludeInfiniteLight	= 1 << 0,	//## Do not render horizon map for lights with the infinite base type.
		kHorizonExcludePointLight		= 1 << 1	//## Do not render horizon map for lights with the point base type.
	};


	//# \class	HorizonTextureAttribute		Material attribute for a horizon map.
	//
	//# The $HorizonTextureAttribute$ class represents the material attribute for a horizon map.
	//
	//# \def	class HorizonTextureAttribute final : public TextureAttribute
	//
	//# \ctor	explicit HorizonTextureAttribute(const char *name);
	//# \ctor	explicit HorizonTextureAttribute(Texture *texture);
	//# \ctor	HorizonTextureAttribute(const TextureHeader *header, const void *image = nullptr);
	//
	//# \param	name		The name of the texture map to load.
	//# \param	texture		The texture object to use.
	//# \param	header		A texture header from which to construct a new texture object.
	//# \param	image		A pointer to a texture image that is used if the texture header does not specify an offset to an image.
	//
	//# \desc
	//# The $HorizonTextureAttribute$ class represents the material attribute for a horizon map.
	//# A specially-computed horizon map created by the Texture Tool is required for horizon mapping to work properly.
	//#
	//# See the $@TextureAttribute@$ class for a description of the differences among the various constructors.
	//
	//# \base	TextureAttribute	All attributes using a texture map are subclasses of $TextureAttribute$.


	class HorizonTextureAttribute final : public TextureAttribute
	{
		private:

			unsigned_int32		horizonFlags;

			Attribute *Replicate(void) const override;

		public:

			C4API HorizonTextureAttribute();
			C4API explicit HorizonTextureAttribute(const char *name);
			C4API explicit HorizonTextureAttribute(Texture *texture);
			C4API HorizonTextureAttribute(const TextureHeader *header, const void *image = nullptr);
			C4API HorizonTextureAttribute(const HorizonTextureAttribute& horizonTextureAttribute);
			C4API ~HorizonTextureAttribute();

			unsigned_int32 GetHorizonFlags(void) const
			{
				return (horizonFlags);
			}

			void SetHorizonFlags(unsigned_int32 flags)
			{
				horizonFlags = flags;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			bool operator ==(const Attribute& attribute) const;
	};


	class HorizonProcess final : public TextureMapProcess
	{
		private:

			unsigned_int32		horizonFlags;

			static Texture		*horizonTexture;

			HorizonProcess(const HorizonProcess& horizonProcess);

			Process *Replicate(void) const override;

			bool ProcessEnabled(const ShaderCompileData *compileData) const; 

		public:
 
			HorizonProcess();
			~HorizonProcess(); 

			static void Initialize(void);
			static void Terminate(void); 

			unsigned_int32 GetHorizonFlags(void) const 
			{ 
				return (horizonFlags);
			}

			void SetHorizonFlags(unsigned_int32 flags) 
			{
				horizonFlags = flags;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};
}


#endif

// ZYUQURM
