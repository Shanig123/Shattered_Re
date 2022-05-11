#include "VIBuffer_CubeTexture.h"

USING(Engine)

CVIBuffer_CubeTexture::CVIBuffer_CubeTexture(_DEVICE _pDevice)
	: CVIBuffer(_pDevice)
{
}

CVIBuffer_CubeTexture::CVIBuffer_CubeTexture(const CVIBuffer_CubeTexture & _rOther)
	: CVIBuffer(_rOther)
{
}

HRESULT CVIBuffer_CubeTexture::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_CUBETEXTURE);
	m_iVertexCount = 8;
	m_iTriCount = 12;
	m_iFVF = VTX_CUBETEXTURE_FVF;

	m_iIndexSize = sizeof(INDEX16);
	m_tIndexFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Ready_Component_Prototype");
		return E_FAIL;
	}

	VTX_CUBETEXTURE* pVertices = nullptr;
	/* Lock, Unlock 사이에 버텍스를 조작. 무거운 작업이므로 매 프레임마다 수행하는건 바람직하지 않다.*/
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPos = _float3(-0.5f, 0.5f, -0.5f); //min
	pVertices[0].vUV = pVertices[0].vPos;

	pVertices[1].vPos = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vUV = pVertices[1].vPos;

	pVertices[2].vPos = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vUV = pVertices[2].vPos;

	pVertices[3].vPos = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vUV = pVertices[3].vPos;


	pVertices[4].vPos = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vUV = pVertices[4].vPos;

	pVertices[5].vPos = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vUV = pVertices[5].vPos;

	pVertices[6].vPos = _float3(0.5f, -0.5f, 0.5f); // max
	pVertices[6].vUV = pVertices[6].vPos;

	pVertices[7].vPos = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vUV = pVertices[7].vPos;

	m_pVertices = new VTX_CUBETEXTURE[m_iVertexCount];
	memcpy(m_pVertices, pVertices, m_iVertexSize * m_iVertexCount);

	m_pVB->Unlock();

	INDEX16* pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	// +x
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 5;
	pIndices[0]._3 = 6;

	pIndices[1]._1 = 1;
	pIndices[1]._2 = 6;
	pIndices[1]._3 = 2;

	// -x
	pIndices[2]._1 = 4;
	pIndices[2]._2 = 0;
	pIndices[2]._3 = 3;

	pIndices[3]._1 = 4;
	pIndices[3]._2 = 3;
	pIndices[3]._3 = 7;

	// +y
	pIndices[4]._1 = 4;
	pIndices[4]._2 = 5;
	pIndices[4]._3 = 1;

	pIndices[5]._1 = 4;
	pIndices[5]._2 = 1;
	pIndices[5]._3 = 0;

	// -y
	pIndices[6]._1 = 3;
	pIndices[6]._2 = 2;
	pIndices[6]._3 = 6;

	pIndices[7]._1 = 3;
	pIndices[7]._2 = 6;
	pIndices[7]._3 = 7;

	// +z
	pIndices[8]._1 = 7;
	pIndices[8]._2 = 6;
	pIndices[8]._3 = 5;

	pIndices[9]._1 = 7;
	pIndices[9]._2 = 5;
	pIndices[9]._3 = 4;

	// -z
	pIndices[10]._1 = 0;
	pIndices[10]._2 = 1;
	pIndices[10]._3 = 2;

	pIndices[11]._1 = 0;
	pIndices[11]._2 = 2;
	pIndices[11]._3 = 3;

	m_pIndices = new INDEX16[m_iTriCount];
	memcpy(m_pIndices, pIndices, m_iIndexSize * m_iTriCount);

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_CubeTexture::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Ready_Component_Clone");
		return E_FAIL;
	}

	return S_OK;
}

CVIBuffer_CubeTexture * CVIBuffer_CubeTexture::Create(_DEVICE _pDevice)
{
	CVIBuffer_CubeTexture* pInstance = new CVIBuffer_CubeTexture(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_CubeTexture::Clone(void * _pArg)
{
	CVIBuffer_CubeTexture* pClone = new CVIBuffer_CubeTexture(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pClone);
	}

	return pClone;
}
