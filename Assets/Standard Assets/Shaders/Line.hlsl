// = INCLUDES ========
#include "Common.hlsl"
//====================

Texture2D depthTexture 		: register(t0);
SamplerState samplerPoint 	: register(s0);

cbuffer MiscBuffer : register(b0)
{
	matrix mWorld;
	matrix mView;
	matrix mProjection;
};

struct PixelInputType
{
    float4 position 	: SV_POSITION;
    float4 color 		: COLOR;
	float4 colliderPos 	: WHATEVER;
};

// Vertex Shader
PixelInputType mainVS(Vertex_PosColor input)
{
    PixelInputType output;
    	
    input.position.w = 1.0f;
	
    output.position = mul(input.position, mWorld);
	output.position = mul(output.position, mView);
	output.position = mul(output.position, mProjection);
	
	output.colliderPos = mul(input.position, mView);
	output.colliderPos = mul(output.colliderPos, mProjection);
	
	output.color = input.color;
	
	return output;
}

// Pixel Shader
float4 mainPS(PixelInputType input) : SV_TARGET
{
	float2 projectDepthMapTexCoord;
	projectDepthMapTexCoord.x = input.colliderPos.x / input.colliderPos.w / 2.0f + 0.5f;
	projectDepthMapTexCoord.y = -input.colliderPos.y / input.colliderPos.w / 2.0f + 0.5f;
	
	float colliderDepthValue 	= input.position.z;
	float depthMapValue 		= depthTexture.Sample(samplerPoint, projectDepthMapTexCoord).r;
	
	if (depthMapValue > colliderDepthValue) // If an object is in front of the line
		discard;
	
	return input.color;
}