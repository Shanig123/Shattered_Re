#pragma once
#ifndef __UI_Select_Upgrade_H__
#define __UI_Select_Upgrade_H__

#include "UIBase.h"
#include "UI_Status.h"
#include "UI_Forge.h"

USING(Engine)

class CUI_Select_Upgrade : public CUIBase
{
protected:
	explicit CUI_Select_Upgrade(_DEVICE _pDevice);
	explicit CUI_Select_Upgrade(const CUI_Select_Upgrade& _rhs);
	virtual ~CUI_Select_Upgrade() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;

public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
public:
	void Set_View_SelectUpgrade(_bool _bView) { m_bUIView = _bView; }
	_bool Get_Selected_UpgradeStone () { return m_bSelected; }
private:
	HRESULT SetUp_ConstantTable_Upgrage(_EFFECT & _pEffect);
	
	void Select_Upgrade();
	void KeyInput();
	
	HRESULT Render_Option(_EFFECT & _pEffect);
protected:
	virtual HRESULT Add_Com() override;

public:
	static CUI_Select_Upgrade* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free()override;

private:
	_float				m_fBackDissolveTime;
	_uint				m_iUIDiffuse;
	_uint				m_iOptionNumber;
	_uint				m_iMainRenderPass;
	_bool				m_bUIView;
	_bool				m_bSelected;

	class CTexture*			m_pSelectTexture;
	class CTexture*			m_pTextTexture;

	class CUI_Forge*	m_pForge;

};

#endif // !__UI_Select_Upgrade_H__

