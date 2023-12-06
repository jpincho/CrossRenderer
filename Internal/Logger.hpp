#pragma once
#include <stdarg.h>

namespace CrossRenderer
{
enum class LogLevel
	{
	Error,
	Warning,
	Debug,
	Log,
	Raw
	};

struct LoggerOptions
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
	};

extern LoggerOptions Options;


void Log ( const char *Filename, const char *PrettyFunctionName, const char *FunctionName, const unsigned Line, const LogLevel Level, const char *Message, ... );
}

#if defined ( _MSC_VER )
#define LOG_MSG(...) do {CrossRenderer::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Log, __VA_ARGS__ );} while(0)
#define LOG_WARNING(...) do{CrossRenderer::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Warning, __VA_ARGS__ );} while(0)
#define LOG_DEBUG(...) do{CrossRenderer::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Debug, __VA_ARGS__ );} while(0)
#define LOG_ERROR(...) do{CrossRenderer::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Error, __VA_ARGS__ );} while(0)
#define LOG_RAW(...) do{CrossRenderer::Log ( __FILE__, __FUNCSIG__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Raw, __VA_ARGS__ );} while(0)
#else
#define LOG_MSG(...) do {CrossRenderer::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Log, __VA_ARGS__ );} while(0)
#define LOG_WARNING(...) do{CrossRenderer::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Warning, __VA_ARGS__ );} while(0)
#define LOG_DEBUG(...) do{CrossRenderer::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Debug, __VA_ARGS__ );} while(0)
#define LOG_ERROR(...) do{CrossRenderer::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Error, __VA_ARGS__ );} while(0)
#define LOG_RAW(...) do{CrossRenderer::Log ( __FILE__, __PRETTY_FUNCTION__, __FUNCTION__, __LINE__, CrossRenderer::LogLevel::Raw, __VA_ARGS__ );} while(0)
#endif
