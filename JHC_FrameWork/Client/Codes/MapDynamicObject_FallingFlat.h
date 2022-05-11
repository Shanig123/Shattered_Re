#pragma once
#ifndef __MAPDYNAMICOBJECT_FALLINGFLAT_H__
#define __MAPDYNAMICOBJECT_FALLINGFLAT_H__

#include "MapDynamicObjectBase.h"

class CMapDynamicObject_FallingFlat : public CMapDynamicObjectBase
{
private:
	explicit CMapDynamicObject_FallingFlat(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapDynamicObject_FallingFlat(const CMapDynamicObject_FallingFlat& _rOther);
	virtual ~CMapDynamicObject_FallingFlat() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject();

private:
	void Do_Awak();
	void Do_Idle(const _float & _fDeltaTime);
	void Do_AniStart(const _float & _fDeltaTime);

public:
	static CMapDynamicObject_FallingFlat* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_float	m_fDropTimer;
	_float	m_fReviveTimer;
	_bool	m_bPlayDead;

	_bool		m_b3DSound[2];
	_uint		m_iSound;
};

#endif // !__MAPDYNAMICOBJECT_SAVEPOINT_H__
