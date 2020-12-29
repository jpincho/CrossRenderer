#include "../TestBase.h"
#include "../CameraInput.h"
#include "../Material.h"
#include "../ModelLoader.h"

class AsteroidRingTest : public TestBase
    {
    protected:
        Camera SceneCamera;
        CameraInput CameraController;
        uint32_t FrameCount = 0;
        CrossRenderer::ShaderBufferHandle Light0Buffer, Light1Buffer;
        struct
            {
            CrossRenderer::ShaderHandle Handle;
            struct
                {
                CrossRenderer::ShaderUniformHandle ViewMatrix, ProjectionMatrix, ModelMatrix, ModelTransposeInverseMatrix;
                CrossRenderer::ShaderUniformHandle ViewPosition;
                CrossRenderer::ShaderUniformHandle MaterialShininess, MaterialDiffuseTexture, MaterialSpecularTexture;
                CrossRenderer::ShaderUniformHandle PointLight[2];
                struct
                    {
                    CrossRenderer::ShaderUniformHandle Color, Intensity;
                    } AmbientLight;
                } Uniforms;
            struct
                {
                CrossRenderer::ShaderAttributeHandle Position, Normal, TextureCoord;
                } Attributes;
            } AsteroidShader;

        struct
            {
            uint32_t Reset;
            } Keys;
        struct
            {
            glm::vec4 Position;
            glm::vec4 Diffuse, Specular;
            float Intensity;
            float ConstantAttenuation;
            float LinearAttenuation;
            float ExponentialAttenuation;
            } LightData[2];
        struct
            {
            glm::vec3 Color;
            float Intensity;
            } AmbientLight;
        std::vector <glm::mat4> AsteroidMatrices;
        ModelData AsteroidModelData, PlanetModelData;

    public:
        void Reset ( void )
            {
            SceneCamera.SetPosition ( glm::vec3 ( 0.0f, 0.0f, 3.0f ) );
            SceneCamera.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );

            LightData[0].Position = glm::vec4 ( -5, 5, 0, 1 );
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

            AmbientLight.Color = glm::vec3 ( 1, 1, 1 );
            AmbientLight.Intensity = 0.1f;
            }
        void GenerateAsteroidField ( const size_t AsteroidCount )
            {
            //unsigned int amount = 1000;
            //glm::mat4* modelMatrices;
            //modelMatrices = new glm::mat4[amount];
            //srand(glfwGetTime()); // initialize random seed
            float radius = 50.0;
            float offset = 2.5f;
            AsteroidMatrices.clear();
            for ( unsigned int i = 0; i < AsteroidCount; i++ )
                {
                glm::mat4 model = glm::mat4 ( 1.0f );
                // 1. translation: displace along circle with 'radius' in range [-offset, offset]
                float angle = ( float ) i / ( float ) AsteroidCount * 360.0f;
                float displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
                float x = sin ( angle ) * radius + displacement;
                displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
                float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
                displacement = ( rand() % ( int ) ( 2 * offset * 100 ) ) / 100.0f - offset;
                float z = cos ( angle ) * radius + displacement;
                model = glm::translate ( model, glm::vec3 ( x, y, z ) );

                // 2. scale: scale between 0.05 and 0.25f
                float scale = ( rand() % 20 ) / 100.0f + 0.05f;
                model = glm::scale ( model, glm::vec3 ( scale ) );

                // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
                float rotAngle = ( rand() % 360 );
                model = glm::rotate ( model, rotAngle, glm::vec3 ( 0.4f, 0.6f, 0.8f ) );

                // 4. now add to list of matrices
                AsteroidMatrices.push_back ( model );
                }
            }
        void SpecificOnEvent ( const CrossRenderer::WindowEvent &Event )
            {
            if ( ( Event.EventType == CrossRenderer::WindowEventType::KeyReleased ) && ( Event.EventData.KeyReleased.Key == Keys.Reset ) )
                {
                Reset();
                }
            }
        bool SpecificInitialize ( void )
            {
            CameraController.Initialize();
            CameraController.SetCamera ( &SceneCamera );

            // Load the shader
            AsteroidShader.Handle = LoadShader ( "Asteroid.vert", "", "Asteroid.frag" );
            if ( !AsteroidShader.Handle )
                return false;

            AsteroidShader.Attributes.Position = CrossRenderer::GetShaderAttributeHandle ( AsteroidShader.Handle, "a_VertexPosition" );
            AsteroidShader.Attributes.Normal = CrossRenderer::GetShaderAttributeHandle ( AsteroidShader.Handle, "a_Normal" );
            AsteroidShader.Attributes.TextureCoord = CrossRenderer::GetShaderAttributeHandle ( AsteroidShader.Handle, "a_TexCoord" );

#define GET_UNIFORM(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderUniformHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Uniform %s not found", NAME );\
                    return false;\
                    }
            GET_UNIFORM ( AsteroidShader.Uniforms.ViewMatrix, AsteroidShader.Handle, "u_ViewMatrix" );
            GET_UNIFORM ( AsteroidShader.Uniforms.ProjectionMatrix, AsteroidShader.Handle, "u_ProjectionMatrix" );
            GET_UNIFORM ( AsteroidShader.Uniforms.ViewPosition, AsteroidShader.Handle, "u_ViewPosition" );
            GET_UNIFORM ( AsteroidShader.Uniforms.ModelMatrix, AsteroidShader.Handle, "u_ModelMatrix" );
            GET_UNIFORM ( AsteroidShader.Uniforms.ModelTransposeInverseMatrix, AsteroidShader.Handle, "u_ModelTransposeInverse" );
            GET_UNIFORM ( AsteroidShader.Uniforms.MaterialShininess, AsteroidShader.Handle, "u_Shininess" );
            GET_UNIFORM ( AsteroidShader.Uniforms.MaterialDiffuseTexture, AsteroidShader.Handle, "u_DiffuseTexture" );
            GET_UNIFORM ( AsteroidShader.Uniforms.MaterialSpecularTexture, AsteroidShader.Handle, "u_SpecularTexture" );

            GET_UNIFORM ( AsteroidShader.Uniforms.AmbientLight.Color, AsteroidShader.Handle, "u_AmbientLight.Color" );
            GET_UNIFORM ( AsteroidShader.Uniforms.AmbientLight.Intensity, AsteroidShader.Handle, "u_AmbientLight.Intensity" );

            GET_UNIFORM ( AsteroidShader.Uniforms.PointLight[0], AsteroidShader.Handle, "u_PointLightBlock[0]" );
            GET_UNIFORM ( AsteroidShader.Uniforms.PointLight[1], AsteroidShader.Handle, "u_PointLightBlock[1]" );

            Light0Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            Light1Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            // Load the asteroid model
            if ( LoadModel ( "Asteroid/", "rock.obj", AsteroidModelData.Materials, AsteroidModelData.Meshes ) == false )
                return false;
            for ( auto &MeshIterator : AsteroidModelData.Meshes )
                {
                CrossRenderer::RenderCommand NewCommand;
                NewCommand.IndexBuffer = MeshIterator.Index;
                NewCommand.IndexBufferStream.BufferHandle = MeshIterator.Index;
                NewCommand.IndexBufferStream.ComponentsPerElement = 1;
                NewCommand.IndexBufferStream.ComponentType = CrossRenderer::ShaderBufferComponentType::Unsigned32;
                NewCommand.IndexBufferStream.NormalizeData = false;
                NewCommand.IndexBufferStream.StartOffset = 0;
                NewCommand.IndexBufferStream.Stride = sizeof ( uint32_t );

                NewCommand.State.DepthTest.Set ( CrossRenderer::DepthTestMode::Less );
                NewCommand.State.Culling.Enabled = true;

                NewCommand.Primitive = CrossRenderer::PrimitiveType::TriangleList;
                NewCommand.Shader = AsteroidShader.Handle;
                NewCommand.StartVertex = 0;
                NewCommand.VertexCount = MeshIterator.VertexCount;
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( AsteroidShader.Attributes.Position, CrossRenderer::ShaderBufferDataStream ( MeshIterator.Vertex, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( AsteroidShader.Attributes.Normal, CrossRenderer::ShaderBufferDataStream ( MeshIterator.Normal, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( AsteroidShader.Attributes.TextureCoord, CrossRenderer::ShaderBufferDataStream ( MeshIterator.TexCoord, 0, sizeof ( glm::vec2 ), CrossRenderer::ShaderBufferComponentType::Float, 2 ) ) );

                NewCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( AsteroidShader.Uniforms.MaterialDiffuseTexture, CrossRenderer::TextureBindSettings ( MeshIterator.MeshMaterial.Textures[ ( int ) Material::MaterialTextureType::Diffuse] ) ) );
                NewCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( AsteroidShader.Uniforms.MaterialSpecularTexture, CrossRenderer::TextureBindSettings ( MeshIterator.MeshMaterial.Textures[ ( int ) Material::MaterialTextureType::Diffuse] ) ) );

                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( AsteroidShader.Uniforms.MaterialShininess, MeshIterator.MeshMaterial.Shininess ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( AsteroidShader.Uniforms.PointLight[0], Light0Buffer ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( AsteroidShader.Uniforms.PointLight[1], Light1Buffer ) );

                AsteroidModelData.RenderCommands.push_back ( NewCommand );
                }
            if ( LoadModel ( "Planet/", "planet.obj", PlanetModelData.Materials, PlanetModelData.Meshes ) == false )
                return false;

            GenerateAsteroidField ( 500 );
            AsteroidMatrices.push_back ( glm::mat4 ( 1.0f ) );

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
            ImGui::Begin ( "Lights" );
            ImGui::PushItemWidth ( 100 );
            ImGui::Text ( "Ambient" );
            ImGui::ColorPicker3 ( "Ambient diffuse", glm::value_ptr ( AmbientLight.Color ) );
            ImGui::SliderFloat ( "Ambient intensity", &AmbientLight.Intensity, 0.0f, 200.0f );
            ImGui::Text ( "Light 1" );
            ImGui::InputFloat4 ( "Light 1 position", glm::value_ptr ( LightData[0].Position ) );
            ImGui::ColorPicker4 ( "Light 1 diffuse", glm::value_ptr ( LightData[0].Diffuse ) );
            ImGui::ColorPicker4 ( "Light 1 specular", glm::value_ptr ( LightData[0].Specular ) );
            ImGui::SliderFloat ( "Light 1 intensity", &LightData[0].Intensity, 0.0f, 200.0f );
            ImGui::SliderFloat ( "Light 1 const. atten.", &LightData[0].ConstantAttenuation, 0.0f, 200.0f );
            ImGui::SliderFloat ( "Light 1 linear atten.", &LightData[0].LinearAttenuation, 0.0f, 200.0f );
            ImGui::SliderFloat ( "Light 1 exp. atten.", &LightData[0].ExponentialAttenuation, 0.0f, 200.0f );

            ImGui::Separator();
            ImGui::Text ( "Light 2" );
            ImGui::InputFloat4 ( "Light 2 position", glm::value_ptr ( LightData[1].Position ) );
            ImGui::ColorPicker4 ( "Light 2 diffuse", glm::value_ptr ( LightData[1].Diffuse ) );
            ImGui::ColorPicker4 ( "Light 2 specular", glm::value_ptr ( LightData[1].Specular ) );
            ImGui::SliderFloat ( "Light 2 intensity", &LightData[1].Intensity, 0.0f, 200.0f );
            ImGui::SliderFloat ( "Light 2 const. atten.", &LightData[1].ConstantAttenuation, 0.0f, 200.0f );
            ImGui::SliderFloat ( "Light 2 linear atten.", &LightData[1].LinearAttenuation, 0.0f, 200.0f );
            ImGui::SliderFloat ( "Light 2 exp. atten.", &LightData[1].ExponentialAttenuation, 0.0f, 200.0f );
            ImGui::End();
            }
        void SpecificDraw ( void )
            {
            ++FrameCount;
            const float TimeDelta = 1.0f / 60;
            CrossRenderer::ChangeShaderBufferContents ( Light0Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            CrossRenderer::ChangeShaderBufferContents ( Light1Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            RenderCommands.clear();
            CameraController.Update ( TimeDelta );
            for ( auto AsteroidMatrixInstance : AsteroidMatrices )
                {
                for ( auto NewCommand : AsteroidModelData.RenderCommands )
                    {
                    NewCommand.UniformValues.push_back( CrossRenderer::ShaderUniformValuePair( AsteroidShader.Uniforms.ModelMatrix, AsteroidMatrixInstance ) );
                    NewCommand.UniformValues.push_back( CrossRenderer::ShaderUniformValuePair( AsteroidShader.Uniforms.ModelTransposeInverseMatrix, glm::inverse( glm::transpose( AsteroidMatrixInstance ) ) ) );

                    NewCommand.UniformValues.push_back( CrossRenderer::ShaderUniformValuePair( AsteroidShader.Uniforms.ViewPosition, SceneCamera.GetPosition() ) );
                    NewCommand.UniformValues.push_back( CrossRenderer::ShaderUniformValuePair( AsteroidShader.Uniforms.ViewMatrix, SceneCamera.GetViewMatrix() ) );
                    NewCommand.UniformValues.push_back( CrossRenderer::ShaderUniformValuePair( AsteroidShader.Uniforms.ProjectionMatrix, SceneCamera.GetProjectionMatrix() ) );

                    NewCommand.UniformValues.push_back( CrossRenderer::ShaderUniformValuePair( AsteroidShader.Uniforms.AmbientLight.Color, AmbientLight.Color ) );
                    NewCommand.UniformValues.push_back( CrossRenderer::ShaderUniformValuePair( AsteroidShader.Uniforms.AmbientLight.Intensity, AmbientLight.Intensity ) );
                    assert( CrossRenderer::SanityCheckRenderCommand( NewCommand ) == true );

                    RenderCommands.push_back( NewCommand );
                    }
                }
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        AsteroidRingTest Test;
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
