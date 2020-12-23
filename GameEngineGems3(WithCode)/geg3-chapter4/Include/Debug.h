#pragma once

#include "Leadwerks.h"

namespace Leadwerks
{
    class Debug//lua
    {
    public:
        static void Assert(const bool condition, const std::string& errortext="Assert failed.");//lua
        static void Error(const std::string& error);//lua
        static void Stop(const std::string& err="");//lua
    };
}
