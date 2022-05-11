#include "Sphere_Collider.h"

USING(Engine)

CSphere_Collider::CSphere_Collider(_DEVICE _pDevice, _float _fRadius, _uint _iSlices, _uint _iStacks)
	: CComponent(_pDevice)
	, m_pMesh(nullptr)
	, m_fRadius(_fRadius)
	, m_iSlices(_iSlices)
	, m_iStacks(_iStacks)
{
	ZeroMemory(&m_pTexture, sizeof(m_pTexture));
}

CSphere_Collider::CSphere_Collider(const CSphere_Collider & _rOther)
	: CComponent(_rOther)
	, m_pMesh(_rOther.m_pMesh)
	, m_fRadius(_rOther.m_fRadius)
	, m_iSlices(_rOther.m_iSlices)
	, m_iStacks(_rOther.m_iStacks)
{	
	m_pMesh->AddRef();

	for (_uint i = 0; i < (_uint)EColliderType::End; ++i)
	{
		m_pTexture[i] = _rOther.m_pTexture[i];
	}
}

HRESULT CSphere_Collider::Ready_Component_Prototype()
{
	D3DXCreateSphere(m_pDevice, m_fRadius, m_iSlices, m_iStacks, &m_pMesh, NULL);

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

HRESULT CSphere_Collider::Ready_Component_Clone(void * _pArg)
{
	if (nullptr != _pArg)
	{
		memcpy(&m_SphereColliderDesc, _pArg, sizeof(SPHERECOLLIDER_DESC));
	}

	return S_OK;
}

_uint CSphere_Collider::Update_Component(const _float & _fDeltaTime)
{
	_float4x4 matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_SphereColliderDesc.vScale.x, m_SphereColliderDesc.vScale.y, m_SphereColliderDesc.vScale.z);
	D3DXMatrixTranslation(&matTrans, m_SphereColliderDesc.vPos.x, m_SphereColliderDesc.vPos.y, m_SphereColliderDesc.vPos.z);

	m_SphereColliderDesc.matWorld = matScale * matTrans;

	return _uint();
}

HRESULT CSphere_Collider::Render_Collider(EColliderType _eType, _float4x4* _pColliderMatrix)
{
	m_pDevice->SetTexture(0, m_pTexture[(_uint)_eType]);
	m_pMesh->DrawSubset(0);

	return S_OK;
}

HRESULT CSphere_Collider::Render_Collider(EColliderType _eType, _EFFECT & _pEffect)
{
#ifndef _AFXDLL

	_float4x4 matWorld, matView, matProj;

	matWorld = m_SphereColliderDesc.matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);

	_pEffect->SetTexture("g_BaseTexture", m_pTexture[(_uint)_eType]);
	_pEffect->CommitChanges();
#else
	_float4x4 matWorld;
	matWorld = m_SphereColliderDesc.matWorld;
	m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
#endif // !_AFXDLL

	m_pMesh->DrawSubset(0);

	return S_OK;
}

HRESULT CSphere_Collider::Set_Transform()
{
	return m_pDevice->SetTransform(D3DTS_WORLD, &m_SphereColliderDesc.matWorld);
}

_bool CSphere_Collider::Sphere_Picking(HWND _hWnd, _uint _iWinCX, _uint _iWinCY, CCamera * _pCamera, const _float4x4 * _pMatWorld)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(_hWnd, &ptMouse);

	/* 뷰포트 -> 투영스페이스 */
	_float3 vMouse = _float3(0.f, 0.f, 0.f);
	vMouse.x = ptMouse.x / (_iWinCX * 0.5f) - 1.f;
	vMouse.y = 1.f - ptMouse.y / (_iWinCY * 0.5f);

	/* 투영스페이스 -> 뷰스페이스 */
	_float4x4 matProj = _pCamera->Get_CameraDesc().matProj;
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMouse, &vMouse, &matProj);

	_float3 m_vRayPos = _float3(0.f, 0.f, 0.f);
	_float3 m_vRayDir = vMouse - m_vRayPos;

	/* 뷰스페이스 -> 월드스페이스 */
	_float4x4 matView = _pCamera->Get_CameraDesc().matView;
	D3DXMatrixInverse(&matView, 0, &matView);
	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &matView);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matView);

	/* 월드스페이스 -> 로컬스페이스 */
	_float4x4 matInvWorld;
	D3DXMatrixInverse(&matInvWorld, 0, _pMatWorld);
	D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &matInvWorld);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matInvWorld);

	_float3 v = m_vRayPos - m_SphereColliderDesc.vPos;
	_float b = 2.0f * D3DXVec3Dot(&m_vRayDir, &v);
	_float c = D3DXVec3Dot(&v, &v) - (m_SphereColliderDesc.vScale.x * m_SphereColliderDesc.vScale.x);
	_float dist = (b *b) - (4.f * c);
	
	if (dist < 0.f)
		return false;

	dist = sqrtf(dist);
	_float s0 = (-b + dist) * 0.5f;
	_float s1 = (-b - dist) * 0.5f;

	if (s0 < 0.f && s1 < 0.f)
		return false;

	return true;
}

void CSphere_Collider::Set_ParentMatrix(const _float4x4 * _pParentMatrix)
{
	m_SphereColliderDesc.matWorld *= *_pParentMatrix;
}

CSphere_Collider * CSphere_Collider::Create(_DEVICE _pDevice, _float _fRadius, _uint _iSlices, _uint _iStacks)
{
	CSphere_Collider* pInstance = new CSphere_Collider(_pDevice, _fRadius, _iSlices, _iStacks);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CSphere_Collider::Clone(void * _pArg)
{
	CSphere_Collider* pClone = new CSphere_Collider(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CSphere_Collider::Free()
{
	if (false == m_bClone)
	{
		Safe_Release(m_pMesh);

		for (_uint i = 0; i < (_uint)EColliderType::End; ++i)
			Safe_Release(m_pTexture[i]);
	}

	CComponent::Free();
}
