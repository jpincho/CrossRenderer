#include "ModelLoader.h"
#include <Logger/Logger.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <list>
#include <stdexcept>
#include <CrossRendererConfig.h>

class AssimpIOStream : public Assimp::IOStream
    {
    public:
        FILE *File;
        ~AssimpIOStream ( void )
            {
            if ( File )
                {
                fclose ( File );
                File = nullptr;
                }
            }
        size_t FileSize () const
            {
            long pos = ftell ( File );
            fseek ( File, 0, SEEK_END );
            size_t size = ftell ( File );
            fseek ( File, pos, SEEK_SET );
            return size;
            }
        void Flush ()
            {}
        size_t Read ( void *pvBuffer, size_t pSize, size_t pCount )
            {
            return fread ( pvBuffer, pSize, pCount, File );
            }
        aiReturn Seek ( size_t pOffset, aiOrigin pOrigin )
            {
            int origin;

            switch ( pOrigin )
                {
                case aiOrigin_CUR:
                    origin = SEEK_CUR;
                    break;
                case aiOrigin_SET:
                    origin = SEEK_SET;
                    break;
                case aiOrigin_END:
                    origin = SEEK_END;
                    break;
                default:
                    return aiReturn_FAILURE;
                }
            fseek ( File, ( long ) pOffset, origin );
            return aiReturn_SUCCESS;
            }
        size_t Tell () const
            {
            return ftell ( File );
            }
        size_t Write ( const void *pvBuffer, size_t pSize, size_t pCount )
            {
            return fwrite ( pvBuffer, pSize, pCount, File );
            }
    };

class AssimpIOSystem : public Assimp::IOSystem
    {
    protected:
        std::list <AssimpIOStream*> OpenStreams;
    public:
        ~AssimpIOSystem ( void )
            {
            /*for ( auto &iterator : OpenStreams )
                {
                Close ( iterator );
                }*/
            }
        bool ComparePaths ( const char *one, const char *second ) const
            {
            std::string Path1 ( one ), Path2 ( second );
            return Path1 == Path2;
            }
        bool Exists ( const char *pFile ) const
            {
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
            FILE *temp;
            if ( fopen_s ( &temp, pFile, "rb" ) != 0 )
                return false;
#else
            FILE *temp = fopen ( pFile, "rb" );
#endif
            if ( temp == nullptr )
                return false;
            fclose ( temp );
            return true;
            }
        char getOsSeparator () const
            {
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
            return '\\';
#else
            return '/';
#endif
            }
        Assimp::IOStream *Open ( const char *pFile, const char *Mode = "rb" )
            {
            AssimpIOStream *NewStream = new AssimpIOStream;
#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
            if ( fopen_s ( & ( NewStream->File ), pFile, Mode ) != 0 )
                return nullptr;
#else
            NewStream->File = fopen ( pFile, Mode );
#endif
            if ( NewStream->File == nullptr )
                {
                delete NewStream;
                return nullptr;
                }
            OpenStreams.push_back ( NewStream );
            return NewStream;
            }
        void Close ( Assimp::IOStream *pFile )
            {
            AssimpIOStream *NewStream = static_cast <AssimpIOStream*> ( pFile );
            fclose ( NewStream->File );
            std::list <AssimpIOStream*>::iterator Iterator = std::find ( OpenStreams.begin(), OpenStreams.end(), NewStream );
            assert ( Iterator != OpenStreams.end() );
            OpenStreams.erase ( Iterator );
            delete NewStream;
            }
    };

glm::vec3 assign ( const aiColor3D &value )
    {
    glm::vec3 result;
    result.x = value.r;
    result.y = value.g;
    result.z = value.b;
    return result;
    }

CrossRenderer::TextureWrapMode Translate ( aiTextureMapMode Value )
    {
    switch ( Value )
        {
        case aiTextureMapMode_Wrap:
            return CrossRenderer::TextureWrapMode::Repeat;
        case aiTextureMapMode_Clamp:
            return CrossRenderer::TextureWrapMode::ClampToEdge;
        case aiTextureMapMode_Mirror:
            return CrossRenderer::TextureWrapMode::MirroredRepeat;
        default:
            return CrossRenderer::TextureWrapMode::ClampToEdge;
        }
    }

Material::MaterialTextureType Translate ( const aiTextureType AssimpTextureType )
    {
    switch ( AssimpTextureType )
        {
        case aiTextureType_DIFFUSE:
            return Material::MaterialTextureType::Diffuse;
        case aiTextureType_SPECULAR:
            return Material::MaterialTextureType::Specular;
        case aiTextureType_AMBIENT:
            return Material::MaterialTextureType::Ambient;
        case aiTextureType_EMISSIVE:
            return Material::MaterialTextureType::Emissive;
        case aiTextureType_HEIGHT:
            return Material::MaterialTextureType::Height;
        case aiTextureType_NORMALS:
            return Material::MaterialTextureType::Normal;
        case aiTextureType_SHININESS:
            return Material::MaterialTextureType::Shininess;
        case aiTextureType_OPACITY:
            return Material::MaterialTextureType::Opacity;
        case aiTextureType_DISPLACEMENT:
            return Material::MaterialTextureType::Displacement;
        case aiTextureType_LIGHTMAP:
            return Material::MaterialTextureType::Lightmap;
        case aiTextureType_REFLECTION:
            return Material::MaterialTextureType::Reflection;
        default:
            throw std::runtime_error ( "Unhandled aiTextureType" );
        };
    }

aiTextureType Translate ( const Material::MaterialTextureType TextureType )
    {
    switch ( TextureType )
        {
        case Material::MaterialTextureType::Diffuse:
            return aiTextureType_DIFFUSE;
        case Material::MaterialTextureType::Specular:
            return aiTextureType_SPECULAR;
        case Material::MaterialTextureType::Ambient:
            return aiTextureType_AMBIENT;
        case Material::MaterialTextureType::Emissive:
            return aiTextureType_EMISSIVE;
        case Material::MaterialTextureType::Height:
            return aiTextureType_HEIGHT;
        case Material::MaterialTextureType::Normal:
            return aiTextureType_NORMALS;
        case Material::MaterialTextureType::Shininess:
            return aiTextureType_SHININESS;
        case Material::MaterialTextureType::Opacity:
            return aiTextureType_OPACITY;
        case Material::MaterialTextureType::Displacement:
            return aiTextureType_DISPLACEMENT;
        case Material::MaterialTextureType::Lightmap:
            return aiTextureType_LIGHTMAP;
        case Material::MaterialTextureType::Reflection:
            return aiTextureType_REFLECTION;
        default:
            throw std::runtime_error ( "Unhandled MaterialTextureType" );
        };

    }

ModelLoader::ModelLoader ( void )
    {
    File = nullptr;
    }

bool ModelLoader::Set ( const std::string &NewBasePath, const std::string &NewRelativePath )
    {
    Materials.clear();
    Models.clear();
    BasePath = NewBasePath;
    RelativePath = NewRelativePath;
    FullPath = BasePath + std::string ( "/" ) + RelativePath;

    LOG_DEBUG ( "Checking %s", FullPath.c_str() );
    if ( File )
        {
        fclose ( File );
        File = nullptr;
        }

#if defined ( CROSS_RENDERER_TARGET_PLATFORM_WINDOWS )
    if ( fopen_s ( &File, FullPath.c_str(), "rb" ) != 0 )
        return nullptr;
#else
    File = fopen ( FullPath.c_str(), "rb" );
#endif

    if ( File == nullptr )
        return false;
    return true;
    }

bool ModelLoader::Load ( void )
    {
    Materials.clear();
    Models.clear();
    LOG_DEBUG ( "Loading file '%s'", FullPath.c_str() );
    Assimp::Importer Importer;
    const aiScene *Scene;
    AssimpIOSystem IOSystem;
    Importer.SetIOHandler ( &IOSystem );
    Scene = Importer.ReadFile ( FullPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph );
    if ( Scene == nullptr )
        {
        LOG_ERROR ( "Unable to load model. %s", Importer.GetErrorString() );
        Importer.FreeScene ();
        Importer.SetIOHandler ( nullptr );

        return false;
        }

    LOG_DEBUG ( "Detected %u materials, %u meshes", Scene->mNumMaterials, Scene->mNumMeshes );

    for ( unsigned MatIndex = 0; MatIndex < Scene->mNumMaterials; ++MatIndex )
        {
        // Material name
        MaterialDescriptor Descriptor;

        aiString Name;
        aiColor3D Color3;
        aiString String;
        float FloatValue;

        if ( ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_NAME, Name ) == AI_SUCCESS ) && ( Name.length != 0 ) )
            Descriptor.Name = RelativePath + std::string ( "/" ) + std::string ( Name.C_Str() );
        else
            return false;

        /* Read material values
         AI_MATKEY_NAME
         AI_MATKEY_TWOSIDED
         AI_MATKEY_SHADING_MODEL
         AI_MATKEY_ENABLE_WIREFRAME
         AI_MATKEY_BLEND_FUNC
         AI_MATKEY_OPACITY
         AI_MATKEY_TRANSPARENCYFACTOR
         AI_MATKEY_BUMPSCALING
         AI_MATKEY_SHININESS
         AI_MATKEY_REFLECTIVITY
         AI_MATKEY_SHININESS_STRENGTH
         AI_MATKEY_REFRACTI
         AI_MATKEY_COLOR_DIFFUSE
         AI_MATKEY_COLOR_AMBIENT
         AI_MATKEY_COLOR_SPECULAR
         AI_MATKEY_COLOR_EMISSIVE
         AI_MATKEY_COLOR_TRANSPARENT
         AI_MATKEY_COLOR_REFLECTIVE
         AI_MATKEY_GLOBAL_BACKGROUND_IMAGE
         AI_MATKEY_GLOBAL_SHADERLANG
         AI_MATKEY_SHADER_VERTEX
         AI_MATKEY_SHADER_FRAGMENT
         AI_MATKEY_SHADER_GEO
         AI_MATKEY_SHADER_TESSELATION
         AI_MATKEY_SHADER_PRIMITIVE
         AI_MATKEY_SHADER_COMPUTE
         */

        if ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_COLOR_DIFFUSE, Color3 ) == AI_SUCCESS )
            Descriptor.Diffuse = glm::vec3 ( Color3.r, Color3.g, Color3.b );
        if ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_COLOR_AMBIENT, Color3 ) == AI_SUCCESS )
            Descriptor.Ambient = glm::vec3 ( Color3.r, Color3.g, Color3.b );
        if ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_COLOR_SPECULAR, Color3 ) == AI_SUCCESS )
            Descriptor.Specular = glm::vec3 ( Color3.r, Color3.g, Color3.b );
        if ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_COLOR_EMISSIVE, Color3 ) == AI_SUCCESS )
            Descriptor.Emissive = glm::vec3 ( Color3.r, Color3.g, Color3.b );
        if ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_SHININESS, FloatValue ) == AI_SUCCESS )
            Descriptor.Shininess = FloatValue;
        if ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_OPACITY, FloatValue ) == AI_SUCCESS )
            Descriptor.Opacity = FloatValue;

        /* Read material textures
        aiTextureType_DIFFUSE
        aiTextureType_SPECULAR
        aiTextureType_AMBIENT
        aiTextureType_EMISSIVE
        aiTextureType_HEIGHT
        aiTextureType_NORMALS
        aiTextureType_SHININESS
        aiTextureType_OPACITY
        aiTextureType_DISPLACEMENT
        aiTextureType_LIGHTMAP
        aiTextureType_REFLECTION

        // PBR Materials
        aiTextureType_BASE_COLOR
        aiTextureType_NORMAL_CAMERA
        aiTextureType_EMISSION_COLOR
        aiTextureType_METALNESS
        aiTextureType_DIFFUSE_ROUGHNESS
        aiTextureType_AMBIENT_OCCLUSION
         */

        for ( unsigned TypeIterator = 0; TypeIterator < ( unsigned ) Material::MaterialTextureType::COUNT; ++TypeIterator )
            {
            aiTextureType AssimpType = Translate ( ( Material::MaterialTextureType ) TypeIterator );
            if ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_TEXTURE ( AssimpType, 0 ), String ) != AI_SUCCESS ) continue;

            Descriptor.Textures[TypeIterator] = std::string ( String.C_Str() );
            aiTextureMapMode HWrap, VWrap;
            CrossRenderer::TextureWrapSettings WrapSettings ( CrossRenderer::TextureWrapMode::Repeat );
            if ( ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_MAPPINGMODE_U ( AssimpType, 0 ), HWrap ) == AI_SUCCESS ) &&
                    ( Scene->mMaterials[MatIndex]->Get ( AI_MATKEY_MAPPINGMODE_V ( AssimpType, 0 ), VWrap ) == AI_SUCCESS ) )
                WrapSettings = CrossRenderer::TextureWrapSettings ( Translate ( HWrap ), Translate ( VWrap ) );
            Descriptor.WrapSettings[TypeIterator] = WrapSettings;
            }
        Materials.push_back ( Descriptor );
        LogMaterial ( Descriptor );
        }

    if ( Scene->mNumMeshes )
        {
        ModelDescriptor Descriptor;
        Descriptor.Name = FullPath;

        for ( unsigned MeshIndex = 0; MeshIndex < Scene->mNumMeshes; ++MeshIndex )
            {
            ModelDescriptor::MeshDescriptor MeshDescriptor;

            // Vertex, texcoord and normal data
            MeshDescriptor.Vertices.assign ( ( glm::vec3* ) & ( Scene->mMeshes[MeshIndex]->mVertices[0] ), ( glm::vec3* ) & ( Scene->mMeshes[MeshIndex]->mVertices[Scene->mMeshes[MeshIndex]->mNumVertices] ) );
            MeshDescriptor.Normals.assign ( ( glm::vec3* ) & ( Scene->mMeshes[MeshIndex]->mNormals[0] ), ( glm::vec3* ) & ( Scene->mMeshes[MeshIndex]->mNormals[Scene->mMeshes[MeshIndex]->mNumVertices] ) );
            if ( Scene->mMeshes[MeshIndex]->HasTextureCoords ( 0 ) )
                {
                for ( unsigned cont = 0; cont < Scene->mMeshes[MeshIndex]->mNumVertices; ++cont )
                    {
                    MeshDescriptor.TexCoords.push_back ( glm::vec2 ( Scene->mMeshes[MeshIndex]->mTextureCoords[0][cont].x, Scene->mMeshes[MeshIndex]->mTextureCoords[0][cont].y ) );
                    }
                }

            // Index data
            for ( unsigned cont = 0; cont < Scene->mMeshes[MeshIndex]->mNumFaces; ++cont )
                {
                MeshDescriptor.Indices.push_back ( Scene->mMeshes[MeshIndex]->mFaces[cont].mIndices[0] );
                MeshDescriptor.Indices.push_back ( Scene->mMeshes[MeshIndex]->mFaces[cont].mIndices[1] );
                MeshDescriptor.Indices.push_back ( Scene->mMeshes[MeshIndex]->mFaces[cont].mIndices[2] );
                }

            // Material name
            aiString InternalMaterialName;
            if ( Scene->mMaterials[Scene->mMeshes[MeshIndex]->mMaterialIndex]->Get ( AI_MATKEY_NAME, InternalMaterialName ) == AI_SUCCESS )
                MeshDescriptor.MaterialName = RelativePath + std::string ( "/" ) + std::string ( InternalMaterialName.C_Str () );
            else
                return false;

            // Mesh name
            if ( Scene->mMeshes[MeshIndex]->mName.length != 0 )
                MeshDescriptor.Name = RelativePath + std::string ( "/" ) + std::string ( Scene->mMeshes[MeshIndex]->mName.C_Str () );
            else
                return false;

            Descriptor.Meshes.push_back ( MeshDescriptor );
            LOG_DEBUG ( "Loaded mesh '%s'", MeshDescriptor.Name.c_str() );
            }

        Models.push_back ( Descriptor );
        }

    Importer.FreeScene ();
    Importer.SetIOHandler ( nullptr );
    LOG_DEBUG ( "File load complete" );
    return true;
    }
std::string ModelLoader::GetPath ( void )
    {
    return FullPath;
    }

void ModelLoader::LogMaterial ( const MaterialDescriptor &Descriptor )
    {
    LOG_DEBUG ( "Material '%s'", Descriptor.Name.c_str() );
#define LOG_COLOR(color) LOG_DEBUG ( "    %s - %f %f %f", #color, Descriptor.color.r, Descriptor.color.g, Descriptor.color.b );
    LOG_COLOR ( Ambient );
    LOG_COLOR ( Diffuse );
    LOG_COLOR ( Specular );
    LOG_COLOR ( Emissive );
#undef LOG_COLOR

#define LOG_TEXTUREMAP(color) LOG_DEBUG ( "    %s - %s", #color, Descriptor.Textures[(int)Material::MaterialTextureType::color].c_str());
    LOG_TEXTUREMAP ( Ambient );
    LOG_TEXTUREMAP ( Diffuse );
    LOG_TEXTUREMAP ( Specular );
    LOG_TEXTUREMAP ( Emissive );
    LOG_TEXTUREMAP ( Height );
    LOG_TEXTUREMAP ( Normal );
    LOG_TEXTUREMAP ( Shininess );
    LOG_TEXTUREMAP ( Opacity );
    LOG_TEXTUREMAP ( Displacement );
    LOG_TEXTUREMAP ( Lightmap );
    LOG_TEXTUREMAP ( Reflection );
#undef LOG_TEXTUREMAP
    }
