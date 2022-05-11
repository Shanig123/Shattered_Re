#include "stdafx.h"
#include "Loading_Circle_H.h"

CLoading_Circle_H::CLoading_Circle_H(_DEVICE _pDevice)
	: CUIBase(_pDevice), m_bCheckFadeIn(false), m_bCheckInit(false)
{
}

CLoading_Circle_H::CLoading_Circle_H(const CLoading_Circle_H & _rOther)
	: CUIBase(_rOther), m_bCheckFadeIn(false), m_bCheckInit(false)
{
}

HRESULT CLoading_Circle_H::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Circle_H::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CUIBase::Add_Com()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	//_float3 vPos = {};
	//m_pTransform->Get_Info(EInfo::Pos, &vPos);
	//vPos.z = 1.f;
	//m_pTransform->Set_Pos(vPos);
	m_iDissolveId = 1;
	return S_OK;
}

_uint CLoading_Circle_H::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);
	CManagement* pManagement = GET_MANAGEMENT;
	//DissolveControl(_fDeltaTime);

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

	m_pTransform->Rotate(EAxis::Z, D3DXToRadian(5.f), -3.f, _fDeltaTime);
	m_pTransform->Update_Transform();
	return (_uint)EEvent::None;
}

HRESULT CLoading_Circle_H::Render_GameObject()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		return E_FAIL;
	}
	pEffect->SetFloat("g_ftime", m_fDissolveTime*1.f);
	pEffect->SetFloat("g_fDissolveSpeed", 1.f);
	pEffect->SetVector("g_vColor", &_float4(1.f, 1.f, 1.f, 0.f));
	CManagement::Get_Instance()->Set_Texture(0,
		L"Component_Texture_Dissolve", pEffect, "g_DissolveTexture", m_iDissolveId);

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(m_iShaderPass);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

void CLoading_Circle_H::DissolveControl(const _float & _fDeltaTime)
{
	if (!m_bCheckFadeIn)	//페이드인
	{
		m_fDissolveTime += _fDeltaTime;
		m_iShaderPass = 3;

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
	else   if (m_bCheckFadeIn)//페이드인 끝
	{
		m_fDissolveTime = 0.f;

		m_iShaderPass = 0;

	}
}

CLoading_Circle_H * CLoading_Circle_H::Create(_DEVICE _pDevice)
{
	CLoading_Circle_H* pInstance = new CLoading_Circle_H(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLoading_Circle_H::Clone(const _uint& _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CLoading_Circle_H* pClone = new CLoading_Circle_H(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLoading_Circle_H::Free()
{
	//Safe_Release(m_pTexture);
	//Safe_Release(m_pTransform);
	//Safe_Release(m_pVIBuffer);
	//Safe_Release(m_pShader);

	CGameObject::Free();
}
