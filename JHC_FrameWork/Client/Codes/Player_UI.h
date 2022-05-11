#pragma once

#ifndef __PLAYER_UI_H__
#define __PLAYER_UI_H__

#include "UIBase.h"

USING(Engine)

class CPlayer_UI final : public CUIBase
{
protected:
	explicit CPlayer_UI(_DEVICE _pDevice);
	explicit CPlayer_UI(const CPlayer_UI& _rOther);
	virtual ~CPlayer_UI() = default;

private:
	enum eMENU { MENU_INVEN, MENU_STAT, MENU_WEAPON, MENU_END };

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	const _bool& Get_CheckViewInterface()const { return m_bViewInterface; }
	
	Client::SLOT_INFO* Get_QucikItemInfo() { return m_pQuickItem; }
	Client::SLOT_INFO* Get_QucikEquipInfo() { return m_pQuickEquip; }

	class CUI_Equipment* Get_Equip() { return m_pEquipUI; }
	class CUI_Inventory* Get_Inven() { return m_pInvenUI; }
	class CUI_Status*	 Get_StatusUI() { return m_pStatusUI; }
	const _uint& Get_Souls() const { return m_iSoul; }	//최민성 가만안두겠다.

	const _bool& Get_OnRenderGetItem() const { return m_bOnRenderGetItem; }
public:
	void Add_Souls(_int _iSoul) { m_iSoul += _iSoul; }
	void Set_ViewInterface(const _bool& _bViewInterface) { m_bViewInterface = _bViewInterface; }
	void SwapWeapon();
	_bool Use_Item();

	_bool Add_Equip_Item(const eWeaponID& _eWeaponID);
	_bool Add_Used_Item(const eItemID & _eItemID);
	void On_Item(const eItemID & _eItemID);
	void OK();
private:
	virtual	HRESULT Add_Com() override;
	HRESULT Render_ItemIcon(_EFFECT& _pEffect);
	HRESULT Render_HP_Bar(_EFFECT& _pEffect);
	HRESULT Render_Stamina_Bar(_EFFECT& _pEffect);
	HRESULT Render_Soul(_EFFECT& _pEffect);
	HRESULT Render_Menu(_EFFECT& _pEffect);
	HRESULT Render_Menu_Icon(_EFFECT& _pEffect);
	HRESULT Render_Interact_Icon(_EFFECT& _pEffect);
	HRESULT Render_Get_Item(_EFFECT& _pEffect);
	HRESULT Render_New_Area(_EFFECT& _pEffect);







	void KeyInput();
	void AddItem();
	void CheckQuickItem();
	void Update_Interact(const _float & _fDeltaTime);
	void Update_GetItem(const _float & _fDeltaTime);
	void Update_Area(const _float & _fDeltaTime);
private:
	eMENU m_eMenu;
	_bool m_bViewInterface;
	_bool m_bViewMenu;
	_float m_fHPSpectrum, m_fSTSpectrum,m_fDeltaTime,m_fActTime,m_fAreaTime,m_fWaitTime;
	_uint m_iPreArea, m_iCurArea,m_iAreaCount;
	_bool m_bNewArea,m_bAreaWait;

	class CTexture*			m_pIconTexture;
	class CTexture*			m_pNumTexture;
	Client::SLOT_INFO*		m_pQuickItem;
	Client::SLOT_INFO*		m_pQuickEquip;

	INTERACT				m_eInteract;
	_uint					m_iInteract;
	_uint					m_iGetItem;
	_bool					m_bGetItem;
	_bool					m_bOnRenderGetItem;
	_float					m_fGet_ItemTime;

	class CUI_Inventory*		m_pInvenUI;
	class CUI_Equipment*		m_pEquipUI;
	class CUI_Status*			m_pStatusUI;
	_float				m_fAccTime;
public:
	static CPlayer_UI* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_uint					m_iSoul;
};

#endif // !__PLAYER_UI_H__