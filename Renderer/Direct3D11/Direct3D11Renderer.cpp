#include "Direct3D11Renderer.hpp"
#include "Direct3D11Internals.hpp"
#include "../WindowManager/WindowManager.hpp"
#include "../Internal/Logger.hpp"
#include "../Renderer.hpp"
#include <algorithm>

namespace CrossRenderer
{
namespace Direct3D11
{
RendererConfiguration Configuration;
RenderWindowHandle ActiveWindow;

bool InitializeRenderer ( const RendererConfiguration &NewConfiguration )
	{
	Configuration = NewConfiguration;
	for ( auto &Iterator : NewConfiguration.InitialWindowConfigurations )
		{
		RenderWindowHandle NewWindow = WindowManager::CreateNewWindow ( Iterator );
		if ( !NewWindow )
			return false;
		}
	ActiveWindow = *( WindowManager::WindowList.begin () );

	DXGI_SWAP_CHAIN_DESC swap_chain_descr = { 0 };
	swap_chain_descr.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_descr.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_descr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swap_chain_descr.SampleDesc.Count = 1;
	swap_chain_descr.SampleDesc.Quality = 0;
	swap_chain_descr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_descr.BufferCount = 1;
	swap_chain_descr.OutputWindow = (HWND) WindowManager::GetPlatformWindowHandle ( ActiveWindow );
	swap_chain_descr.Windowed = true;

	D3D_FEATURE_LEVEL feature_level;
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT hr = D3D11CreateDeviceAndSwapChain (
	  nullptr,
	  D3D_DRIVER_TYPE_HARDWARE,
	  nullptr,
	  flags,
	  nullptr,
	  0,
	  D3D11_SDK_VERSION,
	  &swap_chain_descr,
	  &D3D11Information.swap_chain_ptr,
	  &D3D11Information.DevicePointer,
	  &feature_level,
	  &D3D11Information.DeviceContextPointer );
	if ( ( hr != S_OK ) ||
		 ( D3D11Information.swap_chain_ptr != nullptr ) ||
		 ( D3D11Information.DevicePointer != nullptr ) ||
		 ( D3D11Information.DeviceContextPointer != nullptr ) )
		return false;

	FramebufferInfo	NewFramebufferInfo;	 
	hr = D3D11Information.swap_chain_ptr->GetBuffer (
	  0,
	  __uuidof( ID3D11Texture2D ),
	  (void **) &NewFramebufferInfo.framebuffer );
	assert ( SUCCEEDED ( hr ) );

	hr = D3D11Information.DevicePointer->CreateRenderTargetView (
	  NewFramebufferInfo.framebuffer, 0, &NewFramebufferInfo.render_target_view_ptr );
	assert ( SUCCEEDED ( hr ) );
	NewFramebufferInfo.framebuffer->Release ();

	return true;
	}

bool ShutdownRenderer ( void )
	{
	return true;
	}

bool StartRenderToFramebuffer ( const FramebufferHandle &Handle )
	{
	FramebufferInfo *FramebufferInformation = &Framebuffers[Handle.GetKey ()];
	// Bind render target
	D3D11Information.DeviceContextPointer->OMSetRenderTargets ( 1, &FramebufferInformation->render_target_view_ptr, nullptr );

	// Set viewport
	auto viewport = CD3D11_VIEWPORT ( 0.f, 0.f, (float)FramebufferInformation->Dimensions.x, (float)FramebufferInformation->Dimensions.y);
	D3D11Information.DeviceContextPointer->RSSetViewports ( 1, &viewport );

	// Set the background color
	D3D11Information.DeviceContextPointer->ClearRenderTargetView ( FramebufferInformation->render_target_view_ptr, glm::value_ptr(FramebufferInformation->ClearColor) );

	return false;
	}

bool DisplayFramebuffer ( const FramebufferHandle &, const RenderWindowHandle & )
	{
	D3D11Information.swap_chain_ptr->Present ( 0, 0 );
	return false;
	}

bool StartRenderToWindow ( const RenderWindowHandle & )
	{
	return false;
	}

bool DisplayWindow ( const RenderWindowHandle & )
	{
	D3D11Information.swap_chain_ptr->Present ( 0, 0 );
	return false;
	}

bool RunCommand ( const RenderCommand &Command )
	{
	if ( !Command.Shader )
		return false;
	if ( !Command.VertexCount )
		return false;
	return false;
	}

}
}
