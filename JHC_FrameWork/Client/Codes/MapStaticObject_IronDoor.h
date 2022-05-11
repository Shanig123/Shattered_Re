#pragma once

#ifndef __MapStaticObject_IronDoor_H__
#define __MapStaticObject_IronDoor_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_IronDoor final : public CMapStaticObjectBase
{
protected:
	explicit CMapStaticObject_IronDoor(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_IronDoor(const CMapStaticObject_IronDoor& _rOther);
	virtual ~CMapStaticObject_IronDoor() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime);

private:
	void Door_Move(const _float& _fDeltaTime);
	//void Create_Effect(const _float& _fDeltaTime);
public:
	static CMapStaticObject_IronDoor* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;

private:
	_float				m_fDownTime;
	_float				m_fParticleTime;
};

#endif // !__MapStaticObject_IronDoor_H__