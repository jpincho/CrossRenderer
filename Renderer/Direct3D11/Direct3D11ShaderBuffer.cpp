#include "Direct3D11ShaderBuffer.hpp"
#include "Direct3D11Internals.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
ShaderBufferHandle CreateShaderBuffer ( const ShaderBufferDescriptor CreationParameters )
    {
    ShaderBufferInfo NewBufferInformation;
	D3D11_BUFFER_DESC Description;

	Description.ByteWidth = (unsigned) CreationParameters.DataSize;
	Description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Description.Usage = D3D11_USAGE_DEFAULT;
	Description.CPUAccessFlags = 0;
	Description.MiscFlags = 0;
	Description.StructureByteStride = 0;
	HRESULT Result = D3D11Information.DevicePointer->CreateBuffer ( &Description, (D3D11_SUBRESOURCE_DATA*) CreationParameters.Data, &NewBufferInformation.BufferPointer );
	if ( Result != S_OK )
		return ShaderBufferHandle::Invalid;

    ShaderBufferHandle NewHandle ( ShaderBuffers.GetFreeIndex() );
    ShaderBuffers[NewHandle.GetKey ()] = NewBufferInformation;
    return NewHandle;
    }

bool DeleteShaderBuffer ( const ShaderBufferHandle Handle )
    {
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];
	ShaderBufferInformation->BufferPointer->Release ();
	ShaderBuffers.ReleaseIndex ( Handle.GetKey () );
    return true;
    }

bool ChangeShaderBufferContents ( const ShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize )
    {
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];
	D3D11_BOX destRegion;
	destRegion.left = (unsigned) Offset;
	destRegion.right = (unsigned) Offset + (unsigned) DataSize;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.front = 0;
	destRegion.back = 1;
	D3D11Information.DeviceContextPointer->UpdateSubresource ( ShaderBufferInformation->BufferPointer, 0, &destRegion, Data, 0, 0 );
	return true;
    }

void *MapShaderBuffer ( const ShaderBufferHandle Handle, const ShaderBufferMapAccessType  )
    {
	ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;
	HRESULT Result = D3D11Information.DeviceContextPointer->Map ( ShaderBufferInformation->BufferPointer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource );
	if ( Result != S_OK )
		return nullptr;

	ShaderBufferInformation->MappedPointer = MappedSubResource.pData;
	return ShaderBufferInformation->MappedPointer;
    }

bool UnmapShaderBuffer ( const ShaderBufferHandle Handle )
    {
    ShaderBufferInfo *ShaderBufferInformation = &ShaderBuffers[Handle.GetKey ()];

    if ( ShaderBufferInformation->MappedPointer == nullptr )
        return true;

	D3D11Information.DeviceContextPointer->Unmap ( ShaderBufferInformation->BufferPointer, 0 );

    ShaderBufferInformation->MappedPointer = nullptr;
    return true;
    }

const ShaderBufferDescriptor GetShaderBufferDescriptor ( const ShaderBufferHandle Handle )
	{
	return ShaderBuffers[Handle.GetKey()];
	}
}
}
