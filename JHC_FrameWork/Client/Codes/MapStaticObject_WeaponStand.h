#pragma once

#ifndef __MAPSTATICOBJECT_WEAPONSTAND_H__
#define __MAPSTATICOBJECT_WEAPONSTAND_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_WeaponStand final : public CMapStaticObjectBase
{
protected:
	explicit CMapStaticObject_WeaponStand(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_WeaponStand(const CMapStaticObject_WeaponStand& _rOther);
	virtual ~CMapStaticObject_WeaponStand() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;

public:
	static CMapStaticObject_WeaponStand* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;
public:
	_bool	Get_OnNormalSword() { return m_bOnNormalSword; }
	_bool	Get_OnMoonSword() { return m_bOnMoonSword; }
	void	Set_OnNormalSword(_bool _bOnSword) { m_bOnNormalSword = _bOnSword; }
	void	Set_OnMoonSword(_bool _bOnSword) { m_bOnMoonSword = _bOnSword; }
private:
	void CreateEffect(const _float& _fDeltaTime);
private:
	_bool	m_bOnNormalSword;
	_bool	m_bOnMoonSword;
	_float m_fParticleTime;
};

#endif // !__MAPSTATICOBJECT_WEAPONSTAND_H__