#pragma once

#ifndef __MAPSTATICOBJECT_ELEVATOR_H__
#define __MAPSTATICOBJECT_ELEVATOR_H__

#include "MapStaticObjectBase.h"
#include "MapDynamicObject_Lever.h"

class CMapStaticObject_Elevator final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_Elevator(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_Elevator(const CMapStaticObject_Elevator& _rOther);
	virtual ~CMapStaticObject_Elevator() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;

private:
	HRESULT Add_Lever();

public:
	_bool Get_MovingOn() { return m_bMovingOn; }
	_bool Get_MovingDir() { return m_bMovingDown; }
	_float3	Get_Pos() { return m_bEleavtorPos; }

public:
	void Set_MovingOn() { m_bMovingOn = !m_bMovingOn; }
	void Set_MovingDir(_bool _bMovingDown) { m_bMovingDown = _bMovingDown; }

public:
	static CMapStaticObject_Elevator* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	class CMapDynamicObject_Lever*	m_pLeverMesh;
	class CTransform*				m_pLeverTransform;
	TRANSFORM_DESC					m_pLeverTransformDesc;

	_bool							m_bMovingOn, m_bMovingDown;
	_bool							m_b3DSound, m_bSound;
	_float3							m_bEleavtorPos, m_bPlayerPos;
};

#endif // !__MAPSTATICOBJECT_ELEVATOR_H__