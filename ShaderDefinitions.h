#pragma once
#include "GLMHeaders.h"
#include "HandleTemplate.h"
#include "ShaderBufferDefinitions.h"

namespace CrossRenderer
{
typedef struct
    {
    std::string VertexShader, GeometryShader, FragmentShader;
    } ShaderCode;

enum class ShaderUniformType
    {
    Bool = 0,
    Bool2,
    Bool3,
    Bool4,
    Float,
    Float2,
    Float3,
    Float4,
    UnsignedInteger,
    UnsignedInteger2,
    UnsignedInteger3,
    UnsignedInteger4,
    Integer,
    Integer2,
    Integer3,
    Integer4,
    Matrix2,
    Matrix3,
    Matrix4,

    Sampler2D,
    Block,
    Unknown
    };
typedef ShaderUniformType ShaderAttributeType;

struct ShaderTag {};
typedef HandleTemplate <ShaderTag> ShaderHandle;
typedef HandleTemplate <void> ShaderUniformHandle;
typedef HandleTemplate <void> ShaderAttributeHandle;

typedef struct
    {
    typedef struct
        {
        std::string Name;
        ShaderUniformHandle Handle;
        ShaderUniformType Type;
        } ShaderUniformInformation;
    typedef struct
        {
        std::string Name;
        ShaderAttributeHandle Handle;
        ShaderAttributeType Type;
        } ShaderAttributeInformation;
    std::vector <ShaderUniformInformation> Uniforms;
    std::vector <ShaderAttributeInformation> Attributes;
    ShaderHandle Handle;
    } ShaderInformation;

typedef struct ShaderUniformValue
    {
    union
        {
        bool BoolValue;
        glm::bvec2 Bool2Value;
        glm::bvec3 Bool3Value;
        glm::bvec4 Bool4Value;

        float FloatValue;
        glm::vec2 Float2Value;
        glm::vec3 Float3Value;
        glm::vec4 Float4Value;

        uint32_t UnsignedIntegerValue;
        glm::uvec2 UnsignedInteger2Value;
        glm::uvec3 UnsignedInteger3Value;
        glm::uvec4 UnsignedInteger4Value;

        int32_t IntegerValue;
        glm::ivec2 Integer2Value;
        glm::ivec3 Integer3Value;
        glm::ivec4 Integer4Value;

        glm::mat2 Matrix2Value;
        glm::mat3 Matrix3Value;
        glm::mat4 Matrix4Value;

        ShaderBufferHandle BlockValue;
        };
    ShaderUniformType Type;
#define CONSTRUCTOR(TYPENAME, TYPE)\
    ShaderUniformValue ( const TYPE Value )\
        {\
        TYPENAME##Value = Value;\
        Type = ShaderUniformType::TYPENAME;\
        }

    CONSTRUCTOR ( Bool, bool )
    CONSTRUCTOR ( Bool2, glm::bvec2 )
    CONSTRUCTOR ( Bool3, glm::bvec3 )
    CONSTRUCTOR ( Bool4, glm::bvec4 )

    CONSTRUCTOR ( Float, float )
    CONSTRUCTOR ( Float2, glm::vec2 )
    CONSTRUCTOR ( Float3, glm::vec3 )
    CONSTRUCTOR ( Float4, glm::vec4 )

    CONSTRUCTOR ( UnsignedInteger, uint32_t )
    CONSTRUCTOR ( UnsignedInteger2, glm::uvec2 )
    CONSTRUCTOR ( UnsignedInteger3, glm::uvec3 )
    CONSTRUCTOR ( UnsignedInteger4, glm::uvec4 )

    CONSTRUCTOR ( Integer, int32_t )
    CONSTRUCTOR ( Integer2, glm::ivec2 )
    CONSTRUCTOR ( Integer3, glm::ivec3 )
    CONSTRUCTOR ( Integer4, glm::ivec4 )

    CONSTRUCTOR ( Matrix2, glm::mat2 )
    CONSTRUCTOR ( Matrix3, glm::mat3 )
    CONSTRUCTOR ( Matrix4, glm::mat4 )

    CONSTRUCTOR ( Block, ShaderBufferHandle )
#undef CONSTRUCTOR

    ShaderUniformValue ( void )
        {
        Type = ShaderUniformType::Unknown;
        }
    bool operator == ( const ShaderUniformValue &Other ) const
        {
        switch ( Type )
            {
            case ShaderUniformType::Bool:
                return BoolValue == Other.BoolValue;
            case ShaderUniformType::Bool2:
                return Bool2Value == Other.Bool2Value;
            case ShaderUniformType::Bool3:
                return Bool3Value == Other.Bool3Value;
            case ShaderUniformType::Bool4:
                return Bool4Value == Other.Bool4Value;
            case ShaderUniformType::Float:
                return FloatValue == Other.FloatValue;
            case ShaderUniformType::Float2:
                return Float2Value == Other.Float2Value;
            case ShaderUniformType::Float3:
                return Float3Value == Other.Float3Value;
            case ShaderUniformType::Float4:
                return Float4Value == Other.Float4Value;
            case ShaderUniformType::UnsignedInteger:
                return UnsignedIntegerValue == Other.UnsignedIntegerValue;
            case ShaderUniformType::UnsignedInteger2:
                return UnsignedInteger2Value == Other.UnsignedInteger2Value;
            case ShaderUniformType::UnsignedInteger3:
                return UnsignedInteger3Value == Other.UnsignedInteger3Value;
            case ShaderUniformType::UnsignedInteger4:
                return UnsignedInteger4Value == Other.UnsignedInteger4Value;
            case ShaderUniformType::Integer:
                return IntegerValue == Other.IntegerValue;
            case ShaderUniformType::Integer2:
                return Integer2Value == Other.Integer2Value;
            case ShaderUniformType::Integer3:
                return Integer3Value == Other.Integer3Value;
            case ShaderUniformType::Integer4:
                return Integer4Value == Other.Integer4Value;
            case ShaderUniformType::Matrix2:
                return Matrix2Value == Other.Matrix2Value;
            case ShaderUniformType::Matrix3:
                return Matrix3Value == Other.Matrix3Value;
            case ShaderUniformType::Matrix4:
                return Matrix4Value == Other.Matrix4Value;
            case ShaderUniformType::Block:
                return BlockValue == Other.BlockValue;
            default:
                return true;
            }
        }
    } ShaderUniformValue;
}
