#pragma once
#ifndef __UI_NPC_VALNIR_H__
#define __UI_NPC_VALNIR_H__

#include "UIBase.h"

#define VALNIR_SLOT_Y_COUNT	(5)

USING(Engine)

class CUI_NPC_Valnir : public CUIBase
{
protected:
	explicit CUI_NPC_Valnir(_DEVICE _pDevice);
	explicit CUI_NPC_Valnir(const CUI_NPC_Valnir& _rhs);
	virtual ~CUI_NPC_Valnir() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;

public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
	HRESULT Render_Soul(_EFFECT & _pEffect);
	HRESULT Render_CostSoul(_EFFECT & _pEffect);
	HRESULT SetUp_ConstantTable_Valnir(_EFFECT & _pEffect);
private:
	void Buy_Item();
	void KeyInput();
	HRESULT Render_Slot(_EFFECT& _pEffect);
	HRESULT Render_Item(_EFFECT & _pEffect);
	HRESULT Render_ItemCount(_EFFECT & _pEffect);
	HRESULT Render_FlavorText(_EFFECT & _pEffect);
	HRESULT Render_FlavorTitle(_EFFECT & _pEffect);
	HRESULT Render_Script(_EFFECT & _pEffect);
	HRESULT Render_ShopBack(_EFFECT& _pEffect);

protected:
	virtual HRESULT Add_Com() override;

public:
	static CUI_NPC_Valnir* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;


private:
	Client::SHOPSLOT_INFO	m_arrShopSlot[VALNIR_SLOT_Y_COUNT];

	_uint				m_iPlayerSoul;
	_uint				m_iUIDiffuse;
	_uint				m_iArrSize;
	_uint				m_iCapacity;
	_uint				m_iArrBegin;
	_uint				m_iDivide;
	_uint				m_iMainRenderPass;
	_int				m_iQuickItemIdx;

	_bool				m_bUIView;
	_bool				m_bShoppingMode;
	_bool				m_bPlayerInteract;

	_float				m_fSlotInterval;
	_float				m_fBackDissolveTime;
	class CTexture*			m_pSlotTexture;
	class CTexture*			m_pBackTexture;
	class CTexture*			m_pScriptTexture;
	class CTexture*			m_pFlavorTexture;
	class CTexture*			m_pItemTitleTexture;
};

#endif // !__UI_NPC_VALNIR_H__

