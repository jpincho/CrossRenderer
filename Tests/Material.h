#pragma once
#include <string>
#include "../TextureDefinitions.h"

class Material
    {
    public:
        enum class MaterialTextureType
            {
            Diffuse,
            Specular,
            Ambient,
            Emissive,
            Height,
            Normal,
            Shininess,
            Opacity,
            Displacement,
            Lightmap,
            Reflection,
            COUNT
            };

    protected:
    public:
        CrossRenderer::TextureBindSettings Textures[ ( int ) MaterialTextureType::COUNT];
        std::string Name;
        glm::vec3 Diffuse, Specular, Emissive;
        float Shininess;

        Material ( void );
    };
