#include "stdafx.h"
#include "NPC_Yaak.h"

CNPC_Yaak::CNPC_Yaak(_DEVICE _pDevice, const wstring & _PrototypeName)
	: CNPCBase(_pDevice, _PrototypeName)
	, m_pYaakUI(nullptr)
{
}

CNPC_Yaak::CNPC_Yaak(const CNPC_Yaak & _rOther)
	: CNPCBase(_rOther)
	, m_pYaakUI(_rOther.m_pYaakUI)
{
}

HRESULT CNPC_Yaak::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CNPCBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	CManagement* pManagement = GET_MANAGEMENT;
	CUIBase::UI_DESC tDesc;

	tDesc.vPos = { 0.f,0.f,1.f };
	tDesc.vSize = { _float(WINCX),_float(WINCY) };
	_tchar szBuff[MAX_PATH] = L"Component_Texture_InvenBack";
	lstrcpy(tDesc.szTexName, szBuff);

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_Yaak_UI", (CGameObject**)&m_pYaakUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pYaakUI, m_iSceneIdx, L"Layer_NPCUI"), E_FAIL);
	Safe_AddRef(m_pYaakUI);


	return S_OK;
}

_uint CNPC_Yaak::Update_GameObject(const _float & _fDeltaTime)
{
	CNPCBase::Update_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CNPC_Yaak::Render_GameObject()
{
	m_pMesh->Play_Animationset(m_fDeltaTime);

	CNPCBase::Render_GameObject();

	return S_OK;
}


CNPC_Yaak * CNPC_Yaak::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CNPC_Yaak* pInstance = new CNPC_Yaak(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPC_Yaak::Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	CNPC_Yaak* pClone = new CNPC_Yaak(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CNPC_Yaak::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pYaakUI);

	CNPCBase::Free();
}