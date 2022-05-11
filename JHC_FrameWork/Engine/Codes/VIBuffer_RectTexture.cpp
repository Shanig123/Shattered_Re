#include "VIBuffer_RectTexture.h"

USING(Engine)

CVIBuffer_RectTexture::CVIBuffer_RectTexture(_DEVICE _pDevice)
	: CVIBuffer(_pDevice)
{
}

CVIBuffer_RectTexture::CVIBuffer_RectTexture(const CVIBuffer_RectTexture & _rOther)
	: CVIBuffer(_rOther)
{
}

HRESULT CVIBuffer_RectTexture::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_TEXTURE);
	m_iVertexCount = 4;
	m_iTriCount = 2;
	m_iFVF = VTX_TEXTURE_FVF;

	m_iIndexSize = sizeof(INDEX16);
	m_tIndexFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Ready_Component_Prototype");
		return E_FAIL;
	}

	//if (FAILED(CVIBuffer::Ready_Component_Dynamic()))
	//{
	//	ERROR_BOX(L"Failed To Ready_Component_Prototype");
	//	return E_FAIL;
	//}

	//if (FAILED(CVIBuffer::Ready_Component_Dynamic_WriteOnly()))
	//{
	//	ERROR_BOX(L"Failed To Ready_Component_Prototype");
	//	return E_FAIL;
	//}
	VTX_TEXTURE* pVertices = nullptr;
	/* Lock, Unlock 사이에 버텍스를 조작. 무거운 작업이므로 매 프레임마다 수행하는건 바람직하지 않다.*/
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPos = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vUV = _float2(0.f, 0.f);
	pVertices[1].vPos = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vUV = _float2(1.f, 0.f);
	pVertices[2].vPos = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vUV = _float2(1.f, 1.f);
	pVertices[3].vPos = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vUV = _float2(0.f, 1.f);

	m_pVertices = new VTX_TEXTURE[m_iVertexCount];
	memcpy(m_pVertices, pVertices, m_iVertexSize * m_iVertexCount);

	m_pVB->Unlock();

	INDEX16* pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._1 = 0;
	pIndices[0]._2 = 1;
	pIndices[0]._3 = 2;

	pIndices[1]._1 = 0;
	pIndices[1]._2 = 2;
	pIndices[1]._3 = 3;

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_RectTexture::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Ready_Component_Clone");
		return E_FAIL;
	}

	return S_OK;
}


CVIBuffer_RectTexture * CVIBuffer_RectTexture::Create(_DEVICE _pDevice)
{
	CVIBuffer_RectTexture* pInstance = new CVIBuffer_RectTexture(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectTexture::Clone(void * _pArg)
{
	CVIBuffer_RectTexture* pClone = new CVIBuffer_RectTexture(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pClone);
	}

	return pClone;
}
