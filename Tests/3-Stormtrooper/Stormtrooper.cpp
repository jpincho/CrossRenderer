#include "../TestBase.h"
#include "../Material.h"
#include "../ModelLoader.h"
class StormtrooperTest : public TestBase
    {
    protected:
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

    public:
        void Reset ( void )
            {
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
            }
        bool SpecificInitialize ( void )
            {
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

            Light0Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ), CrossRenderer::ShaderBufferAccessType::Dynamic ) );
            Light1Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ), CrossRenderer::ShaderBufferAccessType::Dynamic ) );

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
            CrossRenderer::ChangeShaderBufferContents ( Light0Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            CrossRenderer::ChangeShaderBufferContents ( Light1Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            RenderCommands.clear();
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
            }
    };

int main ( void )
    {
    std::vector <CrossRenderer::RendererBackend> RenderersToTest;
    CrossRenderer::GetAvailableRendererBackends ( RenderersToTest );

    for ( unsigned index = 0; index < RenderersToTest.size(); ++index )
        {
        StormtrooperTest Test;
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
