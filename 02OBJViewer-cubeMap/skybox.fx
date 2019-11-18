//--------------------------------------------------------------------------------------
// basicEffect.fx
//--------------------------------------------------------------------------------------
matrix modelMatrix;
matrix viewMatrix;
matrix projectionMatrix;
matrix normalMatrix;

TextureCube cubeMapTexture;
SamplerState triLinearSam{

	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT{

	float3 position : POSITION; 
};

struct PS_INPUT{

	float4 position : SV_POSITION;
	float3 texCoordCube : TEXCOORD;
 
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input ){

	PS_INPUT output;

	output.position = mul( float4(input.position, 1.0), modelMatrix );
    output.position = mul( output.position, viewMatrix );    
    output.position = mul( output.position, projectionMatrix );
	output.texCoordCube = input.position;
	
    return output;  
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target{

   return cubeMapTexture.Sample(triLinearSam,input.texCoordCube); 
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 render{

    pass P0{
	
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
