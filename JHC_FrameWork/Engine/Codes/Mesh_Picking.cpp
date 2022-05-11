#include "Mesh.h"

USING(Engine)

template <typename T, typename I>
_bool CMesh::Picking_Mesh(_float3* _pOut, EPickingDir* _ePickingDir, _float* _fDir, HWND _hWnd, _uint _iWinCX, _uint _iWinCY, class CCamera* _pCamera, const _float4x4* _pMatWrold)
{
	if (FAILED(Translation_ToLocalSpace(_hWnd, _iWinCX, _iWinCY, _pCamera, _pMatWrold)))
	{
		ERROR_BOX(L"Failed To Translation_ToLocalSpace In IsMousePicking");
		return false;
	}

	T* pVertices = (T*)m_pVtxPos;
	if (nullptr == pVertices)
	{
		ERROR_BOX(L"Failed To pVertices Is nullptr In IsMousePicking");
		return false;
	}

	I* pIndices = (I*)m_pIndices;
	if (nullptr == pIndices)
	{
		ERROR_BOX(L"Failed To pIndices Is nullptr In IsMousePicking");
		return false;
	}

	_float3 v1, v2, v3;
	_float U, V, Dist;
	_float m_fDir = 9999.f;
	_bool m_bPicking = false;
	for (_long i = 0; i < m_iTriCount; ++i)
	{
		v1 = pVertices[pIndices[i]._1];
		v2 = pVertices[pIndices[i]._2];
		v3 = pVertices[pIndices[i]._3];

		if (D3DXIntersectTri(&v1, &v2, &v3, &m_vRayPos, &m_vRayDir, &U, &V, &Dist))
		{
			if (m_fDir > Dist)
			{
				*_pOut = v1 + U * (v2 - v1) + V * (v3 - v1);
				D3DXVec3TransformCoord(_pOut, _pOut, _pMatWrold);

				_float3 vu = v2 - v1;
				_float3 vv = v3 - v1;
				_float3 vNormal = *D3DXVec3Cross(&vNormal, &vu, &vv);

				if (D3DXVec3Dot(&(-m_vRayDir), &vNormal) < 0)
					continue;

				if (0 != vNormal.x)
				{
					if (vNormal.x < 0)
						*_ePickingDir = EPickingDir::_X;
					else
						*_ePickingDir = EPickingDir::X;
				}

				if (0 != vNormal.y)
				{
					if (vNormal.y < 0)
						*_ePickingDir = EPickingDir::_Y;
					else
						*_ePickingDir = EPickingDir::Y;
				}

				if (0 != vNormal.z)
				{
					if (vNormal.z < 0)
						*_ePickingDir = EPickingDir::_Z;
					else
						*_ePickingDir = EPickingDir::Z;
				}

				m_fDir = Dist;
				m_bPicking = true;
			}
		}
	}

	if (m_bPicking)
	{
		*_fDir = m_fDir;
		return true;
	}

	return false;
}

template<typename T, typename I>
_bool CMesh::Picking_Vertex(_float3* _pOut, EPickingDir* _ePickingDir, _float* _fDir, HWND _hWnd, _uint _iWinCX, _uint _iWinCY, CCamera* _pCamera, const _float4x4* _pMatWrold)
{
	if (FAILED(Translation_ToLocalSpace(_hWnd, _iWinCX, _iWinCY, _pCamera, _pMatWrold)))
	{
		ERROR_BOX(L"Failed To Translation_ToLocalSpace In IsMousePicking");
		return false;
	}

	T* pVertices = (T*)m_pVtxPos;
	if (nullptr == pVertices)
	{
		ERROR_BOX(L"Failed To pVertices Is nullptr In IsMousePicking");
		return false;
	}

	I* pIndices = (I*)m_pIndices;
	if (nullptr == pIndices)
	{
		ERROR_BOX(L"Failed To pIndices Is nullptr In IsMousePicking");
		return false;
	}

	_float3 v1, v2, v3;
	_float U, V, Dist;
	_float m_fDir = 9999.f;
	_bool m_bPicking = false;
	for (_long i = 0; i < m_iTriCount; ++i)
	{
		v1 = pVertices[pIndices[i]._1];
		v2 = pVertices[pIndices[i]._2];
		v3 = pVertices[pIndices[i]._3];

		if (D3DXIntersectTri(&v1, &v2, &v3, &m_vRayPos, &m_vRayDir, &U, &V, &Dist))
		{
			if (m_fDir > Dist)
			{
				*_pOut = v1 + U * (v2 - v1) + V * (v3 - v1);
				D3DXVec3TransformCoord(_pOut, _pOut, _pMatWrold);

				_float3 vu = v2 - v1;
				_float3 vv = v3 - v1;
				_float3 vNormal = *D3DXVec3Cross(&vNormal, &vu, &vv);

				if (D3DXVec3Dot(&(-m_vRayDir), &vNormal) < 0)
					continue;

				if (0 != vNormal.x)
				{
					if (vNormal.x < 0)
						*_ePickingDir = EPickingDir::_X;
					else
						*_ePickingDir = EPickingDir::X;
				}

				if (0 != vNormal.y)
				{
					if (vNormal.y < 0)
						*_ePickingDir = EPickingDir::_Y;
					else
						*_ePickingDir = EPickingDir::Y;
				}

				if (0 != vNormal.z)
				{
					if (vNormal.z < 0)
						*_ePickingDir = EPickingDir::_Z;
					else
						*_ePickingDir = EPickingDir::Z;
				}

				m_fDir = Dist;
				m_bPicking = true;
			}
		}
	}

	if (m_bPicking)
	{
		*_fDir = m_fDir;
		return true;
	}

	return false;


	return _bool();
}
