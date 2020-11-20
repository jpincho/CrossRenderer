#include "../TestBase.h"

class SimpleGeometryTest : public TestBase
    {
    public:
        SimpleGeometryTest ( void )
            {
            TestName.assign ( "SimpleGeometry" );
            }
        bool SpecificInitialize ( void )
            {
                {
                CrossRenderer::ShaderBufferHandle TriangleBuffer;
                CrossRenderer::ShaderHandle TriangleShader;
                CrossRenderer::RenderCommand TriangleRenderCommand;

                // Create the triangle
                glm::vec3 TriangleVertices[3] =
                    {
                        {-0.5f, -0.5f, 0.0f},
                        { 0.5f, -0.5f, 0.0f},
                        { 0.0f,  0.5f, 0.0f}
                    };
                TriangleBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( TriangleVertices, sizeof ( TriangleVertices ) ) );

                // Load the shader
                std::string Path ( TEST_SOURCE_LOCATION );
                Path.append ( "SimpleGeometry/" );
                Path.append ( CrossRenderer::Stringify ( RendererBackend ) );

                TriangleShader = LoadShader ( Path + "/Color.vert", "", Path + "/Color.frag" );
                if ( !TriangleShader )
                    return false;

                // Load the texture
                CrossRenderer::TextureHandle Texture;
                if ( LoadTextureFile ( std::string ( TEST_SOURCE_LOCATION ) + std::string ( "Data/WoodCrate/WoodCrate_Diffuse.png" ), Texture ) == false )
                    return false;

                // Configure the draw command
                CrossRenderer::ShaderBufferDataStream PositionStream;
                CrossRenderer::ShaderAttributeHandle PositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TriangleShader, "a_VertexPosition" );
                CrossRenderer::ShaderUniformHandle ColorUniform = CrossRenderer::GetShaderUniformHandle ( TriangleShader, "u_Color" );
                CrossRenderer::ShaderUniformHandle MVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( TriangleShader, "u_MVPMatrix" );

                glm::mat4 MVP;
                MVP = glm::translate ( glm::vec3 ( -0.5f, 0, 0 ) );
                MVP *= glm::scale ( glm::vec3 ( 0.5f, 0.5f, 0.5f ) );

                PositionStream.BufferHandle = TriangleBuffer;
                PositionStream.Stride = sizeof ( glm::vec3 );
                PositionStream.StartOffset = 0;
                PositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
                PositionStream.ComponentsPerElement = 3;

                TriangleRenderCommand.Shader = TriangleShader;
                TriangleRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                            PositionShaderAttribute,
                            PositionStream ) );
                TriangleRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( MVPMatrixUniform, MVP ) );
                TriangleRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( ColorUniform, glm::vec4 ( 0, 1, 0, 1 ) ) );
                TriangleRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
                TriangleRenderCommand.StartVertex = 0;
                TriangleRenderCommand.VertexCount = 3;
                RenderCommands.push_back ( TriangleRenderCommand );
                }

                {
                CrossRenderer::ShaderBufferHandle TexturedQuadBuffer;
                CrossRenderer::ShaderHandle TexturedQuadShader;
                CrossRenderer::RenderCommand TexturedQuadRenderCommand;

                // Create the quad
                typedef struct
                    {
                    glm::vec3 Position;
                    glm::vec2 TexCoord;
                    } Vertex;
                Vertex QuadVertices[4] =
                    {
                        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
                        {{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
                        {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                        {{ 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}
                    };
                TexturedQuadBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( QuadVertices, sizeof ( QuadVertices ) ) );

                // Load the shader
                std::string Path ( TEST_SOURCE_LOCATION );
                Path.append ( "SimpleGeometry/" );
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

                glm::mat4 MVP;
                MVP = glm::translate ( glm::vec3 ( 0.5f, 0, 0 ) );
                MVP *= glm::scale ( glm::vec3 ( 0.5f, 0.5f, 0.5f ) );

                PositionStream.BufferHandle = TexturedQuadBuffer;
                PositionStream.Stride = sizeof ( Vertex );
                PositionStream.StartOffset = offsetof ( Vertex, Position );
                PositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
                PositionStream.ComponentsPerElement = 3;

                TextureStream.BufferHandle = TexturedQuadBuffer;
                TextureStream.Stride = sizeof ( Vertex );
                TextureStream.StartOffset = offsetof ( Vertex, TexCoord );
                TextureStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
                TextureStream.ComponentsPerElement = 2;

                TexturedQuadRenderCommand.Shader = TexturedQuadShader;
                TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                            PositionShaderAttribute,
                            PositionStream ) );
                TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                            TextureCoordShaderAttribute,
                            TextureStream ) );
                TexturedQuadRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TextureUniform, Texture ) );
                TexturedQuadRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( MVPMatrixUniform, MVP ) );
                TexturedQuadRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
                TexturedQuadRenderCommand.StartVertex = 0;
                TexturedQuadRenderCommand.VertexCount = 4;
                RenderCommands.push_back ( TexturedQuadRenderCommand );
                }

            return true;
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        SimpleGeometryTest Test;
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
