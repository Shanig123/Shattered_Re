#pragma once

#ifndef __MYFONT_H__
#define __MYFONT_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMyFont final : public CBase
{
private:
	explicit CMyFont(_DEVICE _pDevice);
	virtual ~CMyFont() = default;

public:
	HRESULT Ready_MyFont(const wstring& _wstMyFontType, const _uint& _iWidth, const _uint& _iHeight, const _uint& _iWeight);
	void Render_MyFont(const wstring& _wstr, const _float2* _vPos, D3DXCOLOR _Color);

public:
	static CMyFont* Create(_DEVICE _pDevice, const wstring& _wstMyFontType, const _uint& _iWidth, const _uint& _iHeight, const _uint& _iWeight);
	
private:
	virtual void Free() override;

private:
	_DEVICE			m_pDevice;
	_FONT			m_pMyFont;
	LPD3DXSPRITE	m_pSprite;
};

END

#endif // !__MYFONT_H__
