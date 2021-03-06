// = INCLUDES ========
#include "Common.hlsl"
//====================

//= TEXTURES ===============================
Texture2D texAlbedo 		: register (t0);
Texture2D texRoughness 		: register (t1);
Texture2D texMetallic 		: register (t2);
Texture2D texNormal 		: register (t3);
Texture2D texHeight 		: register (t4);
Texture2D texOcclusion 		: register (t5);
Texture2D texEmission 		: register (t6);
Texture2D texMask 			: register (t7);
//==========================================

//= SAMPLERS =============================
SamplerState samplerAniso : register (s0);
//========================================

//= BUFFERS ==================================
cbuffer PerMaterialBuffer : register(b0)
{		
    float4 materialAlbedoColor;
	
	float2 materialTiling;
	float2 materialOffset;
	
    float materialRoughness;
    float materialMetallic;
    float materialNormalStrength;
	float materialHeight;

	float3 cameraPosWS;
	float padding;
	
	float2 planes;
	float2 resolution;
	
	float materialShadingMode;
	float3 padding2;
};

cbuffer PerObjectBuffer : register(b1)
{
	matrix mWorld;
    matrix mView;
    matrix mProjection;
}
//===========================================

//= STRUCTS =================================
struct PixelInputType
{
    float4 positionCS 	: SV_POSITION;
    float2 uv 			: TEXCOORD;
    float3 normal 		: NORMAL;
    float3 tangent 		: TANGENT;
	float3 bitangent 	: BITANGENT;
	float4 positionVS 	: POSITIONT0;
    float4 positionWS 	: POSITIONT1;
};

struct PixelOutputType
{
	float4 albedo	: SV_Target0;
	float4 normal	: SV_Target1;
	float4 specular	: SV_Target2;
	float2 depth	: SV_Target3;
};
//===========================================

PixelInputType mainVS(Vertex_PosUvTbn input)
{
    PixelInputType output;
    
    input.position.w 	= 1.0f;	
	output.positionWS 	= mul(input.position, mWorld);
    output.positionVS   = mul(output.positionWS, mView);
    output.positionCS   = mul(output.positionVS, mProjection);
	output.normal 		= normalize(mul(input.normal, 		(float3x3)mWorld)).xyz;	
	output.tangent 		= normalize(mul(input.tangent, 		(float3x3)mWorld)).xyz;
	output.bitangent 	= normalize(mul(input.bitangent, 	(float3x3)mWorld)).xyz;
    output.uv 			= input.uv;
	
	return output;
}

PixelOutputType mainPS(PixelInputType input)
{
	PixelOutputType g_buffer;

    float depth_linear  = input.positionVS.z / planes.y;
    float depth_cs      = input.positionCS.z / input.positionVS.w;
	float2 texCoords 	= float2(input.uv.x * materialTiling.x + materialOffset.x, input.uv.y * materialTiling.y + materialOffset.y);
	float4 albedo		= materialAlbedoColor;
	float roughness 	= materialRoughness;
	float metallic 		= materialMetallic;
	float emission		= 0.0f;
	float occlusion		= 1.0f;
	float3 normal		= input.normal.xyz;
	float type			= 0.0f; // pbr mesh
	
	//= TYPE CODES ============================
	// 0.0 = Default mesh 	-> PBR
	// 0.1 = CubeMap 		-> texture mapping
	//=========================================

	//= HEIGHT ==================================================================================
#if HEIGHT_MAP
		// Parallax Mapping
		float height_scale 	= materialHeight * 0.01f;
		float3 viewDir 		= normalize(cameraPosWS - input.positionWS.xyz);
		float height 		= texHeight.Sample(samplerAniso, texCoords).r;
		float2 offset 		= viewDir.xy * (height * height_scale);
		if(texCoords.x <= 1.0 && texCoords.y <= 1.0 && texCoords.x >= 0.0 && texCoords.y >= 0.0)
		{
			texCoords += offset;
		}
#endif
	
	//= MASK ====================================================================================
#if MASK_MAP
		float3 maskSample = texMask.Sample(samplerAniso, texCoords).rgb;
		float threshold = 0.6f;
		if (maskSample.r <= threshold && maskSample.g <= threshold && maskSample.b <= threshold)
			discard;
#endif
	
	//= ALBEDO ==================================================================================
#if ALBEDO_MAP
		albedo *= texAlbedo.Sample(samplerAniso, texCoords);
#endif
	
	//= ROUGHNESS ===============================================================================
#if ROUGHNESS_MAP
		roughness *= texRoughness.Sample(samplerAniso, texCoords).r;
#endif
	
	//= METALLIC ================================================================================
#if METALLIC_MAP
		metallic *= texMetallic.Sample(samplerAniso, texCoords).r;
#endif
	
	//= NORMAL ==================================================================================
#if NORMAL_MAP
		float3 normalSample = normalize(UnpackNormal(texNormal.Sample(samplerAniso, texCoords).rgb));
		normal = TangentToWorld(normalSample, input.normal.xyz, input.tangent.xyz, input.bitangent.xyz, materialNormalStrength);
#endif
	//============================================================================================
	
	//= OCCLUSION ================================================================================
#if OCCLUSION_MAP
		occlusion = texOcclusion.Sample(samplerAniso, texCoords).r;
#endif
	
	//= EMISSION ================================================================================
#if EMISSION_MAP
		emission = texEmission.Sample(samplerAniso, texCoords).r;
#endif
		
	//= CUBEMAP ==================================================================================
#if CUBE_MAP
		type = 1.0f;
#endif
	//============================================================================================

	// Write to G-Buffer
	g_buffer.albedo		= albedo;
	g_buffer.normal 	= float4(PackNormal(normal), occlusion);
	g_buffer.specular	= float4(roughness, metallic, emission, type);
    g_buffer.depth      = float2(depth_linear, depth_cs);

    return g_buffer;
}