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
	virtual void Get_Damaged()	PURE;	//������ �Ͻʽÿ�.

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
	//��ü ������ �����մϴ�. 1���� ȣ��Ǵ� �Լ����� �����մϴ�.
	HRESULT PatternTotalControl(const _float& _fDeltaTime);

	/*���Ͽ� ���� �Լ��� ��� ȣ���ϵ��� �����մϴ�.*/
	HRESULT Do_TotalPattern(const _float& _fDeltaTime);

	//�������� �ܺ� Ŭ���� ���� ���� �޴� �Լ��� �����մϴ�.
	HRESULT OutClassCheck(const _float& _fDeltaTime);

protected:
	//�׻� ���� �Ǵ� �Լ�
	void Do_Always(const _float& _fDeltaTime);
	//���� Ȱ��ȭ �� �׻� �����Ǵ� �Լ�, �޸�� �������� ����.
	void Do_AfterActivation_Always(const _float& _fDeltaTime);
	//������ �����Լ�
	void Do_PhysxFunction(const _float& _fDeltaTime);
	//Ȱ��ȭ �� �ѹ� ����Ǵ� �Լ�
	void Do_Activation(const _float& _fDeltaTime);
	//Ȱ��ȭ ���� �� ȣ��� 
	virtual void Do_Activating(const _float& _fDeltaTime) PURE;
	//Ȱ�� ��� ������ �� ȣ��˴ϴ�.
	void Do_Ready(const _float& _fDeltaTime);
	//ü���� üũ�Ͽ� ������·� �����մϴ�.
	void CheckDead(const _float& _fDeltaTime);

protected:
	virtual void Do_Idle(const _float& _fDeltaTime)		PURE;	//������ �Ͻʽÿ�.
	virtual void Do_Attack(const _float& _fDeltaTime)	PURE;	//������ �Ͻʽÿ�.
	virtual void Do_Hit(const _float& _fDeltaTime)		PURE;	//������ �Ͻʽÿ�.
	virtual void Do_Walk(const _float& _fDeltaTime)		PURE;	//������ �Ͻʽÿ�.

	//Dead,Other,Sleep������ �� ȣ��˴ϴ�.
	virtual void Do_Other(const _float& _fDeltaTime)	PURE;	//������ �Ͻʽÿ�.

	void DeadParticle();

	HRESULT Item_Drop();

	void Back_To_Look(const _float & _fDeltaTime, _float3 _vPos);

private:
	//������Ʈ ���� �� ���� ȣ�� �Ǵ� �Լ��Դϴ�.
	void Update_Init(const _float& _fDeltaTime);
	//��ü�� �����մϴ�. �ʿ��� ��� ������ �� �θ� Ŭ���� �Լ��� ȣ�����ֽʽÿ�.
	virtual void Create_RigidBody();
	//��ü�� �����մϴ�.	
	virtual void Delete_RigidBody() PURE;	//������ �Ͻʽÿ�.

	//�̺�Ʈ �Ŵ����� ųī��Ʈ���� �߰��մϴ�.
	void Check_KillCount();

	//Sleep������ �� ��� ���θ� Ȯ���մϴ�.
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

	_bool						m_bTargetLook;/*Ÿ�� �� Ȱ��ȭ OnOff*/

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

	// ������
	_bool						m_bJump, m_bJumpStart, m_bDoubleJump, m_bJumping, m_bGravity, m_bGravityStart, m_bHitEffectOnce;
	_float						m_fJumpTime;
	_int						m_iJump, m_iDash;
	_float3						m_vJumpPos, m_vGravityPos;

	_bool m_bUpdatePhysx;

#pragma endregion

#pragma region STRUCT_DESC

	// ü�¹�
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
