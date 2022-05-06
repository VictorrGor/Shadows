#include "Camera.h"
#include <DirectXMath.h>


Camera::Camera(vec3 _eye, vec3 _lookAt, vec3 _up, UINT _width, UINT _height) : eye(_eye), lookAt(_lookAt), up(_up)
{
	fFar = 100;
	fNear = 0.1;
	mxProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, (FLOAT)_width / _height, fNear, fFar);

	DirectX::XMVECTOR vEye, vLookAt, vUp;
	vEye = XMLoadFloat3(&eye);
	vLookAt = XMLoadFloat3(&lookAt);
	vUp = XMLoadFloat3(&up);
	mxView = DirectX::XMMatrixLookAtLH(vEye, vLookAt, vUp);
};

void Camera::setPos(vec3 _eye, vec3 _lookAt, vec3 _up)
{
	eye = _eye;
	lookAt = _lookAt;
	up = _up;

	DirectX::XMVECTOR vEye, vLookAt, vUp;
	vEye = XMLoadFloat3(&eye);
	vLookAt = XMLoadFloat3(&lookAt);
	vUp = XMLoadFloat3(&up);
	mxView = DirectX::XMMatrixLookAtLH(vEye, vLookAt, vUp);
};

void Camera::getVP(mtx& _mxProjection, mtx& _mxView)
{
	_mxProjection = XMMatrixTranspose(mxProjection);
	_mxView = XMMatrixTranspose(mxView);
}
void Camera::getProjectionMxAndAttributes(mtx& _mxProjection, float& _near, float& _far)
{
	_mxProjection = mxProjection;
	_near = fNear;
	_far = fFar;
}
vec3 Camera::getUp()
{
	return up;
}
vec3 Camera::getLookAt()
{
	return lookAt;
}