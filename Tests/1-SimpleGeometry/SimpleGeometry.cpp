#include "../TestBase.h"

class SimpleGeometryTest : public TestBase
    {
    public:
        bool SpecificInitialize ( void )
            {
            CrossRenderer::ShaderBufferHandle TriangleBuffer, TexturedQuadBuffer;
            CrossRenderer::ShaderHandle TriangleShader, TexturedQuadShader;
            CrossRenderer::RenderCommand TriangleRenderCommand, TexturedQuadRenderCommand;

            // Create the shapes
            typedef struct
                {
                glm::vec3 Position;
                glm::vec2 TexCoord;
                glm::vec3 Color;
                } Vertex;
            Vertex TriangleVertices[3] =
                {
                    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                    {{ 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
                };
            TriangleBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( TriangleVertices, sizeof ( TriangleVertices ) ) );

            Vertex QuadVertices[4] =
                {
                    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                    {{ 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                    {{ 1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}
                };
            TexturedQuadBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( QuadVertices, sizeof ( QuadVertices ) ) );

            // Load the shaders
            TriangleShader = LoadShader ( "Triangle.vert", "", "Triangle.frag" );
            if ( !TriangleShader )
                return false;
            TexturedQuadShader = LoadShader ( "TexturedQuad.vert", "", "TexturedQuad.frag" );
            if ( !TexturedQuadShader )
                return false;

            // Load the texture
            CrossRenderer::TextureHandle Texture;
            if ( ! ( Texture = LoadTexture ( std::string ( "WoodCrate/WoodCrate_Diffuse.png" ) ) ) )
                return false;

            // Configure the draw commands
            CrossRenderer::ShaderBufferDataStream TrianglePositionStream, TriangleColorStream;
            CrossRenderer::ShaderAttributeHandle TrianglePositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TriangleShader, "a_Position" );
            CrossRenderer::ShaderUniformHandle TriangleColorShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TriangleShader, "a_Color" );
            CrossRenderer::ShaderUniformHandle TriangleMVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( TriangleShader, "u_MVPMatrix" );

            glm::mat4 MVP;
            MVP = glm::translate ( glm::vec3 ( -0.5f, 0, 0 ) );
            MVP *= glm::scale ( glm::vec3 ( 0.5f, 0.5f, 0.5f ) );

            TrianglePositionStream.BufferHandle = TriangleBuffer;
            TrianglePositionStream.Stride = sizeof ( Vertex );
            TrianglePositionStream.StartOffset = offsetof ( Vertex, Position );
            TrianglePositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TrianglePositionStream.ComponentsPerElement = 3;
            TrianglePositionStream.NormalizeData = false;

            TriangleColorStream.BufferHandle = TriangleBuffer;
            TriangleColorStream.Stride = sizeof ( Vertex );
            TriangleColorStream.StartOffset = offsetof ( Vertex, Color );
            TriangleColorStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TriangleColorStream.ComponentsPerElement = 3;
            TriangleColorStream.NormalizeData = false;

            TriangleRenderCommand.Shader = TriangleShader;
            TriangleRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TrianglePositionShaderAttribute,
                        TrianglePositionStream ) );
            TriangleRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TriangleColorShaderAttribute,
                        TriangleColorStream ) );
            TriangleRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( TriangleMVPMatrixUniform, MVP ) );
            TriangleRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
            TriangleRenderCommand.StartVertex = 0;
            TriangleRenderCommand.VertexCount = 3;
            RenderCommands.push_back ( TriangleRenderCommand );

            CrossRenderer::ShaderBufferDataStream TexturedQuadPositionStream, TexturedQuadTextureStream;
            CrossRenderer::ShaderAttributeHandle TexturedQuadPositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TexturedQuadShader, "a_Position" );
            CrossRenderer::ShaderAttributeHandle TexturedQuadTextureCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( TexturedQuadShader, "a_TexCoord" );
            CrossRenderer::ShaderUniformHandle TexturedQuadTextureUniform = CrossRenderer::GetShaderUniformHandle ( TexturedQuadShader, "u_Texture" );
            CrossRenderer::ShaderUniformHandle TexturedQuadMVPMatrixUniform = CrossRenderer::GetShaderUniformHandle ( TexturedQuadShader, "u_MVPMatrix" );

            MVP = glm::translate ( glm::vec3 ( 0.5f, 0, 0 ) );
            MVP *= glm::scale ( glm::vec3 ( 0.5f, 0.5f, 0.5f ) );

            TexturedQuadPositionStream.BufferHandle = TexturedQuadBuffer;
            TexturedQuadPositionStream.Stride = sizeof ( Vertex );
            TexturedQuadPositionStream.StartOffset = offsetof ( Vertex, Position );
            TexturedQuadPositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TexturedQuadPositionStream.ComponentsPerElement = 3;
            TexturedQuadPositionStream.NormalizeData = false;

            TexturedQuadTextureStream.BufferHandle = TexturedQuadBuffer;
            TexturedQuadTextureStream.Stride = sizeof ( Vertex );
            TexturedQuadTextureStream.StartOffset = offsetof ( Vertex, TexCoord );
            TexturedQuadTextureStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TexturedQuadTextureStream.ComponentsPerElement = 2;
            TexturedQuadTextureStream.NormalizeData = false;

            TexturedQuadRenderCommand.Shader = TexturedQuadShader;
            TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TexturedQuadPositionShaderAttribute,
                        TexturedQuadPositionStream ) );
            TexturedQuadRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TexturedQuadTextureCoordShaderAttribute,
                        TexturedQuadTextureStream ) );
            TexturedQuadRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( TexturedQuadTextureUniform, Texture ) );
            TexturedQuadRenderCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( TexturedQuadMVPMatrixUniform, MVP ) );
            TexturedQuadRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleStrip;
            TexturedQuadRenderCommand.StartVertex = 0;
            TexturedQuadRenderCommand.VertexCount = 4;
            RenderCommands.push_back ( TexturedQuadRenderCommand );

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
