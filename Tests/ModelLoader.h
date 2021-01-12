#pragma once
#include "Material.h"

typedef struct
    {
    std::string Name;
    glm::vec3 Ambient, Diffuse, Specular, Emissive;

    std::string Textures[ ( int ) Material::MaterialTextureType::COUNT];
    CrossRenderer::TextureWrapSettings WrapSettings[ ( int ) Material::MaterialTextureType::COUNT];
    CrossRenderer::TextureFilterSettings FilterSettings[ ( int ) Material::MaterialTextureType::COUNT];
    float Shininess, Opacity;
    } MaterialDescriptor;

typedef struct
    {
    std::string Name;
    typedef struct
        {
        std::vector <glm::vec3> Vertices, Normals;
        std::vector <glm::vec2> TexCoords;
        std::vector <uint32_t> Indices;
        std::string Name;
        std::string MaterialName;
        } MeshDescriptor;
    std::vector <MeshDescriptor> Meshes;
    } ModelDescriptor;

class ModelLoader
    {
    protected:
        std::string FullPath, RelativePath, BasePath;
        FILE *File;

    public:
        std::vector<MaterialDescriptor> Materials;
        std::vector<ModelDescriptor> Models;
        ModelLoader ( void );
        virtual bool Set ( const std::string &NewBasePath, const std::string &NewRelativePath );
        virtual bool Load ( void );
        std::string GetPath ( void );

        void LogMaterial ( const MaterialDescriptor &Descriptor );
    };
