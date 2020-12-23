#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class Pixmap
	{
	public:
		int format;
		Bank* pixels;
		int BytesPerPixel();
		void WritePixel(const int x, const int y, const char* p);
		void ReadPixel(const int x, const int y, const char* p);
		};
}
