#pragma once

#ifndef __MAPSTATICOBJECT_CHURCHDOOR_H__
#define __MAPSTATICOBJECT_CHURCHDOOR_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_ChurchDoor final : public CMapStaticObjectBase
{
protected:
	explicit CMapStaticObject_ChurchDoor(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_ChurchDoor(const CMapStaticObject_ChurchDoor& _rOther);
	virtual ~CMapStaticObject_ChurchDoor() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime);

private:
	void Door_Move(const _float& _fDeltaTime);

public:
	static CMapStaticObject_ChurchDoor* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;

private:
	_bool				m_bClose;
	_float				m_fDownTime;
	_float				m_fParticleTime;
	_float				m_fEndY;
	_bool				m_b3DSound;
};

#endif // !__MAPSTATICOBJECT_CHURCHDOOR_H__