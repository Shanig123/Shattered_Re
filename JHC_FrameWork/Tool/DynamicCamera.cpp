#include "stdafx.h"
#include "DynamicCamera.h"



CDynamicCamera::CDynamicCamera(_DEVICE _pDevcie)
	: CCamera(_pDevcie)
	, m_bFirst(false)
{
}

CDynamicCamera::CDynamicCamera(const CDynamicCamera & _rOther)
	: CCamera(_rOther)
	, m_bFirst(false)
{
}

HRESULT CDynamicCamera::Ready_GameObject_Prototype()
{
	if (FAILED(CCamera::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed To Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDynamicCamera::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CCamera::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed To Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component")
			return E_FAIL;
	}

	return S_OK;
}

_uint CDynamicCamera::Update_GameObject(const _float& _fDeltaTime)
{
	CCamera::Update_GameObject(_fDeltaTime);

	Key_Input(_fDeltaTime);

	return (_uint)EEvent::None;
}

_uint CDynamicCamera::LateUpdate_GameObject(const _float& _fDeltaTime)
{
	CCamera::LateUpdate_GameObject(_fDeltaTime);

	if (!m_bFirst)
	{
		memcpy(&m_vOldPos, &m_tCameraDesc.matView.m[3][0], sizeof(_float3));
		
		m_fSpeed = 5.f;
		m_bFirst = true;
	}

	return (_uint)EEvent::None;
}

HRESULT CDynamicCamera::Render_GameObject()
{
	return S_OK;
}

HRESULT CDynamicCamera::Add_Component()
{
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Calculator",
		L"Com_Calculator",
		EComponentType::Dynamic,
		(CComponent**)&m_pCalculator
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}



CDynamicCamera * CDynamicCamera::Create(_DEVICE _pDevice)
{
	CDynamicCamera* pInstance = new CDynamicCamera(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDynamicCamera::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CDynamicCamera* pClone = new CDynamicCamera(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed To pClone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CDynamicCamera::Free()
{
	CCamera::Free();
}
