#pragma once

#include <string>
#include <stdarg.h>

namespace Logger
{
enum class LogLevel
    {
    Error,
    Warning,
    Debug,
    Log
    };

typedef struct LoggerOptions
    {
    enum class FunctionNameStyle
        {
        Off,
        Short,
        Long
        } FunctionStyle;
    bool ShowFilename, ShowLineNumber, ShowTimestamp;
    LoggerOptions ( void )
        {
        FunctionStyle = FunctionNameStyle::Short;
        ShowFilename = false;
        ShowLineNumber = true;
        ShowTimestamp = false;
        }
    } LoggerOptions;

extern LoggerOptions Options;


void Log ( const std::string In_Filename, const std::string In_PrettyFunctionName, const std::string In_FunctionName, const unsigned In_Line, const LogLevel In_Level, const char *In_LogMessage, ... );
}

#if defined ( _MSC_VER )
#define LOG_MSG(...) do {Logger::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, Logger::LogLevel::Log, __VA_ARGS__ );} while(0)
#define LOG_WARNING(...) do{Logger::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, Logger::LogLevel::Warning, __VA_ARGS__ );} while(0)
#define LOG_DEBUG(...) do{Logger::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, Logger::LogLevel::Debug, __VA_ARGS__ );} while(0)
#define LOG_ERROR(...) do{Logger::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, Logger::LogLevel::Error, __VA_ARGS__ );} while(0)
#else
#define LOG_MSG(...) do {Logger::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, Logger::LogLevel::Log, __VA_ARGS__ );} while(0)
#define LOG_WARNING(...) do{Logger::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, Logger::LogLevel::Warning, __VA_ARGS__ );} while(0)
#define LOG_DEBUG(...) do{Logger::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, Logger::LogLevel::Debug, __VA_ARGS__ );} while(0)
#define LOG_ERROR(...) do{Logger::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, Logger::LogLevel::Error, __VA_ARGS__ );} while(0)
#endif