#include "stdafx.h"
#include "MainCamera.h"
#include "Player.h"
#include "MapStaticObject_Elevator.h"
#include "Map_Stage2Chess.h"

#define CAM_NORMAL_DISTANCE (7.f)
#define CAM_FIGHT_BOSSCAM_DSTANCE			(CAM_NORMAL_DISTANCE*2.5f)
#define CAM_FIGHT_BOSSCAM_DSTANCE_MALKIHEL	(CAM_NORMAL_DISTANCE*2.f)

CMainCamera::CMainCamera(_DEVICE _pDevice)
	: CCamera(_pDevice)
	, m_fDistanceToTarget(0.f)
	, m_bIsFixMouse(true)
	, m_fCameraAngleX(0.f), m_fCameraAngleX_Smooth(0.f)
	, m_fCameraAngleY(0.f), m_fCameraAngleY_Smooth(0.f)
	, m_fSmoothTimeX(0.f), m_fSmoothTimeY(0.f)
	, m_bCinematicOnOff(true)
	, m_eCamMode(eCameraMode::End)
	, m_dwMouseMoveY(0), m_dwMouseMoveX(0)
	, m_fScrollDistance(1.f)
	, m_bScrollFirstStart(true)
	, m_bScrollSecondStart(true)
	, m_bThirdViewStart(true)
	, m_bScrollMoveEye(false)
	, m_bTopViewStart(true)
	, m_bTopViewAt(true)
	, m_bTopviewEye(true)
{
}

CMainCamera::CMainCamera(const CMainCamera & _orther)
	: CCamera(_orther)
	, m_fDistanceToTarget(0.f)
	, m_bIsFixMouse(true)
	, m_fCameraAngleX(0.f), m_fCameraAngleX_Smooth(0.f)
	, m_fCameraAngleY(0.f), m_fCameraAngleY_Smooth(0.f)
	, m_fSmoothTimeX(0.f), m_fSmoothTimeY(0.f)
	, m_bCinematicOnOff(true)
	, m_eCamMode(eCameraMode::End)
	, m_dwMouseMoveY(0), m_dwMouseMoveX(0)
	, m_fScrollDistance(1.f)
	, m_bScrollFirstStart(true)
	, m_bScrollSecondStart(true)
	, m_bThirdViewStart(true)
	, m_bScrollMoveEye(false)
	, m_bTopViewStart(true)
	, m_bTopviewEye(true)
{
	ZeroMemory(m_iPlayerAttackCheck, sizeof(_uint) * 3);
}

HRESULT CMainCamera::Ready_GameObject_Prototype()
{
	if (FAILED(CCamera::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainCamera::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
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
	m_fDistanceToTarget = CAM_NORMAL_DISTANCE;

	if (eCameraMode::End == m_eCamMode)
		m_eCamMode = eCameraMode::ThirdView;

	m_listNextAt.push_back(m_tCameraDesc.vAt);

	return S_OK;
}

_uint CMainCamera::Update_GameObject(const _float & _fDeltaTime)
{
	if ((_uint)ESceneType::Shrine == m_iSceneIdx || (_uint)ESceneType::Stage2 == m_iSceneIdx)
		m_bCinematicOnOff = false;
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART)
		m_eCamMode = eCameraMode::TopView;
	if (!m_bCinematicOnOff)
	{
		CCamera::Update_GameObject(_fDeltaTime);

		_float3 vPos;
		_float3 vLook;
		_float3 vUp;
		_float3 vPlayerLook;
		CTransform* pPlayerTransform;

		if (!m_iCameraEffect)
		{
			switch (m_eCamMode)
			{
			case eCameraMode::ThirdView:
				if (m_bThirdViewStart)
				{
					m_bScrollMoveEye = true;
					m_bScrollFirstStart = true;
					m_bScrollSecondStart = true;
					m_bThirdViewStart = false;
					m_bTopViewStart = true;
				}
				ThirdViewCamMoveMent(_fDeltaTime);
				
				vPos = m_tCameraDesc.vAt;
				pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player")->Get_Component(L"Com_Transform"));
				pPlayerTransform->Get_Info(EInfo::Look, &vPlayerLook);
				D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
				vLook = m_tCameraDesc.vAt - m_tCameraDesc.vEye;
				vLook.y = 0;
				D3DXVec3Normalize(&vLook, &vLook);
				vUp = m_tCameraDesc.vUp;
				D3DXVec3Normalize(&vUp, &vUp);
				Get_Manager()->Set_3DListener(vPos, vLook, vUp, _fDeltaTime);
				break;
			case eCameraMode::TopView:
				if (m_bTopViewStart)
				{
					m_bTopViewStart = false;
					m_bScrollMoveEye = true;
					m_bTopViewAt = true;
					m_bScrollFirstStart = true;
					m_bScrollSecondStart = true;
					m_bThirdViewStart = true;
					m_bTopviewEye = true;
				}
				TopViewCamMoveMent(_fDeltaTime);
				break;
			case eCameraMode::ScrollFirstView:
				if (m_bScrollFirstStart)
				{
					m_bScrollMoveEye = true;
					m_bTopViewStart = true;
					m_bThirdViewStart = true;
					m_bScrollSecondStart = true;
					m_bScrollFirstStart = false;
				}
				ScrollViewCamMoveMent(_fDeltaTime);
				break;
			case eCameraMode::ScrollSecondView:
				if (m_bScrollSecondStart)
				{
					m_bScrollMoveEye = true;
					m_bTopViewStart = true;
					m_bThirdViewStart = true;
					m_bScrollSecondStart = false;
					m_bScrollFirstStart = true;
				}
				ScrollViewCamMoveMent(_fDeltaTime);
				break;
			default:
				switch (m_eCamMode)
				{
				case eCameraMode::End:
					break;
				case eCameraMode::TopView:
					break;
				}
				break;
			}
		}
	}
	Fix_Mouse();
	return 0;
}
//_uint CMainCamera::Update_GameObject(const _float& _fDeltaTime)
//{
//		CCamera::Update_GameObject(_fDeltaTime);
//
//		//CManagement* pManagement = GET_MANAGEMENT;
//
//		//CAMERA_DESC tLightCam = *pManagement->Get_LightCamDesc();
//		//CAMERA_DESC tCamZero;
//		//D3DLIGHT9 Light = *pManagement->Get_Light();
//
//		////CPlayer* pPlayer = dynamic_cast<CPlayer*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
//		////CTransform* pTransform = dynamic_cast<CTransform*> (pPlayer->Get_Component(L"Com_Transform"));
//		//Light.Direction.x = Light.Direction.y = Light.Direction.z = 1.f;
//		//Light.Direction.x = Light.Direction.z = -0.5f;
//		//_float3 vLightDir;
//		//D3DXVec3Normalize(&vLightDir, &(_float3)Light.Direction);
//		////Light.Direction.y = -1.f;
//
//
//		//tLightCam.vAt = m_tCameraDesc.vAt;
//		//tLightCam.vEye = tLightCam.vAt;
//		//tLightCam.vEye += LIGHT_DISTANCE*vLightDir;
//
//		//Light.Direction.x = -Light.Direction.x;
//		//Light.Direction.y = -Light.Direction.y;
//		//Light.Direction.z = -Light.Direction.z;
//		//pManagement->Set_LightInfo(Light, 32.f);
//		//pManagement->Set_LightCamDesc(tLightCam);
//	return _uint();
//}


_uint CMainCamera::LateUpdate_GameObject(const _float& _fDeltaTime)
{
	if (!m_bCinematicOnOff)
		CCamera::LateUpdate_GameObject(_fDeltaTime);

	return 0;
}

//void CMainCamera::Movement(const _float & _fDeltaTime)
//{
//	if( eCameraMode::ThirdView ==  m_eCamMode)
//		
//	else if(eCameraMode::ScrollView == m_eCamMode)
//		
//}



void CMainCamera::MoveAt(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	//if (pPlayer->Get_Targeting())
	//{
	//	m_listNextAt.front() = pPlayer->Get_TargetPos();	//타겟 포스를 가져옴
	//	_float3 vTargetDir = pPlayerTransform->Get_TransformDesc().vPos - pPlayer->Get_TargetPos();

	NULL_CHECK_RETURN(pPlayer, );
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform"));
	NULL_CHECK_RETURN(pPlayerTransform, );
	//pPlayer->
	//플레이어가 락온 상태가 아닐 때
	if(m_listNextAt.size()<2)
	{ 
		m_listNextAt.front() = pPlayerTransform->Get_TransformDesc().vPos;
		if (pPlayer->Get_Targeting())
			m_listNextAt.front() = pPlayer->Get_TargetPos();
		m_listNextAt.front().y += 3.f;

	}
	_float3 vDir;
	_float vDirLength = D3DXVec3Length(&(m_listNextAt.front() - m_tCameraDesc.vAt));

	_float fElevatorSprintSpeed = 1.5f;
	_float fElevatoRunSpeed = 1.f;
	_float fSpeedPow = 1.f;
	_float fAtY = 0.f;
	// 엘리베이터 탈경우 올라갈때 혹은 가만히 있을경우 카메라 무빙 판별하기 위해
	CMapStaticObject_Elevator* pElevator = nullptr;
	if (lstrcmp(pPlayer->Get_ElevatorName().c_str(), L""))
	{
		pElevator = dynamic_cast<CMapStaticObject_Elevator*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, pPlayer->Get_ElevatorName()));
	}

	

	////기존 set하여 카메라 이동
	//memcpy(&m_tCameraDesc.vAt, &m_vNextAt, sizeof(_float3));
	if (m_eCamMode == eCameraMode::ScrollFirstView || m_eCamMode == eCameraMode::ScrollSecondView)
		m_fScrollDistance = 1.7f;
	else
		m_fScrollDistance = 1.f;

	m_fDistanceToTarget = CAM_NORMAL_DISTANCE * m_fScrollDistance;
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_MEETBOSS)
	{
		if(m_iSceneIdx == (_uint)ESceneType::Tutorial)
			m_fDistanceToTarget = CAM_FIGHT_BOSSCAM_DSTANCE * m_fScrollDistance;
		else
			m_fDistanceToTarget = (CAM_NORMAL_DISTANCE*1.5f) * m_fScrollDistance;
	}
	//카메라 AT을 무브시킴

	if (pPlayer->Get_Cheat() || pPlayer->Get_ResetCamera() || pPlayer->Get_OnElevetor())
	{
		memcpy(&m_tCameraDesc.vAt, &m_listNextAt.front(), sizeof(_float3));
	}
	else
	{
			ZeroMemory(m_iPlayerAttackCheck, sizeof(_uint) * 3);
			if (m_listNextAt.size() > 1)
			{
				_float3 vFront = m_listNextAt.front();
				m_listNextAt.clear();
				m_listNextAt.push_back(vFront);	
			}
			if (vDirLength > 0.5f)		//오차 값 이내면 플레이어가 움직여도 카메라가 움직이지 않음
			{
			
				if (vDirLength > 3.f)
				{
					fSpeedPow = min(pow(1.3f, vDirLength),10.f);
					fSpeedPow -= 1.5f;
					fSpeedPow = abs(fSpeedPow);
					m_listNextAt.front().y += 0.5f;
				}
				D3DXVec3Normalize(&vDir, &(m_listNextAt.front() - m_tCameraDesc.vAt));
				if (pPlayer->Get_PlayerState() == 9)	//슬립상태
				{
					memcpy(&m_tCameraDesc.vAt, &m_listNextAt.front(), sizeof(_float3));
				}
				else if (pPlayer->Get_PlayerState() == 0)	//아이들상태
				{
					if (nullptr != pElevator && pElevator->Get_MovingOn() && pPlayer->Get_Elevator())
					{
						m_tCameraDesc.vAt += vDir*_fDeltaTime;	
						m_tCameraDesc.vAt.y += 10.f;//10.f = 엘리베이터 스피드임.
					}
					else
					{
						m_tCameraDesc.vAt += vDir*_fDeltaTime*
							pPlayerTransform->Get_TransformDesc().fSpeedPerSecond*fSpeedPow;
					}
				}
				else if (pPlayer->Get_PlayerState() == 4 ||
					pPlayer->Get_PlayerState() == 5 ||
					pPlayer->Get_PlayerState() == 7 ||
					(pPlayer->Get_PlayerState() >= 29 && pPlayer->Get_PlayerState() <= 33))	//뛰거나 점프 대쉬상태
				{
					m_tCameraDesc.vAt += (vDir*_fDeltaTime*
						pPlayerTransform->Get_TransformDesc().fSpeedPerSecond*2.f) * fElevatorSprintSpeed*fSpeedPow;

					m_tCameraDesc.vAt.y += fAtY;
				}
				else
				{
					//그외
					m_tCameraDesc.vAt += vDir*_fDeltaTime*
						pPlayerTransform->Get_TransformDesc().fSpeedPerSecond*1.7f * fElevatoRunSpeed*fSpeedPow;

					m_tCameraDesc.vAt.y += fAtY;
				}

				//플레이어 상태에 따라 속도조절 필요함.
			}
		//}
	}	
}

void CMainCamera::Key_Input(const _float & _fDeltaTime)
{
	_float4x4		matCameraView;
	D3DXMatrixIdentity(&matCameraView);
	D3DXMatrixInverse(&matCameraView, NULL, &m_tCameraDesc.matView);

	_long dwMouseMove = 0;
	_float fSmoothMove = 0.f;

	if (dwMouseMove = Get_Manager()->Mouse_Move(EAxis::X))
	{
		m_fSmoothTimeY = 0.f;
		m_dwMouseMoveY = dwMouseMove;
		//m_fCameraAngleY_Smooth += dwMouseMove*0.1f;
		m_fCameraAngleY += dwMouseMove*0.1f;
	}

	//if (m_dwMouseMoveY)
	//{
	//	m_fSmoothTimeY += _fDeltaTime;
	//	fSmoothMove = m_dwMouseMoveY * pow(0.1f, m_fSmoothTimeY)*0.1f;
	//	m_fCameraAngleY += fSmoothMove;
	//	if (!(dwMouseMove = (_long)(fSmoothMove*100.f)))
	//	{
	//		m_fSmoothTimeY = 0.f;
	//		m_dwMouseMoveY = 0;
	//	}
	//}

	if (GET_MANAGEMENT->Key_Down(DIK_NUMPAD7))
	{
		m_fScrollDistance += 0.1f;
	}
	if (GET_MANAGEMENT->Key_Down(DIK_NUMPAD8))
	{
		m_fScrollDistance -= 0.1f;
	}
	if (dwMouseMove = Get_Manager()->Mouse_Move(EAxis::Y))
	{
		m_fSmoothTimeX = 0.f;
		m_dwMouseMoveX = dwMouseMove;
		
		//m_fCameraAngleX_Smooth += dwMouseMove*0.1f;
		m_fCameraAngleX += dwMouseMove*0.1f;
		if (m_fCameraAngleX >= 45.f)
			m_fCameraAngleX = 45.f;

		if (m_fCameraAngleX <= -45.f)
			m_fCameraAngleX = -45.f;

	}
	//if (m_dwMouseMoveX)
	//{
	//	m_fSmoothTimeX += _fDeltaTime;
	//	fSmoothMove = m_dwMouseMoveX * pow(0.1f, m_fSmoothTimeX)*0.1f;
	//	m_fCameraAngleX += fSmoothMove;
	//	if (m_fCameraAngleX >= 45.f)
	//		m_fCameraAngleX = 45.f;

	//	if (m_fCameraAngleX <= -45.f)
	//		m_fCameraAngleX = -45.f;
	//	if (!(dwMouseMove = (_long)(fSmoothMove*100.f)))
	//	{
	//		m_fSmoothTimeX = 0.f;
	//		m_dwMouseMoveX = 0;
	//	}
	//}


	//if (m_dwMouseMoveX > 0)
	//{
	//	m_fCameraAngleX += m_dwMouseMoveX * pow(0.5f, m_fSmoothTime);
	//
	//	if (m_fCameraAngleX_Smooth < m_fCameraAngleX)
	//	{
	//		m_fSmoothTime = 0.f;
	//		m_dwMouseMoveX = 0;
	//	}
	//	if (m_fCameraAngleX >= 45.f)
	//		m_fCameraAngleX = 45.f;

	//	if (m_fCameraAngleX <= -45.f)
	//		m_fCameraAngleX = -45.f;
	//}
	//else if (m_dwMouseMoveX < 0)
	//{
	//	m_fCameraAngleX += m_dwMouseMoveX * pow(0.5f, m_fSmoothTime);
	//	if (m_fCameraAngleX_Smooth > m_fCameraAngleX)
	//	{
	//		m_fSmoothTime = 0.f;
	//		m_dwMouseMoveX = 0;
	//	}
	//	if (m_fCameraAngleX >= 45.f)
	//		m_fCameraAngleX = 45.f;

	//	if (m_fCameraAngleX <= -45.f)
	//		m_fCameraAngleX = -45.f;
	//}
}

void CMainCamera::Fix_Mouse()
{
	if (Get_Manager()->Key_Down(DIK_C))
	{
		m_bIsFixMouse = !m_bIsFixMouse;
	}
	if (m_bIsFixMouse)
	{
		POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

		ClientToScreen(g_hWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
	}
}

void CMainCamera::ThirdViewCamMoveMent(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	NULL_CHECK_RETURN(pPlayer, );
	
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform"));
	NULL_CHECK_RETURN(pPlayerTransform, );
	if(!pPlayer->Get_Targeting()&& !pPlayer->Get_Npc_Interacting())
		Key_Input(_fDeltaTime);
	//pPlayer->
	//플레이어가 락온 상태가 아닐 때
	ThirdViewCam_Player(_fDeltaTime);
	MoveAt(_fDeltaTime);

	if (pPlayer->Get_Targeting())
	{
		_float3 vvPlayerLook, vvPlayerRight;
		_float3 vCamLook;
		vCamLook = m_tCameraDesc.vAt - m_tCameraDesc.vEye;
		pPlayerTransform->Get_Info(EInfo::Right, &vvPlayerRight);
		memcpy(&vvPlayerLook, &pPlayerTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
		D3DXVec3Normalize(&vvPlayerLook, &vvPlayerLook);
		D3DXVec3Normalize(&vvPlayerRight, &vvPlayerRight);

		//_float fDot = acos(D3DXVec3Dot(&vvPlayerLook, &vCamLook)) * (180.f / D3DX_PI);

		//_float fDot = acos(D3DXVec3Dot(&vvPlayerLook, &vCamLook))/* * (180.f / D3DX_PI)*/;
		//_float fLeft = D3DXVec3Dot(&-vCamLook, &-vvPlayerRight);
		//_float fRight = D3DXVec3Dot(&-vCamLook, &vvPlayerRight);



		_float fDot = D3DXVec3Dot(&vCamLook, &vvPlayerLook);
		_float fLeft = D3DXVec3Dot(&-vCamLook, &-vvPlayerRight);
		_float fRight = D3DXVec3Dot(&-vCamLook, &vvPlayerRight);
	
		if (fDot != fDot)
			return;
		if (fRight > 0 && fLeft < 0.f)
			fDot *= -1.f;
		else if (fRight < 0 && fLeft > 0.f)
			fDot *= 1.f;

		if (fDot >= 0.5f)
		{
			m_fCameraAngleY += -fDot*_fDeltaTime;
		}


		//_float3 vPos = pPlayer->Get_TargetPos() - pPlayer->Get_TransformDesc().vPos;
		//D3DXVec3Normalize(&vPos, &vPos);
		//vPos *= m_fDistanceToTarget;
		//vPos.y += m_fDistanceToTarget / 2;
		//_float3 vPlayerPos = pPlayer->Get_TransformDesc().vPos + vPos;
		////vEye = m_tPlayerMovingCamera.vAt + vInvvPlayerLook;

		//vEye = vPlayerPos;

	}



	_float3 vPlayerLook;
	//pPlayerTransform->Get_Info(EInfo::Pos, &vPlayerLook);
	memcpy(&vPlayerLook, &pPlayerTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	_float3 vInvvPlayerLook = vPlayerLook * m_fDistanceToTarget;

	_float4x4 matRot;
	D3DXVec3TransformNormal(&vInvvPlayerLook, &vInvvPlayerLook,
		D3DXMatrixRotationAxis(&matRot, &_float3(0.f, 1.f, 0.f), D3DXToRadian(m_fCameraAngleY)));

	_float4x4		matCameraView;
	D3DXMatrixIdentity(&matCameraView);
	D3DXMatrixInverse(&matCameraView, NULL, &m_tCameraDesc.matView);

	D3DXVec3TransformNormal(&vInvvPlayerLook, &vInvvPlayerLook,
		D3DXMatrixRotationAxis(&matRot, (_float3*)(&matCameraView.m[0][0]), D3DXToRadian(m_fCameraAngleX)));

	if (!pPlayer->Get_FallDeath())
	{
		_float3 vEyeDirection, vEye,vPEye;
		vEye = m_tCameraDesc.vAt + vInvvPlayerLook;
		if (pPlayer->Get_Targeting())
		{
			vEye = m_tPlayerMovingCamera.vAt + vInvvPlayerLook;
			vEye.y += 3.f;
		}
			
		vPEye = m_tPlayerMovingCamera.vAt + vInvvPlayerLook;
		if (m_bScrollMoveEye)
		{
			if (m_tCameraDesc.vEye != vEye)
			{
				_float3 vDir = vEye - m_tCameraDesc.vEye;
				_float fLength = D3DXVec3Length(&vDir);
				if (fLength < 0.1f)
				{
					m_bScrollMoveEye = false;
				}
				//D3DXVec3Normalize(&vDir, &vDir);

				m_tCameraDesc.vEye += vDir * 5.f *  _fDeltaTime;
				m_tPlayerMovingCamera.vEye = vPEye;
			}
		}
		else if (!m_bScrollMoveEye)
		{
			m_tCameraDesc.vEye = vEye;
			m_tPlayerMovingCamera.vEye = vPEye;
		}
		
		_float3 vNormal = m_tCameraDesc.vAt - m_tCameraDesc.vEye;
		D3DXVec3Normalize(&vNormal, &vNormal);

		MyRayCast* RayCast = Get_Manager()->CameraRayCast_Update(L"", m_tCameraDesc.vAt, -vNormal, 7.f);

		if (nullptr != RayCast)
		{
			if(2.f < RayCast->fDist)
				vInvvPlayerLook = vPlayerLook * RayCast->fDist;
			else
				vInvvPlayerLook = vPlayerLook * 2.f;

			_float4x4 matRot;
			D3DXVec3TransformNormal(&vInvvPlayerLook, &vInvvPlayerLook,	D3DXMatrixRotationAxis(&matRot, &_float3(0.f, 1.f, 0.f), D3DXToRadian(m_fCameraAngleY)));

			_float4x4 matCameraView;
			D3DXMatrixIdentity(&matCameraView);
			D3DXMatrixInverse(&matCameraView, NULL, &m_tCameraDesc.matView);

			D3DXVec3TransformNormal(&vInvvPlayerLook, &vInvvPlayerLook,	D3DXMatrixRotationAxis(&matRot, (_float3*)(&matCameraView.m[0][0]), D3DXToRadian(m_fCameraAngleX)));

			m_tCameraDesc.vEye = m_tCameraDesc.vAt + (vInvvPlayerLook);

			m_tPlayerMovingCamera.vEye = m_tPlayerMovingCamera.vAt + vInvvPlayerLook;
		}
	}
}		
	

void CMainCamera::ScrollViewCamMoveMent(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	MoveAt(_fDeltaTime);
	ScrollViewCam_Player(_fDeltaTime);
	_float3 vEyeDirection,vEye;
	if(m_eCamMode == eCameraMode::ScrollFirstView)
		vEyeDirection = _float3(15.f, 7.f, 0.f);
	else if (m_eCamMode == eCameraMode::ScrollSecondView)
		vEyeDirection = _float3(6.f, 7.f, -9.f);
	vEye = m_tCameraDesc.vAt + vEyeDirection;
	
	if (!pPlayer->Get_FallDeath())
	{
		if (m_bScrollMoveEye)
		{
			if (m_tCameraDesc.vEye != vEye)
			{
				_float3 vDir = vEye - m_tCameraDesc.vEye;
				_float fLength = D3DXVec3Length(&vDir);
				if (fLength < 0.1f)
				{
					m_bScrollMoveEye = false;
				}
				//D3DXVec3Normalize(&vDir, &vDir);

				m_tCameraDesc.vEye += vDir * 5.f *  _fDeltaTime;
				m_tPlayerMovingCamera.vEye += vDir * 5.f *  _fDeltaTime;
			}
		}
		else if (!m_bScrollMoveEye)
		{
			m_tCameraDesc.vEye = vEye;
			m_tPlayerMovingCamera.vEye = vEye;
		}

	}
}

void CMainCamera::TopViewCamMoveMent(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	CMap_Stage2Chess* pStage2Chess = dynamic_cast<CMap_Stage2Chess*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Proto, L"Map_Stage2Chess"));
	//CMap_Stage2Chess* pChess = dynamic_cast<CMap_Stage2Chess*>(GET_MANAGEMENT->Get_GameObject(ESceneType::))
	_float3 vAt = pStage2Chess->Get_StageMesh(12)->Get_ObjectCenterPos();
	if (m_bTopViewAt)
	{
		if (m_tCameraDesc.vAt != vAt)
		{
			_float3 vDir = vAt - m_tCameraDesc.vAt;
			_float fLength = D3DXVec3Length(&vDir);
			if (fLength <= 0.2f)
			{
				m_bTopViewAt = false;
			}
			D3DXVec3Normalize(&vDir, &vDir);

			m_tCameraDesc.vAt += vDir * 20.f *  _fDeltaTime;
			m_tPlayerMovingCamera.vAt += vDir * 20.f *  _fDeltaTime;
		}
	}
	else if (!m_bTopViewAt)
	{
		m_tCameraDesc.vAt = vAt;
		m_tPlayerMovingCamera.vAt = vAt;
	}
	//MoveAt(_fDeltaTime);
	//ScrollViewCam_Player(_fDeltaTime);
	_float3 vEyeDirection, vEye;
	vEyeDirection = _float3(5.f, 20.f, -25.f);

	vEye = vAt + vEyeDirection;

	//if (!pPlayer->Get_FallDeath())
	//{
		if (m_bTopviewEye)
		{
			if (m_tCameraDesc.vEye != vEye)
			{
				_float3 vDir = vEye - m_tCameraDesc.vEye;
				_float fLength = D3DXVec3Length(&vDir);
				if (fLength <= 0.2f)
				{
					m_bTopviewEye = false;
				}
				//D3DXVec3Normalize(&vDir, &vDir);

				m_tCameraDesc.vEye += vDir * 5.f *  _fDeltaTime;
				m_tPlayerMovingCamera.vEye += vDir * 5.f *  _fDeltaTime;
			}
		}
		else if (!m_bTopviewEye)
		{
			m_tCameraDesc.vEye = vEye;
			m_tPlayerMovingCamera.vEye = vEye;
		}

	//}
}

void CMainCamera::ThirdViewCam_Player(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	NULL_CHECK_RETURN(pPlayer, );

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform"));
	NULL_CHECK_RETURN(pPlayerTransform, );
	memcpy(&m_tPlayerMovingCamera.vAt, &pPlayerTransform->Get_TransformDesc().vPos, sizeof(_float3));
	m_tPlayerMovingCamera.vAt.y += 1.5f;
}

void CMainCamera::ScrollViewCam_Player(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	NULL_CHECK_RETURN(pPlayer, );

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform"));
	NULL_CHECK_RETURN(pPlayerTransform, );
	memcpy(&m_tPlayerMovingCamera.vAt, &pPlayerTransform->Get_TransformDesc().vPos, sizeof(_float3));
	m_tPlayerMovingCamera.vAt.y += 1.5f;
}

CMainCamera * CMainCamera::Create(_DEVICE _pDevice)
{
	CMainCamera* pInstance = new CMainCamera(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Engine::CGameObject * CMainCamera::Clone(const _uint& _iSceneIdx, void *_pArg)
{
	CMainCamera* pClone = new CMainCamera(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

//void CMainCamera::Free()
//{
//	CCamera::Free();
//}
