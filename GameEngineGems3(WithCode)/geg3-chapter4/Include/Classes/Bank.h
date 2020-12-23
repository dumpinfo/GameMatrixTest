#include "../Leadwerks.h"

namespace Leadwerks
{
	class Bank : public Object
	{
	public:
		int size;
		char* buf;
		
		Bank();
		virtual ~Bank();
		
		virtual std::string GetClassName();
		virtual void Clear();
		virtual int GetSize();
		virtual bool Resize(const int size);
		virtual void PokeByte(const int pos, const unsigned char c);
		virtual void PokeShort(const int pos, const unsigned short i);
		virtual void PokeInt(const int pos, const int i);
		virtual void PokeFloat(const int pos, const float f);
		virtual void PokeString(const int pos, const std::string& s);
		virtual void AppendInt(const int i);
		virtual void AppendFloat(const float f);
		virtual void AppendString(const std::string& s);
		virtual unsigned char PeekByte(const int pos);
		virtual unsigned short PeekShort(const int pos);
		virtual int PeekInt(const int pos);
		virtual float PeekFloat(const int pos);
		virtual std::string PeekString(const int pos);
		virtual Bank* Compress();
		virtual Bank* Decompress();
		virtual void Copy(const int srcoffset, Bank* dst, const int dstoffset, const int size);
		virtual Bank* Copy();
		virtual void PokeMem(const int pos, const char* buf, const int size);
		virtual void PeekMem(const int pos, const char* buf, const int size);		
		virtual void Serialize(Stream* stream);
		virtual void Deserialize(Stream* stream);
		
		static Bank* Load(const std::string& path, uint64_t fileid = 0, const std::string& key=NULL);
		static Bank* Create(const int size=0);
	};
}
