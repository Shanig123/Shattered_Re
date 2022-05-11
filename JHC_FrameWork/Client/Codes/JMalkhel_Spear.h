#pragma once
#ifndef __JMALKIHEL_Spear_H__
#define __JMALKIHEL_Spear_H__

#include "GameObject.h"

class CJMalkhel_Spear : public CGameObject
{
protected:
	explicit CJMalkhel_Spear(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJMalkhel_Spear(const CJMalkhel_Spear& _rOther);
	virtual ~CJMalkhel_Spear() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;	
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT	Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);
	void SkillUpdate(const _float& _fDeltaTime);
	void SKillCoolTimer(const _float& _fDeltaTime);
	void SkillDelete(const _uint& _iSkillIndex);
	_float3 SKillPosToPlayer();

public:
	void SkillCreate(const _float _fRotateY, const _float& _fDamage);

public:
	static CJMalkhel_Spear* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	class CStaticMesh*	m_pMesh;
	class CTransform*	m_pTransform;
	class CShader*		m_pShader;

	_bool				m_bFirst;
	_bool				m_bUpdate;
	_bool				m_bRender;
	_bool				m_bFlyStart;

	_float				m_fFlyDelayTime;
	_float				m_fDist;
	_float				m_fInputRotateY;
	_float				m_fOldRotateY;

	_float				m_fAccTime;

	_float3				m_vPlayerPos;
	_float3				m_vCreatePos;
	_float				m_fDeltaTime;	

	_uint				m_icount;
};

#endif // !__JMALKIHEL_Spear_H__
