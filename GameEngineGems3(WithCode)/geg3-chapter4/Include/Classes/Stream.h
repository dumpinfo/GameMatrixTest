#pragma once
#pragma warning(disable:4290)
#pragma warning(disable:4996)

#include "../Leadwerks.h"

#undef EOF

namespace Leadwerks
{
	
	#define STREAM_READ 1
	#define STREAM_WRITE 2
	
	#define FILETYPE_FILE 1
	#define FILETYPE_DIR 2
	
	class Stream : public Object//lua
	{
		bool eofstate;
		public:
		FILE* file;
		int pos;
		
		int accessmode;
		long int size;
		
		Stream();
		virtual ~Stream();
		
		virtual long GetPos();//lua
		virtual void Seek(const int pos);//lua
		virtual long GetSize();//lua
		virtual bool EOF();//lua
		
		//static virtual Stream* ReadFile(const std::string& path);
		
		virtual unsigned char ReadUChar();//lua
		virtual int ReadInt();//lua
		virtual short ReadShort();//lua
		virtual unsigned short ReadUShort();//lua
		virtual unsigned int ReadUInt();//lua
		virtual uint64_t ReadULong();//lua
		virtual int64_t ReadLong();//lua
		virtual float ReadFloat();//lua
		virtual double ReadDouble();//lua
		virtual std::string ReadString(const int maxlength=0);//lua
		virtual std::string ReadLine();//lua
		virtual void Read(const void* b, const int size);//lua
		virtual void Write(const void* b, const int size);//lua
		virtual void WriteUChar(const unsigned char b);//lua
		virtual void WriteInt(const int n);//lua
		virtual void WriteFloat(const float f);//lua
		virtual void WriteDouble(const double f);//lua
		virtual void WriteString(const std::string& s);//lua
		virtual void WriteLine(const std::string& s);//lua
		virtual void WriteUShort(const unsigned short n);//lua
	};
	
}
