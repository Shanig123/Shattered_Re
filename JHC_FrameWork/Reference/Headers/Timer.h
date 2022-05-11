#pragma once

#ifndef __TIMER_H__
#define __TIMER_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer final : public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer() = default;

public:
	HRESULT Ready_Timer();
	void Setup_Timer();
	
public:
	_float Get_DeltaTime() { return m_fDeltaTime; }

public:
	static	CTimer*		Create();
	virtual void		Free() override;

private:
	LARGE_INTEGER		m_CPUCount;
	LARGE_INTEGER		m_StartTime;
	LARGE_INTEGER		m_GoalTime;
	LARGE_INTEGER		m_FixTime;

	_float				m_fDeltaTime;
};

END

#endif // !__TIMER_H__