#pragma once
#include <stdio.h>
#include <string>

namespace ConsoleColor
{

enum class Attribute
    {
    Ignore = -1,
    Reset = 0,
    Bright,
    Dim,
    Blink,
    Reverse = 7,
    Hidden
    };

enum class Color
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

std::string TextColor ( Color In_Foreground = Color::Ignore, Color In_Background = Color::Ignore, Attribute In_Attribute = Attribute::Ignore );
const std::string &NoTextColor ( void );

}
