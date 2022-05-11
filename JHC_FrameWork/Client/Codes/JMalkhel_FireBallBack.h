#pragma once
#ifndef __JMALKIHEL_FIREBALLBACK_H__
#define __JMALKIHEL_FIREBALLBACK_H__

#include "GameObject.h"

class CJMalkhel_FireBallBack : public CGameObject
{
protected:
	explicit CJMalkhel_FireBallBack(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CJMalkhel_FireBallBack(const CJMalkhel_FireBallBack& _rOther);
	virtual ~CJMalkhel_FireBallBack() = default;

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
	void FireBallBack_Render(_bool _bRender);
	void Set_FireBallBackPos(const _float4x4& _matWorld);
	void DissolveOn();
	void ReDissolveOn();

private:
	void Dissolve(const _float & _fDeltaTime);

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
	_float				m_fDeltaTime;
	_float4x4			m_matParent;

public:
	static CJMalkhel_FireBallBack* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
};

#endif // !__JMALKIHEL_FIREBALLBACK_H__
