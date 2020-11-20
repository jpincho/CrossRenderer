#include "Logger.h"
#include "ConsoleColor.h"
#include <mutex>
#include <time.h>
#include <iostream>
#include <memory.h>

namespace Logger
{
LoggerOptions Options;
static bool Initialized = false;
static std::mutex LoggerMutex;

void Initialize ( void )
    {
    Initialized = true;
    }

std::string GetTimeString ( void )
    {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t ( now );
    tm local_tm = *localtime ( &tt );

    static char Tick[100];
    snprintf ( Tick, sizeof ( Tick ), "%02u-%02u-%04u %02u:%02u:%02u ", local_tm.tm_mday, local_tm.tm_mon + 1, local_tm.tm_year + 1900,
               local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec );
    return Tick;
    }

void Log ( const std::string In_Filename, const std::string In_PrettyFunctionName, const std::string In_FunctionName, const unsigned In_Line, const LogLevel In_Level, const char* In_LogMessage, ... )
    {
    std::lock_guard<std::mutex> MutexLock ( LoggerMutex );
    std::string LogMessage;

        {
        // Formats the string
        char *Buffer;
        va_list args;
        va_start ( args, In_LogMessage );

#if defined ( LOGGER_PLATFORM_WINDOWS )
//#if _MSC_VER >= 1500
        int Written;
        int BufferSize = _vscprintf ( In_LogMessage, args ) + 1;
        Buffer = ( char * ) malloc ( BufferSize );
        memset ( Buffer, 0, BufferSize );
        Written = _vsnprintf ( Buffer, BufferSize, In_LogMessage, args );
        if ( Written == -1 )
            Buffer[0] = 0;
#elif defined ( LOGGER_PLATFORM_DARWIN ) || defined ( LOGGER_PLATFORM_LINUX )
        // Linux just takes care of it... coooooooooool
        vasprintf ( &Buffer, In_LogMessage, args );
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
        FilenameString += In_Filename + std::string ( " " );
        }

    std::string FunctionString;
    switch ( Options.FunctionStyle )
        {
        case LoggerOptions::FunctionNameStyle::Short:
            {
            FunctionString = ConsoleColor::TextColor ( ConsoleColor::Color::Green, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
            FunctionString += In_FunctionName + std::string ( " " );
            break;
            }
        case LoggerOptions::FunctionNameStyle::Long:
            {
            FunctionString = ConsoleColor::TextColor ( ConsoleColor::Color::Green, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
            FunctionString = In_PrettyFunctionName + std::string ( " " );
            break;
            }
        default:
            break;
        }

    std::string LineNumberString;
    if ( Options.ShowLineNumber )
        LineNumberString = std::string ( "[" ) + std::to_string ( In_Line ) + std::string ( "] " );

    // Get all these strings into static variables
    std::string LogLevelString;
    switch ( In_Level )
        {
        case LogLevel::Debug:
            LogLevelString += ConsoleColor::TextColor ( ConsoleColor::Color::Cyan, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( "[DEBUG]" );
            break;
        case LogLevel::Error:
            LogLevelString += ConsoleColor::TextColor ( ConsoleColor::Color::Black, ConsoleColor::Color::Red, ConsoleColor::Attribute::Bright ) + std::string ( "[ERROR]" );
            break;
        case LogLevel::Log:
            LogLevelString += ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( "[MSG]" );
            break;
        case LogLevel::Warning:
            LogLevelString += ConsoleColor::TextColor ( ConsoleColor::Color::Yellow, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( "[WARN]" );
            break;
        }
    LogLevelString += ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset ) + std::string ( " " );

#if defined ( LOGGER_USE_SDL2 )
    switch ( In_Level )
        {
        default:
        case LogLevel::Debug:
            SDL_LogDebug ( SDL_LOG_CATEGORY_APPLICATION, LogMessage.c_str() );
            break;
        }
#else
    std::cout << TimeString << FilenameString << FunctionString << LineNumberString << LogLevelString << LogMessage << std::endl;
    std::cout << ConsoleColor::TextColor ( ConsoleColor::Color::Ignore, ConsoleColor::Color::Ignore, ConsoleColor::Attribute::Reset );
#endif
    }
}

