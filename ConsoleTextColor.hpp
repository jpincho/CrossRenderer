#pragma once
#include <string>

namespace ConsoleTextColor
{
enum class Attribute : int8_t
	{
	Ignore = -1,
	Reset = 0,
	Bright,
	Dim,
	Blink,
	Reverse = 7,
	Hidden
	};

enum class Color : int8_t
	{
	Ignore = -1,
	Black = 0,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White
	};

void TextColor ( std::string &Output, const Color DesiredForeground, const Color DesiredBackground = Color::Ignore, const Attribute DesiredAttribute = Attribute::Ignore );
void TextColorReset ( std::string &Output );
}
