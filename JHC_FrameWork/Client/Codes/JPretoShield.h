#pragma once
#ifndef __JPRETOSHIELD_H__
#define __JPRETOSHIELD_H__

#include "JBaseWeapon.h"

USING(Engine)

class CJPretoShield : public CJBaseWeapon
{
protected:
	explicit CJPretoShield(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJPretoShield(const CJPretoShield& _rOther);
	virtual ~CJPretoShield() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Clone_TrailEffect() override;
public:
	virtual HRESULT WeaponSound(_uint iSoundIndex) override;
public:
	static CJPretoShield* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;



};

#endif // !__JPRETOSHIELD_H__