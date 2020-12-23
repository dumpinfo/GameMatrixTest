 

#ifndef C4Values_h
#define C4Values_h


//# \component	Controller System
//# \prefix		Controller/


#include "C4Configuration.h"


namespace C4
{
	enum
	{
		kMaxStringValueLength	= 255
	};


	typedef Type	ValueType;
	typedef Type	ValueScope;


	enum : ValueType
	{
		kValueBoolean			= 'BOOL',
		kValueInteger			= 'INTG',
		kValueFloat				= 'FLOT',
		kValueString			= 'STRG',
		kValueColor				= 'COLR',
		kValueVector			= 'VECT',
		kValuePoint				= 'PONT'
	};


	enum : ValueScope
	{
		kValueScopeScript		= 'SCPT',
		kValueScopeController	= 'CTRL',
		kValueScopeObject		= 'OBJC'
	};


	class ScriptState;


	//# \class	Value		Encapsulates a script variable.
	//
	//# The $Value$ class encapsulates a script variable.
	//
	//# \def	class Value : public MapElement<Value>, public Packable, public Configurable, public Memory<ScriptState>
	//
	//# \ctor	Value(ValueType type);
	//
	//# \param	type	The type of the value.
	//
	//# \desc
	//# The $Value$ class encapsulates a variable belonging to a script.
	//
	//# \base	Utilities/MapElement<Value>			Used internally by a script.
	//# \base	ResourceMgr/Packable				Values can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable			Values can be configured by the user in the Script Editor.
	//# \base	MemoryMgr/Memory<ScriptState>		Values are stored in a dedicated heap.
	//
	//# \also	$@ScriptState::GetValue@$


	class Value : public MapElement<Value>, public Packable, public Configurable, public EngineMemory<ScriptState>
	{
		private:

			ValueType		valueType;
			ValueScope		valueScope;
			ValueName		valueName;

			virtual Value *Replicate(void) const = 0;

		protected:

			Value(ValueType type);
			Value(const Value& value);

		public:

			typedef ConstCharKey KeyType;

			~Value();

			KeyType GetKey(void) const
			{
				return (valueName);
			}

			ValueType GetValueType(void) const
			{
				return (valueType);
			}

			ValueScope GetValueScope(void) const
			{
				return (valueScope); 
			}

			void SetValueScope(ValueScope scope) 
			{
				valueScope = scope; 
			}

			const char *GetValueName(void) const 
			{
				return (valueName); 
			} 

			void SetValueName(const char *name)
			{
				valueName = name; 
			}

			Value *Clone(void) const
			{
				return (Replicate());
			}

			static Value *Create(Unpacker& data);
			C4API static Value *New(ValueType type);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			virtual bool GetBooleanValue(void) const = 0;

			virtual bool SetValue(bool v);
			virtual bool SetValue(int32 v);
			virtual bool SetValue(float v);
			virtual bool SetValue(const char *v);
			virtual bool SetValue(const ColorRGBA& v);
			virtual bool SetValue(const Vector3D& v);

			bool SetValue(const Value *v);
	};


	class BooleanValue final : public Value
	{
		private:

			bool		value;

			Value *Replicate(void) const override;

		public:

			C4API explicit BooleanValue(bool v = false);
			BooleanValue(const BooleanValue& booleanValue);
			C4API ~BooleanValue();

			bool GetValue(void) const
			{
				return (value);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool GetBooleanValue(void) const override;

			bool SetValue(bool v) override;
			bool SetValue(int32 v) override;
			bool SetValue(float v) override;
			bool SetValue(const char *v) override;
	};


	class IntegerValue final : public Value
	{
		private:

			int32		value;

			Value *Replicate(void) const override;

		public:

			C4API explicit IntegerValue(int32 v = 0);
			IntegerValue(const IntegerValue& integerValue);
			C4API ~IntegerValue();

			int32 GetValue(void) const
			{
				return (value);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool GetBooleanValue(void) const override;

			bool SetValue(bool v) override;
			bool SetValue(int32 v) override;
			bool SetValue(float v) override;
			bool SetValue(const char *v) override;
	};


	class FloatValue final : public Value
	{
		private:

			float		value;

			Value *Replicate(void) const override;

		public:

			C4API explicit FloatValue(float v = 0.0F);
			FloatValue(const FloatValue& floatValue);
			C4API ~FloatValue();

			float GetValue(void) const
			{
				return (value);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool GetBooleanValue(void) const override;

			bool SetValue(bool v) override;
			bool SetValue(int32 v) override;
			bool SetValue(float v) override;
			bool SetValue(const char *v) override;
	};


	class StringValue final : public Value
	{
		private:

			String<kMaxStringValueLength>	value;

			Value *Replicate(void) const override;

		public:

			C4API StringValue();
			explicit StringValue(const char *v);
			StringValue(const StringValue& stringValue);
			C4API ~StringValue();

			const String<kMaxStringValueLength>& GetValue(void) const
			{
				return (value);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool GetBooleanValue(void) const override;

			bool SetValue(bool v) override;
			bool SetValue(int32 v) override;
			bool SetValue(float v) override;
			bool SetValue(const char *v) override;
	};


	class ColorValue final : public Value
	{
		private:

			ColorRGBA		value;

			Value *Replicate(void) const override;

		public:

			C4API ColorValue();
			explicit ColorValue(const ColorRGBA& v);
			ColorValue(const ColorValue& colorValue);
			C4API ~ColorValue();

			const ColorRGBA& GetValue(void) const
			{
				return (value);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool GetBooleanValue(void) const override;

			bool SetValue(const ColorRGBA& v) override;
	};


	class VectorValue : public Value
	{
		private:

			Vector3D		value;

			Value *Replicate(void) const override;

		protected:

			VectorValue(ValueType type);
			VectorValue(ValueType type, const Vector3D& v);

		public:

			C4API VectorValue();
			explicit VectorValue(const Vector3D& v);
			VectorValue(const VectorValue& vectorValue);
			C4API ~VectorValue();

			const Vector3D& GetValue(void) const
			{
				return (value);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool GetBooleanValue(void) const override;

			bool SetValue(const Vector3D& v) override;
	};


	class PointValue final : public VectorValue
	{
		private:

			Value *Replicate(void) const override;

		public:

			C4API PointValue();
			explicit PointValue(const Point3D& v);
			PointValue(const PointValue& pointValue);
			C4API ~PointValue();

			const Point3D& GetValue(void) const
			{
				return (static_cast<const Point3D&>(VectorValue::GetValue()));
			}
	};
}


#endif

// ZYUQURM
