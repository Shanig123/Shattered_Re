#pragma once
#ifndef __UI_Forge_H__
#define __UI_Forge_H__

#include "UIBase.h"
#include "Player_Weapon.h"
USING(Engine)

class CUI_Forge : public CUIBase
{
protected:
	explicit CUI_Forge(_DEVICE _pDevice);
	explicit CUI_Forge(const CUI_Forge& _rhs);
	virtual ~CUI_Forge() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;

public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	void Start_WeaponUpgrad();
	_bool	Get_View() 	
	{
		for (_uint i = 0; i < m_iSlotSize; i++)
		{
			Add_Damage_And_Level(i);
		}
		return m_bUIView; 
	}

private:
	HRESULT SetUp_ConstantTable_Forge(_EFFECT & _pEffect);

	HRESULT Render_Slot(_EFFECT & _pEffect);
	HRESULT Render_Material(_EFFECT & _pEffect);
	HRESULT Render_WeaponName(_EFFECT & _pEffect);
	HRESULT Render_Sign(_EFFECT & _pEffect, _float3 & vPos, _float3 & vSize, _uint iIndex);
	HRESULT Render_Counting(_EFFECT & _pEffect, _float3 & vPos, _float3 & vSize, const _float & fDist, const _uint & iCount);
	HRESULT Render_ReverseCounting(_EFFECT & _pEffect, _float3 & vPos, _float3 & vSize, const _float & fDist, const _uint & iCount);

	void KeyInput();

	_ubyte Find_Weapon(const _ubyte & _ubyItemID);

	_bool Check_ForgeAble();
	void Check_Upgrade_Recipe();
	void Add_Damage_And_Level(_uint iInven);
	void SlotMove(const _int & _iIndex);
	void Check_Damage_And_Level(_uint iInven);
	void Upgrade_Weapon();
	void ScrollingSlot();
protected:
	virtual HRESULT Add_Com() override;

public:
	static CUI_Forge* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;


private:
	Client::FORGE_SLOT_INFO		m_arrForgeSlot[3];

	_uint						m_iSlotSize;
	_uint						m_iCapacity;
	_uint						m_iArrSize;
	_uint						m_iPlySoul;

	_ubyte						m_ubyPlyItem;

	_int						m_iSelectSlotNumber;
	_int						m_iUpgradeChance;

	_bool						m_bUIView;

	class CTexture*				m_pSlotTexture;
	class CTexture*				m_pFlavorTexture;
};

#endif // !__UI_Forge_H__

