#pragma once

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	explicit CTexture(_DEVICE _pDevice, ETextureType _eTextureType, wstring _wstrFilePath, _uint _iCount);
	explicit CTexture(const CTexture& _rOther);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;

public:
	HRESULT Set_Texture(const _uint& _iIndex = 0);
	HRESULT Set_Texture(_EFFECT& _pEffect, const char* _pConstantTable, const _uint& _iIndex = 0);
public:
	const _uint& Get_TextureTotalSize() const { return m_iCount; }
public:
	static CTexture* Create(_DEVICE _pDevice, ETextureType _eTextureType, wstring _wstrFilePath, _uint _iCount);
	virtual CComponent * Clone(void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	typedef vector<LPDIRECT3DBASETEXTURE9>	TEXTURES;
	TEXTURES		m_Textures;
	wstring			m_wstrFilePath;
	_uint			m_iCount;
	ETextureType	m_eTextureType;
};

END

#endif // !__TEXTURE_H__