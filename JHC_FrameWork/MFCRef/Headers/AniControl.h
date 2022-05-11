#pragma once

#ifndef __ANICONTROL_H__
#define __ANICONTROL_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAniControl final : public CBase
{
private:
	explicit CAniControl(_ANICONTROLLER _pAniControl);
	explicit CAniControl(const CAniControl& _rOther);
	virtual ~CAniControl() = default;

public:
	HRESULT Ready_AniControl();
	void Set_Animationset(const _uint& _iIndex);
	void Change_Animationset(const _uint& _iIndex);
	_uint& Get_Animationset() { return m_iOldAniIndex; }
	_bool Get_AnimationFrame(_double _dMaxFrame, _double _dWantFrame);
	void Play_Animationset(const _float& _fDeltaTime);

public:
	_bool Is_AnimationSetEnd();

public:
	static CAniControl* Create(_ANICONTROLLER _pAniControl);
	static CAniControl* Create(const CAniControl& _rOther);
	virtual void Free() override;

private:
	_ANICONTROLLER		m_pAniControl;
	_uint				m_iCurrentTrack;
	_uint				m_iNewTrack;
	_float				m_fAccTime;
	_uint				m_iOldAniIndex;
	_double				m_dPeriod;
};

END

#endif // !__ANICONTROL_H__