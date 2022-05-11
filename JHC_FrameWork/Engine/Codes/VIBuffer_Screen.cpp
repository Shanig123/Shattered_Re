#include "VIBuffer_Screen.h"

USING(Engine)



CVIBuffer_Screen::CVIBuffer_Screen(_DEVICE _pDevice)
	:CVIBuffer(_pDevice)
{
}

CVIBuffer_Screen::CVIBuffer_Screen(const CVIBuffer_Screen & _rhs)
	: CVIBuffer(_rhs)
{
}

CVIBuffer_Screen::~CVIBuffer_Screen()
{
}

HRESULT CVIBuffer_Screen::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_SCREEN);
	m_iVertexCount = 4;
	m_iTriCount = 2;
	m_iFVF = VTX_SCREEN_FVF;
	m_iIndexSize = sizeof(INDEX16);

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Ready_Component_Prototype");
		return E_FAIL;
	}

	D3DVIEWPORT9			ViewPort;
	m_pDevice->GetViewport(&ViewPort);

	VTX_SCREEN*		pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPos = _float4(0.f - 0.5f, 0.f - 0.5f, 0.f, 1.f);
	pVertex[0].vUV = _float2(0.f, 0.f);

	pVertex[1].vPos = _float4(_float(ViewPort.Width) - 0.5f, 0.f - 0.5f, 0.f, 1.f);
	pVertex[1].vUV = _float2(1.f, 0.f);

	pVertex[2].vPos = _float4(_float(ViewPort.Width) - 0.5f, _float(ViewPort.Height) - 0.5f, 0.f, 1.f);
	pVertex[2].vUV = _float2(1.f, 1.f);

	pVertex[3].vPos = _float4(0.f - 0.5f, _float(ViewPort.Height) - 0.5f, 0.f, 1.f);
	pVertex[3].vUV = _float2(0.f, 1.f);

	m_pVB->Unlock();
	
	INDEX16*		pIndex = NULL;
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._1 = 0;
	pIndex[0]._2 = 1;
	pIndex[0]._3 = 2;

	pIndex[1]._1 = 0;
	pIndex[1]._2 = 2;
	pIndex[1]._3 = 3;

	m_pIB->Unlock();
	return S_OK;
}

HRESULT CVIBuffer_Screen::Ready_Component_Clone(void * _pArg)
{
	if (FAILED(CVIBuffer::Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Ready_Component_Clone");
		return E_FAIL;
	}

	return S_OK;
}


CVIBuffer_Screen * CVIBuffer_Screen::Create(_DEVICE _pDevice)
{
	CVIBuffer_Screen* pInstance = new CVIBuffer_Screen(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Screen::Clone(void * _pArg)
{
	CVIBuffer_Screen* pClone = new CVIBuffer_Screen(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed To Clone");
		Safe_Release(pClone);
	}

	return pClone;
}
