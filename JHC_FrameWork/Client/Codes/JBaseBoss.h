#pragma once
#ifndef __JBASEBOSS_H__
#define __JBASEBOSS_H__

#include "GameObject.h"
#include "JBaseWeapon.h"
#include "UIBase.h"
#include "UI_BossMonster.h"

USING(Engine)

class CJBaseBoss abstract : public CGameObject
{
protected:
	explicit CJBaseBoss(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJBaseBoss(const CJBaseBoss& _rOther);
	virtual ~CJBaseBoss() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg);
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime);
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime);
	virtual HRESULT Render_GameObject();
	virtual void Get_Damaged()	PURE;	//재정의 하십시오.

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

public:/*Setter*/
	//void Set_Cinematic() { m_bCinematicStart = true; }

public:/*Getter*/
	inline	const eBossMonsterPattern& Get_MonsterPattern()const { return m_ePreTotalPattern; }
	inline const _float4x4& Get_ChildWeaponTransform()const { return m_matChildWeapon; }
	inline const _float4x4& Get_SecondChildWeaponTransform()const { return m_matSecondChildWeapon; }
	//_bool Get_UIRender() { return m_bNormalUIRender; };
	inline const _float4& Get_DissolveColor() { return m_vDissolveColor; }
	inline const _uint& Get_ShaderPass() { return m_iShaderPass; }
	inline const _float& Get_DissolveSpeed() { return m_fDissolveSpeed; }
	inline const _float& Get_DissolveTime() { return m_fDissolveTime; }

protected:
	//전체 패턴을 관리합니다. 1번씩 호출되는 함수들을 관리합니다.
	HRESULT PatternTotalControl(const _float& _fDeltaTime);

	/*패턴에 따라 함수를 계속 호출하도록 관리합니다.*/
	HRESULT Do_TotalPattern(const _float& _fDeltaTime);

	//지속적인 외부 클래스 값을 참조 받는 함수를 관리합니다.
	HRESULT OutClassCheck(const _float& _fDeltaTime);

protected:
	//항상 구동 되는 함수
	void Do_Always(const _float& _fDeltaTime);
	//몬스터 활성화 후 항상 구동되는 함수, 휴면시 구동되지 않음.
	void Do_AfterActivation_Always(const _float& _fDeltaTime);
	//피직스 구동함수
	void Do_PhysxFunction(const _float& _fDeltaTime);
	//활성화 시 한번 실행되는 함수
	void Do_Activation(const _float& _fDeltaTime);
	//활성화 중일 때 호출됨 
	virtual void Do_Activating(const _float& _fDeltaTime) PURE;
	//활성 대기 상태일 때 호출됩니다.
	void Do_Ready(const _float& _fDeltaTime);
	//체력을 체크하여 데드상태로 변경합니다.
	void CheckDead(const _float& _fDeltaTime);

protected:
	virtual void Do_Idle(const _float& _fDeltaTime)		PURE;	//재정의 하십시오.
	virtual void Do_Attack(const _float& _fDeltaTime)	PURE;	//재정의 하십시오.
	virtual void Do_Hit(const _float& _fDeltaTime)		PURE;	//재정의 하십시오.
	virtual void Do_Walk(const _float& _fDeltaTime)		PURE;	//재정의 하십시오.

	//Dead,Other,Sleep상태일 시 호출됩니다.
	virtual void Do_Other(const _float& _fDeltaTime)	PURE;	//재정의 하십시오.

	void DeadParticle();

	HRESULT Item_Drop();

	void Back_To_Look(const _float & _fDeltaTime, _float3 _vPos);

private:
	//업데이트 이후 한 번만 호출 되는 함수입니다.
	void Update_Init(const _float& _fDeltaTime);
	//강체를 생성합니다. 필요한 경우 재정의 후 부모 클래스 함수를 호출해주십시오.
	virtual void Create_RigidBody();
	//강체를 삭제합니다.	
	virtual void Delete_RigidBody() PURE;	//재정의 하십시오.

	//이벤트 매니저에 킬카운트값을 추가합니다.
	void Check_KillCount();

	//Sleep상태일 때 사망 여부를 확인합니다.
	void IsSleepDead();


private:
	void TargetCheck();
	void Target_Look(const _float& _fDeltaTime);
	void CoolTimer(const _float& _fDeltaTime);
	void MonsterData_Update();
	void Set_Hit(_float _fDamage);
#pragma region Member_Value

protected:
#pragma region CLASS_POINTER_VALUE

	class CDynamicMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CTransform*			m_pTargetTransform;
	class CShader*				m_pShader;
	class CJBaseWeapon*			m_pWeapon;
	class CUI_BossMonster*		m_pBossUI;

#pragma endregion

#pragma region ENUM_STATE_VALUE
	eBossMonsterPattern				m_eCurTotalPattern;
	eBossMonsterPattern				m_ePreTotalPattern;

#pragma endregion

#pragma region BOOLEAN_VALUE

	_bool						m_bTargetLook;/*타겟 룩 활성화 OnOff*/

	_bool						m_bAttackCoolTime, m_bApplyGravity, m_bHit;
	_bool						m_bAppearEnd, m_bDropField;

	_bool						m_bAttackSound;
	_bool						m_bLeftLeg;
	_bool						m_bRightLeg;
#pragma endregion

#pragma region FLOATING_VALUE

	_float						m_fFarRange;
	_float						m_iItemCreateYAxisPosFix;
	_float						m_fDeltaTime, m_fSkillMoveSpeed, m_fAttackCoolTime, m_fTargetDist, m_fWalkMoveTime;
	_float						m_fHitDamage, m_fAddAniPlayTime;
	_float						m_fDissolveTime, m_fDissolveSpeed;

#pragma endregion

#pragma region FLOATING_VECTOR AND MATRIX
	const _float3				m_vInitilizePos;
	_float3						m_vRight, m_vLook;

	_float4						m_vDissolveColor;
	_float4x4					m_matChildWeapon;
	_float4x4					m_matSecondChildWeapon;

#pragma endregion

#pragma region INTGER_VALUE

	_uint						m_iShaderPass, m_iDiffuseIndex;
	_uint						m_iDelayMovePattern;
	_uint						m_iGold;
	_uint						m_iDissolveTexture;
#pragma endregion

#pragma region PHYSX_VALUE

	// 피직스
	_bool						m_bJump, m_bJumpStart, m_bDoubleJump, m_bJumping, m_bGravity, m_bGravityStart, m_bHitEffectOnce;
	_float						m_fJumpTime;
	_int						m_iJump, m_iDash;
	_float3						m_vJumpPos, m_vGravityPos;

	_bool m_bUpdatePhysx;

#pragma endregion

#pragma region STRUCT_DESC

	// 체력바
	CUIBase::UI_DESC			m_tBackGroundDesc;
	CUIBase::UI_DESC			m_tHpGageDesc;

#pragma endregion

#pragma region PRIVATE_VALUE

private:
	_bool						m_bRenderOnce;
	_bool						m_bCheck_FirstDeadFlag;

#pragma endregion

#pragma endregion

public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free();


};

#endif // !__JBASEBOSS_H__
