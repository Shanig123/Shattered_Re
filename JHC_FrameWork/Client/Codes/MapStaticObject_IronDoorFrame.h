#pragma once

#ifndef __MAPSTATICOBJECT_IRONDOORFRAME_H__
#define __MAPSTATICOBJECT_IRONDOORFRAME_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_IronDoorFrame final : public CMapStaticObjectBase
{
protected:
	explicit CMapStaticObject_IronDoorFrame(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_IronDoorFrame(const CMapStaticObject_IronDoorFrame& _rOther);
	virtual ~CMapStaticObject_IronDoorFrame() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime);
private:
	void Create_Effect(const _float& _fDeltaTime);
private:
	_float	m_fParticleTime;
public:
	static CMapStaticObject_IronDoorFrame* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;
};

#endif // !__MAPSTATICOBJECT_IRONDOORFRAME_H__