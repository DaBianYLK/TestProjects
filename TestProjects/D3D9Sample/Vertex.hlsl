
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