float4x4 m_world; // World matrix for object
float4x4 m_worldViewProj; // World * View * Projection matrix

struct VS_INPUT
{
	float4 Position	: POSITION0;
	float3 Normal	: NORMAL0;
	float4 Color	: COLOR0;
	
};

struct VS_OUTPUT
{
	float4 Position	: POSITION0;
	float4 Color	: COLOR0;
	float3 Normal	: NORMAL0;
};

VS_OUTPUT SimpleVertexShader(VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.Position = mul(input.Position, m_worldViewProj);

	output.Normal = normalize(mul(input.Normal, (float3x3)m_world));
	output.Color = input.Color;

	return output;
}

struct PS_OUTPUT
{
	float4 Color	: COLOR00;
};

PS_OUTPUT SimplePixelShader(VS_OUTPUT input)
{
	PS_OUTPUT output;
	output.Color = input.Color;
}

technique SimpleTechnique
{
	pass P0
	{
		VertexShader = compile vs_2_0 SimpleVertexShader();
		PixelShader = compile ps_2_0 SimplePixelShader();
	}
}