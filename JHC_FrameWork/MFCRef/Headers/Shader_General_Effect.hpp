#include "Shader_Value.h"

#pragma region Texture

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
//AddressU = wrap;
//AddressV = wrap;
//AddressW = wrap;
AddressU = clamp;
AddressV = clamp;
AddressW = clamp;
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

texture            g_DistortionTexture;

sampler        DistortionSampler = sampler_state
{
	texture = g_DistortionTexture;

minfilter = linear;
magfilter = linear;
AddressU = wrap;
AddressV = wrap;
AddressW = wrap;
};

#pragma endregion

float g_fFogIn;
float g_fFogOut;
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
	float2			vTexUV : TEXCOORD0;
	float3			vNormal : NORMAL;
	float3			vTangent : TANGENT;
	float3			vBinormal : BINORMAL;
};
struct VS_IN_COLOR
{
	float4			vPosition : POSITION;
	//int				iColor : COLOR;
	float4			vColor : COLOR;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	//float3			vNormal : NORMAL;
	//float3			vTangent : TANGENT;
	//float3			vBinormal : BINORMAL;
};
struct VS_OUT_COLOR
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	//int				iColor : COLOR;
	float4			vColor : COLOR;
	//float3			vNormal : NORMAL;
	//float3			vTangent : TANGENT;
	//float3			vBinormal : BINORMAL;
};
struct VS_OUT_FOG
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	float			fFog : FOG;
	//float3			vNormal : NORMAL;
	//float3			vTangent : TANGENT;
	//float3			vBinormal : BINORMAL;
};
// ¡§¡° Ω¶¿Ã¥ı
VS_OUT			VS_MAIN_Effect(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	In.vPosition.w = 1.f;
	Out.vPosition = mul(In.vPosition, g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matView);
	Out.vPosition = mul(Out.vPosition, g_matProj);

	//Out.vNormal = normalize(mul(In.vNormal, (float3x3)g_matWorld));
	//float3 vTemp = Out.vNormal;
	//vTemp.y += 1.f;
	//vTemp = normalize(vTemp);
	//vTemp = cross(vTemp, Out.vNormal);
	//Out.vTangent = normalize(mul(vTemp, (float3x3)g_matWorld));
	//Out.vBinormal = cross(Out.vNormal, Out.vTangent);
	//Out.vTangent = cross(Out.vBinormal, Out.vNormal);

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	return Out;
}

VS_OUT_COLOR	VS_MAIN_Effect_COLOR(VS_IN_COLOR In)
{
	VS_OUT_COLOR		Out = (VS_OUT_COLOR)0;

	In.vPosition.w = 1.f;
	Out.vPosition = mul(In.vPosition, g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matView);
	Out.vPosition = mul(Out.vPosition, g_matProj);

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vColor = In.vColor;
	return Out;
}

VS_OUT_FOG		VS_MAIN_Fog(VS_IN In)
{
	VS_OUT_FOG		Out = (VS_OUT_FOG)0;

	In.vPosition.w = 1.f;
	Out.vPosition = mul(In.vPosition, g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matView);
	Out.vPosition = mul(Out.vPosition, g_matProj);

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	////fogdensity : 0~1
	//Out.fFog = 1.0/2.71828 power(g_vCamPos.z*g_FogDensity);
	Out.fFog = (g_fFogOut - g_vCamPos.z) / (g_fFogOut - g_fFogIn);
	return Out;
}

struct PS_IN
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	//float3			vNormal : NORMAL;
	//float3			vTangent : TANGENT;
	//float3			vBinormal : BINORMAL;
};

struct PS_IN_Effect
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	//float3			vNormal : NORMAL;
	//float3			vTangent : TANGENT;
	//float3			vBinormal : BINORMAL;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
	vector      vEmissive : COLOR1;
	vector		vNormal : COLOR2;
	vector		vDepth : COLOR3;
	//float		vShadow : DEPTH;
};

float4 gaussFilter[7] =
{
	0.0, -3.0,0.0, 1.0 / 64.0,
	0.0, -2.0,0.0, 6.0 / 64.0,
	0.0, -1.0,0.0, 15.0 / 64.0,
	0.0,  0.0,0.0, 20.0 / 64.0,
	0.0,  1.0,0.0, 15.0 / 64.0,
	0.0,  2.0,0.0, 6.0 / 64.0,
	0.0,  3.0,0.0, 1.0 / 64.0
};
float texscaler = 1.0 / 128.0;
float texOffset = 0.0;
static const float samplesWe[7] =
{//stddev=2.0
	0.07130343198685299,
	0.13151412084312236,
	0.1898792328888381,

	0.214606428562373,

	0.1898792328888381,
	0.13151412084312236,
	0.07130343198685299
};

vector Gauss(float2 vUV)
{
	float4 color0 = 0.0;
	float4 color1 = 0.0;
	int i;

	for (i = 0; i<7; i++)
	{

		color0 += tex2D(EmissiveSampler, float2(vUV.x + gaussFilter[i].x * texscaler,
			vUV.y + gaussFilter[i].y * texscaler))
			* samplesWe[i];
	}

	for (i = 0; i<7; i++)
	{

		color1 += tex2D(EmissiveSampler, float2(vUV.x + gaussFilter[i].y * texscaler,
			vUV.y + gaussFilter[i].x * texscaler))
			* samplesWe[i];
	}


	return  (color0*0.9 + color1*0.5);
}


PS_OUT			PS_MAIN_Effect(PS_IN_Effect In)
{
	//PS_OUT		Out = (PS_OUT)0;
	//Out.vColor = 0.f;
	//vector vEmissive;

	//vEmissive = 0.f;
	//vEmissive = tex2D(EmissiveSampler, In.vTexUV);


	////Out Emissive 
	//Out.vColor = vEmissive;

	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = 0.f;
	Out.vDepth = 0.f;
	Out.vEmissive = 0.f;
	Out.vNormal = 0.f;
	vector vEmissive;

	vEmissive = 0.f;
	//vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2);
	vEmissive = tex2D(EmissiveSampler, In.vTexUV);

	if (vEmissive.r >0.f)
	{
		//vEmissive.rgb -= 1.f;
		//vEmissive.rgb += g_vColor.rgb;
		vEmissive.rgb *= g_vColor.rgb;
		vEmissive.a += g_vColor.a;
	}
	else
	{
		vEmissive.rgb = 0.f;
		vEmissive.a = 0.f;
	}
	//Out Emissive 
	Out.vEmissive = vector(pow(abs(vEmissive), 2.2).rgb, vEmissive.a);

	return Out;

}


PS_OUT			PS_MAIN_BloomEffect(PS_IN_Effect In)
{
	//PS_OUT		Out = (PS_OUT)0;
	//Out.vColor = 0.f;
	//vector vEmissive;

	//vEmissive = 0.f;
	//vEmissive = tex2D(EmissiveSampler, In.vTexUV);


	////Out Emissive 
	//Out.vColor = vEmissive;

	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = 0.f;
	Out.vDepth = 0.f;
	Out.vEmissive = 0.f;
	Out.vNormal = 0.f;
	vector vEmissive;

	vEmissive = 0.f;
	//vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2);
	vEmissive = tex2D(EmissiveSampler, In.vTexUV);
	if (vEmissive.r >0.f)
	{
		vEmissive.rgb -= 1.f;
		vEmissive += (Gauss(In.vTexUV))*g_vColor.rgba;
	

		vEmissive.a += g_vColor.a;
	}
	else
	{
		vEmissive.rgb = 0.f;
		vEmissive.a = 0.f;
	}
	//Out Emissive 
	Out.vEmissive = vEmissive;

	return Out;

}


PS_OUT			PS_MAIN_Fog(VS_OUT_FOG In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vEmissive = 0.f;

	//Out Fog 
	Out.vEmissive = In.fFog*pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2)+(1.f - In.fFog)*g_vColor;

	return Out;
}
PS_OUT			PS_MAIN_DecalEffect(PS_IN_Effect In)
{
	//PS_OUT		Out = (PS_OUT)0;
	//Out.vColor = 0.f;
	//vector vEmissive;

	//vEmissive = 0.f;
	//vEmissive = tex2D(EmissiveSampler, In.vTexUV);


	////Out Emissive 
	//Out.vColor = vEmissive;

	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = 0.f;
	Out.vDepth = 0.f;
	Out.vEmissive = 0.f;
	Out.vNormal = 0.f;
	vector vEmissive;

	vEmissive = 0.f;
	vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2);

	if (vEmissive.r >0.f)
	{
		//vEmissive -= (Gauss(In.vTexUV));
		//vEmissive.rgb += g_vColor.rgb;

		vEmissive.a += g_vColor.a;
	}
	else
	{
		vEmissive.rgb = 0.f;
		vEmissive.a = 0.f;
	}
	//Out Emissive 
	Out.vEmissive = vEmissive;

	return Out;

}

PS_OUT	PS_MAIN_Effect_Color(VS_OUT_COLOR In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vEmissive;

	vEmissive = 0.f;
	//vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2);

	//// grab coordinates
	float2 vUV = 0.f;
	float noise = 0.f; 
	noise = length(tex2D(DistortionSampler, In.vTexUV+ g_ftime).rgb)/*+0.0001f*/;

	//float progress = frac(g_ftime);
	//vUV = In.vTexUV - tex2D(DistortionSampler, In.vTexUV).rg * progress;

	//vUV.x += cos(noise+g_ftime*1.f) * 1.f;
	//vUV.y += sin(noise+g_ftime*1.f) * 1.f;
	//vEmissive = tex2D(EmissiveSampler, vUV);
	
	vEmissive = tex2D(EmissiveSampler, float2(In.vTexUV.x+ length(tex2D(DistortionSampler, In.vTexUV + g_ftime).rgb), In.vTexUV.y));
	
	if (vEmissive.r >0.f)
	{
		//vEmissive.rgb -= 1.f;
		//vEmissive.rgb += g_vColor.rgb;
		vEmissive.rgb *= In.vColor.rgb;
		//vEmissive.a += In.vColor.a;
		//vEmissive.a = 1.f- In.vTexUV.x;
		//vEmissive.a = In.vColor.a;
		//vEmissive.a = 0.f;
		vEmissive.a = In.vColor.a;
		//if(In.vColor.a <= 0.f)
		//	vEmissive.a = In.vColor.a;
		if(noise)
			vEmissive.a -= noise;
	}
	else
	{
		vEmissive.rgb = 0.f;
		vEmissive.a = 0.f;
	}
	//Out Emissive 
	Out.vEmissive = vector(pow(abs(vEmissive), 2.2).rgb, vEmissive.a);

	return Out;
}




technique Default_Device
{
	pass    Effect//0
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;
		srcblend = srcalpha;
		//srcblend = srccolor;
		//destblend = invsrccolor;
		destblend = invsrcalpha;
		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_BloomEffect();
	}
	pass Fog		//1
	{
		SRGBWriteEnable = true;
		VertexShader = compile vs_3_0 VS_MAIN_Fog();
		PixelShader = compile ps_3_0 PS_MAIN_Fog();
	}
		pass NoGauss	//2
	{
			zwriteenable = false;
		cullmode = None;
		alphablendenable = true;
		srcblend = srcalpha;
		//srcblend = srccolor;
		//destblend = invsrccolor;
		destblend = invsrcalpha;
		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Effect();
	}
	pass NoGauss		//3
	{
			zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_DecalEffect();
	}

	pass Effect_Color			//4
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect_COLOR();
		PixelShader = compile ps_3_0 PS_MAIN_Effect_Color();
	}
};
