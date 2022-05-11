#pragma once

#ifndef __MAPSTATICOBJECT_YAAKSTAND_H__
#define __MAPSTATICOBJECT_YaakStand_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_YaakStand final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_YaakStand(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_YaakStand(const CMapStaticObject_YaakStand& _rOther);
	virtual ~CMapStaticObject_YaakStand() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;

public:
	static CMapStaticObject_YaakStand* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
};

#endif // !__MAPSTATICOBJECT_YAAKSTAND_H__