#pragma once
#ifndef __MAPDYNAMICOBJECT_SAVEPOINT_H__
#define __MAPDYNAMICOBJECT_SAVEPOINT_H__

#include "MapDynamicObjectBase.h"

class CMapDynamicObject_SavePoint : public CMapDynamicObjectBase
{
private:
	explicit CMapDynamicObject_SavePoint(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapDynamicObject_SavePoint(const CMapDynamicObject_SavePoint& _rOther);
	virtual ~CMapDynamicObject_SavePoint() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual HRESULT Render_GameObject();

public:
	static CMapDynamicObject_SavePoint* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
public:/*Setter*/
	void Set_Active() { m_bActive = true; }
	void On_Sound();
private:
	_bool		m_bActive;
private:
	//HRESULT Create_TexAni();

	//class CEffect_Texture_Ani* m_pFire;
	HRESULT Clone_Distortion();

	class CEffect_Texture*		m_pDistort;
	_float	m_fAccTime;

private:
	void GenerateParticle(const _float& _fDeltaTime);
	void Check_GenerateParticle(const _float& _fDeltaTime);

	_float					m_fParticleCreateCoolTime0;
	_float					m_fParticleCreateCoolTime1;
	_float					m_fParticleCreate_Range;
	
};

#endif // !__MAPDYNAMICOBJECT_SAVEPOINT_H__
