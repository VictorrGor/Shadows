#pragma once
#include <d3dcompiler.h>
#include "Render.h"
#include "Light.h"
#include "Camera.h"

struct MVP;
struct FrameState;


class PhongShader
{
	ID3D11PixelShader* pPxSh;
	ID3D11VertexShader* pVxSh;
	ID3D11InputLayout* pVtxLayout;
	ID3D11Buffer* pMVP_CB;
	ID3D11Buffer* pLightCB;
	ID3D11Buffer* pCameraCB;
	
public:
	PhongShader();
	~PhongShader();
	HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LPCWSTR _vsName, LPCWSTR _psName);
	void Release();
	void Render(ID3D11DeviceContext* _pDeviceContext, const FrameState* _pFS);
	void initCB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
};

class DepthShader
{
	ID3D11PixelShader* pPxSh;
	ID3D11VertexShader* pVxSh;
	ID3D11InputLayout* pVtxLayout;
	ID3D11Buffer* pMVP_CB;
public:
	DepthShader();
	~DepthShader();
	HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LPCWSTR _vsName, LPCWSTR _psName);
	void Release();
	void Render(ID3D11DeviceContext* _pDeviceContext, const FrameState* _pFS);
	void initCB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
};