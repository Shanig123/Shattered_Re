#include "stdafx.h"
#include "NPC_Valnir.h"

CNPC_Valnir::CNPC_Valnir(_DEVICE _pDevice, const wstring & _PrototypeName)
	: CNPCBase(_pDevice, _PrototypeName)
	, m_pValnirUI(nullptr)
{
}

CNPC_Valnir::CNPC_Valnir(const CNPC_Valnir & _rOther)
	: CNPCBase(_rOther)
	, m_pValnirUI(nullptr)
{
}

HRESULT CNPC_Valnir::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
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
	_tchar szBuff[MAX_PATH] = L"Component_Texture_ValnirUI";
	lstrcpy(tDesc.szTexName, szBuff);

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_Valnir_UI",
		(CGameObject**)&m_pValnirUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pValnirUI, m_iSceneIdx,
		L"Layer_NPCUI"), E_FAIL);
	Safe_AddRef(m_pValnirUI);
	return S_OK;
}

_uint CNPC_Valnir::Update_GameObject(const _float & _fDeltaTime)
{
	CNPCBase::Update_GameObject(_fDeltaTime);
	return m_iEvent;
}

HRESULT CNPC_Valnir::Render_GameObject()
{
	m_pMesh->Play_Animationset(m_fDeltaTime);
	CNPCBase::Render_GameObject();

	return S_OK;
}


CNPC_Valnir * CNPC_Valnir::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CNPC_Valnir* pInstance = new CNPC_Valnir(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNPC_Valnir::Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	CNPC_Valnir* pClone = new CNPC_Valnir(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}
	return pClone;
}

void CNPC_Valnir::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pValnirUI);
	CNPCBase::Free();
}