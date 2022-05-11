#pragma once

#ifndef __MAPSTATICOBJECT_LEVELSTONE_H__
#define __MAPSTATICOBJECT_LEVELSTONE_H__

#include "MapStaticObjectBase.h"
#include "UI_Select_Upgrade.h"
class CMapStaticObject_LevelStone final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_LevelStone(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_LevelStone(const CMapStaticObject_LevelStone& _rOther);
	virtual ~CMapStaticObject_LevelStone() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;

public:
	static CMapStaticObject_LevelStone* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
	class CUI_Select_Upgrade* m_pUpgrader;
};

#endif // !__MAPSTATICOBJECT_LEVELSTONE_H__