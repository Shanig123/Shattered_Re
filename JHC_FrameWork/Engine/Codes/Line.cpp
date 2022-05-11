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
		m_iVertexSize * m_iVertexCount,		// ���ؽ� �迭�� �޸� �� ������
		0,									// D3DUSAGE_DYNAMIC(��������)�� �ƴ� ��� ��� ��������
		m_iFVF,
		D3DPOOL_MANAGED,					// �޸� ���� ���. D3DUSAGE_DYNAMIC�� �� ��� D3DPOOL_DEFAULT�� ����ؾ���.
		&m_pVB,								// ������ ���ؽ� ���� �İ�ü�� �ּҸ� ��ȯ
		0
	)))
	{
		ERROR_BOX(L"Failed To CreateVertexBuffer");
		return E_FAIL;
	}

	VTX_COLOR* pVertices = nullptr;
	/* Lock, Unlock ���̿� ���ؽ��� ����. ���ſ� �۾��̹Ƿ� �� �����Ӹ��� �����ϴ°� �ٶ������� �ʴ�.*/
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
		0,	// �ؽ�ó�� �뵵
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
