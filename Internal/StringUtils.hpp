#pragma once
#include <string>
#include <vector>

namespace CrossRenderer
{
namespace StringUtils
{
void Split ( const std::string &Original, const char Delimiter, std::vector<std::string> &Result, const bool IgnoreEmptyLines = false, const bool RemoveDelimiter = true );
void Split ( const std::string &Original, const std::string &Delimiter, std::vector<std::string> &Result, const bool IgnoreEmptyLines = false, const bool RemoveDelimiter = true );
void Replace ( std::string &Original, const std::string &Old, const std::string &New );
void Replace ( std::string &Original, const char Old, const char New );
void Trim ( const std::string &Original, std::string &Result, const std::string &TrimCharacters = " \n\r\t" );
bool Compare ( const std::string &String1, const std::string &String2, const bool CaseSensitive = true );
void Format ( std::string &Output, const char *FormatString, ... );
std::string FormattedString ( const char *FormatString, ... );
}
}
