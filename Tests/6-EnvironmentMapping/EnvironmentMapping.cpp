#include "../TestBase.h"
#include "../CameraInput.h"

class EnvironmentMappingTest : public TestBase
    {
    protected:
        Camera SceneCamera;
        CameraInput CameraController;

        struct
            {
            MovableObject Crate;
            CrossRenderer::RenderCommand RenderCommand;
            CrossRenderer::ShaderBufferHandle VertexBuffer, IndexBuffer;
            CrossRenderer::ShaderHandle Shader;
            struct
                {
                CrossRenderer::ShaderUniformHandle MVP, ModelMatrix, ModelTransposeInverseMatrix;
                CrossRenderer::ShaderUniformHandle ViewPosition, UseRefraction, RefractionFactor, Texture;
                } Uniforms;
            } CrateData;

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
        bool UseRefraction;
        float RefractionFactor;

    public:
        void Reset ( void )
            {
            SceneCamera.SetPosition ( glm::vec3 ( 0.0f, 0.0f, 3.0f ) );
            SceneCamera.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );

            CrateData.Crate.SetPosition ( glm::vec3 ( 0, 0, 0 ) );
            CrateData.Crate.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );
            UseRefraction = false;
            RefractionFactor = 1.0f / 1.52f;
            ActiveSkyboxIndex = 0;
            }

        void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
            {
            if ( ( Event.EventType == CrossRenderer::WindowEventType::KeyReleased ) && ( Event.EventData.KeyReleased.Key == Keys.Reset ) )
                {
                Reset();
                }
            }
        bool CreateCrate ( void )
            {
            typedef struct
                {
                glm::vec3 Position;
                glm::vec3 Normal;
                glm::vec2 TexCoord;
                } Vertex;

            Vertex CrateVertices[] =
                {
                // Front
                    {{-1.0f, -1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
                    {{-1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                    {{ 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                // Right
                    {{ 1.0f, -1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                    {{ 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                    {{ 1.0f, 1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                // Back
                    {{ 1.0f, -1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
                    {{-1.0f, -1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
                    {{ 1.0f, 1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
                    {{-1.0f, 1.0f, -1.0f}, { 0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
                // Left
                    {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                    {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                    {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                    {{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
                // Top
                    {{-1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                    {{ 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{-1.0f, 1.0f, -1.0f}, { 0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                    {{ 1.0f, 1.0f, -1.0f}, { 0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
                // Bottom
                    {{-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{-1.0f, -1.0f, 1.0f}, { 0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
                    {{ 1.0f, -1.0f, 1.0f}, { 0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
                };

            uint16_t CrateIndices[] = { 0, 1, 2,
                                        1, 3, 2,

                                        4, 5, 6,
                                        5, 7, 6,

                                        8, 9, 10,
                                        9, 11, 10,

                                        12, 13, 14,
                                        13, 15, 14,

                                        16, 17, 18,
                                        17, 19, 18,

                                        20, 21, 22,
                                        21, 23, 22
                                      };
            CrateData.VertexBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( CrateVertices, sizeof ( CrateVertices ) ) );
            CrateData.IndexBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( CrateIndices, sizeof ( CrateIndices ) ) );

            // Load the shader
            CrateData.Shader = LoadShader ( "Crate.vert", "", "Crate.frag" );
            if ( !CrateData.Shader )
                return false;

            // Configure the draw command
            CrossRenderer::ShaderBufferDataStream PositionStream, NormalStream, IndexStream;
            CrossRenderer::ShaderAttributeHandle PositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( CrateData.Shader, "a_Position" );
            CrossRenderer::ShaderAttributeHandle NormalShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( CrateData.Shader, "a_Normal" );

#define GET_UNIFORM(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderUniformHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Uniform %s not found", NAME );\
                    return false;\
                    }
            GET_UNIFORM ( CrateData.Uniforms.MVP, CrateData.Shader, "u_MVPMatrix" );
            GET_UNIFORM ( CrateData.Uniforms.ViewPosition, CrateData.Shader, "u_ViewPosition" );
            GET_UNIFORM ( CrateData.Uniforms.Texture, CrateData.Shader, "u_SkyboxTexture" );
            GET_UNIFORM ( CrateData.Uniforms.ModelMatrix, CrateData.Shader, "u_ModelMatrix" );
            GET_UNIFORM ( CrateData.Uniforms.ModelTransposeInverseMatrix, CrateData.Shader, "u_ModelTransposeInverse" );
            GET_UNIFORM ( CrateData.Uniforms.UseRefraction, CrateData.Shader, "u_UseRefraction" );
            GET_UNIFORM ( CrateData.Uniforms.RefractionFactor, CrateData.Shader, "u_RefractionFactor" );
#undef GET_UNIFORM
            PositionStream.BufferHandle = CrateData.VertexBuffer;
            PositionStream.Stride = sizeof ( Vertex );
            PositionStream.StartOffset = offsetof ( Vertex, Position );
            PositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            PositionStream.ComponentsPerElement = 3;
            PositionStream.NormalizeData = false;

            NormalStream.BufferHandle = CrateData.VertexBuffer;
            NormalStream.Stride = sizeof ( Vertex );
            NormalStream.StartOffset = offsetof ( Vertex, Normal );
            NormalStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            NormalStream.ComponentsPerElement = 3;
            NormalStream.NormalizeData = true;

            IndexStream.BufferHandle = CrateData.IndexBuffer;
            IndexStream.Stride = sizeof ( uint16_t );
            IndexStream.StartOffset = 0;
            IndexStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Unsigned16;
            IndexStream.ComponentsPerElement = 1;

            CrateData.RenderCommand.Shader = CrateData.Shader;
            CrateData.RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        PositionShaderAttribute,
                        PositionStream ) );
            CrateData.RenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        NormalShaderAttribute,
                        NormalStream ) );

            CrateData.RenderCommand.IndexBufferStream = IndexStream;
            CrateData.RenderCommand.IndexBuffer = CrateData.IndexBuffer;

            CrateData.RenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                        CrateData.Uniforms.Texture,
                        CrossRenderer::TextureBindSettings ( Skyboxes[ActiveSkyboxIndex].second ) ) );

            CrateData.RenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
            CrateData.RenderCommand.StartVertex = 0;
            CrateData.RenderCommand.VertexCount = sizeof ( CrateIndices ) / sizeof ( uint16_t );
            CrateData.RenderCommand.State.Culling.Enabled = true;
            CrateData.RenderCommand.State.DepthTest.Enabled = true;
            return true;
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
            if ( CreateCrate() == false )
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
            ImGui::Checkbox ( "Use refraction", &UseRefraction );
            if ( UseRefraction )
                ImGui::InputFloat ( "Refraction factor", &RefractionFactor );

            std::vector <const char *> SkyboxNames;
            for ( unsigned Index = 0; Index < Skyboxes.size(); ++Index )
                SkyboxNames.push_back ( Skyboxes[Index].first.c_str() );
            ImGui::Combo ( "Active skybox", &ActiveSkyboxIndex, SkyboxNames.data(), SkyboxNames.size() );
            ImGui::End();
            }
        void SpecificDraw ( void )
            {
            const float TimeDelta = 1.0f / 60;
            CameraController.Update ( TimeDelta );

            RenderCommands.clear();
            RenderCommands.push_back ( CrateData.RenderCommand );
            RenderCommands.push_back ( SkyboxData.RenderCommand );

            RenderCommands[0].TextureBindings[0].BindSettings.Handle = Skyboxes[ActiveSkyboxIndex].second;
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( CrateData.Uniforms.MVP, SceneCamera.GetViewProjectionMatrix() * CrateData.Crate.GetTransformationMatrix() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( CrateData.Uniforms.ModelMatrix, CrateData.Crate.GetTransformationMatrix() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( CrateData.Uniforms.ModelTransposeInverseMatrix, glm::inverse ( glm::transpose ( CrateData.Crate.GetTransformationMatrix() ) ) ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( CrateData.Uniforms.ViewPosition, SceneCamera.GetPosition() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( CrateData.Uniforms.UseRefraction, ( int ) UseRefraction ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( CrateData.Uniforms.RefractionFactor, RefractionFactor ) );
            CrossRenderer::SanityCheckRenderCommand ( RenderCommands[0] );

            RenderCommands[1].TextureBindings[0].BindSettings.Handle = Skyboxes[ActiveSkyboxIndex].second;
            RenderCommands[1].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( SkyboxData.Uniforms.ProjectionMatrix, SceneCamera.GetProjectionMatrix() ) );
            RenderCommands[1].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( SkyboxData.Uniforms.ViewMatrix, glm::mat4 ( glm::mat3 ( SceneCamera.GetViewMatrix() ) ) ) );
            CrossRenderer::SanityCheckRenderCommand ( RenderCommands[1] );
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        EnvironmentMappingTest Test;
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
