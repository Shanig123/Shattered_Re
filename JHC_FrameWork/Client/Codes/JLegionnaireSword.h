#pragma once
#ifndef __JLEGIONNAIRESWORD_H__
#define __JLEGIONNAIRESWORD_H__

#include "JBaseWeapon.h"

USING(Engine)

class CJLegionnaireSword : public CJBaseWeapon
{
protected:
	explicit CJLegionnaireSword(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJLegionnaireSword(const CJLegionnaireSword& _rOther);
	virtual ~CJLegionnaireSword() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Clone_TrailEffect()override;
public:
	virtual HRESULT WeaponSound(_uint iSoundIndex) override;
public:
	static CJLegionnaireSword* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

};

#endif // !__JLEGIONNAIRESWORD_H__