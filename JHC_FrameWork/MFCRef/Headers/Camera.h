#pragma once

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
protected:
	explicit CCamera(_DEVICE _pDevice);
	explicit CCamera(const CCamera& _orther);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
public:
	//
	/*
	CAMEFFECT_TOP_CUTSCENE, 
	CAMEFFECT_CAMSHAKING_QUAKE,
	CAMEFFECT_CAMSHAKING_DAMAGE
	*/
	inline void SetCamEffectFlag(const _uint& _iCamEffectFlag) { m_iCameraEffect = _iCamEffectFlag; }
	inline void AddCamEffectFlag(const _uint& _iCamEffectFlag) { m_iCameraEffect |= _iCamEffectFlag; }
	inline void MinCamEffectFlag(const _uint& _iCamEffectFlag) { m_iCameraEffect &= ~_iCamEffectFlag; }
public:
	inline const CAMERA_DESC& Get_CameraDesc() const;
	inline const _uint& Get_CamEffect() const {return m_iCameraEffect;	}
	inline void Set_At(const _float3& _vAt) { m_tCameraDesc.vAt = _vAt; }
public:
	void CamShakerWeak();
	void CamShakerHeavy();
	void CamShakeQuake();
protected:
	void CameraTopCutScene(const _float& _fDeltaTime);
	void CameraShaking(const _float& _fDeltaTime);
	void CameraShakingDamaged(const _float& _fDeltaTime);
	void CameraShakingHeavy(const _float& _fDeltaTime);
public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() override;

protected:
	//탑 컷씬 관련 변수
	_float				m_fTopCutSceneTime;
	_bool				m_bTopCutSceneInit;
	_float3				m_vTopCutSceneEyeInit;
	_float3				m_vTopCutSceneAtInit;
	//쉐이킹 관련 변수
	_float				m_fShakingTime;
	_bool				m_bShakingCamInit;
	_float3				m_vShakingPosInit;

	//타격 쉐이킹 관련 변수
	_float				m_fShakingTimeDamage;
	_bool				m_bShakingDamageCamInit;
	_float3				m_vShakingDamagePosInit;

	//헤비 쉐이킹 관련 변수
	_float				m_fShakingTimeHeavy;
	_bool				m_bShakingHeavyCamInit;
	_float3				m_vShakingHeavyPosInit;

	//카메라 효과
	_uint				m_iCameraEffect;

	CAMERA_DESC			m_tCameraDesc;
};
#define CAMEFFECT_NO						(0)
#define CAMEFFECT_TOP_CUTSCENE				(1)
#define CAMEFFECT_CAMSHAKING_QUAKE			(1<<1)
#define CAMEFFECT_CAMSHAKING_DAMAGE			(1<<2)
#define CAMEFFECT_CAMSHAKING_DAMAGE2		(1<<3)

END

#endif // !__CAMERA_H__