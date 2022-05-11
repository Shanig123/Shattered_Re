#pragma once

#ifndef __MAINAPP_H__
#define __MAINAPP_H__

#include "Base.h"

USING(Engine)

class CMainApp final : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Ready_MainApp();
	_uint Update_MainApp(_float& _rfDeltaTime);

private:
	HRESULT Setup_DefaultSetting();
	HRESULT Add_Component();
	void Key_Input();

	void Test();
	void Test2();

public:
	static CMainApp* Create();
	virtual void Free() override;

private:
	CManagement*		m_pManagement;
	_DEVICE				m_pDevice;

	_ulong				m_dwRenderCnt = 0;
	_tchar				m_szFPS[256] = L"";
	_float				m_fTime = 0.f;

	_bool				m_bPhysx;
};

#endif // !__MAINAPP_H__