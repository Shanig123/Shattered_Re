#pragma once
#ifndef _AFXDLL
#ifndef __PHYSX_MANAGER_H__
#define __PHYSX_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

using namespace physx;

struct MyPublicData
{
	MyPublicData()
	{
		Collision = false;
		Attack = false;
		Damage = 0.f;
	}

	TCHAR		szName[MAX_PATH];
	_bool		Collision;
	_bool		Attack;
	_float		Damage;
};

struct MyPlayerData : public MyPublicData
{
	MyPlayerData()
	{
		Collision = false;
		ObstacleType = 0;
		ItemID = eItemID::Item_End;
		Damage = 0.f;
	}
	TCHAR		szActivteName[MAX_PATH];		// 플레이어가 상호 작용 하는것(레버, 세이브 포인트)
	TCHAR		szActivtedName[MAX_PATH];		// 플레이어가 상호 작용 당하는것(엘리베이터)
	PxU32		ObstacleType;
	eItemID		ItemID;
};

struct MyMonsterData : public MyPublicData
{
	MyMonsterData()
	{
		Collision = false;
		DropField = false;
		Damage = 0.f;
	}

	_bool		DropField;
};

struct MyItemData : public MyPublicData
{
	MyItemData()
	{
		ItemID = eItemID::Item_End;
	}
	eItemID		ItemID;
};

struct MyRayCast
{
	MyRayCast()
	{
		TypeID = RayCastID::End;
	}

	_float3		vPos;
	_float3		vNormal;
	_float		fDist;
	RayCastID	TypeID;
	
};

struct MyFilterGroup
{
	enum Enum
	{
		ePlayer = (1 << 0),				//	1
		eMonster = (1 << 1),			//	2
		eObject = (1 << 2),				//	4
		ePlayerWeapon = (1 << 3),		//	8
		eMonsterWeapon = (1 << 4),		//	16
		eTerrain = (1 << 5),			//	32
		eAttackObject = (1 << 6),		//	64
	};
};

class CPhysX_Manager final : public CBase, public PxUserControllerHitReport, public PxControllerBehaviorCallback, public PxQueryFilterCallback, public PxSimulationEventCallback
{
	DECLARE_SINGLETON(CPhysX_Manager)

public:
	explicit CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

public:
	HRESULT Ready_PhysX_Manager();
	HRESULT CreateScene_PhysX_Manager();
	_bool Render_PhysX_Manager(const _float& _fDeltaTime);
	void Free_PhysX_Manager();

public:
	void TriangleMeshCooking(const wstring& _Name, _float3 _vScale, PxTriangleMeshDesc& _rMeshCooking, PxTransform& _rTransform);
	void Add_Controller(const wstring& _Name, _float3 _vPos, EInteractionType _eType, PxGeometry& _Geometry);
	void Add_Interaction(const wstring& _Name, _float3 _vPos, EInteractionType _eRigidType, PxGeometry& _RigidGeometry, EInteractionType _eTriggerType, PxGeometry& _TriggerGeometry);
	void Add_StaticActor(const wstring& _Name, PxTransform& _rTransform, EInteractionType _eRigidType, PxGeometry& _RigidGeometry);
	void Add_DynamicActor(const wstring& _Name, PxTransform& _rTransform, EInteractionType _eRigidType, PxGeometry& _RigidGeometry);
	void Add_Weapon(const wstring& _ActorName, _float3 _vPos, _float _fDamage, PxGeometry& _RigidGeometry, _float _fFocusY, _bool _bVertical = true);
	void Add_Trigger(const wstring& _Name, PxTransform& _rTransform, EInteractionType _eType, PxGeometry& _Geometry);
	void Add_Trigger(const wstring& _Name, PxTransform& _rTransform, _float _fDamage, EInteractionType _eType, PxGeometry& _Geometry);

public:
	_float3 Move(const wstring& _Name, _float3 _vPos, _float _fPower, const _float& _fDeltaTime);
	void ActorMove(const wstring& _Name, _float3* _vOutPos, _float3 _vPos, _float _fPower, const _float& _fDeltaTime);
	void ActorMove(const wstring& _Name, _float3* _vOutPos, _float4x4* _vOutmatRotate, _float3 _vPos, _float4Q _Quat, _float _fPower, const _float& _fDeltaTime);
	void ActorSizeUpdate(const wstring& _Name, _float _fPower, const _float& _fDeltaTime);
	_float3 Set_GlobalPos(const wstring& _Name, _float3 _vPos);
	void Set_ActorGlobalPos(const wstring& _Name, _float3* _vOutPos, PxTransform& _rTransform);
	_float3 Public_Gravity(const wstring& _Name, _bool * _bGravity, _bool * _bGraviStart, _float3* _vPos, _float* _fJumpTime, _float _fPower, const _float& _fDeltaTime);
	_float3 Player_Gravity(const wstring & _Name, _bool * _bJump, _bool * _bJumpStart, _bool * _bDoubleJump, _bool* _bJumping, _bool * _bGravity, _bool * _bGravStart, _int* _iJump, _int* _iDash, _float3 * _vPos, _float _fPower, _float * _fJumpTime, _float * _fFallDamage, _float * _fAirTime, const _float & _fDeltaTime);
	
	void* PhysXData_Update(const wstring& _Name);
	void GetData_Trigger(const wstring& _Name, void** _Data);
	void* GetData_Controller(const wstring& _Name);
	void SetData_Controller(const wstring& _Name, void* _Data);
	_float4Q SetRotate_Controller(const wstring& _Name, _float4Q _Quat);
	void Controller_ContactChange(const wstring& _Name, _float _fValue);
	void SetData_Trigger(const wstring& _Name, void* _Data);
	void SetPos_Trigger(const wstring& _Name, PxTransform& _rTransform);
	void SetDynamic_Actor(const wstring& _Name);
	void Weapon_Update(const wstring& _Name, _float3 _vPos, _float4Q _Quat);
	MyRayCast* RayCast_Update(const wstring& _Name, _float3 _vPos, _float3 _vNormal, _float _Dist);
	MyRayCast* CameraRayCast_Update(const wstring& _Name, _float3 _vPos, _float3 _vNormal, _float _Dist);

	void Delete_Controller(const wstring& _Name);
	void Delete_Trigger(const wstring& _Name);

public:
	virtual void Free() override;

private:
	PxPvd*									m_pPvD;
	PxPvdTransport*							m_pTransport;
	PxScene*								m_pScene;
	PxFoundation*							m_pFoundation;
	PxPhysics*								m_pPhysics;
	PxCooking*								m_pCooking;
	PxDefaultCpuDispatcher*					m_pDispatcher;
	//PxCudaContextManager*					m_pCudaContextManager;
	PxControllerManager*					m_pControllerManager;
	PxCapsuleObstacle*						m_pObstacleCapsule;

	PxDefaultAllocator						m_Allocator;
	PxDefaultErrorCallback					m_ErrorCallback;

	const PxFilterData*						m_pFilterData;		// User-defined filter data for 'move' function
	PxQueryFilterCallback*					m_pFilterCallback;	// User-defined filter data for 'move' function
	PxControllerFilterCallback*				m_pCCTFilterCallback;	// User-defined filter data for 'move' function
	PxObstacleContext*						m_pObstacleContext;	// User-defined additional obstacles

	_float									m_fAccumulator;
	_float									m_fStepSize;
	PxU32									m_fFlag;
	vector<const char*>						m_vecActroName;
	map<wstring, void*>						m_mapTriiger;



	vector<ObstacleHandle*>					m_vecObstacleHandle;
	vector<MyPublicData*>					m_vecPublicData;

	// 상호 작용
	_bool									m_bPlayerCollision;
	_bool									m_bObstacle;
	_uint									m_itest;

	// RayCast
	MyRayCast*								m_RayCast;
	MyRayCast*								m_CameraRayCast;

	// PxUserControllerHitReport을(를) 통해 상속됨
	virtual void onShapeHit(const PxControllerShapeHit & hit) override;
	virtual void onControllerHit(const PxControllersHit & hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit & hit) override;

	// PxControllerBehaviorCallback을(를) 통해 상속됨
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape & shape, const PxActor & actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController & controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle & obstacle) override;

	void defaultCCTInteraction(const PxControllerShapeHit& hit);

	void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
	static PxFilterFlags contactReportFilterShader
	(
		PxFilterObjectAttributes attributes0,
		PxFilterData filterData0,
		PxFilterObjectAttributes attributes1,
		PxFilterData filterData1,
		PxPairFlags& pairFlags,
		const void* constantBlock,
		PxU32 constantBlockSize
	);

	//void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
	//{
	//	PxFilterData filterData;
	//	filterData.word0 = filterGroup;  //  word0 = own ID
	//	filterData.word1 = filterMask;   //  word1 = ID mask to filter pairs that trigger a contact callback;
	//	const PxU32 numShapes = actor->getNbShapes();
	//	PxShape** shapes = new PxShape*[numShapes];
	//	actor->getShapes(shapes, numShapes);
	//	for (PxU32 i = 0; i <numShapes; i++)
	//	{         
	//		PxShape* shape = shapes[i];
	//		shape->setSimulationFilterData(filterData);
	//	}

	//	delete[] shapes;
	//}

	// PxQueryFilterCallback을(를) 통해 상속됨
	virtual PxQueryHitType::Enum preFilter(const PxFilterData & filterData, const PxShape * shape, const PxRigidActor * actor, PxHitFlags & queryFlags) override;
	virtual PxQueryHitType::Enum postFilter(const PxFilterData & filterData, const PxQueryHit & hit) override;

	// PxSimulationEventCallback을(를) 통해 상속됨
	virtual void onConstraintBreak(PxConstraintInfo * constraints, PxU32 count) override;
	virtual void onWake(PxActor ** actors, PxU32 count) override;
	virtual void onSleep(PxActor ** actors, PxU32 count) override;
	virtual void onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair * pairs, PxU32 count) override;
	virtual void onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count) override;
};

END

#endif // !__PHYSX_MANAGER_H__
#endif // !_AFXDLL	