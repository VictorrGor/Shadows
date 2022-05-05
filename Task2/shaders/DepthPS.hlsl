
struct PS_INPUT
{
	float4 pos: SV_POSITION;
	float4 depthPosition: TEXTURE0;
};

float4 main(PS_INPUT inp) : SV_TARGET
{
	float depth = inp.depthPosition.z / inp.depthPosition.w;
	return float4(depth, depth, depth, 1);
}