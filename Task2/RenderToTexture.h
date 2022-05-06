#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Render.h"

using namespace DirectX;

class RenderTexture
{
	ID3D11Texture2D* pTargetTexture;
	ID3D11Texture2D* pDepthStencilBuffer;
	ID3D11DepthStencilView* pDepthStecncilView;
	ID3D11RenderTargetView* pRTV;
	ID3D11ShaderResourceView* pSRV;
	D3D11_VIEWPORT mViewport;
	mtx mxProjection;
public:
	RenderTexture();
	~RenderTexture();
	HRESULT Initialize(ID3D11Device* _pDevice,	UINT _textureWidth, UINT _textureHeight, float _screenDepth, float _screenNear);
	void setRenderTarget(ID3D11DeviceContext* _pDeviceContext);
	ID3D11ShaderResourceView* getShaderResourceView();
	ID3D11Texture2D* getTargetTexture();
	void getProjectionMx(mtx& mProj);
};