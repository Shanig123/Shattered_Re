#include "Component.h"

USING(Engine)

CComponent::CComponent(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
	, m_bClone(false)
{
	Safe_AddRef(m_pDevice);
}

CComponent::CComponent(const CComponent & _rOther)
	: m_pDevice(_rOther.m_pDevice)
	, m_bClone(true)
{
	Safe_AddRef(m_pDevice);
}

_uint CComponent::Update_Component(const _float & _fDeltaTime)
{
	return _uint();
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
}
