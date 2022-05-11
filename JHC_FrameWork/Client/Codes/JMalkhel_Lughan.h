#pragma once
#ifndef __JMALKIHEL_LUGHAN_H__
#define __JMALKIHEL_LUGHAN_H__

#include "JBaseWeapon.h"
class CJMalkhel_Lughan : public CJBaseWeapon
{
protected:
	explicit CJMalkhel_Lughan(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJMalkhel_Lughan(const CJMalkhel_Lughan& _rOther);
	virtual ~CJMalkhel_Lughan() = default;

protected:
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
private:
	virtual HRESULT Clone_TrailEffect()override;
public:
	virtual HRESULT WeaponSound(_uint iSoundIndex) override;
private:
	CTrailEffect*		m_pTrailSmall1;
	CTrailEffect*		m_pTrailSmall2;

	CTrailEffect*		m_pDistortTrail;

public:
	static CJMalkhel_Lughan* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

};

#endif // !__JMALKIHEL_LUGHAN_H__
