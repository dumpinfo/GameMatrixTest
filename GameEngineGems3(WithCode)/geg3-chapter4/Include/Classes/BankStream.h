#pragma once
#ifdef _WIN32
	#pragma warning(disable:4290)
	#pragma warning(disable:4996)
#endif

#include "../Leadwerks.h"

#undef EOF

namespace Leadwerks
{
	class Stream;

	class BankStream : public Stream
	{
	public:
		int pos;
		Bank* data;
		bool takeownership;

		BankStream();
		virtual ~BankStream();

		virtual bool EOF();
		virtual long GetPos();
		virtual void Seek(const int pos);
		virtual long GetSize();

		virtual void Read(const void* b, const int size);
		virtual void Write(const void* b, const int size);

		static BankStream* Create(Bank* bank, const bool takeownership = true);
	};
}
