#pragma once
#ifndef _AFXDLL

#ifndef __FONT_H__
#define __FONT_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFont final : public CBase
{
private:
	explicit CFont(_DEVICE _pDevice);
	virtual ~CFont() = default;

public:
	HRESULT Ready_Font(const wstring& _wstFontType, const _uint& _iWidth, const _uint& _iHeight, const _uint& _iWeight);
	void Render_Font(const wstring& _wstr, const _float2* _vPos, D3DXCOLOR _Color);

public:
	static CFont* Create(_DEVICE _pDevice, const wstring& _wstFontType, const _uint& _iWidth, const _uint& _iHeight, const _uint& _iWeight);
	
private:
	virtual void Free() override;

private:
	_DEVICE			m_pDevice;
	_FONT			m_pFont;
	LPD3DXSPRITE	m_pSprite;
};

END

#endif // !__FONT_H__

#endif // !_AFXDLL
