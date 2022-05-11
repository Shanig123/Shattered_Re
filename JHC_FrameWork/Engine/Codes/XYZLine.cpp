#include "XYZLine.h"

USING(Engine)

CXYZLine::CXYZLine(_DEVICE _pDevice)
	: CComponent(_pDevice)
{
	for (_uint i = 0; i < 3; ++i)
	{
		m_pLine[i] = nullptr;
		m_pSphere_Collider[i] = nullptr;
	}
		
}

CXYZLine::CXYZLine(const CXYZLine & _rOther)
	: CComponent(_rOther)
{
	for (_uint i = 0; i < 3; ++i)
	{
		m_pLine[i] = _rOther.m_pLine[i];
		m_pSphere_Collider[i] = _rOther.m_pSphere_Collider[i];
	}
		
}

HRESULT CXYZLine::Ready_Component_Prototype()
{
	m_pLine[0] = CLine::Create(m_pDevice, _float3(100.f, 0.f, 0.f), ARGB(255, 0, 0, 255));
	m_pLine[1] = CLine::Create(m_pDevice, _float3(0.f, 100.f, 0.f), ARGB(255, 255, 0, 0));
	m_pLine[2] = CLine::Create(m_pDevice, _float3(0.f, 0.f, 100.f), ARGB(255, 0, 255, 0));
	
	m_pSphere_Collider[0] = CSphere_Collider::Create(m_pDevice, 1.f, 20, 20);
	m_pSphere_Collider[1] = CSphere_Collider::Create(m_pDevice, 1.f, 20, 20);
	m_pSphere_Collider[2] = CSphere_Collider::Create(m_pDevice, 1.f, 20, 20);

	return S_OK;
}

HRESULT CXYZLine::Ready_Component_Clone(void * _pArg)
{
	if (nullptr != _pArg)
	{
		memcpy(&m_XYZLineDesc, _pArg, sizeof(XYZLINE_DESC));
	}

	SPHERECOLLIDER_DESC tSphereColliderDesc;

	tSphereColliderDesc.vPos = _float3(100.f, 0.f, 0.f);
	tSphereColliderDesc.vScale = _float3(10.f, 10.f, 10.f);
	m_pSphere_Collider[0]->Set_Desc(tSphereColliderDesc);

	tSphereColliderDesc.vPos = _float3(0.f, 100.f, 0.f);
	tSphereColliderDesc.vScale = _float3(10.f, 10.f, 10.f);
	m_pSphere_Collider[1]->Set_Desc(tSphereColliderDesc);

	tSphereColliderDesc.vPos = _float3(0.f, 0.f, 100.f);
	tSphereColliderDesc.vScale = _float3(10.f, 10.f, 10.f);
	m_pSphere_Collider[2]->Set_Desc(tSphereColliderDesc);

	return S_OK;
}

_uint CXYZLine::Update_Component(const _float & _fDeltaTime)
{
	_float4x4 matScale, matTrans;

	D3DXMatrixScaling(&matScale, m_XYZLineDesc.vScale.x, m_XYZLineDesc.vScale.y, m_XYZLineDesc.vScale.z);
	D3DXMatrixTranslation(&matTrans, m_XYZLineDesc.vPos.x, m_XYZLineDesc.vPos.y, m_XYZLineDesc.vPos.z);

	m_XYZLineDesc.matWorld = matScale * matTrans;

	for (_uint i = 0; i < 3; ++i)
		m_pSphere_Collider[i]->Update_Component(_fDeltaTime);

	return _uint();
}

HRESULT CXYZLine::Render_XYZLine(_EFFECT & _pEffect)
{
	for (_uint i = 0; i < 3; ++i)
	{
		m_pLine[i]->Render_Line(_pEffect, &m_XYZLineDesc.matWorld);
		m_pSphere_Collider[i]->Set_ParentMatrix(&m_XYZLineDesc.matWorld);
		m_pSphere_Collider[i]->Set_Transform();
		m_pSphere_Collider[i]->Render_Collider(EColliderType::False, _pEffect);
	}
		

	return S_OK;
}
HRESULT CXYZLine::Render_XYZLine()
{
	for (_uint i = 0; i < 3; ++i)
	{
		m_pLine[i]->Render_Line(&m_XYZLineDesc.matWorld);
		m_pSphere_Collider[i]->Set_ParentMatrix(&m_XYZLineDesc.matWorld);
		m_pSphere_Collider[i]->Set_Transform();
		m_pSphere_Collider[i]->Render_Collider(EColliderType::False, nullptr);	
	}


	return S_OK;
}

HRESULT CXYZLine::Set_Transform()
{
	return m_pDevice->SetTransform(D3DTS_WORLD, &m_XYZLineDesc.matWorld);
}

void CXYZLine::Set_ParentMatix(const _float4x4 * _pParentMatrix)
{
	m_XYZLineDesc.matWorld *= *_pParentMatrix;
}

CXYZLine * CXYZLine::Create(_DEVICE _pDevice)
{
	CXYZLine* pInstance = new CXYZLine(_pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CXYZLine::Clone(void * _pArg)
{
	CXYZLine* pClone = new CXYZLine(*this);
	if (FAILED(pClone->Ready_Component_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CXYZLine::Free()
{
	if (false == m_bClone)
	{
		for (_uint i = 0; i < 3; ++i)
		{
			Safe_Release(m_pLine[i]);
			Safe_Release(m_pSphere_Collider[i]);
		}		
	}

	CComponent::Free();
}
