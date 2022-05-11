#pragma once
#ifndef __UIBASE_H__
#define __UIBASE_H__

#ifndef _AFXDLL

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUIBase : public CGameObject
{
public:
	typedef struct tagUIDesc
	{
		tagUIDesc() :vSize(_float2(0.f, 0.f)), vPos(_float3(0.f,0.f,1.f)), szTexName(L"") ,tArgb (255,255,255,255){}
		_float2		vSize;
		_float3		vPos;
		_ARGB		tArgb;
		_tchar		szTexName[MAX_PATH];
	}UI_DESC;
protected:
	explicit CUIBase(_DEVICE _pDevice);
	explicit CUIBase(const CUIBase& _rhs);
	virtual ~CUIBase();

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() PURE;
protected:
	HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);
	virtual HRESULT Add_Com();
protected:
	
	class CTransform* m_pTransform;
	//class CTexture*	m_pTexture;
	class CVIBuffer*	m_pVIBuffer;
	class CShader*		m_pShader;
	
	UI_DESC		m_tUIDesc;

	_float		m_fDissolveTime;
	_uint		m_iShaderPass;
	_uint		m_iDissolveId;
public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free()override;
};
END
#endif // !_AFXDLL

#endif // !__UIBASE_H__