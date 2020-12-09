#include "../TestBase.h"
#include <imgui/imgui.h>
#include "../Camera.h"

class LitCubeTest : public TestBase
    {
    protected:
        Camera SceneCamera;
        uint32_t FrameCount = 0;
        bool ShouldMoveLight = true, ShouldRotateCube = false;
        MovableObject Box;
        CrossRenderer::ShaderBufferHandle BoxBuffer, BoxIndexBuffer, Light0Buffer, Light1Buffer;
        CrossRenderer::ShaderHandle BoxShader;
        struct
            {
            CrossRenderer::ShaderUniformHandle ShaderMVP, ModelMatrix, ModelTransposeInverseMatrix;
            CrossRenderer::ShaderUniformHandle ViewPosition;
            CrossRenderer::ShaderUniformHandle MaterialShininess, MaterialDiffuseTexture, MaterialSpecularTexture;
            CrossRenderer::ShaderUniformHandle PointLight[2];
            } Uniforms;
        struct
            {
            struct
                {
                uint32_t Reset, MovePosX, MoveNegX, MovePosY, MoveNegY, MovePosZ, MoveNegZ, RotatePosX, RotateNegX, RotatePosY, RotateNegY, RotatePosZ, RotateNegZ;
                } Keys;
            float MovementSpeed = 2.0f, RotationSpeed = 2.0f;
            glm::vec3 Movement, Rotation;
            } CameraVariables;

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
        LitCubeTest ( void )
            {
            TestName.assign ( "LitCube" );
            }
        void Reset ( void )
            {
            SceneCamera.SetPosition ( glm::vec3 ( 0.0f, 0.0f, 3.0f ) );
            SceneCamera.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );
            CameraVariables.Movement = glm::vec3 ( 0 );
            CameraVariables.Rotation = glm::vec3 ( 0 );
            Box.SetPosition ( glm::vec3 ( 0, 0, 0 ) );
            Box.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );

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

        void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
            {
            if ( ( Event.EventType == CrossRenderer::WindowEventType::KeyReleased ) && ( Event.EventData.KeyReleased.Key == CameraVariables.Keys.Reset ) )
                {
                Reset();
                }
            if ( ( Event.EventType == CrossRenderer::WindowEventType::KeyPressed ) || ( Event.EventType == CrossRenderer::WindowEventType::KeyReleased ) )
                {
                int Delta = ( Event.EventType == CrossRenderer::WindowEventType::KeyPressed ? 1 : -1 );
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.MovePosX )
                    CameraVariables.Movement.x += Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.MoveNegX )
                    CameraVariables.Movement.x -= Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.MovePosY )
                    CameraVariables.Movement.y += Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.MoveNegY )
                    CameraVariables.Movement.y -= Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.MovePosZ )
                    CameraVariables.Movement.z += Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.MoveNegZ )
                    CameraVariables.Movement.z -= Delta;

                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.RotatePosX )
                    CameraVariables.Rotation.x += Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.RotateNegX )
                    CameraVariables.Rotation.x -= Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.RotatePosY )
                    CameraVariables.Rotation.y += Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.RotateNegY )
                    CameraVariables.Rotation.y -= Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.RotatePosZ )
                    CameraVariables.Rotation.z += Delta;
                if ( Event.EventData.KeyPressed.Key == CameraVariables.Keys.RotateNegZ )
                    CameraVariables.Rotation.z -= Delta;
                }
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

            Vertex CubeVertices[] =
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

            uint16_t BoxIndices[] = {0, 1, 2,
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
            BoxBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( CubeVertices, sizeof ( CubeVertices ) ) );
            BoxIndexBuffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( BoxIndices, sizeof ( BoxIndices ) ) );
            Light0Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            Light1Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            // Load the shader
            std::string Path ( TEST_SOURCE_LOCATION );
            Path.append ( "LitCube/" );
            Path.append ( CrossRenderer::Stringify ( RendererBackend ) );

            BoxShader = LoadShader ( Path + "/Box.vert", "", Path + "/Box.frag" );
            if ( !BoxShader )
                return false;

            // Load the texture
            CrossRenderer::TextureHandle DiffuseTexture;
            if ( LoadTextureFile ( std::string ( TEST_SOURCE_LOCATION ) + std::string ( "Data/WoodCrate/WoodCrate_Diffuse.png" ), DiffuseTexture ) == false )
                return false;
            CrossRenderer::TextureHandle SpecularTexture;
            if ( LoadTextureFile ( std::string ( TEST_SOURCE_LOCATION ) + std::string ( "Data/WoodCrate/WoodCrate_Specular.png" ), SpecularTexture ) == false )
                return false;

            // Configure the draw command
            CrossRenderer::ShaderBufferDataStream PositionStream, NormalStream, TextureStream, IndexStream;
            CrossRenderer::ShaderAttributeHandle PositionShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( BoxShader, "a_VertexPosition" );
            CrossRenderer::ShaderAttributeHandle NormalShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( BoxShader, "a_Normal" );
            CrossRenderer::ShaderAttributeHandle TextureCoordShaderAttribute = CrossRenderer::GetShaderAttributeHandle ( BoxShader, "a_TexCoord" );

#define GET_UNIFORM(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderUniformHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Uniform %s not found", NAME );\
                    return false;\
                    }
            GET_UNIFORM ( Uniforms.ShaderMVP, BoxShader, "u_MVPMatrix" );
            GET_UNIFORM ( Uniforms.ViewPosition, BoxShader, "u_ViewPosition" );
            GET_UNIFORM ( Uniforms.MaterialShininess, BoxShader, "u_Shininess" );
            GET_UNIFORM ( Uniforms.MaterialDiffuseTexture, BoxShader, "u_DiffuseTexture" );
            GET_UNIFORM ( Uniforms.MaterialSpecularTexture, BoxShader, "u_SpecularTexture" );
            GET_UNIFORM ( Uniforms.ModelMatrix, BoxShader, "u_ModelMatrix" );
            GET_UNIFORM ( Uniforms.ModelTransposeInverseMatrix, BoxShader, "u_ModelTransposeInverse" );

            GET_UNIFORM ( Uniforms.PointLight[0], BoxShader, "u_PointLightBlock[0]" );
            GET_UNIFORM ( Uniforms.PointLight[1], BoxShader, "u_PointLightBlock[1]" );

            PositionStream.BufferHandle = BoxBuffer;
            PositionStream.Stride = sizeof ( Vertex );
            PositionStream.StartOffset = offsetof ( Vertex, Position );
            PositionStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            PositionStream.ComponentsPerElement = 3;
            PositionStream.NormalizeData = false;

            NormalStream.BufferHandle = BoxBuffer;
            NormalStream.Stride = sizeof ( Vertex );
            NormalStream.StartOffset = offsetof ( Vertex, Normal );
            NormalStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            NormalStream.ComponentsPerElement = 3;
            NormalStream.NormalizeData = true;

            TextureStream.BufferHandle = BoxBuffer;
            TextureStream.Stride = sizeof ( Vertex );
            TextureStream.StartOffset = offsetof ( Vertex, TexCoord );
            TextureStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Float;
            TextureStream.ComponentsPerElement = 2;
            TextureStream.NormalizeData = false;

            IndexStream.BufferHandle = BoxIndexBuffer;
            IndexStream.Stride = sizeof ( uint16_t );
            IndexStream.StartOffset = 0;
            IndexStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Unsigned16;
            IndexStream.ComponentsPerElement = 1;

            CrossRenderer::RenderCommand BoxRenderCommand;

            BoxRenderCommand.Shader = BoxShader;
            BoxRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        PositionShaderAttribute,
                        PositionStream ) );
            BoxRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        NormalShaderAttribute,
                        NormalStream ) );
            BoxRenderCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                        TextureCoordShaderAttribute,
                        TextureStream ) );

            BoxRenderCommand.IndexBufferStream = IndexStream;
            BoxRenderCommand.IndexBuffer = BoxIndexBuffer;

            BoxRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                        Uniforms.MaterialDiffuseTexture,
                        CrossRenderer::TextureBindSettings ( DiffuseTexture ) ) );
            BoxRenderCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                        Uniforms.MaterialSpecularTexture,
                        CrossRenderer::TextureBindSettings ( SpecularTexture ) ) );

            BoxRenderCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
            BoxRenderCommand.StartVertex = 0;
            BoxRenderCommand.VertexCount = sizeof ( BoxIndices ) / sizeof ( uint16_t );
            BoxRenderCommand.State.Culling.Enabled = true;
            BoxRenderCommand.State.DepthTest.Enabled = true;
            RenderCommands.push_back ( BoxRenderCommand );

            Camera::PerspectiveParameters NewParameters;
            NewParameters.AspectRatio = 1.0f;
            NewParameters.Far = 1000;
            NewParameters.Near = 1.0;
            NewParameters.FOV = glm::pi<float>() / 2;
            SceneCamera.SetPerspectiveParameters ( NewParameters );

            CameraVariables.Keys.MoveNegZ = CrossRenderer::WindowManager::GetKeyCode ( "w" );
            CameraVariables.Keys.MovePosZ = CrossRenderer::WindowManager::GetKeyCode ( "s" );
            CameraVariables.Keys.MoveNegX = CrossRenderer::WindowManager::GetKeyCode ( "a" );
            CameraVariables.Keys.MovePosX = CrossRenderer::WindowManager::GetKeyCode ( "d" );
            CameraVariables.Keys.MovePosY = CrossRenderer::WindowManager::GetKeyCode ( "z" );
            CameraVariables.Keys.MoveNegY = CrossRenderer::WindowManager::GetKeyCode ( "x" );

            CameraVariables.Keys.RotatePosX = CrossRenderer::WindowManager::GetKeyCode ( "up" );
            CameraVariables.Keys.RotateNegX = CrossRenderer::WindowManager::GetKeyCode ( "down" );
            CameraVariables.Keys.RotatePosY = CrossRenderer::WindowManager::GetKeyCode ( "left" );
            CameraVariables.Keys.RotateNegY = CrossRenderer::WindowManager::GetKeyCode ( "right" );
            CameraVariables.Keys.RotatePosZ = CrossRenderer::WindowManager::GetKeyCode ( "q" );
            CameraVariables.Keys.RotateNegZ = CrossRenderer::WindowManager::GetKeyCode ( "e" );

            CameraVariables.Keys.Reset = CrossRenderer::WindowManager::GetKeyCode ( "r" );

            Reset();
            return true;
            }
        void SpecificDraw ( void )
            {
            ++FrameCount;
            CrossRenderer::ChangeShaderBufferContents ( Light0Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            CrossRenderer::ChangeShaderBufferContents ( Light1Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            const float TimeDelta = 1.0f / 60;
            if ( glm::length ( CameraVariables.Movement ) > 0 )
                {
                glm::vec3 FrameCameraMovement = glm::normalize ( CameraVariables.Movement );
                FrameCameraMovement = SceneCamera.GetOrientation() * CameraVariables.Movement;
                SceneCamera.UpdateMatrices();
                SceneCamera.SetPosition ( SceneCamera.GetPosition() + FrameCameraMovement * CameraVariables.MovementSpeed * TimeDelta );
                }

            if ( glm::length ( CameraVariables.Rotation ) > 0 )
                {
                SceneCamera.Rotate ( SceneCamera.GetXVector (), CameraVariables.Rotation.x * CameraVariables.RotationSpeed * TimeDelta );
                SceneCamera.Rotate ( glm::vec3 ( 0, 1, 0 ) /*CameraVariables.->GetYVector ()*/, CameraVariables.Rotation.y * CameraVariables.RotationSpeed * TimeDelta );
                SceneCamera.UpdateMatrices();
                }

            // The test itself
            if ( ShouldRotateCube )
                {
                Box.Rotate ( glm::vec3 ( 0, 1, 0 ), glm::pi<float>() / 120 );
                }
            if ( ShouldMoveLight )
                {
                LightData[0].Position.x = 5 * sin ( FrameCount * ( glm::pi<float>() / 120 ) );
                }

            // Go through all renderers, all windows, and update
            RenderCommands[0].UniformValues.clear();

            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.ShaderMVP, SceneCamera.GetViewProjectionMatrix() * Box.GetTransformationMatrix() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.ModelMatrix, Box.GetTransformationMatrix() ) );
            RenderCommands[0].UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( Uniforms.ModelTransposeInverseMatrix, glm::inverse ( glm::transpose ( Box.GetTransformationMatrix() ) ) ) );
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
        LitCubeTest Test;
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
