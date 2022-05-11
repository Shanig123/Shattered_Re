#pragma once

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "GameObject.h"

USING(Engine)
class CPlayer final : public CGameObject
{
private:
	explicit CPlayer(_DEVICE _pDevice);
	explicit CPlayer(const CPlayer& _rOther);
	virtual ~CPlayer() = default;

private:
	enum STATE
	{
		IDLE_PEACE,
		IDLE_ATTACK,
		WALK,
		RUN,
		SPRINT,
		ROLL,
		JUMP_START,
		JUMP_AIR,
		JUMP_END,
		SLEEPING,
		WAKE_UP,
		HEAL,
		HEAL_WALK,
		USE,
		USE_WALK,
		TWALK_FRONT,
		TWALK_BACK,
		TWALK_LEFT,
		TWALK_RIGHT,
		TWALK_HEAL_IDLE,
		TWALK_HEAL_FRONT,
		TWALK_HEAL_BACK,
		TWALK_HEAL_LEFT,
		TWALK_HEAL_RIGHT,
		TWALK_USE_IDLE,
		TWALK_USE_FRONT,
		TWALK_USE_BACK,
		TWALK_USE_LEFT,
		TWALK_USE_RIGHT,
		DASH_AIR,
		DASH_FRONT,
		DASH_BACK,
		DASH_LEFT,
		DASH_RIGHT,
		INTERACTION,
		NORMAL_ATTACK1,
		NORMAL_ATTACK2,
		NORMAL_ATTACK3,
		NORMAL_HEAVY_ATTACK1,
		NORMAL_HEAVY_ATTACK2,
		NORMAL_CHARGING,
		KATANA_ATTACK1,
		KATANA_ATTACK2,
		KATANA_ATTACK3,
		KATANA_HEAVY_ATTACK1,
		KATANA_HEAVY_ATTACK2,
		KATANA_CHARGING,
		MOON_PARRY,
		HIT,
		HARD_HIT,
		DEAD,
		STATE_END
	};
	enum DIR
	{
		DIR_LEFT,
		DIR_RIGHT,
		DIR_FRONT,
		DIR_BACK,
		DIR_LF,
		DIR_RF,
		DIR_LB,
		DIR_RB,
	};
	enum ITEM
	{
		ITEM_NONE,
		HEALTH,
		SOUL,
		ITEM_END
	};
	enum WEAPON
	{
		NORMAL,
		MOON,
		NO_WEAPON
	};
	enum ATTACK
	{
		ATTACK1,
		ATTACK2,
		ATTACK3,
		HEAVY1,
		HEAVY2,
		HEAVY3,
		CHARGE,
		PARRY,
		NO_ATTACK
	};
	enum DAMAGE
	{
		NORMAL_DAMAGE,
		FALL_DAMAGE,
		NO_DAMAGE
	};
private:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	_bool	Get_OnElevetor() { return m_bOnElevater; }
	_bool	Get_bPxAttack();
	WEAPON	Get_Now_Weapon() { return m_eWeapon; }
	_bool	Get_bAttack() { return m_bOnWeapon; }
	_bool	Get_FallDeath() { return m_bFallDeath; }
	_bool	Get_Cheat() { return m_bCheat_Gravity; }
	_bool	Get_LeverOn() { return m_bLeverOn; }
	_bool	Get_Elevator() { return m_bElevator; }
	wstring Get_ElevatorName() { return m_wstrlevator; }
	const wstring& Get_LeverName() { return m_wstrLeverName; }
	const _uint Get_PlayerState() const { return m_ePreState; }
	const _bool&	Get_ResetCamera() { return m_bResetCamera; }


	_bool	Get_Targeting() { return m_bTargeting; }
	_float3	Get_TargetPos() { return m_vTargetPos; }
	_bool	Get_Check_Weapon_Normal() { return m_bGet_Weapon_Normal; }
	_bool	Get_Check_Weapon_Moon() { return m_bGet_Weapon_Moon; }
	_uint	Get_PlayerAni() { return m_pMesh->Get_Animationset(); }
	_bool	Get_Npc_Interacting() { return m_bNPCInteracting; }
	_bool	Get_Npc_Choron_Interact() { return m_bNpcChoronInteract; }
	_bool	Get_Npc_Valnir_Interact() { return m_bNpcValnirInteract; }
	_bool	Get_Npc_Yaak_Interact() { return m_bNpcYaakInteract; }
	_bool	Get_LevelStone_Interact() { return m_bLevelStoneInteract; }
	_bool	Get_WeaponStand_Interact() { return m_bInteract_StandTrigger; }
	_bool	Get_PortalGateOn_Interact() { return m_bPortalGateOn; }
	INTERACT	Get_bCollAct() { return m_eCollAct; }
	_bool	Get_WeaponParry() { return m_bWeaponParry; }
	wstring Get_TargetName() { return m_szTargetName; }

	

	_uint	Get_Stat_Vitality() { return m_iVitality; }
	_uint	Get_Stat_Stamina() { return m_iStamina; }
	_uint	Get_Stat_Strength() { return m_iStrength; }
	_uint	Get_Stat_Armor() { return m_iArmor; }


	void	Set_WeaponParry(_bool _bWeaponParry) { m_bWeaponParry = _bWeaponParry; }
	void	Set_Rotate(_bool _bRotate) { m_bRotate = _bRotate; }
	void	Set_AttackMove(_bool _bAttackMove) { m_bAttackMove = _bAttackMove; }
	void	Set_Stat_Vitality(_uint iVitality) { m_iVitality = iVitality; }
	void	Set_Stat_Stamina(_uint iStamina) { m_iStamina = iStamina; }
	void	Set_Stat_Strength(_uint iStrength) { m_iStrength = iStrength; }
	void	Set_Stat_Armor(_uint iArmor) { m_iArmor = iArmor; }

public:
	void	Set_Npc_Interacting(_bool _bNpcInteract) { m_bNPCInteracting = _bNpcInteract; }
	void	Set_Npc_LevelStone(_bool _bNpcInteract) { m_bLevelStoneInteract = _bNpcInteract; }
	void	Set_Npc_Valnir(_bool _bNpcInteract) { m_bNpcValnirInteract = _bNpcInteract; }
	void	Set_Npc_Chrono(_bool _bNpcInteract) { m_bNpcChoronInteract = _bNpcInteract; }
	void	Set_Npc_Yaak(_bool _bNpcInteract) { m_bNpcYaakInteract = _bNpcInteract; }
	void	Min_ObstacleType(EObstacle _eType);
	void	Set_LeverOn(_bool _bLeverOn) { m_bLeverOn = _bLeverOn; }
	void	Set_Init(_float3 _vPos);
	void	Set_ResetCamera(_bool _bCamera) { m_bResetCamera = _bCamera; }
	void	Set_HitSkill() { m_bHitSkill = false; }
private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT _pEffect);

	void Light_Attack(const _float & _fDeltaTime);
	void Heavy_Attack(const _float & _fDeltaTime);
	void Check_Attack(const _float & _fDeltaTime);
	void Targeting();
	void OnTarget(const _float & _fDeltaTime);

	void Use_QuickItem();
	void Swap_Item();
	void Update_Item();

	void Wake_Up(const _float & _fDeltaTime);
	void Weapon_Swap();

	void Update_Move(const _float & _fDeltaTime);
	void Update_AnimationControl();

	void Player_Interact(const _float & _fDeltaTime);
	void Update_Interact();
	void Player_Rotate(DIR _eDir, const _float & _fDeltaTime, class CMainCamera* _pCamera); // 절대 건들지 마세요...
	void Player_Fall(const _float & _fDeltaTime);
	void Update_Camera_Dir(class CMainCamera* _pCamera);
	void Update_Hit(const _float & _fDeltaTime);
	void Update_Parry();
	void Create_Parry();
	void Delete_Parry();
	void Dash(const _float & _fDeltaTime);

	void Player_UI_Input();

	void CreateBoneParticle();

	void Cheat(const _float & _fDeltaTime);


	void Reset_Parameter();

	// 카메라 관련
	void Update_CameraView();

	//
	void Collider();
	void Update_Dead(const _float & _fDeltaTime);
	// 스텟 관련
	void Ready_Stat();
	void Update_Stat(const _float & _fDeltaTime);
	void Add_PlayerAttack(_float _fDamage, DAMAGE _eDamage);

	void Heal(const _float & _fDeltaTime);
	//엔딩씬 작동용. 이게 작동하면 플레이어 움직이지 못할것.
	void Ending(const _float & _fDeltaTime);

	// 디버그용
	void Print_To_Debug();
	HRESULT AddUIClass();

	//---------------------------------------이펙트 관련---------------------------------------//
	HRESULT AddTrailEffect_Class();

	void DashTrailSwitch(_bool _bOnOff);
	void DashTrailRandomSwitch();
	void DashTrailColor_Set(const _float3& _vColorRgb);
	void DashTrailWidth_Set(const _float& _fWidth);
	void DashTrailRenderId_Set(const _uint& _iRenderId);
	void DashTrailUpdateMatrix();

	void ChargingEffect();
	//---------------------------------------이펙트 관련---------------------------------------//
public:
	static CPlayer* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	class CDynamicMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	STATE						m_eCurState, m_ePreState;
	WEAPON						m_eWeapon;
	ATTACK						m_eAttack;
	ITEM						m_eItem;
	
	_int						m_iLightAttack, m_iHeavyAttack, m_iJump, m_iDash;
	_float						m_fStartTime, m_fRollTime , m_fDashTime ,m_fStaminaCool, m_fAniSpeed, m_fAttackTime, m_fAirTime, m_fFallDamage,m_fCinematicTimer;
	_float3						m_vRollDir, m_vLook, m_vRight, m_vLU, m_vRU, m_vUp, m_vSpawnPos, m_vDashDir,m_vChargingDir,m_vCameraAt,m_vCameraEye;
	_bool						m_bAttack, m_bPxAttack, m_bRoll, m_bStart, m_bTargeting, m_bDashCool, m_bCharging,m_bAttackMove,m_bParry,
								m_bUse_Item, m_bUse_Heal, m_bOnWeapon, m_bDash, m_bInteract, m_bFallDeath, m_bRotate,
								m_bInteract_Stand, m_bStaminaCool, m_bInteract_Save, m_bHit, m_bDashDir,
								m_bHeal, m_bUse, m_bInteract_StandTrigger, m_bNPCInteracting,m_bHeavyShake, m_bLevelStoneInteract,
								m_bChargeAttack, m_bChargingDir,m_bOnElevater,m_bResetCamera,m_bWeaponParry, m_bCinematicTeleport1, m_bCinematicTeleport2, m_bEndingStart;
	_float						m_fPoiseTime;
		
	_bool						m_bUse_Quick_Item;
	_uint						m_iPreScene;
	// 점프 관련
	_bool						m_bJump, m_bJumping, m_bJumpStart, m_bDoubleJump, m_bGravity, m_bGravStart;
	_float						m_fJumpTime;
	_float3						m_vJumpPos;
	PxShape*					m_pPxShape;

	class CPlayer_UI*			m_pInterfaceUI;
	class CUI_Dead*				m_pDeadUI;
	class CUI_Target_Dot*		m_pTargetDot;
	MyPlayerData*				m_pPlayerData;

	// 치트 관련
	_bool						m_bCheat_Gravity;

	// 스텟 관련

	_float						m_fBasicHP, m_fBasicStamina;
	_int						m_iVitality, m_iStamina, m_iStrength, m_iArmor;


	// 힐 관련
	_bool						m_bBonfireHeal;
	_float						m_fHealTime, m_fHealing;
	// 점프 관련

	// 타겟팅

	wstring						m_szTargetName;
	_float3						m_vTargetPos;

	// 레버
	_bool						m_bLeverOn;
	wstring						m_wstrLeverName;

	// 엘리베이터				
	_bool						m_bElevator;
	wstring						m_wstrlevator;

	// 포탈게이트
	_bool						m_bPortalGateOn;

	// 스킬 타격
	_bool						m_bHitSkill;

	// 무기
	_bool						m_bGet_Weapon_Normal, m_bGet_Weapon_Moon;
	_bool						m_bGod_Mode;
	
	//NPC 상호작용
	INTERACT					m_eCollAct;
	_bool						m_bNpcChoronInteract;
	_bool						m_bNpcValnirInteract;
	_bool						m_bNpcYaakInteract;

	//뼈 파티클
	_bool						m_bBoneParticle;


	// 사운드
	void						Update_Sound();
	void						Reset_Sound();

	_bool						m_bRollSound, m_bDashSound;
	_bool						m_bLeftLag, m_bRightLag;
	_bool						m_bJumpSound, m_bJumpOnDown;
	_bool						m_bAttackSound;
	_bool						m_bHitSound, m_bDeadSound;

	// 사운드

	// 디졸브
	void Update_Dsv(const _float & _fDeltaTime);
	void Dash_Dsv(const _float & _fDeltaTime);
	_bool m_bDisolve, m_bDashDsv, m_bDeadDsv, m_bDashReDsv;
	_float m_fDsvTime, m_fDashDsvTime, m_fDashReDsvTime, m_fDeadDsvTime;

	// 디졸브
	//트레일
	class CTrailEffect*			m_pArrDash_TrailEffect[5];
	class CTrailEffect*			m_pArrDash_TrailDistortEffect[5];
	// 트레일
};

#endif // !__PLAYER_H__