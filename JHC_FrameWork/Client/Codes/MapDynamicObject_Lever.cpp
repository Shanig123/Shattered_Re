#include "stdafx.h"
#include "MapDynamicObject_Lever.h"
#include "Player.h"
#include "MapStaticObject_Elevator.h"
#include "CinematicCamera_0.h"
CMapDynamicObject_Lever::CMapDynamicObject_Lever(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapDynamicObjectBase(_pDevice, _PrototypeName)
	, m_iCinematicIndex(0)
	, m_bLeverOn(false)
	, m_bAniPlay(false)
	, m_bSound(false)
{
}

CMapDynamicObject_Lever::CMapDynamicObject_Lever(const CMapDynamicObject_Lever & _rOther)
	: CMapDynamicObjectBase(_rOther)
	, m_iCinematicIndex(0)
	, m_bLeverOn(false)
	, m_bAniPlay(false)
	, m_bSound(false)
{
}

HRESULT CMapDynamicObject_Lever::Ready_GameObject_Prototype()
{
	if (FAILED(CMapDynamicObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapDynamicObject_Lever::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CMapDynamicObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}
	m_iShaderPass = 13;
	m_bCheckRimDistance = true;
	m_bAniPlay = false;
	return S_OK;
}

_uint CMapDynamicObject_Lever::Update_GameObject(const _float & _fDeltaTime)
{	
	CMapDynamicObjectBase::Update_GameObject(_fDeltaTime);

	CManagement* pManagement = GET_MANAGEMENT;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	if ((_uint)ESceneType::Tutorial == m_iSceneIdx)
	{
		CMapStaticObject_Elevator* pElevator = dynamic_cast<CMapStaticObject_Elevator*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, L"Elevator_0"));

		if (!m_bLeverOn && !pElevator->Get_MovingOn() && pPlayer->Get_LeverOn())
		{
			if (pElevator->Get_MovingDir())
			{
				// 아래에 있을 경우
				m_iCinematicIndex = 0;
				pElevator->Set_MovingDir(false);
			}
			else
			{
				// 위에 있을 경우
				m_iCinematicIndex = 1;
				pElevator->Set_MovingDir(true);
			}
			pPlayer->Set_LeverOn(false);
			m_bAniPlay = true;
			m_bLeverOn = true;
		}

		if (m_bAniPlay)
			LeverAni_Play(pElevator, m_iCinematicIndex);
	}
	else if ((_uint)ESceneType::Stage2 == m_iSceneIdx)
	{
		if (!m_bLeverOn && pPlayer->Get_LeverOn())
		{
			m_bAniPlay = true;
			m_bLeverOn = true;
		}			
			
		if (m_bAniPlay)
			LeverAni_Play();
	}	

	return m_iEvent;
}

HRESULT CMapDynamicObject_Lever::Render_GameObject()
{
	if (m_bAniPlay)
		m_pMesh->Play_Animationset(m_fDeltaTime);
	else
		m_pMesh->Play_Animationset(0);

	//CMapDynamicObjectBase::Render_GameObject();

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(m_iShaderPass);

	m_pMesh->Render_Meshes(pEffect, 0, nullptr, true);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

void CMapDynamicObject_Lever::LeverAni_Play(CMapStaticObject_Elevator* _pElevatro/* = nullptr*/, _int _CinematicIndex/* = 0*/)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)LeverID::Lever_Start)
	{
		if (!m_bSound)
		{
			pManagement->PlaySounds(L"Environment_Lever.ogg", CSound_Manager::ESoundID::Object, 0.3f);
			m_bSound = true;
		}
		
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_pMesh->Set_Animationset((_uint)LeverID::Lever_End);
		}
	}

	if (m_pMesh->Get_Animationset() == (_uint)LeverID::Lever_End)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			if (nullptr != _pElevatro)
			{
				_pElevatro->Set_MovingOn();
				if (0 <= _CinematicIndex)
				{
					CCinematicCamera_0* pCamera = dynamic_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
					pCamera->Play_NextCinematic(_CinematicIndex);
				}
			}
			else
			{
				if ((_uint)ESceneType::Stage2 == m_iSceneIdx)
				{
					Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_BRIDGE_STAGE2);
					Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_SHORTCUTDOOR_OPEN_STAGE2);
				}					
			}

			m_pMesh->Set_Animationset((_uint)LeverID::Lever_Start);
			m_bAniPlay = false;
			m_bSound = false;
		}
	}
}

CMapDynamicObject_Lever * CMapDynamicObject_Lever::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapDynamicObject_Lever* pInstance = new CMapDynamicObject_Lever(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapDynamicObject_Lever::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CMapDynamicObject_Lever* pClone = new CMapDynamicObject_Lever(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapDynamicObject_Lever::Free()
{
	CMapDynamicObjectBase::Free();
}
