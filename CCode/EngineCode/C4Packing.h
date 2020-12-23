 

#ifndef C4Packing_h
#define C4Packing_h


//# \component	Resource Manager
//# \prefix		ResourceMgr/


#include "C4Types.h"


namespace C4
{
	typedef unsigned_int32	PackHandle;


	enum
	{
		kPackageDefaultSize			= 16384
	};


	//# \enum	PackFlags

	enum
	{
		kPackEditor					= 1 << 0,		//## Packing is occurring in the World Editor.
		kPackSelected				= 1 << 1,		//## Only nodes selected in the World Editor are being packed.
		kPackSettings				= 1 << 2,		//## Nodes are being packed only to save settings so that they can later be restored. If this flag is set, it is not necessary to pack node or object indexes that would be used to link to other nodes or objects when unpacking.
		kPackInitialize				= 1 << 3		//## Controller indexes are re-initialized.
	};


	//# \enum	UnpackFlags

	enum
	{
		kUnpackEditor				= 1 << 0,		//## Unpacking is occurring in the World Editor.
		kUnpackNonpersistent		= 1 << 1,		//## All unpacked nodes will be set to nonpersistent.
		kUnpackExternal				= 1 << 2		//## Object indexes are not set because nodes are being unpacked into a different world.
	};


	class Object;
	class Node;


	//# \struct	ChunkHeader		Contains information about a data chunk.
	//
	//# The $ChunkHeader$ structure contains information about a data chunk.
	//
	//# \def	struct ChunkHeader
	//
	//# \ctor	ChunkHeader();
	//# \ctor	ChunkHeader(unsigned_int32 type, unsigned_int32 size);
	//
	//# \param	type	The type identifier for the chunk. This should not be 0.
	//# \param	size	The size of the chunk's data, in bytes. This size must be a multiple of four, and it does not include the chunk header.
	//
	//# \data	ChunkHeader
	//
	//# \also	$@Packer::BeginChunk@$
	//# \also	$@Packer::EndChunk@$


	//# \member		ChunkHeader

	struct ChunkHeader
	{
		unsigned_int32		chunkType;		//## The type of the data chunk.
		unsigned_int32		chunkSize;		//## The size of the data chunk, in bytes.

		ChunkHeader() = default;

		ChunkHeader(unsigned_int32 type, unsigned_int32 size)
		{
			Assert(((size & 3) == 0), "ChunkHeader::ChunkHeader(), chunk size must be a multiple of 4\n");

			chunkType = type;
			chunkSize = size;
		}

		ChunkHeader *GetNextChunk(void)
		{
			return (reinterpret_cast<ChunkHeader *>(reinterpret_cast<char *>(this + 1) + chunkSize));
		}

		const ChunkHeader *GetNextChunk(void) const
		{
			return (reinterpret_cast<const ChunkHeader *>(reinterpret_cast<const char *>(this + 1) + chunkSize));
		}
	};


	class ObjectLink : public ListElement<ObjectLink>, public EngineMemory<ObjectLink>
	{
		public:

			typedef void LinkProc(Object *, void *);

		private: 

			int32		objectIndex;
			LinkProc	*linkProc; 
			void		*linkCookie;
 
		public:

			ObjectLink(int32 index, LinkProc *proc, void *cookie) 
			{
				objectIndex = index; 
				linkProc = proc; 
				linkCookie = cookie;
			}

			int32 GetObjectIndex(void) const 
			{
				return (objectIndex);
			}

			void CallLinkProc(Object *object)
			{
				(*linkProc)(object, linkCookie);
			}
	};


	class NodeLink : public ListElement<NodeLink>, public EngineMemory<ObjectLink>
	{
		public:

			typedef void LinkProc(Node *, void *);

		private:

			int32		nodeIndex;
			LinkProc	*linkProc;
			void		*linkCookie;

		public:

			NodeLink(int32 index, LinkProc *proc, void *cookie)
			{
				nodeIndex = index;
				linkProc = proc;
				linkCookie = cookie;
			}

			int32 GetNodeIndex(void) const
			{
				return (nodeIndex);
			}

			void CallLinkProc(Node *node)
			{
				(*linkProc)(node, linkCookie);
			}
	};


	class Package
	{
		friend class Packer;

		private:

			unsigned_int32		logicalSize;
			unsigned_int32		physicalSize;

			char				*activeStorage;
			char				*packageStorage;

			void Resize(unsigned_int32 size);

		public:

			C4API Package(void *storage, unsigned_int32 size = 0);
			C4API ~Package();

			unsigned_int32 GetSize(void) const
			{
				return (logicalSize);
			}

			void *GetStorage(void) const
			{
				return (activeStorage);
			}

			void Reset(void)
			{
				logicalSize = 0;
			}

			C4API void *Reserve(unsigned_int32 size);
	};


	//# \class	Packer		Represents an output serialization stream.
	//
	//# \def	class Packer
	//
	//# \ctor	Packer(Package *package);
	//
	//# \param	package		A pointer to the package that receives the packed data.
	//
	//# \desc
	//# A $Packer$ object is passed to the $@Packable::Pack@$ function when the engine needs to serialize
	//# data stored in an object. The $Packer$ object encapsulates an output serialization stream, and the
	//# $<<$ operator is typically used to write data to it.
	//
	//# \operator	template <typename type> Packer& operator <<(const type& data);
	//#				Writes the data given by the $data$ parameter to the output stream. The size of the data must be a multiple of four bytes.
	//
	//# \operator	Packer& operator <<(const bool& data);
	//#				Writes the boolean value given by the $data$ parameter to the output stream.
	//#				(This operation always writes four bytes to the output stream.)
	//
	//# \operator	template <int32 len> Packer& operator <<(const String<len>& string);
	//#				Writes the string given by the $string$ parameter to the output stream.
	//
	//# \desc
	//# In order to maintain four-byte alignment, $char$ and $int16$ types should not be serialized directly
	//# with the $<<$ operator. Instead, such values should first be copied or cast to a 32-bit type.
	//#
	//# The $bool$ type can safely be serialized directly because it is given special handling by the $<<$
	//# operator that forces it to always occupy four bytes.
	//#
	//# The $machine$ type should never be serialized directly because it can be a different size on different
	//# platforms, and this difference can appear between 32-bit and 64-bit versions of the same operating system.
	//
	//# \also	$@Unpacker@$
	//# \also	$@Packable@$


	//# \function	Packer::WriteData		Writes arbitrary data to the output stream.
	//
	//# \proto	void WriteData(const void *data, unsigned_int32 size);
	//
	//# \param	data	A pointer to the data to be written.
	//#\ param	size	The number of bytes to write.
	//
	//# \desc
	//# The $WriteData$ function writes $size$ bytes of data to the output stream, reading them from memory
	//# starting at the pointer specified by the $data$ parameter.
	//#
	//# The value of the $size$ parameter must be a multiple of four bytes.
	//
	//# \also	$@Packer::WriteArray@$
	//# \also	$@Unpacker::ReadData@$
	//# \also	$@Unpacker::ReadArray@$


	//# \function	Packer::WriteArray		Writes an array of objects to the output stream.
	//
	//# \proto	template <typename type> void WriteArray(int32 count, const type *array);
	//
	//# \tparam	type	The type of the objects in the array.
	//
	//# \param	count	The number of objects to write.
	//#\ param	array	A pointer to the array of objects to be written.
	//
	//# \desc
	//# The $WriteArray$ function writes $count$ objects of the type given by the $type$ template parameter
	//# to the output stream, reading them from memory starting at the pointer specified by the $array$ parameter.
	//#
	//# The quantity $count * sizeof(type)$ must be a multiple of four bytes.
	//
	//# \also	$@Packer::WriteData@$
	//# \also	$@Unpacker::ReadArray@$
	//# \also	$@Unpacker::ReadData@$


	//# \function	Packer::BeginSection	Begins measuring the size of a section of the output stream.
	//
	//# \proto	PackHandle BeginSection(void);
	//
	//# \desc
	//# The $BeginSection$ function reserves four bytes in the output stream to which a size is later written,
	//# and it returns a handle that should be passed to the $@Packer::EndSection@$ function. When the $EndSection$
	//# function is called, the number of bytes written to the output stream since the matching call to $BeginSection$
	//# is written into the four-byte location that was initially reserved. When unpacking, this four-byte value must
	//# be read before any of the data packed between the calls to $BeginSection$ and $EndSection$ is read.
	//
	//# \important
	//# In order to maintain four-byte alignment in the output stream, the number of bytes written between calls to
	//# the $BeginSection$ and $EndSection$ function must be a multiple of four.
	//
	//# \also	$@Packer::EndSection@$


	//# \function	Packer::EndSection		Ends measuring the size of a section of the output stream.
	//
	//# \proto	void EndSection(PackHandle handle);
	//
	//# \param	handle		A handle that was returned by a previous call to the $@Packer::BeginSection@$ function.
	//
	//# \desc
	//# The $EndSection$ function causes the size of the data packed since the corresponding call to the $@Packer::BeginSection@$
	//# function (the one that returned the $handle$ parameter) to be written at the location reserved when the $BeginSection$
	//# function was called.
	//
	//# \important
	//# In order to maintain four-byte alignment in the output stream, the number of bytes written between calls to
	//# the $BeginSection$ and $EndSection$ function must be a multiple of four.
	//
	//# \also	$@Packer::EndSection@$


	//# \function	Packer::BeginChunk		Writes a chunk type and begins measuring the size of a chunk in the output stream.
	//
	//# \proto	PackHandle BeginChunk(unsigned_int32 type);
	//
	//# \param	type	The chunk type.
	//
	//# \desc
	//# The $BeginChunk$ function writes the four-byte chunk type specified by the $type$ parameter to the output stream and
	//# then reserves four bytes in the output stream to which the size of the chunk is later written. This function returns
	//# a handle that should be passed to the $@Packer::EndChunk@$ function. When the $EndChunk$ function is called, the number
	//# of bytes written to the output stream since the matching call to $BeginChunk$ is written into the four-byte location
	//# immediately following the chunk type.
	//
	//# \important
	//# In order to maintain four-byte alignment in the output stream, the number of bytes written between calls to
	//# the $BeginChunk$ and $EndChunk$ function must be a multiple of four.
	//
	//# \also	$@Packer::EndChunk@$
	//# \also	$@ChunkHeader@$


	//# \function	Packer::EndChunk		Ends measuring the size of a chunk in the output stream.
	//
	//# \proto	void EndChunk(PackHandle handle);
	//
	//# \param	handle		A handle that was returned by a previous call to the $@Packer::BeginChunk@$ function.
	//
	//# \desc
	//# The $EndChunk$ function causes the size of the data packed since the corresponding call to the $@Packer::BeginChunk@$
	//# function (the one that returned the $handle$ parameter) to be written at the location reserved after the chunk type
	//# when the $BeginChunk$ function was called.
	//
	//# \important
	//# In order to maintain four-byte alignment in the output stream, the number of bytes written between calls to
	//# the $BeginChunk$ and $EndChunk$ function must be a multiple of four.
	//
	//# \also	$@Packer::BeginChunk@$
	//# \also	$@ChunkHeader@$


	class Packer
	{
		private:

			Package		*outputPackage;

		public:

			Packer(Package *package)
			{
				outputPackage = package;
			}

			void WriteData(const void *data, unsigned_int32 size)
			{
				void *ptr = outputPackage->Reserve(size);
				MemoryMgr::CopyMemory(data, ptr, size);
			}

			Packer& operator <<(const bool& data)
			{
				unsigned_int32 *ptr = static_cast<unsigned_int32 *>(outputPackage->Reserve(4));
				ptr[0] = data;
				return (*this);
			}

			Packer& operator <<(const Matrix4D& data)
			{
				Vector4D *ptr = static_cast<Vector4D *>(outputPackage->Reserve(sizeof(Matrix4D)));
				ptr[0] = data[0];
				ptr[1] = data[1];
				ptr[2] = data[2];
				ptr[3] = data[3];
				return (*this);
			}

			Packer& operator <<(const Transform4D& data)
			{
				Vector4D *ptr = static_cast<Vector4D *>(outputPackage->Reserve(sizeof(Matrix4D)));
				ptr[0] = data.Matrix4D::operator [](0);
				ptr[1] = data.Matrix4D::operator [](1);
				ptr[2] = data.Matrix4D::operator [](2);
				ptr[3] = data.Matrix4D::operator [](3);
				return (*this);
			}

			template <typename type> Packer& operator <<(const type& data)
			{
				static_assert((sizeof(type) & 3) == 0, "The size of packed data must be a multiple of 4 bytes");

				type *ptr = static_cast<type *>(outputPackage->Reserve(sizeof(type)));
				ptr[0] = data;
				return (*this);
			}

			template <typename type> void WriteArray(int32 count, const type *array)
			{
				unsigned_int32 size = count * sizeof(type);
				void *ptr = outputPackage->Reserve(size);
				MemoryMgr::CopyMemory(array, ptr, size);
			}

			template <int32 len> Packer& operator <<(const String<len>& string)
			{
				return (operator <<(string.operator const char *()));
			}

			C4API Packer& operator <<(const char *string);

			C4API PackHandle BeginSection(void);
			C4API void EndSection(PackHandle handle);

			C4API PackHandle BeginChunk(unsigned_int32 type);
			C4API void EndChunk(PackHandle handle);
	};


	//# \class	Unpacker	Represents an input deserialization stream.
	//
	//# \def	class Unpacker
	//
	//# \ctor	Unpacker(const void *ptr, int32 ver = 0);
	//
	//# \param	ptr		A pointer to a memory block containing the packed data.
	//# \param	ver		The storage format version of the packed data. The constant $kEngineInternalVersion$ corresponds to the current version.
	//
	//# \desc
	//# A $Unpacker$ object is passed to the $@Packable::Unpack@$ function when the engine needs to deserialize
	//# data to be stored in an object. The $Unpacker$ object encapsulates an input serialization stream, and the
	//# $>>$ operator is typically used to read data from it.
	//
	//# \operator	template <typename type> Unpacker& operator >>(type& data);
	//#				Reads the data given by the $data$ parameter from the input stream.
	//
	//# \operator	Unpacker& operator >>(bool& data);
	//#				Reads the boolean value given by the $data$ parameter from the input stream.
	//
	//# \operator	template <int32 len> Unpacker& operator >>(String<len>& string);
	//#				Reads the string given by the $string$ parameter from the input stream.
	//
	//# \also	$@Packer@$
	//# \also	$@Packable@$


	//# \function	Unpacker::GetType	Returns the generic type appearing next in the input stream.
	//
	//# \proto	Type GetType(void) const;
	//
	//# \desc
	//# The $GetType$ function returns the generic type appearing next in the input stream encapsulated by an
	//# $Unpacker$ object. This function does not advance the pointer into the stream. If it is necessary to
	//# read additional types, the preincrement operator ($++$) should be used on the $Unpacker$ object to
	//# explicitly advance the pointer to the next type.


	//# \function	Packer::ReadData	Reads arbitrary data from the input stream.
	//
	//# \proto	void ReadData(void *data, unsigned_int32 size);
	//
	//# \param	data	A pointer to the storage buffer for the data.
	//#\ param	size	The number of bytes to read.
	//
	//# \desc
	//# The $ReadData$ function reads $size$ bytes of data from the input stream, writing them to memory
	//# starting at the pointer specified by the $data$ parameter.
	//#
	//# The value of the $size$ parameter must be a multiple of four bytes.
	//
	//# \also	$@Unacker::ReadArray@$
	//# \also	$@Packer::WriteData@$
	//# \also	$@Packer::WriteArray@$


	//# \function	Unpacker::ReadArray		Reads an array of objects from the input stream.
	//
	//# \proto	template <typename type> void ReadArray(int32 count, type *array);
	//
	//# \tparam	type	The type of the objects in the array.
	//
	//# \param	count	The number of objects to read.
	//#\ param	array	A pointer to the storage for the array of objects.
	//
	//# \desc
	//# The $ReadArray$ function reads $count$ objects of the type given by the $type$ template parameter
	//# from the input stream, writing them to memory starting at the pointer specified by the $array$ parameter.
	//#
	//# The quantity $count * sizeof(type)$ must be a multiple of four bytes.
	//
	//# \also	$@Unpacker::ReadData@$
	//# \also	$@Packer::WriteArray@$
	//# \also	$@Packer::WriteData@$


	//# \function	Unpacker::AddObjectLink		Adds an object link that will be resolved after unpacking is complete.
	//
	//# \proto	void AddObjectLink(int32 index, ObjectLink::LinkProc *proc, void *cookie);
	//
	//# \param	index	The index of the object to be linked.
	//# \param	proc	A pointer to the callback function.
	//# \param	cookie	A user-defined pointer that is passed to the callback function.
	//
	//# \desc
	//# The $AddObjectLink$ function registers a callback function that is called after unpacking is complete in order
	//# to resolve an object index to the actual pointer to an object. The $AddObjectLink$ function should be called
	//# from within an overridden $@Packable::Unpack@$ function or the callback passed to the $@Packable::UnpackChunkList@$
	//# function. The $index$ parameter should be set to the object index, which would typically have just been unpacked.
	//#
	//# The $ObjectLink::LinkProc$ type is defined as follows.
	//
	//# \code	typedef void LinkProc(Object *, void *);
	//
	//# The $proc$ parameter should be set to a pointer to a static function having this prototype, and the $cookie$ parameter
	//# should be any pointer that the calling function needs. The value of the $cookie$ parameter is passed to the callback
	//# function as its second parameter. The object pointer received in the first parameter of the callback function will
	//# be the object whose index was previously specified by the $index$ parameter when the $AddObjectLink$ function was called.
	//# Note that the object pointer can be $nullptr$ if the object could not be constructed during the unpacking process.
	//
	//# \also	$@Unpacker::AddNodeLink@$


	//# \function	Unpacker::AddNodeLink		Adds a node link that will be resolved after unpacking is complete.
	//
	//# \proto	void AddNodeLink(int32 index, NodeLink::LinkProc *proc, void *cookie);
	//
	//# \param	index	The index of the node to be linked.
	//# \param	proc	A pointer to the callback function.
	//# \param	cookie	A user-defined pointer that is passed to the callback function.
	//
	//# \desc
	//# The $AddNodeLink$ function registers a callback function that is called after unpacking is complete in order
	//# to resolve a node index to the actual pointer to a node. The $AddNodeLink$ function should be called
	//# from within an overridden $@Packable::Unpack@$ function or the callback passed to the $@Packable::UnpackChunkList@$
	//# function. The $index$ parameter should be set to the node index, which would typically have just been unpacked.
	//#
	//# The $NodeLink::LinkProc$ type is defined as follows.
	//
	//# \code	typedef void LinkProc(Node *, void *);
	//
	//# The $proc$ parameter should be set to a pointer to a static function having this prototype, and the $cookie$ parameter
	//# should be any pointer that the calling function needs. The value of the $cookie$ parameter is passed to the callback
	//# function as its second parameter. The node pointer received in the first parameter of the callback function will
	//# be the node whose index was previously specified by the $index$ parameter when the $AddNodeLink$ function was called.
	//# Note that the node pointer can be $nullptr$ if the node could not be constructed during the unpacking process.
	//
	//# \also	$@Unpacker::AddObjectLink@$


	class Unpacker
	{
		public:

			const void			*pointer;
			int32				version;

			List<ObjectLink>	objectList;
			List<NodeLink>		nodeList;

		public:

			C4API Unpacker(const void *ptr, int32 ver = 0);
			C4API ~Unpacker();

			Unpacker& operator +=(unsigned_int32 size)
			{
				pointer = static_cast<const char *>(pointer) + size;
				return (*this);
			}

			Unpacker& operator ++(void)
			{
				pointer = static_cast<const Type *>(pointer) + 1;
				return (*this);
			}

			const void *GetPointer(void) const
			{
				return (pointer);
			}

			int32 GetVersion(void) const
			{
				return (version);
			}

			void Skip(const void *ptr, unsigned_int32 size)
			{
				pointer = static_cast<const char *>(ptr) + size;
			}

			void ReadData(void *data, unsigned_int32 size)
			{
				MemoryMgr::CopyMemory(pointer, data, size);
				pointer = static_cast<const char *>(pointer) + size;
			}

			Unpacker& operator >>(bool& data)
			{
				const unsigned_int32 *ptr = static_cast<const unsigned_int32 *>(pointer);
				pointer = ptr + 1;
				data = (*ptr != 0);
				return (*this);
			}

			Unpacker& operator >>(Matrix4D& data)
			{
				const Vector4D *ptr = static_cast<const Vector4D *>(pointer);
				pointer = ptr + 4;
				data[0] = ptr[0];
				data[1] = ptr[1];
				data[2] = ptr[2];
				data[3] = ptr[3];
				return (*this);
			}

			Unpacker& operator >>(Transform4D& data)
			{
				const Vector4D *ptr = static_cast<const Vector4D *>(pointer);
				pointer = ptr + 4;
				data.Matrix4D::operator [](0) = ptr[0];
				data.Matrix4D::operator [](1) = ptr[1];
				data.Matrix4D::operator [](2) = ptr[2];
				data.Matrix4D::operator [](3) = ptr[3];
				return (*this);
			}

			template <typename type> Unpacker& operator >>(type& data)
			{
				const type *ptr = static_cast<const type *>(pointer);
				pointer = ptr + 1;
				data = *ptr;
				return (*this);
			}

			Type GetType(void) const
			{
				return (*static_cast<const Type *>(pointer));
			}

			unsigned_int32 GetUnknownSize(void) const
			{
				return (static_cast<const unsigned_int32 *>(pointer)[-2]);
			}

			template <typename type> void ReadArray(int32 count, type *array)
			{
				unsigned_int32 size = count * sizeof(type);
				MemoryMgr::CopyMemory(pointer, array, size);
				pointer = static_cast<const char *>(pointer) + size;
			}

			ObjectLink *GetFirstObjectLink(void) const
			{
				return (objectList.First());
			}

			void AddObjectLink(int32 index, ObjectLink::LinkProc *proc, void *cookie)
			{
				objectList.Append(new ObjectLink(index, proc, cookie));
			}

			NodeLink *GetFirstNodeLink(void) const
			{
				return (nodeList.First());
			}

			void AddNodeLink(int32 index, NodeLink::LinkProc *proc, void *cookie)
			{
				nodeList.Append(new NodeLink(index, proc, cookie));
			}

			C4API const char *ReadString(void);
			template <int32 len> Unpacker& operator >>(String<len>& string);
	};


	template <int32 len> Unpacker& Unpacker::operator >>(String<len>& string)
	{
		const unsigned_int32 *ptr = static_cast<const unsigned_int32 *>(pointer);
		unsigned_int32 physicalLength = *ptr;

		const char *data = reinterpret_cast<const char *>(ptr + 1);
		string = data;

		pointer = data + physicalLength;
		return (*this);
	}


	//# \class	 Packable		The base class for a packable object.
	//
	//# The $Packable$ class is the base class for a packable object.
	//
	//# \def	class Packable
	//
	//# \ctor	Packable();
	//
	//# \desc
	//# The $Packable$ class is a base class used by all packable objects. It contains virtual functions that are
	//# used for packing and unpacking object data to and from storage (typically disk files).


	//# \function	Packable::PackType		Packs the sequence of type codes.
	//
	//# \proto	virtual void PackType(Packer& data) const;
	//
	//# \param	data	The $@Packer@$ object into which the type codes are written.
	//
	//# \desc
	//# The $PackType$ function is called when the engine needs to pack the type of an object. The implementation
	//# of an override should first call the direct base class's $PackType$ function and then use the $<<$ operator
	//# for the $@Packer@$ object to pack its own type identifier.
	//
	//# \also	$@Packer@$


	//# \function	Packable::Prepack	Called before packing to allow object linking.
	//
	//# \proto	virtual void Prepack(List<Object> *linkList) const;
	//
	//# \param	linkList		A list to which linked objects should be added.
	//
	//# \desc
	//# The $Prepack$ function is called before a packable object is packed. If the packable object owns references
	//# to $@WorldMgr/Object@$ instances, then it should use the $@Utilities/List::Append@$ function to add those
	//# objects to the list specified by the $linkList$ parameter. This is necessary in order for the object
	//# to be included in the packed data so that it can be restored when the data is unpacked.
	//
	//# \also	$@Packable::Pack@$
	//# \also	$@Packable::Unpack@$


	//# \function	Packable::Pack		Packs an object's data.
	//
	//# \proto	virtual void Pack(Packer& data, unsigned_int32 packFlags) const;
	//
	//# \param	data		The $@Packer@$ object into which the object data is written.
	//# \param	packFlags	The packing flags.
	//
	//# \desc
	//# The $Pack$ function is called when the engine needs to pack an object's data. The implementation of an
	//# override should first call the direct base class's $Pack$ function and then use the functions of the
	//# $@Packer@$ object to pack its own data.
	//#
	//# Often, chunks are used when packing data so that the format of the data can be changed in different versions
	//# of the code without breaking backwards compatibility. A chunk is written to the output stream by first
	//# writing a $@ChunkHeader@$ structure containing the chunk type and chunk size, and then writing the data
	//# belonging to the chunk. Each chunk must have a unique type within the set of chunks written by the
	//# particular implementation of the $Pack$ function, and the size of each chunk must match the number of
	//# bytes of data written for the chunk.
	//#
	//# After writing all of the chunks for an object, a special terminator chunk must be written to the output
	//# stream using the following code.
	//
	//# \source
	//# data << TerminatorChunk;
	//
	//# \desc
	//# In the $@Packable::Unpack@$ function, chunks are read by calling the $@Packable::UnpackChunkList@$ function.
	//
	//# \also	$@Packable::Unpack@$
	//# \also	$@Packable::UnpackChunkList@$
	//# \also	$@Packer@$
	//# \also	$@ChunkHeader@$


	//# \function	Packable::Unpack	Unpacks an object's data.
	//
	//# \proto	virtual void Unpack(Unpacker& data, unsigned_int32 unpackFlags);
	//
	//# \param	data			The $@Unpacker@$ object from which the object data is read.
	//# \param	unpackFlags		The unpacking flags.
	//
	//# \desc
	//# The $Unpack$ function is called when the engine needs to unpack an object's data. The implementation
	//# of an override should first call the direct base class's $Unpack$ function and then use the functions
	//# of the $@Unpacker@$ object to unpack its own data.
	//#
	//# If chunks were used to write the object data in the override of the $@Packable::Pack@$ function, then
	//# the $Unpack$ function should call the $@Packable::UnpackChunkList@$ function to unpack its data.
	//
	//# \also	$@Packable::Pack@$
	//# \also	$@Packable::UnpackChunkList@$
	//# \also	$@Unpacker@$


	//# \function	Packable::UnpackChunkList	Unpacks a sequence of chunks from an input stream.
	//
	//# \proto	template <class type> void UnpackChunkList(Unpacker& data, unsigned_int32 unpackFlags);
	//
	//# \param	data			The $@Unpacker@$ object from which the object data is read.
	//# \param	unpackFlags		The unpacking flags.
	//
	//# \desc
	//# The $UnpackChunkList$ function is used to unpack a sequence of chunks from an input stream that was previously
	//# packed using $@ChunkHeader@$ structures. This function should be called from the $@Packable::Unpack@$ function,
	//# and it iterates through the chunks in the input stream until a terminator chunk is found. For each chunk,
	//# the $UnpackChunk$ member function of the class specified by the $type$ template parameter is called.
	//# The $UnpackChunk$ function must have the following declaration.
	//
	//# \source
	//# bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
	//
	//# \desc
	//# The $chunkHeader$ parameter passed to this function should be examined, and the data corresponding to its
	//# type should be read from the $@Unpacker@$ object passed as the $data$ parameter. The $unpackFlags$ parameter
	//# passed to the $UnpackChunk$ function is the same value of the $unpackFlags$ parameter that was passed to the
	//# $UnpackChunkList$ function. The $UnpackChunk$ function should return $true$ if data was unpacked from the chunk,
	//# and it should return $false$ if the chunk type is not supported (which ordinarily means that it's obsolete).
	//# The $UnpackChunkList$ function simply skips the data for chunks that are identified as unsupported by the $UnpackChunk$ function.
	//
	//# \also	$@Packable::Unpack@$
	//# \also	$@Unpacker@$
	//# \also	$@ChunkHeader@$


	//# \function	Packable::BeginSettingsUnpack	Notifies an object that its settings are about to be unpacked as part of a restoration operation.
	//
	//# \proto	virtual void *BeginSettingsUnpack(void);
	//
	//# \desc
	//# The $BeginSettingsUnpack$ function is called right before an object is about to have its settings unpacked as part
	//# of a restoration operation such as an undo command. This function is <i>not</i> called before an object is unpacked as it
	//# is being loaded from a resource file. An implementation of the $BeginSettingsUnpack$ function should restore an object
	//# to the state it would be in before any extra memory allocations or resource acquisitions are made inside the
	//# $@Packable::Unpack@$ function. That is, it should release any previously allocated memory or resources that will
	//# be allocated again when the $@Packable::Unpack@$ function is called. Furthermore, if any settings have a default state
	//# that would cause them not to be stored inside the $@Packable::Pack@$ function, then they should be restored to this
	//# default state because it's possible that those settings will not be restored inside the $Unpack$ function.
	//#
	//# After the object is unpacked, its $@Packable::EndSettingsUnpack@$ function will be called. It is not a requirement that
	//# the $EndSettingsUnpack$ function be implemented whenever the $BeginSettingsUnpack$ function is implemented. If the
	//# object needs to retain information between calls to the $BeginSettingsUnpack$ and $EndSettingsUnpack$ functions, then
	//# is can allocate some structure in memory and return a pointer to it from the $BeginSettingsUnpack$ function. This pointer
	//# will then be passed to the $cookie$ parameter of the $EndSettingsUnpack$ function.
	//
	//# \also	$@Packable::EndSettingsUnpack@$
	//# \also	$@Packable::Pack@$
	//# \also	$@Packable::Unpack@$


	//# \function	Packable::EndSettingsUnpack		Notifies an object that its settings have finished being unpacked as part of a restoration operation.
	//
	//# \proto	virtual void EndSettingsUnpack(void *cookie);
	//
	//# \param	cookie		The pointer returned by the $@Packable::BeginSettingsUnpack@$ function.
	//
	//# \desc
	//# The $EndSettingsUnpack$ function is called right after an object has had its settings unpacked as part of a restoration
	//# operation such as an undo command. This function is <i>not</i> called after an object is unpacked as it is being loaded
	//# from a resource file. An implementation of the $EndSettingsUnpack$ function can do whatever is necessary to clean up
	//# after settings have been restored. The $cookie$ parameter contains the pointer that was previously returned by the
	//# implementation of the $@Packable::BeginSettingsUnpack@$ function. It is not a requirement that the $EndSettingsUnpack$
	//# function be implemented whenever the $BeginSettingsUnpack$ function is implemented.
	//
	//# \also	$@Packable::BeginSettingsUnpack@$
	//# \also	$@Packable::Pack@$
	//# \also	$@Packable::Unpack@$


	class Packable
	{
		protected:

			template <class type> void UnpackChunkList(Unpacker& data, unsigned_int32 unpackFlags);

		public:

			C4API virtual void PackType(Packer& data) const;
			C4API virtual void Prepack(List<Object> *linkList) const;
			C4API virtual void Pack(Packer& data, unsigned_int32 packFlags) const;
			C4API virtual void Unpack(Unpacker& data, unsigned_int32 unpackFlags);
			C4API virtual void *BeginSettingsUnpack(void);
			C4API virtual void EndSettingsUnpack(void *cookie);
	};


	template <class type> inline void Packable::UnpackChunkList(Unpacker& data, unsigned_int32 unpackFlags)
	{
		type *packable = static_cast<type *>(this);
		for (;;)
		{
			ChunkHeader		chunkHeader;

			data >> chunkHeader;
			if (chunkHeader.chunkType == 0)
			{
				break;
			}

			const void *mark = data.GetPointer();
			if (!packable->type::UnpackChunk(&chunkHeader, data, unpackFlags))
			{
				data.Skip(mark, chunkHeader.chunkSize);
			}
		}
	}


	C4API extern ChunkHeader TerminatorChunk;
}


#endif

// ZYUQURM
