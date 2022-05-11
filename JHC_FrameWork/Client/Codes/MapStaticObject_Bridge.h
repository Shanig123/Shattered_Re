#pragma once

#ifndef __MAPSTATICOBJECT_BRIDGE_H__
#define __MAPSTATICOBJECT_BRIDGE_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_Bridge final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_Bridge(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_Bridge(const CMapStaticObject_Bridge& _rOther);
	virtual ~CMapStaticObject_Bridge() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	static CMapStaticObject_Bridge* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool			m_bCreateRigid;
};

#endif // !__MAPSTATICOBJECT_BRIDGE_H__