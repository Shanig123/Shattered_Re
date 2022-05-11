#include "stdafx.h"
#include "MapStaticObject_YaakStand.h"

CMapStaticObject_YaakStand::CMapStaticObject_YaakStand(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
{
}

CMapStaticObject_YaakStand::CMapStaticObject_YaakStand(const CMapStaticObject_YaakStand & _rOther)
	: CMapStaticObjectBase(_rOther)
{
}

HRESULT CMapStaticObject_YaakStand::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_YaakStand::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	auto pManagement = GET_MANAGEMENT;

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
	_float3 vRotate = m_pTransform->Get_TransformDesc().vRotate;

	PxTriangleMeshDesc tTriangleMeshDesc;
	tTriangleMeshDesc.points.count = m_pMesh->Get_VtxCount();
	tTriangleMeshDesc.points.data = m_pMesh->Get_Vertices();
	tTriangleMeshDesc.points.stride = sizeof(VTX);
	tTriangleMeshDesc.triangles.count = m_pMesh->Get_TriCount();
	tTriangleMeshDesc.triangles.data = m_pMesh->Get_Indices();
	if (m_pMesh->Get_IndexSize() == sizeof(INDEX32))
		tTriangleMeshDesc.triangles.stride = sizeof(INDEX32);
	else
	{
		tTriangleMeshDesc.triangles.stride = sizeof(INDEX16);
		tTriangleMeshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
	}

	_float4Q Quat;
	D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);
	PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	pManagement->TriangleMeshCooking(m_tTransformDesc.szFileTitle, vScale, tTriangleMeshDesc, transform);

	return S_OK;
}

CMapStaticObject_YaakStand * CMapStaticObject_YaakStand::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_YaakStand* pInstance = new CMapStaticObject_YaakStand(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_YaakStand::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_YaakStand* pClone = new CMapStaticObject_YaakStand(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_YaakStand::Free()
{
	CMapStaticObjectBase::Free();
}
