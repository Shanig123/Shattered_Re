#include "stdafx.h"
#include "CinematicCamera_0.h"
#include "MainCamera.h"
#include "MapStaticObject_Elevator.h"
#include "JPreto.h"
#include "JMalkhel.h"

CCinematicCamera_0::CCinematicCamera_0(_DEVICE _pDevice)
	: CCamera(_pDevice)
	, m_fDistanceToTarget(7.f)
	, m_fCameraAngleX(0.f)
	, m_fCameraAngleY(0.f)
	, m_fBezierTime(0.f)
	, m_bCinemaEnd(false)
	, m_bPlay(false)
	, m_bEditMode(false)
	, m_iRecIndex(0)
	, m_bRecord(false)
	, m_iLerp(0)
	, m_bPlayerLockon(false), m_bInit(false), m_iSaveCount(0)
	, m_fStaticCameraTimer(0.f)
	, m_bFirstPos(false)
	, m_bFirstCheck(true)
{
	m_vecPosition.clear();
}

CCinematicCamera_0::CCinematicCamera_0(const CCinematicCamera_0 & _orther)
	: CCamera(_orther)
	, m_fDistanceToTarget(_orther.m_fDistanceToTarget)
	, m_fCameraAngleX(_orther.m_fCameraAngleX)
	, m_fCameraAngleY(_orther.m_fCameraAngleY)
	, m_fBezierTime(0.f)
	, m_bCinemaEnd(false)
	, m_bPlay(true)
	, m_bEditMode(false)
	, m_iRecIndex(_orther.m_iRecIndex)
	, m_bRecord(false)
	, m_iLerp(0)
	, m_bPlayerLockon(false), m_bInit(false),m_iSaveCount(0)
	, m_fStaticCameraTimer(0.f)
	, m_bFirstPos(false)
	, m_bFirstCheck(true)
{
	m_vecPosition.clear();
}

HRESULT CCinematicCamera_0::Ready_GameObject_Prototype()
{
	if (FAILED(CCamera::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCinematicCamera_0::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CCamera::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (_pArg)
	{
		memcpy(&m_tCameraDesc, _pArg, sizeof(CAMERA_DESC));
	}
	Load_Record();

	return S_OK;
}

_uint CCinematicCamera_0::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bFirstCheck)
	{
		if (m_iSceneIdx == (_uint)ESceneType::Tutorial)
			m_bPlay = true;
		else
		{
			m_bPlay = false;
			m_bCinemaEnd = true;
		}
		m_bFirstCheck = false;
	}
	if (!m_bCinemaEnd)
	{
		CCamera::Update_GameObject(_fDeltaTime);
		CManagement* pManagement = GET_MANAGEMENT;

		if (pManagement->Key_Down(DIK_ESCAPE))
		{
			if (!m_bCinemaEnd)
			{
				m_bCinemaEnd = true;
				m_bPlay = false;
				m_iRecIndex = 0;
				CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
				NULL_CHECK_RETURN(pCamera, 0);
				pCamera->Set_Cinematic(false);
				m_fStaticCameraTimer = 0.f;
				if (m_iSaveCount == 0)
				{
					pManagement->StopSound(CSound_Manager::ESoundID::BGM);
				}
				m_iSaveCount++;
				Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_AWAKEPLAYER);
			}

		}


		if (m_bPlay)
		{
		
			if (m_iSceneIdx == (_uint)ESceneType::Tutorial)
			{
				CCinematicCamera_0::Play_TutorialCine(_fDeltaTime);
			}
			else if (m_iSceneIdx == (_uint)ESceneType::Stage2)
			{
				CCinematicCamera_0::Play_Stage2Cine(_fDeltaTime);
			}
			else
			{
				return 0;
			}


			
		}
		if ( m_bEditMode)
		{
			Key_Input(_fDeltaTime);
			Movement(_fDeltaTime);
		}

		if (m_bPlayerLockon)
		{
			CTransform*	pPlayerTransformCom = (CTransform*)pManagement->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform");
			_float3 fPlayerPos;
			pPlayerTransformCom->Get_Info(EInfo::Pos, &fPlayerPos);
			fPlayerPos.y += 1.f;
			m_tCameraDesc.vAt = fPlayerPos;
		}
	}
	else
	{
		if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAY_BOSS_CINE) &&
			!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_SKIP_BOSS_CINE))
		{
			Play_NextCinematic(2);
		}

		CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
		memcpy(&m_tCameraDesc, &pCamera->Get_CameraDesc(), sizeof(CAMERA_DESC));
	}
	if (!m_bInit&&m_iSceneIdx == (_uint)ESceneType::Tutorial)
	{
		m_bInit = true;
		CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
		CManagement::Get_Instance()->PlaySounds(L"Cinematic_Tutorial.mp3", CSound_Manager::ESoundID::BGM, BGM_VOLUME);
	}

	if (GET_MANAGEMENT->Key_Down(DIK_6))
		m_bEditMode = !m_bEditMode;
	if (GET_MANAGEMENT->Key_Down(DIK_5))
		m_bCinemaEnd = !m_bCinemaEnd;
	if (GET_MANAGEMENT->Key_Down(DIK_P))
		m_bPlay = !m_bPlay;
	return 0;
}

_uint CCinematicCamera_0::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bCinemaEnd)
	{
		CCamera::LateUpdate_GameObject(_fDeltaTime);
	}
	else
	{
		D3DXMatrixLookAtLH
		(
			&m_tCameraDesc.matView,			// 뷰 행렬 반환
			&m_tCameraDesc.vEye,			// Eye : 카메라의 월드상 위치
			&m_tCameraDesc.vAt,				// At : 카메라가 바라볼 월드상 위치
			&m_tCameraDesc.vUp				// Up : 월드상의 절대축. 카메라의 상태행렬 만들기 위해 필요한 재료
		);

		D3DXMatrixPerspectiveFovLH
		(
			&m_tCameraDesc.matProj,			// 원근투영행렬 반환
			m_tCameraDesc.fFovY,			// FovY : Y축의 Field of View(Y축 시야각)
			m_tCameraDesc.fAspect,			// Aspect : 종횡비
			m_tCameraDesc.fNear,			// Near Z : 관잘자(카메라)와 가장 가까운 면과 Z거리
			m_tCameraDesc.fFar				// Far Z : 관잘자(카메라)와 가장 먼 면과 Z거리
		);
	}

	return 0;
}
//void CCinematicCamera_0::Shaker()
//{
//	if (m_iCameraEffect & CAMEFFECT_CAMSHAKING_QUAKE)
//	{
//		m_bShakingCamInit = false;
//		m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_QUAKE;
//	}
//	else
//	{
//		m_iCameraEffect |= CAMEFFECT_CAMSHAKING_QUAKE;
//		m_bShakingCamInit = true;
//	}
//	if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_QUAKE))
//	{
//		ZeroMemory(&m_vShakingPosInit, sizeof(_float3));
//		m_fShakingTime = 0.f;
//	}
//}
void CCinematicCamera_0::Movement(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"Failed To pManagement Is nullptr In KeyInput");
		return;
	}
	//메모
	_float4x4		matCamWorld;
	D3DXMatrixInverse(&matCamWorld, NULL, &m_tCameraDesc.matView);

	LONG	dwMouseMove = 0;

	if (dwMouseMove = pManagement->Mouse_Move(EAxis::Y))
	{
		_float3	vRight, vLength;
		memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_float3));

		vLength = m_tCameraDesc.vAt - m_tCameraDesc.vEye;

		_float4x4	matRot;
		D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

		D3DXVec3TransformNormal(&vLength, &vLength, &matRot);

		m_tCameraDesc.vAt = m_tCameraDesc.vEye + vLength;

	}
	if (dwMouseMove = pManagement->Mouse_Move(EAxis::X))
	{
		_float3	vUp = _float3(0.f, 1.f, 0.f);
		_float3 vecLength = m_tCameraDesc.vAt - m_tCameraDesc.vEye;

		_float4x4	matRot;
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
		D3DXVec3TransformNormal(&vecLength, &vecLength, &matRot);

		m_tCameraDesc.vAt = m_tCameraDesc.vEye + vecLength;
	}
	
}

void CCinematicCamera_0::Key_Input(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;

	_float4x4		matCameraView;
	D3DXMatrixIdentity(&matCameraView);
	D3DXMatrixInverse(&matCameraView, NULL, &m_tCameraDesc.matView);

	_long dwMouseMove = 0;

	if (pManagement->Key_Pressing(DIK_O))
	{
		_float3		vLook;
		memcpy(&vLook, &matCameraView.m[2][0], sizeof(_float3));

		_float3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 10.f * _fDeltaTime;

		m_tCameraDesc.vEye += vLength;
		m_tCameraDesc.vAt += vLength;
	}

	if (pManagement->Key_Pressing(DIK_K))
	{
		_float3		vLook;
		memcpy(&vLook, &matCameraView.m[2][0], sizeof(_float3));

		_float3		vLength = *D3DXVec3Normalize(&vLook, &vLook) * 10.f * _fDeltaTime;

		m_tCameraDesc.vEye -= vLength;
		m_tCameraDesc.vAt -= vLength;
	}

	if (pManagement->Key_Pressing(DIK_J))
	{
		_float3		vRight;
		memcpy(&vRight, &matCameraView.m[0][0], sizeof(_float3));

		_float3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 10.f * _fDeltaTime;

		m_tCameraDesc.vEye -= vLength;
		m_tCameraDesc.vAt -= vLength;
	}

	if (pManagement->Key_Pressing(DIK_L))
	{
		_float3		vRight;
		memcpy(&vRight, &matCameraView.m[0][0], sizeof(_float3));

		_float3		vLength = *D3DXVec3Normalize(&vRight, &vRight) * 10.f * _fDeltaTime;

		m_tCameraDesc.vEye += vLength;
		m_tCameraDesc.vAt += vLength;
	}

	if (pManagement->Key_Down(DIK_0))
	{
		m_bRecord = !m_bRecord;
	}
	if (pManagement->Key_Down(DIK_9))
	{
		m_bPlay = true;
	}
	if (pManagement->Key_Down(DIK_8))
	{
		Save_Record();
	}
	if (pManagement->Key_Down(DIK_7))
	{
		Load_Record();
	}
	//if (pManagement->Key_Down(DIK_P))
	//{
	//	if (m_iLerp<2)
	//	{
	//		m_iLerp++;
	//	}
	//	else
	//	{
	//		m_iLerp = 0;
	//	}
	//}

	if (m_bRecord)
		Record();

	if (pManagement->Key_Down(DIK_INSERT))
	{
		Delete_Record();
	}
	if (pManagement->Key_Down(DIK_HOME))
	{
		m_bPlayerLockon = !m_bPlayerLockon;
	}
}

void CCinematicCamera_0::Record()
{
	RECORD tRec;

	tRec.vEye = m_tCameraDesc.vEye;
	tRec.vAt = m_tCameraDesc.vAt;
	m_vNextEyePos = m_tCameraDesc.vEye;

	if (D3DXVec3Length(&(m_vNextEyePos-m_vCurEyePos))>5.f)
	{
		m_vecPosition.emplace_back(tRec);
		m_vCurEyePos = m_vNextEyePos;
	}
}

void CCinematicCamera_0::Delete_Record()
{
	m_iRecIndex = 0;
	m_vecPosition.clear();
	m_vecPosition.shrink_to_fit();
}

void CCinematicCamera_0::BezierSpline(const _float & _fDeltaTime)
{
	_float3 vPos = {};
	_float fInterT = 0.0f;        // (1 - fT)
	_float fInterT2 = 0.0f;      // (1 - fT) * (1 - fT), Interpolation T
	_float fInterT3 = 0.0f;

	if (m_iRecIndex < m_vecPosition.size() - 2)
	{
		if (m_fBezierTime < 1.0f)
		{
			fInterT = (1.0f - m_fBezierTime);
			fInterT2 = fInterT * fInterT;

			vPos = m_vecPosition[m_iRecIndex].vEye * fInterT2 +
				m_vecPosition[m_iRecIndex + 1].vEye * 2.0f * m_fBezierTime * fInterT +
				m_vecPosition[m_iRecIndex + 2].vEye * m_fBezierTime*m_fBezierTime;
			m_tCameraDesc.vEye = vPos;

			vPos = m_vecPosition[m_iRecIndex].vAt * fInterT2 +
				m_vecPosition[m_iRecIndex + 1].vAt * 2.0f * m_fBezierTime * fInterT +
				m_vecPosition[m_iRecIndex + 2].vAt * m_fBezierTime*m_fBezierTime;
			m_tCameraDesc.vAt = vPos;

			m_fBezierTime += _fDeltaTime*0.25f;
		}
		else
		{
			m_fBezierTime = 0.f;
			m_iRecIndex+=2;
		}
	}
	else
	{
		Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_AWAKEPLAYER);
		m_bCinemaEnd = true;
		m_bPlay = false;
		m_iRecIndex = 0;
		m_iSaveCount++;
		CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
		NULL_CHECK_RETURN(pCamera, );
		pCamera->Set_Cinematic(false);
	}
}

void CCinematicCamera_0::Play_TutorialCine(const _float & _fDeltaTime)
{
	CJPreto* pPreto = nullptr;
	CMapStaticObject_Elevator* pElevator = nullptr;
	switch (m_iSaveCount)
	{
	case 0:
		BezierSpline(_fDeltaTime);
		break;
	case 1:
		if (m_bFirstPos)
		{
			m_tCameraDesc.vEye = { -349.785431f, 114.118271f, 5.32659054f };
			m_bFirstPos = false;
		}
		pElevator = dynamic_cast<CMapStaticObject_Elevator*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, L"Elevator_0"));
		Get_EventMgr()->Set_Event(EVENT_VOLATILE_CINEMATICSTART);

		m_tCameraDesc.vAt = pElevator->Get_Pos();
		Play_StaticPlayer_Tutorial(_fDeltaTime);
		break;
	case 2:
		if (m_bFirstPos)
		{
			m_tCameraDesc.vEye = { -466.709290f ,190.323975f  ,20.2004566f };
			m_bFirstPos = false;
		}
		if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_QUAKE))
		{
			pPreto = dynamic_cast<CJPreto*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, L"Preto_0"));
			_float3 vPos = dynamic_cast<CTransform*>(pPreto->Get_Component(L"Com_Transform"))->Get_TransformDesc().vPos;
			vPos.y += 10.f;
			m_tCameraDesc.vAt = vPos;
		}
		Play_StaticPlayer_Tutorial(_fDeltaTime);
		break;
	}
}

void CCinematicCamera_0::Play_Stage2Cine(const _float & _fDeltaTime)
{

	CJMalkhel* pMalkhel = dynamic_cast<CJMalkhel*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, L"Malkhel_0"));
	_float3 vPos = dynamic_cast<CTransform*>(pMalkhel->Get_Component(L"Com_Transform"))->Get_TransformDesc().vPos;
	switch (m_iSaveCount)
	{
	case 0:
		if (m_bFirstPos)
		{
			m_tCameraDesc.vEye = { 59.4334030f ,31.1765289f ,553.320923f };
			m_bFirstPos = false;
		}
		vPos.y +=5.f;
		m_tCameraDesc.vAt = vPos;
		break;
	case 1:
		vPos = dynamic_cast<CTransform*>(pMalkhel->Get_Component(L"Com_Transform"))->Get_TransformDesc().vPos;
		if (m_bFirstPos)
		{
			m_tCameraDesc.vEye = { 56.3751259f ,37.8846893f ,559.155579f };
			m_bFirstPos = false;
		}
		m_tCameraDesc.vAt = vPos;
		break;
	case 2:
		if (m_bFirstPos)
		{
			m_tCameraDesc.vEye = { 58.7011490f, 32.3104706f, 566.022888f };
			m_bFirstPos = false;
		}
		vPos = dynamic_cast<CTransform*>(pMalkhel->Get_Component(L"Com_Transform"))->Get_TransformDesc().vPos;
		vPos.y += 5.f;
		m_tCameraDesc.vAt = vPos;
		break;
	case 3:
		if (m_bFirstPos)
		{
			m_tCameraDesc.vEye = { 42.3982773f, 26.6347885f, 584.648865f };
			m_tCameraDesc.vAt = { 37.8984261f ,30.7042809f ,593.218567f };
			m_bFirstPos = false;
		}
		break;
	case 4:
		if (m_bFirstPos)
		{
			m_tCameraDesc.vEye = { -38.4863930f, 103.210320f,705.355896f };
			m_tCameraDesc.vAt = { -38.8883018f ,103.771896f ,712.321960f };
			m_bFirstPos = false;
		}
		break;
	default:
		m_bPlay = false;
		m_bCinemaEnd = true;
		break;
	}
}

void CCinematicCamera_0::Play_StaticPlayer_Tutorial(const _float & _fDeltaTime)
{
	m_fStaticCameraTimer += _fDeltaTime;
	if (m_fStaticCameraTimer < 5.f)
	{
		m_bCinemaEnd = false;
		m_bPlay = true;
		CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
		NULL_CHECK_RETURN(pCamera, );
		pCamera->Set_Cinematic(true);
		if (m_iSaveCount == 1)
		{
			Get_EventMgr()->Set_Event(EVENT_VOLATILE_CINEMATICSTART);
			GET_MANAGEMENT->StopMonsterSound();
		}
	}
	else
	{
		m_bCinemaEnd = true;
		m_bPlay = false;
		CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
		NULL_CHECK_RETURN(pCamera, );
		pCamera->Set_Cinematic(false);
		m_fStaticCameraTimer = 0.f;
		if (m_iSaveCount == 1)
		{
			Get_EventMgr()->Set_Event(EVENT_VOLATILE_CINEMATICEND);
		}
		if (m_iSaveCount==2)
		{
			CJPreto* pPreto = dynamic_cast<CJPreto*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, L"Preto_0"));
			_float3 vPos =	dynamic_cast<CTransform*>(pPreto->Get_Component(L"Com_Transform"))->Get_TransformDesc().vPos;
			vPos.y += 10.f;
			pCamera->Set_At(vPos);
			//Get_EventMgr()->Add_Event(EVENT_VOLATILE_END_BOSS_CINE | EVENT_VOLATILE_SKIP_BOSS_CINE);
		}
		m_iSaveCount++;
	}
}

void CCinematicCamera_0::Chase_Target(const _float3 & _vTargetPos, const _float & _fSpeed, const _float & _fDeltaTime)
{
	_float3	vDir = _vTargetPos - m_tCameraDesc.vEye;
	m_tCameraDesc.vEye += *D3DXVec3Normalize(&vDir, &vDir) * _fSpeed * _fDeltaTime;
}

HRESULT CCinematicCamera_0::Save_Record()
{	
	//RECORD tRec;
	//tRec.vEye = { 0.500000000f,3.70000005f,-5.79999971f };
	//tRec.vAt = { -6.50000000f,3.70000005f,-5.80000019f };
	
	//m_vecPosition.emplace_back(tRec);
	//m_vecPosition.emplace_back(tRec);
	//m_vecPosition.emplace_back(tRec);


	if (0 != m_vecPosition.size())
	{
		_tchar szBuffer[MAX_PATH] = L"";
		wsprintf(szBuffer, L"../../Data/CinematicCamera_%d_Data_%d.dat", m_iSceneIdx, m_iSaveCount);
		HANDLE hFile = CreateFile(szBuffer, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			ERROR_BOX(L"File Save Failed");
			return E_FAIL;
		}
		DWORD dwByte = 0;
		size_t size = m_vecPosition.size();


		//HEAD
		WriteFile(hFile, &size, sizeof(size_t), &dwByte, nullptr);

		for (size_t i = 0; i < size; ++i)
			WriteFile(hFile, &m_vecPosition[i], sizeof(RECORD), &dwByte, nullptr);

		++m_iSaveCount;
		CloseHandle(hFile);
		if (!m_vecPosition.empty())
		{
			m_vecPosition.clear();
			m_vecPosition.shrink_to_fit();
		}
	}
	
	return S_OK;
}

HRESULT CCinematicCamera_0::Load_Record()
{
	if (!m_vecPosition.empty())
	{
		m_vecPosition.clear();
		m_vecPosition.shrink_to_fit();
	}
	HANDLE hFile = CreateFile(L"../../Data/CinematicCamera_4_Data_0.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD dwByte = 0;
	RECORD tRecord;
	size_t iSize = 0;
	ZeroMemory(&tRecord, sizeof(RECORD));
	

	//HEAD
	ReadFile(hFile, &iSize, sizeof(size_t), &dwByte, nullptr);
	m_vecPosition.reserve(iSize);
	while (true)
	{
		tRecord.vAt = { 0.f, 0.f, 0.f };
		tRecord.vEye = { 0.f, 0.f, 0.f};
		ReadFile(hFile, &tRecord, sizeof(RECORD), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}
		m_vecPosition.emplace_back(tRecord);
	}
	CloseHandle(hFile);
	m_bPlay = true;
	return S_OK;
}



CCinematicCamera_0 * CCinematicCamera_0::Create(_DEVICE _pDevice)
{
	CCinematicCamera_0* pInstance = new CCinematicCamera_0(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCinematicCamera_0::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CCinematicCamera_0* pClone = new CCinematicCamera_0(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCinematicCamera_0::Free()
{
	CCamera::Free();
}