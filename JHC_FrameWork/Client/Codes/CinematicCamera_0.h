#pragma once
#ifndef __CINEMATICCAMERA_0_H__
#define __CINEMATICCAMERA_0_H__

#include "Camera.h"

USING(Engine)

class CCinematicCamera_0 : public CCamera
{
private:
	explicit CCinematicCamera_0(_DEVICE _pDevice);
	explicit CCinematicCamera_0(const CCinematicCamera_0& _orther);
	virtual ~CCinematicCamera_0() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg) override;
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	//virtual HRESULT Render_GameObject() override;

public:
	//CAMERA_DESC& Get_CameraDesc() { return m_tCameraDesc; }
	_bool	Get_CineEnd() { return m_bCinemaEnd; }
	void	Play_NextCinematic(_uint i) 
	{
		if (m_iSaveCount <= i)
		{
			m_iSaveCount = i;
			m_bCinemaEnd = false;
			m_bPlay = true;
			m_bFirstPos = true;
		}
	}
	void	Play_NextEndingCinematic(_uint i)
	{
		m_iSaveCount = i;
		m_bCinemaEnd = false;
		m_bPlay = true;
		m_bFirstPos = true;
	}
	//void	Shaker();
private:
	void Movement(const _float& _fDeltaTime);
	void Key_Input(const _float& _fDeltaTime);

private:
	void Record();//지정 위치 녹화
	void Delete_Record();//지정 위치 녹화
	void BezierSpline(const _float& _fDeltaTime);
	void Play_TutorialCine(const _float& _fDeltaTime);
	void Play_Stage2Cine(const _float& _fDeltaTime);

	void Play_StaticPlayer_Tutorial(const _float& _fDeltaTime);
	void Chase_Target(const _float3 & _vTargetPos, const _float & _fSpeed, const _float & _fDeltaTime);
	//녹화한 위치들을 재생
	HRESULT Save_Record();
	HRESULT Load_Record();
private:
	typedef struct tagCameraRecord
	{
		_float3		vEye;
		_float3		vAt;
	}RECORD;

	_float			m_fSpeed;

	_bool			m_bMove;

	_float3					m_vCurEyePos;
	_float3					m_vNextEyePos;
	_float3					m_vCurAtPos;
	_float3					m_vNextAtPos;

	_bool					m_bEditMode;
	_bool					m_bRecord;
	_bool					m_bPlay;
	_bool					m_bPlayerLockon;
	_bool					m_bFirstPos;
	_bool					m_bFirstCheck;

	_float					m_fDistanceToTarget;
	_float					m_fCameraAngleX;
	_float					m_fCameraAngleY;
	_float					m_fBezierTime;
	_float					m_fStaticCameraTimer;

	_int					m_iRecIndex;
	_int					m_iLerp;
	_uint					m_iSaveCount;

	_bool					m_bCinemaEnd;

	vector<RECORD>			m_vecPosition;

	_bool					m_bInit;
public:

	static CCinematicCamera_0* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

};

#endif // !1
