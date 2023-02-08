#pragma once
#include <CrossRendererConfig.hpp>
#include <string>
#include "../RendererHandleTemplate.hpp"
#include "../GLMHeaders.hpp"

namespace CrossRenderer
{
struct RenderWindowDescriptor
    {
    RenderWindowDescriptor ( const glm::ivec2 NewPosition = glm::ivec2 ( 0, 0 ), const glm::uvec2 NewSize = glm::uvec2 ( 640, 480 ) )
        {
        Position = NewPosition;
        Size = NewSize;
        Resizable = true;
        Fullscreen = false;
        Title.assign ( "CrossRenderer" );
        }
    glm::uvec2 Size;
    glm::ivec2 Position;
    bool Resizable, Fullscreen;
    std::string Title;
    };

enum class WindowState
	{
	Minimized,
	Windowed,
	Maximized
	};

enum class WindowEventType
    {
	WindowCreated,
	WindowClosed,
	WindowMoved,
	WindowResized,
	WindowFocusChanged,
	WindowStateChanged,
	KeyPressed,
	KeyReleased,
	ButtonPressed,
	ButtonReleased,
	MouseWheel,
	MouseMoved,
	Quit,
	TextInput
    };

typedef struct WindowEvent
    {
    WindowEventType EventType;
    RenderWindowHandle OwnerHandle;
	struct
		{
		struct
			{
			uint32_t Key, ScanCode;
			} KeyPressed, KeyReleased;
		struct
			{
			uint32_t Button;
			} ButtonPressed, ButtonReleased;
		struct
			{
			glm::vec2 Amount;
			} MouseWheel;
		struct
			{
			glm::ivec2 NewPosition, Delta;
			} MouseMoved;
		struct
			{
			uint32_t Data1, Data2;
			} Raw;
		struct
			{
			glm::ivec2 NewPosition;
			} WindowMoved;
		struct
			{
			glm::uvec2 NewSize;
			} WindowResized;
		struct
			{
			bool NewState;
			} WindowFocusChanged;
		struct
			{
			WindowState NewState;
			} WindowStateChanged;
		struct
			{
			char Text[32];
			} TextInput;
		void *Pointer;
		} EventData;
    WindowEvent ( void ) = default;
    } WindowEvent;

class EventListener
    {
    public:
        virtual ~EventListener ( void ) = default;
        virtual void OnEvent ( const WindowEvent &Event ) = 0;
    };

enum class WindowManagerBackend
    {
    GLFW3
    };
}
