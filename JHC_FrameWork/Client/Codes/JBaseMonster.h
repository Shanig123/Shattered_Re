#pragma once
#ifndef __JBASEMONSTER_H__
#define __JBASEMONSTER_H__


#include "GameObject.h"
#include "JBaseWeapon.h"
#include "UIBase.h"

USING(Engine)

enum class eMonsterParent
{
	Normal,
	End
};

class CJBaseMonster abstract : public CGameObject
{
protected:
	explicit CJBaseMonster(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJBaseMonster(const CJBaseMonster& _rOther);
	virtual ~CJBaseMonster() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg);
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime)PURE;
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime);
	virtual HRESULT Render_GameObject();

public:/*Setter*/
	void Set_PatrolReady() { m_bFriendPatrolReady = true; }
	
public:/*Getter*/
	inline	const eMonsterPattern& Get_MonsterPattern()const { return m_ePreTotalPattern; }
	inline const _float4x4& Get_ChildWeaponTransform()const { return m_matChildWeapon; }
	//_bool Get_UIRender() { return m_bNormalUIRender; };
	inline const _float4& Get_DissolveColor() { return m_vDissolveColor; }
	inline const _uint& Get_ShaderPass() { return m_iShaderPass; }
	inline const _float& Get_DissolveSpeed() { return m_fDissolveSpeed; }
	inline const _float& Get_DissolveTime() { return m_fDissolveTime; }
	inline const _bool& Get_Targeting() { return m_bTargeting; }
	
protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

protected:
	void MonsterData_Update();

protected:
	//��ü ������ �����մϴ�. 1���� ȣ��Ǵ� �Լ����� �����մϴ�.
	HRESULT PatternTotalControl(const _float& _fDeltaTime);

	//�������� �ܺ� Ŭ���� ���� ���� �޴� �Լ��� �����մϴ�.
	HRESULT OutClassCheck(const _float& _fDeltaTime);

	/*���Ͽ� ���� �Լ��� ��� ȣ���ϵ��� �����մϴ�.*/
	HRESULT Do_TotalPattern(const _float& _fDeltaTime);

protected:
	//�׻� ���� �Ǵ� �Լ�
	void Do_Always(const _float& _fDeltaTime);
	//���� Ȱ��ȭ �� �׻� �����Ǵ� �Լ�, �޸�� �������� ����.
	void Do_AfterActivation_Always(const _float& _fDeltaTime);
	//������ �����Լ�
	void Do_PhysxFunction(const _float& _fDeltaTime);
	//Ȱ��ȭ �� �ѹ� ����Ǵ� �Լ�
	virtual void Do_Activation(const _float& _fDeltaTime);
	//Ȱ��ȭ ���� �� ȣ��� 
	void Do_Activating(const _float& _fDeltaTime);
	//Ȱ�� ��� ������ �� ȣ��˴ϴ�.
	void Do_Ready(const _float& _fDeltaTime);
	//ü���� üũ�Ͽ� ������·� �����մϴ�.
	void CheckDead(const _float& _fDeltaTime);

protected:
	virtual void Do_Idle(const _float& _fDeltaTime)		PURE;	//������ �Ͻʽÿ�.
	virtual void Do_Attack(const _float& _fDeltaTime)	PURE;	//������ �Ͻʽÿ�.
	virtual void Do_Hit(const _float& _fDeltaTime)		PURE;	//������ �Ͻʽÿ�.
	virtual void Do_Walk(const _float& _fDeltaTime)		PURE;	//������ �Ͻʽÿ�.
	virtual void Do_Other(const _float& _fDeltaTime)	PURE;	//������ �Ͻʽÿ�.
	virtual void Get_Damaged()	PURE;	//������ �Ͻʽÿ�.

protected:
	void TargetCheck();
	void Target_Look(const _float& _fDeltaTime);
	void CoolTimer(const _float& _fDeltaTime);
	
	void Set_Hit(_float _fDamage);
	//void DissolvePlay(DissolveID _eType);
	//void MonsterSpawn();
	//_bool MonsterSpawnDelay(const _float& _fDeltaTime);
	void Drop_Dead();
	HRESULT Item_Drop();
	void DeadParticle();
	// ��Ʈ�� ����� �����ϴ� �Լ�
	_bool Patrol_Look(const _float & _fDeltaTime);
	// ��Ʈ�� ��Ʈ�� �߰��ϴ� �Լ�
	HRESULT Add_Friend(_uint iFriendIndex, const wstring& _PrototypeName);

private:
	//������Ʈ ���� �� ���� ȣ�� �Ǵ� �Լ��Դϴ�.
	void Update_Init(const _float& _fDeltaTime);
	//��ü�� �����մϴ�.
	void Create_RigidBody();
	//��ü�� �����մϴ�.
	void Delete_RigidBody();

	//�̺�Ʈ �Ŵ����� ųī��Ʈ���� �߰��մϴ�.
	void Check_KillCount();
	//Sleep������ �� Ÿ���� üũ�Ͽ� ��Ȱ ���θ� ���մϴ�.
	void Check_SleepTime_Const();

public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() override;

protected:
#pragma region CLASS_POINTER_VALUE

	class CDynamicMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CPlayer*				m_pPlayer;
	class CTransform*			m_pTargetTransform;
	class CShader*				m_pShader;
	class CJBaseWeapon*			m_pWeapon;
#pragma endregion

#pragma region ENUM_STATE_VALUE

	eMonsterPattern				m_eCurTotalPattern;
	eMonsterPattern				m_ePreTotalPattern;
	eMonsterParent				m_eParentCategory;

#pragma endregion

	

#pragma region BOOLEAN_VALUE

	_bool						m_bTargetLook;/*Ÿ�� �� Ȱ��ȭ OnOff*/
	_bool						m_bTargetYLook;/*Ÿ�� ��Y ��� Ȱ��ȭ OnOff*/

	_bool						m_bApplyGravity;	//�߷� OnOff;
	_bool						m_bCheckHit;		//���ݹ޾Ҵ� �� �ƴ��� üũ��.

	_bool						m_bAttackCoolTime;
	_bool						m_bDissolve, m_bReverseDissolve;
	_bool						m_bTargeting;
	_bool						m_bCheck_FirstDeadFlag;

	_bool						m_bLeftLeg;
	_bool						m_bRightLeg;
	_bool						m_bAttackSound;

	_bool						m_bCamMode;

	const _bool					m_bDropItem;
#pragma endregion

#pragma region FLOATING_VALUE
	_float						m_fDeltaTime, m_fSkillMoveSpeed, m_fAttackCoolTime, m_fAttackCoolTimeEnd, m_fTargetDist, m_fWalkMoveTime, m_fDeathTime, m_fDelayTime;
	_float						m_fHitDamage, m_fAddAniPlayTime;
	_float						m_fDissolveTime, m_fDissolveSpeed;
	_float						m_fFindDistance;
	_float						m_iItemCreateYAxisPosFix;
	_float						m_fActivateTimer;//���� �� Ȱ��ȭ �Ǵ� �ð��� �����մϴ�.
	_float						m_fAdd_ActivateTimer;

	_float						m_fDamageMultiple;
	_float						m_fFarRange;
	_float						m_fSleepCountTime;	//Sleep���� ���� �ð��� ������ Ready���°� �˴ϴ�. ���ð��� ī��Ʈ�մϴ�.
	const _float						m_fSleepTime_Const;	//������ �ð��� �����մϴ�. Sleep->Ready ������ ������ ��� ��Ȱ���� �ʽ��ϴ�.
#pragma endregion

#pragma region FLOATING_VECTOR AND MATRIX

	_float4						m_vDissolveColor;
	_float3						m_vRight, m_vLook;

	const _float3						m_vInitilizePos;
	
	_float4x4					m_matChildWeapon;
#pragma endregion

#pragma region PHYSX_VALUE

	_bool						m_bGravity, m_bGraviStart;
	_float						m_fJumpTime;
	_float3						m_vGravityPos;
	MyMonsterData*				m_pMonsterData;

#pragma endregion

#pragma region INTEGER_VALUE

	_uint						m_iShaderPass, m_iDiffuseIndex;
	_uint						m_iGold;
	_uint						m_iDropGoldMin, m_iDropGoldMax;
	_uint						m_iSoundIndex;
#pragma endregion
	// ��Ʈ���� ���� ����
#pragma region PATROL_VALUE
	vector<_float3*>			m_vecPatrolPos;
	vector<class CJBaseMonster*>	m_vecFriendMonster;
	_float						m_fPatrolCoolTime;
	_uint						m_iCapacitySize;
	_uint						m_iNowPatrol;
	_bool						m_bPatrolReady;
	_bool						m_bFriendPatrolReady;
#pragma endregion
	CSound_Manager::ESoundID	m_eSound;

private:
	_bool						m_bRenderOnce;
};

#endif // !__JBaseMonster_H__
