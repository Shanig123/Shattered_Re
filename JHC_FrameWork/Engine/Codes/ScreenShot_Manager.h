#pragma once

#ifndef __ScreenShot_MANAGER_H__
#define __ScreenShot_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CScreenShot_Manager final : public CBase
{
	DECLARE_SINGLETON(CScreenShot_Manager)

private:
	explicit CScreenShot_Manager();
	virtual ~CScreenShot_Manager() = default;

public:
	HRESULT CreateSurface(_DEVICE _Device, const _float& _fScreenX, const _float& _fScreenY);

public:
	virtual void Free() override;

private:
	_uint		m_iScreenShot;
	wstring		m_wstrScreenShotPath;

};

END

#endif // !__ScreenShot_MANAGER_H__