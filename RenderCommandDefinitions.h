#pragma once
#include <vector>
#include "ShaderBufferDefinitions.h"
#include "ShaderDefinitions.h"
#include "TextureDefinitions.h"
#include "RendererDefinitions.h"

namespace CrossRenderer
{
typedef struct ShaderTextureBindPair
    {
    CrossRenderer::ShaderUniformHandle UniformHandle;
    CrossRenderer::TextureBindSettings BindSettings;
    ShaderTextureBindPair ( const CrossRenderer::ShaderUniformHandle &NewUniformHandle, const CrossRenderer::TextureBindSettings NewBindSettings ) : UniformHandle ( NewUniformHandle ), BindSettings ( NewBindSettings )
        {
        }
    } ShaderTextureBindPair;

typedef struct ShaderBufferBindPair
    {
    ShaderBufferBindPair ( const ShaderAttributeHandle &NewAttributeHandle, const ShaderBufferDataStream &NewDataStream ) : AttributeHandle ( NewAttributeHandle ), DataStream ( NewDataStream )
        {}
    ShaderAttributeHandle AttributeHandle;
    ShaderBufferDataStream DataStream;
    } ShaderBufferBindPair;

typedef struct ShaderUniformValuePair
    {
    ShaderUniformValuePair ( const ShaderUniformHandle &NewUniformHandle, const ShaderUniformValue &NewUniformValue ) : UniformHandle ( NewUniformHandle ), UniformValue ( NewUniformValue )
        {}
    ShaderUniformHandle UniformHandle;
    ShaderUniformValue UniformValue;
    } ShaderUniformValuePair;

typedef struct
    {
    std::vector <ShaderBufferBindPair> ShaderBufferBindings;
    std::vector <ShaderTextureBindPair> TextureBindings;
    std::vector <ShaderUniformValuePair> UniformValues;
    ShaderHandle Shader;
    ShaderBufferHandle IndexBuffer;
    ShaderBufferDataStream IndexBufferStream;
    PrimitiveType Primitive;
    unsigned VertexCount, StartVertex;

    RenderState State;
    } RenderCommand;
}
