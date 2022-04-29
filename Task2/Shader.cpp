#include "Shader.h"

PhongShader::PhongShader()
{
	pPxSh = nullptr;
	pVxSh = nullptr;
	pVtxLayout = nullptr;
	pMVP_CB = nullptr;
	pLightCB  = nullptr;
	pCameraCB = nullptr;
}

PhongShader::~PhongShader()
{
	Release();
}

void PhongShader::initCB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
{
	D3D11_BUFFER_DESC ds;
	memset(&ds, 0, sizeof(ds));

	ds.ByteWidth = sizeof(MVP);
	ds.Usage = D3D11_USAGE_DEFAULT;
	ds.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hRes = _pDevice->CreateBuffer(&ds, NULL, &pMVP_CB);
	_pDeviceContext->VSSetConstantBuffers(0, 1, &pMVP_CB);
	ds.ByteWidth = sizeof(Light);
	hRes = _pDevice->CreateBuffer(&ds, NULL, &pLightCB);
	_pDeviceContext->PSSetConstantBuffers(0, 1, &pLightCB);
	
	ds.ByteWidth = sizeof(vec4);
	hRes = _pDevice->CreateBuffer(&ds, NULL, &pCameraCB);
	_pDeviceContext->PSSetConstantBuffers(1, 1, &pCameraCB);
}

HRESULT PhongShader::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LPCWSTR _vsName, LPCWSTR _psName)
{
	HRESULT hRes = S_OK;
	ID3DBlob* pShaderBuffer = nullptr, * pErrorBuffer = nullptr;

	//VS
	hRes = D3DCompileFromFile(_vsName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", NULL, NULL, &pShaderBuffer, &pErrorBuffer);
	if (pErrorBuffer)
	{
		pErrorBuffer->Release();
		pErrorBuffer = nullptr;
	}
	if (FAILED(hRes))
	{
		if (pShaderBuffer) pShaderBuffer->Release();
		return hRes;
	}
	hRes = _pDevice->CreateVertexShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &pVxSh);
	if (FAILED(hRes))
	{
		if (pShaderBuffer) pShaderBuffer->Release();
		return hRes;
	}
	//InputLayout
	D3D11_INPUT_ELEMENT_DESC elemDesc[] = { {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0} };
	UINT numElements = ARRAYSIZE(elemDesc);
	hRes = _pDevice->CreateInputLayout(elemDesc, numElements, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), &pVtxLayout);
	if (pShaderBuffer)
	{
		pShaderBuffer->Release();
		pShaderBuffer = nullptr;
	}
	if (FAILED(hRes))
	{
		return hRes;
	}
	//PS
	hRes = D3DCompileFromFile(_psName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_4_0", NULL, NULL, &pShaderBuffer, &pErrorBuffer);
	if (pErrorBuffer)
	{
		pErrorBuffer->Release();
		pErrorBuffer = nullptr;
	}
	if (FAILED(hRes))
	{
		return hRes;
	}
	hRes = _pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &pPxSh);
	if (pShaderBuffer)
	{
		pShaderBuffer->Release();
		pShaderBuffer = nullptr;
	}
	if (FAILED(hRes))
	{
		return hRes;
	}
	initCB(_pDevice, _pDeviceContext);

	return hRes;
}

void PhongShader::Release()
{
	if (pVtxLayout)
	{
		pVtxLayout->Release();
		pVtxLayout = nullptr;
	}
	if (pVxSh)
	{
		pVxSh->Release();
		pVxSh = nullptr;
	}
	if (pPxSh)
	{
		pPxSh->Release();
		pPxSh = nullptr;
	}
	if (pMVP_CB)
	{
		pMVP_CB->Release();
		pMVP_CB = nullptr;
	}
	if (pCameraCB)
	{
		pCameraCB->Release();
		pCameraCB = nullptr;
	}
	if (pLightCB)
	{
		pLightCB->Release();
		pLightCB = nullptr;
	}
}

void PhongShader::Render(ID3D11DeviceContext* _pDeviceContext, const FrameState* _pFS)
{
	_pDeviceContext->VSSetShader(pVxSh, NULL, 0);
	_pDeviceContext->PSSetShader(pPxSh, NULL, 0);
	_pDeviceContext->IASetInputLayout(pVtxLayout);
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_pDeviceContext->UpdateSubresource(pCameraCB, 0, NULL, _pFS->pCameraPos, 0, 0);
	_pDeviceContext->UpdateSubresource(pLightCB, 0, NULL, _pFS->pLight, 0, 0);
	_pDeviceContext->UpdateSubresource(pMVP_CB, 0, NULL, _pFS->mMVP, 0, 0);
	_pDeviceContext->DrawIndexed(_pFS->indicesCount, 0, 0);
}
