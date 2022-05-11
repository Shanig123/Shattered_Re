#pragma once

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(_DEVICE _pDevice);
	explicit CComponent(const CComponent& _rOther);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Ready_Component_Prototype() PURE;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) PURE;
	virtual _uint Update_Component(const _float& _fDeltaTime) PURE;
public:
	const _bool& Get_CheckClone()const { return m_bClone; }
public:
	virtual CComponent* Clone(void* _pArg = nullptr) PURE;
	virtual void Free() override;

protected:
	_DEVICE		m_pDevice;
	_bool		m_bClone;
};

END

#endif // !__COMPONENT_H__