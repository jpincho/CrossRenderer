#include "ConsoleTextColor.hpp"

namespace ConsoleTextColor
{
void TextColor ( std::string &Output, const Color DesiredForeground, const Color DesiredBackground, const Attribute DesiredAttribute )
	{
	Output.clear ();
	Output.reserve ( 15 );

	Output.append ( 1, 0x1B );
	Output.append ( 1, '[' );
	if ( DesiredAttribute != Attribute::Ignore )
		Output.append ( std::to_string ( ( int ) DesiredAttribute ) );

	if ( DesiredForeground != Color::Ignore )
		{
		Output.append ( 1, ';' );
		Output.append ( std::to_string ( ( int8_t ) ( DesiredForeground ) + 30 ) );
		}
	if ( DesiredBackground != Color::Ignore )
		{
		Output.append ( 1, ';' );
		Output.append ( std::to_string ( ( int8_t ) ( DesiredBackground ) + 40 ) );
		}
	Output.insert ( Output.length (), "m" );
	}

void TextColorReset ( std::string &Output )
	{
	Output.clear ();
	static std::string NoColorString;
	if ( NoColorString.length () == 0 )
		{
		NoColorString.append ( 1, 0x1B );
		NoColorString.append ( "[0m" );
		}
	Output = NoColorString;
	}
}
