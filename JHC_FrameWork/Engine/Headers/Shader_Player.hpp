#include "Shader_Value.h"
#include "Shader_TotalFuction.h"

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

texture			g_DissolveTexture;

sampler		DissolveSampler = sampler_state
{
	texture = g_DissolveTexture;

	minfilter = linear;
	magfilter = linear;
};

#pragma endregion

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
	float4			vBlendIndices : BLENDINDICES;
	float4			vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	float4			vLightProjPos : TEXCOORD2;
	float4			vWorldPos : TEXCOORD3;
	float3			vNormal : NORMAL;
	float3			vTangent : TANGENT;
	float3			vBinormal : BINORMAL;
};
float4 DoDissolve(float3 vTex, float2 vUV);

// 정점 쉐이더
VS_OUT			VS_MAINOren(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4 vPos = float4(0.f, 0.f, 0.f, 1.f);
	float3 vNormal = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int iMax = g_numBonInfluences - 1;

	In.vNormal = normalize(In.vNormal);
	
	for (int i = 0; i < iMax; ++i)
	{
		lastWeight += In.vBlendWeight[i];

		vPos += In.vBlendWeight[i] * float4(mul(In.vPosition, g_MatrixPalette[In.vBlendIndices[i]]).xyz, 1.f);
		vNormal += In.vBlendWeight[i] * mul(In.vNormal, (float3x3)g_MatrixPalette[In.vBlendIndices[i]]);
	}

	lastWeight = 1.f - lastWeight;
	vPos += lastWeight * float4(mul(In.vPosition, g_MatrixPalette[In.vBlendIndices[iMax]]).xyz, 1.f);
	vNormal += lastWeight * mul(In.vNormal, (float3x3)g_MatrixPalette[In.vBlendIndices[iMax]]);
	vPos.w = 1.f;


	Out.vPosition = mul(vPos, g_matWorld);	// 하드웨어 스키닝 계산 끝난 Pos
	Out.vWorldPos = Out.vPosition;

	Out.vPosition = mul(Out.vPosition, g_matView);
	Out.vPosition = mul(Out.vPosition, g_matProj);

	Out.vNormal = normalize(mul(vNormal, (float3x3)g_matWorld));	// 하드웨어 스키닝 계산 끝난 Normal
	
	Out.vTangent = normalize(mul(vPos.xyz, (float3x3)g_matWorld));
	Out.vBinormal = cross(Out.vNormal, Out.vTangent);
	Out.vTangent = cross(Out.vBinormal, Out.vNormal);

	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PS_IN
{
	//float4			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	float3			vNormal : NORMAL;
	float3			vTangent : TANGENT;
	float3			vBinormal : BINORMAL;
};

struct PS_INOren
{
	float4			vPosition : TEXCOORD3;
	float2			vTexUV : TEXCOORD0;
	float4			vProjPos : TEXCOORD1;
	float4			vLightProjPos : TEXCOORD2;
	float4			vWorldPos : TEXCOORD3;
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

PS_OUT			PS_MAIN_N(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor, vBumpMap, vEmissive, vOpacity;
	vBumpMap = 0.f;
	vColor = pow(abs(tex2D(BaseSampler, In.vTexUV)),2.2);
	vBumpMap = pow(abs(tex2D(NormalSampler, In.vTexUV)), 2.2);
	vEmissive = 0.f;
	vEmissive = pow(abs(tex2D(EmissiveSampler, In.vTexUV)), 2.2);
	vOpacity = pow(abs(tex2D(OpacitySampler, In.vTexUV)), 2.2);

	float3 vBumpNormal;
	float3 vLightDir;
	float lightIntensity;
	float4 vNormal, vCamEye, vInvLightDir, vDiffuseNormal;
	float3 vVtxNormal;
	vVtxNormal = In.vNormal;

	//Out Depth
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w,	// R의 위치에 z나누기가 끝난 투영의 z를 보관
		0,			// B의 위치에 뷰스페이스 영역의 z를 보관(1000(FAR)으로 나누고 있는 중 : 텍스쳐UV는 0~1이니깐)
		In.vProjPos.w * 0.001f,
		1.f);

	//Out Normal
	//if (vBumpMap.g || vBumpMap.b)
	//{
		float3 vInNormal;
		float4 vInTangent, vInBinormal;
		
		vInTangent = 0.f;
		vInBinormal = 0.f;

	
		vBumpMap.g = 1.f - vBumpMap.g;
		vBumpMap.r = 1.f - vBumpMap.r;
		vBumpNormal = normalize((vBumpMap.xyz * 2.f) - 1.f);

		//후보 0
		float3 WorldNormal = normalize((vBumpNormal.x * In.vTangent) 
			+ (vBumpNormal.y * In.vBinormal) + (vBumpNormal.z * In.vNormal));
		
		vNormal = vector(WorldNormal.x, WorldNormal.y, WorldNormal.z, 1.f);
		
		vNormal = normalize(vNormal);
		vNormal.xyz *= .5;
		vNormal.xyz += float3(.5, .5, .5);

		Out.vNormal = float4(vNormal.x, vNormal.y, vNormal.z, 1.0);

	//}
		Out.vNormal.rgb = normalize(Out.vNormal.rgb);
	//Out Alebedo
	Out.vColor = vColor;

	//Out.vColor.rgb += Ps_Rim_Light(dot(Out.vNormal,
	//	normalize(g_vCamPos - In.vProjPos)));
	//Out.vColor.rgb += Ps_Rim_Light(dot(Out.vNormal.rgb, normalize(-g_vLightDir).rgb));
	//Out.vColor.rgb += Ps_Rim_Light(1-dot(normalize(-g_vLightDir).rgb, Out.vNormal.rgb));
	//float NoL = dot(normalize(Out.vNormal.rgb), normalize(-g_vLightDir).rgb);
	//float NoH = dot(normalize(Out.vNormal.rgb), normalize(normalize(g_vCamPos - In.vProjPos) - normalize(-g_vLightDir).rgb));
	
	
	float NoV = dot(In.vNormal.rgb, normalize(g_vCamPos.rgb - In.vWorldPos.rgb));
	//float3 Fresnel = pow(abs(1 - saturate(NoV)), 10.f)*float3(0.0745f*0.5f, 0.3568f*0.5f, 0.2745f*0.5f);
	//float3 Fresnel = pow(abs(1 - saturate(NoV)), 13.f)*float3(0.03f, 0.05f, 0.05f);
	//float Back = pow(abs(max(0, -LoV)), 10.f)*0.2f;

	//Out.vColor.rgb += Ps_Rim_Light(1 - NoH);
	//Out.vEmissive.rgb += Ps_Rim_Light(dot(Out.vNormal.rgb, normalize(normalize(g_vCamPos - In.vProjPos) - normalize(-g_vLightDir).rgb)));

	//Out.vColor.rgb += Ps_Rim_Light(dot(Out.vNormal,
	//	normalize(g_vCamPos - In.vProjPos)),
	//	dot(normalize(In.vProjPos.xyz - g_vLightDir.xyz), (In.vProjPos - g_vCamPos).xyz));
	//
	//	
		//Out Emissive 
	Out.vEmissive = vEmissive;
	//if(NoV > 0.25f)
		Out.vEmissive.rgb += Ps_Rim_Light(NoV);
	//Out.vEmissive = vEmissive;
	Out.vEmissive.a = 1.f;

	//Out Opacity
	if (vOpacity.r)
	{
		Out.vColor.a = 0.f;
		Out.vNormal = 0.f;
	}
	return Out;
}
PS_OUT			PS_MAIN_DISSOLVE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_N(In);
	float3 vDissolve = tex2D(DissolveSampler, float2(In.vTexUV.x, In.vTexUV.y)).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);
	float v = saturate(pow(abs(mul1 + mul2), 20));
	float b = v * 2.f;
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
PS_OUT			PS_MAIN_DISSOLVE_RE(PS_INOren In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out = PS_MAIN_N(In);

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
float4 DoDissolve(float3 vTex, float2 vUV)
{
	float3 vDissolve = pow(abs(tex2D(DissolveSampler, vUV)), 2.2).rgb;
	//float3 vDissolve =tex2D(DissolveSampler, vUV).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);

	float b = saturate(pow(abs(mul1+mul2), 20));
	return vector((1.0f >= b ? (g_fDissolve_per >= b ? vTex : g_vColor.rgb) : g_vColor.rgb), 1 - b);	
}

technique Default_Device
{

	pass  Player	//0
	{

		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
		//stencilenable = true;
		//destblend = invsrccolor;
		/*	srcblend = one;
		destblend = one;*/
		VertexShader = compile vs_3_0 VS_MAINOren();
		PixelShader = compile ps_3_0 PS_MAIN_N();
	}
	pass  Dissolve  //1
	{
		cullmode = none;
		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;
	//stencilenable = true;
	//destblend = invsrccolor;
	/*	srcblend = one;
	destblend = one;*/
	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_DISSOLVE();
	}
	pass  Dissolve_Reverse //2
	{
		cullmode = none;
		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;
	VertexShader = compile vs_3_0 VS_MAINOren();
	PixelShader = compile ps_3_0 PS_MAIN_DISSOLVE_RE();
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
