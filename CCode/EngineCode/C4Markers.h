 

#ifndef C4Markers_h
#define C4Markers_h


//# \component	World Manager
//# \prefix		WorldMgr/


#include "C4Node.h"


namespace C4
{
	typedef Type	MarkerType;
	typedef Type	LocatorType;


	//# \enum	MarkerType

	enum : MarkerType
	{
		kMarkerLocator		= 'LOCA',		//## Locator marker.
		kMarkerConnection	= 'CONN',		//## Connection marker.
		kMarkerCube			= 'CUBE',		//## Cube environment map marker.
		kMarkerShader		= 'SHDR',		//## Shader warmup marker.
		kMarkerPath			= 'PATH'		//## Path marker.
	};


	//# \enum	CubeFlags

	enum
	{
		kCubeFilter			= 1 << 0		//## Apply a blur filter to the cube map.
	};


	//# \class	Marker		Represents a marker node in a world.
	//
	//# The $Marker$ class represents a marker node in a world.
	//
	//# \def	class Marker : public Node, public ListElement<Marker>
	//
	//# \ctor	Marker(MarkerType type);
	//
	//# The constructor has protected access. A $Marker$ class can only exist as the base class for a more specific type of marker.
	//
	//# \param	type		The type of the marker. See below for a list of possible types.
	//
	//# \desc
	//# The $Marker$ class is the base class for all marker nodes. The $type$ parameter can be one of the following
	//# constants, and it is specified by the subclass's constructor.
	//
	//# \table	MarkerType
	//
	//# \base	Node							A $Marker$ node is a scene graph node.
	//# \base	Utilities/ListElement<Marker>	Markers are stored in a list owned by their containing zone.
	//
	//# \wiki	Markers


	//# \function	Marker::GetMarkerType		Returns the type of a marker.
	//
	//# \proto	MarkerType GetMarkerType(void) const;
	//
	//# \desc
	//# The $GetMarkerType$ function returns the specific marker type, which can be one of the following constants.
	//
	//# \table	MarkerType


	class Marker : public Node, public ListElement<Marker>
	{
		friend class Node;

		private:

			MarkerType		markerType;

			static Marker *Create(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API Marker(MarkerType type);
			C4API Marker(const Marker& marker);

		public:

			C4API virtual ~Marker();

			using ListElement<Marker>::Previous;
			using ListElement<Marker>::Next;

			MarkerType GetMarkerType(void) const
			{
				return (markerType);
			}

			C4API void PackType(Packer& data) const override;

			C4API void Preprocess(void) override; 
			C4API void Neutralize(void) override;
	};
 

	class LocatorRegistration : public MapElement<LocatorRegistration> 
	{
		private:
 
			LocatorType		locatorType;
			const char		*locatorName; 
 
		public:

			typedef LocatorType KeyType;
 
			C4API LocatorRegistration(LocatorType type, const char *name);
			C4API ~LocatorRegistration();

			KeyType GetKey(void) const
			{
				return (locatorType);
			}

			LocatorType GetLocatorType(void) const
			{
				return (locatorType);
			}

			const char *GetLocatorName(void) const
			{
				return (locatorName);
			}
	};


	//# \class	LocatorMarker		Represents a locator marker node in a world.
	//
	//# The $LocatorMarker$ class represents a locator marker node in a world.
	//
	//# \def	class LocatorMarker final : public Marker
	//
	//# \ctor	LocatorMarker(LocatorType type);
	//
	//# \param	type		The type of the locator.
	//
	//# \desc
	//#
	//
	//# \base	Marker		A locator marker is a specific type of marker.


	//# \function	LocatorMarker::GetLocatorType		Returns the locator type.
	//
	//# \proto	LocatorType GetLocatorType(void) const;
	//
	//# \desc
	//
	//# \also	$@LocatorMarker::SetLocatorType@$


	//# \function	LocatorMarker::SetLocatorType		Sets the locator type.
	//
	//# \proto	void SetLocatorType(LocatorType type);
	//
	//# \param	type	The new locator type.
	//
	//# \desc
	//
	//# \also	$@LocatorMarker::GetLocatorType@$


	class LocatorMarker final : public Marker
	{
		friend class LocatorRegistration;
		friend class Marker;

		private:

			LocatorType		locatorType;

			static C4API Map<LocatorRegistration>		registrationMap;

			LocatorMarker();
			LocatorMarker(const LocatorMarker& locatorMarker);

			Node *Replicate(void) const override;

		public:

			C4API LocatorMarker(LocatorType type);
			C4API ~LocatorMarker();

			LocatorType GetLocatorType(void) const
			{
				return (locatorType);
			}

			void SetLocatorType(LocatorType type)
			{
				locatorType = type;
			}

			static const LocatorRegistration *GetFirstRegistration(void)
			{
				return (registrationMap.First());
			}

			static const LocatorRegistration *FindRegistration(LocatorType type)
			{
				return (registrationMap.Find(type));
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	ConnectionMarker	Represents a connection marker in a world.
	//
	//# The $ConnectionMarker$ class represents a connection marker in a world.
	//
	//# \def	class ConnectionMarker final : public Node
	//
	//# \ctor	ConnectionMarker();
	//
	//# \desc
	//#
	//
	//# \base	Marker		A connection marker is a specific type of marker.
	//
	//# \also	$@Connector@$


	class ConnectionMarker final : public Marker
	{
		private:

			ConnectionMarker(const ConnectionMarker& connectionMarker);

			Node *Replicate(void) const override;

		public:

			C4API ConnectionMarker();
			C4API ~ConnectionMarker();
	};


	//# \class	CubeMarker		Represents a cube environment map marker node in a world.
	//
	//# The $CubeMarker$ class represents a cube environment map marker node in a world.
	//
	//# \def	class CubeMarker final : public Marker
	//
	//# \ctor	CubeMarker(const char *name, TextureFormat format, int32 size);
	//
	//# \param	name		The name of the environment map texture resource.
	//# \param	format		The texture format that is generated.
	//# \param	size		The square size of the cube texture map, in pixels.
	//
	//# \desc
	//#
	//
	//# \base	Marker		A cube environment map marker is a specific type of marker.


	//# \function	CubeMarker::GetCubeFlags		Returns the cube environment map flags.
	//
	//# \proto	unsigned_int32 GetCubeFlags(void) const;
	//
	//# \desc
	//# The $GetCubeFlags$ function returns the cube environment map flags, which can be a combination
	//# (through logical OR) of the following constants.
	//
	//# \table	CubeFlags
	//
	//# \also	$@CubeMarker::SetCubeFlags@$


	//# \function	CubeMarker::SetCubeFlags		Sets the cube environment map flags.
	//
	//# \proto	void SetCubeFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new cube environment map flags.
	//
	//# \desc
	//# The $GetCubeFlags$ function sets the cube environment map flags to the value specified by the $flags$ parameter.
	//# It can be a combination (through logical OR) of the following constants.
	//
	//# \table	CubeFlags
	//
	//# \also	$@CubeMarker::GetCubeFlags@$


	//# \function	CubeMarker::GetCubeSize		Returns the size of the cube texture map.
	//
	//# \proto	int32 GetCubeSize(void) const;
	//
	//# \desc
	//
	//# \also	$@CubeMarker::SetCubeSize@$


	//# \function	CubeMarker::SetCubeSize		Sets the size of the cube texture map.
	//
	//# \proto	void SetCubeSize(int32 size);
	//
	//# \param	size	The size of the cube texture map. This must be a power of two.
	//
	//# \desc
	//
	//# \also	$@CubeMarker::GetCubeSize@$


	//# \function	CubeMarker::GetTextureFormat		Returns the texture format.
	//
	//# \proto	TextureFormat GetTextureFormat(void) const;
	//
	//# \desc
	//
	//# \also	$@CubeMarker::SetTextureFormat@$


	//# \function	CubeMarker::SetTextureFormat		Sets the texture format.
	//
	//# \proto	void SetTextureFormat(TextureFormat format);
	//
	//# \param	format	The format of the cube texture map. This must be $kTextureRGBA8$ or $kTextureI8$.
	//
	//# \desc
	//
	//# \also	$@CubeMarker::GetTextureFormat@$


	//# \function	CubeMarker::GetTextureName		Returns the texture resource name.
	//
	//# \proto	const ResourceName& GetTextureName(void) const;
	//
	//# \desc
	//
	//# \also	$@CubeMarker::SetTextureName@$


	//# \function	CubeMarker::SetTextureName		Sets the texture resource name.
	//
	//# \proto	void SetTextureName(const char *name);
	//
	//# \param	name	The name of the cube texture map resource.
	//
	//# \desc
	//
	//# \also	$@CubeMarker::GetTextureName@$


	class CubeMarker final : public Marker
	{
		friend class Marker;

		private:

			unsigned_int32		cubeFlags;
			int32				cubeSize;

			TextureFormat		textureFormat;
			ResourceName		textureName;

			CubeMarker();
			CubeMarker(const CubeMarker& cubeMarker);

			Node *Replicate(void) const override;

		public:

			C4API CubeMarker(const char *name, TextureFormat format, int32 size);
			C4API ~CubeMarker();

			unsigned_int32 GetCubeFlags(void) const
			{
				return (cubeFlags);
			}

			void SetCubeFlags(unsigned_int32 flags)
			{
				cubeFlags = flags;
			}

			int32 GetCubeSize(void) const
			{
				return (cubeSize);
			}

			void SetCubeSize(int32 size)
			{
				cubeSize = size;
			}

			TextureFormat GetTextureFormat(void) const
			{
				return (textureFormat);
			}

			void SetTextureFormat(TextureFormat format)
			{
				textureFormat = format;
			}

			const ResourceName& GetTextureName(void) const
			{
				return (textureName);
			}

			void SetTextureName(const char *name)
			{
				textureName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetCategoryCount(void) const override;
			Type GetCategoryType(int32 index, const char **title) const override;
			int32 GetCategorySettingCount(Type category) const override;
			Setting *GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const override;
			void SetCategorySetting(Type category, const Setting *setting) override;
	};


	//# \class	ShaderMarker		Represents a shader warmup marker node in a world.
	//
	//# The $ShaderMarker$ class represents a shader warmup marker node in a world.
	//
	//# \def	class ShaderMarker final : public Marker
	//
	//# \ctor	ShaderMarker();
	//
	//# \desc
	//# The $ShaderMarker$ class is used to specify a location in a world where rendering should be
	//# performed before gameplay starts for the purpose of pregenerating shaders. This helps avoid
	//# hesitations due to shaders being compiled when the camera enters a new area of a world.
	//# Multiple shader warmup markers can be placed in a world in order to cover all of the places
	//# where new lighting and fog environments would cause many shaders to be generated.
	//
	//# \base	Marker		A shader warmup marker is a specific type of marker.


	class ShaderMarker final : public Marker
	{
		private:

			ShaderMarker(const ShaderMarker& shaderMarker);

			Node *Replicate(void) const override;

		public:

			C4API ShaderMarker();
			C4API ~ShaderMarker();
	};
}


#endif

// ZYUQURM
