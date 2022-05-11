#include "Mesh.h"

USING(Engine)

CMesh::CMesh(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
	, m_bClone(false)
	, m_eMeshType(EMeshType::End)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
	, m_pVertices(nullptr)
	, m_pIndices(nullptr)
	, m_iTriCount(0)
	, m_pVtxPos(nullptr)
	, m_dwVtxCount(0)
	, m_dwStride(0)
	, m_pIndexSize(0)
	, m_vObjectMax(0.f, 0.f, 0.f)
	, m_vObjectMix(0.f, 0.f, 0.f)
	, m_vObjectCenter(0.f, 0.f, 0.f)
	, m_fObjectRadius(0.f)
	, m_vMax(nullptr)
	, m_vMin(nullptr)
	, m_dwSubsetCount(0)
{
	Safe_AddRef(m_pDevice);
}

CMesh::CMesh(const CMesh & _rOther)
	: m_pDevice(_rOther.m_pDevice)
	, m_bClone(true)
	, m_eMeshType(_rOther.m_eMeshType)
	, m_pVB(_rOther.m_pVB)
	, m_pIB(_rOther.m_pIB)
	, m_pVertices(_rOther.m_pVertices)
	, m_pIndices(_rOther.m_pIndices)
	, m_iTriCount(_rOther.m_iTriCount)
	, m_pVtxPos(_rOther.m_pVtxPos)
	, m_dwVtxCount(_rOther.m_dwVtxCount)
	, m_dwStride(_rOther.m_dwStride)
	, m_pIndexSize(_rOther.m_pIndexSize)
	, m_vObjectMax(_rOther.m_vObjectMax)
	, m_vObjectMix(_rOther.m_vObjectMix)
	, m_vObjectCenter(_rOther.m_vObjectCenter)
	, m_fObjectRadius(_rOther.m_fObjectRadius)
	, m_vMax(_rOther.m_vMax)
	, m_vMin(_rOther.m_vMin)
	, m_dwSubsetCount(_rOther.m_dwSubsetCount)
	, m_vecVertices(_rOther.m_vecVertices)
	, m_vecIndices(_rOther.m_vecIndices)
	, m_vecVerticesCount(_rOther.m_vecVerticesCount)
	, m_vecIndicesCount(_rOther.m_vecIndicesCount)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CMesh::Ready_Meshes_Clone(void * _pArg)
{
	return S_OK;
}

EMeshType & CMesh::Get_MeshType()
{
	return m_eMeshType;
}

HRESULT CMesh::Translation_ToLocalSpace(HWND _hWnd, _uint _iWinCX, _uint _iWinCY, CCamera * _pCamera, const _float4x4 * _pMatWorld)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(_hWnd, &ptMouse);

	/* 뷰포트 -> 투영스페이스 */
	_float3 vMouse = _float3(0.f, 0.f, 0.f);
	vMouse.x = ptMouse.x / (_iWinCX * 0.5f) - 1.f;
	vMouse.y = 1.f - ptMouse.y / (_iWinCY * 0.5f);

	/* 투영스페이스 -> 뷰스페이스 */
	_float4x4 matProj = _pCamera->Get_CameraDesc().matProj;
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMouse, &vMouse, &matProj);

	m_vRayPos = _float3(0.f, 0.f, 0.f);
	m_vRayDir = vMouse - m_vRayPos;

	/* 뷰스페이스 -> 월드스페이스 */
	_float4x4 matView = _pCamera->Get_CameraDesc().matView;
	D3DXMatrixInverse(&matView, 0, &matView);
	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &matView);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matView);

	/* 월드스페이스 -> 로컬스페이스 */
	_float4x4 matInvWorld;
	D3DXMatrixInverse(&matInvWorld, 0, _pMatWorld);
	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &matInvWorld);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matInvWorld);

	return S_OK;
}

void CMesh::Free()
{
	Safe_Release(m_pDevice);

	if (!m_bClone)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_vMax);
		Safe_Delete_Array(m_vMin);

		for (auto& iter : m_vecVertices)
		{
			Safe_Delete_Array(iter);
		}
		m_vecVertices.clear();

		for (auto& iter : m_vecIndices)
		{
			Safe_Delete_Array(iter);
		}
		m_vecIndices.clear();
	}

	

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}

