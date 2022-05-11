#include "Timer.h"

USING(Engine)

CTimer::CTimer()
	: m_fDeltaTime(0.f)
{
	ZeroMemory(&m_CPUCount, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_StartTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_GoalTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_FixTime, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Ready_Timer()
{
	if (!QueryPerformanceCounter(&m_StartTime))
	{
		ERROR_BOX(L"Failed To m_StartTime");
		return E_FAIL;
	}

	if (!QueryPerformanceCounter(&m_GoalTime))
	{
		ERROR_BOX(L"Failed To m_GoalTime");
		return E_FAIL;
	}

	if (!QueryPerformanceCounter(&m_FixTime))
	{
		ERROR_BOX(L"Failed To m_FixTime");
		return E_FAIL;
	}

	if (!QueryPerformanceFrequency(&m_CPUCount))
	{
		ERROR_BOX(L"Failed To m_CPUCount");
		return E_FAIL;
	}

	return S_OK;
}

void CTimer::Setup_Timer()
{
	if (!QueryPerformanceFrequency(&m_CPUCount))
	{
		ERROR_BOX(L"Failed To m_CPUCount");
		return;
	}

	if (!QueryPerformanceCounter(&m_GoalTime))
	{
		ERROR_BOX(L"Failed To m_GoalTime");
		return;
	}

	if (m_GoalTime.QuadPart - m_FixTime.QuadPart >= m_CPUCount.QuadPart)
	{
		if (!QueryPerformanceFrequency(&m_CPUCount))
		{
			ERROR_BOX(L"Failed To m_CPUCount");
			return;
		}

		m_FixTime = m_GoalTime;
	}

	m_fDeltaTime = (m_GoalTime.QuadPart - m_StartTime.QuadPart) / _float(m_CPUCount.QuadPart);
	m_StartTime.QuadPart = m_GoalTime.QuadPart;
}

CTimer * CTimer::Create()
{
	CTimer*	pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
}