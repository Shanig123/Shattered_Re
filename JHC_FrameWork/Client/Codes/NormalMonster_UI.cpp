#include "stdafx.h"
#include "NormalMonster_UI.h"


CNormalMonster_UI::CNormalMonster_UI(_DEVICE _pDevice)
	: CBaseMonster_UI(_pDevice)
{
}

CNormalMonster_UI::CNormalMonster_UI(const CNormalMonster_UI & _rOther)
	: CBaseMonster_UI(_rOther)
{
}

HRESULT CNormalMonster_UI::Ready_GameObject_Prototype()
{
	if (FAILED(CBaseMonster_UI::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed To Ready_GameObject_Prototype");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CNormalMonster_UI::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	//if (FAILED(CNormalMonster_UI::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	//{
	//	ERROR_BOX(L"Failed to Ready_GameObject_Clone");
	//	return E_FAIL;
	//}

	if (_pArg)
	{
		memcpy(&m_tTransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	}
	Add_Component();
	return S_OK;
}

_uint CNormalMonster_UI::Update_GameObject(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_bDead)
		m_iEvent = (_uint)EEvent::Dead;
	else
		m_iEvent = (_uint)EEvent::None;


	return m_iEvent;
}

_uint CNormalMonster_UI::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CBaseMonster_UI::LateUpdate_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CNormalMonster_UI::Render_GameObject()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}
	
	if (FAILED(CBaseMonster_UI::Render_GameObject()))
	{
		ERROR_BOX(L"Failed to Render_GameObject");
		return E_FAIL;
	}

	return S_OK;
}

CNormalMonster_UI * CNormalMonster_UI::Create(_DEVICE _pDevice)
{
	CNormalMonster_UI* pInstance = new CNormalMonster_UI(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNormalMonster_UI::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CNormalMonster_UI* pInstance = new CNormalMonster_UI(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNormalMonster_UI::Free()
{
	CBaseMonster_UI::Free();
}


HRESULT CNormalMonster_UI::Add_Component()
{
	CBaseMonster_UI::Add_Component();
	return S_OK;
}