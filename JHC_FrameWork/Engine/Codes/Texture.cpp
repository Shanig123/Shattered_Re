#include "Texture.h"

USING(Engine)

CTexture::CTexture(_DEVICE _pDevice, ETextureType _eTextureType, wstring _wstrFilePath, _uint _iCount)
	: CComponent(_pDevice)
	, m_eTextureType(_eTextureType)
	, m_wstrFilePath(_wstrFilePath)
	, m_iCount(_iCount)
{
}

CTexture::CTexture(const CTexture & _rOther)
	: CComponent(_rOther)
	, m_eTextureType(_rOther.m_eTextureType)
	, m_wstrFilePath(_rOther.m_wstrFilePath)
	, m_iCount(_rOther.m_iCount)
	, m_Textures(_rOther.m_Textures)
{
}

HRESULT CTexture::Ready_Component_Prototype()
{
	TCHAR _szFullPath[MAX_PATH] = L"";
	IDirect3DBaseTexture9* pTexture = nullptr;
	HRESULT hr = 0;

	m_Textures.reserve(m_iCount);

	for (_uint i = 0; i < m_iCount; ++i)
	{
		swprintf_s(_szFullPath, m_wstrFilePath.c_str(), i);

		switch (m_eTextureType)
		{
		case ETextureType::Normal:
			hr = D3DXCreateTextureFromFile(m_pDevice, _szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture);
			break;
		case ETextureType::Cube:
			hr = D3DXCreateCubeTextureFromFile(m_pDevice, _szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);
			break;
		}

		if (FAILED(hr))
		{
			TCHAR _szLog[MAX_PATH] = L"";
			TCHAR _szMessage[] = L"Failed To %s";
			swprintf_s(_szLog, _szMessage, _szFullPath);
			ERROR_BOX(_szLog);

			return E_FAIL;
		}

		m_Textures.push_back(pTexture);
	}

	return S_OK;
}

HRESULT CTexture::Ready_Component_Clone(void * _pArg)
{
	return S_OK;
}

_uint CTexture::Update_Component(const _float& _fDeltaTime)
{
	return _uint();
}

HRESULT CTexture::Set_Texture(const _uint& _iIndex)
{
	if (m_Textures.size() <= _iIndex)
	{
		ERROR_BOX(L"m_Textures is Over");
	}

	return m_pDevice->SetTexture(0, m_Textures[_iIndex]);
}

HRESULT CTexture::Set_Texture(_EFFECT& _pEffect, const char* _pConstantTable, const _uint& _iIndex)
{
	if (m_Textures.size() <= _iIndex)
	{

		ERROR_BOX(L"m_Textures is Over");
		return E_FAIL;
	}

	// 텍스쳐를 그리기 하는 함수
	_pEffect->SetTexture(_pConstantTable, m_Textures[_iIndex]);	

	return S_OK;
}

CTexture * CTexture::Create(_DEVICE _pDevice, ETextureType _eTextureType, wstring _wstrFilePath, _uint _iCount)
{
	CTexture* pInstance = new CTexture(_pDevice, _eTextureType, _wstrFilePath, _iCount);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * _pArg)
{
	CTexture* pClone = new CTexture(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CTexture::Free()
{
	if (false == m_bClone)
	{
		for (auto& pTexture : m_Textures)
		{
			Safe_Release(pTexture);
		}

		m_Textures.clear();
	}

	CComponent::Free();
}
