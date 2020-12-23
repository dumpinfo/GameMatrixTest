 

#ifndef C4String_h
#define C4String_h


//# \component	Utility Library
//# \prefix		Utilities/


#include "C4Memory.h"


namespace C4
{
	//# \namespace	Text	Contains miscellaneous text functions.
	//
	//# The $Text$ namespace contains miscellaneous text functions.
	//
	//# \def	namespace C4 { namespace Text {...} }
	//
	//# \also	$@String@$


	//# \function	Text::GetTextLength		Returns the length of a character string.
	//
	//# \proto	int32 GetTextLength(const char *text);
	//
	//# \param	text	A pointer to a null-terminated character string.
	//
	//# \desc
	//# The $GetTextLength$ function returns the number of bytes occupied by the string specified by the
	//# $text$ parameter.
	//#
	//# The string must be null-terminated by a zero byte. The null terminator is not included in the length.
	//
	//# \also	$@Text::GetGlyphCountUTF8@$


	//# \function	Text::GetGlyphCountUTF8		Returns the number of UTF-8 glyphs in a character string.
	//
	//# \proto	int32 GetGlyphCountUTF8(const char *text);
	//
	//# \param	text	A pointer to a null-terminated character string.
	//
	//# \desc
	//# The $GetGlyphCountUTF8$ function returns the number of individual glyphs encoded as UTF-8 in the string
	//# specified by the $text$ parameter. This number is at most the number of bytes occupied by the string, but
	//# it is less for strings containing multi-byte characters. Each character is counted as one glyph regardless
	//# of the number of bytes it occupies.
	//#
	//# The string must be null-terminated by a zero byte. The null terminator is not included in the length.
	//
	//# \also	$@Text::GetTextLength@$


	//# \function	Text::Hash		Calculates the hash value for a character string.
	//
	//# \proto	unsigned_int32 Hash(const char *text);
	//
	//# \param	text	A pointer to a null-terminated character string.
	//
	//# \desc
	//# The $Hash$ function returns a 32-bit hash value for the character string specified by the $text$ parameter.
	//# The hash algorithm is designed so that different strings appear to have random hash values even if the strings
	//# only differ by a small amount.
	//#
	//# The hash value returned for the empty string is zero.
	//
	//# \note
	//# The $Hash$ function is <i>not</i> intended for cryptographic applications and does not produce a secure hash value.
	//# It should not be used for things like password storage.
	//
	//# \also	$@Utilities/HashTable@$


	//# \function	Text::FindChar		Searches for a particular byte value in a string.
	//
	//# \proto	int32 FindChar(const char *text, unsigned_int32 k);
	//# \proto	int32 FindChar(const char *text, unsigned_int32 k, int32 max);
	//
	//# \param	text	A pointer to a null-terminated character string.
	//# \param	k		An 8-bit bytes value to search for.
	//# \param	max		The maximum number of bytes to search.
	//
	//# \desc
	//# The $FindChar$ function searches the string specified by the $text$ parameter for the byte value given by the $k$ parameter.
	//# If the $max$ parameter is included, then it specifies the maximum number of bytes to search from the beginning of the string.
	//# Otherwise, the search continues until a zero byte is encountered. The string does not need to be null-terminated if the $max$
	//# parameter is included, but the search will still end if a zero byte is encountered before $max$ bytes have been searched.
	//#
	//# If the character given by $k$ is found, then the byte position within the string of the first occurrence is returned.
	//# If it is not found, then the return value is &minus;1.
	//
	//# \also	$@Text::FindUnquotedChar@$


	//# \function	Text::FindUnquotedChar		Searches for a particular byte value in a string.
	//
	//# \proto	int32 FindUnquotedChar(const char *text, unsigned_int32 k);
	//
	//# \param	text	A pointer to a null-terminated character string.
	//# \param	k		An 8-bit bytes value to search for. 
	//
	//# \desc
	//# The $FindUnquotedChar$ function searches the string specified by the $text$ parameter for the byte value given by the $k$ parameter. 
	//# Any text enclosed in double quotes (ASCII character 34) is ignored during the search. The quotes themselves are also ignored, so
	//# it is not possible to search for a quote character using this function. 
	//#
	//# If the character given by $k$ is found outside of double quotes, then the byte position within the string of the first occurrence
	//# is returned. If it is not found, then the return value is &minus;1. 
	//
	//# \also	$@Text::FindChar@$ 
 

	//# \function	Text::CompareText		Compares two strings for equality.
	//
	//# \proto	bool CompareText(const char *s1, const char *s2); 
	//# \proto	bool CompareText(const char *s1, const char *s2, int32 max);
	//
	//# \param	s1		A pointer to the first character string.
	//# \param	s2		A pointer to the second character string.
	//# \param	max		The maximum number of bytes to compare.
	//
	//# \desc
	//# The $CompareText$ function returns a boolean value indicating whether the two strings specified by the
	//# $s1$ and $s2$ parameters are equal. The comparison is case-sensitive.
	//#
	//# In general, both strings should be null-terminated by a zero byte. The comparison only goes as far as the
	//# length of the shorter string. If the $max$ parameter is specified, then at most $max$ bytes of each string
	//# are compared, even if both strings are longer.
	//
	//# \also	$@Text::CompareTextCaseless@$
	//# \also	$@Text::CompareTextLessThan@$
	//# \also	$@Text::CompareTextLessThanCaseless@$
	//# \also	$@Text::CompareTextLessEqual@$
	//# \also	$@Text::CompareTextLessEqualCaseless@$


	//# \function	Text::CompareTextCaseless		Compares two strings for equality, ignoring case.
	//
	//# \proto	bool CompareTextCaseless(const char *s1, const char *s2);
	//# \proto	bool CompareTextCaseless(const char *s1, const char *s2, int32 max);
	//
	//# \param	s1		A pointer to the first character string.
	//# \param	s2		A pointer to the second character string.
	//# \param	max		The maximum number of bytes to compare.
	//
	//# \desc
	//# The $CompareTextCaseless$ function returns a boolean value indicating whether the two strings specified by the
	//# $s1$ and $s2$ parameters are equal when case is ignored. All lowercase characters between $a$ and $z$ are considered
	//# equal to their corresponding uppercase characters between $A$ and $Z$.
	//#
	//# In general, both strings should be null-terminated by a zero byte. The comparison only goes as far as the
	//# length of the shorter string. If the $max$ parameter is specified, then at most $max$ bytes of each string
	//# are compared, even if both strings are longer.
	//
	//# \also	$@Text::CompareText@$
	//# \also	$@Text::CompareTextLessThan@$
	//# \also	$@Text::CompareTextLessThanCaseless@$
	//# \also	$@Text::CompareTextLessEqual@$
	//# \also	$@Text::CompareTextLessEqualCaseless@$


	//# \function	Text::CompareTextLessThan		Determines whether one string precedes another.
	//
	//# \proto	bool CompareTextLessThan(const char *s1, const char *s2);
	//# \proto	bool CompareTextLessThan(const char *s1, const char *s2, int32 max);
	//
	//# \param	s1		A pointer to the first character string.
	//# \param	s2		A pointer to the second character string.
	//# \param	max		The maximum number of bytes to compare.
	//
	//# \desc
	//# The $CompareTextLessThan$ function returns a boolean value indicating whether the string specified by the $s1$
	//# parameter precedes the string specified by the $s2$ parameter in lexicographical order.
	//#
	//# In general, both strings should be null-terminated by a zero byte. The comparison only goes as far as the
	//# length of the shorter string. If the $max$ parameter is specified, then at most $max$ bytes of each string
	//# are compared, even if both strings are longer.
	//
	//# \also	$@Text::CompareTextLessThanCaseless@$
	//# \also	$@Text::CompareTextLessEqual@$
	//# \also	$@Text::CompareTextLessEqualCaseless@$
	//# \also	$@Text::CompareTextCaseless@$
	//# \also	$@Text::CompareText@$


	//# \function	Text::CompareTextLessThanCaseless		Determines whether one string precedes another, ignoring case.
	//
	//# \proto	bool CompareTextLessThanCaseless(const char *s1, const char *s2);
	//# \proto	bool CompareTextLessThanCaseless(const char *s1, const char *s2, int32 max);
	//
	//# \param	s1		A pointer to the first character string.
	//# \param	s2		A pointer to the second character string.
	//# \param	max		The maximum number of bytes to compare.
	//
	//# \desc
	//# The $CompareTextLessThan$ function returns a boolean value indicating whether the string specified by the $s1$
	//# parameter precedes the string specified by the $s2$ parameter in lexicographical order when case is ignored.
	//# All lowercase characters between $a$ and $z$ are considered equal to their corresponding uppercase characters between $A$ and $Z$.
	//#
	//# In general, both strings should be null-terminated by a zero byte. The comparison only goes as far as the
	//# length of the shorter string. If the $max$ parameter is specified, then at most $max$ bytes of each string
	//# are compared, even if both strings are longer.
	//
	//# \also	$@Text::CompareTextLessThan@$
	//# \also	$@Text::CompareTextLessEqual@$
	//# \also	$@Text::CompareTextLessEqualCaseless@$
	//# \also	$@Text::CompareTextCaseless@$
	//# \also	$@Text::CompareText@$


	//# \function	Text::CompareTextLessEqual		Determines whether one string precedes another or the two strings are equal.
	//
	//# \proto	bool CompareTextLessEqual(const char *s1, const char *s2);
	//# \proto	bool CompareTextLessEqual(const char *s1, const char *s2, int32 max);
	//
	//# \param	s1		A pointer to the first character string.
	//# \param	s2		A pointer to the second character string.
	//# \param	max		The maximum number of bytes to compare.
	//
	//# \desc
	//# The $CompareTextLessEqual$ function returns a boolean value indicating whether the string specified by the $s1$
	//# parameter precedes the string specified by the $s2$ parameter in lexicographical order, or the two strings are equal.
	//#
	//# In general, both strings should be null-terminated by a zero byte. The comparison only goes as far as the
	//# length of the shorter string. If the $max$ parameter is specified, then at most $max$ bytes of each string
	//# are compared, even if both strings are longer.
	//
	//# \also	$@Text::CompareTextLessEqualCaseless@$
	//# \also	$@Text::CompareTextLessThan@$
	//# \also	$@Text::CompareTextLessThanCaseless@$
	//# \also	$@Text::CompareTextCaseless@$
	//# \also	$@Text::CompareText@$


	//# \function	Text::CompareTextLessEqualCaseless		Determines whether one string precedes another or the two strings are equal, ignoring case.
	//
	//# \proto	bool CompareTextLessEqualCaseless(const char *s1, const char *s2);
	//# \proto	bool CompareTextLessEqualCaseless(const char *s1, const char *s2, int32 max);
	//
	//# \param	s1		A pointer to the first character string.
	//# \param	s2		A pointer to the second character string.
	//# \param	max		The maximum number of bytes to compare.
	//
	//# \desc
	//# The $CompareTextLessEqual$ function returns a boolean value indicating whether the string specified by the $s1$
	//# parameter precedes the string specified by the $s2$ parameter in lexicographical order, or the two strings are equal, when case is ignored.
	//# All lowercase characters between $a$ and $z$ are considered equal to their corresponding uppercase characters between $A$ and $Z$.
	//#
	//# In general, both strings should be null-terminated by a zero byte. The comparison only goes as far as the
	//# length of the shorter string. If the $max$ parameter is specified, then at most $max$ bytes of each string
	//# are compared, even if both strings are longer.
	//
	//# \also	$@Text::CompareTextLessEqual@$
	//# \also	$@Text::CompareTextLessThan@$
	//# \also	$@Text::CompareTextLessThanCaseless@$
	//# \also	$@Text::CompareTextCaseless@$
	//# \also	$@Text::CompareText@$


	//# \function	Text::ReadString		Reads a string of non-whitespace characters from a character string.
	//
	//# \proto	int32 ReadString(const char *text, char *string, int32 max);
	//
	//# \param	text		A pointer to a character string.
	//# \param	string		A pointer to a buffer that will receive the read characters.
	//# \param	max			The maximum length of a string that can be stored in the buffer specified by the $string$ parameter.
	//
	//# \desc
	//# The $ReadString$ function reads a sequence of non-whitespace characters from the string specified by
	//# the $text$ parameter and returns the number of characters that were read. The characters composing the
	//# string are stored in the buffer specified by the $string$ parameter. The maximum number of characters
	//# returned in this buffer is specified by the $max$ parameter. The buffer specified by the $string$ parameter
	//# should be large enough to hold $max$ characters plus a zero terminator.
	//#
	//# If the first character pointed to by the $text$ parameter is not a double quote, then the $ReadString$
	//# function reads characters until it encounters a whitespace character as defined in the description of the
	//# $@System/Data::GetWhitespaceLength@$ function. If the first character is a double quote, then the $ReadString$
	//# function reads all of the characters between the first double quote and a closing double quote, including any
	//# whitespace. (The quotes themselves are not returned in the buffer pointed to by the $string$ parameter.)
	//# Any quote preceded by a backslash is not considered a closing quote, but is instead included in the returned
	//# string without the backslash character.


	namespace Text
	{
		typedef bool TextComparisonFunction(const char *, const char *);


		C4API int32 ReadGlyphCodeUTF8(const char *text, unsigned_int32 *code);
		C4API int32 WriteGlyphCodeUTF8(char *text, unsigned_int32 code);
		C4API int32 ValidateGlyphCodeUTF8(const char *text);

		C4API int32 GetGlyphCodeByteCountUTF8(unsigned_int32 code);
		C4API int32 GetGlyphCountUTF8(const char *text);
		C4API int32 GetGlyphCountUTF8(const char *text, int32 max);

		C4API int32 GetPreviousGlyphByteCountUTF8(const char *text, int32 max);
		C4API int32 GetNextGlyphByteCountUTF8(const char *text, int32 max);
		C4API int32 GetGlyphStringByteCountUTF8(const char *text, int32 glyphCount);

		C4API int32 GetTextLength(const char *text);
		C4API unsigned_int32 Hash(const char *text);

		C4API int32 FindChar(const char *text, unsigned_int32 k);
		C4API int32 FindChar(const char *text, unsigned_int32 k, int32 max);
		C4API int32 FindUnquotedChar(const char *text, unsigned_int32 k);
		C4API int32 CountChars(const char *text, unsigned_int32 k, int32 max);

		C4API int32 CopyText(const char *source, char *dest);
		C4API int32 CopyText(const char *source, char *dest, int32 max);
		C4API int32 CopyText(const wchar_t *source, char *dest, int32 max);

		C4API bool CompareText(const char *s1, const char *s2);
		C4API bool CompareText(const char *s1, const char *s2, int32 max);
		C4API bool CompareTextCaseless(const char *s1, const char *s2);
		C4API bool CompareTextCaseless(const char *s1, const char *s2, int32 max);
		C4API bool CompareTextLessThan(const char *s1, const char *s2);
		C4API bool CompareTextLessThan(const char *s1, const char *s2, int32 max);
		C4API bool CompareTextLessThanCaseless(const char *s1, const char *s2);
		C4API bool CompareTextLessThanCaseless(const char *s1, const char *s2, int32 max);
		C4API bool CompareTextLessEqual(const char *s1, const char *s2);
		C4API bool CompareTextLessEqual(const char *s1, const char *s2, int32 max);
		C4API bool CompareTextLessEqualCaseless(const char *s1, const char *s2);
		C4API bool CompareTextLessEqualCaseless(const char *s1, const char *s2, int32 max);
		C4API bool CompareNumberedTextLessThan(const char *s1, const char *s2);
		C4API bool CompareNumberedTextLessThanCaseless(const char *s1, const char *s2);

		C4API int32 FindText(const char *s1, const char *s2);
		C4API int32 FindTextCaseless(const char *s1, const char *s2);

		C4API int32 IntegerToString(int32 num, char *text, int32 max);
		C4API int32 StringToInteger(const char *text);
		C4API int32 Integer64ToString(int64 num, char *text, int32 max);
		C4API int64 StringToInteger64(const char *text);
		C4API int32 FloatToString(float num, char *text, int32 max);
		C4API float StringToFloat(const char *text);

		C4API int32 GetResourceNameLength(const char *text);
		C4API int32 GetDirectoryPathLength(const char *text);

		inline int32 GetPrefixDirectoryLength(const char *text)
		{
			return (FindChar(text, '/') + 1);
		}

		C4API int32 ReadInteger(const char *text, char *number, int32 max);
		C4API int32 ReadFloat(const char *text, char *number, int32 max);
		C4API int32 ReadString(const char *text, char *string, int32 max);
		C4API int32 ReadType(const char *text, unsigned_int32 *type);


		//# \class	StaticHash		Calculates a compile-time hash value for a string.
		//
		//# The $StaticHash$ class template is used to calculate a constant hash value for a string at compile time.
		//
		//# \def	template <...> class StaticHash
		//
		//# \desc
		//# The hash value for a string of up to 17 characters in length can be calculated as a compile-time constant
		//# by specifying each of the characters separately as template parameters. The constant hash value is accessed
		//# by using the $value$ enumerant member of the $StaticHash$ class. For example, the hash value of the
		//# string "foobar" is turned into a compile-time constant with the following expression:
		//
		//# \source
		//# Text::StaticHash<'f', 'o', 'o', 'b', 'a', 'r'>::value
		//
		//# \desc
		//# Hash values are case-sensitive.
		//
		//# \also	$@WorldMgr/Model::FindNode@$


		template <char c0, char c1 = 0, char c2 = 0, char c3 = 0, char c4 = 0, char c5 = 0, char c6 = 0, char c7 = 0, char c8 = 0, char c9 = 0, char c10 = 0, char c11 = 0, char c12 = 0, char c13 = 0, char c14 = 0, char c15 = 0, char c16 = 0> class StaticHash
		{
			private:

				template <char c, unsigned_int32 h> struct H
				{
					enum
					{
						hash = (c == 0) ? h : (unsigned_int32) ((h ^ c) * 0x6B84DF47ULL + 1)
					};
				};

			public:

				enum
				{
					value = H<c16, H<c15, H<c14, H<c13, H<c12, H<c11, H<c10, H<c9, H<c8, H<c7, H<c6, H<c5, H<c4, H<c3, H<c2, H<c1, H<c0, 0>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash>::hash
				};
		};
	}


	//# \class	String		Encapsulates a character string.
	//
	//# The $String$ class template encapsulates a character string having a fixed
	//# maximum length.
	//
	//# \def	template <int32 len = 0> class String
	//
	//# \tparam		len		The maximum length of the string, excluding the null terminator.
	//
	//# \ctor	String();
	//# \ctor	String(const String<len>& s);
	//# \ctor	String(const char *s);
	//# \ctor	String(const char *s, int32 length);
	//# \ctor	explicit String(int32 n);
	//# \ctor	explicit String(float n);
	//
	//# \param	s		A reference to another $String$ object or a pointer to a null-terminated
	//#					string that initializes the new $String$ object.
	//# \param	length	If specified, then the string is initialized with the character string pointed
	//#					to by the $s$ parameter, but at most $length$ characters are copied.
	//# \param	n		A signed integer or floating-point value that is converted to a decimal string
	//#					to initialize the new $String$ object.
	//
	//# \desc
	//# The $String$ class template can be used to store and manipulate strings. The $len$
	//# parameter establishes the maximum length of the string, excluding the null terminator.
	//# If the $len$ parameter is zero (the default if the parameter is not specified), then
	//# the string has unlimited length, and allocation for the memory used by the string is
	//# automatically managed.
	//#
	//# If the $len$ parameter is not zero, then the default constructor leaves the contents of the string
	//# undefined (and unterminated). If the $len$ parameter is zero, then the $String$ object is initially
	//# set to the empty string.
	//#
	//# $String$ objects can always be implicitly converted to a pointer to $char$ and thus can
	//# be accepted by any function expecting a parameter of type $char *$.
	//
	//# In addition to the member functions of the $String$ class, the $@Text@$ namespace contains several
	//# functions that are useful for manipulating character strings.
	//
	//# \operator	operator char *(void);
	//#				A $String$ object can be implicitly converted to an array of $char$.
	//
	//# \operator	operator const char *(void) const;
	//#				A $const String$ object can be implicitly converted to an array of $const char$.
	//
	//# \operator	String& operator =(const char *s);
	//#				Sets the contents of the $String$ object to the string pointed to by $s$.
	//#				If necessary, the string is truncated to the maximum length given by $len$.
	//
	//# \operator	String& operator +=(const char *s);
	//#				Appends the string pointed to by $s$ to the $String$ object, truncating to
	//#				the maximum length given by $len$.
	//
	//# \operator	String& operator +=(int32 n);
	//#				Converts the signed integer $n$ to a decimal string and appends it to the
	//#				$String$ object, truncating to the maximum length given by $len$.
	//
	//# \operator	String operator +(const char *s) const;
	//#				Returns a new $String$ object containing the concatenation with the string
	//#				pointed to by $s$. If necessary, the new string is truncated to the maximum
	//#				length given by $len$.
	//
	//# \operator	String operator +(int32 n) const;
	//#				Returns a new $String$ object containing the concatenation with the decimal
	//#				string corresponding to &n&. If necessary, the new string is truncated to the
	//#				maximum length given by $len$.
	//
	//# \operator	bool operator ==(const char *s) const;
	//#				Returns a boolean value indicating whether two strings have equal contents.
	//
	//# \operator	bool operator !=(const char *s) const;
	//#				Returns a boolean value indicating whether two strings have differing contents.
	//
	//# \operator	bool operator <(const char *s) const;
	//#				Returns a boolean value indicating whether the text in the $String$ object
	//#				precedes the text pointed to by $s$ in lexicographical order.
	//
	//# \operator	bool operator >(const char *s) const;
	//#				Returns a boolean value indicating whether the text in the $String$ object
	//#				follows the text pointed to by $s$ in lexicographical order.
	//
	//# \operator	bool operator <=(const char *s) const;
	//#				Returns a boolean value indicating whether the text in the $String$ object
	//#				precedes the text pointed to by $s$ in lexicographical order
	//#				or the two strings are equal.
	//
	//# \operator	bool operator >=(const char *s) const;
	//#				Returns a boolean value indicating whether the text in the $String$ object
	//#				follows the text pointed to by $s$ in lexicographical order
	//#				or the two strings are equal.
	//
	//# \also	$@Text@$


	//# \function	String::Length		Returns the length of a string.
	//
	//# \proto	int32 Length(void) const;
	//
	//# \desc
	//# The $Length$ function returns the length of the text contained in a $String$ object.
	//# The contents of the $String$ object must be defined so that the text is null-terminated.
	//# (The overloaded operators that manipulate the $String$ object all maintain the null terminator.)


	//# \function	String::ConvertToLowerCase		Converts each alphabetic character to lower case.
	//
	//# \proto	String<0>& ConvertToLowerCase(void);
	//
	//# \desc
	//# The $ConvertToLowerCase$ function examines each character in the string and converts any
	//# in the range $'A'$ to $'Z'$ to lower case.
	//#
	//# \note
	//# The $ConvertToLowerCase$ function is available only when the $len$ template parameter for the $String$
	//# class is the default value of zero.
	//
	//# \also	$@String::ConvertToUpperCase@$


	//# \function	String::ConvertToUpperCase		Converts each alphabetic character to upper case.
	//
	//# \proto	String<0>& ConvertToUpperCase(void);
	//
	//# \desc
	//# The $ConvertToUpperCase$ function examines each character in the string and converts any
	//# in the range $'a'$ to $'z'$ to upper case.
	//#
	//# \note
	//# The $ConvertToUpperCase$ function is available only when the $len$ template parameter for the $String$
	//# class is the default value of zero.
	//
	//# \also	$@String::ConvertToLowerCase@$


	template <int32 len = 0> class String
	{
		private:

			char	c[len + 1];

		public:

			String() = default;

			String(const String& s)
			{
				Text::CopyText(s.c, c, len);
			}

			String(const char *s)
			{
				Text::CopyText(s, c, len);
			}

			String(const char *s, int32 length)
			{
				Text::CopyText(s, c, Min(length, len));
			}

			explicit String(int32 n)
			{
				Text::IntegerToString(n, c, len);
			}

			explicit String(unsigned_int32 n)
			{
				Text::IntegerToString(n, c, len);
			}

			explicit String(int64 n)
			{
				Text::Integer64ToString(n, c, len);
			}

			explicit String(float n)
			{
				Text::FloatToString(n, c, len);
			}

			String(char c1, char c2, char c3, char c4)
			{
				c[0] = c1;
				c[1] = c2;
				c[2] = c3;
				c[3] = c4;
				c[4] = 0;
			}

			String& Set(const char *s, int32 length)
			{
				Text::CopyText(s, c, Min(length, len));
				return (*this);
			}

			operator char *(void)
			{
				return (c);
			}

			operator const char *(void) const
			{
				return (c);
			}

			String& operator =(const String& s)
			{
				Text::CopyText(s.c, c, len);
				return (*this);
			}

			String& operator =(const char *s)
			{
				Text::CopyText(s, c, len);
				return (*this);
			}

			String& operator =(int32 n)
			{
				Text::IntegerToString(n, c, len);
				return (*this);
			}

			String& operator =(unsigned_int32 n)
			{
				Text::IntegerToString(n, c, len);
				return (*this);
			}

			String& operator =(int64 n)
			{
				Text::Integer64ToString(n, c, len);
				return (*this);
			}

			String& operator =(float n)
			{
				Text::FloatToString(n, c, len);
				return (*this);
			}

			String& operator +=(const char *s)
			{
				int32 l = Length();
				Text::CopyText(s, &c[l], len - l);
				return (*this);
			}

			String& operator +=(char k)
			{
				int32 l = Length();
				if (l < len)
				{
					c[l] = k;
					c[l + 1] = 0;
				}

				return (*this);
			}

			String& operator +=(int32 n)
			{
				int32 l = Length();
				Text::IntegerToString(n, &c[l], len - l);
				return (*this);
			}

			String& operator +=(unsigned_int32 n)
			{
				int32 l = Length();
				Text::IntegerToString(n, &c[l], len - l);
				return (*this);
			}

			String& operator +=(int64 n)
			{
				int32 l = Length();
				Text::Integer64ToString(n, &c[l], len - l);
				return (*this);
			}

			String& operator +=(unsigned_int64 n)
			{
				int32 l = Length();
				Text::Integer64ToString(n, &c[l], len - l);
				return (*this);
			}

			String operator +(const char *s) const
			{
				return (String(c) += s);
			}

			String operator +(int32 n) const
			{
				return (String(c) += n);
			}

			String operator +(unsigned_int32 n) const
			{
				return (String(c) += n);
			}

			String operator +(int64 n) const
			{
				return (String(c) += n);
			}

			bool operator ==(const char *s) const
			{
				return (Text::CompareTextCaseless(c, s));
			}

			bool operator !=(const char *s) const
			{
				return (!Text::CompareTextCaseless(c, s));
			}

			bool operator <(const char *s) const
			{
				return (Text::CompareTextLessThanCaseless(c, s));
			}

			bool operator >=(const char *s) const
			{
				return (!Text::CompareTextLessThanCaseless(c, s));
			}

			bool operator <=(const char *s) const
			{
				return (Text::CompareTextLessEqualCaseless(c, s));
			}

			bool operator >(const char *s) const
			{
				return (!Text::CompareTextLessEqualCaseless(c, s));
			}

			int32 Length(void) const
			{
				return (Text::GetTextLength(c));
			}

			String& SetLength(int32 length)
			{
				c[length] = 0;
				return (*this);
			}

			String& Append(const char *s, int32 length)
			{
				int32 l = Length();
				Text::CopyText(s, &c[l], Min(len - l, length));
				return (*this);
			}
	};


	template <> class String<0>
	{
		private:

			enum
			{
				kStringAllocSize	= 63
			};

			int32		logicalSize;
			int32		physicalSize;
			char		*stringPointer;

			C4API static char	emptyString[1];

			C4API String(const char *s1, const char *s2);
			C4API String(int32 n, const char *s1);
			C4API String(unsigned_int32 n, const char *s1);
			C4API String(int64 n, const char *s1);

			static unsigned_int32 GetPhysicalSize(unsigned_int32 size)
			{
				return ((size + (kStringAllocSize + 4)) & ~kStringAllocSize);
			}

			void Resize(int32 size);

		public:

			C4API String();
			C4API ~String();

			String(String&& s)
			{
				logicalSize = s.logicalSize;
				physicalSize = s.physicalSize;
				stringPointer = s.stringPointer;

				s.stringPointer = emptyString;
			}

			C4API String(const String& s);
			C4API String(const char *s);
			C4API String(const char *s, int32 length);
			C4API String(const wchar_t *s);
			C4API explicit String(int32 n);
			C4API explicit String(unsigned_int32 n);
			C4API explicit String(int64 n);
			C4API explicit String(float n);

			operator char *(void)
			{
				return (stringPointer);
			}

			operator const char *(void) const
			{
				return (stringPointer);
			}

			bool operator ==(const char *s) const
			{
				return (Text::CompareTextCaseless(stringPointer, s));
			}

			bool operator !=(const char *s) const
			{
				return (!Text::CompareTextCaseless(stringPointer, s));
			}

			bool operator <(const char *s) const
			{
				return (Text::CompareTextLessThanCaseless(stringPointer, s));
			}

			bool operator >=(const char *s) const
			{
				return (!Text::CompareTextLessThanCaseless(stringPointer, s));
			}

			bool operator <=(const char *s) const
			{
				return (Text::CompareTextLessEqualCaseless(stringPointer, s));
			}

			bool operator >(const char *s) const
			{
				return (!Text::CompareTextLessEqualCaseless(stringPointer, s));
			}

			String operator +(const char *s) const
			{
				return (String(stringPointer, s));
			}

			String operator +(int32 n) const
			{
				return (String(n, stringPointer));
			}

			String operator +(unsigned_int32 n) const
			{
				return (String(n, stringPointer));
			}

			String operator +(int64 n) const
			{
				return (String(n, stringPointer));
			}

			int32 Length(void) const
			{
				return (logicalSize - 1);
			}

			C4API void Purge(void);
			C4API String& Set(const char *s, int32 length);

			C4API String& operator =(String&& s);
			C4API String& operator =(const String& s);
			C4API String& operator =(const char *s);
			C4API String& operator =(int32 n);
			C4API String& operator =(unsigned_int32 n);
			C4API String& operator =(int64 n);
			C4API String& operator =(float n);
			C4API String& operator +=(const String& s);
			C4API String& operator +=(const char *s);
			C4API String& operator +=(char k);
			C4API String& operator +=(int32 n);
			C4API String& operator +=(unsigned_int32 n);
			C4API String& operator +=(int64 n);
			C4API String& operator +=(unsigned_int64 n);

			C4API String& SetLength(int32 length);
			C4API String& Append(const char *s, int32 length);

			C4API String& ConvertToLowerCase(void);
			C4API String& ConvertToUpperCase(void);

			C4API String& EncodeEscapeSequences(void);
	};


	class ConstCharKey
	{
		private:

			const char		*ptr;

		public:

			ConstCharKey() = default;

			ConstCharKey(const char *c)
			{
				ptr = c;
			}

			template <int32 len> ConstCharKey(const String<len>& s)
			{
				ptr = s;
			}

			operator const char *(void) const
			{
				return (ptr);
			}

			ConstCharKey& operator =(const char *c)
			{
				ptr = c;
				return (*this);
			}

			bool operator ==(const char *c) const
			{
				return (Text::CompareText(ptr, c));
			}

			bool operator !=(const char *c) const
			{
				return (!Text::CompareText(ptr, c));
			}

			bool operator <(const char *c) const
			{
				return (Text::CompareTextLessThan(ptr, c));
			}
	};


	class StringKey
	{
		private:

			const char		*ptr;

		public:

			StringKey() = default;

			StringKey(const char *c)
			{
				ptr = c;
			}

			template <int32 len> StringKey(const String<len>& s)
			{
				ptr = s;
			}

			operator const char *(void) const
			{
				return (ptr);
			}

			StringKey& operator =(const char *c)
			{
				ptr = c;
				return (*this);
			}

			bool operator ==(const char *c) const
			{
				return (Text::CompareTextCaseless(ptr, c));
			}

			bool operator !=(const char *c) const
			{
				return (!Text::CompareTextCaseless(ptr, c));
			}

			bool operator <(const char *c) const
			{
				return (Text::CompareTextLessThanCaseless(ptr, c));
			}
	};


	class FileNameKey
	{
		private:

			const char		*ptr;

		public:

			FileNameKey() = default;

			FileNameKey(const char *c)
			{
				ptr = c;
			}

			template <int32 len> FileNameKey(const String<len>& s)
			{
				ptr = s;
			}

			operator const char *(void) const
			{
				return (ptr);
			}

			FileNameKey& operator =(const char *c)
			{
				ptr = c;
				return (*this);
			}

			bool operator ==(const char *c) const
			{
				return (Text::CompareTextCaseless(ptr, c));
			}

			bool operator !=(const char *c) const
			{
				return (!Text::CompareTextCaseless(ptr, c));
			}

			bool operator <(const char *c) const
			{
				return (Text::CompareNumberedTextLessThanCaseless(ptr, c));
			}
	};


	namespace Text
	{
		C4API extern const char hexDigit[16];


		inline String<15> IntegerToString(int32 num)
		{
			return (String<15>(num));
		}

		inline String<31> Integer64ToString(int64 num)
		{
			return (String<31>(num));
		}

		inline String<15> FloatToString(float num)
		{
			return (String<15>(num));
		}

		C4API String<31> Integer64ToHexString16(unsigned_int64 num);
		C4API String<15> IntegerToHexString8(unsigned_int32 num);
		C4API String<7> IntegerToHexString4(unsigned_int32 num);
		C4API String<3> IntegerToHexString2(unsigned_int32 num);

		C4API unsigned_int32 StringToType(const char *string);
		C4API String<4> TypeToString(unsigned_int32 type);
		C4API String<31> TypeToHexCharString(unsigned_int32 type);
	}
}


#endif

// ZYUQURM
