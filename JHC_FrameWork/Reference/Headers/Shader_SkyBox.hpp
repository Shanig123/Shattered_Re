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

//
//vector		g_vLightDiffuse;
//vector		g_vLightAmbient;
//vector		g_vLightSpecular;
//
//vector		g_vMtrlDiffuse;
//vector		g_vMtrlAmbient;
//vector		g_vMtrlSpecular;
//
//float		g_fPower;
//



struct VS_IN
{
	float4			vPosition : POSITION;
	float3			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : POSITION;
	float3			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
};

// ¡§¡° Ω¶¿Ã¥ı
VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	//float4x4 matWV, matWVP;

	//matWV = mul(g_matWorld, g_matView);
	//matWVP = mul(matWV, g_matProj);


	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);


	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;


	return Out;


}

struct PS_IN
{
	float4			vPosition : TEXCOORD3;
	float3			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;

};
struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT			PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	//Out Alebedo
	//Out.vColor = pow(abs(tex3D(BaseSampler, In.vTexUV)), 2.2);
	Out.vColor= pow(abs(texCUBE(BaseSampler, In.vTexUV)), 2.2);
	Out.vColor.a = 1.f;

	return Out;
}

technique Default_Device
{

	pass  SkyBox  //0
	{ 
	zwriteenable = false;
	cullmode = none;
		//destblend = invsrccolor;
		/*	srcblend = one;
		destblend = one;*/
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

//pass    //1
//{
//
//
//	alphablendenable = true;
//
//srcblend = srccolor;
//destblend = invsrcalpha;
//
//VertexShader = compile vs_3_0 VS_MAINOren();
//PixelShader = compile ps_3_0 PS_MAIN_N();
//}
//
//pass    //2
//{
//
//
//	alphablendenable = true;
//
//srcblend = srcalpha;
//destblend = invsrccolor;
//
//VertexShader = compile vs_3_0 VS_MAINOren();
//PixelShader = compile ps_3_0 PS_MAIN_N();
//}

};
