#include "Line.h"

USING(Engine)

CLine::CLine(_DEVICE _pDevice, _float3 _vDist, _COLOR _Color)
	: CComponent(_pDevice)
	, m_vDist(_vDist)
	, m_Color(_Color)
{
}

CLine::CLine(const CLine & _rOther)
	: CComponent(_rOther)
{
}

HRESULT CLine::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iVertexCount = 6;
	m_iFVF = VTX_COLOR_FVF;

	if (FAILED(m_pDevice->CreateVertexBuffer
	(
		m_iVertexSize * m_iVertexCount,		// 버텍스 배열의 메모리 총 사이즈
		0,									// D3DUSAGE_DYNAMIC(동적버퍼)이 아닌 경우 모두 정적버퍼
		m_iFVF,
		D3DPOOL_MANAGED,					// 메모리 보관 방식. D3DUSAGE_DYNAMIC을 쓸 경우 D3DPOOL_DEFAULT을 사용해야함.
		&m_pVB,								// 생성된 버텍스 버퍼 컴객체의 주소를 반환
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateVertexBuffer");
		return E_FAIL;
	}

	VTX_COLOR* pVertices = nullptr;
	/* Lock, Unlock 사이에 버텍스를 조작. 무거운 작업이므로 매 프레임마다 수행하는건 바람직하지 않다.*/
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPos = _float3(0.f, 0.f, 0.f);
	pVertices[0].iColor = m_Color;
	pVertices[1].vPos = _float3(m_vDist);
	pVertices[1].iColor = m_Color;

	m_pVB->Unlock();

	D3DXCreateTexture(
		m_pDevice,
		1,
		1,
		1, // miplevel
		0,	// 텍스처의 용도
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&m_pTexture);

	D3DLOCKED_RECT LockRect;

	m_pTexture->LockRect(0, &LockRect, NULL, 0);
	*((_ulong*)LockRect.pBits) = m_Color;
	m_pTexture->UnlockRect(0);

	return S_OK;
}

HRESULT CLine::Ready_Component_Clone(void * _pArg)
{
	return S_OK;
}

_uint CLine::Update_Component(const _float & _fDeltaTime)
{
	return _uint();
}

HRESULT CLine::Render_Line(_EFFECT & _pEffect, _float4x4* _matWorld)
{
#ifndef _AFXDLL

	_float4x4 matWorld, matView, matProj;

	matWorld = *_matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);

	_pEffect->SetTexture("g_BaseTexture", m_pTexture);
	_pEffect->CommitChanges();

	if (FAILED(m_pDevice->SetTransform(D3DTS_WORLD, _matWorld)))
	{
		ERROR_BOX(L"Failed To SetStreamSource");
		return E_FAIL;
	}
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

	return m_pDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_iVertexCount);
#else
	if (FAILED(m_pDevice->SetTransform(D3DTS_WORLD, _matWorld)))
	{
		ERROR_BOX(L"Failed To SetStreamSource");
		return E_FAIL;
	}
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

	return m_pDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_iVertexCount);
#endif // !_AFXDLL

}

HRESULT CLine::Render_Line(_float4x4 * _matWorld)
{
	if (FAILED(m_pDevice->SetTransform(D3DTS_WORLD, _matWorld)))
	{
		ERROR_BOX(L"Failed To SetStreamSource");
		return E_FAIL;
	}

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

	return m_pDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_iVertexCount);
}

CLine * CLine::Create(_DEVICE _pDevice, _float3 _vDist, _COLOR _Color)
{
	CLine* pInstance = new CLine(_pDevice, _vDist, _Color);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CLine::Clone(void * _pArg)
{
	return nullptr;
}

void CLine::Free()
{
	Safe_Release(m_pVB);
	Safe_Release(m_pTexture);

	CComponent::Free();
}
