#pragma once

#ifndef __MAPDYNAMICOBJECTBASE_H__
#define __MAPDYNAMICOBJECTBASE_H__

#include "GameObject.h"

USING(Engine)

class CMapDynamicObjectBase abstract : public CGameObject
{
protected:
	enum class eDynamicObjectPattern
	{
		Awak,
		Idle,
		Start,
		Delete,
		Sleep,
		End
	};
protected:
	explicit CMapDynamicObjectBase(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapDynamicObjectBase(const CMapDynamicObjectBase& _rOther);
	virtual ~CMapDynamicObjectBase() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual _uint Update_GameObject(const _float & _fDeltaTime);
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime);
	virtual HRESULT Render_GameObject();

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

protected:
	HRESULT PatternTotalControl(const _float& _fDeltaTime);

private:
	void Player_EventCheck();

private:
	void Create_RigidBody();
	void Delete_RigidBody();



public:
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() PURE;

protected:
	class CDynamicMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;
	_float						m_fDeltaTime;

	eDynamicObjectPattern		m_eCurPattern;
	eDynamicObjectPattern		m_ePrePattern;

	_float						m_fRimPower;
	_uint						m_iShaderPass;
	_bool						m_bCheckRimDistance;
	//MyPublicData*				m_ObjectData;
};

#endif // !__MAPDYNAMICOBJECTBASE_H__