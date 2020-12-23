#pragma once

#include "Leadwerks.h"

namespace Leadwerks
{
    class String//lua
    {
    public:
		static int Find(const std::string& s, const std::string& token);//lua
        static char Asc(const std::string& s);//lua
        static std::string Chr(const unsigned char c);//lua
        static std::string Right(const std::string& s, const int length);//lua
        static std::string Left(const std::string& s, const int length);//lua
        static std::string Mid(const std::string& s, const int pos, const int length);//lua
        static int Length(const std::string& s);//lua
        static std::string Replace(const std::string& s, const std::string& searchstring, const std::string& replacestring);//lua
        static std::string Lower(const std::string& s);//lua
        static std::string Upper(const std::string& s);//lua
		static std::string XOR(const std::string& s, const std::string& key);//lua

		static std::string LTrim(std::string &ss);//lua
		static std::string RTrim(std::string &ss);//lua
		static std::string Trim(std::string &ss);//lua
		static uint64_t UInt64(const std::string& s);
        //static std::string Trim(const std::string& s);
        static int Int(const std::string& s);//lua
        static float Float(const std::string& s);//lua
        static vector<std::string> Split(const std::string& s, const std::string& sep);
		//static std::vector<std::string> String::Split(const std::string &s, char delim);
    };
	
	std::string UInt64ToString( uint64_t i );
}
