#include "Frustum.h"

USING(Engine)

CFrustum::CFrustum(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
{
	m_pDevice->AddRef();
}

CFrustum::CFrustum(const CFrustum & _rOther)
	: m_pDevice(_rOther.m_pDevice)
{
	m_pDevice->AddRef();
}

HRESULT CFrustum::Ready_Frustum()
{
	return S_OK;
}

void CFrustum::ConstructFrustum(_float _fFar)
{
	_float4x4 matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);

	_float fMinimum = -matProj._43 / matProj._33;
	_float r = _fFar / (_fFar - fMinimum);

	matProj._33 = r;
	matProj._43 = -r * fMinimum;

	_float4x4 matFinalProj;
	D3DXMatrixMultiply(&matFinalProj, &matView, &matProj);

	// near
	m_vPlanes[0].a = matFinalProj._14 + matFinalProj._13;
	m_vPlanes[0].b = matFinalProj._24 + matFinalProj._23;
	m_vPlanes[0].c = matFinalProj._34 + matFinalProj._33;
	m_vPlanes[0].d = matFinalProj._44 + matFinalProj._43;
	D3DXPlaneNormalize(&m_vPlanes[0], &m_vPlanes[0]);

	// far
	m_vPlanes[1].a = matFinalProj._14 - matFinalProj._13;
	m_vPlanes[1].b = matFinalProj._24 - matFinalProj._23;
	m_vPlanes[1].c = matFinalProj._34 - matFinalProj._33;
	m_vPlanes[1].d = matFinalProj._44 - matFinalProj._43;
	D3DXPlaneNormalize(&m_vPlanes[1], &m_vPlanes[1]);

	// left
	m_vPlanes[2].a = matFinalProj._14 + matFinalProj._11;
	m_vPlanes[2].b = matFinalProj._24 + matFinalProj._21;
	m_vPlanes[2].c = matFinalProj._34 + matFinalProj._31;
	m_vPlanes[2].d = matFinalProj._44 + matFinalProj._41;
	D3DXPlaneNormalize(&m_vPlanes[2], &m_vPlanes[2]);

	// right
	m_vPlanes[3].a = matFinalProj._14 - matFinalProj._11;
	m_vPlanes[3].b = matFinalProj._24 - matFinalProj._21;
	m_vPlanes[3].c = matFinalProj._34 - matFinalProj._31;
	m_vPlanes[3].d = matFinalProj._44 - matFinalProj._41;
	D3DXPlaneNormalize(&m_vPlanes[3], &m_vPlanes[3]);

	// top
	m_vPlanes[4].a = matFinalProj._14 - matFinalProj._12;
	m_vPlanes[4].b = matFinalProj._24 - matFinalProj._22;
	m_vPlanes[4].c = matFinalProj._34 - matFinalProj._32;
	m_vPlanes[4].d = matFinalProj._44 - matFinalProj._42;
	D3DXPlaneNormalize(&m_vPlanes[4], &m_vPlanes[4]);

	// bottom
	m_vPlanes[5].a = matFinalProj._14 + matFinalProj._12;
	m_vPlanes[5].b = matFinalProj._24 + matFinalProj._22;
	m_vPlanes[5].c = matFinalProj._34 + matFinalProj._32;
	m_vPlanes[5].d = matFinalProj._44 + matFinalProj._42;
	D3DXPlaneNormalize(&m_vPlanes[5], &m_vPlanes[5]);
}

_bool CFrustum::CheckCube_Frustum(const _float3& _vMin, const _float3& _vMax, const _float4x4& _matWorld)
{
	_float3 vMin = _vMin;
	_float3 vMax = _vMax;
	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXVec3TransformCoord(&vMin, &_vMin, &matWorld);
	D3DXVec3TransformCoord(&vMax, &_vMax, &matWorld);

	for (_uint i = 0; i < 6; ++i)
	{
		// 왼 아래 앞
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMin.x, vMin.y, vMin.z)) >= 0.f)
			continue;
		
		// 오른 아래 앞
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMax.x, vMin.y, vMin.z)) >= 0.f)
			continue;

		// 왼 위 앞
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMin.x, vMax.y, vMin.z)) >= 0.f)
			continue;

		//  왼 아래 뒤
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMin.x, vMin.y, vMax.z)) >= 0.f)
			continue;

		// 오른 위 앞
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMax.x, vMax.y, vMin.z)) >= 0.f)
			continue;

		// 오른 아래 뒤
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMax.x, vMin.y, vMax.z)) >= 0.f)
			continue;

		//오른 위 뒤
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMin.x, vMax.y, vMax.z)) >= 0.f)
			continue;		

		// 오른 위 뒤
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &_float3(vMax.x, vMax.y, vMax.z)) >= 0.f)
			return true;

		return false;
	}

	return true;
}

_bool CFrustum::CheckSphere_Frustum(const _float & _fCenterX, const _float & _fCenterY, const _float & _fCenterZ, _float _fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (D3DXPlaneDotCoord(&m_vPlanes[i], &D3DXVECTOR3(_fCenterX, _fCenterY, _fCenterZ)) < -_fRadius)
		{
			return false;
		} 
	} 

	return true;
}

CFrustum * CFrustum::Create(_DEVICE _pDevice)
{
	CFrustum* pInstance = new CFrustum(_pDevice);
	if (FAILED(pInstance->Ready_Frustum()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	Safe_Release(m_pDevice);
}
