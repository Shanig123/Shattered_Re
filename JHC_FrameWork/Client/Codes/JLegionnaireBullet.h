#pragma once
#ifndef __JLegionnaireBullet_H__
#define __JLegionnaireBullet_H__

#include "JBaseWeapon.h"

USING(Engine)

class CJLegionnaireBullet : public CJBaseWeapon
{
public:
	enum class eBulletState
	{
		Ready,
		Shoot,
		End
	};
private:
	explicit CJLegionnaireBullet(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJLegionnaireBullet(const CJLegionnaireBullet& _rOther);
	virtual ~CJLegionnaireBullet() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject()override;
	virtual HRESULT Clone_TrailEffect()override;
protected:
	virtual HRESULT	Add_Component() override;
public:
	void Set_ShootAndLook(const _float3 & _vLook, const _float3 & _vPos);
	void Delete_Rigidbody();
	virtual HRESULT WeaponSound(_uint iSoundIndex) override;
public:
	_uint Get_BulletState() { return (_uint)m_eBulletState; }
private:
	void Shooting(const _float & _fDeltaTime);
public:
	static CJLegionnaireBullet* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
private:
	_float	m_fLifeTime;
	_float3 m_vLook;
	_float3 m_vOriginPos;
	eBulletState m_eBulletState;
	_bool			m_bCoolTime;
};

#endif // !__JLegionnaireBullet_H__