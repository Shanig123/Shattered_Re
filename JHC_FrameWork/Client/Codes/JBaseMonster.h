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
	//전체 패턴을 관리합니다. 1번씩 호출되는 함수들을 관리합니다.
	HRESULT PatternTotalControl(const _float& _fDeltaTime);

	//지속적인 외부 클래스 값을 참조 받는 함수를 관리합니다.
	HRESULT OutClassCheck(const _float& _fDeltaTime);

	/*패턴에 따라 함수를 계속 호출하도록 관리합니다.*/
	HRESULT Do_TotalPattern(const _float& _fDeltaTime);

protected:
	//항상 구동 되는 함수
	void Do_Always(const _float& _fDeltaTime);
	//몬스터 활성화 후 항상 구동되는 함수, 휴면시 구동되지 않음.
	void Do_AfterActivation_Always(const _float& _fDeltaTime);
	//피직스 구동함수
	void Do_PhysxFunction(const _float& _fDeltaTime);
	//활성화 시 한번 실행되는 함수
	virtual void Do_Activation(const _float& _fDeltaTime);
	//활성화 중일 때 호출됨 
	void Do_Activating(const _float& _fDeltaTime);
	//활성 대기 상태일 때 호출됩니다.
	void Do_Ready(const _float& _fDeltaTime);
	//체력을 체크하여 데드상태로 변경합니다.
	void CheckDead(const _float& _fDeltaTime);

protected:
	virtual void Do_Idle(const _float& _fDeltaTime)		PURE;	//재정의 하십시오.
	virtual void Do_Attack(const _float& _fDeltaTime)	PURE;	//재정의 하십시오.
	virtual void Do_Hit(const _float& _fDeltaTime)		PURE;	//재정의 하십시오.
	virtual void Do_Walk(const _float& _fDeltaTime)		PURE;	//재정의 하십시오.
	virtual void Do_Other(const _float& _fDeltaTime)	PURE;	//재정의 하십시오.
	virtual void Get_Damaged()	PURE;	//재정의 하십시오.

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
	// 패트롤 룩방향 조절하는 함수
	_bool Patrol_Look(const _float & _fDeltaTime);
	// 패트롤 파트너 추가하는 함수
	HRESULT Add_Friend(_uint iFriendIndex, const wstring& _PrototypeName);

private:
	//업데이트 이후 한 번만 호출 되는 함수입니다.
	void Update_Init(const _float& _fDeltaTime);
	//강체를 생성합니다.
	void Create_RigidBody();
	//강체를 삭제합니다.
	void Delete_RigidBody();

	//이벤트 매니저에 킬카운트값을 추가합니다.
	void Check_KillCount();
	//Sleep상태일 시 타임을 체크하여 부활 여부를 정합니다.
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

	_bool						m_bTargetLook;/*타겟 룩 활성화 OnOff*/
	_bool						m_bTargetYLook;/*타겟 룩Y 계산 활성화 OnOff*/

	_bool						m_bApplyGravity;	//중력 OnOff;
	_bool						m_bCheckHit;		//공격받았는 지 아닌지 체크함.

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
	_float						m_fActivateTimer;//등장 시 활성화 되는 시간을 지정합니다.
	_float						m_fAdd_ActivateTimer;

	_float						m_fDamageMultiple;
	_float						m_fFarRange;
	_float						m_fSleepCountTime;	//Sleep에서 일정 시간이 지나면 Ready상태가 됩니다. 대기시간을 카운트합니다.
	const _float						m_fSleepTime_Const;	//리스폰 시간을 설정합니다. Sleep->Ready 음수로 지정될 경우 부활하지 않습니다.
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
	// 패트롤을 위한 변수
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
