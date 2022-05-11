#pragma once

#ifndef __MAPSTATICOBJECT_PROTALGATE_H__
#define __MAPSTATICOBJECT_PROTALGATE_H__

#include "MapStaticObjectBase.h"

class CMapStaticObject_PortalGate final : public CMapStaticObjectBase
{
private:
	explicit CMapStaticObject_PortalGate(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CMapStaticObject_PortalGate(const CMapStaticObject_PortalGate& _rOther);
	virtual ~CMapStaticObject_PortalGate() = default;
	
protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime)override;
public:
	_bool Get_PortalOn() { return m_bPortalOn; }

public:
	void Set_PortalOff() { m_bPortalOn = false; }

private:
	void CreateParticle(const _float& _fDeltaTime);
	void PortalActivate(const _float& _fDeltaTime);
	void PortalOn(const _float& _fDeltaTime);
	void PortalOnTimer(const _float& _fDeltaTime);

private:
	HRESULT Clone_DistortionEffect();
public:
	static CMapStaticObject_PortalGate* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;

private:
	class CEffect_Texture* m_pDistortion;
	_float		m_fDistortionAlpha;


	_float		m_fPortalOnTime;
	_float		m_fParticleTime;
	_float		m_fAccTime;
	_bool		m_bPortalOn;
	_bool		m_bSound;
	_bool		m_b3DSound;
};

#endif // !__MAPSTATICOBJECT_PROTALGATE_H__