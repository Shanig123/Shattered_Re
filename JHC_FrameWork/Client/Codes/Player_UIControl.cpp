#include "stdafx.h"
#include "Player.h"
#include "UI_Inventory.h"
#include "UI_Equipment.h"
#include "Player_UI.h"
#include "UI_Dead.h"
#include "UI_Target_Dot.h"




void CPlayer::Print_To_Debug()
{
	/*_float3 vPos;
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	Print_Debug_Float(L"", vPos.y);*/
}

HRESULT CPlayer::AddUIClass()
{
	CManagement* pManagement = GET_MANAGEMENT;

	CUIBase::UI_DESC tDesc;
	/*tDesc.vPos = { _float(0),_float(0) ,1.f };
	tDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(tDesc.szTexName, L"Component_Texture_InvenBack");*/

	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_UI_Player_Inventory", (CGameObject**)&m_pInvenUI, &tDesc),E_FAIL);
	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pInvenUI, m_iSceneIdx, L"Layer_PlayerUI"), E_FAIL);
	//Safe_AddRef(m_pInvenUI);

	//m_pInvenUI->Add_ItemCount((_uint)eItemID::Heal_Small);

	//lstrcpy(tDesc.szTexName, L"Component_Texture_EquipBack");

	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_UI_Player_Equipment", (CGameObject**)&m_pEquipUI, &tDesc), E_FAIL);
	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pEquipUI, m_iSceneIdx, L"Layer_PlayerUI"), E_FAIL);
	//Safe_AddRef(m_pEquipUI);

	//m_pEquipUI->Add_ItemCount((_uint)eWeaponID::Weapon_1);

	tDesc.vPos = { 0.f,0.f,1.f };
	tDesc.vSize = { _float(WINCX),_float(WINCY) };
	_tchar szBuff[MAX_PATH] = L"Component_Texture_Player_InterFace";
	lstrcpy(tDesc.szTexName, szBuff);


	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_Player_InterFace",
		(CGameObject**)&m_pInterfaceUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pInterfaceUI, m_iSceneIdx,
		L"Layer_PlayerUI"), E_FAIL);
	Safe_AddRef(m_pInterfaceUI);


	_tchar szBff[MAX_PATH] = L"Component_Texture_Dead";
	lstrcpy(tDesc.szTexName, szBff);

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_UI_Dead",
		(CGameObject**)&m_pDeadUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pDeadUI, m_iSceneIdx,
		L"Layer_PlayerUI"), E_FAIL);
	Safe_AddRef(m_pDeadUI);




	TRANSFORM_DESC tTransformDesc = m_tTransformDesc;
	_tchar szBfff[MAX_PATH] = L"Component_Texture_LockOn";
	lstrcpy(tTransformDesc.szFileTitle, szBfff);
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Dot",
		(_uint)ESceneType::Static,
		L"Layer_PlayerUI",
		&tTransformDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	//Safe_AddRef(m_pTargetDot);


	return S_OK;
}

void CPlayer::Player_UI_Input()
{
	auto pManagement = GET_MANAGEMENT;
	
}
//void CPlayer::OpenUI()
//{
//	
//}
//
//void CPlayer::AddItem()
//{
//	
//}