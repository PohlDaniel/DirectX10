////////////////////////////////////////////////////////////////////////////////
// Filename: color.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VS_INPUT{

    float3 position : POSITION;
    float2 tex : TEXCOORD;
	float3 normal: NORMAL;
};

struct PS_INPUT{

    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
	float3 normal: NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
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