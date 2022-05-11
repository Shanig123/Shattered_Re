#pragma region Texture


texture				g_AlbedoTexture;

sampler		AlbedoSampler = sampler_state
{
	texture = g_AlbedoTexture;

minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};

texture				g_ShadeTexture;

sampler		ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;

minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};

texture				g_SpecularTexture;

sampler		SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;

minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};

texture            g_EmissiveTexture;

sampler        EmissiveSampler = sampler_state
{
	texture = g_EmissiveTexture;
minfilter = linear;
magfilter = linear;
AddressU = clamp;
AddressV = clamp;
AddressW = clamp;
SRGBTexture = true;
};

texture            g_NoBlurEmissiveTexture;

sampler        NoBlurEmissiveSampler = sampler_state
{
	texture = g_NoBlurEmissiveTexture;
minfilter = linear;
magfilter = linear;
AddressU = clamp;
AddressV = clamp;
AddressW = clamp;
SRGBTexture = true;
};

texture            g_UITexture;

sampler        UISampler = sampler_state
{
	texture = g_UITexture;
minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};

texture            g_ShadowTexture;
sampler        ShadowSampler = sampler_state
{
	texture = g_ShadowTexture;
minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};
texture            g_EnvironmentTexture;
sampler       EnviornmentSampler = sampler_state
{
	texture = g_EnvironmentTexture;
minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};

texture g_SkyBoxTexture;
sampler       SkyBoxSampler = sampler_state
{
	texture = g_SkyBoxTexture;
minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};

texture g_BlendTexture;
sampler       BlendSampler = sampler_state
{
	texture = g_BlendTexture;
minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};

texture			g_DownTexture;
sampler       DownSampler = sampler_state
{
	texture = g_DownTexture;
minfilter = linear;
magfilter = linear;
SRGBTexture = true;
};


texture			g_HoriBlurTexture;
sampler       HorizontalBlurSampler = sampler_state
{
	texture = g_HoriBlurTexture;
minfilter = linear;
magfilter = linear;
AddressU = clamp;
AddressV = clamp;
AddressW = clamp;
SRGBTexture = true;
};
texture			g_VerBlurTexture;
sampler      VerticalBlurSampler = sampler_state
{
	texture = g_VerBlurTexture;
minfilter = linear;
magfilter = linear;
AddressU = clamp;
AddressV = clamp;
AddressW = clamp;

SRGBTexture = true;
};

texture g_DistortionTexture1;
sampler      DistortionSampler1 = sampler_state
{
	texture = g_DistortionTexture1;
minfilter = linear;
magfilter = linear;

AddressU = clamp;
AddressV = clamp;
AddressW = clamp;

SRGBTexture = true;
};
texture g_DistortionTexture2;
sampler      DistortionSampler2 = sampler_state
{
	texture = g_DistortionTexture2;
minfilter = linear;
magfilter = linear;

AddressU = clamp;
AddressV = clamp;
AddressW = clamp;

SRGBTexture = true;
};

texture g_DistortionAlbedoTexture;
sampler      DistortionAlbedoSampler = sampler_state
{
	texture = g_DistortionAlbedoTexture;
minfilter = linear;
magfilter = linear;

AddressU = clamp;
AddressV = clamp;
AddressW = clamp;

SRGBTexture = true;
};


#pragma endregion

static const float g_fdx9MiddleGray = 0.18f;

static const float g_fdx9WhiteCutoff = 0.9f;

static const float g_fLuminance = 0.08f;

float g_fDeltaTime;

//static const float g_fMiddleGray = 0.72f;
//
//static const float g_fLum_white = 1.5f;

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
//float texscaler =1.f/12800.f;
static const float texscaler = 0.0001f;
static const float texOffset = 0.0;
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


struct PS_IN
{
	float2			vTexUV : TEXCOORD0;
};
struct PS_OUT
{
	vector			vColor : COLOR0;
};
struct PS_OUTUI
{
	vector			vColor : COLOR0;
};
float3 rgb_to_hsv(float3 RGB)
{
	float r = RGB.x;
	float g = RGB.y;
	float b = RGB.z;

	float minChannel = min(r, min(g, b));
	float maxChannel = max(r, max(g, b));

	float h = 0;
	float s = 0;
	float v = maxChannel;

	float delta = maxChannel - minChannel;

	if (delta != 0)
	{
		s = delta / v;

		if (r == v) h = (g - b) / delta;
		else if (g == v) h = 2 + (b - r) / delta;
		else if (b == v) h = 4 + (r - g) / delta;
	}
	return float3(h, s, v);
	//vector p = (RGB.g < RGB.b) ? vector(RGB.bg, -1., 2. / 3.) : vector(RGB.gb, 0., -1. / 3.);
	//vector q = (RGB.r < p.x) ? vector(p.xyw, RGB.r) : vector(RGB.r, p.yzx);
	//float c = q.x - min(q.w, q.y);
	//float h = abs((q.w - q.y) / (6. * c + EPSILON) + q.z);
	//
	//float3 hcv = float3(h, c, q.x);
	//float s = hcv.y / (hcv.z + EPSILON);
	//return float3(hcv.x, s, hcv.z);
}
float rgb_to_v(float3 RGB)
{
	float maxChannel = max(RGB.x, max(RGB.y, RGB.z));

	return maxChannel;
}

#pragma region Main


PS_OUT		PS_MAIN_HDR(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;
	vector vDistort1 = tex2D(DistortionSampler1, In.vTexUV);
	vector vDistort2 = tex2D(DistortionSampler2, In.vTexUV);
	vector vDistortAlbedo = tex2D(DistortionAlbedoSampler, In.vTexUV);
	float2 vUV = In.vTexUV;
	//float3 vNoise = 1.f;
	if(vDistort1.a > 0.f)
	{
		vUV.xy += vDistort1.rg*0.01f * vDistort1.a;
		//vNoise*= vDistort1.rgb*0.01f;
	}	
	if (vDistort2.a > 0.f)
	{
		//vUV = In.vTexUV;
		vUV.xy += vDistort2.rg*0.01f * vDistort2.a;
		//vNoise *= vDistort2.rgb*0.01f;
	}
	vector	vUITexture = tex2D(UISampler, vUV);
	vector	vShade = tex2D(ShadeSampler, vUV);
	vector	vSpecular = tex2D(SpecularSampler, vUV);
	vector vAlbedo = tex2D(AlbedoSampler, vUV);
	vector    vEmissive = tex2D(EmissiveSampler, vUV);
	vector    vNoBlurEmissive = tex2D(NoBlurEmissiveSampler, vUV);
	
	//if (vDistortAlbedo.a> 0)
	//	vEmissive = vDistortAlbedo;
	vector    vShadow = tex2D(ShadowSampler, vUV);


	Out.vColor.rgb = (/*vAlbedo**/( vShade.rgb + vSpecular.rgb) * vShadow.rgb + vEmissive.rgb+ vNoBlurEmissive.rgb)/** vNoise*/;
	//Out.vColor.rgb = vSpecular.rgb;
	//Out.vColor.rgb = vShade.rgb  /*+ vEmissive.rgb + vNoBlurEmissive.rgb*//** vNoise*/;
	Out.vColor.a = 1.f;

	return Out;
}


PS_OUT		PS_MAIN_TONEDOWN(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;
	vector	vDownTexture = tex2D(DownSampler, In.vTexUV);
	vector	vUITexture = pow(abs(tex2D(UISampler, In.vTexUV)),1/2.2);
	Out.vColor = vDownTexture;

	float3 Color = Out.vColor.rgb * g_fdx9MiddleGray / (g_fLuminance + 0.001f);

	Color *= (1.f + (Color / (g_fdx9WhiteCutoff * g_fdx9WhiteCutoff)));

	Color /= (1.f + Color);
	Out.vColor.rgb = pow(abs(Color), 1.f / 2.2f).rgb;
	Out.vColor.a = 1.f;

	vector vDestColor = Out.vColor;
	vector vSrcColor = vUITexture;

	//	Out.vColor = (1 - vSrcColor.a)*vDestColor + vSrcColor.a*vSrcColor;
	Out.vColor = (1 - saturate(vSrcColor.a))*vDestColor + vSrcColor * vSrcColor;


	return Out;

}

#pragma endregion

PS_OUT		PS_MAIN_FADEIN(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;
	vector	vFinal = tex2D(DownSampler, In.vTexUV);
	vector	vUITexture = tex2D(UISampler, In.vTexUV);

	Out.vColor = vFinal;
	/*vector vDestColor = Out.vColor;
	vector vSrcColor = vUITexture;

	Out.vColor = (1 - vSrcColor.a)*vDestColor + vSrcColor.a*vSrcColor;*/
	float3 fadecolor = 0.f;

	fadecolor += g_fDeltaTime;

	//Out.vColor.rgb = fadecolor.r > max(Out.vColor.b, max(Out.vColor.r, Out.vColor.g)) ? Out.vColor.rgb : fadecolor;
	Out.vColor.rgb = fadecolor.r > Out.vColor.r ? Out.vColor.rgb : fadecolor;
	float3 Color = Out.vColor.rgb * g_fdx9MiddleGray / (g_fLuminance + 0.001f);

	Color *= (1.f + (Color / (g_fdx9WhiteCutoff * g_fdx9WhiteCutoff)));
	
	Color /= (1.f + Color);

	
	
	Out.vColor.rgb = pow(abs(Color), 1.f / 2.2f).rgb ;
	Out.vColor.a = 1.f;



	return Out;

}
PS_OUT		PS_MAIN_FADEOUT(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;
	vector	vFinal = tex2D(DownSampler, In.vTexUV);
	vector	vUITexture = tex2D(UISampler, In.vTexUV);

	Out.vColor = vFinal;
	/*vector vDestColor = Out.vColor;
	vector vSrcColor = vUITexture;

	Out.vColor = (1 - vSrcColor.a)*vDestColor + vSrcColor.a*vSrcColor;*/

	float3 Color = Out.vColor.rgb * g_fdx9MiddleGray / (g_fLuminance + 0.001f);

	Color *= (1.f + (Color / (g_fdx9WhiteCutoff * g_fdx9WhiteCutoff)));

	Color /= (1.f + Color);
	Color -= g_fDeltaTime;
	
	if (max(Color.b, max(Color.r, Color.g)) < 0.f)
		Color.rgb = 0.f;
	Out.vColor.rgb = pow(abs(Color), 1.f / 2.2f).rgb;
	Out.vColor.a = 1.f;



	return Out;

}

#pragma region Blur


struct PS_OUT_BLUR
{
	vector			vColor0: COLOR0;
	vector			vColor1 : COLOR1;
};

PS_OUT_BLUR		PS_MAIN_Horizontal(PS_IN In)
{

	PS_OUT_BLUR			Out = (PS_OUT_BLUR)0;
	//vector	vBlur = tex2D(HorizontalBlurSampler, In.vTexUV);
	float4 color0 = 0.0;
	//float4 color1 = 0.0;
	int i;
	//if (length((tex2D(HorizontalBlurSampler, In.vTexUV)).rgb))
	//{
		for (i = 0; i < 7; i++)
		{

			color0 += tex2D(HorizontalBlurSampler, float2(In.vTexUV.x + gaussFilter[i].y * texscaler,
				In.vTexUV.y))
				* samplesWe[i];
		}
	//}
	//else
	//	color0 = tex2D(HorizontalBlurSampler, In.vTexUV);
	Out.vColor0 = color0;

	return Out;

}

PS_OUT_BLUR		PS_MAIN_Vertical(PS_IN In)
{

	PS_OUT_BLUR			Out = (PS_OUT_BLUR)0;
	//vector	vBlur = tex2D(VerticalBlurSampler, In.vTexUV);
	float4 color1 = 0.0;
	//Out.vColor1 = vBlur;
	//if (length((tex2D(VerticalBlurSampler, In.vTexUV)).rgb))
	//{
		for (int i = 0; i < 7; i++)
		{

			color1 += tex2D(VerticalBlurSampler, float2(In.vTexUV.x,
				In.vTexUV.y + gaussFilter[i].y * texscaler))
				* samplesWe[i];
		}
	//}
	//else
	//	color1 = tex2D(VerticalBlurSampler, In.vTexUV);
	Out.vColor1 = color1;

	return Out;

}


#pragma endregion

PS_OUT_BLUR		PS_MAIN_DownScale(PS_IN In)
{

	PS_OUT_BLUR			Out = (PS_OUT_BLUR)0;
	//vector	vBlur = tex2D(VerticalBlurSampler, In.vTexUV);
	float4 color1 = 0.0;
	//Out.vColor1 = vBlur;

	for (int i = 0; i<7; i++)
	{

		color1 += tex2D(VerticalBlurSampler, float2(In.vTexUV.x + gaussFilter[i].y * texscaler,
			In.vTexUV.y + gaussFilter[i].x * texscaler))
			* samplesWe[i];
	}
	Out.vColor1 = color1;

	return Out;

}

PS_OUTUI		PS_MAIN_LOW(PS_IN In)
{

	PS_OUT			Out = (PS_OUT)0;
	
	vector	vShade = tex2D(ShadeSampler, In.vTexUV);
	vector	vUITexture = tex2D(UISampler, In.vTexUV);
	vector vAlbedo = tex2D(AlbedoSampler, In.vTexUV);
	vector    vEmissive = tex2D(EmissiveSampler, In.vTexUV);
	Out.vColor = vAlbedo + vEmissive;
	Out.vColor.a = 1.f;

	float3 Color = Out.vColor.rgb * g_fdx9MiddleGray / (g_fLuminance + 0.001f);

	Color *= (1.f + (Color / (g_fdx9WhiteCutoff * g_fdx9WhiteCutoff)));

	Color /= (1.f + Color);
	Out.vColor.rgb = pow(abs(Color), 1.f / 2.2f).rgb;

	vector vDestColor = Out.vColor;
	vector vSrcColor = vUITexture;

	Out.vColor = (1 - vSrcColor.a)*vDestColor + vSrcColor.a*vSrcColor;
	return Out;


}
//PS_OUTUI		PS_MAIN_SkyBox(PS_IN In)
//{
//
//	PS_OUT			Out = (PS_OUT)0;
//	vector		vSkyBox = tex2D(SkyBoxSampler, In.vTexUV);
//
//	vector	vTexture = vSkyBox;
//
//	float3 Color = vTexture.rgb * g_fdx9MiddleGray / (g_fLuminance + 0.001f);
//
//	Color *= (1.f + (Color / (g_fdx9WhiteCutoff * g_fdx9WhiteCutoff)));
//
//	Color /= (1.f + Color);
//	Out.vColor = float4(pow(abs(Color), 1.f / 2.2f), 1.f);
//	return Out;
//
//
//}
technique Default_Device
{
	pass	//0 default
	{
		zenable = false;
		ZWriteEnable = false;

		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
		SRGBWriteEnable = true;


		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_HDR();
	}
	pass	//1 low
	{
		zenable = false;
	ZWriteEnable = false;

	alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;


	VertexShader = NULL;
	PixelShader = compile ps_3_0 PS_MAIN_LOW();
	}
	pass ToneDown//2 최종 출력
	{
		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = NULL;
	PixelShader = compile ps_3_0 PS_MAIN_TONEDOWN();
	}
		pass Horizontal//3
	{
		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = NULL;
	PixelShader = compile ps_3_0 PS_MAIN_Horizontal();
	}
		pass Vertical//4
	{
		alphablendenable = true;
	srcblend = srcalpha;
	destblend = invsrcalpha;

	SRGBWriteEnable = true;

	VertexShader = NULL;
	PixelShader = compile ps_3_0 PS_MAIN_Vertical();
	}
		pass DownScale//5
	{
		srcblend = srcalpha;		alphablendenable = true;

		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_Vertical();
	}
		pass FadeIn//6
	{
		alphablendenable = true;
		srcblend = srcalpha;		
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_FADEIN();
	}
		pass FadeOut//7
	{
		alphablendenable = true;

		srcblend = srcalpha;		
		destblend = invsrcalpha;

		SRGBWriteEnable = true;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_FADEOUT();
	}



	//pass
	//{
	//	zwriteenable = false;

	//alphablendenable = true;
	//srcblend = srcalpha;
	//destblend = invsrcalpha;


	//VertexShader = NULL;
	//PixelShader = compile ps_3_0 PS_MAIN_SkyBox();

	//}

};
