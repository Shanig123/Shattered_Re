#pragma once

#ifndef __DYNAMICMESH_OBJECT_H__
#define __DYNAMICMESH_OBJECT_H__

#include "GameObject.h"

USING(Engine)

class CDynamicMesh_Object final : public CGameObject
{
private:
	explicit CDynamicMesh_Object(_DEVICE _pDevice, OBJECTINFO_DESC& _tDesc);
	explicit CDynamicMesh_Object(const CDynamicMesh_Object& _rOther);
	virtual ~CDynamicMesh_Object() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT _pEffect);
	_bool Collision();

public:
	class CSphere_Collider* Add_Collider(const wstring& _pBoneName);
	class CSphere_Collider* Add_Collider(const SPHERECOLLIDER_DESC& _tSphereColliderDesc);
	void Delete_Collider(const wstring& _pBoneName);

public:
	map<wstring, class CSphere_Collider*>& Get_SphereCollider() { return m_mapSphereCollider; }

public:
	void Set_Select(_bool _bSelect) { m_bSelect = _bSelect; }

public:
	static CDynamicMesh_Object* Create(_DEVICE _pDevice, OBJECTINFO_DESC& _tDesc);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	class CDynamicMesh*								m_pMesh;
	map<wstring, class CSphere_Collider*>			m_mapSphereCollider;
	class CTransform*								m_pTransform;
	class CShader*									m_pShader;
	class CCalculator*								m_pCalculator;

	_bool											m_bSelect;
};

#endif // !__DYNAMICMESH_OBJECT_H__
