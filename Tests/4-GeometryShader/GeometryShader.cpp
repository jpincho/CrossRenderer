#include "../TestBase.h"
#include "../CameraInput.h"
#include "../Material.h"
#include "../ModelLoader.h"
class GeometryShaderTest : public TestBase
    {
    protected:
        Camera SceneCamera;
        CameraInput CameraController;
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
            } StormtrooperShader;
        struct
            {
            CrossRenderer::ShaderHandle Handle;
            struct
                {
                CrossRenderer::ShaderUniformHandle ViewMatrix, ProjectionMatrix, ModelMatrix;
                CrossRenderer::ShaderUniformHandle NormalLength, StartNormalColor, EndNormalColor, ShowVertexNormals, ShowFaceNormals;
                } Uniforms;
            struct
                {
                CrossRenderer::ShaderAttributeHandle Position, Normal;
                } Attributes;
            } NormalsShader;

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
        MovableObject Stormtrooper;
        ModelData StormtrooperModelData;

        float NormalLength;
        glm::vec4 NormalStartColor, NormalEndColor;
        bool ShowVertexNormals, ShowFaceNormals;
        std::vector <CrossRenderer::RenderCommand> NormalRenderCommands;
    public:
        void Reset ( void )
            {
            SceneCamera.SetPosition ( glm::vec3 ( 0.0f, 0.0f, 3.0f ) );
            SceneCamera.SetOrientation ( glm::vec3 ( 0, 1, 0 ), 0 );
            Stormtrooper.SetPosition ( glm::vec3 ( 0, 0, 0 ) );
            Stormtrooper.Rotate ( glm::vec3 ( 1, 0, 0 ), -glm::pi<float>() / 2 );

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
            AmbientLight.Intensity = 0.6f;

            NormalLength = 0.4f;
            NormalStartColor = glm::vec4 ( 1, 0, 0, 1 );
            NormalEndColor = glm::vec4 ( 0, 1, 0, 1 );
            ShowVertexNormals = true;
            ShowFaceNormals = true;
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
            StormtrooperShader.Handle = LoadShader ( "Stormtrooper.vert", "", "Stormtrooper.frag" );
            if ( !StormtrooperShader.Handle )
                return false;

            StormtrooperShader.Attributes.Position = CrossRenderer::GetShaderAttributeHandle ( StormtrooperShader.Handle, "a_VertexPosition" );
            StormtrooperShader.Attributes.Normal = CrossRenderer::GetShaderAttributeHandle ( StormtrooperShader.Handle, "a_Normal" );
            StormtrooperShader.Attributes.TextureCoord = CrossRenderer::GetShaderAttributeHandle ( StormtrooperShader.Handle, "a_TexCoord" );

#define GET_UNIFORM(VARIABLE,SHADER,NAME)\
                VARIABLE = CrossRenderer::GetShaderUniformHandle ( SHADER, NAME );\
                if ( !VARIABLE )\
                    {\
                    LOG_ERROR ( "Uniform %s not found", NAME );\
                    return false;\
                    }
            GET_UNIFORM ( StormtrooperShader.Uniforms.ViewMatrix, StormtrooperShader.Handle, "u_ViewMatrix" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.ProjectionMatrix, StormtrooperShader.Handle, "u_ProjectionMatrix" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.ViewPosition, StormtrooperShader.Handle, "u_ViewPosition" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.ModelMatrix, StormtrooperShader.Handle, "u_ModelMatrix" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.ModelTransposeInverseMatrix, StormtrooperShader.Handle, "u_ModelTransposeInverse" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.MaterialShininess, StormtrooperShader.Handle, "u_Shininess" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.MaterialDiffuseTexture, StormtrooperShader.Handle, "u_DiffuseTexture" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.MaterialSpecularTexture, StormtrooperShader.Handle, "u_SpecularTexture" );

            GET_UNIFORM ( StormtrooperShader.Uniforms.AmbientLight.Color, StormtrooperShader.Handle, "u_AmbientLight.Color" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.AmbientLight.Intensity, StormtrooperShader.Handle, "u_AmbientLight.Intensity" );

            GET_UNIFORM ( StormtrooperShader.Uniforms.PointLight[0], StormtrooperShader.Handle, "u_PointLightBlock[0]" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.PointLight[1], StormtrooperShader.Handle, "u_PointLightBlock[1]" );

            NormalsShader.Handle = LoadShader ( "Normals.vert", "Normals.geom", "Normals.frag" );
            if ( !NormalsShader.Handle )
                return false;
            NormalsShader.Attributes.Position = CrossRenderer::GetShaderAttributeHandle ( NormalsShader.Handle, "a_VertexPosition" );
            NormalsShader.Attributes.Normal = CrossRenderer::GetShaderAttributeHandle ( NormalsShader.Handle, "a_Normal" );

            GET_UNIFORM ( NormalsShader.Uniforms.ViewMatrix, NormalsShader.Handle, "u_ViewMatrix" );
            GET_UNIFORM ( NormalsShader.Uniforms.ProjectionMatrix, NormalsShader.Handle, "u_ProjectionMatrix" );
            GET_UNIFORM ( NormalsShader.Uniforms.ModelMatrix, NormalsShader.Handle, "u_ModelMatrix" );

            GET_UNIFORM ( NormalsShader.Uniforms.NormalLength, NormalsShader.Handle, "u_NormalLength" );
            GET_UNIFORM ( NormalsShader.Uniforms.StartNormalColor, NormalsShader.Handle, "u_StartNormalColor" );
            GET_UNIFORM ( NormalsShader.Uniforms.EndNormalColor, NormalsShader.Handle, "u_EndNormalColor" );
            GET_UNIFORM ( NormalsShader.Uniforms.ShowVertexNormals, NormalsShader.Handle, "u_ShowVertexNormals" );
            GET_UNIFORM ( NormalsShader.Uniforms.ShowFaceNormals, NormalsShader.Handle, "u_ShowFaceNormals" );

            Light0Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            Light1Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            // Load the model
            if ( LoadModel ( "Stormtrooper/", "Stormtrooper.fbx", StormtrooperModelData.Materials, StormtrooperModelData.Meshes ) == false )
                return false;
            for ( auto &MeshIterator : StormtrooperModelData.Meshes )
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
                NewCommand.Shader = StormtrooperShader.Handle;
                NewCommand.StartVertex = 0;
                NewCommand.VertexCount = MeshIterator.VertexCount;
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( StormtrooperShader.Attributes.Position, CrossRenderer::ShaderBufferDataStream ( MeshIterator.Vertex, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( StormtrooperShader.Attributes.Normal, CrossRenderer::ShaderBufferDataStream ( MeshIterator.Normal, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( StormtrooperShader.Attributes.TextureCoord, CrossRenderer::ShaderBufferDataStream ( MeshIterator.TexCoord, 0, sizeof ( glm::vec2 ), CrossRenderer::ShaderBufferComponentType::Float, 2 ) ) );

                NewCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( StormtrooperShader.Uniforms.MaterialDiffuseTexture, CrossRenderer::TextureBindSettings ( MeshIterator.MeshMaterial.Textures[ ( int ) Material::MaterialTextureType::Diffuse] ) ) );
                NewCommand.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair ( StormtrooperShader.Uniforms.MaterialSpecularTexture, CrossRenderer::TextureBindSettings ( MeshIterator.MeshMaterial.Textures[ ( int ) Material::MaterialTextureType::Specular] ) ) );

                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.MaterialShininess, MeshIterator.MeshMaterial.Shininess ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.PointLight[0], Light0Buffer ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.PointLight[1], Light1Buffer ) );

                StormtrooperModelData.RenderCommands.push_back ( NewCommand );
                }
            // Create the commands for the normals
            for ( auto &MeshIterator : StormtrooperModelData.Meshes )
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
                NewCommand.Shader = NormalsShader.Handle;
                NewCommand.StartVertex = 0;
                NewCommand.VertexCount = MeshIterator.VertexCount;
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( NormalsShader.Attributes.Position, CrossRenderer::ShaderBufferDataStream ( MeshIterator.Vertex, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );
                NewCommand.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair ( NormalsShader.Attributes.Normal, CrossRenderer::ShaderBufferDataStream ( MeshIterator.Normal, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );

                NormalRenderCommands.push_back ( NewCommand );
                }
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
            ImGui::PushItemWidth ( 100 );
            ImGui::SliderFloat ( "Normal length", &NormalLength, 0.0f, 2.0f );
            ImGui::ColorPicker4 ( "Normal start color", glm::value_ptr ( NormalStartColor ) );
            ImGui::ColorPicker4 ( "Normal end color", glm::value_ptr ( NormalEndColor ) );
            ImGui::Checkbox ( "Show vertex normals", &ShowVertexNormals );
            ImGui::Checkbox ( "Show face normals", &ShowFaceNormals );
            ImGui::End();

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
            const float TimeDelta = 1.0f / 60;
            CrossRenderer::ChangeShaderBufferContents ( Light0Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            CrossRenderer::ChangeShaderBufferContents ( Light1Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            RenderCommands.clear();
            CameraController.Update ( TimeDelta );
            for ( auto NewCommand : StormtrooperModelData.RenderCommands )
                {
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ModelMatrix, Stormtrooper.GetTransformationMatrix() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ModelTransposeInverseMatrix, glm::inverse ( glm::transpose ( Stormtrooper.GetTransformationMatrix() ) ) ) );

                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ViewPosition, SceneCamera.GetPosition() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ViewMatrix, SceneCamera.GetViewMatrix() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ProjectionMatrix, SceneCamera.GetProjectionMatrix() ) );

                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.AmbientLight.Color, AmbientLight.Color ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.AmbientLight.Intensity, AmbientLight.Intensity ) );
                assert ( CrossRenderer::SanityCheckRenderCommand ( NewCommand ) == true );

                RenderCommands.push_back ( NewCommand );
                }
            for ( auto NewCommand : NormalRenderCommands )
                {
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.ModelMatrix, Stormtrooper.GetTransformationMatrix() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.ViewMatrix, SceneCamera.GetViewMatrix() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.ProjectionMatrix, SceneCamera.GetProjectionMatrix() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.NormalLength, NormalLength ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.StartNormalColor, NormalStartColor ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.EndNormalColor, NormalEndColor ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.ShowVertexNormals, ShowVertexNormals ? 1 : 0 ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( NormalsShader.Uniforms.ShowFaceNormals, ShowFaceNormals ? 1 : 0 ) );

                assert ( CrossRenderer::SanityCheckRenderCommand ( NewCommand ) == true );

                RenderCommands.push_back ( NewCommand );
                }
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        GeometryShaderTest Test;
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
