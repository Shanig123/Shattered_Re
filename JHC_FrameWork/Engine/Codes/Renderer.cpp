#include "Renderer.h"
#include "GameObject.h"
#include "Management.h"
#include "Scene.h"

//#include "RenderTarget_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
	: m_bDebug(false), m_iFadeEventFlag(0)
	, m_fFadeOutTime(0.f), m_fFadeInTime(0.f), m_fAccTime(0.f)
	, m_pTargetSurface(nullptr), m_pOldTargetSurface(nullptr)
{
}

HRESULT CRenderer::Ready_Renderer(_DEVICE _pDevice)
{
	if (nullptr == _pDevice)
	{
		ERROR_BOX(L"_pDevice is nullptr")
			return E_FAIL;
	}

	m_pDevice = _pDevice;
	Safe_AddRef(m_pDevice);

	FAILED_CHECK_RETURN(CComponent_Manager::Get_Instance()->Add_Component_Prototype(
		0, L"Component_VIBuffer_Screen", CVIBuffer_Screen::Create(m_pDevice))
		, E_FAIL);

	m_mapRenderScreen.emplace(L"HDRBlend", 
		(CVIBuffer_Screen*)CComponent_Manager::Get_Instance()->Clone_Component(0, L"Component_VIBuffer_Screen"));
	
	m_mapRenderScreen.emplace(L"Horizontal_Blur",
		(CVIBuffer_Screen*)CComponent_Manager::Get_Instance()->Clone_Component(0, L"Component_VIBuffer_Screen"));
	m_mapRenderScreen.emplace(L"Vertical_Blur",
		(CVIBuffer_Screen*)CComponent_Manager::Get_Instance()->Clone_Component(0, L"Component_VIBuffer_Screen"));
	m_mapRenderScreen.emplace(L"ToneDown",
		(CVIBuffer_Screen*)CComponent_Manager::Get_Instance()->Clone_Component(0, L"Component_VIBuffer_Screen"));

	FAILED_CHECK_RETURN(m_pDevice->CreateDepthStencilSurface(
		_uint(CManagement::Get_Instance()->Get_Aspect().x*SHADOW_DETAIL),
		_uint(CManagement::Get_Instance()->Get_Aspect().y*SHADOW_DETAIL),
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pTargetSurface,
		NULL),
		E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Add_RenderList(ERenderPriority _eRenderPriority, CGameObject * _pGameObject)
{
	if (ERenderPriority::End <= _eRenderPriority || nullptr == _pGameObject)
	{
		ERROR_BOX(L"Failed To Add_RenderList");
		return E_FAIL;
	}

	m_GameObjects[(_uint)_eRenderPriority].push_back(_pGameObject);
	Safe_AddRef(_pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_RenderSceneList(CScene * _pScene)
{
	if (nullptr == _pScene)
	{
		ERROR_BOX(L"Failed To Add_RenderSceneList");
		return E_FAIL;
	}

	m_Scenes.push_back(_pScene);
	Safe_AddRef(_pScene);

	return S_OK;
}

void CRenderer::Render_Begin(D3DXCOLOR _Color)
{
	if (FAILED(m_pDevice->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 1.f, 0)))
	{
		ERROR_BOX(L"Failed to Clear");
		return;
	}

	if (FAILED(m_pDevice->BeginScene()))
	{
		ERROR_BOX(L"Failed to BeginScene");
		return;
	}
}

void CRenderer::Render_End()
{
	if (FAILED(m_pDevice->EndScene()))
	{
		ERROR_BOX(L"Failed to EndScene");
		return;
	}

	if (FAILED(m_pDevice->Present(nullptr, nullptr, nullptr, nullptr)))
	{
		ERROR_BOX(L"Failed to Present");
		return;
	}
}

HRESULT CRenderer::Render_RenderList(HWND _hWnd)
{
	m_fAccTime += Get_Manager()->Get_DeltaTime(TIMER_MAIN);
	if (m_fAccTime >= 600.f)
		m_fAccTime = 0.f;
#ifndef _AFXDLL

	CRenderTarget_Manager* pRenderTargetManager = CRenderTarget_Manager::Get_Instance();
	
	//if (FAILED(Render_Priority(pRenderTargetManager)))
	//{
	//	ERROR_BOX(L"Failed to Render_Priority");
	//	return E_FAIL;
	//}
	if (FAILED(Render_Deferred(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}
	if (FAILED(Render_Effect_NO_BLUR(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}
	
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★ 주석 해제 해야함★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	if (!CManagement::Get_Instance()->Get_LowGraphicMode())
	{
		if (FAILED(Render_Shadow(pRenderTargetManager)))
		{
			ERROR_BOX(L"Failed to Render_Deferred");
			return E_FAIL;
		}
	}
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★

	//if (FAILED(Render_Effect(pRenderTargetManager)))
	//{
	//	ERROR_BOX(L"Failed to Render_Deferred");
	//	return E_FAIL;
	//}
	if (FAILED(Render_Environment(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}

	if (FAILED(Render_HorizontalBlur(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}

	if (FAILED(Render_VerticalBlur(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}
	if (FAILED(Render_LightAcc(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_LightAcc");
		return E_FAIL;
	}



	if (FAILED(Render_Priority(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Priority");
		return E_FAIL;
	}
	if (FAILED(Render_UI(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_UI");
		return E_FAIL;
	}

	if (FAILED(Render_HDR_Blend(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}

	if (FAILED(Render_ToneDown(pRenderTargetManager)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}



	if (FAILED(Render_Scene()))
	{
		ERROR_BOX(L"Failed to Render_Scene");
		return E_FAIL;
	}


	if (m_bDebug)
	{
		pRenderTargetManager->Render_DebugBuffer(L"MRT_Deferred");
		pRenderTargetManager->Render_DebugBuffer(L"MRT_LightAcc");
		pRenderTargetManager->Render_DebugBuffer(L"MRT_UI");
		pRenderTargetManager->Render_DebugBuffer(L"MRT_Blur");
		pRenderTargetManager->Render_DebugBuffer(L"MRT_Environment");
		pRenderTargetManager->Render_DebugBuffer(L"MRT_Shadow");
		pRenderTargetManager->Render_DebugBuffer(L"MRT_Effect_NoBlur");
		pRenderTargetManager->Render_DebugBuffer(L"MRT_Blend");
	}	
#else
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	if (FAILED(Render_Deferred(nullptr)))
	{
		ERROR_BOX(L"Failed to Render_Deferred");
		return E_FAIL;
	}

	if (FAILED(Render_Scene()))
	{
		ERROR_BOX(L"Failed to Render_Scene");
		return E_FAIL;
	}

	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
#endif // !_AFXDLL


	return S_OK;
}

void CRenderer::FadeEvent(const _uint & _Event_012)
{
	if (_Event_012 > 2)
		m_iFadeEventFlag = 2;
	m_iFadeEventFlag = _Event_012;
}

HRESULT CRenderer::Render_Priority(class CRenderTarget_Manager* _RenderTargetManager)
{
//#ifndef _AFXDLL
//	_RenderTargetManager->Begin_MRT(L"MRT_SkyBox");
//#endif // !_AFXDLL
//
	if (FAILED(m_pDevice->Clear(0, nullptr,
		D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f), 1.f, 0)))
	{
		ERROR_BOX(L"Failed to Clear");
		return E_FAIL;
	}
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	for (auto& pObj : m_GameObjects[(_uint)ERenderPriority::Priority])
	{
		if (FAILED(pObj->Render_GameObject()))
		{
			ERROR_BOX(L"Failed to Render_Priority");
			return E_FAIL;
		}

		Safe_Release(pObj);
	}

	m_GameObjects[(_uint)ERenderPriority::Priority].clear();

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//#ifndef _AFXDLL
//	_RenderTargetManager->End_MRT(L"MRT_SkyBox");
//#endif // !_AFXDLL

	return S_OK;
}

HRESULT CRenderer::Render_NoAlpha()
{
	for (auto& pObj : m_GameObjects[(_uint)ERenderPriority::NoAlpha])
	{
		if (FAILED(pObj->Render_GameObject()))
		{
			ERROR_BOX(L"Failed to Render_NoAlpha");
			return E_FAIL;
		}

		Safe_Release(pObj);
	}

	m_GameObjects[(_uint)ERenderPriority::NoAlpha].clear();

	return S_OK;
}

_bool Compare_Z(CGameObject* pDest, CGameObject* pSrc)
{
	return pDest->Get_ViewZ() > pSrc->Get_ViewZ();
}

HRESULT CRenderer::Render_Alpha()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_GameObjects[(_uint)ERenderPriority::Alpha].sort(Compare_Z);

	for (auto& pGameObject : m_GameObjects[(_uint)ERenderPriority::Alpha])
	{
		if (FAILED(pGameObject->Render_GameObject()))
		{
			ERROR_BOX(L"Failed to Render_GameObject");
			return E_FAIL;
		}

		Safe_Release(pGameObject);
	}

	m_GameObjects[(_uint)ERenderPriority::Alpha].clear();
#pragma region 알파 블랜딩


#pragma endregion

	return S_OK;
}

HRESULT CRenderer::Render_UI(class CRenderTarget_Manager* _RenderTargetManager,HWND _hWnd)
{
#ifndef _AFXDLL
	_RenderTargetManager->Begin_MRT(L"MRT_UI");
#endif // !_AFXDLL

	for (auto& pObj : m_GameObjects[(_uint)ERenderPriority::UIPersFovLH])
	{
		if (FAILED(pObj->Render_GameObject()))
		{
			ERROR_BOX(L"Failed to Render_GameObject");
			return E_FAIL;
		}

		Safe_Release(pObj);
	}

	m_GameObjects[(_uint)ERenderPriority::UIPersFovLH].clear();

	_float4x4 matViewTemp, matProjTemp;
	_float4x4 matView, matProj;
	matView = {
		1.f,0.f,0.f,0.f,
		0.f,1.f,0.f,0.f,
		0.f,0.f,1.f,0.f,
		0.f,0.f,0.f,1.f
	};
	matProj = matView;
	////뷰와 투영행렬 가져옴.
	m_pDevice->GetTransform(D3DTS_VIEW, &matViewTemp);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProjTemp);
	//D3DXMatrixIsIdentity(&matView);
	//D3DXMatrixIsIdentity(&matProj);

	D3DXMatrixOrthoLH(
		&matProj,
		CManagement::Get_Instance()->Get_Aspect().x,
		CManagement::Get_Instance()->Get_Aspect().y,
		1.f,
		100.f
	);

	////각 스페이스 초기화 밑 직교투영행렬 적용.
	m_pDevice->SetTransform(D3DTS_WORLD, &matView);
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	for (auto& pObj : m_GameObjects[(_uint)ERenderPriority::UIOrtho])
	{
		if (FAILED(pObj->Render_GameObject()))
		{
			ERROR_BOX(L"Failed to Render_GameObject");
			return E_FAIL;
		}

		Safe_Release(pObj);
	}

	m_GameObjects[(_uint)ERenderPriority::UIOrtho].clear();


#ifndef _AFXDLL
	_RenderTargetManager->End_MRT(L"MRT_UI");
#endif // !_AFXDLL

	m_pDevice->SetTransform(D3DTS_VIEW, &matViewTemp);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &matProjTemp);

	return S_OK;
}

HRESULT CRenderer::Render_Scene()
{
	for (auto& pObj : m_Scenes)
	{
		if (FAILED(pObj->Render_Scene()))
		{
			ERROR_BOX(L"Failed to Render_GameObject");
			return E_FAIL;
		}

		Safe_Release(pObj);
	}

	m_Scenes.clear();

	return S_OK;
}

HRESULT CRenderer::Render_Environment(CRenderTarget_Manager * _RenderTargetManager)
{
#ifndef _AFXDLL
	_RenderTargetManager->Begin_MRT(L"MRT_Environment");
#endif // !_AFXDLL

	for (auto& pObj : m_GameObjects[(_uint)ERenderPriority::Environment])
	{
		if (FAILED(pObj->Render_GameObject()))
		{
			ERROR_BOX(L"Failed to Render_NoAlpha");
			return E_FAIL;
		}

		Safe_Release(pObj);
	}

	m_GameObjects[(_uint)ERenderPriority::Environment].clear();

#ifndef _AFXDLL
	_RenderTargetManager->End_MRT(L"MRT_Environment");
#endif // !_AFXDLL

	return S_OK;
}

HRESULT CRenderer::Render_Deferred(CRenderTarget_Manager* _RenderTargetManager)
{
#ifndef _AFXDLL
	_RenderTargetManager->Begin_MRT(L"MRT_Deferred");
#endif // !_AFXDLL
	//if (FAILED(Render_Priority(_RenderTargetManager)))
	//{
	//	ERROR_BOX(L"Failed to Render_Priority");
	//	return E_FAIL;
	//}

	if (FAILED(Render_NoAlpha()))
	{
		ERROR_BOX(L"Failed to Render_NoAlpha");
		return E_FAIL;
	}

	if (FAILED(Render_Alpha()))
	{
		ERROR_BOX(L"Failed to Render_Alpha");
		return E_FAIL;
	}

#ifndef _AFXDLL
	_RenderTargetManager->End_MRT(L"MRT_Deferred");
#endif // !_AFXDLL

	return S_OK;
}

HRESULT CRenderer::Render_Shadow(CRenderTarget_Manager * _RenderTargetManager)
{
#ifndef _AFXDLL
	_RenderTargetManager->Begin_MRT(L"MRT_Shadow");

	auto pManagement = GET_MANAGEMENT;
	m_pDevice->GetDepthStencilSurface(&m_pOldTargetSurface);
	m_pDevice->SetDepthStencilSurface(m_pTargetSurface);


	m_pDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), 1.f, 0);

	for (auto& pObj : m_GameObjects[(_uint)ERenderPriority::Shadow])
	{
		if (FAILED(pObj->Shadow_GameObject()))
		{
			ERROR_BOX(L"Failed to Shadow_GameObject");
			return E_FAIL;
		}
		Safe_Release(pObj);
	}

	m_GameObjects[(_uint)ERenderPriority::Shadow].clear();

	_RenderTargetManager->End_MRT(L"MRT_Shadow");

	m_pDevice->SetDepthStencilSurface(m_pOldTargetSurface);
	Safe_Release(m_pOldTargetSurface);

#endif // !_AFXDLL
	return S_OK;
}

HRESULT CRenderer::Render_LightAcc(CRenderTarget_Manager* _RenderTargetManager)
{
#ifndef _AFXDLL
	_RenderTargetManager->Begin_MRT(L"MRT_LightAcc");

	auto pManagement = GET_MANAGEMENT;
	// 빛만 그리기 위한 공간
	CShader* pShader = dynamic_cast<CShader*>(pManagement->Get_Component((_uint)ESceneType::Static, L"Shader_Shade"));

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	// 노말 텍스처를 얻어와서 상수 테이블로 넘겨주자
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Albedo", "g_AlbedoTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Normal", "g_NormalTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Emissive", "g_EmissiveTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_ShadowDepth", "g_ShadowTexture");

	pEffect->Begin(NULL, 0);

	pManagement->Render_Light(pEffect);

	pEffect->End();

	_RenderTargetManager->End_MRT(L"MRT_LightAcc");

	Safe_Release(pEffect);
#endif // !_AFXDLL
	return S_OK;
}

HRESULT CRenderer::Render_Effect_NO_BLUR(CRenderTarget_Manager * _RenderTargetManager)
{
#ifndef _AFXDLL
	_RenderTargetManager->Begin_MRT(L"MRT_Effect_NoBlur");
#endif // !_AFXDLL
	m_GameObjects[(_uint)ERenderPriority::NoBlur].sort(Compare_Z);

	for (auto& pGameObject : m_GameObjects[(_uint)ERenderPriority::NoBlur])
	{
		if (FAILED(pGameObject->Render_GameObject()))
		{
			ERROR_BOX(L"Failed to Render_GameObject");
			return E_FAIL;
		}

		Safe_Release(pGameObject);
	}

	m_GameObjects[(_uint)ERenderPriority::NoBlur].clear();
#ifndef _AFXDLL
	_RenderTargetManager->End_MRT(L"MRT_Effect_NoBlur");
#endif // !_AFXDLL

	return S_OK;
	return S_OK;
}

HRESULT CRenderer::Render_HDR_Blend(CRenderTarget_Manager * _RenderTargetManager)
{
#ifndef _AFXDLL

	_RenderTargetManager->Begin_MRT(L"MRT_Blend");
	CShader* pShader = dynamic_cast<CShader*>(Get_Manager()->Get_Component((_uint)ESceneType::Static, L"Shader_Blend"));

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();
	
	pEffect->SetFloat("g_fDeltaTime", m_fAccTime);
	pEffect->CommitChanges();

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(0);

	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Albedo", "g_AlbedoTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Shade", "g_ShadeTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Specular", "g_SpecularTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Blur_Ver", "g_EmissiveTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Emissive_NonBlur", "g_NoBlurEmissiveTexture");
	//_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_EmissiveEffect", "g_EmissiveTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_UI", "g_UITexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Shadow", "g_ShadowTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Distortion_Environment1", "g_DistortionTexture1");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Distortion_Environment2", "g_DistortionTexture2");
	
	//_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Environment_UV", "g_DistortionUVTexture");
	//_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_SkyBox", "g_SkyBoxTexture");
	//_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Depth", "g_OpacityTexture");
	//_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Environment", "g_EnvironmentTexture");

	m_mapRenderScreen[L"HDRBlend"]->Render_VIBuffer(&pEffect);
	pEffect->EndPass();
	pEffect->End();


	Safe_Release(pEffect);
	_RenderTargetManager->End_MRT(L"MRT_Blend");
#endif // !_AFXDLL
	return S_OK;
}

HRESULT CRenderer::Render_HorizontalBlur(CRenderTarget_Manager * _RenderTargetManager)
{
#ifndef _AFXDLL

	_RenderTargetManager->Begin_MRT(L"MRT_Blur");
	CShader* pShader = dynamic_cast<CShader*>(Get_Manager()->Get_Component((_uint)ESceneType::Static, L"Shader_Blend"));

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(3);

	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Emissive", "g_HoriBlurTexture");

	m_mapRenderScreen[L"Horizontal_Blur"]->Render_VIBuffer(&pEffect);
	pEffect->EndPass();
	pEffect->End();


	Safe_Release(pEffect);
	//_RenderTargetManager->End_MRT(L"MRT_Blur");
#endif // !_AFXDLL
	return S_OK;
}

HRESULT CRenderer::Render_VerticalBlur(CRenderTarget_Manager * _RenderTargetManager)
{
#ifndef _AFXDLL

	//_RenderTargetManager->Begin_MRT(L"MRT_Blur");
	CShader* pShader = dynamic_cast<CShader*>(Get_Manager()->Get_Component((_uint)ESceneType::Static, L"Shader_Blend"));

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	pEffect->Begin(NULL, 0);
	pEffect->BeginPass(4);

	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_Blur_Hori", "g_VerBlurTexture");

	m_mapRenderScreen[L"Vertical_Blur"]->Render_VIBuffer(&pEffect);
	pEffect->EndPass();
	pEffect->End();


	Safe_Release(pEffect);
	_RenderTargetManager->End_MRT(L"MRT_Blur");
#endif // !_AFXDLL
	return S_OK;
}

HRESULT CRenderer::Render_ToneDown(CRenderTarget_Manager * _RenderTargetManager)
{
#ifndef _AFXDLL
	auto pManagement = GET_MANAGEMENT;

	CShader* pShader = dynamic_cast<CShader*>(pManagement->Get_Component((_uint)ESceneType::Static, L"Shader_Blend"));

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();
	pEffect->AddRef();

	pEffect->Begin(NULL, 0);
	
	switch (m_iFadeEventFlag)
	{
	case 0:
		pEffect->BeginPass(2);
		m_fFadeOutTime = 0.f;
		m_fFadeInTime = 0.f;
		break;
	case 1:
		pEffect->BeginPass(6);
		m_fFadeOutTime = 0.f;
		pEffect->SetFloat("g_fDeltaTime", m_fFadeInTime += (Get_Manager()->Get_DeltaTime(TIMER_MAIN)*0.005f));
		pEffect->CommitChanges();
		break;
	case 2:
		pEffect->BeginPass(7);
		m_fFadeInTime = 0.f;
		pEffect->SetFloat("g_fDeltaTime", m_fFadeOutTime += (Get_Manager()->Get_DeltaTime(TIMER_MAIN)*0.5f));
		pEffect->CommitChanges();
		break;
	default:
		Safe_Release(pEffect);
		return S_OK;
	}


	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_BlendHDR", "g_DownTexture");
	_RenderTargetManager->SetUp_OnShader(pEffect, L"Target_UI", "g_UITexture");
	pEffect->CommitChanges();
	
	m_mapRenderScreen[L"ToneDown"]->Render_VIBuffer(&pEffect);
	pEffect->EndPass();
	pEffect->End();


	Safe_Release(pEffect);
#endif // !_AFXDLL
	return S_OK;
}


void CRenderer::Free()
{
	for (_int i = 0; i < (_uint)ERenderPriority::End; ++i)
	{
		for (auto& pObj : m_GameObjects[i])
		{
			Safe_Release(pObj);
		}

		m_GameObjects[i].clear();
	}
	for (auto& Component : m_mapRenderScreen)
		Safe_Release(Component.second);

	Safe_Release(m_pTargetSurface);
	Safe_Release(m_pOldTargetSurface);
	Safe_Release(m_pDevice);
}
