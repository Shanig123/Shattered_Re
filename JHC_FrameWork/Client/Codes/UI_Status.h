#pragma once
#ifndef __UI_STATUS_H__
#define __UI_STATUS_H__

#include "UIBase.h"

#define STATUS_SLOT_X_COUNT	(2)
#define STATUS_SLOT_Y_COUNT	(4)

#define STATUS_SLOT_SIZE	(STATUS_SLOT_Y_COUNT*STATUS_SLOT_Y_COUNT)


USING(Engine)

class CUI_Status : public CUIBase
{
protected:
	explicit CUI_Status(_DEVICE _pDevice);
	explicit CUI_Status(const CUI_Status& _rhs);
	virtual ~CUI_Status() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;

public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	void	Load_Ini();
	virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

	HRESULT SetUp_ConstantTable_Status(_EFFECT & _pEffect);
public:
	const _bool& Get_CheckViewStatus()const { return m_bUIView; }
	const _uint& Get_Soul() 
	{ 
		if (m_bUpgradeEnd)
		{
			m_bUpgradeEnd = false;
			return	m_iPrePlayerSoul;
		}
		else
		{
			return	m_iCurPlayerSoul;
		}
	}

public:
	void	Set_UpgradeMode() { m_bUpgrade = true; }
	void	Set_Cur_Soul(_uint iSoul) { m_iCurPlayerSoul = iSoul; }
	void	Set_Real_Status(STAT_DESC & tStat) { m_tState = tStat; }
	void	Set_ViewStatus(const _bool& _bViewStatus) { m_bUIView = _bViewStatus; }

public:
	void Start_StatusUpgrade();
private:
	void KeyInput();
	HRESULT Render_Slot(_EFFECT& _pEffect);
	HRESULT Render_OK(_EFFECT& _pEffect);
	HRESULT Render_StatusNum(_EFFECT& _pEffect);
	HRESULT Render_Level(_EFFECT& _pEffect);
	HRESULT Render_RealStatus(_EFFECT& _pEffect);
	HRESULT Render_Soul(_EFFECT& _pEffect);
	HRESULT Render_Cost(_EFFECT& _pEffect);


protected:
	virtual HRESULT Add_Com() override;

public:
	static CUI_Status* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;


private:
	//STATUI_INFO			m_tPlayerStat[4];
	_uint				m_iTotalLevel;
	_uint				m_arrPrePlayerStat[4];
	_uint				m_arrCurPlayerStat[4];
	_uint				m_iPrePlayerSoul;
	_uint				m_iCurPlayerSoul;
	_uint				m_iScriptPreCount;
	_uint				m_iScriptCurCount;
	_uint				m_iArrBegin;

	_int				m_iPreCostSoul;
	_int				m_iCurCostSoul;

	_bool				m_bUIView;
	_bool				m_bUpgrade;
	_bool				m_bUpgradeEnd;

	_float3				m_vSlotStandardPos;
	_float3				m_vSlotScale;
	_float3				m_vShopBackPos;
	_float3				m_vShopBackScale;
	_float3				m_vItemPos;
	_float3				m_vItemScale;
	_float3				m_vSoulPos;
	_float3				m_vSoulScale;
	_float3				m_vCostPos;
	_float3				m_vCostScale;
	_float3				m_vLevelPos;
	_float3				m_vLevelScale;
	_float3				m_vRealStatusPos;
	_float3				m_vRealStatusScale;
	_float3				m_vOKSlotScale;
	_float3				m_vOKSlotPos;

	_float				m_fSlotInterval;
	_float				m_arrPlayerRealStatus[4];
	
	class CTexture*			m_pSlotTexture;
};

#endif // !__UI_STATUSUPGRADE_H__

