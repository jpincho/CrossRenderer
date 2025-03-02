#include <CrossRendererConfig.hpp>
#include "Logger.hpp"
#include "ConsoleTextColor.hpp"
#if defined (CROSS_RENDERER_TARGET_PLATFORM_WINDOWS)
#include <Windows.h>
#endif
#include <mutex>
#include <time.h>
#include <chrono>

namespace CrossRenderer
{
LoggerOptions Options;
static std::mutex LoggerMutex;

const char *Stringify ( const LogLevel Value )
	{
#define STRINGIFY(X) case LogLevel::X: return #X;
	switch ( Value )
		{
			STRINGIFY ( Error );
			STRINGIFY ( Warning );
			STRINGIFY ( Debug );
			STRINGIFY ( Log );
			STRINGIFY ( Raw );
		}
#undef STRINGIFY
	throw "Unhandled log level";
	}

std::string GetTimeString ( void )
	{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now ();
	time_t tt = std::chrono::system_clock::to_time_t ( now );
	tm local_tm;
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
	localtime_s ( &local_tm, &tt );
#else
	local_tm = *localtime ( &tt );
#endif
	static char TimeString[100];
	snprintf ( TimeString, sizeof ( TimeString ), "%02u-%02u-%04u %02u:%02u:%02u ", local_tm.tm_mday, local_tm.tm_mon + 1, local_tm.tm_year + 1900,
	           local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec );
	return TimeString;
	}

void Log ( const char *Filename, const char *PrettyFunctionName, const char *FunctionName, const unsigned Line, const LogLevel Level, const char *Message, ... )
	{
	std::lock_guard<std::mutex> MutexLock ( LoggerMutex );
	static bool Initialized = false;
	static std::string TimeFormatString, FilenameFormatString, FunctionFormatString;
	static std::string LogLevelFormatStrings[4], TextColorResetString;
	static char Buffer[1024];

	static std::string FinalString;

	if ( Initialized == false )
		{
		FinalString.reserve ( 1024 );
		ConsoleTextColor::TextColor ( TimeFormatString, ConsoleTextColor::Color::White );
		ConsoleTextColor::TextColor ( FilenameFormatString, ConsoleTextColor::Color::Magenta );
		ConsoleTextColor::TextColor ( FilenameFormatString, ConsoleTextColor::Color::Magenta );
		ConsoleTextColor::TextColor ( FunctionFormatString, ConsoleTextColor::Color::Green );
		ConsoleTextColor::TextColor ( LogLevelFormatStrings[(int) LogLevel::Error], ConsoleTextColor::Color::Red, ConsoleTextColor::Color::Ignore, ConsoleTextColor::Attribute::Bright );
		ConsoleTextColor::TextColor ( LogLevelFormatStrings[(int) LogLevel::Warning], ConsoleTextColor::Color::Yellow );
		ConsoleTextColor::TextColor ( LogLevelFormatStrings[(int) LogLevel::Debug], ConsoleTextColor::Color::Cyan );
		ConsoleTextColor::TextColorReset ( LogLevelFormatStrings[(int) LogLevel::Log] );
		ConsoleTextColor::TextColorReset ( TextColorResetString );

#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
		// Set output mode to handle virtual terminal sequences
		HANDLE hOut = GetStdHandle ( STD_OUTPUT_HANDLE );
		HANDLE hIn = GetStdHandle ( STD_INPUT_HANDLE );
		DWORD dwOriginalOutMode = 0;
		DWORD dwOriginalInMode = 0;

		if ( ( hOut != INVALID_HANDLE_VALUE ) && ( hIn != INVALID_HANDLE_VALUE ) && ( GetConsoleMode ( hOut, &dwOriginalOutMode ) ) && ( GetConsoleMode ( hIn, &dwOriginalInMode ) ) )
			{
			DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
			DWORD dwRequestedInModes = ENABLE_VIRTUAL_TERMINAL_INPUT;

			DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
			if ( !SetConsoleMode ( hOut, dwOutMode ) )
				{
				// we failed to set both modes, try to step down mode gracefully.
				dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
				if ( !SetConsoleMode ( hOut, dwOutMode ) )
					{
					// Failed to set any VT mode, can't do anything here.
					printf ( "Unable to enable virtual terminal properties\n" );
					}
				}

			DWORD dwInMode = dwOriginalInMode | dwRequestedInModes;
			if ( !SetConsoleMode ( hIn, dwInMode ) )
				{
				// Failed to set VT input mode, can't do anything here.
				printf ( "Unable to enable virtual terminal properties\n" );
				}
			}
#endif
		Initialized = true;
		}

	// Formats the string
	va_list args;
	va_start ( args, Message );

	int Written;
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
	Written = vsnprintf_s ( Buffer, 1024, _TRUNCATE, Message, args );
#elif defined ( CROSS_RENDERER_TARGET_PLATFORM_LINUX )
	Written = vsnprintf ( Buffer, 1024, Message, args );
#endif
	if ( Written == -1 )
		Buffer[0] = 0;
	va_end ( args );

	FinalString = TextColorResetString;
	if ( Level != LogLevel::Raw )
		{
		// Time stamp
		if ( Options.ShowTimestamp )
			{
			FinalString.append ( TimeFormatString );
			FinalString.append ( GetTimeString () );
			FinalString.append ( 1, ' ' );
			}

		// Log level
		FinalString.append ( LogLevelFormatStrings[(int) Level] );
		FinalString.append ( 1, '[' );
		FinalString.append ( Stringify ( Level ) );
		FinalString.append ( 1, ']' );
		FinalString.append ( TextColorResetString );
		FinalString.append ( 1, ' ' );

		if ( Options.ShowFilename )
			{
			FinalString.append ( FilenameFormatString );
			FinalString.append ( Filename );
			FinalString.append ( 1, ' ' );
			}

		std::string FunctionString;
		switch ( Options.FunctionStyle )
			{
			case LoggerOptions::FunctionNameStyle::Short:
				{
				FinalString.append ( FunctionFormatString );
				FinalString.append ( FunctionName );
				FinalString.append ( TextColorResetString );
				FinalString.append ( 1, ' ' );
				break;
				}
			case LoggerOptions::FunctionNameStyle::Long:
				{
				FinalString.append ( FunctionFormatString );
				FinalString.append ( PrettyFunctionName );
				FinalString.append ( TextColorResetString );
				FinalString.append ( 1, ' ' );
				break;
				}
			default:
				break;
			}
		FinalString.append ( 1, '[' );
		FinalString.append ( std::to_string ( Line ) );
		FinalString.append ( "] " );
		}
	FinalString.append ( Buffer );
	printf ( "%s\n\r", FinalString.c_str () );
	}
}

