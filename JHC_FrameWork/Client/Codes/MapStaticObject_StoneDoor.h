#pragma once

#ifndef __MAPSTATICOBJECT_STONEDOOR_H__
#define __MAPSTATICOBJECT_STONEDOOR_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_StoneDoor final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_StoneDoor(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_StoneDoor(const CMapStaticObject_StoneDoor& _rOther);
	virtual ~CMapStaticObject_StoneDoor() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime);

private:
	void Door_Move(const _float& _fDeltaTime);

public:
	static CMapStaticObject_StoneDoor* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_float				m_fUpTime;
};

#endif // !__MAPSTATICOBJECT_STONEDOOR_H__