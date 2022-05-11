#include "stdafx.h"
#include "Map_Stage2.h"
#include "Loading.h"

CMap_Stage2::CMap_Stage2(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pOptimization(nullptr)
	, m_bLoadingThreadStart(false)
	, m_bLoadingThreadEnd(false)
	, m_bChessToBoss(false)
{
	m_PrototypeName = L"Map_Stage2";
	for (_uint i = 0; i < EMapType::End; ++i)
	{
		m_pMesh[i] = nullptr;
	}

	for (_uint i = 0; i < EMapArea::EndArea; ++i)
	{
		m_pAreaObject[i] = nullptr;
	}	
}

CMap_Stage2::CMap_Stage2(const CMap_Stage2 & _rOther)
	: CGameObject(_rOther)
{
}

HRESULT CMap_Stage2::Ready_GameObject_Prototype()
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

	if (FAILED(Add_Area1()))
	{
		ERROR_BOX(L"Failed to Add_Area1");
		return E_FAIL;
	}

	if (FAILED(Add_Area2()))
	{
		ERROR_BOX(L"Failed to Add_Area2");
		return E_FAIL;
	}

	if (FAILED(Add_Area3()))
	{
		ERROR_BOX(L"Failed to Add_Area3");
		return E_FAIL;
	}

	if (FAILED(Add_Area4()))
	{
		ERROR_BOX(L"Failed to Add_Area4");
		return E_FAIL;
	}

	if (FAILED(Add_Area5()))
	{
		ERROR_BOX(L"Failed to Add_Area5");
		return E_FAIL;
	}

	if (FAILED(Add_Area6()))
	{
		ERROR_BOX(L"Failed to Add_Area6");
		return E_FAIL;
	}

	if (FAILED(Add_Area7()))
	{
		ERROR_BOX(L"Failed to Add_Area7");
		return E_FAIL;
	}

	if (FAILED(Add_Area8()))
	{
		ERROR_BOX(L"Failed to Add_Area8");
		return E_FAIL;
	}

	if (FAILED(Add_BossRoom()))
	{
		ERROR_BOX(L"Failed to Add_BossRoom");
		return E_FAIL;
	}

	if (FAILED(Add_Chess()))
	{
		ERROR_BOX(L"Failed to Add_Chess");
		return E_FAIL;
	}

	if(FAILED(Add_Trap()))
	{
		ERROR_BOX(L"Failed to Add_Trap");
		return E_FAIL;
	}

	_float4Q Quat;
	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-5.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	PxTransform transform = PxTransform(-40.f, 70.f, 165.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Trigger(L"Stage2_Lateral_1", transform, EInteractionType::Box, PxBoxGeometry(28.f, 40.f, 75.f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-30.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(-7.f, 70.f, 295.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Trigger(L"Stage2_Lateral_2", transform, EInteractionType::Box, PxBoxGeometry(75.f, 40.f, 40.f));

	if (!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_STAGE2_CHESS_END))
	{
		D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(45.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
		transform = PxTransform(120.f, 40.f, 269.f);
		transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
		Get_Manager()->Add_Obstacle(L"Stage2_ChessToBoss", transform, _float3(15.f, 10.f, 1.f));
	}
	else
	{
		m_bChessToBoss = true;
	}

#pragma region ObstacleWall
	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-40.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(44.f, 32.5f, 52.5f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_1", transform, _float3(37.f, 20.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-25.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(-5.f, 50.f, 72.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_2", transform, _float3(48.f, 20.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-65.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(42.5f, 45.f, 104.6f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_3", transform, _float3(12.f, 15.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(50.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(56.3f, 50.f, 85.3f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_4", transform, _float3(20.f, 25.f, 4.f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-65.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(68.8f, 45.f, 160.5f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_5", transform, _float3(23.f, 15.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-5.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(-29.4f, 70.f, 88.7f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_6", transform, _float3(25.f, 25.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(84.5f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(-11.5f, 70.f, 160.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_7", transform, _float3(70.f, 25.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(45.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(66.5f, 50.f, 194.5f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_8", transform, _float3(43.f, 15.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(24.5f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(13.f, 43.0f, 235.6f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_9", transform, _float3(25.f, 10.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-30.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(20.f, 75.7f, 261.6f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_10", transform, _float3(53.f, 15.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(60.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(67.7f, 75.7f, 313.8f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_11", transform, _float3(15.f, 15.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-10.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(100.f, 70.f, 274.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_12", transform, _float3(45.f, 25.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(45.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(97.f, 65.f, 290.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_13", transform, _float3(18.f, 25.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(45.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(143.f, 47.f, 244.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_14", transform, _float3(21.5f, 10.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(20.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(150.f, 55.f, 375.1f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_15", transform, _float3(27.f, 10.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(63.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(118.f, 45.f, 376.6f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_16", transform, _float3(5.f, 10.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(60.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(228.f, 65.f, 325.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_17", transform, _float3(30.f, 40.f, 0.5f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(20.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(194.f, 65.f, 358.5f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Obstacle(L"Stage2_ObstacleWall_18", transform, _float3(20.f, 40.f, 0.5f));
#pragma endregion


	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(50.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(206.f, 9.f, 237.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Trigger(L"Stage2_Trap", transform, EInteractionType::Box, PxBoxGeometry(45.f, 10.f, 120.f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-30.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(110.3f, 37.f, 335.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Trigger(L"Stage2_ChurchDoor", transform, EInteractionType::Box, PxBoxGeometry(5.f, 3.f, 3.f));

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-30.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	transform = PxTransform(75.f, 29.f, 525.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->Add_Trigger(L"Stage2BossStart", transform, EInteractionType::Box, PxBoxGeometry(4.f, 5.f, 4.f));

	return S_OK;
}

HRESULT CMap_Stage2::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMap_Stage2::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	auto pManagement = GET_MANAGEMENT;

	if ((Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_STAGE2_CHESS_END) && !m_bChessToBoss)
	{
		pManagement->Delete_Obstacle(L"Stage2_ChessToBoss");
	}
	_float4Q Quat;

	D3DXQuaternionRotationYawPitchRoll(&Quat, D3DXToRadian(-30.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
	PxTransform transform = PxTransform(110.3f, 40.f, 335.f);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);
	Get_Manager()->SetPos_Trigger(L"Stage2_ChurchDoor", transform);


	/*if (!m_bLoadingThreadStart)
	{
		m_pLoading = CLoading::Create(m_pDevice, ESceneType::Stage2Area);
		m_bLoadingThreadStart = true;
	}

	if (!m_bLoadingThreadEnd)
	{
		if ((_uint)EEvent::Dead == m_pLoading->Get_Finish())
		{
			pManagement->Set_StaticObjToScene((_int)ESceneType::Stage2);
			m_bLoadingThreadEnd = true;
		}
	}*/
	
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

	Render_Area1();
	Render_Area2();
	Render_Area3();
	Render_Area4();
	Render_Area5();
	Render_Area6();
	Render_Area7();
	Render_Area8();
	Render_BossRoom();
	Render_Chess();
	Render_Trap();

	m_pTransform->Update_Transform();

	return (_uint)EEvent::None;
}

HRESULT CMap_Stage2::Render_GameObject()
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

HRESULT CMap_Stage2::Add_Component()
{
	/* For.Com_DynamicMesh */
	wstring wstrProtoName = L"StaticMesh_Map_";
	wstring wstrCloneName = L"Mesh_";
	wstring wstrMapType = L"";

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		switch (i)
		{
		case EMapType::Stage2Start:
			wstrMapType = to_wstring(i) + L"_Stage2Start";
			break;
		case EMapType::Stage2Floor:
			wstrMapType = to_wstring(i) + L"_Stage2Floor";
			break;
		case EMapType::Stage2Wall:
			wstrMapType = to_wstring(i) + L"_Stage2Wall";
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

	if (FAILED(CGameObject::Add_Component
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
	}

	return S_OK;
}

HRESULT CMap_Stage2::SetUp_ConstantTable(_EFFECT _pEffect)
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

HRESULT CMap_Stage2::Ready_MapCollider()
{
	auto pManagement = GET_MANAGEMENT;

	for (_uint i = 0; i < EMapType::End; ++i)
	{
		wstring wstrMapType = L"";
		switch (i)
		{
		case EMapType::Stage2Start:
			wstrMapType = L"_Stage2Start";
			break;
		case EMapType::Stage2Floor:
			wstrMapType = L"_Stage2Floor";
			break;
		case EMapType::Stage2Wall:
			wstrMapType = L"_Stage2Wall";
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

HRESULT CMap_Stage2::Add_Area1()
{
	m_pAreaObject[EMapArea::Area1] = CMap_Stage2Area1::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area1], E_FAIL);
	m_pAreaObject[EMapArea::Area1]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area1], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap_Stage2::Add_Area2()
{
	m_pAreaObject[EMapArea::Area2] = CMap_Stage2Area2::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area2], E_FAIL);
	m_pAreaObject[EMapArea::Area2]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area2], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CMap_Stage2::Add_Area3()
{
	m_pAreaObject[EMapArea::Area3] = CMap_Stage2Area3::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area3], E_FAIL);
	m_pAreaObject[EMapArea::Area3]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area3], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CMap_Stage2::Add_Area4()
{
	m_pAreaObject[EMapArea::Area4] = CMap_Stage2Area4::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area4], E_FAIL);
	m_pAreaObject[EMapArea::Area4]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area4], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CMap_Stage2::Add_Area5()
{
	m_pAreaObject[EMapArea::Area5] = CMap_Stage2Area5::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area5], E_FAIL);
	m_pAreaObject[EMapArea::Area5]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area5], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap_Stage2::Add_Area6()
{
	m_pAreaObject[EMapArea::Area6] = CMap_Stage2Area6::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area6], E_FAIL);
	m_pAreaObject[EMapArea::Area6]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area6], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap_Stage2::Add_Area7()
{
	m_pAreaObject[EMapArea::Area7] = CMap_Stage2Area7::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area7], E_FAIL);
	m_pAreaObject[EMapArea::Area7]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area7], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap_Stage2::Add_Area8()
{
	m_pAreaObject[EMapArea::Area8] = CMap_Stage2Area8::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Area8], E_FAIL);
	m_pAreaObject[EMapArea::Area8]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Area8], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap_Stage2::Add_BossRoom()
{
	m_pAreaObject[EMapArea::BossRoom] = CMap_Stage2BossRoom::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::BossRoom], E_FAIL);
	m_pAreaObject[EMapArea::BossRoom]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::BossRoom], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap_Stage2::Add_Chess()
{
	m_pAreaObject[EMapArea::Chess] = CMap_Stage2Chess::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Chess], E_FAIL);
	m_pAreaObject[EMapArea::Chess]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Chess], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap_Stage2::Add_Trap()
{
	m_pAreaObject[EMapArea::Trap] = CMap_Stage2Trap::Create(m_pDevice);
	NULL_CHECK_RETURN(m_pAreaObject[EMapArea::Trap], E_FAIL);
	m_pAreaObject[EMapArea::Trap]->Set_ForceCheckClone();

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pAreaObject[EMapArea::Trap], (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	return S_OK;
}

void CMap_Stage2::Render_Area1()
{
	if (nullptr == m_pAreaObject[EMapArea::Area1])
		return;

	CMap_Stage2Area1* pArea1 = dynamic_cast<CMap_Stage2Area1*>(m_pAreaObject[EMapArea::Area1]);
	_uint iCount = pArea1->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea1->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea1->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea1->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea1->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Area2()
{
	if (nullptr == m_pAreaObject[EMapArea::Area2])
		return;

	CMap_Stage2Area2* pArea2 = dynamic_cast<CMap_Stage2Area2*>(m_pAreaObject[EMapArea::Area2]);
	_uint iCount = pArea2->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea2->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea2->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea2->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea2->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Area3()
{
	if (nullptr == m_pAreaObject[EMapArea::Area3])
		return;

	CMap_Stage2Area3* pArea3 = dynamic_cast<CMap_Stage2Area3*>(m_pAreaObject[EMapArea::Area3]);
	_uint iCount = pArea3->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea3->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea3->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea3->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea3->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Area4()
{
	if (nullptr == m_pAreaObject[EMapArea::Area4])
		return;

	CMap_Stage2Area4* pArea4 = dynamic_cast<CMap_Stage2Area4*>(m_pAreaObject[EMapArea::Area4]);	

	_uint iCount = pArea4->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea4->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea4->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea4->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea4->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Area5()
{
	if (nullptr == m_pAreaObject[EMapArea::Area5])
		return;

	CMap_Stage2Area5* pArea5 = dynamic_cast<CMap_Stage2Area5*>(m_pAreaObject[EMapArea::Area5]);

	_uint iCount = pArea5->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea5->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea5->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea5->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea5->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Area6()
{
	if (nullptr == m_pAreaObject[EMapArea::Area6])
		return;

	CMap_Stage2Area6* pArea6 = dynamic_cast<CMap_Stage2Area6*>(m_pAreaObject[EMapArea::Area6]);

	_uint iCount = pArea6->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea6->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea6->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea6->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea6->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Area7()
{
	if (nullptr == m_pAreaObject[EMapArea::Area7])
		return;

	CMap_Stage2Area7* pArea7 = dynamic_cast<CMap_Stage2Area7*>(m_pAreaObject[EMapArea::Area7]);

	_uint iCount = pArea7->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea7->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea7->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea7->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea7->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Area8()
{
	if (nullptr == m_pAreaObject[EMapArea::Area8])
		return;

	CMap_Stage2Area8* pArea8 = dynamic_cast<CMap_Stage2Area8*>(m_pAreaObject[EMapArea::Area8]);

	_uint iCount = pArea8->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pArea8->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pArea8->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pArea8->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pArea8->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_BossRoom()
{
	if (nullptr == m_pAreaObject[EMapArea::BossRoom])
		return;

	CMap_Stage2BossRoom* pBossRoom = dynamic_cast<CMap_Stage2BossRoom*>(m_pAreaObject[EMapArea::BossRoom]);

	_uint iCount = pBossRoom->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pBossRoom->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pBossRoom->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pBossRoom->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pBossRoom->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Chess()
{
	if (nullptr == m_pAreaObject[EMapArea::Chess])
		return;

	CMap_Stage2Chess* pChess = dynamic_cast<CMap_Stage2Chess*>(m_pAreaObject[EMapArea::Chess]);

	_uint iCount = pChess->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pChess->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pChess->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pChess->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pChess->Add_RenderList();
			break;
		}
	}
}

void CMap_Stage2::Render_Trap()
{

	if (nullptr == m_pAreaObject[EMapArea::Trap])
		return;

	CMap_Stage2Trap* pTrap = dynamic_cast<CMap_Stage2Trap*>(m_pAreaObject[EMapArea::Trap]);

	_uint iCount = pTrap->Get_StageMeshCount();
	const _float4x4* matWorld = dynamic_cast<CTransform*>(pTrap->Get_Component(L"Com_Transform"))->Get_WorldMatrix();
	m_pOptimization->ConstructFrustum(STAGE2_CULL);

	for (_uint i = 0; i < iCount; ++i)
	{
		_float3 vCenter = pTrap->Get_StageMesh(i)->Get_ObjectCenterPos();
		_float fRadius = pTrap->Get_StageMesh(i)->Get_ObjectRadius();

		if (m_pOptimization->CheckSphere_Frustum(vCenter.x, vCenter.y, vCenter.z, fRadius))
		{
			pTrap->Add_RenderList();
			break;
		}
	}
}

CMap_Stage2 * CMap_Stage2::Create(_DEVICE _pDevice)
{
	CMap_Stage2* pInstance = new CMap_Stage2(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMap_Stage2::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CMap_Stage2* pClone = new CMap_Stage2(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMap_Stage2::Free()
{
	CGameObject::Free();
}
