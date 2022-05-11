#include "VIBuffer_TriColor.h"

USING(Engine)

CVIBuffer_TriColor::CVIBuffer_TriColor(_DEVICE _pDevice, _COLOR _Color)
	: CVIBuffer(_pDevice)
	, m_tColor(_Color)
{
}

CVIBuffer_TriColor::CVIBuffer_TriColor(const CVIBuffer_TriColor & _rOther)
	: CVIBuffer(_rOther)
	, m_tColor(_rOther.m_tColor)
{
}

HRESULT CVIBuffer_TriColor::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iVertexCount = 3;
	m_iTriCount = 1;
	m_iFVF = VTX_COLOR_FVF;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_Component_Prototype");
		return E_FAIL;
	}

	VTX_COLOR* pVertices = nullptr;
	/* Lock, Unlock 사이에 버텍스를 조작. 무거운 작업이므로 매 프레임마다 수행하는건 바람직하지 않다.*/
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPos = _float3(0.f, 1.f, 0.f);
	pVertices[0].iColor = m_tColor;
	pVertices[1].vPos = _float3(1.f, -1.f, 0.f);
	pVertices[1].iColor = m_tColor;
	pVertices[2].vPos = _float3(-1.f, -1.f, 0.f);
	pVertices[2].iColor = m_tColor;

	m_pVB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_TriColor::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_Component");
		return E_FAIL;
	}

	return S_OK;
}

_uint CVIBuffer_TriColor::Update_Component(const _float& _fDeltaTime)
{
	return _uint();
}

HRESULT CVIBuffer_TriColor::Render_VIBuffer(_EFFECT* _pEffect)
{
	if (FAILED(CVIBuffer::Render_VIBuffer(_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	/* 버텍스를 가지고 도형을 그리는 함수 */
	//return m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iTriCount);
	return S_OK;
}

CVIBuffer_TriColor * CVIBuffer_TriColor::Create(_DEVICE _pDevice, _COLOR _Color)
{
	CVIBuffer_TriColor* pInstance = new CVIBuffer_TriColor(_pDevice, _Color);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TriColor::Clone(void * _pArg)
{
	CVIBuffer_TriColor* pClone = new CVIBuffer_TriColor(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_TriColor::Free()
{
	CVIBuffer::Free();
}
