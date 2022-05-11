#include "MyRenderTarget.h"

USING(Engine)

CMyRenderTarget::CMyRenderTarget(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
	, m_pTargetSurface(nullptr)
	, m_pOldTargetSurface(nullptr)
	, m_pTargetTexture(nullptr)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
{
	m_pDevice->AddRef();
}

HRESULT CMyRenderTarget::Ready_MyRenderTarget(const _uint & _iWidth, const _uint & _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color)
{
	if (FAILED(D3DXCreateTexture
	(
		m_pDevice,
		_iWidth,
		_iHeight,
		1,					// mipmap
		D3DUSAGE_RENDERTARGET,
		_Format,
		D3DPOOL_DEFAULT,
		&m_pTargetTexture
	)))
	{
		ERROR_BOX(L"Failed to Ready_MyRenderTarget");
		return E_FAIL;
	}

	m_ClearColor = _Color;

	// 텍스처로부터 렌더타겟 요소를 추출한다. 첫번째 : 만약 밉맵을 만들었을 경우 레벨 지정이 가능하나 우리는 하나만 만들어서 0번으로 지정
	m_pTargetTexture->GetSurfaceLevel(0, &m_pTargetSurface);
	Safe_Release(m_pTargetTexture);

	return S_OK;
}

void CMyRenderTarget::Clear_MyRenderTarget()
{
	m_pDevice->GetRenderTarget(0, &m_pOldTargetSurface);
	m_pDevice->SetRenderTarget(0, m_pTargetSurface);

	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0);

	m_pDevice->SetRenderTarget(0, m_pOldTargetSurface);
	Safe_Release(m_pOldTargetSurface);
}

void CMyRenderTarget::SetUp_OnGraghicDevice(const _uint & _iIndex)
{
	m_pDevice->GetRenderTarget(_iIndex, &m_pOldTargetSurface);
	m_pDevice->SetRenderTarget(_iIndex, m_pTargetSurface);
}

void CMyRenderTarget::Release_OnGraghicDevice(const _uint & _iIndex)
{
	m_pDevice->SetRenderTarget(_iIndex, m_pOldTargetSurface);
	Safe_Release(m_pOldTargetSurface);
}

HRESULT CMyRenderTarget::Ready_DebugBuffer(const _float & _fX, const _float & _fY, const _float & _fSizeX, const _float & _fSizeY)
{
	if (FAILED(m_pDevice->CreateVertexBuffer
	(
		sizeof(VTX_SCREEN) * 4,
		0,
		VTX_SCREEN_FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		NULL
	)))
	{
		ERROR_BOX(L"Failed to CreateVertexBuffer");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateIndexBuffer
	(
		sizeof(INDEX16) * 2,
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL
	)))
	{
		ERROR_BOX(L"Failed to CreateIndexBuffer");
		return E_FAIL;
	}

	VTX_SCREEN* pVerties = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVerties, 0);

	pVerties[0].vPos = _float4(_fX - 0.5f, _fY - 0.5f, 0.f, 1.f);
	pVerties[0].vUV = _float2(0.f, 0.f);
	
	pVerties[1].vPos = _float4((_fX + _fSizeX) - 0.5f, _fY - 0.5f, 0.f, 1.f);
	pVerties[1].vUV = _float2(1.f, 0.f);
	
	pVerties[2].vPos = _float4((_fX + _fSizeX) - 0.5f, (_fY + _fSizeY) - 0.5f, 0.f, 1.f);
	pVerties[2].vUV = _float2(1.f, 1.f);
	
	pVerties[3].vPos = _float4(_fX - 0.5f, (_fY + _fSizeY) - 0.5f, 0.f, 1.f);
	pVerties[3].vUV = _float2(0.f, 1.f);

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

void CMyRenderTarget::Render_DebugBuffer()
{
	m_pDevice->SetTexture(0, m_pTargetTexture);

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTX_SCREEN));
	m_pDevice->SetFVF(VTX_SCREEN_FVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}

void CMyRenderTarget::SetUp_OnShader(_EFFECT & _pEffect, const char * _pConstantTable)
{
	_pEffect->SetTexture(_pConstantTable, m_pTargetTexture);
}

CMyRenderTarget * CMyRenderTarget::Create(_DEVICE _pDevice, const _uint & _iWidth, const _uint & _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color)
{
	CMyRenderTarget* pInstance = new CMyRenderTarget(_pDevice);
	if (FAILED(pInstance->Ready_MyRenderTarget(_iWidth, _iHeight, _Format, _Color)))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMyRenderTarget::Free()
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
	Safe_Release(m_pOldTargetSurface);
	Safe_Release(m_pTargetSurface);
	Safe_Release(m_pDevice);
}