#include "Optimization.h"

USING(Engine)

COptimization::COptimization(_DEVICE _pDevice)
	: CComponent(_pDevice)
	, m_pFrustum(nullptr)
{
}

COptimization::COptimization(const COptimization & _rOther)
	: CComponent(_rOther)
	, m_pFrustum(_rOther.m_pFrustum)
{
}

HRESULT COptimization::Ready_Component_Prototype()
{
	m_pFrustum = CFrustum::Create(m_pDevice);

	return S_OK;
}

HRESULT COptimization::Ready_Component_Clone(void * _pArg)
{
	return S_OK;
}

_uint COptimization::Update_Component(const _float & _fDeltaTime)
{
	return _uint();
}

void COptimization::ConstructFrustum(_float _fFar)
{
	m_pFrustum->ConstructFrustum(_fFar);
}

_bool COptimization::CheckCube_Frustum(const _float3 & _vMin, const _float3 & _vMax, const _float4x4& _matWorld)
{
	return m_pFrustum->CheckCube_Frustum(_vMin, _vMax, _matWorld);
}

_bool COptimization::CheckSphere_Frustum(const _float & _fCenterX, const _float & _fCenterY, const _float & _fCenterZ, _float _fRadius)
{
	return  m_pFrustum->CheckSphere_Frustum(_fCenterX, _fCenterY, _fCenterZ, _fRadius);
}

COptimization * COptimization::Create(_DEVICE _pDevice)
{
	COptimization* pInstance = new COptimization(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * COptimization::Clone(void * _pArg)
{
	COptimization* pClone = new COptimization(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void COptimization::Free()
{
	if (false == m_bClone)
	{
		Safe_Release(m_pFrustum);
	}

	CComponent::Free();
}
