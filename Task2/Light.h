#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Render.h"



#pragma pack(push, 1)
struct Light
{
	mtx mView;
	mtx mProjection;
	vec4 lightColor;
	vec4 ambientColor;
	vec3 lightPos;
	float distance;
};
#pragma pack(pop)