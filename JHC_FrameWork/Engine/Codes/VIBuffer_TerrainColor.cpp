#include "VIBuffer_TerrainColor.h"

USING(Engine)

CVIBuffer_TerrainColor::CVIBuffer_TerrainColor(_DEVICE _pDevice, _COLOR _Color, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval)
	: CVIBuffer(_pDevice)
	, m_tColor(_Color)
	, m_iVertexCountX(_iVertexCountX)
	, m_iVertexCountZ(_iVertexCountZ)
	, m_fVertexInterval(_fVertexInterval)
{
}

CVIBuffer_TerrainColor::CVIBuffer_TerrainColor(const CVIBuffer_TerrainColor & _rOther)
	: CVIBuffer(_rOther)
	, m_tColor(_rOther.m_tColor)
	, m_iVertexCountX(_rOther.m_iVertexCountX)
	, m_iVertexCountZ(_rOther.m_iVertexCountZ)
	, m_fVertexInterval(_rOther.m_fVertexInterval)
{
}

HRESULT CVIBuffer_TerrainColor::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;
	m_iFVF = VTX_COLOR_FVF;
	m_pPos = new _float3[m_iVertexCount];

	m_iIndexSize = sizeof(INDEX16);
	m_tIndexFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Ready_Component_Prototype");
		return E_FAIL;
	}

	VTX_COLOR* pVertices = nullptr;
	/* Lock, Unlock 사이에 버텍스를 조작. 무거운 작업이므로 매 프레임마다 수행하는건 바람직하지 않다.*/
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	_uint iIndex = 0;
	for (_uint Z = 0; Z < m_iVertexCountZ; ++Z)
	{
		for (_uint X = 0; X < m_iVertexCountX; ++X)
		{
			iIndex = Z * m_iVertexCountX + X;
			_float fX = X * m_fVertexInterval;
			_float fZ = Z * m_fVertexInterval;

			if (m_iVertexCount <= iIndex)
				break;

			pVertices[iIndex].vPos = _float3(fX, 0.f, fZ);
			pVertices[iIndex].iColor = m_tColor;
			m_pPos[iIndex] = pVertices[iIndex].vPos;
		}
	}

	m_pVB->Unlock();

	INDEX16* pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint iTriIndex = 0;
	for (_uint Z = 0; Z < m_iVertexCountZ - 1; ++Z)
	{
		for (_uint X = 0; X < m_iVertexCountX - 1; ++X)
		{
			iIndex = Z * m_iVertexCountX + X;

			pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
			pIndices[iTriIndex]._2 = iIndex + m_iVertexCountX + 1;
			pIndices[iTriIndex++]._3 = iIndex + 1;

			pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
			pIndices[iTriIndex]._2 = iIndex + 1;
			pIndices[iTriIndex++]._3 = iIndex;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_TerrainColor::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Ready_Component_Clone");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer_TerrainColor::Reset_VertexCount(_uint _iVertexCountX, _uint _iVertexCountZ)
{
	m_iVertexCountX = _iVertexCountX;
	m_iVertexCountZ = _iVertexCountZ;

	if (FAILED(Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Reset_VertexCount");
		return E_FAIL;
	}

	return S_OK;
}

CVIBuffer_TerrainColor * CVIBuffer_TerrainColor::Create(_DEVICE _pDevice, _COLOR _Color, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval)
{
	CVIBuffer_TerrainColor* pInstance = new CVIBuffer_TerrainColor(_pDevice, _Color, _iVertexCountX, _iVertexCountZ, _fVertexInterval);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TerrainColor::Clone(void * _pArg)
{
	CVIBuffer_TerrainColor* pClone = new CVIBuffer_TerrainColor(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_TerrainColor::Free()
{
	if(false == m_bClone)
		Safe_Delete_Array(m_pPos);

	CVIBuffer::Free();
}
