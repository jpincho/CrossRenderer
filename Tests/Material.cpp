#include "Material.h"

Material::Material ( void ) : Diffuse ( 1, 1, 1 ), Specular ( 1, 1, 1 ), Emissive ( 0, 0, 0 )
    {
    Shininess = 200.0f;
    }
