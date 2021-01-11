#include "../TestBase.h"

class LitCrateTest : public TestBase
    {
    protected:
        uint32_t FrameCount = 0;
        bool ShouldMoveLight = true, ShouldRotateCube = false;
        MovableObject Crate;
        CrossRenderer::ShaderBufferHandle CrateBuffer, CrateIndexBuffer, Light0Buffer, Light1Buffer;
        CrossRenderer::ShaderHandle CrateShader;
        struct
            {
            CrossRenderer::ShaderUniformHandle ShaderMVP, ModelMatrix, ModelTransposeInverseMatrix;
            CrossRenderer::ShaderUniformHandle ViewPosition;
            CrossRenderer::ShaderUniformHandle MaterialShininess, MaterialDiffuseTexture, MaterialSpecularTexture;
            CrossRenderer::ShaderUniformHandle PointLight[2];
            } Uniforms;
        struct
            {
            glm::vec4 Position;
            glm::vec4 Diffuse, Specular;
            float Intensity;
            float ConstantAttenuation;
            float LinearAttenuation;
            float ExponentialAttenuation;
            } LightData[2];

    public:
        void Reset ( void )
            {
            Crate.SetPosition ( glm::vec3 ( 0, 0, 0 ) );
            Crate.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );

            LightData[0].Position = glm::vec4 ( -5, 5, 3, 1 );
            LightData[0].Diffuse = glm::vec4 ( 1, 1, 1, 1 );
            LightData[0].Specular = glm::vec4 ( 1, 1, 1, 1 );
            LightData[0].Intensity = 5.0f;
            LightData[0].ConstantAttenuation = 1.0f;
            LightData[0].LinearAttenuation = 0.5f;
            LightData[0].ExponentialAttenuation = 0.01f;

            LightData[1].Position = glm::vec4 ( 5, 5, 3, 1 );
            LightData[1].Diffuse = glm::vec4 ( 0, 0, 0, 1 );
            LightData[1].Specular = glm::vec4 ( 0, 0, 0, 1 );
            LightData[1].Intensity = 5.0f;
            LightData[1].ConstantAttenuation = 1.0f;
            LightData[1].LinearAttenuation = 0.5f;
            LightData[1].ExponentialAttenuation = 0.01f;
            }

        bool SpecificInitialize ( void )
            {
            // Create the quad
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

            uint16_t CrateIndices[] = {0, 1, 2,
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
            CrateBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( CrateVertices, sizeof ( CrateVertices ) ) );
            CrateIndexBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( CrateIndices, sizeof ( CrateIndices ) ) );
            Light0Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            Light1Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            // Load the shader
            CrateShader = LoadShader ( "Crate.vert", "", "Crate.frag" );
            if ( !CrateShader )
                return false;

            // Load the texture
            CrossRenderer::TextureHandle DiffuseTexture;
            if ( ! ( DiffuseTexture = LoadTexture ( std::string ( "WoodCrate/WoodCrate_Diffuse.png" ) ) ) )
                return false;
            CrossRenderer::TextureHandle SpecularTexture;
            if ( ! ( SpecularTexture = LoadTexture ( std::string ( "WoodCrate/WoodCrate_Specular.png" ) ) ) )
                return false;

            // Configure the draw command
            CrossRenderer::ShaderBufferDataStream PositionStream, NormalStream, TextureStream, IndexStream;
            CrossRenderer::ShaderAttributeHandle PositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( CrateShader, "a_Position" );
            CrossRenderer::ShaderAttributeHandle NormalShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( CrateShader, "a_Normal" );
            CrossRenderer::ShaderAttributeHandle TextureCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( CrateShader, "a_TexCoord" );

#define GET_UNIFORM(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderUniformHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Uniform %s not found", NAME );\
                    return false;\
                    }
            GET_UNIFORM ( Uniforms.ShaderMVP, CrateShader, "u_MVPMatrix" );
            GET_UNIFORM ( Uniforms.ViewPosition, CrateShader, "u_ViewPosition" );
            GET_UNIFORM ( Uniforms.MaterialShininess, CrateShader, "u_Shininess" );
            GET_UNIFORM ( Uniforms.MaterialDiffuseTexture, CrateShader, "u_DiffuseTexture" );
            GET_UNIFORM ( Uniforms.MaterialSpecularTexture, CrateShader, "u_SpecularTexture" );
            GET_UNIFORM ( Uniforms.ModelMatrix, CrateShader, "u_ModelMatrix" );
            GET_UNIFORM ( Uniforms.ModelTransposeInverseMatrix, CrateShader, "u_ModelTransposeInverse" );

            GET_UNIFORM ( Uniforms.PointLight[0], CrateShader, "u_PointLightBlock[0]" );
            GET_UNIFORM ( Uniforms.PointLight[1], CrateShader, "u_PointLightBlock[1]" );

            PositionStream.BufferHandle = CrateBuffer;
            PositionStream.Stride = sizeof ( Vertex );
            PositionStream.StartOffset = offsetof ( Vertex, Position );
            PositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            PositionStream.ComponentsPerElement = 3;
            PositionStream.NormalizeData = false;

            NormalStream.BufferHandle = CrateBuffer;
            NormalStream.Stride = sizeof ( Vertex );
            NormalStream.StartOffset = offsetof ( Vertex, Normal );
            NormalStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            NormalStream.ComponentsPerElement = 3;
            NormalStream.NormalizeData = true;

            TextureStream.BufferHandle = CrateBuffer;
            TextureStream.Stride = sizeof ( Vertex );
            TextureStream.StartOffset = offsetof ( Vertex, TexCoord );
            TextureStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TextureStream.ComponentsPerElement = 2;
            TextureStream.NormalizeData = false;

            IndexStream.BufferHandle = CrateIndexBuffer;
            IndexStream.Stride = sizeof ( uint16_t );
            IndexStream.StartOffset = 0;
            IndexStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Unsigned16;
            IndexStream.ComponentsPerElement = 1;

            CrossRenderer::RenderCommand CrateRenderCommand;

            CrateRenderCommand.Shader = CrateShader;
            CrateRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        PositionShaderAttribute,
                        PositionStream ) );
            CrateRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        NormalShaderAttribute,
                        NormalStream ) );
            CrateRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TextureCoordShaderAttribute,
                        TextureStream ) );

            CrateRenderCommand.IndexBufferStream = IndexStream;
            CrateRenderCommand.IndexBuffer = CrateIndexBuffer;

            CrateRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                        Uniforms.MaterialDiffuseTexture,
                        CrossRenderer::TextureBindSettings ( DiffuseTexture ) ) );
            CrateRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                        Uniforms.MaterialSpecularTexture,
                        CrossRenderer::TextureBindSettings ( SpecularTexture ) ) );

            CrateRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
            CrateRenderCommand.StartVertex = 0;
            CrateRenderCommand.VertexCount = sizeof ( CrateIndices ) / sizeof ( uint16_t );
            CrateRenderCommand.State.Culling.Enabled = true;
            CrateRenderCommand.State.DepthTest.Enabled = true;
            RenderCommands.push_back ( CrateRenderCommand );

            Reset();
            return true;
            }
        void SpecificDraw ( void )
            {
            ++FrameCount;
            const float TimeDelta = 1.0f / 60;
            CrossRenderer::ChangeShaderBufferContents ( Light0Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            CrossRenderer::ChangeShaderBufferContents ( Light1Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            // The test itself
            if ( ShouldRotateCube )
                {
                Crate.Rotate ( glm::vec3 ( 0, 1, 0 ), glm::pi<float>() / 120 );
                }
            if ( ShouldMoveLight )
                {
                LightData[0].Position.x = 5 * sin ( FrameCount * ( glm::pi<float>() / 120 ) );
                }

            // Go through all renderers, all windows, and update
            RenderCommands[0].UniformValues.clear();

            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.ShaderMVP, SceneCamera.GetViewProjectionMatrix() * Crate.GetTransformationMatrix() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.ModelMatrix, Crate.GetTransformationMatrix() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.ModelTransposeInverseMatrix, glm::inverse ( glm::transpose ( Crate.GetTransformationMatrix() ) ) ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.ViewPosition, SceneCamera.GetPosition() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.MaterialShininess, 8.0f ) );

            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.PointLight[0], Light0Buffer ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.PointLight[1], Light1Buffer ) );
            CrossRenderer::SanityCheckRenderCommand ( RenderCommands[0] );
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        LitCrateTest Test;
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
