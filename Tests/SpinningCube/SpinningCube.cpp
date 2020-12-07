#include "../TestBase.h"

class SpinningCubeTest : public TestBase
    {
    protected:
        float Angle = 0.0f;
        glm::vec3 CubePosition = glm::vec3 ( 0, 0, -3 );
        float CubeTilt = glm::pi<float>() / 6;
        CrossRenderer::ShaderBufferHandle TexturedQuadBuffer;
        CrossRenderer::ShaderHandle TexturedQuadShader;
        CrossRenderer::RenderCommand TexturedQuadRenderCommand;

    public:
        SpinningCubeTest ( void )
            {
            TestName.assign ( "SpinningCube" );
            }

        bool SpecificInitialize ( void )
            {
            // Create the quad
            typedef struct
                {
                glm::vec3 Position;
                glm::vec2 TexCoord;
                } Vertex;

            Vertex QuadVertices[] =
                {
                    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
                    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                    {{ 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}
                };

            TexturedQuadBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( QuadVertices, sizeof ( QuadVertices ) ) );

            // Load the shader
            std::string Path ( TEST_SOURCE_LOCATION );
            Path.append ( "SpinningCube/" );
            Path.append ( CrossRenderer::Stringify ( RendererBackend ) );

            TexturedQuadShader = LoadShader ( Path + "/Texture.vert", "", Path + "/Texture.frag" );
            if ( !TexturedQuadShader )
                return false;

            // Load the texture
            CrossRenderer::TextureHandle Texture;
            if ( LoadTextureFile ( std::string ( TEST_SOURCE_LOCATION ) + std::string ( "Data/WoodCrate/WoodCrate_Diffuse.png" ), Texture ) == false )
                return false;

            // Configure the draw command
            CrossRenderer::ShaderBufferDataStream PositionStream, TextureStream;
            CrossRenderer::ShaderAttributeHandle PositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TexturedQuadShader, "a_VertexPosition" );
            CrossRenderer::ShaderAttributeHandle TextureCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TexturedQuadShader, "a_TexCoord" );
            CrossRenderer::ShaderUniformHandle TextureUniform = CrossRenderer::GetShaderUniformHandle ( TexturedQuadShader, "u_Texture" );
            CrossRenderer::ShaderUniformHandle MVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( TexturedQuadShader, "u_MVPMatrix" );

            PositionStream.BufferHandle = TexturedQuadBuffer;
            PositionStream.Stride = sizeof ( Vertex );
            PositionStream.StartOffset = offsetof ( Vertex, Position );
            PositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            PositionStream.ComponentsPerElement = 3;
            PositionStream.NormalizeData = false;

            TextureStream.BufferHandle = TexturedQuadBuffer;
            TextureStream.Stride = sizeof ( Vertex );
            TextureStream.StartOffset = offsetof ( Vertex, TexCoord );
            TextureStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TextureStream.ComponentsPerElement = 2;
            TextureStream.NormalizeData = false;

            TexturedQuadRenderCommand.Shader = TexturedQuadShader;
            TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        PositionShaderAttribute,
                        PositionStream ) );
            TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TextureCoordShaderAttribute,
                        TextureStream ) );
            TexturedQuadRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TextureUniform, Texture ) );
            TexturedQuadRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( MVPMatrixUniform, glm::mat4 ( 1 ) ) );
            TexturedQuadRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
            TexturedQuadRenderCommand.StartVertex = 0;
            TexturedQuadRenderCommand.VertexCount = 4;
            TexturedQuadRenderCommand.State.Culling.Enabled = true;
            TexturedQuadRenderCommand.State.DepthTest.Enabled = true;
            for ( unsigned cont = 0; cont < 6; ++cont )
                {
                RenderCommands.push_back ( TexturedQuadRenderCommand );
                }
            return true;
            }
        bool RunFrame ( void )
            {
            glm::quat FaceQuaternions[6] =
                {
                glm::angleAxis ( 0.0f, glm::vec3 ( 0.0f, 1.0f, 0.0f ) ),
                glm::angleAxis ( glm::pi<float>() / 2, glm::vec3 ( 0.0f, 1.0f, 0.0f ) ),
                glm::angleAxis ( glm::pi<float>(), glm::vec3 ( 0.0f, 1.0f, 0.0f ) ),
                glm::angleAxis ( 3 * ( glm::pi<float>() / 2 ), glm::vec3 ( 0.0f, 1.0f, 0.0f ) ),
                glm::angleAxis ( glm::pi<float>() / 2, glm::vec3 ( 1.0f, 0.0f, 0.0f ) ),
                glm::angleAxis ( -glm::pi<float>() / 2, glm::vec3 ( 1.0f, 0.0f, 0.0f ) )
                };

            // The test itself
            Angle += ( glm::pi<float>() / 120 );

            glm::mat4 PerspectiveMatrix = glm::perspective ( 90.0f, 1.0f, 0.1f, 100.0f );

            glm::mat4 TranslationMatrix = glm::translate ( glm::mat4 ( 1 ), CubePosition ); // Moves the cube forward
            glm::mat4 FaceTranslationMatrix = glm::translate ( glm::mat4 ( 1 ), glm::vec3 ( 0, 0, 1 ) );
            glm::mat4 TiltMatrix = glm::rotate ( TranslationMatrix, CubeTilt, glm::vec3 ( 1.0f, 0.0f, 0.0f ) ); // Tilts the cube forward a little

            glm::quat RotationQuat = glm::angleAxis ( Angle, glm::vec3 ( 0.0f, 1.0f, 0.0f ) );

            // Go through all renderers, all windows, and update
            for ( unsigned cont = 0; cont < 6; ++cont )
                {
                glm::mat4 RotationMatrix = glm::toMat4 ( RotationQuat * FaceQuaternions[cont] );// Rotates the face + the cube rotation
                RenderCommands[cont].UniformValues[0].UniformValue.Matrix4Value = PerspectiveMatrix * TiltMatrix * RotationMatrix * FaceTranslationMatrix; // MVP
                }
            return TestBase::RunFrame();
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        SpinningCubeTest Test;
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
