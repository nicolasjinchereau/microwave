/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.Graphics.Internal.HWRenderTextureD3D11;
import Microwave.Graphics.GraphicsContext;
import Microwave.Graphics.Internal.HWContextD3D11;
import Microwave.Graphics.Internal.HWTextureD3D11;
import <stdexcept>;

namespace mw {
inline namespace gfx {

HWRenderTextureD3D11::HWRenderTextureD3D11(const sptr<HWTexture>& backingTex)
    : tex(spcast<HWTextureD3D11>(backingTex))
{
	if(!tex)
		throw std::runtime_error("'tex' cannot be null");

	size = tex->size;

	auto graphics = GraphicsContext::GetCurrent();
	auto context = spcast<HWContextD3D11>(graphics->context);
	
	if(tex->format != PixelDataFormat::RGBA32)
		throw std::runtime_error("The only supported texture format is RGBA32");
	
	// create render target view
	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
	HRESULT res = context->device->CreateRenderTargetView(tex->texture.Get(), &renderTargetViewDesc, &renderTargetView);
	if(FAILED(res))
		throw std::runtime_error("render texture creation failed");

	// create depth texture
	CD3D11_TEXTURE2D_DESC depthTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, size.x, size.y, 1, 1, D3D11_BIND_DEPTH_STENCIL);
	res = context->device->CreateTexture2D(&depthTextureDesc, nullptr, &depthTexture);
	if(FAILED(res))
		throw std::runtime_error("render texture creation failed");

	// create depth stencil view
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	res = context->device->CreateDepthStencilView(depthTexture.Get(), &depthStencilViewDesc, &depthStencilView);
	if(FAILED(res))
		throw std::runtime_error("render texture creation failed");

	//// create staging texture for reading pixels
	//D3D11_TEXTURE2D_DESC textureDesc;
	//textureDesc.Width = size.x;
	//textureDesc.Height = size.y;
	//textureDesc.MipLevels = 1;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.SampleDesc.Quality = 0;
	//textureDesc.Usage = D3D11_USAGE_STAGING;
	//textureDesc.BindFlags = 0;
	//textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//textureDesc.MiscFlags = 0;

	//res = context->device->CreateTexture2D(&textureDesc, nullptr, &stagingTexture);
	//if(FAILED(res))
	//	throw std::runtime_error("render texture creation failed");
}

HWRenderTextureD3D11::~HWRenderTextureD3D11()
{
	//stagingTexture = nullptr;
	depthStencilView = nullptr;
	renderTargetView = nullptr;
	depthTexture = nullptr;
}

IVec2 HWRenderTextureD3D11::GetSize() {
    return size;
}

sptr<HWTexture> HWRenderTextureD3D11::GetTexture() {
	return tex;
}

} // gfx
} // mw
