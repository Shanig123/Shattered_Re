#ifndef _SHADER_TOTALFUCTION_H__
#define _SHADER_TOTALFUCTION_H__

//float Ps_Rim_Light(float NoV)

float	g_RimPower;
float3	g_vRimColor = { 0.03f, 0.05f, 0.05f };

float3 Ps_Rim_Light(float NoV)
{
	return pow(abs(1 - saturate(NoV)), g_RimPower)*g_vRimColor;
}

//float3 Ps_Rim_Light(float NoV)
////float Ps_Rim_Light(float NoV, float LoV)
//{
//	//return smoothstep(1.0f - g_RimWidth, 1.0f, 1 - max(0, NoV));
//	return pow(abs(1 - saturate(NoV)), 13.f)*float3(0.03f, 0.05f, 0.05f);
//	//float Back = max(0, -LoV);
//	//return pow(abs(1 - NoV), g_RimWidth);
//
//	//return smoothstep(1.0f - g_RimWidth, 1.0f, 1 - max(0, NoV));
//}

#endif // !_SHADER_TOTALFUCTION_H__
