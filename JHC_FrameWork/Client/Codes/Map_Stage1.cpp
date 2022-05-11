#include "stdafx.h"
#include "Map_Stage1.h"
#include "MainCamera.h"
#include "Player.h"


CMap_Stage1::CMap_Stage1(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	//, m_pOptimization(nullptr)
{
	for (_uint i = 0; i < EMapType::End; ++i)
	{
		m_pMesh[i] = nullptr;
	}
}

CMap_Stage1::CMap_Stage1(const CMap_Stage1 & _rOther)
	: CGameObject(_rOther)
{
}

HRESULT CMap_Stage1::Ready_GameObject_Prototype()
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

HRESULT CMap_Stage1::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMap_Stage1::Update_GameObject(const _float & _fDeltaTime)
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

HRESULT CMap_Stage1::Render_GameObject()
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

	

	

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		if (nullptr == m_pMesh[i])
			continue;

		////Frustum 컬링 예제
		//m_pOptimization->ConstructFrustum(100.f);
		//m_pMesh[i]->Render_Meshes(pEffect, m_pOptimization, m_pTransform->Get_WorldMatrix());
		m_pMesh[i]->Render_Meshes(pEffect);
	}

	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CMap_Stage1::Add_Component()
{
	/* For.Com_DynamicMesh */
	wstring wstrProtoName = L"StaticMesh_Map_";
	wstring wstrCloneName = L"Mesh_";
	wstring wstrMapType = L"";

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		switch (i)
		{
		case EMapType::Glow:
			wstrMapType = to_wstring(i) + L"_Glow";
			break;
		case EMapType::Tutorial:
			wstrMapType = to_wstring(i) + L"_Tutorial";
			break;
		case EMapType::Door:
			wstrMapType = to_wstring(i) + L"_Door";
			break;
		case EMapType::Boss:
			wstrMapType = to_wstring(i) + L"_Boss";
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

	/*if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Optimization",
		L"Com_Optimization",
		EComponentType::Static,
		(CComponent**)&m_pOptimization
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}*/

	return S_OK;
}

HRESULT CMap_Stage1::SetUp_ConstantTable(_EFFECT _pEffect)
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

HRESULT CMap_Stage1::Ready_MapCollider()
{
	auto pManagement = GET_MANAGEMENT;

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		if (nullptr == m_pMesh[i])
			continue;

		wstring wstrMapType = L"";
		switch (i)
		{
		case EMapType::Glow:
			wstrMapType = L"_Glow";
			break;
		case EMapType::Tutorial:
			wstrMapType = L"_Tutorial";
			break;
		case EMapType::Door:
			wstrMapType = L"_Door";
			break;
		case EMapType::Boss:
			wstrMapType = L"_Boss";
			break;
		}

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
		pManagement->TriangleMeshCooking(wstrMapType, vScale, tTriangleMeshDesc, transform);
	}
	/*for (_uint i = 0; i < EMapType::End; ++i)
	{
		wstring wstrMapType = L"";
		switch (i)
		{
		case EMapType::Glow:
			wstrMapType = L"_Glow";
			break;
		case EMapType::Tutorial:
			wstrMapType = L"_Tutorial";
			break;
		case EMapType::Door:
			wstrMapType = L"_Door";
			break;
		case EMapType::Boss:
			wstrMapType = L"_Boss";
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
	}*/


	/*
	// 서브셋 메쉬 굽기 예제
	iSubsetcount = m_pMesh_Boss->Get_SubsetCount();

	vMax = m_pMesh_Boss->Get_MaxPos();
	vMin = m_pMesh_Boss->Get_MinPos();

	vecVertices = m_pMesh_Boss->Get_VecVertices();
	VecIndtices = m_pMesh_Boss->Get_VecIndtices();
	vecVerticesCount = m_pMesh_Boss->Get_VecVerticesCount();
	VecIndticesCount = m_pMesh_Boss->Get_VecIndticesCount();

	for (_ulong i = 0; i < iSubsetcount; ++i)
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		_float3 vScale = m_pTransform->Get_TransformDesc().vScale;

		PxTriangleMeshDesc tTriangleMeshDesc;
		tTriangleMeshDesc.points.count = vecVerticesCount[i];
		tTriangleMeshDesc.points.data = vecVertices[i];
		tTriangleMeshDesc.points.stride = sizeof(VTX);
		tTriangleMeshDesc.triangles.count = VecIndticesCount[i];
		tTriangleMeshDesc.triangles.data = VecIndtices[i];
		if (m_pMesh_Boss->Get_IndexSize() == sizeof(INDEX32))
			tTriangleMeshDesc.triangles.stride = sizeof(INDEX32);
		else
		{
			tTriangleMeshDesc.triangles.stride = sizeof(INDEX16);
			tTriangleMeshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
		}

		PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
		pManagement->TriangleMeshCooking(vScale, tTriangleMeshDesc, transform);
	}*/
	return S_OK;
}

CMap_Stage1 * CMap_Stage1::Create(_DEVICE _pDevice)
{
	CMap_Stage1* pInstance = new CMap_Stage1(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMap_Stage1::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CMap_Stage1* pClone = new CMap_Stage1(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMap_Stage1::Free()
{
	CGameObject::Free();
}
