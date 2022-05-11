#pragma once
#ifndef __CMAPDYNAMICOBJECT_LEVER_H__
#define __CMAPDYNAMICOBJECT_LEVER_H__

#include "MapDynamicObjectBase.h"

class CMapDynamicObject_Lever : public CMapDynamicObjectBase
{
private:
	explicit CMapDynamicObject_Lever(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapDynamicObject_Lever(const CMapDynamicObject_Lever& _rOther);
	virtual ~CMapDynamicObject_Lever() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject();

public:
	_bool Get_LeverOn() { return m_bLeverOn; }

public:
	void Set_LeverOn(_bool _bLeverOn) { m_bLeverOn = _bLeverOn; }

public:
	void LeverAni_Play(class CMapStaticObject_Elevator* _pElevatro = nullptr, _int _CinematicIndex = 0);

public:
	static CMapDynamicObject_Lever* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_uint		m_iCinematicIndex;
	_bool		m_bLeverOn;
	_bool		m_bAniPlay;
	_bool		m_bSound;
};

#endif // !__CMAPDYNAMICOBJECT_LEVER_H__
