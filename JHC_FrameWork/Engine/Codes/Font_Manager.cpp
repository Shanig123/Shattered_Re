#include "Font_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}


HRESULT CFont_Manager::Ready_Font(_DEVICE _pDevice, const wstring & _pFontTag, const wstring & _wstFontType, const _uint & _iWidth, const _uint & _iHeight, const _uint & _iWeight)
{
	CMyFont* pFont = nullptr;

	pFont = Find_Font(_pFontTag);
	if (nullptr != pFont)
		return E_FAIL;

	pFont = CMyFont::Create(_pDevice, _wstFontType, _iWidth, _iHeight, _iWeight);

	m_mapFont.emplace(_pFontTag, pFont);
	

	return S_OK;
}

void CFont_Manager::Render_Font(const wstring & _pFontTag, const wstring & _wstr, const _float2 * _vPos, D3DXCOLOR _Color)
{
	CMyFont* pFont = Find_Font(_pFontTag);
	if (nullptr == pFont)
		return;

	pFont->Render_MyFont(_wstr, _vPos, _Color);
}

CMyFont * CFont_Manager::Find_Font(const wstring & _pFontTag)
{
	auto iter_find = m_mapFont.find(_pFontTag);

	if (iter_find == m_mapFont.end())
		return nullptr;

	return iter_find->second;
}

void CFont_Manager::Free()
{
	for_each(m_mapFont.begin(), m_mapFont.end(), CDeleteMap());
	m_mapFont.clear();
}