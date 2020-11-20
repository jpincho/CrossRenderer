#include <vector>
#include "CrossRendererConfig.h"
#include "../CrossRenderer.h"
#include "LoadFileContents.h"
#include "../WindowManager.h"
#include "../Stringify.h"
#include <Logger/Logger.h>

class TestBase : public CrossRenderer::EventListener
    {
    protected:
        bool ShouldQuit = false;
        bool TestResult = false;
        std::string TestName;
        std::vector <CrossRenderer::RenderCommand> RenderCommands;
        CrossRenderer::RendererBackend RendererBackend;

        virtual void OnEvent ( const CrossRenderer::WindowEvent &Event );
        CrossRenderer::ShaderHandle LoadShader ( const std::string &VertexFile, const std::string &GeometryFile, const std::string &FragmentFile );

    public:
        TestBase ( void );
        bool Initialize ( const CrossRenderer::RendererBackend NewRendererBackend, const glm::ivec2 &NewWindowPosition, const glm::uvec2 &NewWindowSize );
        bool Shutdown ( void );
        bool GetResult ( void ) const;
        virtual bool RunFrame ( void );
        virtual bool SpecificInitialize ( void ) = 0;
    };
