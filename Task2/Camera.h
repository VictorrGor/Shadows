#pragma once
#include "Render.h"


class Camera
{
	vec3 eye;
	vec3 up;
	vec3 lookAt;
	mtx mxView;
	mtx mxProjection;
public:
	Camera(vec3 _eye, vec3 _lookAt, vec3 _up, UINT _width, UINT _height);
	void setPos(vec3 _eye, vec3 _lookAt, vec3 _up);
	void getVP(mtx& _mxProjection, mtx& _mxView);
};