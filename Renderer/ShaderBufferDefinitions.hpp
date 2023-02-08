#pragma once
#include "../RendererHandleTemplate.hpp"
#include "../GLMHeaders.hpp"
#include <vector>

namespace CrossRenderer
{
enum class ShaderBufferComponentType : uint8_t
    {
    Float = 0,
    Unsigned8,
    Unsigned16,
    Unsigned32
    };

enum class ShaderBufferAccessType : uint8_t
    {
    Static,
    Dynamic,
    Stream
    };

enum class ShaderBufferMapAccessType : uint8_t
	{
	ReadOnly,
	WriteOnly,
	ReadWrite
	};

enum class ShaderBufferType : uint8_t
    {
    Array,
    Uniform
    };

struct ShaderBufferDataStream
    {
    ShaderBufferDataStream() = default;
    ShaderBufferDataStream ( const ShaderBufferHandle NewBufferHandle, const size_t NewStartOffset, const size_t NewStride, const ShaderBufferComponentType NewComponentType, const size_t NewComponentsPerElement, const bool ShouldNormalizeData = false )
        {
        BufferHandle = NewBufferHandle;
        StartOffset = NewStartOffset;
        Stride = NewStride;
        ComponentType = NewComponentType;
        ComponentsPerElement = NewComponentsPerElement;
        NormalizeData = ShouldNormalizeData;
        }
    ShaderBufferHandle BufferHandle;
    size_t StartOffset;
    size_t Stride;
    ShaderBufferComponentType ComponentType;
    size_t ComponentsPerElement;
    bool NormalizeData;
	};

struct ShaderBufferDescriptor
    {
	size_t DataSize, Capacity;
    const void *Data;
    ShaderBufferAccessType AccessType;
    ShaderBufferType BufferType;

    ShaderBufferDescriptor ( void )
        {
        Data = nullptr;
		DataSize = Capacity = 0;
        AccessType = ShaderBufferAccessType::Static;
        BufferType = ShaderBufferType::Array;
        }

	ShaderBufferDescriptor ( const void *NewData, const size_t NewDataSize, const ShaderBufferType NewShaderBufferType = ShaderBufferType::Array, const ShaderBufferAccessType NewAccessType = ShaderBufferAccessType::Static )
        {
		Data = reinterpret_cast <const void *> ( NewData );
		DataSize = Capacity = NewDataSize;
        AccessType = NewAccessType;
		BufferType = NewShaderBufferType;
        }

    template <typename Type>
	ShaderBufferDescriptor ( const std::vector <Type> &NewData, const ShaderBufferType NewShaderBufferType = ShaderBufferType::Array, const ShaderBufferAccessType NewAccessType = ShaderBufferAccessType::Static )
        {
		Data = reinterpret_cast <const void *> ( NewData.data () );
		DataSize = Capacity = NewData.size () * sizeof ( Type );
        AccessType = NewAccessType;
		BufferType = NewShaderBufferType;
        }
	};
}

