#include "Entity.h"

Entity::Entity()
{
	pVertexBuffer = nullptr;
	pIndexBuffer = nullptr;
}

void Entity::Initialize(ID3D11Buffer* _pVertexBuffer, UINT _uVertexCount, ID3D11Buffer* _pIndexBuffer, UINT _uIndexCount)
{
	vertexCount = _uVertexCount;
	indexCount = _uIndexCount;
	mxModel = DirectX::XMMatrixIdentity();
	pVertexBuffer = _pVertexBuffer;
	pIndexBuffer = _pIndexBuffer;
}

void Entity::Release()
{
	if (pVertexBuffer)
	{
		pVertexBuffer->Release();
		pVertexBuffer = nullptr;
	}
	if (pIndexBuffer)
	{
		pIndexBuffer->Release();
		pIndexBuffer = nullptr;
	}
}

Entity::~Entity()
{
	Release();
}

void Entity::Render(ID3D11DeviceContext* _pDeviceContext)
{
	static const UINT stride = sizeof(Vertex);
	static const UINT offset = 0;
	_pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
}

UINT Entity::getIndicesCount()
{
	return indexCount;
}

void Entity::getModelMx(mtx& _pModel)
{
	_pModel = XMMatrixTranspose(mxModel);
}
