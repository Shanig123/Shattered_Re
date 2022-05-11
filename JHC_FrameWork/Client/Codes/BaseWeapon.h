#pragma once
#ifndef __BASEWEAPON_H__
#define __BASEWEAPON_H__

#include "GameObject.h"

USING(Engine)
class CBaseWeapon abstract : public CGameObject
{
protected:
	explicit CBaseWeapon(_DEVICE _pDevice, OBJECTINFO_DESC* _tDesc = nullptr);
	explicit CBaseWeapon(const CBaseWeapon& _rOther);
	virtual ~CBaseWeapon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg) PURE;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) PURE;
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) ;
	virtual HRESULT Render_GameObject();

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

public:
	void			Set_MonsterWorld(_float4x4 _pMonWorldMatrix);
	void			Set_BoneWorld(_float4x4 _pBoneWorldMatrix);
	void			Set_Render(_bool	_bRender);
	void			Set_MonsterDir(const _float3	_pDir);
	void			Set_MonsterDead(_bool _bDead);
	void			Set_MonsterAttack(_bool _bAttack);
	void			Set_Collision(_bool _bColl);
	void			Set_Dissolve(_bool _bDissolve, _int iShaderPass);
	void			Set_Damage(_float _fAtt, _float _fDef);
	void			WeaponTrigger_Dead() { m_bWeaponTrigger = true; }
public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() PURE;

protected:
	class CStaticMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;
	_float4x4					m_pParentBoneMatrix;
	_float4x4					m_pParentWorldMatrix;
	_float4x4					m_matWorld;

	class CDynamicMesh*			m_pMonsterMeshCom;
	_float3						m_vDir;
	_float						m_fDeathTime;
	_float						m_fDissolveTime;
	_float						m_fDissolveSpeed;
	_float						m_fTotalDamage;
	_float						m_fWeaponDamage;

	_bool						m_bRender;
	_bool						m_bAttack;
	_bool						m_bDissolve;
	_bool						m_bCollision;

	_uint						m_iShaderPass;
	wstring						m_strStaticName;

	MyPublicData*				m_WeaponData;
	_bool						m_bWeaponTrigger;

};

#endif // !__BASEWEAPON_H__
