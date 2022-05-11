#include "Cube_Collider.h"

USING(Engine)

CCube_Collider::CCube_Collider(_DEVICE _pDevice)
	: CComponent(_pDevice)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
{
	ZeroMemory(&m_pTexture, sizeof(m_pTexture));
}

CCube_Collider::CCube_Collider(const CCube_Collider & _rOther)
	: CComponent(_rOther)
{
	for (_uint i = 0; i < (_uint)EColliderType::End; ++i)
	{
		m_pTexture[i] = _rOther.m_pTexture[i];
	}
}

HRESULT CCube_Collider::Ready_Component_Prototype()
{
	for (_uint i = 0; i < (_uint)EColliderType::End; ++i)
	{
		D3DXCreateTexture(
			m_pDevice,
			1,
			1,
			1, // miplevel
			0,	// 텍스처의 용도
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&m_pTexture[i]);
	}

	D3DLOCKED_RECT LockRect;

	m_pTexture[0]->LockRect(0, &LockRect, NULL, 0);
	*((_ulong*)LockRect.pBits) = ARGB(255, 0, 255, 0);
	m_pTexture[0]->UnlockRect(0);

	m_pTexture[1]->LockRect(0, &LockRect, NULL, 0);
	*((_ulong*)LockRect.pBits) = ARGB(255, 255, 0, 0);
	m_pTexture[1]->UnlockRect(0);

	return S_OK;
}

HRESULT CCube_Collider::Ready_Component_Clone(void * _pArg)
{
	if (nullptr != _pArg)
	{
		memcpy(&m_CubeColliderDesc, _pArg, sizeof(CUBECOLLIDER_DESC));
	}

	D3DXComputeBoundingBox(&m_CubeColliderDesc.vPos, m_CubeColliderDesc.dwVtxCount, sizeof(_float3), &m_vMin, &m_vMax);

	if (FAILED(m_pDevice->CreateVertexBuffer
	(
		sizeof(VTX_CUBETEXTURE) * 8,
		0, // 정적 버퍼 사용 시 숫자 0(D3DUSAGE_DYNAMIC : 파티클을 생성할 때)
		VTX_CUBETEXTURE_FVF,
		D3DPOOL_MANAGED, // 일반적으로 정적 버퍼 사용 시 Managed, 동적버퍼 사용 시 Default 사용
		&m_pVB, NULL)))
	{
		ERROR_BOX(L"Failed to CreateVertexBuffer");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateIndexBuffer
	(
		sizeof(INDEX16) * 12,
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIB, NULL)))
	{
		ERROR_BOX(L"Failed to CreateIndexBuffer");
		return E_FAIL;
	}


	VTX_CUBETEXTURE* pVertex = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 앞면
	pVertex[0].vPos = _float3(m_vMin.x, m_vMax.y, m_vMin.z);
	pVertex[0].vUV = pVertex[0].vPos;

	pVertex[1].vPos = _float3(m_vMax.x, m_vMax.y, m_vMin.z);
	pVertex[1].vUV = pVertex[1].vPos;

	pVertex[2].vPos = _float3(m_vMax.x, m_vMin.y, m_vMin.z);
	pVertex[2].vUV = pVertex[2].vPos;

	pVertex[3].vPos = _float3(m_vMin.x, m_vMin.y, m_vMin.z);
	pVertex[3].vUV = pVertex[3].vPos;

	// 뒷면
	pVertex[4].vPos = _float3(m_vMin.x, m_vMax.y, m_vMax.z);
	pVertex[4].vUV = pVertex[4].vPos;

	pVertex[5].vPos = _float3(m_vMax.x, m_vMax.y, m_vMax.z);
	pVertex[5].vUV = pVertex[5].vPos;

	pVertex[6].vPos = _float3(m_vMax.x, m_vMin.y, m_vMax.z);
	pVertex[6].vUV = pVertex[6].vPos;

	pVertex[7].vPos = _float3(m_vMin.x, m_vMin.y, m_vMax.z);
	pVertex[7].vUV = pVertex[7].vPos;

	m_pVB->Unlock();

	INDEX16*		pIndex = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// X+
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 5;
	pIndex[0]._3 = 6;

	pIndex[1]._1 = 1;
	pIndex[1]._2 = 6;
	pIndex[1]._3 = 2;

	// X-
	pIndex[2]._1 = 4;
	pIndex[2]._2 = 0;
	pIndex[2]._3 = 3;

	pIndex[3]._1 = 4;
	pIndex[3]._2 = 3;
	pIndex[3]._3 = 7;

	// y+
	pIndex[4]._1 = 4;
	pIndex[4]._2 = 5;
	pIndex[4]._3 = 1;

	pIndex[5]._1 = 4;
	pIndex[5]._2 = 1;
	pIndex[5]._3 = 0;

	// y-
	pIndex[6]._1 = 3;
	pIndex[6]._2 = 2;
	pIndex[6]._3 = 6;

	pIndex[7]._1 = 3;
	pIndex[7]._2 = 6;
	pIndex[7]._3 = 7;

	// z+
	pIndex[8]._1 = 7;
	pIndex[8]._2 = 6;
	pIndex[8]._3 = 5;

	pIndex[9]._1 = 7;
	pIndex[9]._2 = 5;
	pIndex[9]._3 = 4;

	// z-
	pIndex[10]._1 = 0;
	pIndex[10]._2 = 1;
	pIndex[10]._3 = 2;

	pIndex[11]._1 = 0;
	pIndex[11]._2 = 2;
	pIndex[11]._3 = 3;

	m_pIB->Unlock();

	return S_OK;
}

_uint CCube_Collider::Update_Component(const _float & _fDeltaTime)
{
	return _uint();
}

HRESULT CCube_Collider::Render_Collider(EColliderType _eType, _float4x4* _pColliderMatrix)
{
	//Update_Transform();
	//m_CubeColliderDesc.matWorld = *_pColliderMatrix;
	m_pDevice->SetTransform(D3DTS_WORLD, _pColliderMatrix);
	m_pDevice->SetTexture(0, m_pTexture[(_uint)_eType]);
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VTX_CUBETEXTURE));
	m_pDevice->SetFVF(VTX_CUBETEXTURE_FVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

	return S_OK;
}

_uint CCube_Collider::Update_Transform()
{
	_float4x4 matScale, matTrans;

	D3DXMatrixScaling(&matScale, 10.f, 10.f, 10.f);
	D3DXMatrixTranslation(&matTrans, m_CubeColliderDesc.vPos.x, m_CubeColliderDesc.vPos.y, m_CubeColliderDesc.vPos.z);

	m_CubeColliderDesc.matWorld = matScale * matTrans;

	return _uint();
}

HRESULT CCube_Collider::Set_Transform()
{
	return m_pDevice->SetTransform(D3DTS_WORLD, &m_CubeColliderDesc.matWorld);
}

void CCube_Collider::Set_ParentMatrix(const _float4x4 * _pParentMatrix)
{
	m_CubeColliderDesc.matWorld *= *_pParentMatrix;
}

CCube_Collider * CCube_Collider::Create(_DEVICE _pDevice)
{
	CCube_Collider* pInstance = new CCube_Collider(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCube_Collider::Clone(void * _pArg)
{
	CCube_Collider* pClone = new CCube_Collider(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCube_Collider::Free()
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	if (false == m_bClone)
	{
		for (_uint i = 0; i < (_uint)EColliderType::End; ++i)
			Safe_Release(m_pTexture[i]);
	}

	CComponent::Free();
}
