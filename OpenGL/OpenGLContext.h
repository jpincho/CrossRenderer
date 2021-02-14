#pragma once
#include "../Version.h"
#include "../RendererDefinitions.h"

namespace CrossRenderer
{
namespace OpenGL
{
class OpenGLContextInterface
    {
    protected:
        Version OpenGLVersion, GLSLVersion;
        std::vector <std::string> Extensions;
        std::string OpenGLRendererString, OpenGLVendorString;

    public:
        virtual ~OpenGLContextInterface ( void ) = default;
        virtual bool Create ( const RendererConfiguration &NewConfiguration, const RenderWindowHandle &Handle ) = 0;
        virtual bool Destroy ( void ) = 0;
        virtual bool MakeActive ( const RenderWindowHandle &Handle ) = 0;
        virtual void SwapWindowBuffer ( const RenderWindowHandle &Handle ) = 0;
        Version GetOpenGLVersion ( void ) const;
        Version GetGLSLVersion ( void ) const;
        bool IsExtensionSupported ( const char *Extension );
    protected:
        bool DetectOpenGLInformation ( void );
    };

OpenGLContextInterface *GetOpenGLContext ( const WindowManagerBackend DesiredType );
}
}
