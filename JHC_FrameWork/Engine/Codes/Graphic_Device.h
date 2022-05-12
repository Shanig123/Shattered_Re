#pragma once

#ifndef __GRAPHIC_DEVICE_H__
#define __GRAPHIC_DEVICE_H__

#include "Base.h"
#include "D3Dclass.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

private:
	explicit CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Ready_Graphic_Device(HWND _hWnd, _uint _iWinCX, _uint _iWinCY, EDisplayMode _eDisplayMode, _bool _bLowGraphicMode,
	_bool _bVsunc);
	void Render_Begin(D3DCOLOR _Color);
	void Render_Begin(float _fRed, float _fGreen, float _fBlue, float _fAlpha);
	void Render_End();

public:
	_DEVICE Get_Device() { return m_pDevice; }
	const _float2& Get_Aspect() const { return m_vAspect; }
	const _bool& Get_LowGraphicMode() const { return m_bLowGraphicMode; }
public:
	virtual void Free() override;

private:
	bool m_bVsync_enabled;
	int m_iVideoCardMemory;
	char m_cVideoCardDescription[128];
	IDXGISwapChain* m_pSwapChain;
	_DEVICE m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11RasterizerState* m_pRasterState;
	XMMATRIX m_matProjectionMatrix;
	XMMATRIX m_matWorldMatrix;
	XMMATRIX m_matOrthoMatrix;
private:
	//_SDK		m_pSDK;
	//_DEVICE		m_pDevice;
	_float2		m_vAspect;
	_bool		m_bLowGraphicMode;
};

END

#endif // !__GRAPHIC_DEVICE_H__