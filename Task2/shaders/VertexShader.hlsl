
struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};


struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

cbuffer cb_object: register(b0)
{
	float4x4 model;
	float4x4 view;
	float4x4 projection;
};

PS_INPUT main(VS_INPUT inp)
{
	PS_INPUT o = (PS_INPUT)0;
	o.pos = float4(inp.pos, 1);
	o.pos = mul(o.pos, model);
	o.worldPos = o.pos;
	o.pos = mul(o.pos, view);
	o.pos = mul(o.pos, projection);

	o.color = inp.color;
	o.normal = mul(inp.normal, model);
	return o;
}