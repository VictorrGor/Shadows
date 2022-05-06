
struct VS_INPUT
{
	float4 pos: POSITION;
};

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 depthPosition: TEXTURE0;
};

cbuffer WVP: register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}

VS_OUT main(VS_INPUT inp)
{
	VS_OUT o = (VS_OUT)0;
	inp.pos.w = 1;
	o.pos = inp.pos;
	o.pos = mul(o.pos, world);
	o.pos = mul(o.pos, view);
	o.pos = mul(o.pos, projection);
	o.depthPosition = o.pos;

	return o;
}