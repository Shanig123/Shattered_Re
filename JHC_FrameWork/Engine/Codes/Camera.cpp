#include "Camera.h"
#include "Management.h"

#define LIGHT_DISTANCE	(30.f)
USING(Engine)

CCamera::CCamera(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_fTopCutSceneTime(0.f)
	, m_iCameraEffect(0)
	, m_fShakingTimeDamage(0.f), m_fShakingTimeHeavy(0.f), m_fShakingTime(0.f)
	, m_bShakingCamInit(false), m_bShakingDamageCamInit(false), m_bShakingHeavyCamInit(false), m_bTopCutSceneInit(false)
	, m_vShakingPosInit(_float3()), m_vShakingDamagePosInit(_float3()), m_vShakingHeavyPosInit(_float3())
	, m_vTopCutSceneEyeInit(_float3()), m_vTopCutSceneAtInit(_float3())

{
}

CCamera::CCamera(const CCamera & _orther)
	: CGameObject(_orther)
	, m_fTopCutSceneTime(0.f)
	, m_iCameraEffect(0)
	, m_fShakingTimeDamage(0.f), m_fShakingTimeHeavy(0.f), m_fShakingTime(0.f)
	, m_bShakingCamInit(false), m_bShakingDamageCamInit(false), m_bShakingHeavyCamInit(false), m_bTopCutSceneInit(false)
	, m_vShakingPosInit(_float3()), m_vShakingDamagePosInit(_float3()), m_vShakingHeavyPosInit(_float3())
	, m_vTopCutSceneEyeInit(_float3()), m_vTopCutSceneAtInit(_float3())
{
}

HRESULT CCamera::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (nullptr != _pArg)
	{
		memcpy(&m_tCameraDesc, _pArg, sizeof(CAMERA_DESC));
	}

	return S_OK;
}

_uint CCamera::Update_GameObject(const _float& _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);
	CManagement* pManagement = GET_MANAGEMENT;

	CAMERA_DESC tLightCam = *pManagement->Get_LightCamDesc();
	CAMERA_DESC tCamZero;
	D3DLIGHT9 Light = *pManagement->Get_Light();

	//CPlayer* pPlayer = dynamic_cast<CPlayer*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	//CTransform* pTransform = dynamic_cast<CTransform*> (pPlayer->Get_Component(L"Com_Transform"));
	//Light.Direction.x = Light.Direction.y = Light.Direction.z = 1.f;
	//Light.Direction.x = Light.Direction.z = -0.5f;
	Light.Direction.y = 1.f;
	_float3 vLightDir;
	D3DXVec3Normalize(&vLightDir, &(_float3)Light.Direction);
	//Light.Direction.y = -1.f;


	tLightCam.vAt = m_tCameraDesc.vAt;
	tLightCam.vEye = tLightCam.vAt;
	tLightCam.vEye += LIGHT_DISTANCE*vLightDir;

	//Light.Direction.x = -Light.Direction.x;
	Light.Direction.y = -Light.Direction.y;
	//Light.Direction.z = -Light.Direction.z;
	pManagement->Set_LightInfo(Light, 32.f);
	pManagement->Set_LightCamDesc(tLightCam);

	//if (Get_Manager()->Key_Down(DIK_G))
	//{
	//	//m_bTopCutScene = !m_bTopCutScene;
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

	//if (Get_Manager()->Key_Down(DIK_G))
	//{
	//    //m_bTopCutScene = !m_bTopCutScene;
	//    if (m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE)
	//    {
	//        m_bShakingDamageCamInit = false;
	//        m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_DAMAGE;
	//    }
	//    else
	//    {
	//        m_iCameraEffect |= CAMEFFECT_CAMSHAKING_DAMAGE;
	//        m_bShakingDamageCamInit = true;
	//    }
	//    if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE))
	//    {
	//        ZeroMemory(&m_vShakingDamagePosInit, sizeof(_float3));
	//        m_fShakingTimeDamage = 0.f;
	//    }
	//}

	//if (Get_Manager()->Key_Down(DIK_H))
	//{
	//	if (m_iCameraEffect & CAMEFFECT_TOP_CUTSCENE)
	//	{
	//		m_bTopCutSceneInit = false;
	//		memcpy(&m_tCameraDesc.vAt, m_vTopCutSceneAtInit, sizeof(_float3));
	//		memcpy(&m_tCameraDesc.vEye, m_vTopCutSceneEyeInit, sizeof(_float3));
	//		//ZeroMemory(&m_vTopCutSceneAtInit, sizeof(_float3));
	//		//ZeroMemory(&m_vTopCutSceneEyeInit, sizeof(_float3));
	//		m_iCameraEffect &= ~CAMEFFECT_TOP_CUTSCENE;
	//	}
	//	else
	//	{
	//		m_bTopCutSceneInit = true;
	//		memcpy(&m_vTopCutSceneAtInit, m_tCameraDesc.vAt, sizeof(_float3));
	//		memcpy(&m_vTopCutSceneEyeInit, m_tCameraDesc.vEye, sizeof(_float3));
	//		m_iCameraEffect |= CAMEFFECT_TOP_CUTSCENE;
	//	}
	//	if (!(m_iCameraEffect & CAMEFFECT_TOP_CUTSCENE))
	//	{
	//		m_fTopCutSceneTime = 0.f;
	//	}
	//}
	CameraTopCutScene(_fDeltaTime);
	CameraShaking(_fDeltaTime);
	CameraShakingDamaged(_fDeltaTime);
	CameraShakingHeavy(_fDeltaTime);
	return 0;
}

_uint CCamera::LateUpdate_GameObject(const _float& _fDeltaTime)
{

	D3DXMatrixLookAtLH
	(
		&m_tCameraDesc.matView,			// 뷰 행렬 반환
		&m_tCameraDesc.vEye,			// Eye : 카메라의 월드상 위치
		&m_tCameraDesc.vAt,				// At : 카메라가 바라볼 월드상 위치
		&m_tCameraDesc.vUp				// Up : 월드상의 절대축. 카메라의 상태행렬 만들기 위해 필요한 재료
	);

	if (FAILED(m_pDevice->SetTransform(D3DTS_VIEW, &m_tCameraDesc.matView)))
	{
		ERROR_BOX(L"Failed to SetTransform");
		return E_FAIL;
	}

	D3DXMatrixPerspectiveFovLH
	(
		&m_tCameraDesc.matProj,			// 원근투영행렬 반환
		m_tCameraDesc.fFovY,			// FovY : Y축의 Field of View(Y축 시야각)
		m_tCameraDesc.fAspect,			// Aspect : 종횡비
		m_tCameraDesc.fNear,			// Near Z : 관잘자(카메라)와 가장 가까운 면과 Z거리
		m_tCameraDesc.fFar				// Far Z : 관잘자(카메라)와 가장 먼 면과 Z거리
	);

	if (FAILED(m_pDevice->SetTransform(D3DTS_PROJECTION, &m_tCameraDesc.matProj)))
	{
		ERROR_BOX(L"Failed to SetTransform");
		return E_FAIL;
	}

	return 0;
}

HRESULT CCamera::Render_GameObject()
{

	return S_OK;
}

const CAMERA_DESC & CCamera::Get_CameraDesc() const
{
	return m_tCameraDesc;
}

void CCamera::CamShakerWeak()
{
	//if (m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE)
	//{
	//	m_bShakingDamageCamInit = false;
	//	m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_DAMAGE;
	//}
	//else
	//{
		m_iCameraEffect |= CAMEFFECT_CAMSHAKING_DAMAGE;
		m_bShakingDamageCamInit = true;
		m_fShakingTimeDamage = 0.f;
	//}
	//if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE))
	//{
	//	ZeroMemory(&m_vShakingDamagePosInit, sizeof(_float3));
	//	m_fShakingTimeDamage = 0.f;
	//}
}

void CCamera::CamShakerHeavy()
{
	//if (m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE2)
	//{
	//	m_bShakingHeavyCamInit = false;
	//	m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_DAMAGE2;
	//}
	//else
	//{
	//	m_iCameraEffect |= CAMEFFECT_CAMSHAKING_DAMAGE2;
	//	m_bShakingHeavyCamInit = true;
	//}
	//if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE2))
	//{
	//	ZeroMemory(&m_vShakingDamagePosInit, sizeof(_float3));
	//	m_fShakingTimeHeavy = 0.f;
	//}
	//if(!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE2))
	//{
		m_iCameraEffect |= CAMEFFECT_CAMSHAKING_DAMAGE2;
		m_bShakingHeavyCamInit = true;
		m_fShakingTimeHeavy = 0.f;
	//}
	//if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE2))
	//{
	//	ZeroMemory(&m_vShakingDamagePosInit, sizeof(_float3));
	//	m_fShakingTimeHeavy = 0.f;
	//}
}

void CCamera::CamShakeQuake()
{
	//if (m_iCameraEffect & CAMEFFECT_CAMSHAKING_QUAKE)
	//{
	//	m_bShakingCamInit = false;
	//	m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_QUAKE;
	//}
	//else
	//{
		m_iCameraEffect |= CAMEFFECT_CAMSHAKING_QUAKE;
		m_bShakingCamInit = true;
		m_fShakingTime = 0.f;
	//}
	//if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_QUAKE))
	//{
	//	ZeroMemory(&m_vShakingPosInit, sizeof(_float3));
	//	m_fShakingTime = 0.f;
	//}
}

void CCamera::CameraTopCutScene(const _float & _fDeltaTime)
{
	if (!(m_iCameraEffect & CAMEFFECT_TOP_CUTSCENE))
		return;
	if (m_bTopCutSceneInit)
	{
		//memcpy(&m_vTopCutSceneAtInit, m_tCameraDesc.vAt, sizeof(_float3));
		//memcpy(&m_vTopCutSceneEyeInit, m_tCameraDesc.vEye, sizeof(_float3));
		m_bShakingCamInit = false;
	}

	m_fTopCutSceneTime += _fDeltaTime;
	volatile _float fWave =	sinf(m_fTopCutSceneTime)*powf(0.5f, m_fTopCutSceneTime);


	if (m_fTopCutSceneTime > 10.f)
	{
		m_iCameraEffect &= ~CAMEFFECT_TOP_CUTSCENE;
		m_fTopCutSceneTime = 0.f;
		memcpy(&m_tCameraDesc.vAt, m_vTopCutSceneAtInit, sizeof(_float3));
		memcpy(&m_tCameraDesc.vEye, m_vTopCutSceneEyeInit, sizeof(_float3));
		//ZeroMemory(&m_vTopCutSceneAtInit, sizeof(_float3));
		//ZeroMemory(&m_vTopCutSceneEyeInit, sizeof(_float3));
		return;
	}
	else
	{
		//_float print = fWave;
		//Print_Debug_Float(L"fWave : ", print);
		m_tCameraDesc.vAt.x += fWave;
		m_tCameraDesc.vEye.y += fWave;

	}
}

void CCamera::CameraShaking(const _float & _fDeltaTime)
{
	if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_QUAKE))
		return;
	if (m_bShakingCamInit)
	{
		memcpy(&m_vShakingPosInit, m_tCameraDesc.vAt, sizeof(_float3));
		m_bShakingCamInit = false;
	}
	

	m_fShakingTime += _fDeltaTime*5.f;
	volatile _float fWave1 = sinf(m_fShakingTime*160.f)*powf(0.9f, m_fShakingTime);
	volatile _float fWave2 = sinf(m_fShakingTime*90.f)*powf(0.9f, m_fShakingTime);

	//volatile _float fWave = sinf(m_fShakingTime*20.f)*powf(0.1f, m_fShakingTime);	//피격했을때
	
	if (m_fShakingTime > 6.f)
	{
		m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_QUAKE;
		m_fShakingTime = 0.f;
		return;
	}
	else
	{
		m_tCameraDesc.vAt.x =  m_vShakingPosInit.x+ fWave1;
		m_tCameraDesc.vAt.y = m_vShakingPosInit.y + fWave2;
		//if (m_vShakingPosInit.x + fWave1*2.f > m_vShakingPosInit.x)
		//{
		//	m_tCameraDesc.vAt.y += fWave*2.f;
		//}
		//else
		//{
		//	m_tCameraDesc.vAt.y -= fWave;
		//}
		//m_tCameraDesc.vEye.y += fWave;
	}



}

void CCamera::CameraShakingDamaged(const _float & _fDeltaTime)
{
	if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE))
		return;
	if (m_bShakingDamageCamInit)
	{
		if (m_fShakingTimeDamage <= 0.f)
			memcpy(&m_vShakingDamagePosInit, m_tCameraDesc.vAt, sizeof(_float3));
		m_fShakingTimeDamage = 0.f;
		m_bShakingDamageCamInit = false;
	}


	m_fShakingTimeDamage += _fDeltaTime*5.f;
	volatile _float fWave1 = sinf(m_fShakingTimeDamage*Mersen_ToFloat(10.f, 15.f))*powf(Mersen_ToFloat(0.1f, 0.2f), m_fShakingTimeDamage)*0.05f;
	volatile _float fWave2 = sinf(m_fShakingTimeDamage*Mersen_ToFloat(10.f, 13.f))*powf(Mersen_ToFloat(0.4f, 0.5f), m_fShakingTimeDamage)*0.05f;

	//volatile _float fWave = sinf(m_fShakingTime*20.f)*powf(0.1f, m_fShakingTime);	//피격했을때

	if (m_fShakingTimeDamage > 1.5f)
	{
		m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_DAMAGE;
		m_fShakingTimeDamage = 0.f;
		return;
	}
	else
	{
		m_tCameraDesc.vAt.x = m_vShakingDamagePosInit.x + fWave1;
		m_tCameraDesc.vAt.y = m_vShakingDamagePosInit.y + fWave2;
	}

}

void CCamera::CameraShakingHeavy(const _float & _fDeltaTime)
{
	if (!(m_iCameraEffect & CAMEFFECT_CAMSHAKING_DAMAGE2))
		return;
	if (m_bShakingHeavyCamInit)
	{
		if(m_fShakingTimeHeavy<=0.f)
			memcpy(&m_vShakingHeavyPosInit, m_tCameraDesc.vAt, sizeof(_float3));
		m_fShakingTimeHeavy = 0.f;
		m_bShakingHeavyCamInit = false;
	}


	m_fShakingTimeHeavy += _fDeltaTime*5.f;
	volatile _float fWave1 = sinf(m_fShakingTimeHeavy*Mersen_ToFloat(10.f, 15.f))*powf(Mersen_ToFloat(0.1f, 0.2f), m_fShakingTimeHeavy)*0.5f;
	volatile _float fWave2 = sinf(m_fShakingTimeHeavy*Mersen_ToFloat(10.f, 13.f))*powf(Mersen_ToFloat(0.4f, 0.5f), m_fShakingTimeHeavy)*0.5f;

	if (m_fShakingTimeHeavy > 1.5f)
	{
		m_iCameraEffect &= ~CAMEFFECT_CAMSHAKING_DAMAGE2;
		m_fShakingTimeHeavy = 0.f;
		return;
	}
	else
	{
		m_tCameraDesc.vAt.x = m_vShakingHeavyPosInit.x + fWave1;
		m_tCameraDesc.vAt.y = m_vShakingHeavyPosInit.y + fWave2;
	}
}

void CCamera::Free()
{
	CGameObject::Free();
}
