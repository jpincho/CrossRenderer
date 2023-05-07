#include <CrossRendererConfig.hpp>
#include "StringUtils.hpp"
#include <cstdarg>
#if defined (CROSS_RENDERER_TARGET_PLATFORM_LINUX)
#include <strings.h>
#endif

namespace CrossRenderer
{
namespace StringUtils
{
void Split ( const std::string &Original, const char Delimiter, std::vector<std::string> &Result, const bool IgnoreEmptyLines, const bool RemoveDelimiter )
	{
	std::string::size_type LastPosition = 0;
	std::string::size_type CurrentPosition = 0;

	while ( ( CurrentPosition = Original.find ( Delimiter, LastPosition ) ) != std::string::npos )
		{
		std::string Found = Original.substr ( LastPosition, ( RemoveDelimiter ? CurrentPosition : CurrentPosition + 1 ) - LastPosition );
		if ( ( IgnoreEmptyLines == false ) || ( Found.length() != 0 ) )
			Result.push_back ( Found );
		LastPosition = CurrentPosition + 1;
		}
	if ( LastPosition < Original.size() )
		{
		Result.push_back ( Original.substr ( LastPosition ) );
		}
	}

void Split ( const std::string &Original, const std::string &Delimiter, std::vector<std::string> &Result, const bool IgnoreEmptyLines, const bool RemoveDelimiter )
	{
	std::string::size_type LastPosition = 0;
	std::string::size_type CurrentPosition = 0;

	while ( ( CurrentPosition = Original.find ( Delimiter, LastPosition ) ) != std::string::npos )
		{
		std::string Found = Original.substr ( LastPosition, ( RemoveDelimiter ? CurrentPosition : CurrentPosition + Delimiter.length() ) - LastPosition );
		if ( ( IgnoreEmptyLines == false ) || ( Found.length () != 0 ) )
			Result.push_back ( Found );
		LastPosition = CurrentPosition + Delimiter.length ();
		}
	if ( LastPosition < Original.size () )
		{
		Result.push_back ( Original.substr ( LastPosition ) );
		}
	}

void Replace ( std::string &Original, const std::string &Old, const std::string &New )
	{
	for ( auto Pos = Original.find ( Old ); Pos != std::string::npos; Pos = Original.find ( Old, Pos ) )
		Original.replace ( Pos, Old.size(), New );
	}

void Replace ( std::string &Original, const char Old, const char New )
	{
	for ( auto Pos = Original.find ( Old ); Pos != std::string::npos; Pos = Original.find ( Old, Pos ) )
		Original[Pos] = New;
	}

void Trim ( const std::string &Original, std::string &Result, const std::string &TrimCharacters )
	{
	size_t First = 0, Last = 0;
	Last = Original.find_last_not_of ( TrimCharacters );
	First = Original.find_first_not_of ( TrimCharacters );
	if ( ( Last == std::string::npos ) || ( First == std::string::npos ) )
		return;
	Result.assign ( Original.begin () + First, Original.begin () + Last + 1 );
	}

bool Compare ( const std::string &String1, const std::string &String2, const bool CaseSensitive )
	{
	if ( CaseSensitive )
		return String1 == String2;
#if defined ( CROSS_RENDERER_COMPILER_MSVC )
	return _stricmp ( String1.c_str (), String2.c_str () ) == 0;
#else
	return strcasecmp ( String1.c_str(), String2.c_str() );
#endif
	}

void Format ( std::string &Output, const char *FormatString, ... )
	{
	va_list ArgumentList;
	va_start ( ArgumentList, FormatString );
	Output.resize ( 128 );
	const int Length = vsnprintf ( &Output[0], Output.size(), FormatString, ArgumentList );
	va_end ( ArgumentList );

	if ( ( unsigned ) Length < Output.size() )
		{
		Output.resize ( Length );
		}
	else
		{
		va_start ( ArgumentList, FormatString );
		Output.resize ( Length  );
		va_end ( ArgumentList );
		}
	}

std::string FormattedString ( const char *FormatString, ... )
	{
	std::string Output;
	va_list ArgumentList;
	va_start ( ArgumentList, FormatString );
	Output.resize ( 128 );
	const int Length = vsnprintf ( &Output[0], Output.size (), FormatString, ArgumentList );
	va_end ( ArgumentList );

	if ( ( unsigned ) Length < Output.size () )
		{
		Output.resize ( Length );
		}
	else
		{
		va_start ( ArgumentList, FormatString );
		Output.resize ( Length );
		va_end ( ArgumentList );
		}
	return Output;
	}
}
}
