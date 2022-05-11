#include "stdafx.h"
#include "DynamicMesh_Object.h"

CDynamicMesh_Object::CDynamicMesh_Object(_DEVICE _pDevice, OBJECTINFO_DESC& _tDesc)
	: CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pCalculator(nullptr)
	, m_bSelect(false)
{
	m_tObjectInfoDesc = _tDesc;
}

CDynamicMesh_Object::CDynamicMesh_Object(const CDynamicMesh_Object & _rOther)
	: CGameObject(_rOther)
	, m_bSelect(_rOther.m_bSelect)
{
	m_tObjectInfoDesc = _rOther.m_tObjectInfoDesc;
}

HRESULT CDynamicMesh_Object::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}	

	return S_OK;
}

HRESULT CDynamicMesh_Object::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
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

_uint CDynamicMesh_Object::Update_GameObject(const _float& _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	m_pMesh->Play_Animationset(_fDeltaTime);

	return _uint();
}

_uint CDynamicMesh_Object::LateUpdate_GameObject(const _float& _fDeltaTime)
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

HRESULT CDynamicMesh_Object::Render_GameObject()
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
	pEffect->BeginPass(5);

	for (auto& iter : m_mapSphereCollider)
	{
		const char* BoneName = WstringToChar(iter.first.c_str());
		const MY_FRAME* pFrame = m_pMesh->Get_FrameByName(BoneName);

		iter.second->Set_ParentMatrix(&(pFrame->CombinedTranformationMatrix * m_pTransform->Get_TransformDesc().matWorld));
		iter.second->Set_Transform();

		if (Collision())
			iter.second->Render_Collider(EColliderType::True, pEffect);
		else
			iter.second->Render_Collider(EColliderType::False, pEffect);

		Safe_Delete(BoneName);
	}

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
#else
	m_pMesh->Render_Meshes();

	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	for (auto& iter : m_mapSphereCollider)
	{
		const char* BoneName = WstringToChar(iter.first.c_str());
		const MY_FRAME* pFrame = m_pMesh->Get_FrameByName(BoneName);

		iter.second->Set_ParentMatrix(&(pFrame->CombinedTranformationMatrix * m_pTransform->Get_TransformDesc().matWorld));
		iter.second->Set_Transform();

		if (Collision())
			iter.second->Render_Collider(EColliderType::True, nullptr);
		else
			iter.second->Render_Collider(EColliderType::False, nullptr);

		Safe_Delete(BoneName);
	}

	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif // !_AFXDLL

	return S_OK;
}

HRESULT CDynamicMesh_Object::Add_Component()
{
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

	m_pMesh->Set_Animationset(0);

	/* For.Com_Transfrom */
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Dynamic,
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
		L"Shader_Player",
		L"Shader_Player",
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

	return S_OK;
}

HRESULT CDynamicMesh_Object::SetUp_ConstantTable(_EFFECT _pEffect)
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

_bool CDynamicMesh_Object::Collision()
{
	return _bool();
}

class CSphere_Collider* CDynamicMesh_Object::Add_Collider(const wstring& _pBoneName)
{
	CSphere_Collider* pSphereCollider = nullptr;

	SPHERECOLLIDER_DESC tSphereColliderDesc;
	tSphereColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	wsprintf(tSphereColliderDesc.szBoneName, _pBoneName.c_str());
	wstring wstrName = L"Com_Sphere_Collider_" + (wstring)_pBoneName;

	//* For.Com_SphereMesh */
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

	m_mapSphereCollider.emplace(_pBoneName, pSphereCollider);

	return pSphereCollider;
}

CSphere_Collider * CDynamicMesh_Object::Add_Collider(const SPHERECOLLIDER_DESC & _tSphereColliderDesc)
{
	CSphere_Collider* pSphereCollider = nullptr;

	wstring wstrName = L"Com_Sphere_Collider_" + (wstring)_tSphereColliderDesc.szBoneName;

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

	m_mapSphereCollider.emplace(_tSphereColliderDesc.szBoneName, pSphereCollider);

	return pSphereCollider;
}

void CDynamicMesh_Object::Delete_Collider(const wstring & _pBoneName)
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

CDynamicMesh_Object * CDynamicMesh_Object::Create(_DEVICE _pDevice, OBJECTINFO_DESC& _tDesc)
{
	CDynamicMesh_Object* pInstance = new CDynamicMesh_Object(_pDevice, _tDesc);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed To Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDynamicMesh_Object::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CDynamicMesh_Object* pClone = new CDynamicMesh_Object(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed To pClone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CDynamicMesh_Object::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);	
#ifndef _AFXDLL

	Safe_Release(m_pShader);

#endif // !_AFXDLL

	Safe_Release(m_pCalculator);

	for (auto& rPair : m_mapSphereCollider)
	{
		Safe_Release(rPair.second);
	}

	m_mapSphereCollider.clear();

	CGameObject::Free();
}
