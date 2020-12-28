#include "../TestBase.h"
#include "../CameraInput.h"

class SkyboxTest : public TestBase
    {
    protected:
        Camera SceneCamera;
        CameraInput CameraController;
        uint32_t FrameCount = 0;

        struct
            {
            CrossRenderer::RenderCommand RenderCommand;
            CrossRenderer::ShaderBufferHandle VertexBuffer;
            CrossRenderer::ShaderHandle Shader;
            struct
                {
                CrossRenderer::ShaderUniformHandle ViewMatrix, ProjectionMatrix, Texture;
                } Uniforms;
            } SkyboxData;

        std::vector <std::pair <std::string, CrossRenderer::TextureHandle>> Skyboxes;
        int ActiveSkyboxIndex;

        struct
            {
            uint32_t Reset;
            } Keys;

    public:
        void Reset ( void )
            {
            SceneCamera.SetPosition ( glm::vec3 ( 0.0f, 0.0f, 3.0f ) );
            SceneCamera.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );
            ActiveSkyboxIndex = 0;
            }

        void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
            {
            if ( ( Event.EventType == CrossRenderer::WindowEventType::KeyReleased ) && ( Event.EventData.KeyReleased.Key == Keys.Reset ) )
                {
                Reset();
                }
            }

        bool CreateSkybox ( void )
            {
            // Initialize the skybox
            glm::vec3 SkyboxVertices[] =
                {
                // positions
                    {-1.0f,  1.0f, -1.0f},
                    {-1.0f, -1.0f, -1.0f},
                    { 1.0f, -1.0f, -1.0f},
                    { 1.0f, -1.0f, -1.0f},
                    { 1.0f,  1.0f, -1.0f},
                    {-1.0f,  1.0f, -1.0f},

                    {-1.0f, -1.0f,  1.0f},
                    {-1.0f, -1.0f, -1.0f},
                    {-1.0f,  1.0f, -1.0f},
                    {-1.0f,  1.0f, -1.0f},
                    {-1.0f,  1.0f,  1.0f},
                    {-1.0f, -1.0f,  1.0f},

                    { 1.0f, -1.0f, -1.0f},
                    { 1.0f, -1.0f,  1.0f},
                    { 1.0f,  1.0f,  1.0f},
                    { 1.0f,  1.0f,  1.0f},
                    { 1.0f,  1.0f, -1.0f},
                    { 1.0f, -1.0f, -1.0f},

                    {-1.0f, -1.0f,  1.0f},
                    {-1.0f,  1.0f,  1.0f},
                    { 1.0f,  1.0f,  1.0f},
                    { 1.0f,  1.0f,  1.0f},
                    { 1.0f, -1.0f,  1.0f},
                    {-1.0f, -1.0f,  1.0f},

                    {-1.0f,  1.0f, -1.0f},
                    { 1.0f,  1.0f, -1.0f},
                    { 1.0f,  1.0f,  1.0f},
                    { 1.0f,  1.0f,  1.0f},
                    {-1.0f,  1.0f,  1.0f},
                    {-1.0f,  1.0f, -1.0f},

                    {-1.0f, -1.0f, -1.0f},
                    {-1.0f, -1.0f,  1.0f},
                    { 1.0f, -1.0f, -1.0f},
                    { 1.0f, -1.0f, -1.0f},
                    {-1.0f, -1.0f,  1.0f},
                    { 1.0f, -1.0f,  1.0f}
                };

            SkyboxData.VertexBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( SkyboxVertices, sizeof ( SkyboxVertices ) ) );

            // Load the shader
            SkyboxData.Shader = LoadShader ( "Skybox.vert", "", "Skybox.frag" );
            if ( !SkyboxData.Shader )
                return false;

            // Load the skyboxes
            ActiveSkyboxIndex = 0;
            LoadSkybox ( "default", "Skybox/default" );
            LoadSkybox ( "Storforsen", "Skybox/Storforsen" );
            LoadSkybox ( "Yokohama3", "Skybox/Yokohama3" );
            LoadSkybox ( "Lycksele3", "Skybox/Lycksele3" );
            if ( Skyboxes.size() == 0 )
                return false;
            // Configure the draw command
            CrossRenderer::ShaderBufferDataStream PositionStream;
            CrossRenderer::ShaderAttributeHandle PositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( SkyboxData.Shader, "a_Position" );

#define GET_UNIFORM(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderUniformHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Uniform %s not found", NAME );\
                    return false;\
                    }
            GET_UNIFORM ( SkyboxData.Uniforms.ProjectionMatrix, SkyboxData.Shader, "u_ProjectionMatrix" );
            GET_UNIFORM ( SkyboxData.Uniforms.ViewMatrix, SkyboxData.Shader, "u_ViewMatrix" );
            GET_UNIFORM ( SkyboxData.Uniforms.Texture, SkyboxData.Shader, "u_SkyboxTexture" );
#undef GET_UNIFORM
            PositionStream.BufferHandle = SkyboxData.VertexBuffer;
            PositionStream.Stride = sizeof ( glm::vec3 );
            PositionStream.StartOffset = 0;
            PositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            PositionStream.ComponentsPerElement = 3;
            PositionStream.NormalizeData = false;

            SkyboxData.RenderCommand.Shader = SkyboxData.Shader;
            SkyboxData.RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        PositionShaderAttribute,
                        PositionStream ) );

            SkyboxData.RenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                        SkyboxData.Uniforms.Texture,
                        CrossRenderer::TextureBindSettings ( Skyboxes[ActiveSkyboxIndex].second ) ) );

            SkyboxData.RenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
            SkyboxData.RenderCommand.StartVertex = 0;
            SkyboxData.RenderCommand.VertexCount = sizeof ( SkyboxVertices ) / sizeof ( glm::vec3 );
            //SkyboxData.RenderCommand.State.Culling.Enabled = true;
            SkyboxData.RenderCommand.State.DepthTest.Enabled = true;
            SkyboxData.RenderCommand.State.DepthTest.Mode = CrossRenderer::DepthTestMode::LessOrEqual;

            return true;
            }
        bool LoadSkybox ( const char* Name, const char* Path )
            {
            std::string Textures[6] =
                {
                std::string ( Path ) + std::string ( "/posx.jpg" ),
                std::string ( Path ) + std::string ( "/negx.jpg" ),
                std::string ( Path ) + std::string ( "/posy.jpg" ),
                std::string ( Path ) + std::string ( "/negy.jpg" ),
                std::string ( Path ) + std::string ( "/posz.jpg" ),
                std::string ( Path ) + std::string ( "/negz.jpg" )
                };
            CrossRenderer::TextureHandle CubemapTexture;
            if ( ! ( CubemapTexture = LoadCubemapTexture ( Textures, false ) ) )
                return false;
            Skyboxes.push_back ( std::pair <std::string, CrossRenderer::TextureHandle> ( Name, CubemapTexture ) );
            return true;
            }

        bool SpecificInitialize ( void )
            {
            CameraController.Initialize ();
            CameraController.SetCamera ( &SceneCamera );

            if ( CreateSkybox() == false )
                return false;

            Camera::PerspectiveParameters NewParameters;
            NewParameters.AspectRatio = 1.0f;
            NewParameters.Far = 1000;
            NewParameters.Near = 1.0;
            NewParameters.FOV = glm::pi<float>() / 2;
            SceneCamera.SetPerspectiveParameters ( NewParameters );
            Keys.Reset = CrossRenderer::WindowManager::GetKeyCode ( "r" );
            Reset();
            return true;
            }
        void SpecificImGuiDraw ( void )
            {
            ImGui::Begin ( "Options" );
            std::vector <const char *> SkyboxNames;
            for ( unsigned Index = 0; Index < Skyboxes.size(); ++Index )
                SkyboxNames.push_back ( Skyboxes[Index].first.c_str() );
            ImGui::Combo ( "Active skybox", &ActiveSkyboxIndex, SkyboxNames.data(), SkyboxNames.size() );
            ImGui::End();
            }
        void SpecificDraw ( void )
            {
            ++FrameCount;
            const float TimeDelta = 1.0f / 60;
            CameraController.Update ( TimeDelta );

            RenderCommands.clear();
            RenderCommands.push_back ( SkyboxData.RenderCommand );

            RenderCommands[0].TextureBindings[0].BindSettings.Handle = Skyboxes[ActiveSkyboxIndex].second;
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( SkyboxData.Uniforms.ProjectionMatrix, SceneCamera.GetProjectionMatrix() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( SkyboxData.Uniforms.ViewMatrix, glm::mat4 ( glm::mat3 ( SceneCamera.GetViewMatrix() ) ) ) );
            CrossRenderer::SanityCheckRenderCommand ( RenderCommands[0] );
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        SkyboxTest Test;
        if ( Test.Initialize ( RenderersToTest[index], glm::ivec2 ( 0, 0 ), glm::uvec2 ( 800, 600 ) ) == false )
            return -1;
        bool ShouldQuit = false;
        while ( !ShouldQuit )
            {
            if ( Test.RunFrame() == false )
                ShouldQuit = true;
            }
        Test.Shutdown();
        }

    return 0;
    }
