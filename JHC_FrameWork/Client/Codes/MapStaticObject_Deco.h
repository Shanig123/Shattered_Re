#pragma once

#ifndef __MAPSTATICOBJECT_DECO_H__
#define __MAPSTATICOBJECT_DECO_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_Deco final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_Deco(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_Deco(const CMapStaticObject_Deco& _rOther);
	virtual ~CMapStaticObject_Deco() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;

public:
	static CMapStaticObject_Deco* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
};

#endif // !__MapStaticObject_Deco_H__