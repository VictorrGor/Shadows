#include "RenderToTexture.h"

RenderTexture::RenderTexture()
{
	  pDepthStecncilView = nullptr;
	  pTargetTexture = nullptr;
}

RenderTexture::~RenderTexture()
{
	if (pTargetTexture)
	{
		pTargetTexture->Release();
		pTargetTexture = nullptr;
	}
	if (pDepthStencilBuffer)
	{
		pDepthStencilBuffer->Release();
		pDepthStencilBuffer = nullptr;
	}
	if (pDepthStecncilView)
	{
		pDepthStecncilView->Release();
		pDepthStecncilView = nullptr;
	}
	if (pRTV)
	{
		pRTV->Release();
		pRTV = nullptr;
	}
	if (pSRV)
	{
		pSRV->Release();
		pSRV = nullptr;
	}
}

HRESULT RenderTexture::Initialize(ID3D11Device* _pDevice, UINT _textureWidth, UINT _textureHeight, float _screenDepth, float _screenNear)
{
	HRESULT hRes = S_OK;
	D3D11_TEXTURE2D_DESC texture_desc; 
	memset(&texture_desc, 0, sizeof(texture_desc));
	texture_desc.Height = _textureHeight;
	texture_desc.Width = _textureWidth;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	hRes = _pDevice->CreateTexture2D(&texture_desc, NULL, &pTargetTexture);
	if (FAILED(hRes)) return hRes;

	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
	memset(&rtv_desc, 0, sizeof(rtv_desc));
	rtv_desc.Format = texture_desc.Format;
	rtv_desc.Texture2D.MipSlice = 0;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hRes = _pDevice->CreateRenderTargetView(pTargetTexture, &rtv_desc, &pRTV);
	if (FAILED(hRes)) return hRes;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	memset(&srv_desc, 0, sizeof(srv_desc));
	srv_desc.Format = texture_desc.Format;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hRes = _pDevice->CreateShaderResourceView(pTargetTexture, &srv_desc, &pSRV);
	if (FAILED(hRes)) return hRes;

	D3D11_TEXTURE2D_DESC depthStencil_ds;
	memset(&depthStencil_ds, 0, sizeof(depthStencil_ds));
	depthStencil_ds.Width = _textureWidth;
	depthStencil_ds.Height = _textureHeight;
	depthStencil_ds.MipLevels = 1;
	depthStencil_ds.ArraySize = 1;
	depthStencil_ds.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencil_ds.SampleDesc.Count = 1;
	depthStencil_ds.Usage = D3D11_USAGE_DEFAULT;
	depthStencil_ds.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hRes = _pDevice->CreateTexture2D(&depthStencil_ds, NULL, &pDepthStencilBuffer);
	if (FAILED(hRes)) return hRes;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	memset(&dsv_desc, 0, sizeof(dsv_desc));
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.Texture2D.MipSlice = 0;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hRes = _pDevice->CreateDepthStencilView(pDepthStencilBuffer, &dsv_desc, &pDepthStecncilView);
	if (FAILED(hRes)) return hRes;

	mViewport.Height = (float)_textureHeight;
	mViewport.Width = (float)_textureWidth;
	mViewport.MaxDepth = 1.f;
	mViewport.MinDepth = 0;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;

	mxProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, _textureWidth / _textureHeight, _screenNear, _screenDepth); 
}

void RenderTexture::setRenderTarget(ID3D11DeviceContext* _pDeviceContext)
{
	float color[4] = { 1., 1., 1., 1. };
	_pDeviceContext->ClearRenderTargetView(pRTV, color);
	_pDeviceContext->ClearDepthStencilView(pDepthStecncilView, D3D11_CLEAR_DEPTH, 1, 0);
	_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDepthStecncilView);
	_pDeviceContext->RSSetViewports(1, &mViewport);
}

ID3D11ShaderResourceView* RenderTexture::getShaderResourceView()
{
	return pSRV;
}

ID3D11Texture2D* RenderTexture::getTargetTexture()
{
	return pTargetTexture;
}

void RenderTexture::getProjectionMx(mtx& mProj)
{
	mProj = XMMatrixTranspose(mxProjection);
}
