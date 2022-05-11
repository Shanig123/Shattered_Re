#include "VIBuffer_RectTextureDynamic.h"


USING(Engine)

CVIBuffer_RectTextureDynamic::CVIBuffer_RectTextureDynamic(_DEVICE _pDevice)
	:CVIBuffer(_pDevice)
	, m_fVertexInterval(0.f)
{
}

CVIBuffer_RectTextureDynamic::CVIBuffer_RectTextureDynamic(const CVIBuffer_RectTextureDynamic & _rhs)
	: CVIBuffer(_rhs)
	, m_iVertexCountX(_rhs.m_iVertexCountX)
	, m_iVertexCountZ(_rhs.m_iVertexCountZ)
	, m_fVertexInterval(_rhs.m_fVertexInterval)
{
}

CVIBuffer_RectTextureDynamic::~CVIBuffer_RectTextureDynamic()
{
}

HRESULT CVIBuffer_RectTextureDynamic::Ready_Component_Prototype()
{
	//m_iVertexCountX = 12;
	////m_iVertexCountX = 6;
	//m_iVertexCountZ = 2;
	//m_fVertexInterval = 0.5f;
	//m_iVertexSize = sizeof(VTX_TEXTURE);
	////m_iVertexSize = sizeof(VTX_TEXTURE_COLOR);


	//m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	//m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;
	//m_iFVF = VTX_TEXTURE_FVF;
	////m_iFVF = VTX_TEXTURE_COLOR_FVF;

	//m_iIndexSize = sizeof(INDEX16);
	//m_tIndexFmt = D3DFMT_INDEX16;

	//if (FAILED(CVIBuffer::Ready_Component_Dynamic()))
	//{
	//	ERROR_BOX(L"Failed To Ready_Component_Prototype");
	//	return E_FAIL;
	//}
	////VTX_TEXTURE_COLOR*  pVertices = nullptr;
	//VTX_TEXTURE* pVertices = nullptr;
	///* Lock, Unlock 사이에 버텍스를 조작. 무거운 작업이므로 매 프레임마다 수행하는건 바람직하지 않다.*/
	//m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	//_uint iIndex = 0;


	//for (_uint i = 0; i < m_iVertexCount; ++i)
	//{
	//	_float fX = (i/2) * m_fVertexInterval;
	//	_float fZ = (i%2) * m_fVertexInterval;
	//	_float fUVX = (m_iVertexCount>>1) / (m_iVertexCountX - 1.f);
	//	_float fUVZ = (i % 2);

	//	_float fY = 0.f;

	//	pVertices[iIndex].vPos = _float3(fX, fY, fZ);
	//	pVertices[iIndex].vUV = _float2(fUVX, fUVZ);
	//}

	//m_pVertices = new VTX_TEXTURE[m_iVertexCount];
	//memcpy(m_pVertices, pVertices, m_iVertexSize * m_iVertexCount);

	//m_pVB->Unlock();

	//INDEX16* pIndices = nullptr;

	//m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	//_uint iTriIndex = 0;


	////for (_uint i = 0; i < m_iVertexCount; ++i)
	////{
	////	//_float fX = (i / 2) * m_fVertexInterval;
	////	//_float fZ = (i % 2) * m_fVertexInterval;
	////	//_float fUVX = (m_iVertexCount >> 1) / (m_iVertexCountX - 1.f);
	////	//_float fUVZ = (i % 2);

	////	//_float fY = 0.f;

	////	//pVertices[iIndex].vPos = _float3(fX, fY, fZ);
	////	//pVertices[iIndex].vUV = _float2(fUVX, fUVZ);
	////	pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
	////	pIndices[iTriIndex]._2 = iIndex + m_iVertexCountX + 1;
	////	pIndices[iTriIndex++]._3 = iIndex + 1;

	////	pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
	////	pIndices[iTriIndex]._2 = iIndex + 1;
	////	pIndices[iTriIndex++]._3 = iIndex;
	////}

	////for (_uint Z = 0; Z < m_iVertexCountZ - 1; ++Z)
	////{
	//	for (_uint iX = 0; iX < m_iVertexCountX-1; ++iX)
	//	{
	//		iIndex = iX*2;

	//		pIndices[iTriIndex]._1 = iIndex;
	//		pIndices[iTriIndex]._2 = iIndex+1;
	//		pIndices[iTriIndex++]._3 = iIndex + 2;

	//		pIndices[iTriIndex]._1 = iIndex+1;
	//		pIndices[iTriIndex]._2 = iIndex + 3;
	//		pIndices[iTriIndex++]._3 = iIndex + 2;
	//	}
	////}

	//m_pIndices = new INDEX16[m_iTriCount];
	//memcpy(m_pIndices, pIndices, m_iIndexSize * m_iTriCount);

	//m_pIB->Unlock();

	return S_OK;
}
HRESULT CVIBuffer_RectTextureDynamic::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Ready_Component_Clone");
		return E_FAIL;
	}
	m_iVertexCountX = 24;
	m_iVertexCountZ = 2;
	m_fVertexInterval = 0.5f;
	//m_iVertexSize = sizeof(VTX_TEXTURE);
	m_iVertexSize = sizeof(VTX_TEXTURE_COLOR);


	m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;
	//m_iFVF = VTX_TEXTURE_FVF;
	m_iFVF = VTX_TEXTURE_COLOR_FVF;

	m_iIndexSize = sizeof(INDEX16);
	m_tIndexFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Dynamic()))
	{
		ERROR_BOX(L"Failed To Ready_Component_Prototype");
		return E_FAIL;
	}
	VTX_TEXTURE_COLOR*  pVertices = nullptr;
	//VTX_TEXTURE* pVertices = nullptr;
	/* Lock, Unlock 사이에 버텍스를 조작. 무거운 작업이므로 매 프레임마다 수행하는건 바람직하지 않다.*/
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	_uint iIndex = 0;


	for (_uint i = 0; i < m_iVertexCount; ++i)
	{
		_float fX = (i / 2) * m_fVertexInterval;
		_float fZ = (i % 2) * m_fVertexInterval;
		_float fUVX = (m_iVertexCount >> 1) / (m_iVertexCountX - 1.f);
		_float fUVZ = (_float)(i % 2);

		_float fY = 0.f;

		pVertices[iIndex].vPos = _float3(fX, fY, fZ);
		pVertices[iIndex].vUV = _float2(fUVX, fUVZ);
	}

	//m_pVertices = new VTX_TEXTURE[m_iVertexCount];
	//memcpy(m_pVertices, pVertices, m_iVertexSize * m_iVertexCount);

	m_pVB->Unlock();

	INDEX16* pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	_uint iTriIndex = 0;


	//for (_uint i = 0; i < m_iVertexCount; ++i)
	//{
	//	//_float fX = (i / 2) * m_fVertexInterval;
	//	//_float fZ = (i % 2) * m_fVertexInterval;
	//	//_float fUVX = (m_iVertexCount >> 1) / (m_iVertexCountX - 1.f);
	//	//_float fUVZ = (i % 2);

	//	//_float fY = 0.f;

	//	//pVertices[iIndex].vPos = _float3(fX, fY, fZ);
	//	//pVertices[iIndex].vUV = _float2(fUVX, fUVZ);
	//	pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
	//	pIndices[iTriIndex]._2 = iIndex + m_iVertexCountX + 1;
	//	pIndices[iTriIndex++]._3 = iIndex + 1;

	//	pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
	//	pIndices[iTriIndex]._2 = iIndex + 1;
	//	pIndices[iTriIndex++]._3 = iIndex;
	//}

	//for (_uint Z = 0; Z < m_iVertexCountZ - 1; ++Z)
	//{
	for (_uint iX = 0; iX < m_iVertexCountX - 1; ++iX)
	{
		iIndex = iX * 2;

		pIndices[iTriIndex]._1 = iIndex;
		pIndices[iTriIndex]._2 = iIndex + 1;
		pIndices[iTriIndex++]._3 = iIndex + 2;

		pIndices[iTriIndex]._1 = iIndex + 1;
		pIndices[iTriIndex]._2 = iIndex + 3;
		pIndices[iTriIndex++]._3 = iIndex + 2;
	}
	//}

	//m_pIndices = new INDEX16[m_iTriCount];
	//memcpy(m_pIndices, pIndices, m_iIndexSize * m_iTriCount);

	m_pIB->Unlock();
	return S_OK;
}

HRESULT CVIBuffer_RectTextureDynamic::Render_VIBuffer(_EFFECT * _pEffect)
{
	if (FAILED(m_pDevice->SetStreamSource(0, m_pVB, 0, m_iVertexSize)))
	{
		ERROR_BOX(L"Failed To SetStreamSource");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->SetFVF(m_iFVF)))
	{
		ERROR_BOX(L"Failed To SetFVF");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->SetIndices(m_pIB)))
	{
		ERROR_BOX(L"Failed To SetIndices");
		return E_FAIL;
	}
	//D3DPT_TRIANGLESTRIP
	//D3DPT_TRIANGLELIST
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexCount, 0, m_iTriCount);
	if (_pEffect != nullptr)
	{
		(*_pEffect)->CommitChanges();
	}
	return S_OK;
}

HRESULT CVIBuffer_RectTextureDynamic::SetTrailNode(const list<TRAIL_NODE>& _listNode, const _uint & _dwCnt)
{
//	VTX_TEXTURE* pVertices = nullptr;
	VTX_TEXTURE_COLOR* pVertices = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_NOOVERWRITE);
	//_uint iNodeCount = _dwCnt;

	m_iTriCount = _dwCnt - 2;
	
	_uint i = 0;
	for (auto& iter : _listNode)
	{
		pVertices[i].vPos = iter.vPos;
		pVertices[i].iColor = D3DCOLOR_ARGB((_int)(iter.vColor.w * 255), (_int)(iter.vColor.x * 255), (_int)(iter.vColor.y * 255), (_int)(iter.vColor.z * 255));
		pVertices[i].vUV = (i % 2) ? _float2(pVertices[i - 1].vUV.x, 1.f) : _float2(i / ((_dwCnt)-1.f), 0.f);
		if (i > (m_iVertexCountX*m_iVertexCountZ) - 2)
			pVertices[i].vUV.x = 1.f;
		++i;
	}

	m_pVB->Unlock();

	return S_OK;
}

//HRESULT CVIBuffer_RectTextureDynamic::SetPos(const _float3 vPos[], const _uint & _dwCnt)
//{
//	m_pVertices;
//	VTX_TEXTURE* pVertices = nullptr;
//	m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_NOOVERWRITE);
//	_uint iNodeCount = _dwCnt;
//	//m_iTriCount = (iNodeCount << 1) - 2;
//	//_uint j = 0;
//	//for (_uint i = 0; i < (iNodeCount << 1); ++i)
//	//{
//	//	pVertices[i].vPos = vPos[j];
//	//	if (i % 2 == 0)
//	//	{
//	//		++j;
//	//	}
//	////	pVertices[i].vPos = vPos[i];
//	//	pVertices[i].vPos.z += (i % 2) ? 1.f : -1.f;
//	//	pVertices[i].vUV = (i % 2) ? _float2(1.f/((iNodeCount << 1) -1.f),0.f) : _float2((i-1.f) / ((iNodeCount << 1) - 1.f), 1.f);
//	//}
//	m_iTriCount = iNodeCount - 2;
//	_uint j = 0;
//	for (_uint i = 0; i < iNodeCount; ++i)
//	{
//		pVertices[i].vPos = vPos[i];
//
//
//		//if (i % 2)
//		//	m_pVertex[i].vTexUV = D3DXVECTOR2(1 / (dwCnt - 1.f), 0.f);
//		//else
//		//	m_pVertex[i].vTexUV = D3DXVECTOR2((i - 1) / (dwCnt - 1.f), 1.f);
//
//		pVertices[i].vUV = (i % 2) ?  _float2(pVertices[i-1].vUV.x, 1.f): _float2(i / ((iNodeCount)-1.f), 0.f);
//	}
//	
//	
//	m_pVB->Unlock();
//	
//	return S_OK;
//}


CVIBuffer_RectTextureDynamic * CVIBuffer_RectTextureDynamic::Create(_DEVICE _pDevice)
{
	CVIBuffer_RectTextureDynamic* pInstance = new CVIBuffer_RectTextureDynamic(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectTextureDynamic::Clone(void * _pArg)
{
	CVIBuffer_RectTextureDynamic* pClone = new CVIBuffer_RectTextureDynamic(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pClone);
	}

	return pClone;
}