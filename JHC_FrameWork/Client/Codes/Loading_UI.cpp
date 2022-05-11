#include "stdafx.h"
#include "Loading_UI.h"

CLoading_UI::CLoading_UI(_DEVICE _pDevice)
	: CUIBase(_pDevice)
{
}

CLoading_UI::CLoading_UI(const CLoading_UI & _rOther)
	: CUIBase(_rOther)
{
}

HRESULT CLoading_UI::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_UI::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg/* = nullptr*/)
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
	return S_OK;
}

_uint CLoading_UI::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	return (_uint)EEvent::None;
}

_uint CLoading_UI::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(_fDeltaTime);

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

HRESULT CLoading_UI::Render_GameObject()
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

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(0);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (FAILED(CGameObject::Render_GameObject()))
	{
		ERROR_BOX(L"Failed to Render_GameObject");
		return E_FAIL;
	}

	return S_OK;
}

CLoading_UI * CLoading_UI::Create(_DEVICE _pDevice)
{
	CLoading_UI* pInstance = new CLoading_UI(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLoading_UI::Clone(const _uint& _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CLoading_UI* pClone = new CLoading_UI(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLoading_UI::Free()
{
	CGameObject::Free();
}
