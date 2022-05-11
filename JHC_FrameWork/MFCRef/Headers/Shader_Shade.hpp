vector		g_vLightDir;
vector		g_vLightDiffuse;
vector		g_vLightAmbient;

vector		g_vMtrlDiffuse = (vector)1.f;
vector		g_vMtrlAmbient = (vector)1.f;

vector		g_vCamPos;
vector		g_vSkyBoxReflect;
float		g_fPower;

matrix     g_matViewInv;
matrix     g_matProjInv;

vector		g_vLightPos;
vector		g_vFogColor;


float		g_fRange;

matrix		g_matView;
matrix		g_matProj;
matrix		g_matLightView;
matrix		g_matLightProj;
matrix		g_matPSM;
matrix		g_matShadowView;


const static float g_fBias = 0.001f;
const static float g_fBias2 = 0.00001f;
const static float g_fOne = 1.f;
const static float g_PI = 3.141592f;

float g_fFogIn, g_fFogOut;

#pragma region Texture



texture			g_NormalTexture;

sampler		NormalSampler = sampler_state
{
	texture = g_NormalTexture;
SRGBTexture = true;
};

texture			g_AlbedoTexture;

sampler		AlbedoSampler = sampler_state
{
	texture = g_AlbedoTexture;
SRGBTexture = true;

};
texture			g_DepthTexture;

sampler		DepthSampler = sampler_state
{
	texture = g_DepthTexture;
	SRGBTexture = true;
//sampletype = clamp;
};



texture			g_EmissiveTexture;

sampler		EmissiveSampler = sampler_state
{
	texture = g_EmissiveTexture;
SRGBTexture = true;
};

texture			g_SpecularTexture;

sampler		SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
SRGBTexture = true;
};

texture			g_ShadowTexture;

sampler		ShadowSampler = sampler_state
{
	texture = g_ShadowTexture;
SRGBTexture = true;
minfilter = linear;
magfilter = linear;
mipfilter = none;
AddressU = clamp;
AddressV = clamp;
};

texture g_SkyBoxTexture;

sampler SkyBoxSampler = sampler_state
{
	texture = g_SkyBoxTexture;
	SRGBTexture = true;
};

texture		g_FogTexture;
sampler		FogSampler = sampler_state
{
	texture = g_FogTexture;
	SRGBTexture = true;
	minfilter = linear;
	magfilter = linear;


};
//texture			g_AlbedoTexture;
//
//sampler		AlbedoSampler = sampler_state
//{
//	texture = g_NormalTexture;
//};

#pragma endregion

struct PS_IN
{
	float2			vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vShade : COLOR0;
	vector		vSpecular : COLOR1;
	vector		vShadow : COLOR2;
};

float3 PS_Oren_Nayer(vector vNormal, vector vInvLightDir, vector vCamEye, float _fRough, vector vAlbedo, float fSpecular, float NoL);
float3 PS_SpecularF(float fSpecular, vector vAlbedo, float NoL);
float3 PS_SpecularS(float fRough,float NoH, float NoL, float NoV, float fSpecular, vector vAlbedo);

float3 PS_DirShadow(float NoL, vector vWorldPos, vector vShadeColor);
float3 PS_Environment(float3 _reflect);
vector Ps_Fog(vector BaseColor, float vViewPosZ, float2 vUV);

PS_OUT			PS_DIROREN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector	vNormal = vector(tex2D(NormalSampler, In.vTexUV).rgb,1.f);
	vector	vAlbedo = tex2D(AlbedoSampler, In.vTexUV);
	float	fSpecular = abs(tex2D(DepthSampler, In.vTexUV).g);
	vector	vDepth = tex2D(DepthSampler, In.vTexUV);
	vector	vCamEye, vInvLightDir;

	float	fRough = length(tex2D(NormalSampler, In.vTexUV).rgb) <= 1.f ? 0 : saturate(abs(length(tex2D(NormalSampler, In.vTexUV).rgb) - 1.f));
	float NoL, NoH, NoV;

	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);
	float		fViewZ = vDepth.z * 1000.f;
	vector		vPosition;

	// 0 ~ 1 -> -1 ~ 1 (텍스처 uv좌표에서 투영 좌표로 변환)
	// z나누기 이전의 투영상태의 z값
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;

	vPosition.z = vDepth.x * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_matProjInv);
	float fViewPosZ = vPosition.z;
	vPosition = mul(vPosition, g_matViewInv);

	vInvLightDir = normalize(-g_vLightDir);
	vCamEye = normalize(g_vCamPos - vPosition);

	NoL = dot(vNormal, vInvLightDir);
	NoH = dot(vNormal, normalize(vCamEye - vInvLightDir));
	NoV = dot(vNormal, vCamEye);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Out.vShade.rgb = PS_Oren_Nayer(vNormal,
		vInvLightDir,
		vCamEye,
		fRough,
		vAlbedo,
		fSpecular,
		NoL);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (fRough <= 0 && fSpecular <= 0)
		Out.vSpecular.rgb = PS_SpecularS(fRough, NoH, NoL, NoV, fSpecular, vAlbedo);
	else if (fRough <= 0 && fSpecular > 0)
		Out.vSpecular.rgb = ((PS_SpecularF(fSpecular, vAlbedo, NoL)*PS_Environment(reflect(g_vLightDir, vNormal).rgb)) / (4 * NoV*NoL));
	else
		Out.vSpecular.rgb = (PS_SpecularS(fRough,NoH,NoL,NoV,fSpecular, vAlbedo)*PS_Environment(reflect(g_vLightDir, vNormal).rgb));

///////////////////////////////////////////////////////////////////////
	//shadow 
	Out.vShadow.rgb = PS_DirShadow(NoL, vPosition, vAlbedo);

///////////////////////////////////////////////////////////////////////
	//Out.vSpecular += Fresnel*g_vLightAmbient;
	//float alphavlu = Out.vShade.a;


	Out.vShade =	Ps_Fog(Out.vShade, fViewPosZ, In.vTexUV);
	Out.vSpecular = Ps_Fog(Out.vSpecular, fViewPosZ, In.vTexUV);
	Out.vShadow = Ps_Fog(Out.vShadow, fViewPosZ, In.vTexUV);



	//Out.vShade.a *= alphavlu;
	//Out.vSpecular.a *= alphavlu;
	//Out.vShadow.a *= alphavlu;
///////////////////////////////////////////////////////////////////////
	//float4 LimColor1 = { -1.0,-1.0,-1.0,1.0 };
	//float4 LimColor2 = { 1.0,0.0,1.0,1.0 };
	//float fDot = Ps_Rim_Light(NoV);
	//float4 ColorRim = (fDot * g_vLightAmbient + (1.0 - fDot) * LimColor1);
	//Out.vShadow += Ps_Rim_Light(dot(vInvLightDir, vNormal));
	//Out.vSpecular += ColorRim;
	//Out.vSpecular += Ps_Rim_Light_Rim(fSpecular,NoV);
	//Out.vSpecular += Ps_Rim_Light_Back(fSpecular,NoV,dot(normalize(vPosition .xyz+vInvLightDir.xyz), normalize(vPosition - g_vCamPos).xyz));
	//Out.vSpecular += Ps_Rim_Light_Rim(fRough, NoV);
	//Out.vShadow += Ps_Rim_Light( NoV);
	return Out;
}

PS_OUT			PS_POINTOREN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vInNormal = pow(abs(tex2D(NormalSampler, In.vTexUV)), 2.2);
	vector	vNormal = vector(vInNormal.rgb, 1.f);
	vector	vAlbedo = tex2D(AlbedoSampler, In.vTexUV);



	vAlbedo.rgb = 1.f;
	vAlbedo.rgb = pow(abs(vAlbedo.rgb), 2.2f);

	// texture uv 좌표를 월드 상태의 -1~1사이로 변환
	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);

	vector		vDepth = tex2D(DepthSampler, In.vTexUV);
	vDepth.a = 1.f;
	float		fViewZ = vDepth.z * 1000.f;
	float	fSpecular = abs(tex2D(DepthSampler, In.vTexUV).g);
	float	fRough = length(tex2D(NormalSampler, In.vTexUV).rgb) <= 1.f ? 0.f : saturate(abs(length(tex2D(NormalSampler, In.vTexUV).rgb) - 1.f));
	float	NoL, NoH, NoV;
	
	vector		vCamEye;
	vector		vPosition;
	vector		vLightDir;
	// 0 ~ 1 -> -1 ~ 1 (텍스처 uv좌표에서 투영 좌표로 변환)
	// z나누기 이전의 투영상태의 z값
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;
	vPosition.z = vDepth.x * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);

	vLightDir = vPosition - g_vLightPos;
	vLightDir = normalize(-vLightDir);
	vCamEye = normalize(g_vCamPos - vPosition);

	 
	NoL = dot(vNormal, vLightDir);
	NoH = dot(vNormal, normalize(vCamEye - vLightDir));
	NoV = dot(vNormal, vCamEye);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Out.vShade.rgb = PS_Oren_Nayer(vNormal,
		vLightDir,
		vCamEye,
		fRough,
		vAlbedo,
		fSpecular,
		NoL);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (fRough <= 0 && fSpecular <= 0)
		Out.vSpecular.rgb = PS_SpecularS(fRough, NoH, NoL, NoV, fSpecular, vAlbedo);
	else if (fRough <= 0 && fSpecular > 0)
		Out.vSpecular.rgb = ((PS_SpecularF(fSpecular, vAlbedo, NoL)*PS_Environment(reflect(g_vSkyBoxReflect, vNormal).rgb)) / (4 * NoV*NoL));
	else
		Out.vSpecular.rgb = (PS_SpecularS(fRough, NoH, NoL, NoV, fSpecular, vAlbedo)*PS_Environment(reflect(g_vSkyBoxReflect, vNormal).rgb));
	///////////////////////////////////////////////////////////////////////
	//shadow 
	Out.vShadow.rgb = PS_DirShadow(NoL, vPosition, vAlbedo);

	///////////////////////////////////////////////////////////////////////
	return Out;
}

PS_OUT			PS_LOW(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	vector	vNormal = vector(tex2D(NormalSampler, In.vTexUV).rgb, 1.f);
	vector	vAlbedo = tex2D(AlbedoSampler, In.vTexUV);
	float	fSpecular = abs(tex2D(DepthSampler, In.vTexUV).g);
	vector	vDepth = tex2D(DepthSampler, In.vTexUV);
	vector	vCamEye, vInvLightDir;

	float	fRough = length(tex2D(NormalSampler, In.vTexUV).rgb) <= 1.f ? 0 : saturate(abs(length(tex2D(NormalSampler, In.vTexUV).rgb) - 1.f));
	float NoL, NoH, NoV;

	vNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);
	float		fViewZ = vDepth.z * 1000.f;
	vector		vPosition;

	// 0 ~ 1 -> -1 ~ 1 (텍스처 uv좌표에서 투영 좌표로 변환)
	// z나누기 이전의 투영상태의 z값
	vPosition.x = (In.vTexUV.x * 2.f - 1.f) * fViewZ;
	vPosition.y = (In.vTexUV.y * -2.f + 1.f) * fViewZ;

	vPosition.z = vDepth.x * fViewZ;
	vPosition.w = fViewZ;

	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);

	vInvLightDir = normalize(-g_vLightDir);
	vCamEye = normalize(g_vCamPos - vPosition);

	NoL = dot(vNormal, vInvLightDir);
	NoH = dot(vNormal, normalize(vCamEye - vInvLightDir));
	NoV = dot(vNormal, vCamEye);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Out.vShade.rgb = PS_Oren_Nayer(vNormal,
		vInvLightDir,
		vCamEye,
		fRough,
		vAlbedo,
		fSpecular,
		NoL);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Out.vSpecular.rgb = vAlbedo;
	///////////////////////////////////////////////////////////////////////
	//shadow 
	Out.vShadow.rgb = vAlbedo;

	///////////////////////////////////////////////////////////////////////
	return Out;
}
float3 PS_Oren_Nayer(vector vNormal, vector vInvLightDir,vector vCamEye , float _fRough,vector vAlbedo,float fSpecular,
	float NoL)
{
	float  VoN, LoN;


	VoN = dot(vCamEye, vNormal);
	LoN = dot(vInvLightDir, vNormal);

	float fGamma = dot(normalize(vCamEye - vNormal*VoN),
		normalize(vInvLightDir - vNormal*LoN));


	float fRough = 0.f;

	//fRough = _fRough > 0 ? _fRough : 0.f;
	fRough = _fRough;

	float fRoughSq = fRough*fRough;

	float fA = 1.0f - 0.5f*(fRoughSq / (fRoughSq + 0.33f));
	//float fA = 1.0f - fRoughSq*(lightIntensity / (fRoughSq + 0.33f));
	float fB = 0.45f*(fRoughSq / (fRoughSq + 0.09f));

	float  alpha = max(acos(LoN), acos(VoN));
	float  beta = min(acos(LoN), acos(VoN));
	float fC = sin(alpha)*tan(beta);

	float fABC = (fA + fB*max(0.f, fGamma)*fC);

	return (vAlbedo.rgb*(max(0.0f, NoL))*fABC)/**g_fPower*/;
}

float3 PS_SpecularF(float fSpecular, vector vAlbedo, float NoL)
{
	/////////////////////////////////////////////////////////////////////// 
	//specular test
	//float3 vSpecular = fSpecular;
	//Out.vShade += vector(lerp(0.08 * vSpecular.xxx, Out.vShade, vSpecular.xxx), 1.f);

	/*
	vector		vReflect = normalize(reflect(normalize(vector(g_vLightDir.xyz, 0.f)), vNormal));
	vector		vLook = normalize(g_vCamPos - vPosition);
	vector		vHalf = normalize(vLook - g_vLightDir);

	Out.vSpecular = pow(max(saturate(dot(vNormal, vHalf)), 0), g_fPower);
	Out.vSpecular = pow(max(saturate(dot(vReflect, vHalf)), 0), g_fPower) ;



	Out.vSpecular = normalize(pow(max(saturate(dot(vReflect, vHalf)), 0), fSpecular));
	*/
	//float  D = ggxNormalDistribution(NdotH, rough);
	//float  G = ggxSchlickMaskingTerm(NdotL, NdotV, rough);
	//float3 F = schlickFresnel(spec, LdotH);

	//// Evaluate the Cook-Torrance Microfacet BRDF model
	////     Cancel NdotL here to avoid catastrophic numerical precision issues.
	//float3 ggxTerm = D*G*F / (4 * NdotV /* * NdotL */);
	//float ggxNormalDistribution(float NdotH, float roughness)
	//{
	//	float a2 = roughness * roughness;
	//	float d = ((NdotH * a2 - NdotH) * NdotH + 1);
	//	return a2 / (d * d * M_PI);
	//}

	//float3 schlickFresnel(float3 f0, float lDotH)
	//{
	//	return f0 + (float3(1.0f, 1.0f, 1.0f) - f0) * pow(1.0f - lDotH, 5.0f);
	//}


	///////////////////////////////////////////////////////////////////////
	//	Out.vShade = Out.vSpecular;


	float3 vSpecular = fSpecular;
	vSpecular = lerp(0.08 * vSpecular.xxx, vAlbedo.rgb, vSpecular.xxx);
	//R0
	return float3(1 - (vSpecular + ((1.f - vSpecular)*pow(1.0f - saturate(NoL), 5.0f))));
}
float3 PS_SpecularS(float fRough, float NoH, float NoL, float NoV, float fSpecular, vector vAlbedo)
{
	float a2 = fRough*fRough;
	//float d = (fRough/(1.0f - NoH*NoH + (NoH*fRough)*(NoH*fRough)))*(fRough / (1.0f - NoH*NoH + (NoH*fRough)*(NoH*fRough)))*(1.0/3.141592f);
	float d = (a2 / ((NoH*a2 - NoH)*NoH + 1)*((NoH*a2 - NoH)*NoH + 1)*g_PI);
	//float n = (a2 - 2)*0.5f;
	//d = (n + 2) / ((2 * 3.141592f)*(pow(max(abs(NoH), 0.000001f), n)));
	//float g = 0.5 / ((NoL*sqrt(NoV*NoV*(1.0 - a2)*a2)) +
	//	(NoV*sqrt(NoL*NoL*(1.0 - a2)*a2)));
	//float g = 0.5*rcp((NoL*(NoV* (1 - sqrt(a2)) + sqrt(a2))) + (NoV*(NoL* (1 - sqrt(a2)) + sqrt(a2))));
	float g = (0.5 / ((NoL*(NoV*(1.0 - fRough) + fRough)) + (NoV*(NoL*(1.0 - fRough) + fRough))));
	float3 f = PS_SpecularF(fSpecular, vAlbedo, NoL);
	//float f0 = lerp(0.08 * fSpecular.xxx, vAlbedo.rgb, fSpecular.xxx);
	//float f = f0 + (float3(1.0f, 1.0f, 1.0f) - f0) * pow(1.0f - dot(vInvLightDir, normalize(vCamEye - g_vLightDir)), 5.0f);
	
	return ((((d*g)*f) / (4 * NoV*NoL))).rgb;
}

float3 PS_DirShadow(float NoL, vector vWorldPos, vector vShadeColor)
{
	
	vector color;
	vector vPos = vWorldPos;
	float2 _uv;
	vPos = mul(vPos, g_matLightView);
	vPos = mul(vPos, g_matLightProj);
	vPos.xyz /= vPos.w;

	_uv = vPos.xy;
	_uv.y = -_uv.y;
	_uv = _uv*0.5 + 0.5;
	
	color = vShadeColor;
	color.a = 1.f;
	if ((saturate(_uv.x) != _uv.x) || (saturate(_uv.y)!= _uv.y))	//빛의 절두체 밖이므로 원래 색을 출력함.
		return color.rgb; 

	float depthValue = tex2D(ShadowSampler, _uv).r;

	if (vPos.z > depthValue + g_fBias)	//뎁스값이 작으면 그림자 출력
		color.rgb *= 0.5f;

	color.a = 1.f;

	return color.rgb;

}

float3 PS_Environment(float3 _reflect)
{
	//return (pow(abs(tex3D(SkyBoxSampler, _reflect).rgb), 2.2f))
	return (pow(abs(tex2D(SkyBoxSampler, _reflect.xz).rgb),2.2f));
}

vector Ps_Fog(vector BaseColor,float vViewPosZ,float2 vUV)
{
	float fFog = (g_fFogOut - vViewPosZ) / (g_fFogOut - g_fFogIn);
	//float fFog = 1.0 / exp(pow(abs(vViewPosZ*0.2f),2.f));
	//Out Fog 
	//return (fFog*BaseColor + (1.f - fFog)*pow(abs(tex2D(FogSampler, vUV)), 2.2));
	//return (fFog*BaseColor + (1.f - fFog)*tex2D(FogSampler, vUV));
//	return (fFog*BaseColor + (1.f - fFog)*vector(0.2f, 0.7843f, 0.6078f, 1.f));
	//return (fFog*BaseColor + (1.f - fFog)*vector(0.2f, 0.2f, 0.2f, 0.2f));
	//return vViewPosZ > 500.f ? (fFog*vector(0.2f, 0.2f, 0.2f, 1.f) + (1.f - fFog)*BaseColor) : (fFog*BaseColor + (1.f - fFog)*vector(0.2f, 0.2f, 0.2f, 1.f));
	//return (fFog*BaseColor*g_vLightAmbient + (1.f - fFog)*(vViewPosZ*0.001f*vector(0.2f, 0.7843f, 0.6078f, 1.f)));
	return (fFog*BaseColor + (1.f - fFog)*(vViewPosZ*0.001f*g_vFogColor));
//	return (fFog*vector(0.2f, 0.2f, 0.2f, 1.f) + (1.f - fFog)*BaseColor);
}
technique Default_Device
{
		pass	directionalOren	// 0
	{
		zwriteenable = false;
	alphablendenable = true;
	srcblend = one;
	destblend = one;
	SRGBWriteEnable = true;
	VertexShader = NULL;
	PixelShader = compile ps_3_0 PS_DIROREN();
	}

		pass	PointLightOren		//1
	{
		zwriteenable = false;
	alphablendenable = true;
	srcblend = one;
	destblend = one;
	SRGBWriteEnable = true;

	VertexShader = NULL;
	PixelShader = compile ps_3_0 PS_POINTOREN();
	}
		pass LowMode	//2
	{
		zwriteenable = false;
	alphablendenable = true;
	srcblend = one;
	destblend = one;
	SRGBWriteEnable = true;

	VertexShader = NULL;
	PixelShader = compile ps_3_0 PS_LOW();
	}
};
