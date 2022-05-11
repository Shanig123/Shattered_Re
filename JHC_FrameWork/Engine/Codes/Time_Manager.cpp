#include "Time_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CTime_Manager)

CTime_Manager::CTime_Manager()
{
}

HRESULT CTime_Manager::Ready_Time_Manager(const wstring& _pTimerTag)
{
	CTimer* pTimer = Find_Timer(_pTimerTag);
	if (nullptr != pTimer)
	{
		ERROR_BOX(L"pTimer is Find");
		return E_FAIL;
	}

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
	{
		ERROR_BOX(L"pTimer is nullptr");
		return E_FAIL;
	}

	m_Timers.emplace(_pTimerTag, pTimer);

	return S_OK;
}

_float CTime_Manager::Get_DeltaTime(const wstring& _pTimerTag)
{
	CTimer* pTimer = Find_Timer(_pTimerTag);
	if (nullptr == pTimer)
	{
		ERROR_BOX(L"pTimer is nullptr");
		return 0.f;
	}

	return pTimer->Get_DeltaTime();
}

void CTime_Manager::Set_DeltaTime(const wstring& _pTimerTag)
{
	CTimer* pTimer = Find_Timer(_pTimerTag);
	if (nullptr == pTimer)
	{
		ERROR_BOX(L"pTimer is nullptr");
		return;
	}

	pTimer->Setup_Timer();
}

CTimer * CTime_Manager::Find_Timer(const wstring& _pTimerTag)
{
	auto iter_find = m_Timers.find(_pTimerTag);
	if (m_Timers.end() == iter_find)
	{
		return nullptr;
	}

	return iter_find->second;
}

void CTime_Manager::Free()
{
	for_each(m_Timers.begin(), m_Timers.end(), CDeleteMap());
	m_Timers.clear();
}
