#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
    class Console
    {
    public:
        static void System::Print(const std::string& s);
        static void System::Print(Object* o);
		static void System::Print(Object& o);
        static void System::Print(const int f);
        static void System::Print(const float f);
        static void System::Print(const long i);
    };
}
