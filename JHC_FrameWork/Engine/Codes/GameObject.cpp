#include "GameObject.h"
#include "Management.h"

USING(Engine)

CGameObject::CGameObject(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
	, m_fViewZ(0.f)
	, m_bDead(false), m_bCheckClone(false), m_bUpdateInit(false)
	, m_iSceneIdx(0), m_iEvent((_uint)EEvent::None)
{
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & _rOther)
	: m_pDevice(_rOther.m_pDevice)
	, m_fViewZ(_rOther.m_fViewZ)
	, m_bDead(false), m_bCheckClone(true), m_bUpdateInit(false)
	, m_iSceneIdx(_rOther.m_iSceneIdx), m_iEvent((_uint)EEvent::None)
	, m_PrototypeName(_rOther.m_PrototypeName)
{
	Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	m_iSceneIdx = _iSceneIdx;

	return S_OK;
}

_uint CGameObject::Update_GameObject(const _float& _fDeltaTime)
{
	for (auto& iter : m_Components[(_uint)EComponentType::Dynamic])
		iter.second->Update_Component(_fDeltaTime);

	return m_iEvent;
}

_uint CGameObject::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	return 0;
}

HRESULT CGameObject::Shadow_GameObject()
{
	CShader* pShader = dynamic_cast<CShader*>(CManagement::Get_Instance()->Get_Component((_uint)ESceneType::Static, L"Shader_Shadow"));

	LPD3DXEFFECT pEffect = pShader->Get_EffectHandle();

	//CRenderTarget_Manager::Get_Instance()->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture");
	pEffect->AddRef();

	_float4x4			matWorld, matView, matProj;
	auto pManagement = GET_MANAGEMENT;

	matWorld = dynamic_cast<CTransform*>(Get_Component(L"Com_Transform"))->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matLightView", pManagement->Get_LightView());
	pEffect->SetMatrix("g_matLightProj", pManagement->Get_LightProj());
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(0);
	
	for (auto iter : m_Meshs)
		iter.second->Render_ShadowMeshes(pEffect,nullptr);
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint _iSceneIndex, const wstring & _szPrototype, const wstring & _szComponent, EComponentType _eComponentType, CComponent ** _ppOut, void * _pArg)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CComponent* pComponent = pManagement->Clone_Component(_iSceneIndex, _szPrototype, _pArg);
	if (nullptr == pComponent)
	{
		ERROR_BOX(L"pComponent is nullptr");
		return E_FAIL;
	}

	m_Components[(_uint)_eComponentType].emplace(_szComponent, pComponent);

	if (_ppOut)
	{
		*_ppOut = pComponent;
	}

	return S_OK;
}

HRESULT CGameObject::Add_Mesh(_uint _iSceneIndex, const wstring & _szPrototype, const wstring & _szComponent, CMesh ** _ppOut, void * _pArg)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CMesh* pMesh = pManagement->Clone_Mesh(_iSceneIndex, _szPrototype, _pArg);
	if (nullptr == pMesh)
	{
		ERROR_BOX(L"pMesh is nullptr");
		return E_FAIL;
	}

	m_Meshs.emplace(_szComponent, pMesh);

	if (_ppOut)
	{
		*_ppOut = pMesh;
	}

	return S_OK;
}

void CGameObject::Compute_ViewZ(const _float3 * vPos)
{
	_float4x4 matCamWorld;

	m_pDevice->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

	_float3 vCamPos;
	memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_float3));

	m_fViewZ = D3DXVec3Length(&(vCamPos - *vPos));
}

CComponent * CGameObject::Get_Component(const wstring & _szComponent)
{
	for (_uint i = 0; i < (_uint)EComponentType::End; ++i)
	{
		auto iter_find = m_Components[i].find(_szComponent);
		if (m_Components[i].end() != iter_find)
		{
			return iter_find->second;
		}
	}

	ERROR_BOX(L"m_Components is end");
	return nullptr;
}

HRESULT CGameObject::Delete_Component()
{
	for (_uint i = 0; i < (_uint)EComponentType::End; ++i)
	{
		for (auto& rPair : m_Components[i])
		{
			Safe_Release(rPair.second);
		}

		m_Components[i].clear();
	}

	return S_OK;
}

HRESULT CGameObject::Delete_Component(const wstring & _szComponent)
{
	for (_uint i = 0; i < (_uint)EComponentType::End; ++i)
	{
		for (auto& rPair : m_Components[i])
		{
			auto iter = m_Components[i].begin();
			for (; iter != m_Components[i].end(); )
			{
				if (iter->first == _szComponent)
				{
					Safe_Release(iter->second);
					m_Components[i].erase(iter++);
				}
				else
					++iter;
			}
		}
	}

	return S_OK;
}

CMesh * CGameObject::Get_Mesh(const wstring & _szComponent)
{
	auto iter_find = m_Meshs.find(_szComponent);
	if (m_Meshs.end() != iter_find)
	{
		return iter_find->second;
	}

	return nullptr;
}

void CGameObject::Add_Damage(_uint _iSceneIndex, const wstring& _szGameObject, _uint _iDamage)
{
	auto pManagement = GET_MANAGEMENT;
	CGameObject* pGameObject =  pManagement->Get_GameObject(_iSceneIndex, _szGameObject);
	pGameObject->m_tState.fHP -= _iDamage;
}

STAT_DESC CGameObject::Get_Stat()
{
	return m_tState;
}

void CGameObject::Free()
{
	for (_uint i = 0; i < (_uint)EComponentType::End; ++i)
	{
		for_each(m_Components[i].begin(), m_Components[i].end(), [](auto& Pair) 
		{
		
			{
				if (Pair.second->Get_CheckClone())
					Safe_Release(Pair.second);
			}
		});
		m_Components[i].clear();
	}

	for_each(m_Meshs.begin(), m_Meshs.end(), CDeleteMap());
	m_Meshs.clear();

	Safe_Release(m_pDevice);
}
