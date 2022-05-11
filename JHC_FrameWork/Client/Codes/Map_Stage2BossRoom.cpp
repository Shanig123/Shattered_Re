#include "stdafx.h"
#include "Map_Stage2BossRoom.h"
#include "Effect_Texture.h"

CMap_Stage2BossRoom::CMap_Stage2BossRoom(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pDistortion(nullptr)
{
	for (_uint i = 0; i < EMapType::End; ++i)
	{
		m_pMesh[i] = nullptr;
	}
}

CMap_Stage2BossRoom::CMap_Stage2BossRoom(const CMap_Stage2BossRoom & _rOther)
	: CGameObject(_rOther)
	, m_pDistortion(nullptr)
{
}

HRESULT CMap_Stage2BossRoom::Ready_GameObject_Prototype()
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
	if (FAILED(CMap_Stage2BossRoom::Clone_Distortion()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMap_Stage2BossRoom::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMap_Stage2BossRoom::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	m_pTransform->Update_Transform();
	if (m_pDistortion)
	{
		m_pDistortion->Set_Scale(_float3(11.f,40.f, 1.f));
		m_pDistortion->Set_Pos(_float3(-40.f, 120.f, 725.f));
		//dynamic_cast<CTransform*>(m_pDistortion->Get_Component(L"Com_Transform"))
		//	->Set_Rotate(EAxis::Y, D3DXToRadian(70.f));
		m_pDistortion->Set_Rot(_float3(0.f, D3DXToRadian(-30.f), 0.f));
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_ENDING)
	{
		m_pDistortion->Set_RenderSwitch(true);
	}
	else
	{
		m_pDistortion->Set_RenderSwitch(false);
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	return (_uint)EEvent::None;
}

HRESULT CMap_Stage2BossRoom::Render_GameObject()
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

		m_pMesh[i]->Render_Meshes(pEffect);
	}

	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CMap_Stage2BossRoom::Add_Component()
{
	/* For.Com_DynamicMesh */
	wstring wstrProtoName = L"StaticMesh_Map_";
	wstring wstrCloneName = L"Mesh_";
	wstring wstrMapType = L"";

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		switch (i)
		{
		case EMapType::BossRoomCircle:
			wstrMapType = to_wstring(i) + L"_BossRoomCircle";
			break;
		case EMapType::BossRoomFloor1:
			wstrMapType = to_wstring(i) + L"_BossRoomFloor1";
			break;
		case EMapType::BossRoomFloor2:
			wstrMapType = to_wstring(i) + L"_BossRoomFloor2";
			break;
		case EMapType::BossRoomGrill:
			wstrMapType = to_wstring(i) + L"_BossRoomGrill";
			break;
		case EMapType::BossRoomInFance:
			wstrMapType = to_wstring(i) + L"_BossRoomInFance";
			break;
		case EMapType::BossRoomOutFance:
			wstrMapType = to_wstring(i) + L"_BossRoomOutFance";
			break;
		case EMapType::BossRoomPillar1:
			wstrMapType = to_wstring(i) + L"_BossRoomPillar1";
			break;
		case EMapType::BossRoomPillar2:
			wstrMapType = to_wstring(i) + L"_BossRoomPillar2";
			break;
		case EMapType::BossRoomRock:
			wstrMapType = to_wstring(i) + L"_BossRoomRock";
			break;
		case EMapType::BossRoomStair:
			wstrMapType = to_wstring(i) + L"_BossRoomStair";
			break;
		case EMapType::BossRoomEntrance:
			wstrMapType = to_wstring(i) + L"_BossRoomEntrance";
			break;
		case EMapType::BossRoomEntranvveFloor:
			wstrMapType = to_wstring(i) + L"_BossRoomEntranvveFloor";
			break;
		case EMapType::BossRoomWall:
			wstrMapType = to_wstring(i) + L"_BossRoomWall";
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

HRESULT CMap_Stage2BossRoom::SetUp_ConstantTable(_EFFECT _pEffect)
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

HRESULT CMap_Stage2BossRoom::Ready_MapCollider()
{
	auto pManagement = GET_MANAGEMENT;

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		wstring wstrMapType = L"";
		switch (i)
		{
		case EMapType::BossRoomCircle:
			wstrMapType = L"_BossRoomCircle";
			break;
		case EMapType::BossRoomFloor1:
			wstrMapType = L"_BossRoomFloor1";
			break;
		case EMapType::BossRoomFloor2:
			wstrMapType = L"_BossRoomFloor2";
			break;
		case EMapType::BossRoomGrill:
			wstrMapType = L"_BossRoomGrill";
			break;
		case EMapType::BossRoomInFance:
			wstrMapType = L"_BossRoomInFance";
			break;
		case EMapType::BossRoomOutFance:
			wstrMapType = L"_BossRoomOutFance";
			break;
		case EMapType::BossRoomPillar1:
			wstrMapType = L"_BossRoomPillar1";
			break;
		case EMapType::BossRoomPillar2:
			wstrMapType = L"_BossRoomPillar2";
			break;
		case EMapType::BossRoomRock:
			wstrMapType = L"_BossRoomRock";
			break;
		case EMapType::BossRoomStair:
			wstrMapType = L"_BossRoomStair";
			break;
		case EMapType::BossRoomEntrance:
			wstrMapType = L"_BossRoomEntrance";
			break;
		case EMapType::BossRoomEntranvveFloor:
			wstrMapType = L"_BossRoomEntranvveFloor";
			break;
		case EMapType::BossRoomWall:
			wstrMapType = L"_BossRoomWall";
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
	}

	return S_OK;
}

void CMap_Stage2BossRoom::Add_RenderList()
{
	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}

	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}
}

HRESULT CMap_Stage2BossRoom::Clone_Distortion()
{
	CEffect_Texture::TEXOBJ_DESC tTexDesc;
	tTexDesc.vArgb = { 1.f,1.f,1.f,1.f };

	tTexDesc.tTransformDesc.vPos = { -41.f,102.f,725.f };
	tTexDesc.tTransformDesc.vScale = { 5.2f,10.4f,1.f };
	tTexDesc.tTransformDesc.vScale *= 66.66f;

	tTexDesc.eTexMode = CEffect_Texture::eTexMode::Distortion;
	tTexDesc.eMovePattern = (ePartilcePatternType)0;
	tTexDesc.iRenderPass = 4;
	tTexDesc.iRenderId = 5;
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_Tex_Obj", (CGameObject**)&m_pDistortion, &tTexDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistortion,
		m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistortion);
	m_pDistortion->Set_RenderSwitch();
	return S_OK;
}

CMap_Stage2BossRoom * CMap_Stage2BossRoom::Create(_DEVICE _pDevice)
{
	CMap_Stage2BossRoom* pInstance = new CMap_Stage2BossRoom(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMap_Stage2BossRoom::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CMap_Stage2BossRoom* pClone = new CMap_Stage2BossRoom(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMap_Stage2BossRoom::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pDistortion);
	CGameObject::Free();
}
