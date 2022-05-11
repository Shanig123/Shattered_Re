#pragma once

#ifndef __GRAPHIC_DEVICE_H__
#define __GRAPHIC_DEVICE_H__

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

private:
	explicit CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Ready_Graphic_Device(HWND _hWnd, _uint _iWinCX, _uint _iWinCY, EDisplayMode _eDisplayMode, _bool _bLowGraphicMode);
	void Render_Begin(D3DCOLOR _Color);
	void Render_End();

public:
	_DEVICE Get_Device() { return m_pDevice; }
	const _float2& Get_Aspect() const { return m_vAspect; }
	const _bool& Get_LowGraphicMode() const { return m_bLowGraphicMode; }
public:
	virtual void Free() override;

private:
	_SDK		m_pSDK;
	_DEVICE		m_pDevice;
	_float2		m_vAspect;
	_bool		m_bLowGraphicMode;
};

END

#endif // !__GRAPHIC_DEVICE_H__