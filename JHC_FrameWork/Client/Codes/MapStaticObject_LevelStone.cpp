#include "stdafx.h"
#include "MapStaticObject_LevelStone.h"

CMapStaticObject_LevelStone::CMapStaticObject_LevelStone(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_pUpgrader(nullptr)
{
}

CMapStaticObject_LevelStone::CMapStaticObject_LevelStone(const CMapStaticObject_LevelStone & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_pUpgrader(nullptr)
{
}

HRESULT CMapStaticObject_LevelStone::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_LevelStone::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
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

	CUIBase::UI_DESC tDesc;

	tDesc.vPos = { 0.f,0.f,1.f };
	tDesc.vSize = { _float(WINCX),_float(WINCY) };
	_tchar szBuff[MAX_PATH] = L"Component_Texture_Select_Upgrade_Back";
	lstrcpy(tDesc.szTexName, szBuff);

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_Select_Upgrader",
		(CGameObject**)&m_pUpgrader, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pUpgrader, m_iSceneIdx,
		L"Layer_NPCUI"), E_FAIL);
	Safe_AddRef(m_pUpgrader);

	m_iShaderPass = 13;
	m_bCheckRimDistance = true;
	m_fRimDistanceFix = 5.f;
	return S_OK;
}

CMapStaticObject_LevelStone * CMapStaticObject_LevelStone::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_LevelStone* pInstance = new CMapStaticObject_LevelStone(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_LevelStone::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_LevelStone* pClone = new CMapStaticObject_LevelStone(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_LevelStone::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pUpgrader);
	CMapStaticObjectBase::Free();
}
