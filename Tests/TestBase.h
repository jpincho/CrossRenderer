#include <vector>
#include "CrossRendererConfig.h"
#include "../CrossRenderer.h"
#include "LoadFileContents.h"
#include "../WindowManager.h"
#include "../Stringify.h"
#include <Logger/Logger.h>
#include <imgui/imgui.h>

class TestBase : public CrossRenderer::EventListener
    {
    protected:
        bool ShouldQuit = false;
        bool TestResult = false;
        std::vector <CrossRenderer::RenderCommand> RenderCommands;
        CrossRenderer::RendererBackend RendererBackend;
        CrossRenderer::FramebufferHandle DefaultFramebuffer;

        void OnEvent ( const CrossRenderer::WindowEvent &Event );
        CrossRenderer::TextureHandle LoadTexture ( const std::string &ImageFile, const bool Flip = true );
        CrossRenderer::TextureHandle LoadCubemapTexture ( const std::string ImageFile[6], const bool Flip = true );
        CrossRenderer::ShaderHandle LoadShader ( const std::string &VertexFile, const std::string &GeometryFile, const std::string &FragmentFile );

    public:
        TestBase ( void );
        bool Initialize ( const CrossRenderer::RendererBackend NewRendererBackend, const glm::ivec2 &NewWindowPosition, const glm::uvec2 &NewWindowSize );
        bool Shutdown ( void );
        bool GetResult ( void ) const;
        bool RunFrame ( void );
        virtual bool SpecificInitialize ( void ) = 0;
        virtual void SpecificDraw ( void );
        virtual void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event );
        virtual void SpecificImGuiDraw ( void );

    private:
        CrossRenderer::TextureHandle ImGuiFontTexture;
        CrossRenderer::ShaderHandle ImGuiShader;
        CrossRenderer::ShaderAttributeHandle ImGuiPositionShaderAttribute, ImGuiColorShaderAttribute, ImGuiTexCoordShaderAttribute;
        CrossRenderer::ShaderUniformHandle ImGuiMVPMatrixUniform, ImGuiTextureUniform;
        std::vector <CrossRenderer::ShaderBufferHandle> ImGuiBuffers;

        bool InitializeImGui ( void );
        bool ImGuiProcessEvent( const CrossRenderer::WindowEvent& Event );
        void RenderImGui ( void );
    };
