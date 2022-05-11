#pragma once
#ifndef __UI_INVENTORY_H__
#define __UI_INVENTORY_H__

#include "UIBase.h"

#define INVEN_SLOT_X_COUNT	(6)
#define INVEN_SLOT_Y_COUNT	(3)

#define INVEN_SLOT_SIZE	(INVEN_SLOT_X_COUNT*INVEN_SLOT_Y_COUNT)

USING(Engine)

class CUI_Inventory : public CUIBase
{
protected:
	explicit CUI_Inventory(_DEVICE _pDevice);
	explicit CUI_Inventory(const CUI_Inventory& _rhs);
	virtual ~CUI_Inventory();
protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	const _bool& Get_CheckViewInven()const { return m_bViewInven; }
	const _int& Get_QuickSlotIdx();
	Client::SLOT_INFO* Get_QuickItem();
	Client::SLOT_INFO* Get_ItemInfo(const _ubyte& _iIndex);

public:
	//증가에 성공하면 true를 반환하고 실패하면 false를 반환합니다.
	_bool Add_ItemCount(const _ubyte& _ubyItemID);
	//가감에 성공하면 true를 반환하고 실패하면 flase를 반환합니다.
	_bool Min_ItemCount(const _ubyte& _ubyItemID);
	_bool Min_ItemCount();
	_bool Min_QucikItemCount();
	
	_ubyte Get_ItemCount(const _ubyte& _ubyItemID);
	Client::SLOT_INFO Drop_InvenSlot(const _ubyte& _ubyItemID);

	void Set_ViewInven(const _bool& _bViewInven) { m_bViewInven = _bViewInven; }


protected:
	virtual HRESULT Add_Com() override;
private:
	//인벤토리를 탐색하여 찾으면 해당 슬롯 인덱스d의 +1을 반환합니다
	_ubyte Find_Item(const _ubyte& _ubyItemID);
	byte Find_idx(const _ubyte& _ubyItemID);
	_bool CheckItemCount(const _int& _iIndex);
	void SlotMove(const _int& _iIndex);
	HRESULT Render_Slot(_EFFECT& _pEffect);
	HRESULT Render_Slot_Info(_EFFECT& _pEffect);
	void CloseInventory();
	void KeyInput(const _float& _fDeltaTime);
	
	void RegistQuickSlot();
	void AutoQuickSlot();
private:
	Client::SLOT_INFO m_arrInventorySlot[INVEN_SLOT_SIZE];
	_uint		m_iArrSize;
	_uint		m_iCapacity;
	_uint		m_iArrBegin;
	
	_int		m_iQuickItemIdx;

	_bool		m_bViewInven;

	_float3		m_vSlotStandardPos;
	_float3		m_vSlotScale;
	_float		m_fSlotInterval;

	_uint		m_iItemInfo;
	//CVIBuffer_RectTexture* m_pVIBuffer_Slot;
	CTexture* m_pSlotTexture;
	//CTexture* m_pIconTexture;

	eItemCategory m_eQuickSlot;

public:
	static CUI_Inventory* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free()override;
};


#endif // !__UI_INVENTORY_H__
