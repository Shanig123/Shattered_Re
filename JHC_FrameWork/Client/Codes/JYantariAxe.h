#pragma once
#ifndef __JYantariAxe_H__
#define __JYantariAxe_H__

#include "JBaseWeapon.h"

USING(Engine)

class CJYantariAxe : public CJBaseWeapon
{
protected:
	explicit CJYantariAxe(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJYantariAxe(const CJYantariAxe& _rOther);
	virtual ~CJYantariAxe() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
protected:
	virtual HRESULT Clone_TrailEffect() override;
	virtual HRESULT WeaponSound(_uint iSoundIndex) override;
private:
	CTrailEffect*		m_pTrailSecond;

public:
	static CJYantariAxe* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;


};

#endif // !__JJYantariAxe_H__
