#include "stdafx.h"
#include "ScreenShotCamera.h"
#include "Player.h"

CScreenShotCamera::CScreenShotCamera(_DEVICE _pDevice)
	: CCamera(_pDevice)
{
}

CScreenShotCamera::CScreenShotCamera(const CScreenShotCamera & _orther)
	: CCamera(_orther)
{
}

HRESULT CScreenShotCamera::Ready_GameObject_Prototype()
{
	if (FAILED(CCamera::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CScreenShotCamera::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CCamera::Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (_pArg)
	{
		memcpy(&m_tCameraDesc, _pArg, sizeof(CAMERA_DESC));
	}

	return S_OK;
}

_uint CScreenShotCamera::Update_GameObject(const _float & _fDeltaTime)
{
	CCamera::Update_GameObject(_fDeltaTime);

	/*if (Get_EventMgr()->Get_CurStaticEvent() & EVENT_SCREENSHOT)
	{
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player")->Get_Component(L"Com_Transform"));

		_float3 vPlyaerPos;
		pPlayerTransform->Get_Info(EInfo::Pos, &vPlyaerPos);
		
		ScreenShotView(vPlyaerPos);
	}*/
		

	return 0;
}

_uint CScreenShotCamera::LateUpdate_GameObject(const _float& _fDeltaTime)
{
	//if (Get_EventMgr()->Get_CurStaticEvent() & EVENT_SCREENSHOT)
	//{
	//	CCamera::LateUpdate_GameObject(_fDeltaTime);
	//}

	return 0;
}

void CScreenShotCamera::ScreenShotView(_float3 _vPos)
{
	

	m_tCameraDesc.vAt = _vPos;
	m_tCameraDesc.vEye = _float3(0.f, 50.f, 0.f);
}

CScreenShotCamera * CScreenShotCamera::Create(_DEVICE _pDevice)
{
	CScreenShotCamera* pInstance = new CScreenShotCamera(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Engine::CGameObject * CScreenShotCamera::Clone(const _uint& _iSceneIdx, void *_pArg)
{
	CScreenShotCamera* pClone = new CScreenShotCamera(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}