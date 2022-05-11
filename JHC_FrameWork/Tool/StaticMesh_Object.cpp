#include "stdafx.h"
#include "StaticMesh_Object.h"

#include "DynamicMesh_Object.h"

CStaticMesh_Object::CStaticMesh_Object(_DEVICE _pDevice, OBJECTINFO_DESC& _tDesc)
	: CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pCalculator(nullptr)
	, m_pXYZLine(nullptr)
	, m_bSelect(false)
	//, m_pCubeCollider(nullptr)
{
	m_tObjectInfoDesc = _tDesc;
}

CStaticMesh_Object::CStaticMesh_Object(const CStaticMesh_Object & _rOther)
	: CGameObject(_rOther)
	, m_bSelect(_rOther.m_bSelect)
{
	m_tObjectInfoDesc = _rOther.m_tObjectInfoDesc;
}

HRESULT CStaticMesh_Object::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStaticMesh_Object::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (nullptr != _pArg)
	{
		memcpy(&m_tTransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	}

	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component")
		return E_FAIL;
	}

	return S_OK;
}

_uint CStaticMesh_Object::Update_GameObject(const _float& _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	return _uint();
}

_uint CStaticMesh_Object::LateUpdate_GameObject(const _float& _fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(_fDeltaTime);

	auto pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return 0;
	}

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	return m_pTransform->Update_Transform();
}

HRESULT CStaticMesh_Object::Render_GameObject()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}
#ifndef _AFXDLL


	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(0);

	m_pMesh->Render_Meshes(pEffect);

	pEffect->EndPass();
	pEffect->End();

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(0);

	if (m_bSelect)
	{
		m_pXYZLine->Set_ParentMatix(&m_pTransform->Get_TransformDesc().matWorld);
		m_pXYZLine->Set_Transform();
		if (FAILED(m_pXYZLine->Render_XYZLine(pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
	}	

	for (auto& iter : m_mapSphereCollider)
	{
		iter.second->Set_ParentMatrix(&m_pTransform->Get_TransformDesc().matWorld);
		iter.second->Set_Transform();

		if (Collision())
			iter.second->Render_Collider(EColliderType::True, pEffect);
		else
			iter.second->Render_Collider(EColliderType::False, pEffect);
	}

	pEffect->EndPass();
	pEffect->End();	

	Safe_Release(pEffect);
#else

	m_pMesh->Render_Meshes();

	/*m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	if (m_bSelect)
	{
		m_pXYZLine->Set_ParentMatix(&m_pTransform->Get_TransformDesc().matWorld);
		m_pXYZLine->Set_Transform();
		if (FAILED(m_pXYZLine->Render_XYZLine()))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
	}	

	for (auto& iter : m_mapSphereCollider)
	{
		iter.second->Set_ParentMatrix(&m_pTransform->Get_TransformDesc().matWorld);
		iter.second->Set_Transform();

		if (Collision())
			iter.second->Render_Collider(EColliderType::True);
		else
			iter.second->Render_Collider(EColliderType::False);
	}

	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);*/
#endif // !_AFXDLL

	return S_OK;
}

HRESULT CStaticMesh_Object::Add_Component()
{
	/* For.Com_XYZLine */
	XYZLINE_DESC tXYZLine_Desc;
	tXYZLine_Desc.vScale = _float3(1.f, 1.f, 1.f);
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_XYZLine",
		L"Com_XYZLine",
		EComponentType::Dynamic,
		(CComponent**)&m_pXYZLine,
		&tXYZLine_Desc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	
	//* For.Com_DynamicMesh */
	wstring MeshPrototypeName = m_tObjectInfoDesc.szFileTitle;
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		MeshPrototypeName,
		L"Com_Mesh",
		(CMesh**)&m_pMesh
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	/* For.Com_Transfrom */
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Static,
		(CComponent**)&m_pTransform,
		&m_tTransformDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
#ifndef _AFXDLL


	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Shader_General_Static",
		L"Shader_General_Static",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}	

#endif // !_AFXDLL

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Calculator",
		L"Com_Calculator",
		EComponentType::Dynamic,
		(CComponent**)&m_pCalculator
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	/*CUBECOLLIDER_DESC tCubeColliderDesc;
	tCubeColliderDesc.vPos = *m_pMesh->Get_VtxPos();
	tCubeColliderDesc.dwVtxCount = m_pMesh->Get_VtxCount();

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Collider_Cube",
		L"Com_Collider_Cube",
		EComponentType::Static,
		(CComponent**)&m_pCubeCollider,
		&tCubeColliderDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CStaticMesh_Object::SetUp_ConstantTable(_EFFECT _pEffect)
{
	_float4x4 matWorld, matView, matProj;

	matWorld = *m_pTransform->Get_WorldMatrix();
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);

	auto pManagement = GET_MANAGEMENT;

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	return S_OK;
}

_bool CStaticMesh_Object::Collision()
{
	/*auto pManagement = GET_MANAGEMENT;
	CDynamicMesh_Object* pTemp = dynamic_cast<CDynamicMesh_Object*>(pManagement->Get_GameObject((_uint)ESceneType::Logo, L"DynamicMesh_Layer"));
	_float DynamicScale = dynamic_cast<CTransform*>(pTemp->Get_Component(L"Com_Transform"))->Get_TransformDesc().vScale.x;
	if (nullptr == pTemp)
		return false;

	map<wstring, class CSphere_Collider*> mapTemp = pTemp->Get_SphereCollider();

	for (auto& rWomanBossPair : m_mapSphereCollider)
	{
		for (auto& rWeaponPair : mapTemp)
		{
			if (m_pCalculator->Collision_Spher
			(
				rWomanBossPair.second->Get_WorldMatrix(), rWomanBossPair.second->Get_Radius() * rWomanBossPair.second->Get_Scale().x * m_pTransform->Get_TransformDesc().vScale.x,
				rWeaponPair.second->Get_WorldMatrix(), rWeaponPair.second->Get_Radius() * rWeaponPair.second->Get_Scale().x * DynamicScale
			))
			{
				_float4x4 aa = rWomanBossPair.second->Get_WorldMatrix();
				_float4x4 bb = rWeaponPair.second->Get_WorldMatrix();

				return true;
			}

		}
	}*/

	return false;
}

CSphere_Collider * CStaticMesh_Object::Add_Collider(const wstring & _pBoneName)
{
	CSphere_Collider* pSphereCollider = nullptr;

	wstring wstrName = L"Com_Sphere_" + (wstring)_pBoneName;

	//* For.Com_SphereMesh */
	SPHERECOLLIDER_DESC tSphereColliderDesc;
	tSphereColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	wsprintf(tSphereColliderDesc.szBoneName, _pBoneName.c_str());
	//tSphereColliderDesc.matParentWorld = m_pTransform->Get_TransformDesc().matWorld;
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Sphere_Collider",
		wstrName.c_str(),
		EComponentType::Dynamic,
		(CComponent**)&pSphereCollider,
		&tSphereColliderDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return nullptr;
	}

	m_mapSphereCollider.emplace(wstrName, pSphereCollider);

	return pSphereCollider;
}

CSphere_Collider * CStaticMesh_Object::Add_Collider(const SPHERECOLLIDER_DESC & _tSphereColliderDesc)
{
	CSphere_Collider* pSphereCollider = nullptr;

	wstring wstrName = L"Com_Sphere_" + (wstring)_tSphereColliderDesc.szBoneName;

	//* For.Com_SphereMesh */
	SPHERECOLLIDER_DESC tSphereColliderDesc;
	tSphereColliderDesc = _tSphereColliderDesc;
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Sphere_Collider",
		wstrName.c_str(),
		EComponentType::Dynamic,
		(CComponent**)&pSphereCollider,
		&tSphereColliderDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return nullptr;
	}

	m_mapSphereCollider.emplace(wstrName, pSphereCollider);

	return pSphereCollider;
}

void CStaticMesh_Object::Delete_Collider(const wstring & _pBoneName)
{
	auto iter = m_mapSphereCollider.begin();
	for (; iter != m_mapSphereCollider.end(); )
	{
		if (iter->first == _pBoneName)
		{
			Safe_Release(iter->second);
			m_mapSphereCollider.erase(iter++);
		}
		else
			++iter;
	}
}

CStaticMesh_Object * CStaticMesh_Object::Create(_DEVICE _pDevice, OBJECTINFO_DESC& _tDesc)
{
	CStaticMesh_Object* pInstance = new CStaticMesh_Object(_pDevice, _tDesc);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStaticMesh_Object::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CStaticMesh_Object* pClone = new CStaticMesh_Object(*this);

	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed To pClone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CStaticMesh_Object::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);	
#ifndef _AFXDLL


	Safe_Release(m_pShader);

#endif // !_AFXDLL
	Safe_Release(m_pCalculator);
	Safe_Release(m_pXYZLine);

	for (auto& rPair : m_mapSphereCollider)
	{
		Safe_Release(rPair.second);
	}

	m_mapSphereCollider.clear();

	CGameObject::Free();
}
