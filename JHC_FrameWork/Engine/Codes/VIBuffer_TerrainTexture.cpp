#include "VIBuffer_TerrainTexture.h"

USING(Engine)

CVIBuffer_TerrainTexture::CVIBuffer_TerrainTexture(_DEVICE _pDevice, wstring _wstrFilePath, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval)
	: CVIBuffer(_pDevice)
	, m_wstrFilePath(_wstrFilePath)
	, m_iVertexCountX(_iVertexCountX)
	, m_iVertexCountZ(_iVertexCountZ)
	, m_fVertexInterval(_fVertexInterval)
{
}

CVIBuffer_TerrainTexture::CVIBuffer_TerrainTexture(const CVIBuffer_TerrainTexture & _rOther)
	: CVIBuffer(_rOther)
	, m_wstrFilePath(_rOther.m_wstrFilePath)
	, m_iVertexCountX(_rOther.m_iVertexCountX)
	, m_iVertexCountZ(_rOther.m_iVertexCountZ)
	, m_fVertexInterval(_rOther.m_fVertexInterval)
{
}

HRESULT CVIBuffer_TerrainTexture::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_TEXTURE);
	m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;
	m_iFVF = VTX_TEXTURE_FVF;

	m_iIndexSize = sizeof(INDEX16);
	m_tIndexFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Ready_Component_Prototype");
		return E_FAIL;
	}

	_uint* pPixels = Load_HeightMap(m_wstrFilePath);
	if (nullptr == pPixels)
	{
		ERROR_BOX(L"pPixels Is nullptr");
		return E_FAIL;
	}

	VTX_TEXTURE* pVertices = nullptr;
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
			_float fUVX = X / (m_iVertexCountX - 1.f);
			_float fUVZ = 1.f - (Z / (m_iVertexCountZ - 1.f));
			
			_float fY = (pPixels[iIndex] & D3DCOLOR_ARGB(0, 0, 0, 255)) / 100.f;

			if (m_iVertexCount <= iIndex)
				break;

			pVertices[iIndex].vPos = _float3(fX, fY, fZ);
			pVertices[iIndex].vUV = _float2(fUVX, fUVZ);
		}
	}

	m_pVertices = new VTX_TEXTURE[m_iVertexCount];
	memcpy(m_pVertices, pVertices, m_iVertexSize * m_iVertexCount);

	m_pVB->Unlock();

	Safe_Delete_Array(pPixels);

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

	m_pIndices = new INDEX16[m_iTriCount];
	memcpy(m_pIndices, pIndices, m_iIndexSize * m_iTriCount);

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_TerrainTexture::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Ready_Component_Clone");
		return E_FAIL;
	}

	return S_OK;
}


_bool CVIBuffer_TerrainTexture::IsOnTerrain(_float3 * _pTargetPos)
{
	_int iRow = _int(_pTargetPos->z / m_fVertexInterval);
	_int iCol = _int(_pTargetPos->x / m_fVertexInterval);
	_int iIndex = _int(iRow * m_iVertexCountX + iCol);

	D3DXPLANE tPlane;
	VTX_TEXTURE* pVertices = (VTX_TEXTURE*)m_pVertices;
	if (nullptr == pVertices)
	{
		ERROR_BOX(L"pVertices Is nullptr");
		return false;
	}

	_float fRatioX = fabs(_pTargetPos->x - pVertices[iIndex + m_iVertexCountX].vPos.x);
	_float fRatioZ = fabs(_pTargetPos->z - pVertices[iIndex + m_iVertexCountX].vPos.z);

	if (fRatioZ < fRatioX)	/* 오른쪽 삼각형 밟고 있는 경우 */
	{
		D3DXPlaneFromPoints
		(
			&tPlane,
			&pVertices[iIndex + m_iVertexCountX].vPos,
			&pVertices[iIndex + m_iVertexCountX + 1].vPos,
			&pVertices[iIndex + 1].vPos
		);
	}
	else	/* 왼쪽 삼각형 밟고 있는 경우 */
	{
		D3DXPlaneFromPoints
		(
			&tPlane,
			&pVertices[iIndex + m_iVertexCountX].vPos,
			&pVertices[iIndex + 1].vPos,
			&pVertices[iIndex].vPos
		);
	}
	
	/* 
	평면의 방정식
	ax + by + cz + d = 0
	by = -(ax + cz + d)
	y = -(ax + cz + d) / b;
	*/
	_pTargetPos->y = -(_pTargetPos->x * tPlane.a + _pTargetPos->z * tPlane.c + tPlane.d) / tPlane.b;
	_pTargetPos->y += 0.5f;

	return true;
}

_uint * CVIBuffer_TerrainTexture::Load_HeightMap(wstring _wstrFilePath)
{
	HANDLE hFile = CreateFile(_wstrFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		ERROR_BOX(L"Failed To Load_HeightMap");
		return nullptr;
	}

	DWORD dwBytes = 0;
	BITMAPFILEHEADER tBmpFileHeader;
	ReadFile(hFile, &tBmpFileHeader, sizeof(BITMAPFILEHEADER), &dwBytes, 0);
	BITMAPINFOHEADER tBmpInfoHeader;
	ReadFile(hFile, &tBmpInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytes, 0);

	LONG lPixelsCount = tBmpInfoHeader.biWidth * tBmpInfoHeader.biHeight;
	_uint* pPixels = new _uint[lPixelsCount];
	if (FALSE == ReadFile(hFile, pPixels, sizeof(_uint) * lPixelsCount, &dwBytes, 0))
	{
		Safe_Delete_Array(pPixels);
	}

	CloseHandle(hFile);

	return pPixels;
}

CVIBuffer_TerrainTexture * CVIBuffer_TerrainTexture::Create(_DEVICE _pDevice, wstring _wstrFilePath, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval)
{
	CVIBuffer_TerrainTexture* pInstance = new CVIBuffer_TerrainTexture(_pDevice, _wstrFilePath, _iVertexCountX, _iVertexCountZ, _fVertexInterval);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TerrainTexture::Clone(void * _pArg)
{
	CVIBuffer_TerrainTexture* pClone = new CVIBuffer_TerrainTexture(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pClone);
	}

	return pClone;
}
