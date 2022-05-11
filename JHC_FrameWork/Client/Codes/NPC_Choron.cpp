#include "stdafx.h"
#include "NPC_Choron.h"

CNPC_Choron::CNPC_Choron(_DEVICE _pDevice, const wstring & _PrototypeName)
	: CNPCBase(_pDevice, _PrototypeName)
	, m_pChoronUI(nullptr)
{
}

CNPC_Choron::CNPC_Choron(const CNPC_Choron & _rOther)
	: CNPCBase(_rOther)
	, m_pChoronUI(_rOther.m_pChoronUI)
{
}

HRESULT CNPC_Choron::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
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

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_Choron_UI",
		(CGameObject**)&m_pChoronUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pChoronUI, m_iSceneIdx,
		L"Layer_NPCUI"), E_FAIL);
	Safe_AddRef(m_pChoronUI);


	return S_OK;
}

_uint CNPC_Choron::Update_GameObject(const _float & _fDeltaTime)
{
	CNPCBase::Update_GameObject(_fDeltaTime);
	return m_iEvent;
}

HRESULT CNPC_Choron::Render_GameObject()
{
	m_pMesh->Play_Animationset(m_fDeltaTime);
	CNPCBase::Render_GameObject();

	return S_OK;
}


CNPC_Choron * CNPC_Choron::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CNPC_Choron* pInstance = new CNPC_Choron(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPC_Choron::Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	CNPC_Choron* pClone = new CNPC_Choron(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CNPC_Choron::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pChoronUI);

	CNPCBase::Free();
}