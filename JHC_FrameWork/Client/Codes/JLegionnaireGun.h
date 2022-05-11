#pragma once
#ifndef __JLegionnaireGun_H__
#define __JLegionnaireGun_H__

#include "JBaseWeapon.h"

USING(Engine)

class CJLegionnaireGun : public CJBaseWeapon
{
protected:
	explicit CJLegionnaireGun(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJLegionnaireGun(const CJLegionnaireGun& _rOther);
	virtual ~CJLegionnaireGun() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Clone_TrailEffect()override;
private:
	void BulletMaker();

public:
	virtual HRESULT WeaponSound(_uint iSoundIndex) override;
	void Set_BulletFire(const _float3 & _vLook);
	void Set_MuzzleWorld();

public:
	static CJLegionnaireGun* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
private:
	class CJBaseWeapon* m_pBullet;
	_float3 m_vMuzzlePos;
	_float3 m_vMuzzleLook;
	_float3 m_vWorldMuzzlePos;
};

#endif // !__JLegionnaireGun_H__