#pragma once
#include <CrossRendererConfig.hpp>
#include "WindowManagerDefinitions.hpp"
#include <unordered_set>

namespace CrossRenderer
{
class WindowManagerInterface
    {
    public:
        virtual RenderWindowHandle CreateNewWindow ( const RenderWindowDescriptor &Descriptor ) = 0;
        virtual bool DestroyWindow ( const RenderWindowHandle &Handle ) = 0;
        virtual void SetWindowPosition ( const RenderWindowHandle &Handle, const glm::ivec2 &NewPosition ) = 0;
        virtual glm::ivec2 GetWindowPosition ( const RenderWindowHandle &Handle ) = 0;
        virtual void SetWindowSize ( const RenderWindowHandle &Handle, const glm::uvec2 &NewSize ) = 0;
        virtual glm::uvec2 GetWindowSize ( const RenderWindowHandle &Handle ) = 0;
        virtual void SetWindowTitle ( const RenderWindowHandle &Handle, const std::string &NewTitle ) = 0;
        virtual std::string GetWindowTitle ( const RenderWindowHandle &Handle ) = 0;
        virtual bool SetWindowFullscreen ( const RenderWindowHandle &Handle, const bool NewState ) = 0;
        virtual bool IsWindowFullscreen ( const RenderWindowHandle &Handle ) = 0;

        virtual void ProcessEvents ( void ) = 0;

        virtual uint32_t GetKeyCode ( const std::string KeyName ) = 0;
        virtual uint32_t GetKeyScanCode ( const std::string KeyName ) = 0;
        virtual const char * GetKeyName ( const uint32_t KeyCode ) = 0;
        virtual glm::ivec2 GetMousePosition ( void ) = 0;
        virtual uint32_t GetMouseButtonStatus ( void ) = 0;
        virtual std::string GetErrorDescription ( void ) = 0;

        std::unordered_set<RenderWindowHandle> WindowList;
        WindowManagerBackend GetType ( void );

        void AddEventListener ( void Function ( const WindowEvent & ) );
        void DeleteEventListener ( void Function ( const WindowEvent & ) );
        void AddEventListener ( EventListener *Listener );
        void DeleteEventListener ( EventListener *Listener );
        void AddEventListener ( WindowEventType EventToListen, void Function ( const WindowEvent & ) );
        void DeleteEventListener ( WindowEventType EventToListen, void Function ( const WindowEvent & ) );
        void SendWindowEvent ( const WindowEvent &NewEvent );
    };
}
