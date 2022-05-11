#pragma once

#ifndef __SHADER_H__
#define __SHADER_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	explicit CShader(_DEVICE _pDevice, const wstring& _wstrFilePath);
	explicit CShader(const CShader& _rOther);
	virtual ~CShader() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;

public:
	_EFFECT Get_EffectHandle() { return m_pEffect; }

public:
	static CShader* Create(_DEVICE _pDevice, const wstring& _wstrFilePath);
	virtual CComponent* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	_EFFECT		m_pEffect;
	_BUFFER		m_pErrMsg;

	wstring		m_wstrFilePath;
	
};

END

#endif // !__SHADER_H__