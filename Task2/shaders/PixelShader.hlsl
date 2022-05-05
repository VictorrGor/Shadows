
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

cbuffer light: register(b0)
{
	float4 light_color;
	float4 color_ambient;
	float3 lightPos;
	float range;
}

cbuffer camera: register(b1)
{
	float4 cameraPos;
}

Texture2D depthMap: register(t0);
SamplerState ss: register(s0);

static const float specFactor = 32;

float4 main(PS_INPUT inp) : SV_TARGET
{
	inp.normal = normalize(inp.normal);
	float3 ambient = inp.color.xyz * color_ambient.xyz;
	float4 lightVec = float4(lightPos,1) - inp.worldPos;
	float distance = length(lightVec);
	lightVec = normalize(lightVec);
	float3 toEye = normalize(cameraPos.xyz - inp.worldPos.xyz);

	inp.normal = normalize(inp.normal);
	float3 H = normalize(toEye + lightVec);
	float3 diffuse  = (float3)0;
	float3 specular = (float3)0;
	if (range > distance)
	{
		float factor = lerp(0, 1, (range - distance) / range);
		float spec = pow(max(dot(inp.normal, H), 0), specFactor);
		diffuse = light_color.xyz * inp.color.xyz * max(0, dot(lightVec, inp.normal)) * factor;
		specular = light_color.xyz * spec *  factor;
	}
	return float4(diffuse + ambient + specular, 1);
}