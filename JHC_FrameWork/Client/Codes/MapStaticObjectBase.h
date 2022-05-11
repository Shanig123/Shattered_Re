#pragma once

#ifndef __MAPSTATICOBJECTBASE_H__
#define __MAPSTATICOBJECTBASE_H__

#include "GameObject.h"

USING(Engine)

class CMapStaticObjectBase abstract : public CGameObject
{
protected:
	explicit CMapStaticObjectBase(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObjectBase(const CMapStaticObjectBase& _rOther);
	virtual ~CMapStaticObjectBase() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual _uint Update_GameObject(const _float & _fDeltaTime);
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime);
	virtual HRESULT Render_GameObject();

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

public:
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free();

protected:
	class CStaticMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	_float						m_fRimPower;
	_float						m_fRimDistanceFix;
	_bool						m_bCheckRimDistance;
	_uint						m_iShaderPass;
	_bool						m_bRePos;
};

#endif // !__MapStaticObjectBase_H__