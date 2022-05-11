#include "Management.h"
#include "EventManager.h"

USING(Engine)


#ifndef _AFXDLL


HRESULT CManagement::Ready_Shader(_DEVICE _pDevice, EBuildSetting _eBuildSetting)
{
	D3DVIEWPORT9 tViewPort;

	_pDevice->GetViewport(&tViewPort);

	_float2 vTargetSize = m_pGraphic_Device->Get_Aspect()*0.2f;
	//	tViewPort.Width *= 2;
	//	tViewPort.Height *= 2;


#pragma region SkyBox
	//if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_SkyBox", _pDevice,
	//	tViewPort.Width, tViewPort.Height,
	//	D3DFMT_A16B16G16R16F, ARGB(255, 153, 217, 234))))
	//{
	//	ERROR_BOX(L"Failed to Target_SkyBox");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_SkyBox",
	//	m_pGraphic_Device->Get_Aspect().x - vTargetSize.x,
	//	m_pGraphic_Device->Get_Aspect().y - vTargetSize.y*2.f,
	//		vTargetSize.x, vTargetSize.y)))
	//{
	//	ERROR_BOX(L"Failed to Target_SkyBox");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_SkyBox", L"Target_SkyBox")))
	//{
	//	ERROR_BOX(L"Failed to Ready_MRT");
	//	return E_FAIL;
	//}
#pragma endregion

#pragma region MRT_Deferred

	//Albedo->Emissive->Normal->Depth

#pragma region Albedo
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Albedo", _pDevice,
		tViewPort.Width, tViewPort.Height,
		D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Albedo");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Albedo", 0.f, 0.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Albedo");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Deferred", L"Target_Albedo")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}
#pragma endregion
#pragma region Emissive
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Emissive", _pDevice,
		tViewPort.Width, tViewPort.Height,
		D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Emissive");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Emissive", 0.f, vTargetSize.y*3.f,
		vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Emissive");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Deferred", L"Target_Emissive")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

	//if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_EmissiveEffect", _pDevice,
	//	tViewPort.Width, tViewPort.Height,
	//	D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	//{
	//	ERROR_BOX(L"Failed to Target_EmissiveEffect");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_EmissiveEffect", 
	//	vTargetSize.x*1.5f, vTargetSize.y*3.f,
	//	vTargetSize.x*1.5f, vTargetSize.y*1.5f)))
	//{
	//	ERROR_BOX(L"Failed to Target_EmissiveEffect");
	//	return E_FAIL;
	//}


	//if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Effect", L"Target_EmissiveEffect")))
	//{
	//	ERROR_BOX(L"Failed to Ready_MRT");
	//	return E_FAIL;
	//}
#pragma endregion
#pragma region Normal
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Normal", _pDevice,
		tViewPort.Width, tViewPort.Height,
		D3DFMT_A16B16G16R16F, ARGB(255, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Normal");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Normal", 0.f, vTargetSize.y,
		vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Normal");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Deferred", L"Target_Normal")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}
#pragma endregion

#pragma region Depth
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Depth", _pDevice,
		tViewPort.Width, tViewPort.Height,
		D3DFMT_A32B32G32R32F, ARGB(255, 255, 255, 255))))
	{
		ERROR_BOX(L"Failed to Target_Depth");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Depth", 0.f, vTargetSize.y*2.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Depth");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Deferred", L"Target_Depth")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}
#pragma endregion



#pragma endregion
	
#pragma region MRT_NoBlurEffect
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Emissive_NonBlur", _pDevice,
		tViewPort.Width, tViewPort.Height,
		D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Emissive");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Emissive_NonBlur", 0.f, vTargetSize.y*4.f,
		vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Emissive");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Effect_NoBlur", L"Target_Emissive_NonBlur")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

	//if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_EmissiveEffect", _pDevice,
	//	tViewPort.Width, tViewPort.Height,
	//	D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	//{
	//	ERROR_BOX(L"Failed to Target_EmissiveEffect");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_EmissiveEffect", 
	//	vTargetSize.x*1.5f, vTargetSize.y*3.f,
	//	vTargetSize.x*1.5f, vTargetSize.y*1.5f)))
	//{
	//	ERROR_BOX(L"Failed to Target_EmissiveEffect");
	//	return E_FAIL;
	//}


	//if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Effect", L"Target_EmissiveEffect")))
	//{
	//	ERROR_BOX(L"Failed to Ready_MRT");
	//	return E_FAIL;
	//}
#pragma endregion

#pragma region MRT_Environment

	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Distortion_Environment1", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Distortion_Environment1");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Distortion_Environment1",
		m_pGraphic_Device->Get_Aspect().x - vTargetSize.x, vTargetSize.y*0.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Distortion_Environment1");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Environment", L"Target_Distortion_Environment1")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}


	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Distortion_Environment2", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Distortion_Environment2");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Distortion_Environment2",
		m_pGraphic_Device->Get_Aspect().x - vTargetSize.x, vTargetSize.y*1.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Distortion_Environment2");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Environment", L"Target_Distortion_Environment2")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

	/*if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Distortion_Environment_Albedo", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Distortion_Environment_Albedo");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Distortion_Environment_Albedo",
		m_pGraphic_Device->Get_Aspect().x - vTargetSize.x, vTargetSize.y*2.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Distortion_Environment_Albedo");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Environment", L"Target_Distortion_Environment_Albedo")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}


	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Fog_Environment", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Fog_Environment");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Fog_Environment",
		m_pGraphic_Device->Get_Aspect().x -vTargetSize.x, vTargetSize.y*3.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Fog_Environment");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Environment", L"Target_Fog_Environment")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}*/


	//if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Environment_UV", _pDevice,
	//	tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	//{
	//	ERROR_BOX(L"Failed to Target_Environment_UV");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Environment_UV",
	//	m_pGraphic_Device->Get_Aspect().x - vTargetSize.x, vTargetSize.y*2.f, vTargetSize.x, vTargetSize.y)))
	//{
	//	ERROR_BOX(L"Failed to Target_Environment_UV");
	//	return E_FAIL;
	//}

	//if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Environment", L"Target_Environment_UV")))
	//{
	//	ERROR_BOX(L"Failed to Ready_MRT");
	//	return E_FAIL;
	//}


#pragma endregion
#pragma region MRT_LightAcc
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Shade", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Shade");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Shade",
		vTargetSize.x*2.f, 0.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Shade");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_LightAcc", L"Target_Shade")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Specular", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Specular");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Specular",
		vTargetSize.x*2.f, vTargetSize.y, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Specular");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_LightAcc", L"Target_Specular")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Shadow", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A32B32G32R32F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Shadow");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Shadow",
		vTargetSize.x*2.f, vTargetSize.y*2.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Shadow");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_LightAcc", L"Target_Shadow")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

#pragma endregion
#pragma region MRT_SHADOW
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_ShadowDepth", _pDevice,
		_uint(m_pGraphic_Device->Get_Aspect().x*SHADOW_DETAIL),
		_uint(m_pGraphic_Device->Get_Aspect().y*SHADOW_DETAIL), D3DFMT_A32B32G32R32F, ARGB(255, 255, 255, 255))))
	{
		ERROR_BOX(L"Failed to Target_ShadowDepth");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_ShadowDepth",
		vTargetSize.x, 0.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_ShadowDepth");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Shadow", L"Target_ShadowDepth")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}
#pragma endregion
#pragma region MRT_UI
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_UI", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_UI");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_UI",
		vTargetSize.x*3.f, 0.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_UI");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_UI", L"Target_UI")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

#pragma endregion

#pragma region MRT_Blend
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_BlendHDR", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_BlendHDR");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_BlendHDR",
		vTargetSize.x*3.f, vTargetSize.y, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_BlendHDR");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Blend", L"Target_BlendHDR")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}

#pragma endregion
#pragma region MRT_BLUR
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Blur_Hori", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Blur_Hori");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Blur_Hori",
		vTargetSize.x, vTargetSize.y, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Blur_Hori");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Blur", L"Target_Blur_Hori")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_RenderTarget_Manager(L"Target_Blur_Ver", _pDevice,
		tViewPort.Width, tViewPort.Height, D3DFMT_A16B16G16R16F, ARGB(0, 0, 0, 0))))
	{
		ERROR_BOX(L"Failed to Target_Blur_Ver");
		return E_FAIL;
	}
	if (FAILED(m_pRenderTarget_Manager->Ready_DebugBuffer(L"Target_Blur_Ver",
		vTargetSize.x, vTargetSize.y*2.f, vTargetSize.x, vTargetSize.y)))
	{
		ERROR_BOX(L"Failed to Target_Blur_Ver");
		return E_FAIL;
	}

	if (FAILED(m_pRenderTarget_Manager->Ready_MRT(L"MRT_Blur", L"Target_Blur_Ver")))
	{
		ERROR_BOX(L"Failed to Ready_MRT");
		return E_FAIL;
	}
#pragma endregion



	CShader* pShader = nullptr;

	wstring wstrPath = L"";
	if (EBuildSetting::Client == _eBuildSetting)
		wstrPath = L"../../";
	else
		wstrPath = L"../";


	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_DefaultUI", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_DefaultUI.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Blend", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_Blend.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}


	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Shade", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_Shade.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Player", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_Player.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_General_Dynamic", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_General_Dynamic.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_General_Static", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_General_Static.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_General_Effect", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_General_Effect.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}
	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_General_Environment", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_General_Environment.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}
	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Shadow", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_Shadow.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_SkyBox", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_SkyBox.hpp"))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}
	//if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_General_Effect", CShader::Create(_pDevice, wstrPath + L"Reference/Headers/Shader_General_Effect.hpp"))))
	//{
	//	ERROR_BOX(L"Failed to Add_Component_Prototype");
	//	return E_FAIL;
	//}
	/*pShader = CShader::Create(_pDevice, L"../../Reference/Headers/Shader_Title.hpp");
	if (nullptr == pShader)
	{
	ERROR_BOX(L"Failed to Create");
	return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Title", pShader)))
	{
	ERROR_BOX(L"Failed to Add_Component_Prototype");
	return E_FAIL;
	}

	pShader = CShader::Create(_pDevice, L"../../Reference/Headers/Shader_HPBar.hpp");
	if (nullptr == pShader)
	{
	ERROR_BOX(L"Failed to Create");
	return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_HPBar", pShader)))
	{
	ERROR_BOX(L"Failed to Add_Component_Prototype");
	return E_FAIL;
	}

	pShader = CShader::Create(_pDevice, L"../../Reference/Headers/Shader_Stage.hpp");
	if (nullptr == pShader)
	{
	ERROR_BOX(L"Failed to Create");
	return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Stage", pShader)))
	{
	ERROR_BOX(L"Failed to Add_Component_Prototype");
	return E_FAIL;
	}

	pShader = CShader::Create(_pDevice, L"../../Reference/Headers/Shader_WomanBoss.hpp");
	if (nullptr == pShader)
	{
	ERROR_BOX(L"Failed to Create");
	return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_WomanBoss", pShader)))
	{
	ERROR_BOX(L"Failed to Add_Component_Prototype");
	return E_FAIL;
	}

	pShader = CShader::Create(_pDevice, L"../../Reference/Headers/Shader_Effect.hpp");
	if (nullptr == pShader)
	{
	ERROR_BOX(L"Failed to Create");
	return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Effect", pShader)))
	{
	ERROR_BOX(L"Failed to Add_Component_Prototype");
	return E_FAIL;
	}

	pShader = CShader::Create(_pDevice, L"../../Reference/Headers/Shader_Particle.hpp");
	if (nullptr == pShader)
	{
	ERROR_BOX(L"Failed to Create");
	return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_Particle", pShader)))
	{
	ERROR_BOX(L"Failed to Add_Component_Prototype");
	return E_FAIL;
	}

	pShader = CShader::Create(_pDevice, L"../../Reference/Headers/Shader_SkyBox.hpp");
	if (nullptr == pShader)
	{
	ERROR_BOX(L"Failed to Create");
	return E_FAIL;
	}

	if (FAILED(Add_Component_Prototype((_uint)ESceneType::Static, L"Shader_SkyBox", pShader)))
	{
	ERROR_BOX(L"Failed to Add_Component_Prototype");
	return E_FAIL;
	}*/

	return S_OK;
}

void CManagement::SetUp_OnShader(_EFFECT & _pEffect, const wstring & _pTargetTag, const char * _pConstantTable)
{
	return m_pRenderTarget_Manager->SetUp_OnShader(_pEffect, _pTargetTag, _pConstantTable);
}

#endif // !_AFXDLL
