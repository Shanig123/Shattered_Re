#pragma once
#ifndef __UI_EQUIPMENT_H__
#define __UI_EQUIPMENT_H__

#include "UIBase.h"

#define EQUIP_SLOT_X_COUNT	(6)
#define EQUIP_SLOT_Y_COUNT	(3)

#define EQUIP_SLOT_SIZE	(EQUIP_SLOT_X_COUNT*EQUIP_SLOT_Y_COUNT)

USING(Engine)
class CUI_Equipment : public CUIBase
{
protected:
	explicit CUI_Equipment(_DEVICE _pDevice);
	explicit CUI_Equipment(const CUI_Equipment& _rhs);
	virtual ~CUI_Equipment();
protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
public:
	const _bool& Get_CheckViewEquip()const { return m_bViewEquip; }
	const _int& Get_QuickSlotIdx(const _int& _QuickSlotIdx);
	Client::SLOT_INFO* Get_QuickItem(const _int& _QuickSlotIdx);
	//아이템 정보를 가져옵니다.
	Client::SLOT_INFO* Get_ItemInfo(const _ubyte& _iIndex);

public:
	//증가에 성공하면 true를 반환하고 실패하면 false를 반환합니다.
	_bool Add_ItemCount(const _ubyte& _ubyItemID);
	//가감에 성공하면 true를 반환하고 실패하면 flase를 반환합니다.
	_bool Min_ItemCount(const _ubyte& _ubyItemID);
	_bool Min_ItemCount();

	//Client::SLOT_INFO Drop_InvenSlot(const _ubyte& _ubyItemID);

	void SwapQuickSlot();
	void Set_ViewEquip(const _bool& _bViewEquip) { m_bViewEquip = _bViewEquip; }

	//테스트용
	const _uint & Get_ArrSize() { return m_iArrSize; }

protected:
	virtual HRESULT Add_Com() override;
private:
	_ubyte Find_Item(const _ubyte& _ubyItemID);
	_bool CheckItemCount(const _int& _iIdex);
	void SlotMove(const _int & _iIndex);
	HRESULT Render_Slot(_EFFECT& _pEffect);
	HRESULT Render_Slot_Info(_EFFECT& _pEffect);
	void CloseInventory();
	void KeyInput(const _float& _fDeltaTime);
	void RegistQuickSlot();
	void AutoSwapChangeQuickSlot();
private:
	Client::SLOT_INFO m_arrEquipSlot[EQUIP_SLOT_SIZE];
	_uint		m_iArrSize;
	_uint		m_iCapacity;
	_uint		m_iArrBegin;
	_int		m_arrQuickItemIdx[2];	//나중에 배열로 만들 예정

	_bool		m_bViewEquip;

	//true일 시 0번 인덱스에 퀵슬롯 등록 false일 시 1번에 등록
	_bool		m_bQuickSwap;

	_float3		m_vSlotStandardPos;
	_float3		m_vSlotScale;
	_float		m_fSlotInterval;

	_uint		m_iItemInfo;

	//CVIBuffer_RectTexture* m_pVIBuffer_Slot;
	CTexture* m_pSlotTexture;
	CTexture* m_pIconTexture;
public:
	static CUI_Equipment* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;
};


#endif // !__UI_EQUIPMENT_H__
