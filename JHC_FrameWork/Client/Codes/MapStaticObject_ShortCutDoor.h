#pragma once

#ifndef __MAPSTATICOBJECT_SHORTCUTDOOR_H__
#define __MAPSTATICOBJECT_SHORTCUTDOOR_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_ShortCutDoor final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_ShortCutDoor(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_ShortCutDoor(const CMapStaticObject_ShortCutDoor& _rOther);
	virtual ~CMapStaticObject_ShortCutDoor() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;

private:
	void Set_Hit(_float& _fDamage);

public:
	static CMapStaticObject_ShortCutDoor* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_int				m_iHitCount;
	_bool				m_bActivite, m_bPower;
	_float				m_fDeleteTime;

	_bool				m_bGravity, m_bGraviStart;
	_float				m_fJumpTime;
	_float3				m_vGravityPos;
};

#endif // !__MAPSTATICOBJECT_SHORTCUTDOOR_H__