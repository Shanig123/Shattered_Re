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

public:
	void Add_Souls(_int _iSoul) { m_iSoul += _iSoul; }
	_int Get_Souls() { return m_iSoul; }
	_bool Get_ViewInterface() { return m_bViewInterface; }
	void Set_ViewInterface(const _bool& _bViewInterface) { m_bViewInterface = _bViewInterface; }
	void SwapWeapon();
	_bool Use_Item();

	_bool Add_Equip_Item(const eWeaponID& _eWeaponID);
	_bool Add_Used_Item(const eItemID & _eItemID);

private:
	virtual	HRESULT Add_Com() override;
	HRESULT Render_ItemIcon(_EFFECT& _pEffect);
	HRESULT Render_HP_Bar(_EFFECT& _pEffect);
	HRESULT Render_Stamina_Bar(_EFFECT& _pEffect);
	HRESULT Render_Soul(_EFFECT& _pEffect);

	void KeyInput();
	void AddItem();
	void CheckQuickItem();
private:
	_bool m_bViewInterface;
	class CTexture*			m_pIconTexture;
	class CTexture*			m_pHPBarTexture;
	class CTexture*			m_pStaminaBarTexture;
	class CTexture*			m_pGageTexture;
	class CTexture*			m_pNumTexture;
	Client::SLOT_INFO*		m_pQuickItem;
	Client::SLOT_INFO*		m_pQuickEquip;

	class CUI_Inventory*		m_pInvenUI;
	class CUI_Equipment*		m_pEquipUI;

public:
	static CPlayer_UI* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	_uint					m_iSoul;
};

#endif // !__PLAYER_UI_H__