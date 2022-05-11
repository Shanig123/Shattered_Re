#include "VIBuffer_RectColor.h"

USING(Engine)

CVIBuffer_RectColor::CVIBuffer_RectColor(_DEVICE _pDevice, _COLOR _Color)
	: CVIBuffer(_pDevice)
	, m_tColor(_Color)
{
}

CVIBuffer_RectColor::CVIBuffer_RectColor(const CVIBuffer_RectColor & _rOther)
	: CVIBuffer(_rOther)
	, m_tColor(_rOther.m_tColor)
{
}

HRESULT CVIBuffer_RectColor::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iVertexCount = 4;
	m_iTriCount = 2;
	m_iFVF = VTX_COLOR_FVF;

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

	pVertices[0].vPos = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].iColor = m_tColor;
	pVertices[1].vPos = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].iColor = m_tColor;
	pVertices[2].vPos = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].iColor = m_tColor;
	pVertices[3].vPos = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].iColor = m_tColor;

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

HRESULT CVIBuffer_RectColor::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Ready_Component_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CVIBuffer_RectColor::Update_Component(const _float& _fDeltaTime)
{
	return _uint();
}

HRESULT CVIBuffer_RectColor::Render_VIBuffer(_EFFECT* _pEffect)
{
	if (FAILED(CVIBuffer::Render_VIBuffer(_pEffect)))
	{
		ERROR_BOX(L"Failed To Render_VIBuffer");
		return E_FAIL;
	}
	/* 버텍스를 가지고 도형을 그리는 함수 */
	//return m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexCount, 0, m_iTriCount);
	return S_OK;
}

CVIBuffer_RectColor * CVIBuffer_RectColor::Create(_DEVICE _pDevice, _COLOR _Color)
{
	CVIBuffer_RectColor* pInstance = new CVIBuffer_RectColor(_pDevice, _Color);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectColor::Clone(void * _pArg)
{
	CVIBuffer_RectColor* pClone = new CVIBuffer_RectColor(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_RectColor::Free()
{
	CVIBuffer::Free();
}
