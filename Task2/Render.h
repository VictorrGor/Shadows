#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

typedef DirectX::XMFLOAT3 vec3;
typedef DirectX::XMFLOAT4 vec4;
typedef DirectX::XMMATRIX mtx;

#include "Camera.h"
#include "Entity.h"
#include <queue>


class Entity;
class PhongShader;
class Camera;
struct Light;

struct MVP
{
	mtx mModel;
	mtx mView;
	mtx mProjection;
};

struct FrameState
{
	vec3* pCameraPos;
	Light* pLight;
	MVP* mMVP;
	UINT indicesCount;
};

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec4 color;
};

using namespace DirectX;

/// 
/// @todo Window resize function. Recalculate projectionMx
/// 
class RenderSys
{
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
	ID3D11DepthStencilView* pDepthBuffer;

	Camera* mCamera;
	std::vector<Entity*> objects;
	PhongShader* pSh;
	FrameState* pFS;
public:
	RenderSys();
	~RenderSys();
	HRESULT Initialize(const HWND& hWnd);
	void Release();
	void drawCubeScene();
	void drawPlaneScene();
	void Render();
	void drawNormals(const Vertex* _vertices, UINT _count);

	Entity* createEntity(Vertex* _pVx, UINT _vxCount, UINT* _pIndex, UINT _indexCount);
	ID3D11Buffer* createVertexBuffer(Vertex* _mem, UINT _ptCount);
	ID3D11Buffer* createIndexBuffer(UINT* _mem, UINT _indexCount);

	void renderZScene();
};