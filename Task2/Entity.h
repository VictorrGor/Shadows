#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Render.h"
#include "Shader.h"
#include "Camera.h"
class PhongShader;

class Camera;

class Entity
{
	ID3D11Buffer* pVertexBuffer, *pIndexBuffer;
	UINT vertexCount, indexCount;
	mtx mxModel;
public:
	Entity();
	void Initialize(ID3D11Buffer* _pVertexBuffer, UINT _uVertexCount, ID3D11Buffer* _pIndexBuffer, UINT _uIndexCount);
	void Release();
	~Entity();

	void Render(ID3D11DeviceContext* _pDeviceContext);
	UINT getIndicesCount();
	void getModelMx(mtx& _pModel);
};