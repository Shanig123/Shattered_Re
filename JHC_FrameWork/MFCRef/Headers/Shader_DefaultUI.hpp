#include "Shader_Value.h"

#pragma region Value

float g_fU;
#pragma endregion


#pragma region Texture

texture		g_BaseTexture;

sampler		BaseSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
	AddressU = clamp;
	AddressV = clamp;
	AddressW = clamp;
};
sampler		BaseFlowSampler = sampler_state
{
	texture = g_BaseTexture;

	minfilter = linear;
	magfilter = linear;
	AddressU = wrap;
	AddressV = wrap;
	AddressW = wrap;
};
texture		g_FlowTexture;

sampler		FlowSampler = sampler_state
{
	texture = g_FlowTexture;

	minfilter = linear;
	magfilter = linear;
	AddressU = wrap;
	AddressV = wrap;
	AddressW = wrap;
};


texture		g_DepthTexture;

sampler		DepthSampler = sampler_state
{
	texture = g_DepthTexture;	
};

texture			g_DissolveTexture;

sampler		DissolveSampler = sampler_state
{
	texture = g_DissolveTexture;

minfilter = linear;
magfilter = linear;
};

#pragma endregion


float4 DoDissolve(float3 vTex, float2 vUV);
float4 DoDissolveRe(float3 vTex, float2 vUV);

struct VS_IN
{
	float4			vPosition	: POSITION;
	float2			vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition	: POSITION;
	float2			vTexUV		: TEXCOORD0;
	float4			vProjPos	: TEXCOORD1;
};

// 정점 쉐이더

VS_OUT			VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

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
	float2			vTexUV : TEXCOORD0;
	vector			vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};


PS_OUT			PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = pow(abs(tex2D(BaseSampler, In.vTexUV)),2.2);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor = pow(abs(Out.vColor),2.2);

	//Out.vColor = pow(abs(Out.vColor), 2.2*0.75);

	//Out.vColor = pow(abs(Out.vColor),1/ 2.2);
	return Out;
}
PS_OUT			PS_MAIN_U(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//Out.vColor = pow(abs(tex2D(BaseSampler, In.vTexUV)),2.2);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor = tex2D(BaseFlowSampler, float2(In.vTexUV.x + g_ftime, In.vTexUV.y));
	Out.vColor = pow(abs(Out.vColor), 2.2);

	//Out.vColor = pow(abs(Out.vColor), 2.2*0.75);

	//Out.vColor = pow(abs(Out.vColor),1/ 2.2);
	return Out;
}
PS_OUT			PS_MAIN_Stat(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	if (g_fU < In.vTexUV.x)
		return Out;
	//Out.vColor = pow(abs(tex2D(BaseSampler, In.vTexUV)),2.2);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor = tex2D(BaseFlowSampler, float2(In.vTexUV.x + g_ftime, In.vTexUV.y));
	Out.vColor = pow(abs(Out.vColor), 2.2);

	//Out.vColor = pow(abs(Out.vColor), 2.2*0.75);

	//Out.vColor = pow(abs(Out.vColor),1/ 2.2);
	return Out;
}
PS_OUT			PS_MAIN_StatMask(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	if (g_fU < In.vTexUV.x)
		return Out;

	Out.vColor = tex2D(FlowSampler, float2(In.vTexUV.x + g_ftime, In.vTexUV.y));
	Out.vColor.rgb = 1 - Out.vColor.rgb;
	if (Out.vColor.r > 0.f)
	{

		Out.vColor.rgb *= g_vColor.rgb;
		Out.vColor.a += g_vColor.a;

	}
	else
	{
		Out.vColor.rgb = 0.f;
		Out.vColor.a = 0.f;
	}

	Out.vColor = pow(abs(Out.vColor), 2.2);
	return Out;
}
PS_OUT			PS_MAIN_Mask_U(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(FlowSampler, float2(In.vTexUV.x + g_ftime, In.vTexUV.y));
	if (Out.vColor.r > 0.f)
	{

		Out.vColor.rgb *= g_vColor.rgb;
		Out.vColor.a += g_vColor.a;

	}
	else
	{
		Out.vColor.rgb = 0.f;
		Out.vColor.a = 0.f;
	}

	Out.vColor = pow(abs(Out.vColor), 2.2);

	//Out.vColor = pow(abs(Out.vColor), 2.2*0.75);

	//Out.vColor = pow(abs(Out.vColor),1/ 2.2);
	return Out;
}
PS_OUT			PS_MAIN_INVCOLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor.rgb = 1 - Out.vColor.rgb;
	Out.vColor.rgb = pow(abs(Out.vColor),2.2).rgb;
	return Out;
}
PS_OUT			PS_MAIN_INVCOLOR_U(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(FlowSampler, float2(In.vTexUV.x + g_ftime, In.vTexUV.y));
	Out.vColor.rgb = 1 - Out.vColor.rgb;
	if (Out.vColor.r > 0.f)
	{

		Out.vColor.rgb *= g_vColor.rgb;
		Out.vColor.a += g_vColor.a;

	}
	else
	{
		Out.vColor.rgb = 0.f;
		Out.vColor.a = 0.f;
	}

	Out.vColor = pow(abs(Out.vColor), 2.2);
	return Out;
}
PS_OUT			PS_MAIN_DISSOLVE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor = DoDissolve(Out.vColor.rgb, In.vTexUV);
	Out.vColor = pow(abs(Out.vColor), 2.2);
	//Out.vColor = pow(abs(Out.vColor), 2.2*0.75);

	return Out;
}
PS_OUT			PS_MAIN_DISSOLVE_RE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)	
	Out.vColor = DoDissolveRe(Out.vColor.rgb, In.vTexUV);
	Out.vColor = pow(abs(Out.vColor), 2.2);
	
	//Out.vColor = pow(abs(Out.vColor), 2.2*0.75);

	return Out;
}
PS_OUT			PS_MAIN_DISSOLVE_INV(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor.rgb = 1 - Out.vColor.rgb;
	Out.vColor = DoDissolve(Out.vColor.rgb, In.vTexUV);
	Out.vColor = pow(abs(Out.vColor), 2.2);
	return Out;
}
PS_OUT			PS_MAIN_DISSOLVE_RE_INV(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
	Out.vColor.rgb = 1 - Out.vColor.rgb;
	Out.vColor = DoDissolveRe(Out.vColor.rgb, In.vTexUV);
	Out.vColor = pow(abs(Out.vColor), 2.2);
	return Out;
}
PS_OUT			PS_MAIN_FADE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	Out.vColor.rgb = 0.f;
	Out.vColor.a = g_ftime;
	return Out;
}
//PS_OUT			PS_MAIN_No_Black(PS_IN In)
//{
//	PS_OUT		Out = (PS_OUT)0;
//
//	Out.vColor = tex2D(BaseSampler, In.vTexUV);	// 2차원 텍스처로부터 uv좌표에 해당하는 색을 추출하여 반환하는 함수(반환 타입이 vector타입)
//	//if (Out.vColor.a > 0)
//	//	Out.vColor.rgb += (1.f / 255.f);
//	Out.vColor = pow(abs(Out.vColor), 2.2);
//	return Out;
//}
float4 DoDissolve(float3 vTex, float2 vUV)
{
	//float3 vDissolve = pow(abs(tex2D(DissolveSampler, vUV)), 2.2).rgb;
	float3 vDissolve =tex2D(DissolveSampler, vUV).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);

	float b = saturate(pow(abs(mul1 + mul2), 20));

	return vector((2.2f >= b ? (g_fDissolve_per >= b ? vTex : g_vColor.rgb) : g_vColor.rgb), 1.f - b);
}
float4 DoDissolveRe(float3 vTex, float2 vUV)
{
	//float3 vDissolve = pow(abs(tex2D(DissolveSampler, vUV)), 2.2).rgb;
	float3 vDissolve =tex2D(DissolveSampler, vUV).rgb;
	float mul1 = (length(vDissolve)*g_ftime)*g_fDissolveSpeed;	//	time = 0~1값
	float mul2 = (length(vDissolve)*g_ftime);

	float b = 1.f-saturate(pow(abs(mul1 + mul2), 20));

	return vector((2.2f >= b ? (g_fDissolve_per >= b ? vTex : g_vColor.rgb) : g_vColor.rgb), 1.f - b);
}
technique Default_Device
{
	pass	DeleteBlackColor	// 0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass				// 1
	{	
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = srccolor;
	destblend = invsrcalpha;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_INVCOLOR();
	}
	pass	NoAlpha			//2
	{
	//	alphablendenable = true;
	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass	Dissolve		// 3
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;
	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN_DISSOLVE();
	}
		pass DissolveRe		// 4
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;
	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN_DISSOLVE_RE();
	}
		pass	DissolveInv		// 5
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = srccolor;
	destblend = invsrcalpha;
	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN_DISSOLVE_INV();
	}
		pass DissolveReInv	// 6
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = srccolor;
	
	
	
	destblend = invsrcalpha;
	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN_DISSOLVE_RE_INV();
	}
		pass Fade	// 7
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = srccolor;
	destblend = invsrcalpha;

	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN_FADE();
	}
				pass AlphaBlendOption1	// 8
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = one;
	destblend = destalpha;

	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN();
	}
				pass AlphaBlendOption2	// 9
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = one;
	destblend = srccolor;

	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN();
	}
				pass AlphaBlendOption3	// 10
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = one;
	destblend = one;

	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN();
	}
		pass DissolveReAlphaOption	// 11
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = destcolor;
	destblend = srcalpha;
	VertexShader = compile vs_3_0 VS_MAIN();
	PixelShader = compile ps_3_0 PS_MAIN_DISSOLVE_RE();
	}

		pass	Mask_Flow_UAxis	// 12
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrccolor;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_Mask_U();
	}
		pass	InverseColor_Flow_UAxis			// 13
	{
		alphablendenable = true;
	//BLENDOP = true;
	srcblend = srcalpha;
	destblend = invsrccolor;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_INVCOLOR_U();
	}
		pass	NoAlpha_Flow_UAxis		//14
	{
		//	alphablendenable = true;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_U();
	}
		pass	DeleteBlackColor_Flow_UAxis	// 15
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_U();
	}
		pass	StatusBar		//16
	{
		//	alphablendenable = true;
			alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_Stat();
	}
		pass	StatusBarMask		//17
	{
		//	alphablendenable = true;
			alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrccolor;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_StatMask();
	}
		pass	DeleteBlackColor2	// 18
	{
		alphablendenable = true;
		srcblend = srccolor;
		destblend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};
