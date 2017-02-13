
//////////////////////////////////////////////////////////////////////////////

// shared关键字用于通过Effect Pool在不同的shader间共享常量

struct VSOutput
{
	float4 position	: POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color	: COLOR0;
};

VSOutput BaseVS(float4 inPosition : POSITION,
				float2 inTexCoord : TEXCOORD0)
{
	VSOutput output = (VSOutput)0;

	output.position = float4( inPosition.xy, 0, 1);
	output.texCoord = inTexCoord;
	output.color = float4(1, 1, 1, 1);

	return output;
}