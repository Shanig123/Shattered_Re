#pragma once

#ifndef MAINCAMERA_H
#define MAINCAMERA_H

#include "Camera.h"

USING(Engine)

class CMainCamera : public CCamera
{

private:
	explicit CMainCamera(_DEVICE _pDevice);
	explicit CMainCamera(const CMainCamera& _orther);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;

	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;

	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;

public:
	const eCameraMode& Get_CamMode() const { return m_eCamMode; }
	const tagCameraDesc& Get_PlayerMovingCam() const { return m_tPlayerMovingCamera; }
public:
	void Set_CameraAngleY(_float _fCameraAngleY) { m_fCameraAngleY += _fCameraAngleY; }
	void Set_Cinematic(_bool _bCheckCinematicCamOnOff) { m_bCinematicOnOff = _bCheckCinematicCamOnOff; }
	void Set_CameraMode(eCameraMode _eCamMode) { m_eCamMode = _eCamMode; }

	void Set_Camera_At(_float3 _vCameraAt) { m_tCameraDesc.vAt = _vCameraAt; m_tPlayerMovingCamera.vAt = _vCameraAt; }
	void Set_Camera_Eye(_float3 _vCameraEye) { m_tCameraDesc.vEye = _vCameraEye; m_tPlayerMovingCamera.vEye = _vCameraEye; }

private:
	//void Movement(const _float& _fDeltaTime);

	//카메라의 At을 유동적으로 이동시키는 함수
	void MoveAt(const _float& _fDeltaTime);
	void Key_Input(const _float& _fDeltaTime);
	void Fix_Mouse();
	void ThirdViewCamMoveMent(const _float& _fDeltaTime);
	void ScrollViewCamMoveMent(const _float& _fDeltaTime);
	void TopViewCamMoveMent(const _float& _fDeltaTime);
	//플레이어 보정용 카메라
	void ThirdViewCam_Player(const _float& _fDeltaTime);
	void ScrollViewCam_Player(const _float& _fDeltaTime);
private:
	_bool			m_bMove;
	_bool			m_bIsFixMouse;
	_bool			m_bCinematicOnOff;
	_bool			m_bScrollFirstStart;
	_bool			m_bScrollSecondStart;
	_bool			m_bThirdViewStart;
	_bool			m_bTopViewStart;
	_bool			m_bScrollMoveEye;
	_bool			m_bTopviewEye;
	_bool			m_bTopViewAt;

	_float			m_fDistanceToTarget;
	_float			m_fScrollDistance;
	_float			m_fSpeed;
	_float			m_fCameraAngleX;
	_float			m_fCameraAngleY;
	_float			m_fCameraAngleX_Smooth;
	_float			m_fCameraAngleY_Smooth;
	_float			m_fSmoothTimeX;
	_float			m_fSmoothTimeY;
	
	//카메라의 At의 이동경로 리스트입니다.
	list<_float3>			m_listNextAt;
	_uint			m_iPlayerAttackCheck[3];

	_long			m_dwMouseMoveY;
	_long			m_dwMouseMoveX;

	eCameraMode				m_eCamMode;

	tagCameraDesc			m_tPlayerMovingCamera;
public:
	static CMainCamera* Create(_DEVICE _pDevice);
	virtual Engine::CGameObject * Clone(const _uint& _iSceneIdx, void *_pArg = nullptr);
	//virtual void Free() override;


};

#endif // !MAINCAMERA_H