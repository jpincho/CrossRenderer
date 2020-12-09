#include "../TestBase.h"
#include "../CameraInput.h"
#include "../Material.h"
#include "../ModelLoader.h"
class StormtrooperTest : public TestBase
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
                CrossRenderer::ShaderUniformHandle ShaderMVP, ModelMatrix, ModelTransposeInverseMatrix;
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
                CrossRenderer::ShaderUniformHandle ShaderMVP, ModelTransposeInverseMatrix, NormalLength, StartNormalColor, EndNormalColor, ShowVertexNormals, ShowFaceNormals;
                } Uniforms;
            struct
                {
                CrossRenderer::ShaderAttributeHandle Position, Normal;
                } Attributes;
            } NormalsShader;
        typedef struct
            {
            CrossRenderer::ShaderBufferHandle Vertex, Normal, TexCoord, Index;
            Material MeshMaterial;
            size_t VertexCount;
            CrossRenderer::RenderCommand Command;
            } MeshRenderData;


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
        std::vector <Material> Materials;
        std::vector <MeshRenderData> MeshRenderDataArray;
        MovableObject Stormtrooper;

    public:
        StormtrooperTest ( void )
            {
            TestName.assign ( "Stormtrooper" );
            }
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
            CameraController.Initialize ();
            CameraController.SetCamera ( &SceneCamera );

            // Load the shader
            std::string Path ( TEST_SOURCE_LOCATION );
            Path.append ( "Stormtrooper/" );
            Path.append ( CrossRenderer::Stringify ( RendererBackend ) );

            StormtrooperShader.Handle = LoadShader (Path + "/Stormtrooper.vert", "", Path + "/Stormtrooper.frag" );
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
            GET_UNIFORM ( StormtrooperShader.Uniforms.ShaderMVP, StormtrooperShader.Handle, "u_MVPMatrix" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.ViewPosition, StormtrooperShader.Handle, "u_ViewPosition" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.MaterialShininess, StormtrooperShader.Handle, "u_Shininess" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.MaterialDiffuseTexture, StormtrooperShader.Handle, "u_DiffuseTexture" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.MaterialSpecularTexture, StormtrooperShader.Handle, "u_SpecularTexture" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.ModelMatrix, StormtrooperShader.Handle, "u_ModelMatrix" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.ModelTransposeInverseMatrix, StormtrooperShader.Handle, "u_ModelTransposeInverse" );

            GET_UNIFORM ( StormtrooperShader.Uniforms.AmbientLight.Color, StormtrooperShader.Handle, "u_AmbientLight.Color" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.AmbientLight.Intensity, StormtrooperShader.Handle, "u_AmbientLight.Intensity" );

            GET_UNIFORM ( StormtrooperShader.Uniforms.PointLight[0], StormtrooperShader.Handle, "u_PointLightBlock[0]" );
            GET_UNIFORM ( StormtrooperShader.Uniforms.PointLight[1], StormtrooperShader.Handle, "u_PointLightBlock[1]" );

            Light0Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            Light1Buffer = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            // Load the model
            ModelLoader StormtrooperModel;
            Path.assign ( TEST_SOURCE_LOCATION );
            Path.append ( "Data/Stormtrooper/" );

            if ( StormtrooperModel.Set ( Path, "Stormtrooper.fbx" ) == false )
                return false;
            if ( StormtrooperModel.Load() == false )
                return false;

            for ( auto &MaterialIterator : StormtrooperModel.Materials )
                {
                Material NewMaterial;
                NewMaterial.Name = MaterialIterator.Name;
                NewMaterial.Diffuse = MaterialIterator.Diffuse;
                NewMaterial.Specular = MaterialIterator.Specular;
                NewMaterial.Emissive = MaterialIterator.Emissive;
                NewMaterial.Shininess = MaterialIterator.Shininess;
                for ( auto TextureTypeIterator = 0; TextureTypeIterator < ( int ) Material::MaterialTextureType::COUNT; ++TextureTypeIterator )
                    {
                    CrossRenderer::TextureBindSettings Settings;
                    if ( MaterialIterator.Textures[TextureTypeIterator].empty() == true )
                        continue;
                    if ( LoadTextureFile ( Path + MaterialIterator.Textures[TextureTypeIterator], Settings.Handle ) == false )
                        return false;
                    Settings.WrapSettings = MaterialIterator.WrapSettings[TextureTypeIterator];
                    NewMaterial.Textures[TextureTypeIterator] = Settings;
                    }
                Materials.push_back ( NewMaterial );
                }

            for ( auto &ModelIterator : StormtrooperModel.Models )
                {
                for ( auto &MeshIterator : ModelIterator.Meshes )
                    {
                    MeshRenderData NewData;
                    NewData.Index = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.Indices.data(), MeshIterator.Indices.size() * sizeof ( uint32_t ) ) );
                    NewData.Vertex = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.Vertices.data(), MeshIterator.Vertices.size() * sizeof ( glm::vec3 ) ) );
                    NewData.Normal = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.Normals.data(), MeshIterator.Normals.size() * sizeof ( glm::vec3 ) ) );
                    NewData.TexCoord = CrossRenderer::CreateShaderBuffer ( CrossRenderer::ShaderBufferDescriptor ( MeshIterator.TexCoords.data(), MeshIterator.TexCoords.size() * sizeof ( glm::uvec2 ) ) );
                    NewData.VertexCount = MeshIterator.Indices.size();

                    NewData.Command.Shader = StormtrooperShader.Handle;
                    NewData.Command.Primitive = CrossRenderer::PrimitiveType::TriangleList;
                    NewData.Command.IndexBuffer = NewData.Index;
                    NewData.Command.StartVertex = 0;
                    NewData.Command.VertexCount = MeshIterator.Indices.size();
                    NewData.Command.IndexBufferStream = CrossRenderer::ShaderBufferDataStream ( NewData.Index, 0, sizeof ( uint32_t ), CrossRenderer::ShaderBufferComponentType::Unsigned32, 1 );
                    NewData.Command.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.PointLight[0], Light0Buffer ) );
                    NewData.Command.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.PointLight[1], Light1Buffer ) );

                    NewData.Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                                StormtrooperShader.Attributes.Position,
                                CrossRenderer::ShaderBufferDataStream ( NewData.Vertex, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );
                    NewData.Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                                StormtrooperShader.Attributes.Normal,
                                CrossRenderer::ShaderBufferDataStream ( NewData.Normal, 0, sizeof ( glm::vec3 ), CrossRenderer::ShaderBufferComponentType::Float, 3 ) ) );
                    NewData.Command.ShaderBufferBindings.push_back ( CrossRenderer::ShaderBufferBindPair (
                                StormtrooperShader.Attributes.TextureCoord,
                                CrossRenderer::ShaderBufferDataStream ( NewData.TexCoord, 0, sizeof ( glm::vec2 ), CrossRenderer::ShaderBufferComponentType::Float, 2 ) ) );

                    NewData.Command.State.DepthTest.Set ( CrossRenderer::DepthTestMode::Less );
                    for ( auto &MaterialIterator : Materials )
                        {
                        if ( MeshIterator.MaterialName == MaterialIterator.Name )
                            {
                            NewData.MeshMaterial = MaterialIterator;
                            NewData.Command.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.MaterialShininess, MaterialIterator.Shininess ) );
                            if ( MaterialIterator.Textures[ ( int ) Material::MaterialTextureType::Diffuse].Handle )
                                {
                                NewData.Command.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                                            StormtrooperShader.Uniforms.MaterialDiffuseTexture,
                                            MaterialIterator.Textures[ ( int ) Material::MaterialTextureType::Diffuse] ) );
                                }
                            if ( MaterialIterator.Textures[ ( int ) Material::MaterialTextureType::Specular].Handle )
                                {
                                NewData.Command.TextureBindings.push_back ( CrossRenderer::ShaderTextureBindPair (
                                            StormtrooperShader.Uniforms.MaterialSpecularTexture,
                                            MaterialIterator.Textures[ ( int ) Material::MaterialTextureType::Specular] ) );
                                }
                            break;
                            }
                        }
                    MeshRenderDataArray.push_back ( NewData );
                    }
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
        void SpecificDraw ( void )
            {
            ++FrameCount;
            const float TimeDelta = 1.0f / 60;
            CrossRenderer::ChangeShaderBufferContents ( Light0Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[0], sizeof ( LightData[0] ) ) );
            CrossRenderer::ChangeShaderBufferContents ( Light1Buffer, CrossRenderer::ShaderBufferDescriptor ( &LightData[1], sizeof ( LightData[1] ) ) );

            RenderCommands.clear();
            CameraController.Update ( TimeDelta );
            for ( auto &MeshIterator : MeshRenderDataArray )
                {
                CrossRenderer::RenderCommand NewCommand;
                NewCommand = MeshIterator.Command;
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ShaderMVP, SceneCamera.GetViewProjectionMatrix() * Stormtrooper.GetTransformationMatrix() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ModelMatrix, Stormtrooper.GetTransformationMatrix() ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ModelTransposeInverseMatrix, glm::inverse ( glm::transpose ( Stormtrooper.GetTransformationMatrix() ) ) ) );
                NewCommand.UniformValues.push_back ( CrossRenderer::ShaderUniformValuePair ( StormtrooperShader.Uniforms.ViewPosition, SceneCamera.GetPosition() ) );

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
