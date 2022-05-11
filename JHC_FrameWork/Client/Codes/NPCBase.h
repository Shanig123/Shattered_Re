#pragma once

#ifndef __NPCBase_H__
#define __NPCBase_H__

#include "GameObject.h"

USING(Engine)

class CNPCBase abstract : public CGameObject
{
protected:
	explicit CNPCBase(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CNPCBase(const CNPCBase& _rOther);
	virtual ~CNPCBase() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr);
	virtual _uint Update_GameObject(const _float & _fDeltaTime);
	virtual HRESULT Render_GameObject() PURE;

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT _pEffect);

public:
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() PURE;

protected:
	class CDynamicMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	_float						m_fDeltaTime;
	_float						m_fRimPower;
};

#endif // !__NPCBase_H__