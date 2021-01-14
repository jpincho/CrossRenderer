#include "ConsoleColor.h"

namespace ConsoleColor
{

std::string TextColor ( Color In_Foreground, Color In_Background, Attribute In_Attribute )
    {
    char command[13] = {0};
    char BackStr[10] = {0}, ForeStr[10] = {0};
    if ( In_Foreground != Color::Ignore )
        snprintf ( ForeStr, 10, ";%d", ( int ) ( In_Foreground ) + 30 );
    if ( In_Background != Color::Ignore )
        snprintf ( BackStr, 10, ";%d", ( int ) ( In_Background ) + 40 );

    snprintf ( command, 13, "%c[%d%s%sm", 0x1B, ( int ) In_Attribute, ForeStr, BackStr );
    return std::string ( command );
    }

const std::string &NoTextColor ( void )
    {
    static std::string NoColor;
    if ( NoColor.empty() )
        {
        NoColor.resize ( 5 );
        snprintf ( &NoColor[0], NoColor.length(), "%c[0m", 0x1B );
        }
    return NoColor;
    }
}
