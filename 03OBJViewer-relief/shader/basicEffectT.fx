

struct Light{

	float3 pos;
	float4 ambient;
	float4 diffuse;
	float4 specular;

};

struct Material{

	float3 ambient;
	float3 diffuse;
	float3 specular;
	float shininess;
};

Light light;
Material material;

matrix modelMatrix;
matrix viewMatrix;
matrix projectionMatrix;
matrix normalMatrix;

float3 lightPos;




Texture2D colorMapTexture;
Texture2D normalMapTexture;
Texture2D displacementMapTexture;
TextureCube cubeMapTexture;

SamplerState colorMap{

    Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState normalMap{

    Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState displacementMap{

    Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState triLinearSam{

	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT{

	float3 position : POSITION;
    float2 texCoord : TEXCOORD;  
	float3 normal: NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};



struct PS_INPUT{

	float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD; 
	float3 texCoordCube : TEXCOORD1; 
	float3 halfVector : TEXCOORD2;
	float3 lightDir : TEXCOORD3;
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;	
	
	float3 normal : TEXCOORD4;
	float3 tangent : TEXCOORD5;
	float3 bitangent : TEXCOORD6;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input ){


	float3 viewDir;
	float3 lightDir;
	float3 halfVector;

	float3 n = mul( float4(input.normal, 1.0), normalMatrix ).xyz;
	float3 t = mul( float4(input.tangent, 1.0), normalMatrix ).xyz;
	float3 b = mul( float4(input.bitangent, 1.0), normalMatrix ).xyz;
	
	float3x3 tbnMatrix = float3x3(t.x, b.x, n.x,
	                              t.y, b.y, n.y,
	                              t.z, b.z, n.z);
	
	PS_INPUT output;

	output.normal = n;
	output.tangent = t;
	output.bitangent = b;
	
	output.texCoordCube = input.position;
	
	output.position = mul( float4(input.position, 1.0), modelMatrix );
    output.position = mul( output.position, viewMatrix );    
	
		//viewDirWorld = camPos - modelMatrix * position 
		//viewDirCam = viewMatrix * camPos - viewMatrix * modelMatrix * position
		//viewDirCam = - viewMatrix * modelMatrix * position
		
		viewDir = - output.position.xyz;
		lightDir = mul( float4(light.pos, 1.0), viewMatrix ).xyz - output.position.xyz;	
		halfVector = normalize(normalize(lightDir) + normalize(viewDir));

	output.position = mul( output.position, projectionMatrix );
	output.texCoord = input.texCoord;
   
	output.halfVector = mul(halfVector, tbnMatrix);
	output.lightDir = mul(lightDir, tbnMatrix);
	output.diffuse = float4(material.diffuse, 1.0) * light.diffuse;
	output.specular = float4(material.specular, 1.0) * light.specular;
	
    return output;  
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target{

	float3 n = normalize(normalMapTexture.Sample( normalMap, input.texCoord ).rgb * 2.0f - 1.0f);
    float3 h = normalize(input.halfVector);
    float3 l = normalize(input.lightDir);

	float nDotL = saturate(dot(n, l));
    float nDotH = saturate(dot(n, h));
    float power = (nDotL == 0.0f) ? 0.0f : pow(nDotH, material.shininess);
    
    float4 color = ( float4(material.ambient , 1.0) * (light.ambient)) +
                   (input.diffuse * nDotL) + (input.specular * power);
	
	//return float4(light.dir, 1.0);
	
	//return material.diffuse;
   // return colorMapTexture.Sample( colorMap, input.texCoord ); 
	//return normalMapTexture.Sample( normalMap, input.texCoord ); 
	//return displacementMapTexture.Sample( displacementMap, input.texCoord ); 
	//return float4(0.5, 0.8 ,0.0 ,1);

	//return cubeMapTexture.Sample(triLinearSam,input.texCoordCube);
	
	return float4(0.5, 0.8 ,0.0 ,1);
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
