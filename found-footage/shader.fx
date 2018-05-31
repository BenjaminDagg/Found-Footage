//--------------------------------------------------------------------------------------
// File: lecture 8.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
Texture2D txDepth : register(t1);
SamplerState samLinear : register( s0 );

cbuffer ConstantBuffer : register( b0 )
{
matrix World;
matrix View;
matrix Projection;
int green;
float green_col;
};



//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
	
	if (green == 1) {
		return float4(0, green_col, 0, 1);
	}
	
	float4 color = txDiffuse.Sample(samLinear, input.Tex);
	//color.rgb *= 0.15;
	float depth = saturate(input.Pos.z / input.Pos.w);
	color.r *= depth*.7;
	color.g *= depth*.7;
	color.b *= depth*.7;
	color.a = 1;
	return color;
}
float4 PS2(PS_INPUT input) : SV_Target
{
	
		float4 color = txDiffuse.Sample(samLinear, input.Tex);
		//if (color.r < 0.05)
			//color.a = 0;
		//color.rgb *= 0.15;
		float depth = saturate(input.Pos.z / input.Pos.w);
	
		return color;
	}