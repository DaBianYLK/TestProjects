
//////////////////////////////////////////////////////////////////////////////

sampler2D sampler0 : register(s1);

uniform float4 g_Color : register(c0);

struct VSOutput
{
	float2 texCoord : TEXCOORD0; 
	float4 color	: COLOR0;
};

float4 BasePS(VSOutput input) : COLOR
{
	return tex2D(sampler0, input.texCoord);

	//return float4(1, 1, 1, 1);
}

//////////////////////////////////////////////////////////////////////////////
