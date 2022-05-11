#pragma once
#ifndef __JMALKIHEL_EFFECT_360_H__
#define __JMALKIHEL_EFFECT_360_H__

#include "GameObject.h"

class CJMalkhel_Effect_360 : public CGameObject
{
protected:
	explicit CJMalkhel_Effect_360(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJMalkhel_Effect_360(const CJMalkhel_Effect_360& _rOther);
	virtual ~CJMalkhel_Effect_360() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;	
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT	Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

public:
	void Effect_360_Render(_bool _bRender);
	void Set_Effect_360Pos(const _float3& _vPos);
	void DissolveOn();
	void ReDissolveOn();

private:
	void Dissolve(const _float & _fDeltaTime);
	void UpdateSize(const _float & _fDeltaTime);

private:
	class CStaticMesh*	m_pMesh;		
	class CTransform*	m_pTransform;	
	class CShader*		m_pShader;

	_bool				m_bUpdate;		
	_bool				m_bRender;
	_bool				m_bStart;
	_bool				m_bEnd;
	_uint				m_iRenderPass;
	_float				m_fDissolveTime;
	_float				m_fDissolvePower;
	_float3				m_vSize;

public:
	static CJMalkhel_Effect_360* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
};

#endif // !__JMALKIHEL_EFFECT_360_H__
