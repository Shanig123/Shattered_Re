#ifndef _SHADER_VALUE_H__
#define _SHADER_VALUE_H__

matrix			g_matWorld;	 // 상수 테이블
matrix			g_matView;
matrix			g_matProj;
matrix			g_matLightView;
matrix			g_matLightProj;

vector			g_vLightDiffuse;
vector			g_vLightDir;
vector			g_vCamPos;

vector			g_vColor;
float g_ftime;
float g_fDissolveSpeed;
float g_fDissolve_per;



float4x3 g_MatrixPalette[70];
int g_numBonInfluences;


#endif // !_SHADER_VALUE_H__
