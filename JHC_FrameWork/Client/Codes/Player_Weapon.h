#pragma once

#ifndef __PLAYER_NORMAL_WEAPON_H__
#define __PLAYER_NORMAL_WEAPON_H__

#include "GameObject.h"

USING(Engine)

class CPlayer_Weapon final : public CGameObject
{
public:
	enum WEAPON_STATE
	{
		NORMAL,
		MOON,
		WEAPON_STATE_END
	};

private:
	// 무기 강화 전용 스탯
	typedef struct TagWeaponStat
	{
		_int	iNormalLevel;	// 기본0
		_int	iMoonLevel;		// 기본0 
		_float	fNormalDamage;	// 기본10
		_float	fMoonDamage;	// 기본7
	}WEAPONSTAT;


private:
	explicit CPlayer_Weapon(_DEVICE _pDevice);
	explicit CPlayer_Weapon(const CPlayer_Weapon& _rOther);
	virtual ~CPlayer_Weapon() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	_bool	Get_Take_Normal() { return m_bTake_Normal; }
	_bool	Get_Take_Moon() { return m_bTake_Moon; }
	void	Set_Weapon(Player_Weapon _eWeapon) { m_eWeapon = _eWeapon; }
	void	Set_Take_Normal(_bool _bTake) { m_bTake_Normal = _bTake; }
	void	Set_Take_Moon(_bool _bTake) { m_bTake_Moon = _bTake; }
	void	Set_Reset_AttackCount() { m_iAttackCount = 0; }
	void	Set_AttackColl(_bool _bAttackColl) { m_bAttackColl = _bAttackColl; }
	void	Set_StandDsv() { m_bOnStand_Dsv = true; }


	void	Set_OnMoon_Dsv(_bool _bOnMoon) { m_bOnMoon_Dsv = _bOnMoon; }
	void	Set_OnMoon_ReverseDsv(_bool _bOnMoon) { m_bOnMoon_ReverseDsv = _bOnMoon; }
	void	Set_OnNorma_Dsv(_bool _bOnMoon) { m_bOnNormal_Dsv = _bOnMoon; }
	void	Set_OnNormal_ReverseDsv(_bool _bOnMoon) { m_bOnNormal_ReverseDsv = _bOnMoon; }

	void	Update_Damage();

	void	Set_TrailRenderOnOff(_bool _bRenderOnOff);

	void	Set_Init();

	WEAPONSTAT	Get_WeaponStat() { return m_tWeaponStat; }

	void		Set_WeaponLevelUp(WEAPON_STATE _eStat, _int i);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT _pEffect);
	HRESULT Clone_Trail();
	HRESULT Render_Choice();
public:
	static CPlayer_Weapon* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual void Free() override;

private:
	void Update_OnPlayer();
	void Update_OnStand(const _float & _fDeltaTime);
	void Update_AttackColl();
	void Update_Dsv(const _float & _fDeltaTime);
	////////////////////////////////////////////////////
	// 타격 위치 파티클 생성
	void HitPosToEffect();
	void Generate_Particle(MyRayCast* RayCast , const _float4Q& _qQuat);

	void CheckTrailEffect();
private:
	class CStaticMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	const _float4x4*			m_ParentBoneMatrix;
	const _float4x4*			m_ParentWorldMatrix;

	_bool						m_bTake_Normal;
	_bool						m_bTake_Moon;
	_bool						m_bOnHand;
	_bool						m_bStand;
	_bool						m_bAttackColl;
	_bool						m_bOnStand_Dsv;
	_bool						m_bOnMoon_Dsv;
	_bool						m_bOnNormal_Dsv;
	_bool						m_bOnMoon_ReverseDsv;
	_bool						m_bOnNormal_ReverseDsv;
	_bool						m_bCheat;
	_bool						m_bCollision;
	_int						m_iAttackCount;

	_float						m_fDsvTime;
	_float						m_fDsvTimer;
	_float						m_fReDsvTime;
	_float						m_fDeltaTime;

	WEAPON_STATE				m_eState;
	Player_Weapon				m_eWeapon;

	CTrailEffect*				m_pTrailEffect;
	CTrailEffect*				m_pTrailEffectSecond;
	CTrailEffect*				m_pTrailEffectDistortion;

	////////////////////////////////////////////////////
	// 무기 강화 전용 스탯
	WEAPONSTAT	m_tWeaponStat;

	void Update_Sound();
	_bool						m_bAttackSound;
	_bool						m_bCollWallSound;

	
};

#endif // !__PLAYER_NORMAL_WEAPON_H__