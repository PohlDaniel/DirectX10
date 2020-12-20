cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VS_INPUT{

    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PS_INPUT{

    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(float4(input.position, 1.0), modelMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	output.tex = input.tex;
    
    return output;
}