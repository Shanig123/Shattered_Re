#include "stdafx.h"
#include "Map_Shrine.h"


CMap_Shrine::CMap_Shrine(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
{
	for (_uint i = 0; i < EMapType::End; ++i)
	{
		m_pMesh[i] = nullptr;
	}
}

CMap_Shrine::CMap_Shrine(const CMap_Shrine & _rOther)
	: CGameObject(_rOther)
{
}

HRESULT CMap_Shrine::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(Ready_MapCollider()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMap_Shrine::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMap_Shrine::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	auto pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}
	if (FAILED(pManagement->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	m_pTransform->Update_Transform();

	return (_uint)EEvent::None;
}

HRESULT CMap_Shrine::Render_GameObject()
{
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

	/*
	//Frustum 컬링 예제
	//m_pOptimization->ConstructFrustum(500.f);
	//m_pMesh->Render_Meshes(pEffect, m_pOptimization, m_pTransform->Get_WorldMatrix());
	*/

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		if (nullptr == m_pMesh[i])
			continue;

		m_pMesh[i]->Render_Meshes(pEffect);
	}

	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CMap_Shrine::Add_Component()
{
	/* For.Com_DynamicMesh */
	wstring wstrProtoName = L"StaticMesh_Map_";
	wstring wstrCloneName = L"Mesh_";
	wstring wstrMapType = L"";

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		switch (i)
		{
		case EMapType::Shrine:
			wstrMapType = to_wstring(i) + L"_Shrine";
			break;
		case EMapType::BrokenDoor:
			wstrMapType = to_wstring(i) + L"_BrokenDoor";
			break;
		}

		if (FAILED(CGameObject::Add_Mesh
		(
			(_uint)ESceneType::Static,
			wstrProtoName + wstrMapType,
			wstrCloneName + wstrMapType,
			(CMesh**)&m_pMesh[i]
		)))
		{
			ERROR_BOX(L"Failed to Add_Component");
			return E_FAIL;
		}
	}

	/* For.Com_Transfrom */
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Static,
		(CComponent**)&m_pTransform
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

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

	return S_OK;
}

HRESULT CMap_Shrine::SetUp_ConstantTable(_EFFECT _pEffect)
{
	_float4x4			matWorld, matView, matProj;

	auto pManagement = GET_MANAGEMENT;

	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetMatrix("g_matLightView", pManagement->Get_LightView());
	_pEffect->SetMatrix("g_matLightProj", pManagement->Get_LightProj());

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	return S_OK;
}

HRESULT CMap_Shrine::Ready_MapCollider()
{
	auto pManagement = GET_MANAGEMENT;

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		wstring wstrMapType = L"";
		switch (i)
		{
		case EMapType::Shrine:
			wstrMapType = L"_Shrine";
			break;
		case EMapType::BrokenDoor:
			wstrMapType = L"_BrokenDoor";
			break;
		}

		_ulong iSubsetcount = m_pMesh[i]->Get_SubsetCount();

		_float3* vMax = m_pMesh[i]->Get_MaxPos();
		_float3* vMin = m_pMesh[i]->Get_MinPos();

		vector<void*> vecVertices = m_pMesh[i]->Get_VecVertices();
		vector<void*> VecIndtices = m_pMesh[i]->Get_VecIndtices();
		vector<_uint> vecVerticesCount = m_pMesh[i]->Get_VecVerticesCount();
		vector<_uint> VecIndticesCount = m_pMesh[i]->Get_VecIndticesCount();

		for (_ulong j = 0; j < iSubsetcount; ++j)
		{
			_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
			_float3 vScale = m_pTransform->Get_TransformDesc().vScale;

			PxTriangleMeshDesc tTriangleMeshDesc;
			tTriangleMeshDesc.points.count = vecVerticesCount[j];
			tTriangleMeshDesc.points.data = vecVertices[j];
			tTriangleMeshDesc.points.stride = sizeof(VTX);
			tTriangleMeshDesc.triangles.count = VecIndticesCount[j];
			tTriangleMeshDesc.triangles.data = VecIndtices[j];
			if (m_pMesh[i]->Get_IndexSize() == sizeof(INDEX32))
				tTriangleMeshDesc.triangles.stride = sizeof(INDEX32);
			else
			{
				tTriangleMeshDesc.triangles.stride = sizeof(INDEX16);
				tTriangleMeshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
			}

			PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
			pManagement->TriangleMeshCooking(wstrMapType, vScale, tTriangleMeshDesc, transform);
		}

		/*if (nullptr == m_pMesh[i])
			continue;

		PxTriangleMeshDesc tTriangleMeshDesc;
		tTriangleMeshDesc.points.count = m_pMesh[i]->Get_VtxCount();
		tTriangleMeshDesc.points.data = m_pMesh[i]->Get_Vertices();
		tTriangleMeshDesc.points.stride = sizeof(VTX);
		tTriangleMeshDesc.triangles.count = m_pMesh[i]->Get_TriCount();
		tTriangleMeshDesc.triangles.data = m_pMesh[i]->Get_Indices();
		if (m_pMesh[i]->Get_IndexSize() == sizeof(INDEX32))
			tTriangleMeshDesc.triangles.stride = sizeof(INDEX32);
		else
		{
			tTriangleMeshDesc.triangles.stride = sizeof(INDEX16);
			tTriangleMeshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
		}

		PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
		pManagement->TriangleMeshCooking(vScale, tTriangleMeshDesc, transform);*/
	}

	return S_OK;
}

CMap_Shrine * CMap_Shrine::Create(_DEVICE _pDevice)
{
	CMap_Shrine* pInstance = new CMap_Shrine(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMap_Shrine::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CMap_Shrine* pClone = new CMap_Shrine(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMap_Shrine::Free()
{
	CGameObject::Free();
}
