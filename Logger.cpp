#include "Logger.h"
#include <mutex>
#include <time.h>
//#include <memory.h>*/
#include <iostream>
#include <CrossRendererConfig.h>

namespace ConsoleColor
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

std::string TextColor ( Color Foreground, Color Background, Attribute Attribute )
    {
    static std::string Commands[9][9];
    if ( Commands[1][1].empty () )
        {
        for ( int8_t ForegroundIterator = ( int8_t ) Color::Ignore; ForegroundIterator <= ( int8_t ) Color::White; ++ForegroundIterator )
            {
            for ( int8_t BackgroundIterator = ( int8_t ) Color::Ignore; BackgroundIterator <= ( int8_t ) Color::White; ++BackgroundIterator )
                {
                Commands[ForegroundIterator + 1][BackgroundIterator + 1].assign ( "" );
                if ( ForegroundIterator != ( int8_t ) Color::Ignore )
                    {
                    char Temp[10];
                    snprintf ( Temp, 10, ";%d", ( int8_t ) ( ForegroundIterator ) + 30 );
                    Commands[ForegroundIterator + 1][BackgroundIterator + 1].append ( Temp );
                    }
                if ( BackgroundIterator != ( int8_t ) Color::Ignore )
                    {
                    char Temp[10];
                    snprintf ( Temp, 10, ";%d", ( int8_t ) ( BackgroundIterator ) + 40 );
                    Commands[ForegroundIterator + 1][BackgroundIterator + 1].append ( Temp );
                    }
                }
            }
        }
    char command[13] = { 0 };
    snprintf ( command, 13, "%c[%d%sm", 0x1B, ( int ) Attribute, Commands[ ( int8_t ) Foreground + 1][ ( int8_t ) Background + 1].c_str() );
    return std::string ( command );
    }

const std::string &NoTextColor ( void )
    {
    static std::string NoColor;
    if ( NoColor.empty () )
        {
        NoColor.resize ( 5 );
        snprintf ( &NoColor[0], NoColor.length (), "%c[0m", 0x1B );
        }
    return NoColor;
    }
}

namespace CrossRenderer
{
LoggerOptions Options;
static std::mutex LoggerMutex;

std::string GetTimeString ( void )
    {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t ( now );
    tm local_tm;
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
    localtime_s ( &local_tm, &tt );
#else
    local_tm = *localtime ( &tt );
#endif
    static char Tick[100];
    snprintf ( Tick, sizeof ( Tick ), "%02u-%02u-%04u %02u:%02u:%02u ", local_tm.tm_mday, local_tm.tm_mon + 1, local_tm.tm_year + 1900,
               local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec );
    return Tick;
    }

void Log ( const std::string Filename, const std::string PrettyFunctionName, const std::string FunctionName, const unsigned Line, const LogLevel Level, const char* Message, ... )
    {
    std::lock_guard<std::mutex> MutexLock ( LoggerMutex );
    std::string LogMessage;

        {
        // Formats the string
        char *Buffer;
        va_list args;
        va_start ( args, Message );

#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
        int Written;
        int BufferSize = _vscprintf ( Message, args ) + 1;
        Buffer = ( char * ) malloc ( BufferSize );
        memset ( Buffer, 0, BufferSize );
        Written = vsnprintf_s ( Buffer, BufferSize, BufferSize, Message, args );
        if ( Written == -1 )
            Buffer[0] = 0;
#elif defined ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
        // Linux just takes care of it... coooooooooool
        vasprintf ( &Buffer, Message, args );
#endif
        va_end ( args );
        LogMessage.assign ( Buffer );
        free ( Buffer );
        }

    // Trims \r, \n and spaces from the end
        {
        size_t found;
        found = LogMessage.find_last_not_of ( "\r\n " );
        if ( found != std::string::npos )
            LogMessage.erase ( found + 1 );
        else
            LogMessage.clear();
        }

    std::string TimeString;
    if ( Options.ShowTimestamp )
        {
        TimeString = ConsoleColor::TextColor ( ConsoleColor::Color::White, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
        TimeString += GetTimeString();
        }

    std::string FilenameString;
    if ( Options.ShowFilename )
        {
        FilenameString = ConsoleColor::TextColor ( ConsoleColor::Color::Magenta, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
        FilenameString += Filename + std::string ( " " );
        }

    std::string FunctionString;
    switch ( Options.FunctionStyle )
        {
        case LoggerOptions::FunctionNameStyle::Short:
            {
            FunctionString = ConsoleColor::TextColor ( ConsoleColor::Color::Green, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
            FunctionString += FunctionName + std::string ( " " );
            break;
            }
        case LoggerOptions::FunctionNameStyle::Long:
            {
            FunctionString = ConsoleColor::TextColor ( ConsoleColor::Color::Green, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
            FunctionString = PrettyFunctionName + std::string ( " " );
            break;
            }
        default:
            break;
        }

    std::string LineNumberString;
    if ( Options.ShowLineNumber )
        LineNumberString = std::string ( "[" ) + std::to_string ( Line ) + std::string ( "] " );

    // Get all these strings into static variables
    static std::string LogLevelStrings[] =
        {
        ConsoleColor::TextColor ( ConsoleColor::Color::Black, ConsoleColor::Color::Red, ConsoleColor::Attribute::Bright ) + std::string ( "[ERROR]" ) + ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( " " ),
        ConsoleColor::TextColor ( ConsoleColor::Color::Yellow, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( "[WARN]" ) + ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( " " ),
        ConsoleColor::TextColor ( ConsoleColor::Color::Cyan, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( "[DEBUG]" ) + ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( " " ),
        ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( "[MSG]" ) + ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( " " )
        };

    std::cout << TimeString << FilenameString << FunctionString << LineNumberString << LogLevelStrings[ ( int ) Level] << LogMessage << std::endl;
    std::cout << ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
    }
}

