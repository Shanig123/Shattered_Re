#include "Shader_Value.h"

#pragma region Texture

texture			g_BaseTexture;

sampler		BaseSampler = sampler_state
{
	texture = g_BaseTexture;

minfilter = linear;
magfilter = linear;
AddressU = clamp;
AddressV = clamp;
AddressW = clamp;
//AddressU = wrap;
//AddressV = wrap;
//AddressW = wrap;
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

texture            g_DistortionTexture;

sampler        DistortionSampler = sampler_state
{
	texture = g_DistortionTexture;

minfilter = linear;
//magfilter = linear;
AddressU = wrap;
AddressV = wrap;
AddressW = wrap;
//AddressU = clamp;
//AddressV = clamp;
//AddressW = clamp;
};

texture            g_FilterTexture;

sampler        FilterSampler = sampler_state
{
	texture = g_FilterTexture;

minfilter = linear;
//magfilter = linear;
AddressU = wrap;
AddressV = wrap;
AddressW = wrap;
//AddressU = clamp;
//AddressV = clamp;
//AddressW = clamp;
};


#pragma endregion

float g_fTiling = 1.f;
float g_fFlowStrength = 1.f;
float g_fFlowOffset = 0.f;

bool g_bFlowB = false;

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
	float4			vColor : COLOR;
	float2			vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
};
struct VS_OUT_COLOR
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	float4			vColor : COLOR;
};

// Á¤Á¡ ½¦ÀÌ´õ
VS_OUT			VS_MAIN_Effect(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	In.vPosition.w = 1.f;
	Out.vPosition = mul(In.vPosition, g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matView);
	Out.vPosition = mul(Out.vPosition, g_matProj);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	vector		vDistortion1 : COLOR0;
	vector		vDistortion2 : COLOR1;
	//vector		vAlbedo : COLOR2;
	//vector      vFog : COLOR3;
	//vector      vUV : COLOR3;
	//vector		vNormal : COLOR2;
	//vector		vDepth : COLOR3;
	//float		vShadow : DEPTH;
};
float3 FlowUVW(
	float2 uv, float2 flowVector, float2 jump,float time)
{
	//float phaseOffset = g_bFlowB ? 0.5 : 0;
	//float progress = frac(time + phaseOffset)/**pow(0.5f, time)*/;
	//float3 r_uvw;
	//r_uvw.xy = uv - flowVector * (progress + g_fFlowOffset);
	//r_uvw.xy *= g_fTiling;
	//r_uvw.xy += phaseOffset;
	//r_uvw.xy += (time - progress) * jump;
	//r_uvw.z = 1 - abs(1 - 2 * progress);

	float phaseOffset = g_bFlowB ? 0.5 : 0;
	float progress = sin(time + phaseOffset)/**pow(0.5f, time)*/;
	float3 r_uvw;
	r_uvw.xy = uv - flowVector * (progress + g_fFlowOffset);
	r_uvw.xy *= g_fTiling;
	r_uvw.xy += phaseOffset;
	r_uvw.xy += (time - progress) * jump;
	r_uvw.z = 1 - abs(1 - 2 * progress);


	return r_uvw;
}

PS_OUT			PS_MAIN_Distot_RGB(PS_IN_Effect In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vDistortion1 = 0.f;
	//Out.vFog = 0.f;
	vector		vDistort = tex2D(DistortionSampler, In.vTexUV);

	float3 vUVW = 0.f;
	vUVW.xy = In.vTexUV;

	float fNoise = 0.f;
	fNoise = (vDistort.a>0.f) ? vDistort.rgb : 0.f;
	if (fNoise > 0.f)
	{
		float2 vJump = 0.f;
		float2 vFlow = length(vDistort.rgb).xx;
		vFlow = vFlow * 2.f - 1.f;
		//vUV.x += cos(fNoise*g_ftime*5.f)*5.f;
		//vUV.y += sin(fNoise*g_ftime*5.f)*5.f;
		vUVW = FlowUVW(In.vTexUV, vFlow*g_fFlowStrength, vJump, g_ftime);
	}
	//Out Distortion
	if (fNoise > 0.f)
	{
		//Out.vUV.a = 1.f;
		//Out.vUVW.x = vUV.x;
		//Out.vUVW.y = vUV.y;
		if (g_bFlowB)
		{
			Out.vDistortion2.xyz = vUVW;
			Out.vDistortion2.a = g_vColor.a*vUVW;
		}
		else
		{
			Out.vDistortion1.xyz = vUVW;
			Out.vDistortion1.a = g_vColor.a*vUVW;
		}
	}

	return Out;
}

PS_OUT			PS_MAIN_Distot_R(PS_IN_Effect In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vDistortion1 = 0.f;
	//Out.vFog = 0.f;
	vector		vDistort = tex2D(DistortionSampler, In.vTexUV);

	float3 vUVW = 0.f;
	vUVW.xy = In.vTexUV;

	float fNoise = 0.f;
	fNoise = (vDistort.r > 0.f) ? vDistort.r : 0.f;
	if (fNoise > 0.f)
	{
		float2 vJump = 0.f;

		float2 vFlow = length(fNoise).xx;
		vFlow = vFlow * 2.f - 1.f;
		//vUV.x += cos(fNoise*g_ftime*5.f)*5.f;
		//vUV.y += sin(fNoise*g_ftime*5.f)*5.f;
		vUVW = FlowUVW(In.vTexUV, vFlow*g_fFlowStrength, vJump, g_ftime);
	}
	//Out Distortion
	if (fNoise > 0.f)
	{
		//Out.vUV.a = 1.f;
		//Out.vUVW.x = vUV.x;
		//Out.vUVW.y = vUV.y;
		if (g_bFlowB)
		{
			Out.vDistortion2.xyz = vUVW;
			Out.vDistortion2.a = g_vColor.a*vUVW;
		}
		else
		{
			Out.vDistortion1.xyz = vUVW;
			Out.vDistortion1.a = g_vColor.a*vUVW;
		}
	}
	return Out;
}

PS_OUT			PS_MAIN_Distot_G(PS_IN_Effect In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vDistortion1 = 0.f;
	//Out.vFog = 0.f;
	vector		vDistort = tex2D(DistortionSampler, In.vTexUV);
	
	float3 vUVW = 0.f;
	vUVW.xy = In.vTexUV;

	float fNoise = 0.f;
	fNoise = (vDistort.g > 0.f) ? vDistort.g : 0.f;
	if (fNoise > 0.f)
	{
		float2 vJump = 0.f;

		float2 vFlow = length(fNoise).xx;
		vFlow = vFlow * 2.f - 1.f;
		//vUV.x += cos(fNoise*g_ftime*5.f)*5.f;
		//vUV.y += sin(fNoise*g_ftime*5.f)*5.f;
		vUVW = FlowUVW(In.vTexUV, vFlow*g_fFlowStrength, vJump, g_ftime);
	}
	//Out Distortion
	if (fNoise > 0.f)
	{
		if (g_bFlowB)
		{
			Out.vDistortion2.xyz = vUVW;
			Out.vDistortion2.a = g_vColor.a*vUVW;
		}
		else
		{
			Out.vDistortion1.xyz = vUVW;
			Out.vDistortion1.a = g_vColor.a*vUVW;
		}
	}
	return Out;
}

PS_OUT			PS_MAIN_Distot_B(PS_IN_Effect In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vDistortion1 = 0.f;
	//Out.vFog = 0.f;
	vector		vDistort = tex2D(DistortionSampler, In.vTexUV);

	float2 vUV = In.vTexUV;

	float fNoise = 0.f;
	fNoise = (vDistort.a > 0.f) ? vDistort.b : 0.f;
	if (fNoise > 0.f)
	{
		vUV.x += cos(fNoise*g_ftime*5.f)*5.f;
		vUV.y += sin(fNoise*g_ftime*5.f)*5.f;
	}
	//Out Distortion
	if (fNoise > 0.f)
	{
		//Out.vUV.a = 1.f;
		//Out.vUVW.x = vUV.x;
		//Out.vUVW.y = vUV.y;
		Out.vDistortion1.xy = vUV;
		Out.vDistortion1.a = g_vColor.a*vUV;
	}
	return Out;
}

PS_OUT			PS_MAIN_Distot_RGB_FLOW_U(PS_IN_Effect In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vDistortion1 = 0.f;
	//Out.vFog = 0.f;
	float fTime = g_bFlowB ? -g_ftime*0.5f : g_ftime ;
	float3 vUVW = 0.f;
	float2 vUV = In.vTexUV;
	//vUV.y += fTime;
	vUV.y += fTime;
	if (g_bFlowB)
	{
		vUV.x += fTime;
		vUV.x += 0.2f;
	}
	vector		vDistort = tex2D(DistortionSampler, vUV);

	vUVW.xy = In.vTexUV;


	float fNoise = 0.f;
	fNoise = (vDistort.a > 0.f) ? vDistort.rgb : 0.f;
	if (fNoise > 0.f)
	{
		float2 vJump = 0.f;


		float2 vFlow = length(vDistort.rgb).xx;
		vFlow = vFlow * 2.f - 1.f;
		//vUV.x += cos(fNoise*g_ftime*5.f)*5.f;
		//vUV.y += sin(fNoise*g_ftime*5.f)*5.f;
		vUVW = g_bFlowB ? FlowUVW(In.vTexUV, vFlow*g_fFlowStrength, vJump, g_ftime*0.5) : FlowUVW(In.vTexUV, vFlow*g_fFlowStrength, vJump, g_ftime);
	}
	//Out Distortion
	if (fNoise > 0.f)
	{
		//Out.vUV.a = 1.f;
		//Out.vUVW.x = vUV.x;
		//Out.vUVW.y = vUV.y;
		if (g_bFlowB)
		{
			Out.vDistortion2.xyz = vUVW;
			Out.vDistortion2.a = g_vColor.a*length(vUVW);
		}
		else
		{
			Out.vDistortion1.xyz = vUVW;
			Out.vDistortion1.a = g_vColor.a*length(vUVW);
		}
	}

	return Out;
}


PS_OUT			PS_MAIN_Distot_RGB_FLOW_U_Filter(PS_IN_Effect In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vDistortion1 = 0.f;
	//Out.vFog = 0.f;
	float fTime = g_bFlowB ? -g_ftime : g_ftime;
	float3 vUVW = 0.f;
	float2 vUV = In.vTexUV;
	//vUV.y += fTime;
	vUV.y += fTime;
	if (g_bFlowB)
		vUV.x += fTime;
	vector		vDistort = tex2D(DistortionSampler, vUV);
	vector		vFilter = tex2D(FilterSampler, In.vTexUV);
	vUVW.xy = In.vTexUV;


	float fNoise = 0.f;
	fNoise = (vDistort.a > 0.f) ? vDistort.rgb : 0.f;
	if (fNoise > 0.f)
	{
		float2 vJump = 0.f;


		float2 vFlow = length(vDistort.rgb).xx;
		vFlow = vFlow * 2.f - 1.f;
		//vUV.x += cos(fNoise*g_ftime*5.f)*5.f;
		//vUV.y += sin(fNoise*g_ftime*5.f)*5.f;
		vUVW = FlowUVW(In.vTexUV, vFlow*g_fFlowStrength, vJump, g_ftime);
	}
	//Out Distortion
	if (fNoise > 0.f)
	{
		//Out.vUV.a = 1.f;
		//Out.vUVW.x = vUV.x;
		//Out.vUVW.y = vUV.y;
		if (g_bFlowB)
		{
			Out.vDistortion2.xyz = vUVW;
			Out.vDistortion2.a = vFilter.r;
		}
		else
		{
			Out.vDistortion1.xyz = vUVW;
			Out.vDistortion1.a = vFilter.r;
		}
	}

	return Out;
}


PS_OUT			PS_MAIN_Distot_RGB_Filter(PS_IN_Effect In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vDistortion1 = 0.f;
	//Out.vFog = 0.f;
	vector		vDistort = tex2D(DistortionSampler, In.vTexUV);
	vector		vFilter = tex2D(FilterSampler, In.vTexUV);

	float3 vUVW = 0.f;
	vUVW.xy = In.vTexUV;

	float fNoise = 0.f;
	fNoise = (vDistort.a > 0.f) ? vDistort.rgb : 0.f;
	if (fNoise > 0.f)
	{
		float2 vJump = 0.f;
		float2 vFlow = length(vDistort.rgb).xx;
		vFlow = vFlow * 2.f - 1.f;
		//vUV.x += cos(fNoise*g_ftime*5.f)*5.f;
		//vUV.y += sin(fNoise*g_ftime*5.f)*5.f;
		vUVW = FlowUVW(In.vTexUV, vFlow*g_fFlowStrength, vJump, g_ftime);
	}
	//Out Distortion
	if (fNoise > 0.f)
	{
		//Out.vUV.a = 1.f;
		//Out.vUVW.x = vUV.x;
		//Out.vUVW.y = vUV.y;
		if (g_bFlowB)
		{
			Out.vDistortion2.xyz = vUVW;
			Out.vDistortion2.a = vFilter.r;
		}
		else
		{
			Out.vDistortion1.xyz = vUVW;
			Out.vDistortion1.a = vFilter.r;
		}
	}

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

	
	vEmissive = tex2D(EmissiveSampler, float2(In.vTexUV.x+ length(tex2D(DistortionSampler, In.vTexUV + g_ftime).rgb), In.vTexUV.y));
	
	if (vEmissive.r >0.f)
	{

		vEmissive.rgb *= In.vColor.rgb;

		vEmissive.a = In.vColor.a;

		if(noise)
			vEmissive.a -= noise;
	}
	else
	{
		vEmissive.rgb = 0.f;
		vEmissive.a = 0.f;
	}
	//Out Emissive 
	Out.vDistortion1 = vector(pow(abs(vEmissive), 2.2).rgb, vEmissive.a);

	return Out;
}




technique Default_Device
{
	pass    Distortion_RGB//0
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_RGB();
	}
	pass    Distortion_R//1
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_R();
	}
	pass    Distortion_G//2
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_G();
	}
	pass    Distortion_B//3
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_B();
	}
	pass    Distortion_RGB_FlowUV//4
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_RGB_FLOW_U();
	}
	pass    Distortion_R//5
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_R();
	}
	pass    Distortion_G//6
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_G();
	}
	pass    Distortion_B//7
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_B();
	}
	pass    Distortion_RGB_FlowUV_Filter//8
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_RGB_FLOW_U_Filter();
	}
		pass    Distortion_RGB_Filter//9
	{
		zwriteenable = false;
		cullmode = None;
		alphablendenable = true;

		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN_Effect();
		PixelShader = compile ps_3_0 PS_MAIN_Distot_RGB_Filter();
	}
	pass Effect_Color		//1
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
