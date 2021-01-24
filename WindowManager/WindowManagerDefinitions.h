#pragma once
#include <CrossRendererConfig.h>
#include <string>
#include "../HandleTemplate.h"
#include "../GLMHeaders.h"

namespace CrossRenderer
{
struct RenderWindowTag {};
typedef HandleTemplate <RenderWindowTag> RenderWindowHandle;
typedef struct RenderWindowDescriptor
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
    } RenderWindowDescriptor;

enum class WindowEventType
    {
    WindowCreated,
    WindowClosed,
    WindowMoved,
    WindowResized,
    KeyPressed,
    KeyReleased,
    ButtonPressed,
    ButtonReleased,
    MouseWheel,
    MouseMoved,
    Quit,
    FrameStarted,
    FrameEnded,
    TextInput
    };

typedef struct WindowEvent
    {
    WindowEventType EventType;
    RenderWindowHandle OwnerHandle;
    union
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
            int32_t X, Y;
            } MouseWheel;
        struct
            {
            int32_t X, Y;
            } MouseMoved;
        struct
            {
            uint32_t Data1, Data2;
            } Raw;
        struct
            {
            int32_t X, Y;
            } WindowMoved;
        struct
            {
            uint32_t Width, Height;
            } WindowResized;
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
    SDL2
    };
}