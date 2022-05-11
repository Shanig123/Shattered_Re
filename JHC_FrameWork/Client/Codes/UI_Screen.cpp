#include "stdafx.h"
#include "UI_Screen.h"

CUI_Screen::CUI_Screen(_DEVICE _pDevice)
	: CUIBase(_pDevice)
	, m_bCheckFadeIn(false), m_bCheckFadeOut(false), m_bCheckInit(false)
{
}

CUI_Screen::CUI_Screen(const CUI_Screen & _rOther)
	: CUIBase(_rOther)
	, m_bCheckFadeIn(false), m_bCheckFadeOut(false), m_bCheckInit(false)
{
}

HRESULT CUI_Screen::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Screen::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CUIBase::Ready_GameObject_Clone( _iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CUIBase::Add_Com()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

_uint CUI_Screen::Update_GameObject(const _float & _fDeltaTime)
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

HRESULT CUI_Screen::Render_GameObject()
{
	if (m_bDead)
		return (_uint)EEvent::Dead;

	if (m_bCheckFadeIn&& !lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Black"))
	{
		m_bDead = true;
	}
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

	if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Black"))
	{
		pEffect->SetFloat("g_ftime", m_fDissolveTime*0.9f);
		pEffect->SetFloat("g_fDissolveSpeed", 1.f);
		pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 0.f));
	}
	else
	{
		pEffect->SetFloat("g_ftime", m_fDissolveTime);
		pEffect->SetFloat("g_fDissolveSpeed", 1.f);
		

		pEffect->SetVector("g_vColor", &_float4(0.f, 0.1f, 0.3f, pow(2.f, 2.2f)));

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

	Safe_Release(pEffect);
	return S_OK;
}

void CUI_Screen::DissolveControl(const _float & _fDeltaTime)
{
	if (!m_bCheckFadeIn && !m_bCheckFadeOut)	//페이드인
	{
		m_fDissolveTime += _fDeltaTime;
		if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Logo"))
		{
			m_iShaderPass = 6;
		}
		else if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Black"))
		{
			m_iShaderPass = 3;
		}
		else
		{
			m_iShaderPass = 4;
		}
		if (!m_bCheckInit)
		{
			m_fDissolveTime = 0.f;
			m_bCheckInit = true;
		}
		if (m_fDissolveTime > 5.f)
		{
			m_bCheckFadeIn = true;
		}
	}
	else   if(m_bCheckFadeIn && !m_bCheckFadeOut)//페이드인 끝
	{
		m_fDissolveTime = 0.f;
		if (!lstrcmp(m_tUIDesc.szTexName, L"Component_Texture_Logo"))
		{
			m_iShaderPass = 1;
			//m_iShaderPass = 0;
		}
		else
		{
			m_iShaderPass = 0;
		}
	}
	else if (m_bCheckFadeOut)
	{
		m_fDissolveTime += _fDeltaTime;
		m_iShaderPass = 3;
	}
}

CUI_Screen * CUI_Screen::Create(_DEVICE _pDevice)
{
	CUI_Screen* pInstance = new CUI_Screen(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Screen::Clone(const _uint& _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CUI_Screen* pClone = new CUI_Screen(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CUI_Screen::Free()
{
	CUIBase::Free();
}
