

uniform float4 g_f4Rect : register(c0);


struct VSOutput
{
	float4 position	: POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color	: COLOR0;
};

VSOutput VSTest(float4 pos : POSITION, float2 inTexCoord : TEXCOORD0)
{
	VSOutput output = (VSOutput)0;
	output.position = pos;
	output.texCoord = inTexCoord;
	output.color = g_f4Rect;

	return output;
}