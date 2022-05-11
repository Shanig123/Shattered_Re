#pragma once

#ifndef __FONT_MANAGER_H__
#define __FONT_MANAGER_H__

#include "Base.h"
#include "MyFont.h"

BEGIN(Engine)

class ENGINE_DLL CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)

private:
	explicit CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Ready_Font(_DEVICE _pDevice, const wstring& _pFontTag, const wstring& _wstFontType, const _uint& _iWidth, const _uint& _iHeight, const _uint& _iWeight);
	void Render_Font(const wstring& _pFontTag, const wstring& _wstr, const _float2* _vPos, D3DXCOLOR _Color);

private:
	CMyFont* Find_Font(const wstring& _pFontTag);

private:
	virtual void Free() override;

private:
	map<const wstring, CMyFont*>		m_mapFont;
};

END

#endif // !__FONT_MANAGER_H__