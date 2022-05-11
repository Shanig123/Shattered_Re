#include "PhysX_Manager.h"
#ifndef _AFXDLL
USING(Engine)

IMPLEMENT_SINGLETON(CPhysX_Manager)

CPhysX_Manager::CPhysX_Manager()
	: m_pPvD(nullptr)
	, m_pTransport(nullptr)
	, m_pScene(nullptr)
	, m_pFoundation(nullptr)
	, m_pPhysics(nullptr)
	, m_pCooking(nullptr)
	, m_pDispatcher(nullptr)
	//, m_pCudaContextManager(nullptr)
	, m_pControllerManager(nullptr)
	, m_pFilterData(nullptr)
	, m_pFilterCallback(nullptr)
	, m_pCCTFilterCallback(nullptr)
	, m_pObstacleContext(nullptr)
	, m_fFlag(0)
	, m_bObstacle(false)
	, m_itest(0)
	, m_RayCast(nullptr)
	, m_CameraRayCast(nullptr)
{
}

HRESULT CPhysX_Manager::Ready_PhysX_Manager()
{
	char* strTransport = "127.0.0.1";

	PxAllocatorCallback* allocator = &m_Allocator;


	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *allocator, m_ErrorCallback);


	m_pTransport = PxDefaultPvdSocketTransportCreate(strTransport, 5425, 1000);

	m_pPvD = PxCreatePvd(*m_pFoundation);
	_bool bPVDConnectionResult = m_pPvD->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);
	if (!bPVDConnectionResult)
	{
		//연결 실패함
		//ERROR_BOX(L"Failed to Connect");
		//return false;
	}

	//PxCudaContextManagerDesc cudaContextManagerDesc;
	//m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, cudaContextManagerDesc);

	PxTolerancesScale Scale;
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, Scale, true, m_pPvD);

	if (!PxInitExtensions(*m_pPhysics, m_pPvD))
	{
		ERROR_BOX(L"Failed to PxInitExtensions");
		return E_FAIL;
	}

	PxCookingParams params(Scale);
	// 메쉬 용접
	params.meshWeldTolerance = 0.01f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	//params.buildGPUData = true; //Enable GRB data being produced in cooking.

	//params.suppressTriangleMeshRemapTable = false;

	//params.convexMeshCookingType != PxConvexFlag::eCOMPUTE_CONVEX;
	//params.convexMeshCookingType != PxConvexFlag::eINFLATE_CONVEX;

	//eFORCE_32BIT_INDICES 
	// disable mesh cleaning - perform mesh validation on development configurations
	//params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	// disable edge precompute, edges are set for each triangle, slows contact generation
	//params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	// lower hierarchy for internal mesh
	params.meshPreprocessParams = PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
	params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, params);
	m_RayCast = new MyRayCast;
	m_CameraRayCast = new MyRayCast;

	m_fAccumulator = 0.f;
	m_fStepSize = 1.f / 60.f;

	CreateScene_PhysX_Manager();

	return S_OK;
}

HRESULT CPhysX_Manager::CreateScene_PhysX_Manager()
{

	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());

	//중력
	SceneDesc.gravity = PxVec3(0.f, -9.81f, 0.f);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	SceneDesc.cpuDispatcher = m_pDispatcher;
	//SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	//SceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So kin-kin contacts with be reported
	//SceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So static-kin constacts will be reported
	SceneDesc.simulationEventCallback = this;
	SceneDesc.filterShader = contactReportFilterShader;

	//SceneDesc.gpuDispatcher = m_pCudaContextManager->getGpuDispatcher();
	SceneDesc.flags != PxSceneFlag::eENABLE_KINEMATIC_PAIRS;
	//SceneDesc.flags != PxSceneFlag::eDEPRECATED_TRIGGER_TRIGGER_REPORTS;
	//SceneDesc.flags != PxSceneFlag::eENABLE_PCM;
	//SceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	//SceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_AVERAGE_POINT;
	SceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	//SceneDesc.flags != PxSceneFlag::eREQUIRE_RW_LOCK;
	//sceneDesc.flags |= PxSceneFlag::eADAPTIVE_FORCE;
	//SceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	//SceneDesc.sceneQueryUpdateMode = PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_DISABLED;
	//sceneDesc.flags |= PxSceneFlag::eDISABLE_CONTACT_CACHE;
	//SceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	SceneDesc.gpuMaxNumPartitions = 32;

	m_pScene = m_pPhysics->createScene(SceneDesc);

	PxSceneWriteLock scopedLock(*m_pScene);

	//PVD
	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();

	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_pControllerManager = PxCreateControllerManager(*m_pScene);
	m_pObstacleContext = m_pControllerManager->createObstacleContext();

	return S_OK;
}

_bool CPhysX_Manager::Render_PhysX_Manager(const _float & _fDeltaTime)
{
	/*m_fAccumulator += _fDeltaTime;
	if (m_fAccumulator >= m_fStepSize)
	{
		m_fAccumulator = 0.f;

		m_pScene->simulate(m_fStepSize);
		m_pScene->fetchResults(true);
		return true;
	}*/

	/*m_fAccumulator += _fDeltaTime;
	if (m_fAccumulator < m_fStepSize)
		return false;

	m_fAccumulator -= m_fStepSize;

	m_pScene->simulate(m_fStepSize);
	m_pScene->fetchResults(true);
	return true;*/

	


	if (nullptr != m_pScene)
	{
		m_pScene->simulate(_fDeltaTime);
		m_pScene->fetchResults(true);
	}

	return false;
}

void CPhysX_Manager::Free_PhysX_Manager()
{
	if (nullptr == m_pFoundation)
		return;

	PxCloseExtensions();

	if (m_pObstacleContext)
	{
		m_pObstacleContext->release();
		m_pObstacleContext = NULL;
	}

	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		Safe_Delete(m_pControllerManager->getController(i)->getActor()->userData);
	}

	if (m_pControllerManager)
	{
		m_pControllerManager->purgeControllers();
		m_pControllerManager->release();
		m_pControllerManager = NULL;
	}

	if (m_pScene)
	{
		m_pScene->release();
		m_pScene = NULL;
	}

	if (m_pPhysics)
	{
		m_pPhysics->release();
		m_pPhysics = NULL;
	}

	if (m_pPvD)
	{
		m_pPvD->release();
		m_pPvD = NULL;
	}

	if (m_pDispatcher)
	{
		m_pDispatcher->release();
		m_pDispatcher = NULL;
	}

	if (m_pTransport)
	{
		m_pTransport->release();
		m_pTransport = NULL;
	}

	if (m_pCooking)
	{
		m_pCooking->release();
		m_pCooking = NULL;
	}

	if (m_pFoundation)
	{
		m_pFoundation->release();
		m_pFoundation = NULL;
	}

	for (auto& rObj : m_vecActroName)
	{
		Safe_Delete(rObj);
	}

	m_vecActroName.clear();

	for (auto& rObj : m_vecPublicData)
	{
		Safe_Delete(rObj);
	}

	m_vecPublicData.clear();

	m_mapObstacleHandle.clear();

	Safe_Delete(m_RayCast);
	Safe_Delete(m_CameraRayCast);
}

void CPhysX_Manager::TriangleMeshCooking(const wstring& _Name, _float3 _vScale, PxTriangleMeshDesc & _rMeshCooking, PxTransform & _rTransform)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;

	_bool status = m_pCooking->cookTriangleMesh(_rMeshCooking, writeBuffer, &result);
	if (!status)
	{
		ERROR_BOX(L"Failed to TriangleMeshCooking");
		return;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triangleMesh = m_pPhysics->createTriangleMesh(readBuffer);

	if (triangleMesh)
	{
		PxTriangleMeshGeometry triGeom;
		triGeom.triangleMesh = triangleMesh;
		triGeom.scale = PxMeshScale(PxVec3(_vScale.x, _vScale.y, _vScale.z));
		PxRigidStatic* actor = m_pPhysics->createRigidStatic(_rTransform);
		m_vecActroName.push_back(WstringToChar(_Name));
		actor->setName(m_vecActroName.back());
		PxMaterial* pMaterial = m_pPhysics->createMaterial(1.f, 1.f, 0.f);
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*actor, triGeom, *pMaterial);
		PX_UNUSED(shape);
		setupFiltering(actor, MyFilterGroup::eTerrain, 0);
		m_pScene->addActor(*actor);
	}
}

void CPhysX_Manager::Add_Controller(const wstring& _Name, _float3 _vPos, EInteractionType _eType, PxGeometry& _Geometry)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxControllerDesc* cDesc = nullptr;
	if (EInteractionType::Capsule == _eType)
	{
		PxCapsuleGeometry Geometry;
		Geometry = (PxCapsuleGeometry&)_Geometry;
		PxCapsuleControllerDesc ShapeDesc;
		ShapeDesc.height = Geometry.halfHeight;
		ShapeDesc.radius = Geometry.radius;
		ShapeDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;

		cDesc = &ShapeDesc;
	}
	else if (EInteractionType::Box == _eType)
	{
		PxBoxGeometry Geometry;
		Geometry = (PxBoxGeometry&)_Geometry;
		PxBoxControllerDesc ShapeDesc;
		ShapeDesc.halfHeight = Geometry.halfExtents.x;
		ShapeDesc.halfSideExtent = Geometry.halfExtents.y;
		ShapeDesc.halfForwardExtent = Geometry.halfExtents.z;

		cDesc = &ShapeDesc;
	}
	
	cDesc->density = 10.f;												// 밀도
	cDesc->scaleCoeff = 1.f;											// 사이즈?
	cDesc->material = m_pPhysics->createMaterial(PxReal(1.0f), PxReal(1.0f), PxReal(0.05));
	cDesc->position = PxExtendedVec3(_vPos.x, _vPos.y, _vPos.z);		// 위치
	cDesc->slopeLimit = 0.f;											// 기울기
	cDesc->contactOffset = 0.1f;										// 접촉 오프셋 - 이유 모름
	cDesc->stepOffset = 0.05f;											// 단계 오프셋 - 이유 모름
	cDesc->invisibleWallHeight = 0.f;									// 보이지 않는 벽 신장??
	cDesc->maxJumpHeight = 0.f;											// 점프 높이??
	cDesc->reportCallback = this;
	cDesc->behaviorCallback = this;
	cDesc->volumeGrowth = 1.5f;

	if (_Name.find(L"Preto") != wstring::npos)
	{
		cDesc->stepOffset = 0.f;
	}

	PxController* m_pController = static_cast<PxBoxController*>(m_pControllerManager->createController(*cDesc));
	PxRigidDynamic* Actor = m_pController->getActor();

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());

	PxShape* pShape = nullptr;
	Actor->getShapes(&pShape, 1);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	pShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);	

	if (!lstrcmpi(L"Player", _Name.c_str()))
	{
		setupFiltering(Actor, MyFilterGroup::ePlayer, MyFilterGroup::eObject | MyFilterGroup::eMonster | MyFilterGroup::eMonsterWeapon);

		MyPlayerData* Data = new MyPlayerData;
		lstrcpy(Data->szName, _Name.c_str());
		Actor->userData = Data;
	}
	else
	{
		setupFiltering(Actor, MyFilterGroup::eMonster, MyFilterGroup::ePlayer | MyFilterGroup::ePlayerWeapon);

		MyMonsterData* Data = new MyMonsterData;
		lstrcpy(Data->szName, _Name.c_str());
		Actor->userData = Data;
	}	
}

void CPhysX_Manager::Add_Controller(const wstring & _Name, PxTransform & _rTransform, EInteractionType _eType, PxGeometry & _Geometry)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxControllerDesc* cDesc = nullptr;
	if (EInteractionType::Capsule == _eType)
	{
		PxCapsuleGeometry Geometry;
		Geometry = (PxCapsuleGeometry&)_Geometry;
		PxCapsuleControllerDesc ShapeDesc;
		ShapeDesc.height = Geometry.halfHeight;
		ShapeDesc.radius = Geometry.radius;
		ShapeDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;

		cDesc = &ShapeDesc;
	}
	else if (EInteractionType::Box == _eType)
	{
		PxBoxGeometry Geometry;
		Geometry = (PxBoxGeometry&)_Geometry;
		PxBoxControllerDesc ShapeDesc;
		ShapeDesc.halfHeight = Geometry.halfExtents.y;
		ShapeDesc.halfSideExtent = Geometry.halfExtents.x;
		ShapeDesc.halfForwardExtent = Geometry.halfExtents.z;
		ShapeDesc.upDirection = PxVec3(1, 0, 0);

		cDesc = &ShapeDesc;
	}

	cDesc->density = 10.f;																		// 밀도
	cDesc->scaleCoeff = 1.f;																	// 사이즈?
	cDesc->material = m_pPhysics->createMaterial(PxReal(1.0f), PxReal(1.0f), PxReal(0.05));
	cDesc->position = PxExtendedVec3(_rTransform.p.x, _rTransform.p.y, _rTransform.p.z);		// 위치
	cDesc->slopeLimit = 0.f;																	// 기울기
	cDesc->contactOffset = 0.1f;																// 접촉 오프셋 - 이유 모름
	cDesc->stepOffset = 0.05f;																	// 단계 오프셋 - 이유 모름
	cDesc->invisibleWallHeight = 0.f;															// 보이지 않는 벽 신장??
	cDesc->maxJumpHeight = 0.f;																	// 점프 높이??
	cDesc->reportCallback = this;
	cDesc->behaviorCallback = this;
	cDesc->volumeGrowth = 1.5f;

	PxController* m_pController = static_cast<PxBoxController*>(m_pControllerManager->createController(*cDesc));
	PxRigidDynamic* Actor = m_pController->getActor();

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());

	PxShape* pShape;
	Actor->getShapes(&pShape, 1);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	pShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

	if (_Name.find(L"Parry") != wstring::npos)
	{
		setupFiltering(Actor, MyFilterGroup::eParry, MyFilterGroup::eMonsterWeapon);
		Actor->userData = new MyPublicData;

		PxShape* shapes[1];
		Actor->getShapes(shapes, 1, 0);
		PxShape* shape = shapes[0];
		shape->setLocalPose(PxTransform(PxQuat(_rTransform.q)));
	}
}

void CPhysX_Manager::Add_Interaction(const wstring& _Name, _float3 _vPos, EInteractionType _eRigidType, PxGeometry& _RigidGeometry, EInteractionType _eTriggerType, PxGeometry& _TriggerGeometry)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxRigidStatic* Actor = m_pPhysics->createRigidStatic(PxTransform(PxVec3(_vPos.x, _vPos.y, _vPos.z)));
	PxMaterial* Material = m_pPhysics->createMaterial(PxReal(1.0f), PxReal(1.0f), PxReal(0.05));

	PxShape* pShape;
	if (EInteractionType::Capsule == _eRigidType)
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);
	}
	else
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	}

	PxShape* pTriggerShape;
	if (EInteractionType::Sphere == _eTriggerType)
	{
		pTriggerShape = PxRigidActorExt::createExclusiveShape(*Actor, _TriggerGeometry, *Material);
	}
	else
	{
		pTriggerShape = PxRigidActorExt::createExclusiveShape(*Actor, _TriggerGeometry, *Material);
	}

	pTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());
	MyPublicData* Data = new MyPublicData;
	lstrcpy(Data->szName, _Name.c_str());
	m_vecPublicData.push_back(Data);
	Actor->userData = m_vecPublicData.back();

	setupFiltering(Actor, MyFilterGroup::eObject, MyFilterGroup::ePlayer);

	m_pScene->addActor(*Actor);
}

void CPhysX_Manager::Add_StaticActor(const wstring & _Name, PxTransform& _rTransform, EInteractionType _eRigidType, PxGeometry & _RigidGeometry)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxRigidStatic* Actor = m_pPhysics->createRigidStatic(_rTransform);
	PxMaterial* Material = m_pPhysics->createMaterial(PxReal(1.0f), PxReal(1.0f), PxReal(0.05));

	PxShape* pShape;
	if (EInteractionType::Capsule == _eRigidType)
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);
	}
	else
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	}

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());
	MyPublicData* Data = new MyPublicData;
	lstrcpy(Data->szName, _Name.c_str());
	m_vecPublicData.push_back(Data);
	Actor->userData = Data;

	m_pScene->addActor(*Actor);
}

void CPhysX_Manager::Add_DynamicActor(const wstring & _Name, PxTransform & _rTransform, EInteractionType _eRigidType, PxGeometry & _RigidGeometry)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxRigidDynamic* Actor = m_pPhysics->createRigidDynamic(_rTransform);
	PxMaterial* Material = m_pPhysics->createMaterial(PxReal(0.5f), PxReal(0.5f), PxReal(0.5f));

	PxShape* pShape;
	if (EInteractionType::Capsule == _eRigidType)
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);
	}
	else
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	}

	if (_Name.find(L"ShortCutDoor") != wstring::npos)
	{
		Actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		setupFiltering(Actor, MyFilterGroup::eAttackObject, MyFilterGroup::ePlayerWeapon);
	}

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());
	MyPublicData* Data = new MyPublicData;
	lstrcpy(Data->szName, _Name.c_str());
	m_vecPublicData.push_back(Data);
	Actor->userData = Data;

	m_pScene->addActor(*Actor);
}

void CPhysX_Manager::Add_DynamicActor(const wstring & _Name, PxTransform & _rTransform, _float _fDamage, EInteractionType _eRigidType, PxGeometry & _RigidGeometry)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxRigidDynamic* Actor = m_pPhysics->createRigidDynamic(_rTransform);
	PxMaterial* Material = m_pPhysics->createMaterial(PxReal(0.5f), PxReal(0.5f), PxReal(0.5f));

	PxShape* pShape;
	if (EInteractionType::Capsule == _eRigidType)
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
		pShape->setLocalPose(relativePose);
	}
	else
	{
		pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
		Actor->getShapes(&pShape, 1);
		pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	}

	if (_Name.find(L"_DownSkill") != wstring::npos)
	{
		Actor->setMass(1000.f);
		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		Actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		setupFiltering(Actor, MyFilterGroup::eObject, MyFilterGroup::ePlayer);
	}

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());
	MyPublicData* Data = new MyPublicData;
	lstrcpy(Data->szName, _Name.c_str());
	Data->Damage = _fDamage;
	m_vecPublicData.push_back(Data);
	Actor->userData = Data;

	m_pScene->addActor(*Actor);
}

void CPhysX_Manager::Add_Weapon(const wstring & _Name, _float3 _vPos, _float _fDamage, PxGeometry& _RigidGeometry, _float _fFocusY, _bool _bVertical)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxRigidStatic* Actor = m_pPhysics->createRigidStatic(PxTransform(PxVec3(_vPos.x, _vPos.y, _vPos.z)));
	PxMaterial* Material = m_pPhysics->createMaterial(PxReal(1.0f), PxReal(1.0f), PxReal(0.05));

	PxShape* pShape = PxRigidActorExt::createExclusiveShape(*Actor, _RigidGeometry, *Material);
	Actor->getShapes(&pShape, 1);
	pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);	
	if (PxGeometryType::Enum::eCAPSULE == _RigidGeometry.getType())
	{
		if (_bVertical)
		{
			PxTransform relativePose(PxVec3(0.f, _fFocusY, 0.f), PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
			pShape->setLocalPose(relativePose);
		}		
	}
	else if (PxGeometryType::Enum::eSPHERE == _RigidGeometry.getType())
	{
		PxTransform relativePose(PxVec3(0.f, _fFocusY, 0.f));
		pShape->setLocalPose(relativePose);
	}

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());
	MyPublicData* Data = new MyPublicData;
	Data->Damage = _fDamage;
	lstrcpy(Data->szName, _Name.c_str());
	m_vecPublicData.push_back(Data);
	Actor->userData = Data;

	if (_Name.find(L"Player") != wstring::npos)
	{
		setupFiltering(Actor, MyFilterGroup::ePlayerWeapon, MyFilterGroup::eMonster);
	}
	else if (_Name.find(L"Preto") != wstring::npos)
	{
		setupFiltering(Actor, MyFilterGroup::eMonsterWeapon, MyFilterGroup::ePlayer);

	}
	else if (_Name.find(L"Malkhel") != wstring::npos)
	{
		setupFiltering(Actor, MyFilterGroup::eMonsterWeapon, MyFilterGroup::ePlayer);
	}
	else if (_Name.find(L"_Spear") != wstring::npos)
	{
		setupFiltering(Actor, MyFilterGroup::eMonsterWeapon, MyFilterGroup::ePlayer);
	}
	else
	{
		setupFiltering(Actor, MyFilterGroup::eMonsterWeapon, MyFilterGroup::ePlayer | MyFilterGroup::eParry);
	}

	m_pScene->addActor(*Actor);
}

void CPhysX_Manager::Add_Trigger(const wstring & _Name, PxTransform& _rTransform, EInteractionType _eType, PxGeometry& _Geometry)
{
	PxSceneWriteLock scopedLock(*m_pScene);

	PxRigidStatic* Actor = m_pPhysics->createRigidStatic(_rTransform);
	PxMaterial* Material = m_pPhysics->createMaterial(PxReal(1.0f), PxReal(1.0f), PxReal(0.05));

	PxShape* pTriggerShape;
	pTriggerShape = PxRigidActorExt::createExclusiveShape(*Actor, _Geometry, *Material);

	pTriggerShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
	pTriggerShape->setFlag(PxShapeFlag::eVISUALIZATION, true);
	pTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	pTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	m_vecActroName.push_back(WstringToChar(_Name));
	Actor->setName(m_vecActroName.back());

	MyPublicData* Data = new MyPublicData;
	lstrcpy(Data->szName, _Name.c_str());
	m_vecPublicData.push_back(Data);
	Actor->userData = Data;

	if (_Name.find(L"Preto") != wstring::npos)
	{
		setupFiltering(Actor, MyFilterGroup::eObject, MyFilterGroup::eMonster | MyFilterGroup::ePlayer);
	}
	else
	{
		setupFiltering(Actor, MyFilterGroup::eObject, MyFilterGroup::ePlayer);
	}

	m_pScene->addActor(*Actor);
}

void CPhysX_Manager::Add_Obstacle(const wstring & _Name, PxTransform& _rTransform, _float3 _vSize)
{
	m_vecActroName.push_back(WstringToChar(_Name));
	MyPublicData* Data = new MyPublicData;
	lstrcpy(Data->szName, _Name.c_str());
	m_vecPublicData.push_back(Data);

	PxBoxObstacle m_pObstacleBox;
	m_pObstacleBox.mPos = PxExtendedVec3(_rTransform.p.x, _rTransform.p.y, _rTransform.p.z);
	m_pObstacleBox.mHalfExtents = PxVec3(_vSize.x, _vSize.y, _vSize.z);
	m_pObstacleBox.mRot = _rTransform.q;
	m_pObstacleBox.mUserData = m_vecPublicData.back();

	m_mapObstacleHandle.emplace(_Name, m_pObstacleContext->addObstacle(m_pObstacleBox));	
}

_float3 CPhysX_Manager::Move(const wstring& _Name, _float3 _vPos, _float _fPower, const _float& _fDeltaTime)
{
	PxSceneReadLock scopedLock(*m_pScene);

	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			PxVec3 vDisp = PxVec3(-_vPos.x, _vPos.y, -_vPos.z);

			const PxControllerFilters filters(m_pFilterData, m_pFilterCallback, m_pCCTFilterCallback);
			m_pControllerManager->getController(i)->move(vDisp * _fPower*_fDeltaTime, 0.f, _fDeltaTime, filters, m_pObstacleContext);

			PxVec3 vPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
			PxExtendedVec3 vFoot = m_pControllerManager->getController(i)->getFootPosition();

			if (_Name.find(L"Preto") != wstring::npos)
				return _float3(vPos.x, (_float)vFoot.y, vPos.z);
			else
				return _float3(vPos.x, (_float)vFoot.y, vPos.z);
		}
	}

	return _float3();
}

void CPhysX_Manager::ActorMove(const wstring & _Name, _float3* _vOutPos, _float3 _vPos, _float _fPower, const _float & _fDeltaTime)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* actor = static_cast<PxRigidStatic*>(Actor);
			PxTransform Transform = actor->getGlobalPose();
			Transform.p.x += _vPos.x * _fPower * _fDeltaTime;
			Transform.p.y += _vPos.y * _fPower * _fDeltaTime;
			Transform.p.z += _vPos.z * _fPower * _fDeltaTime;
			actor->setGlobalPose(Transform);
			*_vOutPos = _float3(Transform.p.x, Transform.p.y, Transform.p.z);
			break;
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::ActorMove(const wstring & _Name, _float3 * _vOutPos, _float4x4* _vOutmatRotate, _float3 _vPos, _float4Q _Quat, _float _fPower, const _float & _fDeltaTime)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* actor = static_cast<PxRigidStatic*>(Actor);
			PxTransform Transform = actor->getGlobalPose();
			Transform.p.x += _vPos.x * _fPower * _fDeltaTime;
			Transform.p.y += _vPos.y * _fPower * _fDeltaTime;
			Transform.p.z += _vPos.z * _fPower * _fDeltaTime;
			Transform.q *= PxQuat(_Quat.x, _Quat.y, _Quat.z, _Quat.w);
			actor->setGlobalPose(Transform);
			*_vOutPos = _float3(Transform.p.x, Transform.p.y, Transform.p.z);
			_float4x4 matRotate;
			_float4Q Quat = _float4Q(Transform.q.x, Transform.q.y, Transform.q.z, Transform.q.w);
			D3DXMatrixRotationQuaternion(&matRotate, &Quat);
			*_vOutmatRotate = matRotate;
			break;
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::ActorAddForce(const wstring & _Name, _float3 * _vOutPos, _float3 _vPos, _float _fPower, const _float & _fDeltaTime)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidDynamic* actor = static_cast<PxRigidDynamic*>(Actor);
			actor->addForce(PxVec3(_vPos.x, _vPos.y * _fPower*100.f, _vPos.z), PxForceMode::eFORCE, true);
			
			PxTransform Transform = actor->getGlobalPose();
			*_vOutPos = _float3(Transform.p.x, Transform.p.y, Transform.p.z);
			/*_float4x4 matRotate;
			_float4Q Quat = _float4Q(Transform.q.x, Transform.q.y, Transform.q.z, Transform.q.w);
			D3DXMatrixRotationQuaternion(&matRotate, &Quat);
			*_vOutmatRotate = matRotate;*/
			break;
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::ActorSizeUpdate(const wstring & _Name, _float _fPower, const _float & _fDeltaTime)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* actor = static_cast<PxRigidStatic*>(Actor);
			PxShape* shape;
			actor->getShapes(&shape, 1);
			PxSphereGeometry Geometry;
			shape->getSphereGeometry(Geometry);
			Geometry.radius += _fPower * _fDeltaTime;

			shape->setGeometry(Geometry);
			/*
			PxTransform Transform = actor->getGlobalPose();
			Transform.p.x += _vPos.x * _fPower * _fDeltaTime;
			Transform.p.y += _vPos.y * _fPower * _fDeltaTime;
			Transform.p.z += _vPos.z * _fPower * _fDeltaTime;
			Transform.q *= PxQuat(_Quat.x, _Quat.y, _Quat.z, _Quat.w);
			actor->setGlobalPose(Transform);
			*_vOutPos = _float3(Transform.p.x, Transform.p.y, Transform.p.z);
			_float4x4 matRotate;
			_float4Q Quat = _float4Q(Transform.q.x, Transform.q.y, Transform.q.z, Transform.q.w);
			D3DXMatrixRotationQuaternion(&matRotate, &Quat);
			*_vOutmatRotate = matRotate;*/
			break;
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::Obstacle_SizeUpdate(const wstring & _Name, PxTransform& _rTransform, const _float3 & _vSize)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* actor = static_cast<PxRigidStatic*>(Actor);
			actor->setGlobalPose(_rTransform);

			PxShape* shape;
			actor->getShapes(&shape, 1);

			PxBoxGeometry Geometry;
			Geometry.halfExtents = PxVec3(_vSize.x, _vSize.y, _vSize.z);
			shape->setGeometry(Geometry);

			break;
		}
	}
	Safe_Delete(Actors);
}

_float3 CPhysX_Manager::Set_GlobalPos(const wstring & _Name, _float3 _vPos)
{
	PxSceneReadLock scopedLock(*m_pScene);

	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			PxTransform Transform;
			Transform.p = PxVec3(_vPos.x, _vPos.y, _vPos.z);
			Transform.q = PxQuat(0.f, 0.f, 0.707f, 0.707f);
			m_pControllerManager->getController(i)->setPosition(PxExtendedVec3(_vPos.x, _vPos.y, _vPos.z));
			PxVec3 vPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
			PxExtendedVec3 vFoot = m_pControllerManager->getController(i)->getFootPosition();

			return _float3(vPos.x, (_float)vFoot.y, vPos.z);
		}
	}

	return _float3();
}

void CPhysX_Manager::Set_ActorGlobalPos(const wstring & _Name, _float3* _vOutPos, PxTransform& _rTransform)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	_bool bNotFind = true;
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* actor = static_cast<PxRigidStatic*>(Actor);
			actor->setGlobalPose(_rTransform);
			PxTransform Transform = actor->getGlobalPose();
			*_vOutPos = _float3(Transform.p.x, Transform.p.y, Transform.p.z);
			bNotFind = false;
			break;
		}
	}
	if(bNotFind)
		*_vOutPos = _float3(FLT_MAX, FLT_MAX, FLT_MAX);

	Safe_Delete(Actors);
}

void CPhysX_Manager::ActorGravity(const wstring & _Name, _float3 * _vOutPos, _float3 _vLook, _float _fFirstY, _float _fPower, _float* _fJumpTime, const _float & _fDeltaTime)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	_bool bNotFind = true;
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* actor = static_cast<PxRigidStatic*>(Actor);

			PxTransform Transform = actor->getGlobalPose();
			Transform.p.x += _vLook.x * _fPower * _fDeltaTime;
			Transform.p.z += _vLook.z * _fPower * _fDeltaTime;

			
			//중력 처리 부분
			*_fJumpTime += _fDeltaTime;
			
			
			Transform.p.y = Jump_Function_To_Y(&_fFirstY, *_fJumpTime, 10.f);
			
			//Transform.p.y = _vLook.y * _fPower * _fDeltaTime;	// 중력 Y값 넣기

			actor->setGlobalPose(Transform);

			PxTransform OutTransform = actor->getGlobalPose();
			*_vOutPos = _float3(OutTransform.p.x, OutTransform.p.y, OutTransform.p.z);
			bNotFind = false;
			break;
		}
	}
	if (bNotFind)
		*_vOutPos = _float3(FLT_MAX, FLT_MAX, FLT_MAX);

	Safe_Delete(Actors);
}

_float3 CPhysX_Manager::Public_Gravity(const wstring& _Name, _bool * _bGravity, _bool * _bGraviStart, _float3* _vPos, _float* _fJumpTime, _float _fPower, const _float& _fDeltaTime)
{
	// 파워는 10.f 쓰시오
	// _bGravity = false
	// _bGraviStart = true
	// _vPos = {0.f,0.f,0.f}
	// _fJumpTime= 0.f

	PxSceneReadLock scopedLock(*m_pScene);

	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			PxVec3 vOldPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
			PxVec3 vDisp = PxVec3(0.f, 0.f, 0.f);
			if (*_bGravity)
			{
				if (*_bGraviStart)
				{
					*_vPos = { vOldPos.x, vOldPos.y, vOldPos.z };
					*_fJumpTime = 0.f;
					*_bGraviStart = false;
				}
				_float3 vPos = *_vPos;
				vDisp.y = (Jump_Function_To_Y(&vPos.y, *_fJumpTime * 2.5f, _fPower)) - vOldPos.y;
				*_fJumpTime += _fDeltaTime;
			}
			else
				vDisp.y -= 3.f;

			const PxControllerFilters filters(m_pFilterData, m_pFilterCallback, m_pCCTFilterCallback);
			PxU32 fFlag = m_pControllerManager->getController(i)->move(vDisp * _fPower * _fDeltaTime, 0.f, _fDeltaTime, filters, m_pObstacleContext);
			if (PxControllerCollisionFlag::eCOLLISION_DOWN == fFlag)
			{
				if (*_bGravity)
				{
					*_vPos = { 0.f,0.f,0.f };
					*_bGraviStart = true;
					*_bGravity = false;
				}
			}
			else
				if(*_bGravity)
				*_bGravity = true;

			PxVec3 vPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
			PxExtendedVec3 vFoot = m_pControllerManager->getController(i)->getFootPosition();
			if (_Name.find(L"Preto") != wstring::npos)
			{
				return _float3(vPos.x, (_float)vFoot.y - 1.f, vPos.z);
			}
			else if (_Name.find(L"Item") != wstring::npos)
			{
				return _float3(vPos.x, (_float)vFoot.y + 1.5f, vPos.z);
			}
			else
			{
				return _float3(vPos.x, (_float)vFoot.y, vPos.z);
			}
		}
	}
	return _float3();
}

_float3 CPhysX_Manager::Player_Gravity(const wstring & _Name, _bool * _bJump, _bool * _bJumpStart, _bool * _bDoubleJump, _bool* _bJumping, _bool * _bGravity, _bool * _bGravStart, _int* _iJump, _int* _iDash, _float3 * _vPos, _float _fPower, _float * _fJumpTime, _float * _fFallDamage, _float * _fAirTime, const _float & _fDeltaTime)
{
	PxSceneReadLock scopedLock(*m_pScene);

	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			PxVec3 vOldPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
			PxVec3 vDisp = PxVec3(0.f, 0.f, 0.f);
			if (*_bJump)
			{
				if (*_bJumpStart)
				{
					*_vPos = _float3(vOldPos.x, vOldPos.y, vOldPos.z);
					*_fJumpTime = 0.f;
					*_bJumpStart = false;
					*_bGravity = false;
					*_bGravStart = true;
					*_fAirTime = 0.f;
				}
				if (*_bDoubleJump && *_iJump <= 0)
				{
					*_vPos = _float3(vOldPos.x, vOldPos.y, vOldPos.z);
					*_fJumpTime = 0.f;
					*_bDoubleJump = false;
					*_bGravity = false;
					*_bGravStart = true;
					*_fAirTime = 0.f;
				}
				_float3 vPos = *_vPos;
				vDisp.y = (Jump_Function_To_Y(&vPos.y, (*_fJumpTime) * 2.f, _fPower)) - vOldPos.y;
				*_fJumpTime += _fDeltaTime;
				*_fAirTime += _fDeltaTime;
			}
			else if (*_bGravity)
			{
				if (*_bGravStart)
				{
					*_vPos = _float3(vOldPos.x, vOldPos.y, vOldPos.z);
					*_bGravStart = false;
					*_fJumpTime = 0.f;
					*_fAirTime = 0.f;
					*_iJump = 1;
				}
				_float3 vPos = *_vPos;
				vDisp.y = (Jump_Function_To_Y(&vPos.y, (*_fJumpTime) * 2.f, 0.f)) - vOldPos.y;
				*_fJumpTime += _fDeltaTime;
				*_fAirTime += _fDeltaTime;
			}
			else if(!*_bJump && !*_bGravity)
			{
				*_bJumpStart = true;
				*_bDoubleJump = true;
				//*_bGravity = true;
				vDisp.y -= 3.f;
				*_iJump = 2;
			}

			const PxControllerFilters filters(m_pFilterData, m_pFilterCallback, m_pCCTFilterCallback);
				m_fFlag = m_pControllerManager->getController(i)->move(vDisp * _fPower * _fDeltaTime, 0.f, _fDeltaTime, filters, m_pObstacleContext);
			if (PxControllerCollisionFlag::eCOLLISION_DOWN == m_fFlag)
			{
				if (*_bJump)
				{
					*_bDoubleJump = true;
					*_bJumpStart = true;
					*_bJumping = false;
					*_bJump = false;
					*_iJump = 2;
					*_fAirTime = 0.f;

					PxVec3 vNowPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
					_float3 vStartPos = *_vPos;
					_float vLength = vNowPos.y - vStartPos.y;


					if(vLength <= -50.f)
						*_fFallDamage = 9999.f;
					else if (vLength <= -45.f)
						*_fFallDamage = 80.f;
					else if (vLength <= -40.f)
						*_fFallDamage = 70.f;
					else if (vLength <= -35.f)
						*_fFallDamage = 60.f;
					else if (vLength <= -30.f)
						*_fFallDamage = 50.f;
					else if (vLength <= -25.f)
						*_fFallDamage = 40.f;
					else if (vLength <= -20.f)
						*_fFallDamage = 30.f;
					else if (vLength <= -15.f)
						*_fFallDamage = 20.f;
					
					//else if (vLength >= 30.f)
				}
				else if (*_bGravity)
				{
					*_bJumpStart = true;
					*_bGravStart = true;
					*_bJumping = false;
					*_bGravity = false;
					*_iJump = 2;
					*_fAirTime = 0.f;

					PxVec3 vNowPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
					_float3 vStartPos = *_vPos;
					_float vLength = vNowPos.y - vStartPos.y;

					if (vLength <= -50.f)
						*_fFallDamage = 9999.f;
					else if (vLength <= -45.f)
						*_fFallDamage = 80.f;
					else if (vLength <= -40.f)
						*_fFallDamage = 70.f;
					else if (vLength <= -35.f)
						*_fFallDamage = 60.f;
					else if (vLength <= -30.f)
						*_fFallDamage = 50.f;
					else if (vLength <= -25.f)
						*_fFallDamage = 40.f;
					else if (vLength <= -20.f)
						*_fFallDamage = 30.f;
					else if (vLength <= -15.f)
						*_fFallDamage = 20.f;
				
				}
			}
			else
			{
				if (*_bJump)
					*_bJump = true;
				else 
					*_bGravity = true;
			}

			PxVec3 vPos = m_pControllerManager->getController(i)->getActor()->getGlobalPose().p;
			PxExtendedVec3 vFoot = m_pControllerManager->getController(i)->getFootPosition();
			if (_Name.find(L"Preto") != wstring::npos)
				return _float3(vPos.x, (_float)vFoot.y, vPos.z);
			else
				return _float3(vPos.x, (_float)vFoot.y, vPos.z);
		}
	}
	return _float3();
}

void CPhysX_Manager::AllMonsterCollision(_bool _bCollision)
{
	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		MyPublicData* Data = (MyPublicData*)m_pControllerManager->getController(i)->getActor()->userData;
		Data->CollisionCheck = _bCollision;

		m_pControllerManager->getController(i)->getActor()->userData = (void*)Data;
	}
}

void * CPhysX_Manager::PhysXData_Update(const wstring& _Name)
{
	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			return m_pControllerManager->getController(i)->getActor()->userData;
		}
	}

	return nullptr;
}

void CPhysX_Manager::GetData_Trigger(const wstring & _Name, void** _Data)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			*_Data = Actor->userData;
			break;
		}
	}

	Safe_Delete(Actors);
}

void* CPhysX_Manager::GetData_Controller(const wstring & _Name)
{
	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			return (void*)m_pControllerManager->getController(i)->getActor()->userData;
		}
	}

	return nullptr;
}

void CPhysX_Manager::SetData_Controller(const wstring & _Name, void * _Data)
{
	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			m_pControllerManager->getController(i)->getActor()->userData = _Data;

			return;
		}
	}
}

_float4Q CPhysX_Manager::SetRotate_Controller(const wstring & _Name, _float4Q _Quat)
{
	PxSceneReadLock scopedLock(*m_pScene);

	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			PxShape* shape;
			m_pControllerManager->getController(i)->getActor()->getShapes(&shape, 1);
			PxTransform transform = shape->getLocalPose();
			transform.q *= PxQuat(_Quat.x, _Quat.y, _Quat.z, _Quat.w);
			shape->setLocalPose(transform);

			return _float4Q(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
		}
	}

	return _float4Q();
}

void CPhysX_Manager::Controller_ContactChange(const wstring & _Name, _float _fValue)
{
	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			m_pControllerManager->getController(i)->setContactOffset(_fValue);
			return;
		}
	}
}

void CPhysX_Manager::SetData_Trigger(const wstring & _Name, void * _Data)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			Actor->userData = _Data;
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::SetPos_Trigger(const wstring & _Name, PxTransform & _rTransform)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* actor = static_cast<PxRigidStatic*>(Actor);
			actor->setGlobalPose(_rTransform);
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::SetDynamic_Actor(const wstring & _Name)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidDynamic* actor = static_cast<PxRigidDynamic*>(Actor);
			actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			
			break;
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::Weapon_Update(const wstring & _Name, _float3 _vPos, _float4Q _Quat)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			PxRigidStatic* Rigid = static_cast<PxRigidStatic*>(Actor);
			Rigid->setGlobalPose(PxTransform(PxVec3(_vPos.x, _vPos.y, _vPos.z), PxQuat(_Quat.x, _Quat.y, _Quat.z, _Quat.w)));
			break;
		}
	}

	Safe_Delete(Actors);
}

MyRayCast* CPhysX_Manager::RayCast_Update(const wstring & _Name, _float3 _vPos, _float3 _vNormal, _float _Dist)
{
	

	PxSceneReadLock scopedLock(*m_pScene);

	const PxU32 BufferSize = 256;
	PxRaycastHit HitBuffer[BufferSize];
	PxRaycastBuffer Buf(HitBuffer, BufferSize);
	Buf.maxNbTouches = 10;

	m_pScene->raycast(PxVec3(_vPos.x, _vPos.y, _vPos.z), PxVec3(_vNormal.x, _vNormal.y, _vNormal.z), _Dist, Buf);

	MyPublicData* WeaponData = nullptr;
	PxShape* Shape = nullptr;


	for (PxU32 i = 0; i < Buf.nbTouches; ++i)
	{
		wstring wstrName = CharToWChar(Buf.touches[i].shape->getActor()->getName());
		if (!lstrcmpi(_Name.c_str(), CharToWChar(Buf.touches[i].shape->getActor()->getName()).c_str()))
		{
			Shape = Buf.touches[i].shape;
			WeaponData = (MyPublicData*)Buf.touches[i].actor->userData;
			continue;
		}

		if (Shape == nullptr)
			continue;

		if (MyFilterGroup::eObject == Buf.touches[i].shape->getSimulationFilterData().word0)
			continue;

		PxShapeFlags flag = Buf.touches[i].shape->getFlags();
		if (PxShapeFlag::eTRIGGER_SHAPE & flag)
			continue;

		if (MyFilterGroup::ePlayer == Buf.touches[i].shape->getSimulationFilterData().word0 && MyFilterGroup::ePlayerWeapon != Shape->getSimulationFilterData().word0)
		{
			MyPlayerData* PlayerData = (MyPlayerData*)Buf.touches[i].actor->userData;

			if (!PlayerData->Collision)
			{
				PlayerData->Collision = true;
				PlayerData->Damage = WeaponData->Damage;
				Buf.touches[i].actor->userData = PlayerData;
			}	

			m_RayCast->vPos = _float3(Buf.touches[i].position.x, Buf.touches[i].position.y, Buf.touches[i].position.z);
			return m_RayCast;
		}
	}

	return nullptr;
}

MyRayCast * CPhysX_Manager::RayCast_Update(const wstring & _Name, PxTransform & _rTransform, _float3 _vNormal, _float _Dist, _float _fRadius)
{
	PxSceneReadLock scopedLock(*m_pScene);

	const PxU32 BufferSize = 256;
	PxSweepHit HitBuffer[BufferSize];
	PxSweepBuffer Buf(HitBuffer, BufferSize);
	PxHitFlags HitFlag = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;

	PxCapsuleGeometry Capsule(1.f, 1.f);
	PxSphereGeometry Sphere(_fRadius);
	m_pScene->sweep(Sphere, _rTransform, PxVec3(_vNormal.x, _vNormal.y, _vNormal.z), _Dist, Buf);

	PxU32 hitNum = Buf.getNbAnyHits();
	if (hitNum == 0)
		return nullptr;

	MyPublicData* WeaponData = nullptr;
	PxU32 iFilter;
	PxActor* weaponActor = nullptr;
	for (PxU32 i = 0; i < hitNum; ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(HitBuffer[i].shape->getActor()->getName()).c_str()))
		{
			weaponActor = HitBuffer[i].actor;
			WeaponData = (MyPublicData*)HitBuffer[i].actor->userData;
			iFilter = HitBuffer[i].shape->getSimulationFilterData().word0;
			break;
		}
	}

	for (PxU32 i = 0; i < hitNum; ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(HitBuffer[i].shape->getActor()->getName()).c_str()))
			continue;

		if (iFilter == MyFilterGroup::ePlayerWeapon)	// 플레이어 무기 일 경우
		{
			if (MyFilterGroup::ePlayer == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			PxShapeFlags flag = HitBuffer[i].shape->getFlags();
			if (PxShapeFlag::eTRIGGER_SHAPE & flag)
				continue;

			if (MyFilterGroup::eMonsterWeapon == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			if (MyFilterGroup::eMonster == HitBuffer[i].shape->getSimulationFilterData().word0)
			{
				MyMonsterData* MonsterData = static_cast<MyMonsterData*>(HitBuffer[i].actor->userData);

				if (WeaponData->Collision && !MonsterData->Collision && !MonsterData->CollisionCheck)
				{
					MonsterData->Collision = true;
					MonsterData->CollisionCheck = true;
					MonsterData->Damage = WeaponData->Damage;
					HitBuffer[i].actor->userData = (void*)MonsterData;

					WeaponData->CollisionCheck = true;
					weaponActor->userData = (void*)WeaponData;	

					m_RayCast->vPos = _float3(HitBuffer[i].position.x, HitBuffer[i].position.y, HitBuffer[i].position.z);
					m_RayCast->vNormal = _float3(HitBuffer[i].normal.x, HitBuffer[i].normal.y, HitBuffer[i].normal.z);

					wstring wstrMonster = CharToWChar(HitBuffer[i].actor->getName());
					if (wstrMonster.find(L"Preto") != wstring::npos)
					{
						m_RayCast->TypeID = RayCastID::PretoBoss;
					}
					else if (wstrMonster.find(L"Malkhel") != wstring::npos)
					{
						m_RayCast->TypeID = RayCastID::MalkhelBoss;
					}
					else
						m_RayCast->TypeID = RayCastID::Monster;

					return m_RayCast;
				}

				return nullptr;
			}

			if (MyFilterGroup::eTerrain == HitBuffer[i].shape->getSimulationFilterData().word0 || MyFilterGroup::eObject == HitBuffer[i].shape->getSimulationFilterData().word0)
			{
				Buf.touches[i].distance;
				m_RayCast->vPos = _float3(HitBuffer[i].position.x, HitBuffer[i].position.y, HitBuffer[i].position.z);
				m_RayCast->vNormal = _float3(HitBuffer[i].normal.x, HitBuffer[i].normal.y, HitBuffer[i].normal.z);
				m_RayCast->TypeID = RayCastID::Terrain;
				return m_RayCast;
			}

			if (MyFilterGroup::eAttackObject == HitBuffer[i].shape->getSimulationFilterData().word0)
			{
				MyPublicData* ObjectData = (MyPublicData*)HitBuffer[i].actor->userData;

				if (!ObjectData->Collision)
				{
					ObjectData->Collision = true;
					ObjectData->Damage = 1;
					HitBuffer[i].actor->userData = ObjectData;
				}

				m_RayCast->vPos = _float3(HitBuffer[i].position.x, HitBuffer[i].position.y, HitBuffer[i].position.z);
				m_RayCast->vNormal = _float3(HitBuffer[i].normal.x, HitBuffer[i].normal.y, HitBuffer[i].normal.z);
				m_RayCast->TypeID = RayCastID::Terrain;
				return m_RayCast;
			}
		}

		if (iFilter == MyFilterGroup::eMonsterWeapon)	// 몬스터 무기 일 경우
		{
			if (MyFilterGroup::eMonster == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			PxShapeFlags flag = HitBuffer[i].shape->getFlags();
			if (PxShapeFlag::eTRIGGER_SHAPE & flag)
				continue;

			if (MyFilterGroup::ePlayerWeapon == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			if (MyFilterGroup::eMonsterWeapon == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			if (MyFilterGroup::eTerrain == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			if (MyFilterGroup::eObject == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			if (MyFilterGroup::eAttackObject == HitBuffer[i].shape->getSimulationFilterData().word0)
				continue;

			if (MyFilterGroup::ePlayer == HitBuffer[i].shape->getSimulationFilterData().word0)
			{
				MyPlayerData* PlayerData = static_cast<MyPlayerData*>(HitBuffer[i].actor->userData);

				if (WeaponData->Collision && !PlayerData->Collision && !WeaponData->CollisionCheck)
				{
					PlayerData->Collision = true;
					//PlayerData->CollisionCheck = true;
					PlayerData->Damage = WeaponData->Damage;
					HitBuffer[i].actor->userData = (void*)PlayerData;

					WeaponData->CollisionCheck = true;
					WeaponData->Collision = true;
					weaponActor->userData = (void*)WeaponData;

					m_RayCast->vPos = _float3(HitBuffer[i].position.x, HitBuffer[i].position.y, HitBuffer[i].position.z);
					m_RayCast->vNormal = _float3(HitBuffer[i].normal.x, HitBuffer[i].normal.y, HitBuffer[i].normal.z);

					return m_RayCast;
				}

				return nullptr;
			}			
		}
	}

	return nullptr;
}

MyRayCast * CPhysX_Manager::CameraRayCast_Update(const wstring & _Name, _float3 _vPos, _float3 _vNormal, _float _Dist)
{
	PxSceneReadLock scopedLock(*m_pScene);

	const PxU32 BufferSize = 256;
	PxRaycastHit HitBuffer[BufferSize];
	PxRaycastBuffer Buf(HitBuffer, BufferSize);
	Buf.maxNbTouches = 2;

	m_pScene->raycast(PxVec3(_vPos.x, _vPos.y, _vPos.z), PxVec3(_vNormal.x, _vNormal.y, _vNormal.z), _Dist, Buf);

	for (PxU32 i = 0; i < Buf.nbTouches; ++i)
	{
		if (MyFilterGroup::ePlayer == Buf.touches[i].shape->getSimulationFilterData().word0)
			continue;
		if (MyFilterGroup::eMonster == Buf.touches[i].shape->getSimulationFilterData().word0)
			continue;
		if (MyFilterGroup::ePlayerWeapon == Buf.touches[i].shape->getSimulationFilterData().word0)
			continue;
		if (MyFilterGroup::eMonsterWeapon == Buf.touches[i].shape->getSimulationFilterData().word0)
			continue;

		PxShapeFlags flag = Buf.touches[i].shape->getFlags();
		wstring wstrException = CharToWChar(Buf.touches[i].shape->getActor()->getName());
		if (PxShapeFlag::eTRIGGER_SHAPE & flag && wstrException.find(L"IronDoor") == wstring::npos)
			continue;

		if (MyFilterGroup::eObject == Buf.touches[i].shape->getSimulationFilterData().word0)
		{
			m_CameraRayCast->fDist = Buf.touches[i].distance;
			return m_CameraRayCast;
		}

		if (MyFilterGroup::eTerrain == Buf.touches[i].shape->getSimulationFilterData().word0)
		{
			m_CameraRayCast->fDist = Buf.touches[i].distance;
			return m_CameraRayCast;
		}
	}

	return nullptr;
}

void CPhysX_Manager::Delete_Controller(const wstring & _Name)
{
	for (PxU32 i = 0; i < m_pControllerManager->getNbControllers(); ++i)
	{
		if (!lstrcmpi(_Name.c_str(), CharToWChar(m_pControllerManager->getController(i)->getActor()->getName()).c_str()))
		{
			Safe_Delete(m_pControllerManager->getController(i)->getActor()->userData);
			m_pControllerManager->getController(i)->release();
			return;
		}
	}
}

void CPhysX_Manager::Delete_Trigger(const wstring & _Name)
{
	const PxU32 numPxActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);
	PxActor** Actors = new PxActor*[numPxActors];
	m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC, Actors, sizeof(PxActor) * numPxActors, 0);
	for (PxU32 i = 0; i < numPxActors; ++i)
	{
		PxActor* Actor = Actors[i];
		if (nullptr == Actor->getName())
			continue;

		if (!lstrcmpi(_Name.c_str(), CharToWChar(Actor->getName()).c_str()))
		{
			auto iter = m_vecPublicData.begin();
			for ( ; iter != m_vecPublicData.end(); )
			{
				if (!lstrcmpi((*iter)->szName, _Name.c_str()))
				{
					Safe_Delete(*iter);
					iter = m_vecPublicData.erase(iter);
				}
				else
					++iter;
			}

			m_pScene->removeActor(*Actor);
		}
	}

	Safe_Delete(Actors);
}

void CPhysX_Manager::Delete_Obstacle(const wstring & _Name)
{
	for (PxU32 i = 0; i < m_pObstacleContext->getNbObstacles(); ++i)
	{
		MyPublicData* Data = (MyPublicData*)m_pObstacleContext->getObstacle(i)->mUserData;
		if (!lstrcmpi(_Name.c_str(), Data->szName))
		{
			auto ifer_find = m_mapObstacleHandle.find(_Name);
			if (ifer_find != m_mapObstacleHandle.end())
			{
				m_pObstacleContext->removeObstacle(ifer_find->second);
				m_mapObstacleHandle.erase(_Name);
			}
				
			return;
		}
	}
}

void CPhysX_Manager::Free()
{
	Free_PhysX_Manager();
}

// 컨트롤러가 모양에 충돌 때 호출
void CPhysX_Manager::onShapeHit(const PxControllerShapeHit & hit)
{
	defaultCCTInteraction(hit);
}

// 컨트롤러가 다른 컨트롤러에 충돌 때 호출
void CPhysX_Manager::onControllerHit(const PxControllersHit & hit)
{
}

// 컨트롤러가 사용자 정의 장애물에 충돌 때 호출
void CPhysX_Manager::onObstacleHit(const PxControllerObstacleHit & hit)
{
}

PxControllerBehaviorFlags CPhysX_Manager::getBehaviorFlags(const PxShape & shape, const PxActor & actor)
{
	// eCCT_CAN_RIDE_ON_OBJECT 충돌시 물체 탐
	// eCCT_SLIDE 충돌시 미끄러짐
	//return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;

	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
}

PxControllerBehaviorFlags CPhysX_Manager::getBehaviorFlags(const PxController & controller)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPhysX_Manager::getBehaviorFlags(const PxObstacle & obstacle)
{
	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

void CPhysX_Manager::defaultCCTInteraction(const PxControllerShapeHit & hit)
{
	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
	if (actor)
	{
		if (actor->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
			return;

		if (0)
		{
			const PxVec3 p = actor->getGlobalPose().p + hit.dir * 10.0f;

			PxShape* shape;
			actor->getShapes(&shape, 1);
			PxRaycastHit newHit;
			PxU32 n = PxShapeExt::raycast(*shape, *shape->getActor(), p, -hit.dir, 20.0f, PxHitFlag::ePOSITION, 1, &newHit);
			if (n)
			{
				// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
				// useless stress on the solver. It would be possible to enable/disable vertical pushes on
				// particular objects, if the gameplay requires it.
				const PxVec3 upVector = hit.controller->getUpDirection();
				const PxF32 dp = hit.dir.dot(upVector);
				//		shdfnd::printFormatted("%f\n", fabsf(dp));
				if (fabsf(dp)<1e-3f)
					//		if(hit.dir.y==0.0f)
				{
					const PxTransform globalPose = actor->getGlobalPose();
					const PxVec3 localPos = globalPose.transformInv(newHit.position);
					PxRigidBodyExt::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
				}
			}
		}

		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
		// particular objects, if the gameplay requires it.
		const PxVec3 upVector = hit.controller->getUpDirection();
		const PxF32 dp = hit.dir.dot(upVector);
		//		shdfnd::printFormatted("%f\n", fabsf(dp));
		if (fabsf(dp)<1e-3f)
			//		if(hit.dir.y==0.0f)
		{
			const PxTransform globalPose = actor->getGlobalPose();
			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
			PxRigidBodyExt::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
		}
	}
}

void CPhysX_Manager::setupFiltering(PxRigidActor * actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = new PxShape*[numShapes];
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}

	Safe_Delete(shapes);
}

PxFilterFlags CPhysX_Manager::contactReportFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags & pairFlags, const void * constantBlock, PxU32 constantBlockSize)
{
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.v	
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eTRIGGER_DEFAULT;

	return PxFilterFlag::eDEFAULT;
}

PxQueryHitType::Enum CPhysX_Manager::preFilter(const PxFilterData & filterData, const PxShape * shape, const PxRigidActor * actor, PxHitFlags & queryFlags)
{
	return PxQueryHitType::Enum();
}

PxQueryHitType::Enum CPhysX_Manager::postFilter(const PxFilterData & filterData, const PxQueryHit & hit)
{
	return PxQueryHitType::Enum();
}

void CPhysX_Manager::onConstraintBreak(PxConstraintInfo * constraints, PxU32 count)
{
}

void CPhysX_Manager::onWake(PxActor ** actors, PxU32 count)
{
}

void CPhysX_Manager::onSleep(PxActor ** actors, PxU32 count)
{
}

void CPhysX_Manager::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{
	if (NULL == pairHeader.actors[0]->userData || NULL == pairHeader.actors[1]->userData)
		return;

	/*wstring wstrTriggerb = CharToWChar(pairHeader.actors[0]->getName());
	wstring wstrTriggera = CharToWChar(pairHeader.actors[1]->getName());

	wstring wstrController = CharToWChar(pairHeader.actors[0]->getName());
	wstring wstrWeapon = CharToWChar(pairHeader.actors[1]->getName());
	if (wstrWeapon.find(L"PlayerWeapon") != wstring::npos)
	{
		MyPublicData* WeaponData = (MyPublicData*)pairHeader.actors[1]->userData;

		MyMonsterData* MonsterData = (MyMonsterData*)pairHeader.actors[0]->userData;

		MonsterData->Collision = true;
		MonsterData->Damage = WeaponData->Damage;
		pairHeader.actors[0]->userData = MonsterData;
	}
	else
	{

	}*/
	
	
	
	int a = 0;
}

void CPhysX_Manager::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
	if (NULL == pairs->otherActor->userData)
		return;

	if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
	{
		if (pairs->otherShape->getSimulationFilterData().word0 & MyFilterGroup::eParry)
		{
			wstring wstrTrigger = CharToWChar(pairs->triggerActor->getName());
			if (wstrTrigger.find(L"_Weapon") != wstring::npos)
			{
				MyPublicData* Data = (MyPublicData*)pairs->otherActor->userData;
	
				if (Data->Attack)
				{
					Data->Collision = true;
					pairs->otherActor->userData = Data;
				}				
			}
		}

		if (pairs->otherShape->getSimulationFilterData().word0 & MyFilterGroup::ePlayer)
		{
			wstring wstrTrigger = CharToWChar(pairs->triggerActor->getName());
			if (wstrTrigger.find(L"WeaponStand") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::WeaponStand))
					Data->ObstacleType |= (PxU32)EObstacle::WeaponStand;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"SavePoint") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::SavePoint))
				{
					MyPlayerData* MyTriggerData = (MyPlayerData*)pairs->triggerActor->userData;
					Data->ObstacleType |= (PxU32)EObstacle::SavePoint;
					lstrcpy(Data->szActivteName, CharToWChar(pairs->triggerActor->getName()).c_str());
				}
				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Elevator") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Elevator))
				{
					Data->ObstacleType |= (PxU32)EObstacle::Elevator;
					lstrcpy(Data->szActivtedName, CharToWChar(pairs->triggerActor->getName()).c_str());
				}

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Lever") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Lever))
				{
					Data->ObstacleType |= (PxU32)EObstacle::Lever;
					lstrcpy(Data->szActivteName, CharToWChar(pairs->triggerActor->getName()).c_str());
				}

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Item") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Item))
				{
					MyItemData* MyTriggerData = (MyItemData*)pairs->triggerActor->userData;

					Data->ObstacleType |= (PxU32)EObstacle::Item;
					Data->ItemID = MyTriggerData->ItemID;
					lstrcpy(Data->szActivteName, CharToWChar(pairs->triggerActor->getName()).c_str());
				}

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"PortalGate") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::PortalGate))
				{
					Data->ObstacleType |= (PxU32)EObstacle::PortalGate;
					lstrcpy(Data->szActivteName, CharToWChar(pairs->triggerActor->getName()).c_str());
				}					

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"PortalIn") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::PortalGateIn))
					Data->ObstacleType |= (PxU32)EObstacle::PortalGateIn;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Choron") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Choron))
					Data->ObstacleType |= (PxU32)EObstacle::Choron;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Valnir") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Valnir))
					Data->ObstacleType |= (PxU32)EObstacle::Valnir;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Yaak") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Yaak))
					Data->ObstacleType |= (PxU32)EObstacle::Yaak;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"LevelStone") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::LevelStone))
					Data->ObstacleType |= (PxU32)EObstacle::LevelStone;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"IronDoor") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::IronDoor))
					Data->ObstacleType |= (PxU32)EObstacle::IronDoor;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Lateral_1") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Lateral1))
					Data->ObstacleType |= (PxU32)EObstacle::Lateral1;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Lateral_2") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (!(Data->ObstacleType & (PxU32)EObstacle::Lateral2))
					Data->ObstacleType |= (PxU32)EObstacle::Lateral2;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"FallingFlat") != wstring::npos)
			{
				MyPublicData* Data = (MyPublicData*)pairs->triggerActor->userData;

				if (!Data->Collision)
				{
					Data->Collision = true;
					pairs->triggerActor->userData = Data;
				}
				//else
				//{
				//	Data->Collision = false;
				//	pairs->triggerActor->userData = Data;
				//}
			}
			
			if (wstrTrigger.find(L"BossUnDropField") != wstring::npos)
			{
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;

				if (!(PlayerData->ObstacleType & (PxU32)EObstacle::BossStartCinematic))
				{
					PlayerData->ObstacleType |= (PxU32)EObstacle::BossStartCinematic;
					pairs->otherActor->userData = PlayerData;
				}
			}

			if (wstrTrigger.find(L"Preto") != wstring::npos)
			{
				MyPublicData* WeaponData = (MyPublicData*)pairs->triggerActor->userData;
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;

				if (WeaponData->Collision && !PlayerData->Collision)
				{
					PlayerData->Collision = true;
					PlayerData->Damage = WeaponData->Damage;
					pairs->otherActor->userData = PlayerData;
				}
			}

			if (wstrTrigger.find(L"Homonculus") != wstring::npos)
			{
				MyPublicData* WeaponData = (MyPublicData*)pairs->triggerActor->userData;
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;

				if (WeaponData->Collision && !PlayerData->Collision)
				{
					PlayerData->Collision = true;
					PlayerData->Damage = WeaponData->Damage;
					pairs->otherActor->userData = PlayerData;
				}
			}

			if (wstrTrigger.find(L"_FireBall") != wstring::npos)
			{
				MyPublicData* WeaponData = (MyPublicData*)pairs->triggerActor->userData;
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;

				if (WeaponData->Collision && !PlayerData->Collision)
				{
					PlayerData->Collision = true;
					PlayerData->Damage = WeaponData->Damage;
					pairs->otherActor->userData = PlayerData;
				}
			}

			if (wstrTrigger.find(L"_Spear") != wstring::npos)
			{
				MyPublicData* WeaponData = (MyPublicData*)pairs->triggerActor->userData;
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;

				if (WeaponData->Collision && !PlayerData->Collision)
				{
					PlayerData->Collision = true;
					PlayerData->Damage = WeaponData->Damage;
					pairs->otherActor->userData = PlayerData;
				}
			}

			if (wstrTrigger.find(L"_DownSkill") != wstring::npos)
			{
				MyPublicData* WeaponData = (MyPublicData*)pairs->triggerActor->userData;
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;

				if (WeaponData->Collision && !PlayerData->Collision)
				{
					PlayerData->Collision = true;
					PlayerData->Damage = WeaponData->Damage;
					pairs->otherActor->userData = PlayerData;
				}
			}

			if (wstrTrigger.find(L"Trap") != wstring::npos)
			{
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;
				if (!(PlayerData->ObstacleType & (PxU32)EObstacle::Trap))
				{
					PlayerData->ObstacleType |= (PxU32)EObstacle::Trap;
					PlayerData->Damage = 9999.f;
				}

				pairs->otherActor->userData = PlayerData;
			}

			if (wstrTrigger.find(L"ChurchDoor") != wstring::npos)
			{
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;
				if (!(PlayerData->ObstacleType & (PxU32)EObstacle::ChurchDoor))
				{
					PlayerData->ObstacleType |= (PxU32)EObstacle::ChurchDoor;
					pairs->otherActor->userData = PlayerData;
				}
			}

			if (wstrTrigger.find(L"Stage2BossStart") != wstring::npos)
			{
				MyPlayerData* PlayerData = (MyPlayerData*)pairs->otherActor->userData;
				if (!(PlayerData->ObstacleType & (PxU32)EObstacle::Stage2BossStart))
				{
					PlayerData->ObstacleType |= (PxU32)EObstacle::Stage2BossStart;
					pairs->otherActor->userData = PlayerData;
				}
			}
		}

		if (pairs->otherShape->getSimulationFilterData().word0 & MyFilterGroup::eMonster)
		{
			wstring wstrTrigger = CharToWChar(pairs->triggerActor->getName());
			if (wstrTrigger.find(L"BossUnDropField") != wstring::npos)
			{
				MyMonsterData* MonsterData = (MyMonsterData*)pairs->otherActor->userData;

				if (!MonsterData->DropField)
				{
					MonsterData->DropField = true;
					pairs->otherActor->userData = MonsterData;
				}
			}
		}
	}
	
	else if (pairs->status == PxPairFlag::eNOTIFY_TOUCH_LOST)
	{
		if(pairs->otherShape->getSimulationFilterData().word0 & MyFilterGroup::ePlayer)
		{
			wstring wstrTrigger = CharToWChar(pairs->triggerActor->getName());
			if (wstrTrigger.find(L"WeaponStand") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::WeaponStand)
					Data->ObstacleType ^= (PxU32)EObstacle::WeaponStand;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"SavePoint") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::SavePoint)
					Data->ObstacleType ^= (PxU32)EObstacle::SavePoint;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Elevator") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Elevator)
				{
					Data->ObstacleType ^= (PxU32)EObstacle::Elevator;
					lstrcpy(Data->szActivtedName, L"");
				}

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Lever") != wstring::npos)
			{

				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Lever)
				{
					Data->ObstacleType ^= (PxU32)EObstacle::Lever;
					lstrcpy(Data->szActivteName, L"");
				}

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Item") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Item)
				{
					Data->ObstacleType ^= (PxU32)EObstacle::Item;
					Data->ItemID = eItemID::Item_End;
					lstrcpy(Data->szActivteName, L"");
				}

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"PortalGate") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::PortalGate)
					Data->ObstacleType ^= (PxU32)EObstacle::PortalGate;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"PortalIn") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::PortalGateIn)
					Data->ObstacleType ^= (PxU32)EObstacle::PortalGateIn;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Choron") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Choron)
					Data->ObstacleType ^= (PxU32)EObstacle::Choron;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Valnir") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Valnir)
					Data->ObstacleType ^= (PxU32)EObstacle::Valnir;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Yaak") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Yaak)
					Data->ObstacleType ^= (PxU32)EObstacle::Yaak;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"LevelStone") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::LevelStone)
					Data->ObstacleType ^= (PxU32)EObstacle::LevelStone;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"IronDoor") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::IronDoor)
					Data->ObstacleType ^= (PxU32)EObstacle::IronDoor;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Lateral_1") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Lateral1)
					Data->ObstacleType ^= (PxU32)EObstacle::Lateral1;

				pairs->otherActor->userData = Data;
			}

			if (wstrTrigger.find(L"Lateral_2") != wstring::npos)
			{
				MyPlayerData* Data = (MyPlayerData*)pairs->otherActor->userData;
				if (Data->ObstacleType & (PxU32)EObstacle::Lateral2)
					Data->ObstacleType ^= (PxU32)EObstacle::Lateral2;

				pairs->otherActor->userData = Data;
			}
		}

		if (pairs->otherShape->getSimulationFilterData().word0 & MyFilterGroup::eMonster)
		{
			wstring wstrTrigger = CharToWChar(pairs->triggerActor->getName());
			if (wstrTrigger.find(L"BossUnDropField") != wstring::npos)
			{
				MyMonsterData* MonsterData = (MyMonsterData*)pairs->otherActor->userData;

				if (MonsterData->DropField)
				{
					MonsterData->DropField = false;
					pairs->otherActor->userData = MonsterData;
				}
			}
		}
	}
}

void CPhysX_Manager::onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count)
{
}

#endif // !_AFXDLL	