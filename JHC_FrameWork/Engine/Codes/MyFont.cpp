#include "MyFont.h"

USING(Engine)

CMyFont::CMyFont(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
	, m_pMyFont(nullptr)
	, m_pSprite(nullptr)
{
	m_pDevice->AddRef();
}

HRESULT CMyFont::Ready_MyFont(const wstring & _wstMyFontType, const _uint & _iWidth, const _uint & _iHeight, const _uint & _iWeight)
{
	D3DXFONT_DESC tMyFontDesc;
	ZeroMemory(&tMyFontDesc, sizeof(D3DXFONT_DESC));

	tMyFontDesc.CharSet = HANGEUL_CHARSET;
	lstrcpy(tMyFontDesc.FaceName,_wstMyFontType.c_str());
	tMyFontDesc.Width = _iWidth;
	tMyFontDesc.Height = _iHeight;
	tMyFontDesc.Weight = _iWeight;

	//폰트 생성
	if (FAILED(D3DXCreateFontIndirect(m_pDevice, &tMyFontDesc, &m_pMyFont)))
	{
		ERROR_BOX(L"MyFont Creating Failed");
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSprite(m_pDevice, &m_pSprite)))
	{
		ERROR_BOX(L"Sprite Creating Failed");
		return E_FAIL;
	}

	return S_OK;
}

void CMyFont::Render_MyFont(const wstring & _wstr, const _float2* _vPos, D3DXCOLOR _Color)
{
	RECT rc = { (_long)_vPos->x, (_long)_vPos->y };

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pMyFont->DrawTextW(m_pSprite, _wstr.c_str(), lstrlen(_wstr.c_str()), &rc, DT_NOCLIP, _Color);

	m_pSprite->End();
}

CMyFont * CMyFont::Create(_DEVICE _pDevice, const wstring & _wstMyFontType, const _uint & _iWidth, const _uint & _iHeight, const _uint & _iWeight)
{
	CMyFont* pInstance = new CMyFont(_pDevice);
	if (FAILED(pInstance->Ready_MyFont(_wstMyFontType, _iWidth, _iHeight, _iWeight)))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMyFont::Free()
{
	Safe_Release(m_pMyFont);
	Safe_Release(m_pSprite);
	Safe_Release(m_pDevice);
}
