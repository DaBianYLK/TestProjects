
//////////////////////////////////////////////////////////////////////////////

// shared�ؼ�������ͨ��Effect Pool�ڲ�ͬ��shader�乲����

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