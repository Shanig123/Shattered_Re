#pragma once

#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(_DEVICE _pDevice);
	explicit CGameObject(const CGameObject& _rOther);
	virtual ~CGameObject() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) PURE;
public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) PURE;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime);
	virtual HRESULT Render_GameObject() PURE;
	HRESULT Shadow_GameObject();

protected:
	HRESULT Add_Component(_uint _iSceneIndex, const wstring& _szPrototype, const wstring& _szComponent, EComponentType _eComponentType, class CComponent** _ppOut = nullptr, void* _pArg = nullptr);
	HRESULT Add_Mesh(_uint _iSceneIndex, const wstring& _szPrototype, const wstring& _szComponent, class CMesh** _ppOut = nullptr, void* _pArg = nullptr);
	void Compute_ViewZ(const _float3* vPos);

public:
	class CComponent* Get_Component(const wstring& _szComponent);
	HRESULT Delete_Component();
	HRESULT Delete_Component(const wstring& _szComponent);
	class CMesh* Get_Mesh(const wstring& _szComponent);
	_bool Get_Dead() { return m_bDead; }
	_float& Get_ViewZ() { return m_fViewZ; }
	const _bool& Get_CheckClone()const { return m_bCheckClone; }
	const _uint& Get_SceneIdx()const { return m_iSceneIdx; }
	OBJECTINFO_DESC& Get_ObjectInfo() { return m_tObjectInfoDesc; }
	wstring& Get_PrototypeName() { return m_PrototypeName; }
	TRANSFORM_DESC& Get_TransformDesc() { return m_tTransformDesc; }
	_bool Get_UpdateInit() { return m_bUpdateInit; }
public:
	void Set_Dead(_bool _bDead) { m_bDead = _bDead; }

	//가급적 사용하지 말아주십시오.
	void Set_ForceCheckClone() { m_bCheckClone = true; }
	void Set_SceneIdx(const _uint& _iSceneIdx) { memcpy(&m_iSceneIdx, &_iSceneIdx,sizeof(_uint)); }

public:
	void Set_UpdateInit() { m_bUpdateInit = true; }

public:
	void Add_Damage(_uint _iSceneIndex, const wstring& _szGameObject, _uint _iDamage);
	STAT_DESC Get_Stat();

public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() override;

protected:
	typedef map<wstring, class CComponent*>	COMPONENTS;
	typedef map<wstring, class CMesh*>	MESHS;

	COMPONENTS		m_Components[(_uint)EComponentType::End];	// 컴포넌트의 복제본들 보관
	MESHS			m_Meshs;	// 메쉬의 복제본들 보관
	_DEVICE			m_pDevice;

	_bool			m_bCheckClone;
	_bool			m_bUpdateInit;
	_bool			m_bDead;

	_uint			m_iSceneIdx;
	_int			m_iEvent;

	_float			m_fViewZ;

	TRANSFORM_DESC	m_tTransformDesc;
	OBJECTINFO_DESC m_tObjectInfoDesc;
	STAT_DESC		m_tState;

	wstring			m_PrototypeName;
	
};
typedef class CGameObject*		LPPARENTOBJECT;

END

#endif // !__GAMEOBJECT_H__