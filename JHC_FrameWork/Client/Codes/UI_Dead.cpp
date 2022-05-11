#include "stdafx.h"
#include "UI_Dead.h"
#include "Player.h"

CUI_Dead::CUI_Dead(_DEVICE _pDevice)
	: CUIBase(_pDevice)
	, m_bFadeIn(false)
	, m_bFadeOut(false)
	, m_bFadeNormal(false)
{
}

CUI_Dead::CUI_Dead(const CUI_Dead & _rOther)
	: CUIBase(_rOther)
	, m_bFadeIn(false)
	, m_bFadeOut(false)
	, m_bFadeNormal(false)
{
}

HRESULT CUI_Dead::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Dead::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CUIBase::Ready_GameObject_Clone( _iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}
	//L"Component_Texture_Dead"
	if (FAILED(CUIBase::Add_Com()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

_uint CUI_Dead::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);
	DissolveControl(_fDeltaTime);
	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return 0;
	}

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::UIOrtho, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	m_pTransform->Update_Transform();
	return (_uint)EEvent::None;
}

HRESULT CUI_Dead::Render_GameObject()
{
	/*if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Black"))
	{

	}
	else
	{
	pEffect->SetFloat("g_ftime", m_fDissolveTime);
	pEffect->SetFloat("g_fDissolveSpeed", 1.f);


	pEffect->SetVector("g_vColor", &_float4(0.f, 0.1f, 0.3f, pow(2.f, 2.2f)));

	}*/
	if (!m_bFadeIn && !m_bFadeOut && !m_bFadeNormal)
		return S_OK;

	if (m_bDead)
		return (_uint)EEvent::Dead;

	
	
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUIBase::SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		return E_FAIL;
	}
	GET_MANAGEMENT->Set_Texture(0, m_tUIDesc.szTexName, pEffect, "g_BaseTexture");

	if (m_bFadeIn || m_bFadeOut)
	{
		pEffect->SetFloat("g_ftime", m_fDissolveTime);
		pEffect->SetFloat("g_fDissolveSpeed", 1.f);
		pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 0.f));
	}
	

	pEffect->CommitChanges();
	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(m_iShaderPass);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	/*if (m_bFadeNormal)
		Render_Normal(pEffect);*/

	Safe_Release(pEffect);
	return S_OK;
}

void CUI_Dead::DissolveControl(const _float & _fDeltaTime)
{
	CDynamicMesh* pPlayerMesh = dynamic_cast<CDynamicMesh*>(GET_MANAGEMENT->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::DEAD && pPlayerMesh->Get_AnimationFrame(71, 45))
		m_bFadeIn = true;


	// 2	노알파
	// 3	디졸브
	// 4	역디졸브
	if (m_bFadeOut)
	{
		m_fDissolveTime += _fDeltaTime * powf(2.f, m_fDissolveTime);
		m_iShaderPass = 3;

		if (m_fDissolveTime >= 8.f)
		{
			m_fDissolveTime = 0.f;
			m_bFadeNormal = false;
			m_bFadeIn = false;
			m_bFadeOut = false;
		}
	}
	
	
	if (m_bFadeIn)
	{
		m_fDissolveTime += _fDeltaTime * powf(2.f, m_fDissolveTime);
		m_iShaderPass = 4;

		if (m_fDissolveTime >= 10.f)
		{
			m_fDissolveTime = 0.f;
			m_bFadeNormal = true;
			m_bFadeIn = false;
		}
	}
	if (m_bFadeNormal)
	{
		pPlayer->Set_ResetCamera(true);
		m_fDissolveTime += _fDeltaTime;
		m_iShaderPass = 2;
		if (m_fDissolveTime >= 2.f)
		{
			m_fDissolveTime = 0.f;
			m_bFadeNormal = false;
			m_bFadeIn = false;
			m_bFadeOut = true;
			pPlayer->Set_ResetCamera(false);
		}
	}
	


	//if (!m_bCheckFadeIn && !m_bCheckFadeOut)	//페이드인
	//{
	//	m_fDissolveTime += _fDeltaTime;
	//	if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Logo"))
	//	{
	//		m_iShaderPass = 6;
	//	}
	//	else if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Black"))
	//	{
	//		m_iShaderPass = 3;
	//	}
	//	else
	//	{
	//		m_iShaderPass = 4;
	//	}
	//	if (!m_bCheckInit)
	//	{
	//		m_fDissolveTime = 0.f;
	//		m_bCheckInit = true;
	//	}
	//	if (m_fDissolveTime > 5.f)
	//	{
	//		m_bCheckFadeIn = true;
	//	}
	//}
	//else   if(m_bCheckFadeIn && !m_bCheckFadeOut)//페이드인 끝
	//{
	//	m_fDissolveTime = 0.f;
	//	if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Logo"))
	//	{
	//		m_iShaderPass = 1;
	//		//m_iShaderPass = 0;
	//	}
	//	else
	//	{
	//		m_iShaderPass = 0;
	//	}
	//}
	//else if (m_bCheckFadeOut)
	//{
	//	m_fDissolveTime += _fDeltaTime;
	//	m_iShaderPass = 3;
	//}




	if (!m_bFadeIn && !m_bFadeOut && !m_bFadeNormal)
		m_fDissolveTime = 0.f;

}

HRESULT CUI_Dead::Render_Normal(_EFFECT & _pEffect)
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUIBase::SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		return E_FAIL;
	}
	GET_MANAGEMENT->Set_Texture(0, m_tUIDesc.szTexName, pEffect, "g_BaseTexture");

	pEffect->CommitChanges();
	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(m_iShaderPass);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	return S_OK;
}

CUI_Dead * CUI_Dead::Create(_DEVICE _pDevice)
{
	CUI_Dead* pInstance = new CUI_Dead(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Dead::Clone(const _uint& _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CUI_Dead* pClone = new CUI_Dead(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CUI_Dead::Free()
{
	CUIBase::Free();
}
