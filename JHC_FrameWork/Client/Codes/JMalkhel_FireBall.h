#pragma once
#ifndef __JMALKIHEL_FIREBALL_H__
#define __JMALKIHEL_FIREBALL_H__

#include "GameObject.h"

class CJMalkhel_FireBall : public CGameObject
{
private:
	enum class ERender
	{
		FireBall,
		Weapon,
		End
	};
protected:
	explicit CJMalkhel_FireBall(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJMalkhel_FireBall(const CJMalkhel_FireBall& _rOther);
	virtual ~CJMalkhel_FireBall() = default;

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
	void SkillDelete();


	HRESULT Clone_Distortion();
public:
	void SkillCreate(const _float3& _vCreatePos, const _float3& _vTarget, const _float& _fDamage, _bool _bPage2);
	


private:
	class CStaticMesh*	m_pMesh[(_uint)ERender::End];
	class CTransform*	m_pTransform;	
	class CShader*		m_pShader;
	class CEffect_Texture*	m_pDistortionTex;
	class CEffect_Texture*	m_pDistortionTexLarge;


	_bool				m_bUpdate;		
	_bool				m_bRender;
	_bool				m_bPage2;
	_float3				m_bTargetPos;
	_float				m_fOldRotateY;
	_float				m_fRotateTime;

	ERender				m_eRender;
	_bool				m_bSetting;

	_uint				m_iRotateState;

	_float				m_fDeleteTime;
	_float				m_fDissolveTime;
	_float				m_fMaxDissolve;

public:
	static CJMalkhel_FireBall* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
};

#endif // !__JMALKIHEL_FIREBALL_H__
