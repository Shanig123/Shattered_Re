#pragma once

#ifndef __TIME_MANAGER_H__
#define __TIME_MANAGER_H__

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class CTime_Manager final : public CBase
{
	DECLARE_SINGLETON(CTime_Manager)

private:
	explicit CTime_Manager();
	virtual ~CTime_Manager() = default;

public:
	HRESULT Ready_Time_Manager(const wstring& _pTimerTag);

public:
	_float Get_DeltaTime(const wstring& _pTimerTag);

public:
	void Set_DeltaTime(const wstring& _pTimerTag);

private:
	CTimer* Find_Timer(const wstring& _pTimerTag);

public:
	virtual void Free() override;

private:
	typedef map<wstring, CTimer*>		TIMERS;
	TIMERS		m_Timers;
};

END

#endif // !__TIME_MANAGER_H__