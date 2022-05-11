#include "Shader_value.h"

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


texture            g_RoughnessTexture;

sampler        RoughnessSampler = sampler_state
{
	texture = g_RoughnessTexture;

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
texture			g_DissolveTexture;

sampler		DissolveSampler = sampler_state
{
	texture = g_DissolveTexture;

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


float4 DoDissolve(float3 vTex, float2 vUV);
float4 DoDissolveReverse(float3 vTex, float2 vUV, float fAlpha);
struct VS_IN
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float3			vNormal : NORMAL;
	float3			vTangent : TANGENT;
	float3			vBinormal : BINORMAL;
};

struct VS_OUT
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	float4			vLightProjPos : TEXCOORD2;
	float3			vNormal : NORMAL;
	float3			vTangent : TANGENT;
	float3			vBinormal : BINORMAL;
};

// 정점 쉐이더

VS_OUT			VS_MAINOren(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	In.vPosition.w = 1.f;
	Out.vLightProjPos = In.vPosition;
	Out.vPosition = mul(In.vPosition, g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matView);
	Out.vPosition = mul(Out.vPosition, g_matProj);

	Out.vNormal = normalize(mul(In.vNormal, (float3x3)g_matWorld));


	Out.vTangent = normalize(mul(In.vPosition.xyz, (float3x3)g_matWorld));
	Out.vBinormal = cross(Out.vNormal, Out.vTangent);
	Out.vTangent = cross(Out.vBinormal, Out.vNormal);

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	return Out;
}
struct PS_INOren
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	float4			vLightProjPos : TEXCOORD2;
	float3			vNormal : NORMAL;
	float3			vTangent : TANGENT;
	float3			vBinormal : BINORMAL;

};
struct PS_OUT
{
	vector		vColor : COLOR0;
	vector      vEmissive : COLOR1;
	vector		vNormal : COLOR2;
	vector		vDepth : COLOR3;
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

		color0 += pow(abs(tex2D(EmissiveSampler, float2(vUV.x + gaussFilter[i].x * texscaler,
			vUV.y + gaussFilter[i].y * texscaler))), 2.2)
			* samplesWe[i];
	}

	for (i = 0; i<7; i++)
	{

		color1 += pow(abs(tex2D(EmissiveSampler, float2(vUV.x + gaussFilter[i].y * texscaler,
			vUV.y + gaussFilter[i].x * texscaler))), 2.2)
			* samplesWe[i];
	}


	return  (color0*0.9 + color1*0.5);
}
PS_OUT			PS_MAIN_TOTAL(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor, vBumpMap, vEmissive, vOpacity, vSpecular;
	float fRough;
	vBumpMap = 0.f;
	vColor = pow(abs(tex2D(BaseSampler, In.vTexUV)),2.2);
	vBumpMap = pow(abs(tex2D(NormalSampler, In.vTexUV)), 2.2);
	vEmissive = 0.f;
	vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2);
	vOpacity = pow(abs(tex2D(OpacitySampler, In.vTexUV)), 2.2);
	vSpecular = 0.f;
	vSpecular =tex2D(SpecluarSampler, In.vTexUV);
	fRough = 0.f;
	fRough +=tex2D(RoughnessSampler, In.vTexUV).r;


	float3 vBumpNormal;
	float3 vLightDir;
	float lightIntensity;
	float4 vNormal, vCamEye, vInvLightDir, vDiffuseNormal;


	//Out Depth
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w,	// R의 위치에 z나누기가 끝난 투영의 z를 보관
		0.f,			// B의 위치에 뷰스페이스 영역의 z를 보관(1000(FAR)으로 나누고 있는 중 : 텍스쳐UV는 0~1이니깐)
		In.vProjPos.w * 0.001f,
		1.f);

	//Out Normal
	if (vBumpMap.g || vBumpMap.b)
	{
		float4 vInTangent, vInBinormal;

		vInTangent = 0.f;
		vInBinormal = 0.f;

		vBumpMap.g = 1.f - vBumpMap.g;
		vBumpMap.r = 1.f - vBumpMap.r;
		vBumpNormal = normalize((vBumpMap.xyz * 2.f) - 1.f);

		float3 WorldNormal = normalize((vBumpNormal.x * In.vTangent)
			+ (vBumpNormal.y * In.vBinormal) + (vBumpNormal.z * In.vNormal));

		vNormal = vector(WorldNormal.x, WorldNormal.y, WorldNormal.z, 1.f);
		vNormal.xyz *= .5;
		vNormal.xyz += float3(.5, .5, .5);

		Out.vNormal = float4(vNormal.x, vNormal.y, vNormal.z, 1.0);
	}
	else
	{
		vNormal = vector(normalize(In.vNormal), 1.f);

		vNormal.xyz *= .5;
		vNormal.xyz += float3(.5, .5, .5);

		Out.vNormal = float4(vNormal.x, vNormal.y, vNormal.z, 1.0);
	}

	////Out Rough
	Out.vNormal.rgb = normalize(Out.vNormal.rgb)*(1.f+ fRough);

	//Out Specular
	Out.vDepth.g = pow(abs(vSpecular.x),2.2);

	//Out Alebedo
	Out.vColor = vColor;
	Out.vColor.a = 1.f;

	if (vOpacity.r)
	{
		Out.vColor.a -= vOpacity.r;
		Out.vNormal.a = Out.vColor.a;
	}
	
	//Out Emissive 
	Out.vEmissive = vEmissive;
	Out.vEmissive.a = 1.f;

	return Out;
}

PS_OUT			PS_MAIN_NO(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor, vEmissive, vOpacity, vSpecular;
	float fRough;

	vColor = pow(abs(tex2D(BaseSampler, In.vTexUV)),2.2);
	vEmissive = 0.f;
	vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)),2.2);
	vOpacity = pow(abs(tex2D(OpacitySampler, In.vTexUV)),2.2);
	
	vSpecular = 0.f;
	vSpecular = tex2D(SpecluarSampler, In.vTexUV);
	fRough = 0.f;
	fRough += tex2D(RoughnessSampler, In.vTexUV).r;

	float3 vBumpNormal;
	float3 vLightDir;
	float lightIntensity;
	float4 vNormal, vCamEye, vInvLightDir, vDiffuseNormal;

	//Out Depth
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w,	// R의 위치에 z나누기가 끝난 투영의 z를 보관
		0.f,			// B의 위치에 뷰스페이스 영역의 z를 보관(1000(FAR)으로 나누고 있는 중 : 텍스쳐UV는 0~1이니깐)
		In.vProjPos.w * 0.001f,
		1.f);


	//Out Normal
	vNormal = vector(normalize(In.vNormal), 1.f);

	vNormal.xyz *= .5;
	vNormal.xyz += float3(.5, .5, .5);

	Out.vNormal = float4(vNormal.x, vNormal.y, vNormal.z, 1.0);
	
	////Out Rough
	Out.vNormal.rgb = normalize(Out.vNormal.rgb)*(1.f + fRough);

	//Out Specular
	Out.vDepth.g = pow(abs(vSpecular.x), 2.2);

	//Out vOpavity
	if (vOpacity.r)
	{
		Out.vColor.a = 0.f;
		Out.vNormal = 0.f;
	}
	//Out Specular	
	Out.vDepth.g += vSpecular.x;

	//Out Alebedo
	Out.vColor = vColor;
	Out.vColor.a = 1.f;

	//Out Emissive 
	Out.vEmissive = vEmissive;
	Out.vEmissive.a = 1.f;
	
	return Out;
}

PS_OUT			PS_MAIN_BUMP(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor, vBumpMap, vEmissive, vOpacity, vSpecular;
	float fRough;

	vBumpMap = 0.f;
	vColor = pow(abs(tex2D(BaseSampler, In.vTexUV)),2.2);
	vBumpMap = pow(abs(tex2D(NormalSampler, In.vTexUV)),2.2);
	vEmissive = 0.f;
	vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)),2.2);
	vOpacity = pow(abs(tex2D(OpacitySampler, In.vTexUV)),2.2);
	vSpecular = 0.f;
	vSpecular = tex2D(SpecluarSampler, In.vTexUV);
	fRough = 0.f;
	fRough += tex2D(RoughnessSampler, In.vTexUV).r;

	float3 vBumpNormal;
	float3 vLightDir;
	float lightIntensity;
	float4 vNormal, vCamEye, vInvLightDir, vDiffuseNormal;

	//Out Depth
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w,	// R의 위치에 z나누기가 끝난 투영의 z를 보관
		0.f,			// B의 위치에 뷰스페이스 영역의 z를 보관(1000(FAR)으로 나누고 있는 중 : 텍스쳐UV는 0~1이니깐)
		In.vProjPos.w * 0.001f,
		1.f);
	
	//Out Normal
	float3 vInNormal;
	float4 vInTangent, vInBinormal;

	vInTangent = 0.f;
	vInBinormal = 0.f;

	vBumpMap.g = 1.f - vBumpMap.g;
	vBumpMap.r = 1.f - vBumpMap.r;
	vBumpNormal = normalize((vBumpMap.xyz * 2.f) - 1.f);

	float3 WorldNormal = normalize((vBumpNormal.x * In.vTangent)
		+ (vBumpNormal.y * In.vBinormal) + (vBumpNormal.z * In.vNormal));

	vNormal = vector(WorldNormal.x, WorldNormal.y, WorldNormal.z, 1.f);

	vNormal.xyz *= .5;
	vNormal.xyz += float3(.5, .5, .5);

	Out.vNormal = float4(vNormal.x, vNormal.y, vNormal.z, 1.0);

	////Out Rough
	Out.vNormal.rgb = normalize(Out.vNormal.rgb)*(1.f + fRough);

	//Out Specular
	Out.vDepth.g = pow(abs(vSpecular.x), 2.2);

	//Out Alebedo
	Out.vColor = vColor;
	Out.vColor.a = 1.f;

	//Out Emissive 
	Out.vEmissive = vEmissive;
	Out.vEmissive.a = 1.f;

	//Out Opacity
	if (vOpacity.r)
	{
		Out.vColor.a = 0.f;
		Out.vNormal = 0.f;
	}


	return Out;
}

PS_OUT			PS_MAIN_Effect(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = 0.f;
	Out.vDepth = 0.f;
	Out.vEmissive = 0.f;
	Out.vNormal = 0.f;
	vector vEmissive;

	vEmissive = 0.f;
	vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2);


	//Out Emissive 
	Out.vEmissive = vEmissive;
	return Out;
}


PS_OUT	PS_MAIN_TOTAL_DISSOLVE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_TOTAL(In);

	float3 vDissolve = tex2D(DissolveSampler, float2(In.vTexUV.x, In.vTexUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);
	float v = saturate(pow(abs(mul1 + mul2), 20));
	float b = v*2.f;

	float alpha = 2.f - b;
	if (2.2f >= b)
	{
		if (g_fDissolve_per < b)
		{
			//이미시브
			Out.vEmissive.rgb += g_vColor.rgb;
			Out.vColor.a = 1.f - v;
			Out.vEmissive.a = alpha;
			Out.vNormal.a = alpha;
			Out.vDepth.a = alpha;
		}
		else
		{
			Out.vEmissive.a = 1.f - v;
			Out.vColor.a = 1.f - v;
			Out.vNormal.a = 1.f - v;
			Out.vDepth.a = 1.f - v;
		}
	}
	else
	{
		Out.vEmissive.rgb += g_vColor.rgb;
		Out.vColor.a = 1.f - v;
		Out.vEmissive.a = alpha;
		Out.vNormal.a = alpha;
		Out.vDepth.a = alpha;
		//이미시브
	}
	return Out;
}
PS_OUT	PS_MAIN_NO_DISSOLVE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_NO(In);
	//Out.vColor = 0.f;
	float3 vDissolve = tex2D(DissolveSampler, float2(In.vTexUV.x, In.vTexUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);
	float v = saturate(pow(abs(mul1 + mul2), 20));
	float b = v*2.f;

	float alpha = 2.f - b;
	if (2.2f >= b)
	{
		if (g_fDissolve_per < b)
		{
			//이미시브
			Out.vEmissive.rgb += g_vColor.rgb;
			Out.vColor.a = 1.f - v;
			Out.vEmissive.a = alpha;
			Out.vNormal.a = alpha;
			Out.vDepth.a = alpha;
		}
		else
		{
			Out.vEmissive.a = 1.f - v;
			Out.vColor.a = 1.f - v;
			Out.vNormal.a = 1.f - v;
			Out.vDepth.a = 1.f - v;
		}
	}
	else
	{
		Out.vEmissive.rgb += g_vColor.rgb;
		Out.vColor.a = 1.f - v;
		Out.vEmissive.a = alpha;
		Out.vNormal.a = alpha;
		Out.vDepth.a = alpha;
		//이미시브
	}

	return Out;
}
PS_OUT	PS_MAIN_BUMP_DISSOLVE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_BUMP(In);
	float3 vDissolve = tex2D(DissolveSampler, float2(In.vTexUV.x, In.vTexUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);
	float v = saturate(pow(abs(mul1 + mul2), 20));
	float b = v*2.f;
	float alpha = 2.f - b;
	if (2.2f >= b)
	{
		if (g_fDissolve_per < b)
		{
			//이미시브
			Out.vEmissive.rgb += g_vColor.rgb;
			Out.vColor.a = 1.f - v;
			Out.vEmissive.a = alpha;
			Out.vNormal.a = alpha;
			Out.vDepth.a = alpha;
		}
		else
		{
			Out.vEmissive.a = 1.f - v;
			Out.vColor.a = 1.f - v;
			Out.vNormal.a = 1.f - v;
			Out.vDepth.a = 1.f - v;
		}
	}
	else
	{
		Out.vEmissive.rgb += g_vColor.rgb;
		Out.vColor.a = 1.f - v;
		Out.vEmissive.a = alpha;
		Out.vNormal.a = alpha;
		Out.vDepth.a = alpha;
		//이미시브
	}

	return Out;
}

PS_OUT	PS_MAIN_TOTAL_DISSOLVE_RE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_TOTAL(In);
	float3 vDissolve = tex2D(DissolveSampler, float2(In.vTexUV.x*g_ftime, In.vTexUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);
	float v = saturate(pow(abs(mul1 + mul2), 20));
	//float b = (2.f - v*2.f);
	float b = (1.f - v)*2.f;
	float c = 1.f - v;
	float alpha = 2.f - b;
	if (2.2f >= b)
	{
		if (g_fDissolve_per < b)
		{
			//이미시브
			Out.vEmissive.rgb += g_vColor.rgb;
			Out.vColor.a = 1.f - c;
			Out.vEmissive.a = alpha;
			Out.vNormal.a = alpha;
			Out.vDepth.a = alpha;
		}
		else
		{
			Out.vEmissive.a = 1.f - c;
			Out.vColor.a = 1.f - c;
			Out.vNormal.a = 1.f - c;
			Out.vDepth.a = 1.f - c;
		}
	}
	else
	{
		Out.vEmissive.rgb += g_vColor.rgb;
		Out.vColor.a = 1.f - c;
		Out.vEmissive.a = alpha;
		Out.vNormal.a = alpha;
		Out.vDepth.a = alpha;
		//이미시브
	}
	return Out;
}
PS_OUT	PS_MAIN_NO_DISSOLVE_RE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_NO(In);
	float3 vDissolve = tex2D(DissolveSampler, float2(In.vTexUV.x*g_ftime, In.vTexUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);
	float v = saturate(pow(abs(mul1 + mul2), 20));
	//float b = (2.f - v*2.f);
	float b = (1.f - v)*2.f;
	float c = 1.f - v;
	float alpha = 2.f - b;
	if (2.2f >= b)
	{
		if (g_fDissolve_per < b)
		{
			//이미시브
			Out.vEmissive.rgb += g_vColor.rgb;
			Out.vColor.a = 1.f - c;
			Out.vEmissive.a = alpha;
			Out.vNormal.a = alpha;
			Out.vDepth.a = alpha;
		}
		else
		{
			Out.vEmissive.a = 1.f - c;
			Out.vColor.a = 1.f - c;
			Out.vNormal.a = 1.f - c;
			Out.vDepth.a = 1.f - c;
		}
	}
	else
	{
		Out.vEmissive.rgb += g_vColor.rgb;
		Out.vColor.a = 1.f - c;
		Out.vEmissive.a = alpha;
		Out.vNormal.a = alpha;
		Out.vDepth.a = alpha;
		//이미시브
	}

	return Out;
}
PS_OUT	PS_MAIN_BUMP_DISSOLVE_RE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_BUMP(In);

	float3 vDissolve = tex2D(DissolveSampler, float2(In.vTexUV.x*g_ftime, In.vTexUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);
	float v = saturate(pow(abs(mul1 + mul2), 20));
	//float b = (2.f - v*2.f);
	float b = (1.f - v)*2.f;
	float c = 1.f - v;
	float alpha = 2.f - b;
	if (2.2f >= b)
	{
		if (g_fDissolve_per < b)
		{
			//이미시브
			Out.vEmissive.rgb += g_vColor.rgb;
			Out.vColor.a = 1.f - c;
			Out.vEmissive.a = alpha;
			Out.vNormal.a = alpha;
			Out.vDepth.a = alpha;
		}
		else
		{
			Out.vEmissive.a = 1.f - c;
			Out.vColor.a = 1.f - c;
			Out.vNormal.a = 1.f - c;
			Out.vDepth.a = 1.f - c;
		}
	}
	else
	{
		Out.vEmissive.rgb += g_vColor.rgb;
		Out.vColor.a = 1.f - c;
		Out.vEmissive.a = alpha;
		Out.vNormal.a = alpha;
		Out.vDepth.a = alpha;
		//이미시브
	}
	
	
	

	return Out;
}
PS_OUT	PS_MAIN_ITEM(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	//Out = PS_MAIN_NO(In);
	//Out.vColor =pow(vector(0.1f,0.1f,0.1f,0.1f),2.2f);
	Out.vColor = g_vColor;
	
	//Out Normal
	float4 vNormal = vector(normalize(In.vNormal), 1.f);
	vNormal.xyz *= .5;
	vNormal.xyz += float3(.5, .5, .5);
	Out.vNormal = float4(vNormal.x, vNormal.y, vNormal.z, 0.05f);

	//Out.vEmissive = pow(vector(0.1f, 0.1f, 0.1f, 0.1f), 2.2f);
	Out.vEmissive = 0.f;
	
	//Out Depth
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w,	// R의 위치에 z나누기가 끝난 투영의 z를 보관
		0.f,			// B의 위치에 뷰스페이스 영역의 z를 보관(1000(FAR)으로 나누고 있는 중 : 텍스쳐UV는 0~1이니깐)
		In.vProjPos.w * 0.001f,
		1.f);

	return Out;
}
float4 DoDissolve(float3 vTex, float2 vUV)
{
	//float3 vDissolve = pow(abs(tex2D(DissolveSampler, vUV)), 2.2).rgb;
	float3 vDissolve =tex2D(DissolveSampler, vUV).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);

	float b = saturate(pow(abs(mul1 + mul2), 20))*2.f;
	//b = b > 0.00001f ? b : 0.f;
	return vector((2.2f >= b ? (g_fDissolve_per >= b ? vTex : g_vColor.rgb) : g_vColor.rgb),2 - b);
}
float4 DoDissolveReverse(float3 vTex, float2 vUV, float fAlpha)
{
	//float3 vDissolve = pow(abs(tex2D(DissolveSampler, vUV)), 2.2).rgb;
	float3 vDissolve =tex2D(DissolveSampler, float2(vUV.x*g_ftime,vUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);

	float b =( 2.f - saturate(pow(abs(mul1 + mul2), 20))*2.f);

	return vector((2.2f >= b ? (g_fDissolve_per >= b ? vTex : g_vColor.rgb) : g_vColor.rgb), 2 - b);
}
technique Default_Device
{

	pass    TotalBump//0
	{

		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAINOren();
		PixelShader = compile ps_3_0 PS_MAIN_TOTAL();
	}
	pass    NoBump//1
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAINOren();
		PixelShader = compile ps_3_0 PS_MAIN_NO();
	}
	pass    BumpOnly//2
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_BUMP();
	}
		pass    Effect//3
	{
		zwriteenable = false;
		cullmode = None;

		alphablendenable = true;
		destblend = invsrcalpha;
		srcblend = srcalpha;
		
		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAINOren();
		PixelShader = compile ps_3_0 PS_MAIN_Effect();
	}

		pass    Malkhel//4
	{
		cullmode = none;

		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAINOren();
		PixelShader = compile ps_3_0 PS_MAIN_TOTAL();
	}	//	pass Dissolve

		pass    TotalBump_Dissolve//5//	pass Dissolve
	{
		cullmode = none;

		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_TOTAL_DISSOLVE();
	}
		pass    NoBump_Dissolve//6//	pass Dissolve
	{
		cullmode = none;

		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_NO_DISSOLVE();
	}
		pass    BumpOnly_Dissolve//7//	pass Dissolve
	{ 
		cullmode = none;
		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_BUMP_DISSOLVE();
	}

		pass    Malkhel_Dissolve//8//	pass Dissolve
	{
		cullmode = none;

	alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_TOTAL_DISSOLVE();
	}	
		pass    TotalBump_DissolveReverse//9//	pass DissolveRe
	{
		cullmode = none;

	alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_TOTAL_DISSOLVE_RE();
	}
		pass    NoBump_DissolveReverse//10//	pass DissolveRe
	{
		cullmode = none;

	alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_NO_DISSOLVE_RE();
	}
		pass    BumpOnly_DissolveReverse//11//	pass DissolveRe
	{
		cullmode = none;
	alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_BUMP_DISSOLVE_RE();
	}
		pass    OnlyItem		//12//	pass OnlyItem
	{ 
		cullmode = none;
		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_ITEM();
	}
};
