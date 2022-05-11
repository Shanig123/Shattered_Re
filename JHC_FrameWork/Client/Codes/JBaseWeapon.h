#pragma once
#ifndef __JBaseWeapon_H__
#define __JBaseWeapon_H__

#include "GameObject.h"

USING(Engine)

typedef	class CJBaseMonster*	LPBASEMONSTER;

class CJBaseWeapon abstract : public CGameObject
{
public:
	enum class eMonsterParent
	{
		Normal,
		Boss,
		End
	};
protected:
	explicit CJBaseWeapon(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJBaseWeapon(const CJBaseWeapon& _rOther);
	virtual ~CJBaseWeapon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg);
	virtual _uint Update_GameObject(const _float & _fDeltaTime);
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime);
	virtual HRESULT Render_GameObject();

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);
public:
	inline _float3* Get_TrailColor()const { if (m_pTrail)return m_pTrail->Get_Color(); else return nullptr; }
public:
	void Set_ParentClass(const LPPARENTOBJECT& _pParent, const eMonsterParent& _eCategory) { NULL_CHECK_RETURN(_pParent, ); m_pParentObject = _pParent;/* Safe_AddRef(_pParent);*/ m_eParentCategory = _eCategory; }
	void Set_Update(_bool _bUpdate) { m_bUpdate = _bUpdate; }
	void Set_ParentWorld(_float4x4 _matParentWorld) { m_matParentWorld = _matParentWorld; }
	void Set_Acttack(_bool _bAttack);
	void HitPosToEffect();
	void Set_TriggerDead();
	//void DissolvePlay(DissolveID _eType, _float _fDissolvTime, _float _fDissolveSpeed, _bool _bRevers);
	inline _float Set_AtkDamage(const _float& _fAtt, const _float& _fDef) { return (_fAtt * 0.5f - _fDef * 0.25f + m_fWeaponDamage); };
	inline void Trail_Switch(_bool _bRenderOnOff) { if (m_pTrail) { m_pTrail->Set_RenderOnOff(_bRenderOnOff); } }
	inline void Set_TrailRateTime(const _float& _fTime) { if (m_pTrail) { m_pTrail->Set_TrailRateTime(_fTime); } }
	inline void Set_TrailWidth(const _float& _fWidth){ if (m_pTrail) { m_pTrail->Set_Width(_fWidth); } }
	virtual HRESULT WeaponSound(_uint iSoundIndex) PURE;
protected:
	////////////////////////////////////////////////////
	// 타격 위치 파티클 생성
	void Set_PxWeaponWorld();
	void Set_BossPxWeaponWorld();
	//void HitPosToEffect();
	virtual HRESULT Clone_TrailEffect() PURE;

private:
	void Check_ParentDissolve();
public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() PURE;

protected:
	class CStaticMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	CTrailEffect*				m_pTrail;
	LPPARENTOBJECT				m_pParentObject;

	eMonsterParent				m_eParentCategory;

	_bool						m_bUpdate, m_bDissolve, m_PxRigidCreate, m_bCamMode;
	_float						m_fWeaponDamage;
	_float						m_fDissolveTime, m_fDissolveSpeed;
	_float						m_fSoundMultiple;
	_uint						m_iShaderPass;
	_uint						m_iDiffuseIndex;

	_float4x4					m_matParentWorld;

	////////////////////////////////////////////////////
	// 타격 위치 파티클 생성
	_bool						m_bHitEffectOnce;
	_bool						m_bAttack;
};

#endif // !__JBaseWeapon_H__
