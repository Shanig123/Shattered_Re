#pragma once
#ifndef __JPretoSickle_h__
#define __JPretoSickle_h__

#include "JBaseWeapon.h"

USING(Engine)

class CJPretoSickle : public CJBaseWeapon
{
protected:
	explicit CJPretoSickle(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJPretoSickle(const CJPretoSickle& _rOther);
	virtual ~CJPretoSickle() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;

public:/*Setter*/
	void Set_Trail_LocalWorld(const _float4x4& _matLocal);
public:
	virtual HRESULT WeaponSound(_uint iSoundIndex) override;
private:
	virtual HRESULT Clone_TrailEffect() override;
private:
	CTrailEffect* m_pTrail0;
	CTrailEffect* m_pTrail1;
	CTrailEffect* m_pTrail2;
	CTrailEffect* m_pDistortTrail;
public:
	static CJPretoSickle* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;



};

#endif // !__JPretoSickle_h__