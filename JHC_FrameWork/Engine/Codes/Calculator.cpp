#include "Calculator.h"

USING(Engine)

CCalculator::CCalculator(_DEVICE _pDevice)
	: CComponent(_pDevice)
{
}

CCalculator::CCalculator(const CCalculator & _rOther)
	: CComponent(_rOther)
{
}

HRESULT CCalculator::Ready_Component_Prototype()
{
	return S_OK;
}

HRESULT CCalculator::Ready_Component_Clone(void * _pArg)
{
	return S_OK;
}

_uint CCalculator::Update_Component(const _float& _fDeltaTime)
{
	return _uint();
}

_bool CCalculator::Collision_AABB(const _float3 * _rDestMin, const _float3 * _rDestMax, const _float4x4 * _rDestWorld, const _float3 * _rSrcMin, const _float3 * _rSrctMax, const _float4x4 * _rSrcWorld)
{
	_float3 vDestMin, vDestMax, vSourMin, vSourMax;
	_float fDest, fSrc;

	D3DXVec3TransformCoord(&vDestMin, _rDestMin, _rDestWorld);
	D3DXVec3TransformCoord(&vDestMax, _rDestMax, _rDestWorld);

	D3DXVec3TransformCoord(&vSourMin, _rSrcMin, _rSrcWorld);
	D3DXVec3TransformCoord(&vSourMax, _rSrctMax, _rSrcWorld);

	// x축에서 바라봤을 때

	fDest = max(vDestMin.x, vSourMin.x);
	fSrc = min(vDestMax.x, vSourMax.x);

	if (fDest > fSrc)
		return false;

	// y축에서 바라봤을 때

	fDest = max(vDestMin.y, vSourMin.y);
	fSrc = min(vDestMax.y, vSourMax.y);

	if (fDest > fSrc)
		return false;

	// z축에서 바라봤을 때

	fDest = max(vDestMin.z, vSourMin.z);
	fSrc = min(vDestMax.z, vSourMax.z);

	if (fDest > fSrc)
		return false;

	return true;
}

_bool CCalculator::Collision_OBB(const _float3 * _rDestMin, const _float3 * _rDestMax, const _float4x4 * _rDestWorld, const _float3 * _rSrcMin, const _float3 * _rSrctMax, const _float4x4 * _rSrcWorld)
{
	OBB		tObb[2];
	ZeroMemory(tObb, sizeof(OBB) * 2);

	Set_Point(&tObb[0], _rDestMin, _rDestMax);
	Set_Point(&tObb[1], _rSrcMin, _rSrctMax);

	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&tObb[0].vPoint[i], &tObb[0].vPoint[i], _rDestWorld);
		D3DXVec3TransformCoord(&tObb[1].vPoint[i], &tObb[1].vPoint[i], _rSrcWorld);
	}
	D3DXVec3TransformCoord(&tObb[0].vCenter, &tObb[0].vCenter, _rDestWorld);
	D3DXVec3TransformCoord(&tObb[1].vCenter, &tObb[1].vCenter, _rSrcWorld);

	for (_uint i = 0; i < 2; ++i)
		Set_Axis(&tObb[i]);

	_float fDistance[3];		// 0번 : 첫 번째 obb의 중점에서 각 면을 향해 뻗어나가는 축을 투영한 길이의 총합
								// 1번 : 두 번째 obb의 중점에서 각 면을 향해 뻗어나가는 축을 투영한 길이의 총합
								// 2번 : 두 obb의 중점 간의 거리의 길이를 축에 투영한 길이

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tObb[0].vProjAxis[0], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[0].vProjAxis[1], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[0].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[1] = fabs(D3DXVec3Dot(&tObb[1].vProjAxis[0], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[1].vProjAxis[1], &tObb[i].vAxis[j])) +
				fabs(D3DXVec3Dot(&tObb[1].vProjAxis[2], &tObb[i].vAxis[j]));

			fDistance[2] = fabs(D3DXVec3Dot(&(tObb[1].vCenter - tObb[0].vCenter), &tObb[i].vAxis[j]));

			if (fDistance[0] + fDistance[1] < fDistance[2])
				return false;

		}
	}

	return true;
}

_bool CCalculator::Collision_Spher(_float4x4& _vStartWorld, _float _fStartRadius, _float4x4& _vEndWorld, _float _fEndRadius)
{
	_float3 vStartPos;
	memcpy(&vStartPos, &_vStartWorld.m[3][0], sizeof(_float3));

	_float3 vEndPos;
	memcpy(&vEndPos, &_vEndWorld.m[3][0], sizeof(_float3));

	_float3 vDir = vEndPos - vStartPos;

	_float fDist = D3DXVec3Length(&vDir);

	if (fDist <= (_fStartRadius + _fEndRadius))
		return true;

	return false;
}

void CCalculator::Set_Point(OBB * pObb, const _float3 * pMin, const _float3 * pMax)
{
	pObb->vPoint[0] = _float3(pMin->x, pMax->y, pMin->z);
	pObb->vPoint[1] = _float3(pMax->x, pMax->y, pMin->z);
	pObb->vPoint[2] = _float3(pMax->x, pMin->y, pMin->z);
	pObb->vPoint[3] = _float3(pMin->x, pMin->y, pMin->z);

	pObb->vPoint[4] = _float3(pMin->x, pMax->y, pMax->z);
	pObb->vPoint[5] = _float3(pMax->x, pMax->y, pMax->z);
	pObb->vPoint[6] = _float3(pMax->x, pMin->y, pMax->z);
	pObb->vPoint[7] = _float3(pMin->x, pMin->y, pMax->z);

	pObb->vCenter = (*pMin + *pMax) * 0.5f;
}

void CCalculator::Set_Axis(OBB * pObb)
{
	pObb->vProjAxis[0] = (pObb->vPoint[2] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[1] = (pObb->vPoint[0] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;
	pObb->vProjAxis[2] = (pObb->vPoint[7] + pObb->vPoint[5]) * 0.5f - pObb->vCenter;

	pObb->vAxis[0] = pObb->vPoint[2] - pObb->vPoint[3];
	pObb->vAxis[1] = pObb->vPoint[0] - pObb->vPoint[3];
	pObb->vAxis[2] = pObb->vPoint[7] - pObb->vPoint[3];

	for (_uint i = 0; i < 3; ++i)
		D3DXVec3Normalize(&pObb->vAxis[i], &pObb->vAxis[i]);
}

CCalculator * CCalculator::Create(_DEVICE _pDevice)
{
	CCalculator* pInstance = new CCalculator(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCalculator::Clone(void * _pArg)
{
	CCalculator* pClone = new CCalculator(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCalculator::Free()
{
	CComponent::Free();
}
