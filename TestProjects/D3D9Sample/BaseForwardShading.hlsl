
//////////////////////////////////////////////////////////////////////////////

// shared关键字用于通过Effect Pool在不同的shader间共享常量

struct VSOutput
{
	float4 position	: POSITION;
};

VSOutput BaseVS(float4 inPosition	: POSITION)
{
	VSOutput output = (VSOutput)0;

	output.position = inPosition;

	return output;
}

//////////////////////////////////////////////////////////////////////////////

float4 BasePS() : COLOR
{
	return float4(1, 1, 1, 1);
}

//////////////////////////////////////////////////////////////////////////////

technique BaseTechnique
{
	pass BasePass
	{
		VertexShader = compile vs_3_0 BaseVS();
		PixelShader = compile ps_3_0 BasePS();
	}
}