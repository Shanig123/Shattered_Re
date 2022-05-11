#include "Shader_Value.h"

texture			g_BaseTexture;

sampler		BaseSampler = sampler_state
{
	texture = g_BaseTexture;

minfilter = linear;
magfilter = linear;
};

texture			g_NormalTexture;

sampler		NormalSampler = sampler_state
{
	texture = g_NormalTexture;

minfilter = linear;
magfilter = linear;
};

texture            g_EmissiveTexture;

sampler        EmissiveSampler = sampler_state
{
	texture = g_EmissiveTexture;

	minfilter = linear;
	magfilter = linear;
};

texture            g_SpecularTexture;

sampler        SpecluarSampler = sampler_state
{
	texture = g_SpecularTexture;

	minfilter = linear;
	magfilter = linear;
};


texture            g_OpacityTexture;

sampler        OpacitySampler = sampler_state
{
	texture = g_OpacityTexture;

	minfilter = linear;
	magfilter = linear;
};

texture            g_DepthTexture;

sampler        DepthSampler = sampler_state
{
	texture = g_DepthTexture;

minfilter = linear;
magfilter = linear;
};


struct VS_IN
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vBlendIndices : BLENDINDICES;
	float4			vBlendWeight : BLENDWEIGHT;
};
struct VS_OUT
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vLightProjPos : TEXCOORD1;
};

// ¡§¡° Ω¶¿Ã¥ı


VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4 vPos = float4(0.f, 0.f, 0.f, 1.f);
	float3 vNormal = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int iMax = g_numBonInfluences-1;

	
	for (int i = 0; i < iMax; ++i)
	{
		lastWeight += In.vBlendWeight[i];

		vPos += In.vBlendWeight[i] * float4(mul(In.vPosition, g_MatrixPalette[In.vBlendIndices[i]]).xyz, 1.f);
	}

	lastWeight = 1.f - lastWeight;
	vPos += lastWeight * float4(mul(In.vPosition, g_MatrixPalette[In.vBlendIndices[iMax]]).xyz, 1.f);
	In.vPosition.w = 1.f;
	vPos.w = 1.f;
	Out.vPosition = mul(vPos, g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matLightView);
	Out.vPosition = mul(Out.vPosition, g_matLightProj);

	Out.vTexUV = In.vTexUV;

	Out.vLightProjPos = Out.vPosition;

	return Out;


}

VS_OUT			VS_MAIN_NoSkinning(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;
	In.vPosition.w = 1.f;

	Out.vPosition = mul(In.vPosition, g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matLightView);
	Out.vPosition = mul(Out.vPosition, g_matLightProj);

	Out.vTexUV = In.vTexUV;

	Out.vLightProjPos = Out.vPosition;

	return Out;


}


struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT			PS_MAIN_N(VS_OUT In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor.rgba = 1.f;
	Out.vColor.rgb = In.vLightProjPos.z / In.vLightProjPos.w;

	return Out;
}

technique Default_Device
{

pass  ShadowMap  //0
{
	cullmode = none;
	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN_N();
}

pass  ShadowMapNoSkinning  //1
{
	cullmode = none;
VertexShader = compile vs_3_0 VS_MAIN_NoSkinning();
PixelShader = compile ps_3_0 PS_MAIN_N();
}

};
