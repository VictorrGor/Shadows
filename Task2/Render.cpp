#include "Render.h"

RenderSys::RenderSys()
{
	pDevice = nullptr;
	pDeviceContext = nullptr;
	pSwapChain = nullptr;
	pRenderTargetView = nullptr;
	pDepthBuffer = nullptr;
	mCamera = nullptr;
	pLightSh = nullptr;
	pZSh = nullptr;
	pFS = nullptr;
	pRtT = nullptr;
}

RenderSys::~RenderSys()
{
	Release();
}

HRESULT RenderSys::Initialize(const HWND& hWnd)
{
	HRESULT hRes = S_OK;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
	
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	RECT rc; 
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;


	DXGI_SWAP_CHAIN_DESC ds;
	ZeroMemory(&ds, sizeof(ds));
	ds.BufferCount = 2;
	ds.BufferDesc.Width = width;
	ds.BufferDesc.Height = height;
	ds.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ds.BufferDesc.RefreshRate.Numerator = 60;
	ds.BufferDesc.RefreshRate.Denominator = 1;
	ds.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	ds.OutputWindow = hWnd;
	ds.SampleDesc.Count = 1;
	ds.SampleDesc.Quality = 0;
	ds.Windowed = TRUE;

	hRes = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
		&ds, &pSwapChain, &pDevice, &featureLevel, &pDeviceContext);
	if (FAILED(hRes)) return hRes;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hRes = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hRes))
		return hRes;

	hRes = pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hRes))
		return hRes;

	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
	depthTextureDesc.Width = width;
	depthTextureDesc.Height = height;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* DepthStencilTexture;
	hRes = pDevice->CreateTexture2D(&depthTextureDesc, NULL, &DepthStencilTexture);

	if (FAILED(hRes))
		return hRes;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = depthTextureDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	hRes = pDevice->CreateDepthStencilView(DepthStencilTexture, &dsvDesc, &pDepthBuffer);
	DepthStencilTexture->Release();

	if (FAILED(hRes))
		return hRes;

	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthBuffer);

	// Setup the viewport
	mVP.Width = (FLOAT)width;
	mVP.Height = (FLOAT)height;
	mVP.MinDepth = 0.0f;
	mVP.MaxDepth = 1.0f;
	mVP.TopLeftX = 0;
	mVP.TopLeftY = 0;
	pDeviceContext->RSSetViewports(1, &mVP);

	mCamera = new Camera({ 2,0,0 }, { 0,0,0 }, { 0,1,0 }, width, height);

	pLightSh = new PhongShader();
	pLightSh->Initialize(pDevice, pDeviceContext, L"shaders/VertexShader.hlsl", L"shaders/PixelShader.hlsl");

	pZSh = new DepthShader();
	pZSh->Initialize(pDevice, pDeviceContext, L"shaders/DepthVS.hlsl", L"shaders/DepthPS.hlsl");
	
	float shadowTextureWidth = (FLOAT)width;//1024; 
	float shadowTextureHeight = (FLOAT)height;//1024;
	pFS = new FrameState(); 
	hRes = pFS->Initialize(pDevice, mCamera, shadowTextureWidth, shadowTextureHeight);
	if (FAILED(hRes)) return hRes;

	pRtT = new RenderTexture();
	mtx mxProjective; float fFar, fNear;
	mCamera->getProjectionMxAndAttributes(mxProjective, fNear, fFar);
	pRtT->Initialize(pDevice, shadowTextureWidth, shadowTextureHeight, fFar, fNear);
	
	return hRes;
}

void RenderSys::Release()
{
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		(*it)->Release();
		delete (*it);
	}
	objects.clear();

	if (pDepthBuffer)
	{
		pDepthBuffer->Release();
		pDepthBuffer = nullptr;
	}
	if (pRenderTargetView)
	{
		pRenderTargetView->Release();
		pRenderTargetView = nullptr;
	}
	if (pSwapChain)
	{
		pSwapChain->Release();
		pSwapChain = nullptr;
	}
	if (pDeviceContext)
	{
		pDeviceContext->Release();
		pDeviceContext = nullptr;
	}
	if (pDevice)
	{
		pDevice->Release();
		pDevice = nullptr;
	}
	if (pLightSh)
	{
		delete pLightSh;
		pLightSh = nullptr;
	}
	if (pZSh)
	{
		delete pZSh;
		pZSh = nullptr;
	}
	if (pFS)
	{
		delete pFS;
		pFS = nullptr;
	}
	if (pRtT)
	{
		delete pRtT;
		pRtT = nullptr;
	}
}

XMVECTOR calcTriangleNormal(const Vertex& _vt1, const Vertex& _vt2, const Vertex& _vt3)
{
	XMVECTOR u, v, pt;
	pt = XMLoadFloat3(&_vt1.position);
	u  = XMLoadFloat3(&_vt2.position);
	u  = XMVectorSubtract(u, pt);
	v  = XMLoadFloat3(&_vt3.position);
	v  = XMVectorSubtract(v, pt);
	return XMVector3Normalize(XMVector3Cross(u, v));
}

void calcWeightedNormals(Vertex* _vertices, UINT _vtxCount, const UINT* _indicies, UINT _idxCount)
{
	for (UINT iVtx = 0; iVtx < _vtxCount; ++iVtx)
	{
		_vertices[iVtx].normal = { 0,0,0 };
	}
	if (_idxCount % 3 != 0) return;

	UINT* linkCounter = new UINT[_vtxCount];
	memset(linkCounter, 0, sizeof(UINT) * _vtxCount);

	Vertex *vx1, *vx2, *vx3;
	DirectX::XMVECTOR vNormal, vBuf;
	for (UINT iIdx = 0; iIdx < _idxCount; iIdx += 3)
	{
		vx1 = &_vertices[_indicies[iIdx]];
		vx2 = &_vertices[_indicies[iIdx + 1]];
		vx3 = &_vertices[_indicies[iIdx + 2]];
		vNormal = calcTriangleNormal(*vx1, *vx2, *vx3);
		vBuf = XMLoadFloat3(&vx1->normal);
		XMStoreFloat3(&vx1->normal, DirectX::XMVectorAdd(vNormal, vBuf));
		vBuf = XMLoadFloat3(&vx2->normal);
		XMStoreFloat3(&vx2->normal, DirectX::XMVectorAdd(vNormal, vBuf));
		vBuf = XMLoadFloat3(&vx3->normal);
		XMStoreFloat3(&vx3->normal, DirectX::XMVectorAdd(vNormal, vBuf));

		++linkCounter[_indicies[iIdx]];
		++linkCounter[_indicies[iIdx + 1]];
		++linkCounter[_indicies[iIdx + 2]];
	}
	for (UINT iVtx = 0; iVtx < _vtxCount; ++iVtx)
	{
		vNormal = XMLoadFloat3(&_vertices[iVtx].normal);
		vBuf = XMVectorSet(linkCounter[iVtx] , linkCounter[iVtx], linkCounter[iVtx], 1);
		XMStoreFloat3(&_vertices[iVtx].normal, XMVector3Normalize(XMVectorDivide(vNormal, vBuf)));
	}
	delete[] linkCounter;
}

void RenderSys::drawCubeScene()
{
	Vertex cube[24];
	//up
	cube[0].position = { 0.5, 0.5, 0.5 };
	cube[0].color = { 1, 0, 0, 1 };
	cube[0].normal = { 0, 1, 0 };
	cube[1].position = { -0.5, 0.5, 0.5 };
	cube[1].color = { 0, 1, 0, 1 };
	cube[1].normal = { 0, 1, 0 };
	cube[2].position = { -0.5, 0.5, -0.5 };
	cube[2].color = { 0, 0, 1, 1 };
	cube[2].normal = { 0, 1, 0 };
	cube[3].position = { 0.5, 0.5, -0.5 };
	cube[3].color = { 1, 0, 0, 1 };
	cube[3].normal = { 0, 1, 0 };
	//front
	cube[4].position = { 0.5, 0.5, 0.5 };
	cube[4].color = { 1, 0, 0, 1 };
	cube[4].normal = { 1, 0, 0 };
	cube[5].position = { 0.5, 0.5, -0.5 };
	cube[5].color = { 0, 1, 0, 1 };
	cube[5].normal = { 1, 0, 0 };
	cube[6].position = { 0.5, -0.5, -0.5 };
	cube[6].color = { 0, 0, 1, 1 };
	cube[6].normal = { 1, 0, 0 };
	cube[7].position = { 0.5, -0.5, 0.5 };
	cube[7].color = { 1, 0, 0, 1 };
	cube[7].normal = { 1, 0, 0 };
	//back
	cube[8].position = { -0.5, 0.5, 0.5 };
	cube[8].color = { 1, 0, 0, 1 };
	cube[8].normal = { -1, 0, 0 };
	cube[9].position = { -0.5, 0.5, -0.5 };
	cube[9].color = { 0, 1, 0, 1 };
	cube[9].normal = { -1, 0, 0 };
	cube[10].position = { -0.5, -0.5, -0.5 };
	cube[10].color = { 0, 0, 1, 1 };
	cube[10].normal = { -1, 0, 0 };
	cube[11].position = { -0.5, -0.5, 0.5 };
	cube[11].color = { 1, 0, 0, 1 };
	cube[11].normal = { -1, 0, 0 };
	//left
	cube[12].position = { -0.5, 0.5, -0.5 };
	cube[12].color = { 1, 0, 0, 1 };
	cube[12].normal = { 0, 0, -1 };
	cube[13].position = { 0.5, 0.5, -0.5 };
	cube[13].color = { 0, 1, 0, 1 };
	cube[13].normal = { 0, 0, -1 };
	cube[14].position = { -0.5, -0.5, -0.5 };
	cube[14].color = { 0, 0, 1, 1 };
	cube[14].normal = { 0, 0, -1};
	cube[15].position = { 0.5, -0.5, -0.5 };
	cube[15].color = { 1, 0, 0, 1 };
	cube[15].normal = { 0, 0, -1 };
	//right
	cube[16].position = { -0.5, 0.5, 0.5 };
	cube[16].color = { 1, 0, 0, 1 };
	cube[16].normal = { 0, 0, 1 };
	cube[17].position = { 0.5, 0.5, 0.5 };
	cube[17].color = { 0, 1, 0, 1 };
	cube[17].normal = { 0, 0, 1 };
	cube[18].position = { -0.5, -0.5, 0.5 };
	cube[18].color = { 0, 0, 1, 1 };
	cube[18].normal = { 0, 0, 1 };
	cube[19].position = { 0.5, -0.5, 0.5 };
	cube[19].color = { 1, 0, 0, 1 };
	cube[19].normal = { 0, 0, 1 };
	//down
	cube[20].position = { 0.5, -0.5, 0.5 };
	cube[20].color = { 1, 0, 0, 1 };
	cube[20].normal = { 0, -1, 0 };
	cube[21].position = { -0.5, -0.5, 0.5 };
	cube[21].color = { 0, 1, 0, 1 };
	cube[21].normal = { 0, -1, 0 };
	cube[22].position = { -0.5, -0.5, -0.5 };
	cube[22].color = { 0, 0, 1, 1 };
	cube[22].normal = { 0, -1, 0 };
	cube[23].position = { 0.5, -0.5, -0.5 };
	cube[23].color = { 1, 0, 0, 1 };
	cube[23].normal = { 0, -1, 0 };

	UINT indexes[] = { 0, 2, 1, 0, 3, 2, 20, 21, 22, 20, 22, 23, 
					4, 6, 5, 6, 4, 7, 9, 10, 8, 10, 11, 8, 
					12, 13, 15, 14, 12, 15, 17, 16, 19, 19, 16, 18
	};

	objects.push_back(createEntity(&cube[0], ARRAYSIZE(cube), &indexes[0], ARRAYSIZE(indexes)));
	/*Vertex cube[8];

	cube[0].position = { 0.5, 0.5, 0.5 };
	cube[0].color = { 1, 0, 0, 1 };
	cube[1].position = { -0.5, 0.5, 0.5 };
	cube[1].color = { 0, 1, 0, 1 };
	cube[2].position = { -0.5, 0.5, -0.5 };
	cube[2].color = { 0, 0, 1, 1 };
	cube[3].position = { 0.5, 0.5, -0.5 };
	cube[3].color = { 1, 0, 0, 1 };
	cube[4].position = { 0.5,  -0.5, 0.5 };
	cube[4].color = { 0,  1, 0, 1 };
	cube[5].position = { -0.5, -0.5, 0.5 };
	cube[5].color = { 0, 0, 1, 1 };
	cube[6].position = { -0.5, -0.5, -0.5 };
	cube[6].color = { 1, 0, 0, 1 };
	cube[7].position = { 0.5,  -0.5, -0.5 };
	cube[7].color = { 0,  1, 0, 1 };

	UINT indexes[] = {0, 2, 1, 0, 3, 2, //up
					0, 7 ,3, 7, 0, 4,   //right
					0, 5, 4, 0, 1 ,5,   // back
					1, 6, 5, 6, 1, 2,   // left
					2, 3, 6, 7, 6, 3,   //face
					7, 5, 6, 5, 7, 4 
	}; 

	calcWeightedNormals(&cube[0], 8, &indexes[0], ARRAYSIZE(indexes));
	objects.push_back(createEntity(&cube[0], 8, &indexes[0], ARRAYSIZE(indexes)));

	*/
	Vertex plane[4];
	for (UINT i = 0; i < 4; ++i)
	{
		plane[i].normal = vec3(0, 1, 0);
		plane[i].color = vec4(0.9, 0.9, 0.9, 1);
	}
	float plane_size = 5;
	plane[0].position = vec3(-plane_size, -0.5, -plane_size);
	plane[1].position = vec3(-plane_size, -0.5,  plane_size);
	plane[2].position = vec3( plane_size, -0.5,  plane_size);
	plane[3].position = vec3( plane_size, -0.5, -plane_size);
	UINT plane_indexes[] = { 0, 1, 3, 3, 1, 2 };
	objects.push_back(createEntity(&plane[0], ARRAYSIZE(plane), &plane_indexes[0], ARRAYSIZE(plane_indexes)));
}

void RenderSys::drawPlaneScene()
{
	Vertex plane[4];

	plane[0].position = { 0.5, 0., 0.5 };
	plane[0].color = { 1, 0, 0, 1 };
	plane[1].position = { -0.5, 0., 0.5 };
	plane[1].color = { 0, 1, 0, 1 };
	plane[2].position = { -0.5, 0., -0.5 };
	plane[2].color = { 0, 0, 1, 1 };
	plane[3].position = { 0.5, 0., -0.5 };
	plane[3].color = { 1, 0, 0, 1 };

	UINT indexes[] = { 0, 2, 1, 0, 3, 2 };

	PhongShader* pShader = new PhongShader();
	pShader->Initialize(pDevice, pDeviceContext, L"shaders/vertexShader.hlsl", L"shaders/pixelShader.hlsl");
	objects.push_back(createEntity(&plane[0], ARRAYSIZE(plane), &indexes[0], ARRAYSIZE(indexes)));
	
	calcWeightedNormals(&plane[0], 4, &indexes[0], 6);
	drawNormals(&plane[0], 4);
}

void RenderSys::drawNormals(const Vertex* _vertices, UINT _count)
{
	Vertex* normlas = new Vertex[_count * 2];
	XMVECTOR v1, v2;
	for (UINT iVtx = 0; iVtx < _count; ++iVtx)
	{
		normlas[iVtx * 2].position = _vertices[iVtx].position;
		v1 = XMLoadFloat3(&_vertices[iVtx].position); v2 = XMLoadFloat3(&_vertices[iVtx].normal);
		XMStoreFloat3(&normlas[iVtx * 2 + 1].position, XMVectorAdd(v1, v2));
	}
	PhongShader* pShader = new PhongShader();
	pShader->Initialize(pDevice, pDeviceContext, L"shaders/vertexShader.hlsl", L"shaders/pixelShader.hlsl");
	objects.push_back(createEntity(normlas, _count * 2, nullptr, 0));

}

void flipNormals(Vertex* _vertices, UINT _count)
{
	for (UINT iVtx = 0; iVtx < _count; ++iVtx)
	{
		_vertices[iVtx].normal.x *= -1;
		_vertices[iVtx].normal.y *= -1;
		_vertices[iVtx].normal.z *= -1;
	}
}


void RenderSys::Render()
{
	static DWORD startTime = GetTickCount();
	DWORD actualTime = GetTickCount();
	static float R = 2;
	float t = (actualTime - startTime) / 1500.0f;
	vec3 camPos = { R * sinf(t) , 2, R * cosf(t) };
	mCamera->setPos(camPos, { 0, 0, 0 }, { 0, 1, 0 });
	pFS->pCameraPos = &camPos;

	//ZTest
	pRtT->setRenderTarget(pDeviceContext);
	pRtT->getProjectionMx(pFS->mMVP->mProjection);
	pFS->pLight->mProjection = pFS->mMVP->mProjection;
	pFS->mMVP->mView = pFS->pLight->mView;
	for (std::vector<Entity*>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		(*it)->getModelMx(pFS->mMVP->mModel);
		(*it)->Render(pDeviceContext);
		pFS->indicesCount = (*it)->getIndicesCount();
		pZSh->Render(pDeviceContext, pFS);
	}

	//Forward

	mCamera->getVP(pFS->mMVP->mProjection, pFS->mMVP->mView);
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthBuffer);
	const static float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, ClearColor);
	pDeviceContext->ClearDepthStencilView(pDepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pDeviceContext->RSSetViewports(1, &mVP);
	pFS->pDepthBuffer = pRtT->getTargetTexture();
	pFS->pSRV = pRtT->getShaderResourceView();

	for (std::vector<Entity*>::iterator it = objects.begin(); it != objects.end(); ++it)
	{
		(*it)->getModelMx(pFS->mMVP->mModel);
		(*it)->Render(pDeviceContext);
		pFS->indicesCount = (*it)->getIndicesCount();
		pLightSh->Render(pDeviceContext, pFS);
	}
	pSwapChain->Present(1, 0);
}

Entity* RenderSys::createEntity(Vertex* _pVx, UINT _vxCount, UINT* _pIndex, UINT _indexCount)
{
	Entity* res = new Entity();
	res->Initialize(createVertexBuffer(_pVx, _vxCount), _vxCount, createIndexBuffer(_pIndex, _indexCount), _indexCount);
	return res;
}

ID3D11Buffer* RenderSys::createVertexBuffer(Vertex* _mem, UINT _ptCount)
{
	ID3D11Buffer* res;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = _ptCount * sizeof(Vertex);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA dataStorage;
	memset(&dataStorage, 0, sizeof(dataStorage));
	dataStorage.pSysMem = _mem;
	auto status = pDevice->CreateBuffer(&desc, &dataStorage, &res);
	if (status != S_OK) return nullptr;

	return res;
}

ID3D11Buffer* RenderSys::createIndexBuffer(UINT* _mem, UINT _indexCount)
{
	if (!_mem) return nullptr;
	ID3D11Buffer* res;
	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = _indexCount * sizeof(UINT);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA dataStorage;
	memset(&dataStorage, 0, sizeof(dataStorage));
	dataStorage.pSysMem = _mem;
	auto status = pDevice->CreateBuffer(&desc, &dataStorage, &res);
	if (status != S_OK) return nullptr;

	return res;
}

FrameState::FrameState()
{
	pCameraPos = nullptr;
	pLight = nullptr;
	mMVP = nullptr;
	pDepthBuffer = nullptr;
	pSRV = nullptr;
	pSS = nullptr;
	indicesCount = 0;
}

FrameState::~FrameState()
{
	delete mMVP;
	delete pLight;
	if (pSS)
	{
		pSS->Release();
		pSS = nullptr;
	}
}

HRESULT FrameState::Initialize(ID3D11Device* _pDevice, Camera* _pCamera, UINT _shadowTextureWidth, UINT _shadowTextureHeight)
{
	mMVP = new MVP();

	pLight = new Light();
	pLight->ambientColor = { 0.3, 0.3, 0.3, 1 };
	pLight->distance = 5;
	pLight->lightColor = { 1, 1, 1, 1 };
	pLight->lightPos = { 0, 1, 1 };
	
	vec3 lookAt = _pCamera->getLookAt(), up = _pCamera->getUp();
	pLight->mView = XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&pLight->lightPos), XMLoadFloat3(&lookAt), XMLoadFloat3(&up)));
	

	D3D11_SAMPLER_DESC pSamplerDesc; memset(&pSamplerDesc, 0, sizeof(pSamplerDesc));
	pSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	pSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pSamplerDesc.MaxAnisotropy = 1;
	pSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	pSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hRes = _pDevice->CreateSamplerState(&pSamplerDesc, &pSS);
	return hRes;
}
